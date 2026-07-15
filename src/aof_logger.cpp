#include "aof_logger.h"
#include <chrono>
using namespace std;

AOFLogger::AOFLogger(const string& filename) : filename(filename) {}

AOFLogger::~AOFLogger() {
    close();
}

bool AOFLogger::open() {
    file.open(filename, ios::app);
    if (!file.is_open()) return false;

    running = true;
    flush_thread = thread(&AOFLogger::flushLoop, this);
    return true;
}

void AOFLogger::close() {
    if (running.exchange(false)) {
        flush_thread.join();
        lock_guard<mutex> lock(file_mutex);
        if (file.is_open()) {
            file.flush();
            file.close();
        }
    }
}

// Flushes buffered writes to disk once per second.
void AOFLogger::flushLoop() {
    while (running) {
        this_thread::sleep_for(chrono::seconds(1));
        lock_guard<mutex> lock(file_mutex);
        if (file.is_open()) file.flush();
    }
}

void AOFLogger::appendSet(const string& key, const string& value) {
    lock_guard<mutex> lock(file_mutex);
    file << "SET " << key << " " << value << "\n";
}

void AOFLogger::appendDel(const string& key) {
    lock_guard<mutex> lock(file_mutex);
    file << "DEL " << key << "\n";
}

bool AOFLogger::replay(Store& store) {
    ifstream ifs(filename);
    if (!ifs) return false;

    string line;
    while (getline(ifs, line)) {
        size_t sep = line.find(' ');
        if (sep == string::npos) continue;
        string command = line.substr(0, sep);
        if (command == "SET") {
            size_t sep2 = line.find(' ', sep + 1);
            if (sep2 == string::npos) continue;
            store.set(line.substr(sep + 1, sep2 - sep - 1), line.substr(sep2 + 1));
        } else if (command == "DEL") {
            store.del(line.substr(sep + 1));
        }
    }

    return true;
}
