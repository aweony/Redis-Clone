#include "test_command.h"
#include "../src/command.h"
#include "../src/store.h"
using namespace std;

// ── Parser tests ─────────────────────────────────────────────────────────────

bool test_parse_set() {
    Command cmd = CommandParser::parse("SET name Kamsy");
    ASSERT_EQ(cmd.type, CommandType::SET);
    ASSERT_EQ(cmd.args.size(), 2u);
    ASSERT_STR_EQ(cmd.args[0], "name");
    ASSERT_STR_EQ(cmd.args[1], "Kamsy");
    return true;
}

bool test_parse_get() {
    Command cmd = CommandParser::parse("GET name");
    ASSERT_EQ(cmd.type, CommandType::GET);
    ASSERT_EQ(cmd.args.size(), 1u);
    ASSERT_STR_EQ(cmd.args[0], "name");
    return true;
}

bool test_parse_del() {
    Command cmd = CommandParser::parse("DEL name");
    ASSERT_EQ(cmd.type, CommandType::DEL);
    ASSERT_EQ(cmd.args.size(), 1u);
    ASSERT_STR_EQ(cmd.args[0], "name");
    return true;
}

bool test_parse_exists() {
    Command cmd = CommandParser::parse("EXISTS name");
    ASSERT_EQ(cmd.type, CommandType::EXISTS);
    ASSERT_EQ(cmd.args.size(), 1u);
    return true;
}

bool test_parse_expire() {
    Command cmd = CommandParser::parse("EXPIRE name 60");
    ASSERT_EQ(cmd.type, CommandType::EXPIRE);
    ASSERT_EQ(cmd.args.size(), 2u);
    ASSERT_STR_EQ(cmd.args[1], "60");
    return true;
}

bool test_parse_ping() {
    Command cmd = CommandParser::parse("PING");
    ASSERT_EQ(cmd.type, CommandType::PING);
    ASSERT_TRUE(cmd.args.empty());
    return true;
}

bool test_parse_unknown() {
    Command cmd = CommandParser::parse("FLUSHALL");
    ASSERT_EQ(cmd.type, CommandType::UNKNOWN);
    return true;
}

bool test_parse_case_insensitive() {
    ASSERT_EQ(CommandParser::parse("set k v").type, CommandType::SET);
    ASSERT_EQ(CommandParser::parse("Set k v").type, CommandType::SET);
    ASSERT_EQ(CommandParser::parse("get k").type,   CommandType::GET);
    ASSERT_EQ(CommandParser::parse("ping").type,    CommandType::PING);
    return true;
}

bool test_parse_args_captured() {
    Command cmd = CommandParser::parse("SET key hello world");
    ASSERT_EQ(cmd.args.size(), 3u);
    ASSERT_STR_EQ(cmd.args[0], "key");
    ASSERT_STR_EQ(cmd.args[1], "hello");
    ASSERT_STR_EQ(cmd.args[2], "world");
    return true;
}

bool test_parse_empty_input() {
    Command cmd = CommandParser::parse("");
    ASSERT_EQ(cmd.type, CommandType::UNKNOWN);
    ASSERT_TRUE(cmd.args.empty());
    return true;
}

// ── Executor tests ────────────────────────────────────────────────────────────

bool test_execute_set() {
    Store store;
    Command cmd = CommandParser::parse("SET name Kamsy");
    ASSERT_STR_EQ(CommandExecutor::execute(cmd, store), "+OK\r\n");
    return true;
}

bool test_execute_set_missing_args() {
    Store store;
    Command cmd = CommandParser::parse("SET name");
    ASSERT_STR_EQ(CommandExecutor::execute(cmd, store), "-ERR wrong number of arguments\r\n");
    return true;
}

bool test_execute_get_existing() {
    Store store;
    CommandExecutor::execute(CommandParser::parse("SET name Kamsy"), store);
    ASSERT_STR_EQ(CommandExecutor::execute(CommandParser::parse("GET name"), store), "+Kamsy\r\n");
    return true;
}

bool test_execute_get_missing() {
    Store store;
    ASSERT_STR_EQ(CommandExecutor::execute(CommandParser::parse("GET ghost"), store), "$-1\r\n");
    return true;
}

bool test_execute_get_no_args() {
    Store store;
    ASSERT_STR_EQ(CommandExecutor::execute(CommandParser::parse("GET"), store), "-ERR wrong number of arguments\r\n");
    return true;
}

bool test_execute_del_existing() {
    Store store;
    CommandExecutor::execute(CommandParser::parse("SET name Kamsy"), store);
    ASSERT_STR_EQ(CommandExecutor::execute(CommandParser::parse("DEL name"), store), ":1\r\n");
    return true;
}

bool test_execute_del_missing() {
    Store store;
    ASSERT_STR_EQ(CommandExecutor::execute(CommandParser::parse("DEL ghost"), store), ":0\r\n");
    return true;
}

bool test_execute_del_no_args() {
    Store store;
    ASSERT_STR_EQ(CommandExecutor::execute(CommandParser::parse("DEL"), store), "-ERR wrong number of arguments\r\n");
    return true;
}

bool test_execute_exists_true() {
    Store store;
    CommandExecutor::execute(CommandParser::parse("SET name Kamsy"), store);
    ASSERT_STR_EQ(CommandExecutor::execute(CommandParser::parse("EXISTS name"), store), ":1\r\n");
    return true;
}

bool test_execute_exists_false() {
    Store store;
    ASSERT_STR_EQ(CommandExecutor::execute(CommandParser::parse("EXISTS ghost"), store), ":0\r\n");
    return true;
}

bool test_execute_expire() {
    Store store;
    CommandExecutor::execute(CommandParser::parse("SET name Kamsy"), store);
    ASSERT_STR_EQ(CommandExecutor::execute(CommandParser::parse("EXPIRE name 60"), store), ":1\r\n");
    return true;
}

bool test_execute_ping() {
    Store store;
    ASSERT_STR_EQ(CommandExecutor::execute(CommandParser::parse("PING"), store), "+PONG\r\n");
    return true;
}

bool test_execute_ping_with_args() {
    Store store;
    Command cmd;
    cmd.type = CommandType::PING;
    cmd.args.push_back("extra");
    ASSERT_STR_EQ(CommandExecutor::execute(cmd, store), "-ERR wrong number of arguments\r\n");
    return true;
}

bool test_execute_unknown() {
    Store store;
    ASSERT_STR_EQ(CommandExecutor::execute(CommandParser::parse("FLUSHALL"), store), "-ERR unknown command\r\n");
    return true;
}

// ── Runner ────────────────────────────────────────────────────────────────────

void run_command_tests() {
    cout << "\n── CommandParser ──────────────────────────\n";
    RUN_TEST(test_parse_set);
    RUN_TEST(test_parse_get);
    RUN_TEST(test_parse_del);
    RUN_TEST(test_parse_exists);
    RUN_TEST(test_parse_expire);
    RUN_TEST(test_parse_ping);
    RUN_TEST(test_parse_unknown);
    RUN_TEST(test_parse_case_insensitive);
    RUN_TEST(test_parse_args_captured);
    RUN_TEST(test_parse_empty_input);

    cout << "\n── CommandExecutor ────────────────────────\n";
    RUN_TEST(test_execute_set);
    RUN_TEST(test_execute_set_missing_args);
    RUN_TEST(test_execute_get_existing);
    RUN_TEST(test_execute_get_missing);
    RUN_TEST(test_execute_get_no_args);
    RUN_TEST(test_execute_del_existing);
    RUN_TEST(test_execute_del_missing);
    RUN_TEST(test_execute_del_no_args);
    RUN_TEST(test_execute_exists_true);
    RUN_TEST(test_execute_exists_false);
    RUN_TEST(test_execute_expire);
    RUN_TEST(test_execute_ping);
    RUN_TEST(test_execute_ping_with_args);
    RUN_TEST(test_execute_unknown);

    cout << "\n── Results ────────────────────────────────\n"
         << tests_passed << "/" << tests_run << " tests passed\n\n";
}

int main() {
    run_command_tests();
    return (tests_passed == tests_run) ? 0 : 1;
}
