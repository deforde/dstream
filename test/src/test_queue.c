#include "test_queue.h"

#include <assert.h>
#include <stdlib.h>
#include <time.h>

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

typedef struct {
    queue_t *q;
    pthread_mutex_t *mx;
    pthread_cond_t *cond;
    int blocking;
} thread_blk_args_t;

static void threadBlk(void *p) {
    thread_blk_args_t *targs = (thread_blk_args_t*)p;
    queue_t *q = targs->q;
    pthread_mutex_t *mx = targs->mx;
    pthread_cond_t *cond = targs->cond;

    int32_t data[] = {0, 1};
    for (size_t j = 0; j < QUEUE_CAPACITY + 1; j++) {
        dstream_packet_t *packet = dstreamPacketPack(I32, "test", data, sizeof(data));

        if (j == QUEUE_CAPACITY) {
            pthread_mutex_lock(mx);
            targs->blocking = 1;
            pthread_mutex_unlock(mx);
            pthread_cond_signal(cond);
        }

        if (queuePushBlock(q, packet) == -1) {
            free(packet);
            break;
        }

        data[0]++;
        data[1]++;
    }
}

void testQueueBlocking(void) {
    queue_t q;
    queueInit(&q);

    pthread_mutex_t mx;
    pthread_mutex_init(&mx, NULL);

    pthread_cond_t cond;
    pthread_cond_init(&cond, NULL);

    thread_blk_args_t targs = {
        .q = &q,
        .mx = &mx,
        .cond = &cond,
        .blocking = 0,
    };

    thread_t blk_thread;
    threadStart(&blk_thread, threadBlk, &targs);

    pthread_mutex_lock(&mx);
    while (targs.blocking == 0) {
        pthread_cond_wait(&cond, &mx);
    }
    pthread_mutex_unlock(&mx);

    while(queueIsPushBlocked(&q) == 0) {
        struct timespec ts = {
            .tv_sec = 0,
            .tv_nsec = 100000000,
        };
        nanosleep(&ts, NULL);
    }

    queueClose(&q);

    threadStop(blk_thread);

    TEST_ASSERT_EQUAL(0, queueIsPushBlocked(&q));

    void *packet;
    while (queuePop(&q, &packet) != -1) {
        free(packet);
    }

    queueDestroy(&q);
}
