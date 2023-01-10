#include "test_sock.h"

#include <stdbool.h>
#include <string.h>

#include <pthread.h>
#include <unistd.h>

#include <unity.h>

#include "sock.h"
#include "thread.h"

#define TST_MSG "hello, world"

static void serverThreadFunc(__attribute__((unused)) void *p) {
    int s = acceptClientConnection();
    TEST_ASSERT_NOT_EQUAL(-1, s);

    char buf[128] = {0};
    int rsz = sockRecv(s, buf, sizeof(buf));
    TEST_ASSERT_EQUAL(strlen(TST_MSG), rsz);
    TEST_ASSERT_EQUAL(0, memcmp(buf, TST_MSG, strlen(TST_MSG)));

    close(s);
}

static void clientThreadFunc(__attribute__((unused)) void *p) {
    int s = connectToServer();
    TEST_ASSERT_NOT_EQUAL(-1, s);

    int res = sockSend(s, TST_MSG, strlen(TST_MSG));
    TEST_ASSERT_EQUAL(0, res);

    close(s);
}

void testSockBasic(void) {
    int ret;

    thread_t server_thread;
    ret = threadStart(&server_thread, serverThreadFunc, NULL);
    TEST_ASSERT_NOT_EQUAL(-1, ret);

    thread_t client_thread;
    ret = threadStart(&client_thread, clientThreadFunc, NULL);
    TEST_ASSERT_NOT_EQUAL(-1, ret);

    ret = threadStop(client_thread);
    TEST_ASSERT_NOT_EQUAL(-1, ret);

    ret = threadStop(server_thread);
    TEST_ASSERT_NOT_EQUAL(-1, ret);
}
