#include "store.h"
#include <iostream>
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