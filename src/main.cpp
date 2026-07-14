#include "server.h"
#include "store.h"
#include "aof_logger.h"
#include <iostream>
using namespace std;

int main() {
    AOFLogger aofLogger("data/appendonly.aof");
    Store store;
    store.loadSnapshot("data/dump.rdb");
    aofLogger.replay(store);
    aofLogger.open();
    Server server("127.0.0.1", 6379, store, aofLogger);
    server.start();
    return 0;
}
