#include "test_sock.h"

#include <unity.h>

void setUp(void) {}

void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(testSockBasic);

    return UNITY_END();
}
