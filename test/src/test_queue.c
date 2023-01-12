#include "test_queue.h"

#include <assert.h>
#include <stdlib.h>

#include <unistd.h>

#include <unity.h>

#include "dstream_packet.h"
#include "queue.h"
#include "thread.h"

void testQueueBasic(void) {
    int test_data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    queue_t q;
    queueInit(&q);

    TEST_ASSERT_EQUAL(0, q.len);
    TEST_ASSERT_EQUAL(QUEUE_CAPACITY, queueFreeSpace(&q));
    TEST_ASSERT_EQUAL(NULL, queuePeek(&q));

    TEST_ASSERT_EQUAL(0, queuePush(&q, &test_data[0]));
    TEST_ASSERT_EQUAL(1, q.len);
    TEST_ASSERT_EQUAL(QUEUE_CAPACITY - 1, queueFreeSpace(&q));
    TEST_ASSERT_EQUAL(test_data[0], *(int*)queuePeek(&q));

    TEST_ASSERT_EQUAL(0, queuePush(&q, &test_data[1]));
    TEST_ASSERT_EQUAL(2, q.len);
    TEST_ASSERT_EQUAL(QUEUE_CAPACITY - 2, queueFreeSpace(&q));
    TEST_ASSERT_EQUAL(test_data[0], *(int*)queuePeek(&q));

    for (size_t i = 2; i < sizeof(test_data) / sizeof(*test_data) - 1; i++) {
        TEST_ASSERT_EQUAL(0, queuePush(&q, &test_data[i]));
        TEST_ASSERT_EQUAL(i + 1, q.len);
        TEST_ASSERT_EQUAL(QUEUE_CAPACITY - (i + 1), queueFreeSpace(&q));
        TEST_ASSERT_EQUAL(test_data[0], *(int*)queuePeek(&q));
    }

    TEST_ASSERT_EQUAL(-1, queuePush(&q, NULL));
    TEST_ASSERT_EQUAL(QUEUE_CAPACITY, q.len);
    TEST_ASSERT_EQUAL(0, queueFreeSpace(&q));
    TEST_ASSERT_EQUAL(test_data[0], *(int*)queuePeek(&q));

    int *e;
    for (size_t i = 0; i < 6; i++) {
        TEST_ASSERT_EQUAL(0, queuePop(&q, (void**)&e));
        TEST_ASSERT_EQUAL(&test_data[i], e);
        TEST_ASSERT_EQUAL(QUEUE_CAPACITY - (i + 1), q.len);
        TEST_ASSERT_EQUAL((i + 1), queueFreeSpace(&q));
    }
    for (size_t i = 0; i < 6; i++) {
        TEST_ASSERT_EQUAL(0, queuePush(&q, &test_data[i]));
        TEST_ASSERT_EQUAL(4 + i + 1, q.len);
        TEST_ASSERT_EQUAL(QUEUE_CAPACITY - (4 + i + 1), queueFreeSpace(&q));
        TEST_ASSERT_EQUAL(test_data[6], *(int*)queuePeek(&q));
    }
    TEST_ASSERT_EQUAL(-1, queuePush(&q, NULL));
    TEST_ASSERT_EQUAL(QUEUE_CAPACITY, q.len);
    TEST_ASSERT_EQUAL(0, queueFreeSpace(&q));
    TEST_ASSERT_EQUAL(test_data[6], *(int*)queuePeek(&q));

    for (size_t i = 0; i < QUEUE_CAPACITY; i++) {
        TEST_ASSERT_EQUAL(0, queuePop(&q, (void**)&e));
        TEST_ASSERT_EQUAL(&test_data[(6 + i) % QUEUE_CAPACITY], e);
        TEST_ASSERT_EQUAL(QUEUE_CAPACITY - (i + 1), q.len);
        TEST_ASSERT_EQUAL((i + 1), queueFreeSpace(&q));
    }
    TEST_ASSERT_EQUAL(-1, queuePop(&q, NULL));

    queueDestroy(&q);
}

static void threadTx(void *p) {
    queue_t *q = p;

    int32_t data[] = {0, 1};
    for (size_t j = 0; j < 20; j++) {
        dstream_packet_t *packet = dstreamPacketPack(I32, "test", data, sizeof(data));

        queuePushBlock(q, packet);

        data[0]++;
        data[1]++;
    }
}

static void threadRx(void *p) {
    queue_t *q = p;

    int32_t exp_data[] = {0, 1};
    for (size_t j = 0; j < 20; j++) {
        dstream_packet_t *packet;

        queuePopBlock(q, (void**)&packet);

        int d_ty;
        const char *nm;
        void *data;
        size_t sz;
        dstreamPacketUnpack(packet, &d_ty, &nm, &data, &sz);

        const size_t len = dstreamPacketGetDataLen(d_ty, sz);
        for (size_t i = 0; i < len; i++) {
            TEST_ASSERT_EQUAL(exp_data[i], *(int32_t*)dstreamPacketGetDataElem(data, d_ty, sz, i));
        }

        exp_data[0]++;
        exp_data[1]++;

        free(packet);
    }
}

void testQueueThreaded(void) {
    queue_t q;
    queueInit(&q);

    thread_t rx_thread;
    threadStart(&rx_thread, threadRx, &q);

    thread_t tx_thread;
    threadStart(&tx_thread, threadTx, &q);

    threadStop(tx_thread);
    threadStop(rx_thread);

    queueDestroy(&q);
}
