#pragma once

#include <string>
#include <fstream>
#include "store.h"
using namespace std;

class AOFLogger {
private:
    string filename;
    ofstream file;

public:
    explicit AOFLogger(const string& filename);
    ~AOFLogger();
    bool open();
    void appendSet(const string& key, const string& value);
    void appendDel(const string& key);
    bool replay(Store& store);
    void close();
};