#include "aof_logger.h"
#include <iostream>
using namespace std;

AOFLogger::AOFLogger(const string& filename) : filename(filename) {}

bool AOFLogger::open() {
    file.open(filename, ios::app);
    return file.is_open();
}
AOFLogger::~AOFLogger() {
    if (file.is_open()) {
        file.close();
    }
}
void AOFLogger::appendSet(const string& key, const string& value) {
    file << "SET " << key << " " << value << "\n";
    file.flush();
}

void AOFLogger::appendDel(const string& key) {
    file << "DEL " << key << "\n";
    file.flush();
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
            string key = line.substr(sep + 1, sep2 - sep - 1);
            string value = line.substr(sep2 + 1);
            store.set(key, value);
        } else if (command == "DEL") {
            string key = line.substr(sep + 1);
            store.del(key);
        }
    }

    return true;
}

void AOFLogger::close() {
    file.close();
}
