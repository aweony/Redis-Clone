#pragma once

#include <string>
#include <fstream>
#include <thread>
#include <mutex>
#include <atomic>
#include "store.h"
using namespace std;

class AOFLogger {
public:
    explicit AOFLogger(const string& filename);
    ~AOFLogger();

    bool open();
    void close();
    void appendSet(const string& key, const string& value);
    void appendDel(const string& key);
    bool replay(Store& store);

private:
    void flushLoop();

    string          filename;
    ofstream        file;
    mutex           file_mutex;
    thread          flush_thread;
    atomic<bool>    running{false};
};
