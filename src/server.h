#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <mutex>
#include <atomic>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

class Store;

class Server {
public:
    Server(const string& address, int port);
    ~Server();
    void start();
    void stop();

private:
    void handleClient(int client_fd);

    string address;
    int port;
    int server_fd = -1;
    atomic<bool> running{false};
    mutex store_mutex;
};

#endif
