#include "command.h"
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
        cmd.type = move(token);
    }

    while (stream >> token) {
        cmd.args.push_back(move(token));
    }

    return cmd;
}
