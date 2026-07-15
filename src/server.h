#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "aof_logger.h"
using namespace std;

class Store;
class AOFLogger;

class Server {
public:
    Server(const string& address, int port, Store& store, AOFLogger& logger);
    ~Server();
    void start();
    void stop();

private:
    void handleClient(int client_fd);

    string address;
    int port;
    Store& store;
    int server_fd = -1;
    atomic<bool> running{false};
    AOFLogger& aofLogger;
    shared_mutex store_mutex;
};

#endif
