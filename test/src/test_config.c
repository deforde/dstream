#include "test_config.h"

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
    configDestroy(&cfg);
}
