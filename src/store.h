#ifndef STORE_H
#define STORE_H

#include <string>
#include <unordered_map>
#include <shared_mutex>
using namespace std;

class Store{
    public:
        void set(const string& key, const string& value);
        bool get(const string& key, string& value);
        bool del(const string& key);
        bool exists(const string& key);
        void expire(const string& key, int seconds);
        bool saveSnapshot(const string& filename);
        bool loadSnapshot(const string& filename);
    private:
        unordered_map<string, string> data;
        unordered_map<string, time_t> expirations;
        shared_mutex mtx;
};

#endif