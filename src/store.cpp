#include "store.h"
#include <iostream>
#include <fstream>
using namespace std;

void Store::set(const string& key, const string& value) {
    lock_guard<mutex> lock(mtx);
    data[key] = value;
    expirations.erase(key); // Remove expiration if it exists
}

bool Store::get(const string& key, string& value) {
    lock_guard<mutex> lock(mtx);
    auto it = data.find(key);
    if (it == data.end()){
        return false;
    }
    // Check for expiration
    auto exp_it = expirations.find(key);
    if (exp_it != expirations.end() && time(nullptr) > exp_it->second) {
        data.erase(it);
        expirations.erase(exp_it);
        return false;
    }
    value = it->second;
    return true;
}

bool Store::del(const string& key) {
    lock_guard<mutex> lock(mtx);
    bool existed = data.erase(key) > 0;
    expirations.erase(key);
    return existed;
}

bool Store::exists(const string& key) {
    lock_guard<mutex> lock(mtx);
    auto it = data.find(key);
    if (it == data.end()) return false;
    // Check for expiration
    auto exp_it = expirations.find(key);
    if (exp_it != expirations.end() && time(nullptr) > exp_it->second) {
        data.erase(it);
        expirations.erase(exp_it);
        return false;
    }
    return true;
}

void Store::expire(const string& key, int seconds) {
    lock_guard<mutex> lock(mtx);
    if (data.find(key) != data.end()) {
        expirations[key] = time(nullptr) + seconds;
    }
}

bool Store::saveSnapshot(const string& filename) {
    lock_guard<mutex> lock(mtx);
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

    lock_guard<mutex> lock(mtx);
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