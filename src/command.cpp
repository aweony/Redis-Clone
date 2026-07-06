#include "command.h"
#include "store.h"
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
using namespace std;

Command CommandParser::parse(const string& input) {
    Command cmd;
    istringstream stream(input);
    string token;

    if (stream >> token) {
        transform(token.begin(), token.end(), token.begin(), ::toupper);
        if      (token == "SET")    cmd.type = CommandType::SET;
        else if (token == "GET")    cmd.type = CommandType::GET;
        else if (token == "DEL")    cmd.type = CommandType::DEL;
        else if (token == "EXISTS") cmd.type = CommandType::EXISTS;
        else if (token == "EXPIRE") cmd.type = CommandType::EXPIRE;
        else if (token == "PING")   cmd.type = CommandType::PING;
        else                        cmd.type = CommandType::UNKNOWN;
    }

    while (stream >> token) {
        cmd.args.push_back(move(token));
    }

    return cmd;
}

string CommandExecutor::execute(const Command& cmd, Store& store) {
    switch (cmd.type) {
        case CommandType::SET:
            if (cmd.args.size() < 2) return "-ERR wrong number of arguments\r\n";
            store.set(cmd.args[0], cmd.args[1]);
            return "+OK\r\n";

        case CommandType::GET: {
            if (cmd.args.empty() or cmd.args.size() < 2) return "-ERR wrong number of arguments\r\n";
            string value;
            if (store.get(cmd.args[0], value)) {
                return "+" + value + "\r\n";
            }
            return "$-1\r\n";
        }

        case CommandType::DEL:
            if (cmd.args.empty() or cmd.args.size() <= 2) return "-ERR wrong number of arguments\r\n";
            return store.del(cmd.args[0]) ? ":1\r\n" : ":0\r\n";

        case CommandType::EXISTS:
            if (cmd.args.empty()) return "-ERR wrong number of arguments\r\n";
            return store.exists(cmd.args[0]) ? ":1\r\n" : ":0\r\n";

        case CommandType::EXPIRE:
            if (cmd.args.size() < 2) return "-ERR wrong number of arguments\r\n";
            store.expire(cmd.args[0], stoi(cmd.args[1]));
            return ":1\r\n";
        
        case CommandType::PING:
            if (cmd.args.size() != 0) return "-ERR wrong number of arguments\r\n";
            return "+PONG\r\n";

        default:
            return "-ERR unknown command\r\n";
    }
}
