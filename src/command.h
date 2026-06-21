#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <vector>
using namespace std;

struct Command {
    std::string type;
    std::vector<string> args;
};

struct CommandParser {
    static Command parse(const std::string& input);
};

#endif
