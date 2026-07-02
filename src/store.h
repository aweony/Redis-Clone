#ifndef STORE_H
#define STORE_H

#include <string>
#include <unordered_map>
#include <mutex>
#include <iostream>
using namespace std;

class Store{
    public:
        void set(const string& key, const string& value);
        bool get(const string& key, string& value);
        bool del(const string& key);
        bool exists(const string& key);
        void expire(const string& key, int seconds);
    private:
        unordered_map<string, string> data;
        unordered_map<string, time_t> expirations;
        mutex mtx;
};

#endif