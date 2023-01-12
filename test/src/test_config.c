#include "test_config.h"

#include <unity.h>

#include "config.h"

#define CFG_CONTENT                                                            \
    "{"                                                                        \
    "  \"app\": \"/path/to/exe\","                                             \
    "  \"args\": [ \"arg1\", \"arg2\" ],"                                      \
    "  \"globals\": [\"x\", \"s\"],"                                           \
    "  \"locals\": ["                                                          \
    "    {"                                                                    \
    "      \"ids\": [\"y\", \"p\"],"                                           \
    "      \"loc\": \"main.c:38\""                                             \
    "    },"                                                                   \
    "    {"                                                                    \
    "      \"ids\": [\"arr\"],"                                                \
    "      \"loc\": \"main.c:44\""                                             \
    "    },"                                                                   \
    "    {"                                                                    \
    "      \"ids\": [\"data2@len\"],"                                          \
    "      \"loc\": \"main.c:21\""                                             \
    "    }"                                                                    \
    "  ],"                                                                     \
    "  \"statics\": ["                                                         \
    "    {"                                                                    \
    "      \"ids\": [\"w\"],"                                                  \
    "      \"file\": \"main.c\""                                               \
    "    }"                                                                    \
    "  ]"                                                                      \
    "}"

void testConfigBasic(void) {
    config_t cfg = configParse(CFG_CONTENT);

    TEST_ASSERT_EQUAL_STRING("/path/to/exe", cfg.app);

    TEST_ASSERT_EQUAL_STRING("arg1", cfg.args[0]);
    TEST_ASSERT_EQUAL_STRING("arg2", cfg.args[1]);

    TEST_ASSERT_EQUAL_STRING("x", cfg.globals[0]);
    TEST_ASSERT_EQUAL_STRING("s", cfg.globals[1]);

    TEST_ASSERT_EQUAL_STRING("y", cfg.locals[0]->ids[0]);
    TEST_ASSERT_EQUAL_STRING("p", cfg.locals[0]->ids[1]);
    TEST_ASSERT_EQUAL_STRING("main.c:38", cfg.locals[0]->loc);

    TEST_ASSERT_EQUAL_STRING("arr", cfg.locals[1]->ids[0]);
    TEST_ASSERT_EQUAL_STRING("main.c:44", cfg.locals[1]->loc);

    TEST_ASSERT_EQUAL_STRING("data2@len", cfg.locals[2]->ids[0]);
    TEST_ASSERT_EQUAL_STRING("main.c:21", cfg.locals[2]->loc);

    TEST_ASSERT_EQUAL_STRING("w", cfg.statics[0]->ids[0]);
    TEST_ASSERT_EQUAL_STRING("main.c", cfg.statics[0]->file);

    configDestroy(&cfg);
}
