#include "store.h"
#include <fstream>
#include <mutex>
using namespace std;

void Store::set(const string& key, const string& value) {
    unique_lock<shared_mutex> lock(mtx);
    data[key] = value;
    expirations.erase(key);
}

bool Store::get(const string& key, string& value) {
    // Optimistic shared read for the common (non-expired) path
    {
        shared_lock<shared_mutex> lock(mtx);
        auto it = data.find(key);
        if (it == data.end()) return false;

        auto exp_it = expirations.find(key);
        if (exp_it == expirations.end() || time(nullptr) <= exp_it->second) {
            value = it->second;
            return true;
        }
    }
    // Key expired — re-acquire exclusively to delete it
    unique_lock<shared_mutex> lock(mtx);
    auto it = data.find(key);
    if (it != data.end()) {
        auto exp_it = expirations.find(key);
        if (exp_it != expirations.end() && time(nullptr) > exp_it->second) {
            data.erase(it);
            expirations.erase(exp_it);
        }
    }
    return false;
}

bool Store::del(const string& key) {
    unique_lock<shared_mutex> lock(mtx);
    expirations.erase(key);
    return data.erase(key) > 0;
}

bool Store::exists(const string& key) {
    {
        shared_lock<shared_mutex> lock(mtx);
        auto it = data.find(key);
        if (it == data.end()) return false;

        auto exp_it = expirations.find(key);
        if (exp_it == expirations.end() || time(nullptr) <= exp_it->second) return true;
    }
    // Expired — clean up under exclusive lock
    unique_lock<shared_mutex> lock(mtx);
    auto it = data.find(key);
    if (it != data.end()) {
        auto exp_it = expirations.find(key);
        if (exp_it != expirations.end() && time(nullptr) > exp_it->second) {
            data.erase(it);
            expirations.erase(exp_it);
        }
    }
    return false;
}

void Store::expire(const string& key, int seconds) {
    unique_lock<shared_mutex> lock(mtx);
    if (data.count(key)) {
        expirations[key] = time(nullptr) + seconds;
    }
}

bool Store::saveSnapshot(const string& filename) {
    shared_lock<shared_mutex> lock(mtx);
    ofstream ofs(filename);
    if (!ofs) return false;

    for (const auto& [key, value] : data) {
        ofs << key << ' ' << value << '\n';
    }
    return ofs.good();
}

bool Store::loadSnapshot(const string& filename) {
    ifstream ifs(filename);
    if (!ifs) return false;

    unique_lock<shared_mutex> lock(mtx);
    data.clear();
    expirations.clear();

    string line;
    while (getline(ifs, line)) {
        size_t sep = line.find(' ');
        if (sep == string::npos) continue;
        data[line.substr(0, sep)] = line.substr(sep + 1);
    }
    return true;
}
