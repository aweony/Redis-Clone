#include <algorithm>
#include <arpa/inet.h>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <numeric>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>
using namespace std;

constexpr const char* HOST               = "127.0.0.1";
constexpr int         PORT               = 6379;
constexpr int         CLIENT_COUNT       = 50;
constexpr int         REQUESTS_PER_CLIENT = 10'000;
constexpr int         TOTAL_REQUESTS     = CLIENT_COUNT * REQUESTS_PER_CLIENT;
constexpr int         KEYS_PER_CLIENT    = 100;

atomic<int>       ready_count{0};
atomic<bool>      start_flag{false};
atomic<long long> completed{0};
atomic<long long> failed{0};

int connect_to_server() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return -1;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(PORT);
    inet_pton(AF_INET, HOST, &addr.sin_addr);

    if (connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        close(sock);
        return -1;
    }
    return sock;
}

bool send_command(int sock, const string& cmd, double& latency_ms) {
    auto t0 = chrono::high_resolution_clock::now();

    if (send(sock, cmd.c_str(), cmd.size(), 0) < 0) return false;

    char buf[256];
    ssize_t n = recv(sock, buf, sizeof(buf) - 1, 0);
    if (n <= 0) return false;
    buf[n] = '\0';

    auto t1 = chrono::high_resolution_clock::now();
    latency_ms = chrono::duration<double, milli>(t1 - t0).count();
    return true;
}

void client_thread(int id, vector<double>& latencies) {
    int sock = connect_to_server();
    if (sock < 0) {
        failed.fetch_add(REQUESTS_PER_CLIENT, memory_order_relaxed);
        return;
    }

    for (int i = 0; i < KEYS_PER_CLIENT; i++) {
        string req = "SET k" + to_string(id) + "_" + to_string(i) + " v" + to_string(i) + "\n";
        double lat;
        send_command(sock, req, lat);
    }

    ready_count.fetch_add(1, memory_order_release);
    while (!start_flag.load(memory_order_acquire)) {}

    latencies.reserve(REQUESTS_PER_CLIENT);

    for (int i = 0; i < REQUESTS_PER_CLIENT; i++) {
        string key = "k" + to_string(id) + "_" + to_string(i % KEYS_PER_CLIENT);
        string req = ((i % 5) == 0)
            ? "SET " + key + " val" + to_string(i) + "\n"
            : "GET " + key + "\n";

        double lat;
        if (send_command(sock, req, lat)) {
            latencies.push_back(lat);
            completed.fetch_add(1, memory_order_relaxed);
        } else {
            failed.fetch_add(1, memory_order_relaxed);
        }
    }

    close(sock);
}

double percentile(const vector<double>& sorted, double p) {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(ceil(p / 100.0 * (double)sorted.size())) - 1;
    return sorted[min(idx, sorted.size() - 1)];
}

int main() {
    cout << "Benchmark: " << CLIENT_COUNT << " clients, "
         << REQUESTS_PER_CLIENT << " req each, "
         << TOTAL_REQUESTS << " total, 80% GET / 20% SET\n"
         << "Connecting to " << HOST << ":" << PORT << "...\n";

    vector<thread>         threads(CLIENT_COUNT);
    vector<vector<double>> per_thread(CLIENT_COUNT);

    for (int i = 0; i < CLIENT_COUNT; i++)
        threads[i] = thread(client_thread, i, ref(per_thread[i]));

    while (ready_count.load(memory_order_acquire) < CLIENT_COUNT) {}
    cout << "All " << CLIENT_COUNT << " clients ready, firing...\n";

    auto wall_start = chrono::high_resolution_clock::now();
    start_flag.store(true, memory_order_release);

    for (auto& t : threads) t.join();
    auto wall_end = chrono::high_resolution_clock::now();

    vector<double> all;
    all.reserve(TOTAL_REQUESTS);
    for (auto& v : per_thread)
        all.insert(all.end(), v.begin(), v.end());
    sort(all.begin(), all.end());

    double total_sec = chrono::duration<double>(wall_end - wall_start).count();
    long long done   = completed.load();
    long long errors = failed.load();
    double rps       = (double)done / total_sec;
    double avg       = all.empty() ? 0.0
                     : accumulate(all.begin(), all.end(), 0.0) / (double)all.size();

    cout << fixed << setprecision(3)
         << "\nResults\n"
         << "  time       " << total_sec << " s\n"
         << "  completed  " << done << "\n"
         << "  failed     " << errors << "\n"
         << setprecision(0)
         << "  throughput " << rps << " req/s\n"
         << setprecision(3)
         << "  min        " << (all.empty() ? 0.0 : all.front()) << " ms\n"
         << "  avg        " << avg << " ms\n"
         << "  p50        " << percentile(all, 50) << " ms\n"
         << "  p95        " << percentile(all, 95) << " ms\n"
         << "  p99        " << percentile(all, 99) << " ms\n"
         << "  max        " << (all.empty() ? 0.0 : all.back()) << " ms\n";

    return (errors == 0) ? 0 : 1;
}
