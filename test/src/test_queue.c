#include "test_queue.h"

#include <unity.h>

#include "queue.h"

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
}
