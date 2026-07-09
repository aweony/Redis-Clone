#include "server.h"
#include "command.h"
#include "store.h"
#include <iostream>
#include <thread>
#include <cstring>
#include <cerrno>
#include <sys/socket.h>
#include <netinet/tcp.h>
using namespace std;

Store store; // Global store instance

Server::Server(const string& addr, int p)
    : address(addr), port(p) {}

Server::~Server() {
    stop();
}

void Server::start() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        cerr << "socket: " << strerror(errno) << "\n";
        return;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#ifdef SO_REUSEPORT
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
#endif

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    inet_pton(AF_INET, address.c_str(), &addr.sin_addr);

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        cerr << "bind: " << strerror(errno) << "\n";
        close(server_fd);
        return;
    }

    if (listen(server_fd, SOMAXCONN) < 0) {
        cerr << "listen: " << strerror(errno) << "\n";
        close(server_fd);
        return;
    }

    running = true;
    cout << "Listening on " << address << ":" << port << "\n";

    while (running) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) {
            if (running) cerr << "accept: " << strerror(errno) << "\n";
            continue;
        }

        int flag = 1;
        setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));

        thread(&Server::handleClient, this, client_fd).detach();
    }
}

void Server::handleClient(int client_fd) {
    char buffer[4096];

    while (true) {
        ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_read <= 0) break;

        buffer[bytes_read] = '\0';
        Command cmd = CommandParser::parse(string(buffer, bytes_read));

        string response;
        {
            lock_guard<mutex> lock(store_mutex);
            response = CommandExecutor::execute(cmd, store);
        }
        cout << "Command: " << buffer << "Response: " << response;
        send(client_fd, response.c_str(), response.size(), 0);
    }

    close(client_fd);
}

void Server::stop() {
    running = false;
    if (server_fd >= 0) {
        close(server_fd);
        server_fd = -1;
    }
}