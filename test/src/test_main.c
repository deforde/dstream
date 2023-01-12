#include "test_packet.h"
#include "test_queue.h"
#include "test_sock.h"

#include <unity.h>

void setUp(void) {}

void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(testPacketBasic);
    RUN_TEST(testQueueBasic);
    RUN_TEST(testSockBasic);

    RUN_TEST(testQueueThreaded);
    RUN_TEST(testQueueBlocking);

    return UNITY_END();
}
