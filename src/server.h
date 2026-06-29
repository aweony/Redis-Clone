#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <mutex>
#include <atomic>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "store.h"

class Server {
public:
    Server(const std::string& address, int port);
    void start();
    void stop();

private:
    void handleClient(int client_fd);

    std::string address;
    int port;
    int server_fd = -1;
    std::atomic<bool> running{false};
    Store store;
    std::mutex store_mutex;
};

#endif
