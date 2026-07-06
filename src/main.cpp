#include "server.h"
#include <iostream>
using namespace std;

int main() {
    Server server("127.0.0.1", 6379);
    server.start();
    return 0;
}
