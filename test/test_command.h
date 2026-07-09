#ifndef TEST_COMMAND_H
#define TEST_COMMAND_H

#include <iostream>
#include <string>
using namespace std;

static int tests_run    = 0;
static int tests_passed = 0;

#define ASSERT_EQ(got, expected)                                              \
    do {                                                                      \
        ++tests_run;                                                          \
        if ((got) != (expected)) {                                            \
            cerr << "FAIL [" << __func__ << ":" << __LINE__ << "]\n";        \
            return false;                                                     \
        }                                                                     \
        ++tests_passed;                                                       \
    } while (0)

#define ASSERT_STR_EQ(got, expected)                                          \
    do {                                                                      \
        ++tests_run;                                                          \
        if ((got) != (expected)) {                                            \
            cerr << "FAIL [" << __func__ << ":" << __LINE__ << "]\n"         \
                 << "  expected: \"" << (expected) << "\"\n"                  \
                 << "  got:      \"" << (got)      << "\"\n";                 \
            return false;                                                     \
        }                                                                     \
        ++tests_passed;                                                       \
    } while (0)

#define ASSERT_TRUE(expr)                                                     \
    do {                                                                      \
        ++tests_run;                                                          \
        if (!(expr)) {                                                        \
            cerr << "FAIL [" << __func__ << ":" << __LINE__ << "]: "         \
                 << #expr << " is false\n";                                   \
            return false;                                                     \
        }                                                                     \
        ++tests_passed;                                                       \
    } while (0)

#define RUN_TEST(fn)                                                          \
    do {                                                                      \
        if (fn()) cout << "PASS  " #fn "\n";                                  \
        else      cout << "FAIL  " #fn "\n";                                  \
    } while (0)

// Parser tests
bool test_parse_set();
bool test_parse_get();
bool test_parse_del();
bool test_parse_exists();
bool test_parse_expire();
bool test_parse_ping();
bool test_parse_unknown();
bool test_parse_case_insensitive();
bool test_parse_args_captured();
bool test_parse_empty_input();

// Executor tests
bool test_execute_set();
bool test_execute_set_missing_args();
bool test_execute_get_existing();
bool test_execute_get_missing();
bool test_execute_get_no_args();
bool test_execute_del_existing();
bool test_execute_del_missing();
bool test_execute_del_no_args();
bool test_execute_exists_true();
bool test_execute_exists_false();
bool test_execute_expire();
bool test_execute_ping();
bool test_execute_ping_with_args();
bool test_execute_unknown();

void run_command_tests();

#endif
