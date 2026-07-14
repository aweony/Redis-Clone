#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <vector>
using namespace std;

enum class CommandType {
    SET,
    GET,
    DEL,
    EXISTS,
    EXPIRE,
    PING,
    SAVE,
    UNKNOWN
};

struct Command {
    CommandType type = CommandType::UNKNOWN;
    std::vector<string> args;
};

struct CommandParser {
    static Command parse(const string& input);
};

class Store;
class AOFLogger;

struct CommandExecutor {
    static string execute(const Command& cmd, Store& store, AOFLogger& logger);
};

#endif
