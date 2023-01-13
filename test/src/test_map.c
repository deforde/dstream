#include "test_map.h"

#include <unity.h>

#include "map.h"

void testMapBasic(void) {
    int store[] = { 0, 1, 2, 3 };

    map_t m = {0};
    TEST_ASSERT_EQUAL(NULL, mapGet(&m, "store0"));

    mapPut(&m, "store0", &store[0]);
    TEST_ASSERT_EQUAL(0, *(int*)mapGet(&m, "store0"));
    TEST_ASSERT_EQUAL(NULL, mapGet(&m, "store1"));

    mapPut(&m, "store1", &store[1]);
    TEST_ASSERT_EQUAL(0, *(int*)mapGet(&m, "store0"));
    TEST_ASSERT_EQUAL(1, *(int*)mapGet(&m, "store1"));
    TEST_ASSERT_EQUAL(NULL, mapGet(&m, "store2"));

    mapPut(&m, "store2", &store[2]);
    TEST_ASSERT_EQUAL(0, *(int*)mapGet(&m, "store0"));
    TEST_ASSERT_EQUAL(1, *(int*)mapGet(&m, "store1"));
    TEST_ASSERT_EQUAL(2, *(int*)mapGet(&m, "store2"));

    mapPut(&m, "store3", &store[3]);
    TEST_ASSERT_EQUAL(0, *(int*)mapGet(&m, "store0"));
    TEST_ASSERT_EQUAL(1, *(int*)mapGet(&m, "store1"));
    TEST_ASSERT_EQUAL(2, *(int*)mapGet(&m, "store2"));
    TEST_ASSERT_EQUAL(3, *(int*)mapGet(&m, "store3"));

    mapRemove(&m, "store2");
    TEST_ASSERT_EQUAL(0, *(int*)mapGet(&m, "store0"));
    TEST_ASSERT_EQUAL(1, *(int*)mapGet(&m, "store1"));
    TEST_ASSERT_EQUAL(NULL, mapGet(&m, "store2"));
    TEST_ASSERT_EQUAL(3, *(int*)mapGet(&m, "store3"));

    mapRemove(&m, "store0");
    TEST_ASSERT_EQUAL(NULL, mapGet(&m, "store0"));
    TEST_ASSERT_EQUAL(1, *(int*)mapGet(&m, "store1"));
    TEST_ASSERT_EQUAL(NULL, mapGet(&m, "store2"));
    TEST_ASSERT_EQUAL(3, *(int*)mapGet(&m, "store3"));

    mapPut(&m, "store0", &store[0]);
    TEST_ASSERT_EQUAL(0, *(int*)mapGet(&m, "store0"));
    TEST_ASSERT_EQUAL(1, *(int*)mapGet(&m, "store1"));
    TEST_ASSERT_EQUAL(NULL, mapGet(&m, "store2"));
    TEST_ASSERT_EQUAL(3, *(int*)mapGet(&m, "store3"));

    mapRemove(&m, "store3");
    TEST_ASSERT_EQUAL(0, *(int*)mapGet(&m, "store0"));
    TEST_ASSERT_EQUAL(1, *(int*)mapGet(&m, "store1"));
    TEST_ASSERT_EQUAL(NULL, mapGet(&m, "store2"));
    TEST_ASSERT_EQUAL(NULL, mapGet(&m, "store3"));

    mapRemove(&m, "store0");
    mapRemove(&m, "store1");
    TEST_ASSERT_EQUAL(NULL, mapGet(&m, "store0"));
    TEST_ASSERT_EQUAL(NULL, mapGet(&m, "store1"));
    TEST_ASSERT_EQUAL(NULL, mapGet(&m, "store2"));
    TEST_ASSERT_EQUAL(NULL, mapGet(&m, "store3"));

    mapDestroy(&m);

    mapPut(&m, "store0", &store[0]);
    mapPut(&m, "store1", &store[1]);
    mapPut(&m, "store2", &store[2]);
    mapPut(&m, "store3", &store[3]);
    TEST_ASSERT_EQUAL(0, *(int*)mapGet(&m, "store0"));
    TEST_ASSERT_EQUAL(1, *(int*)mapGet(&m, "store1"));
    TEST_ASSERT_EQUAL(2, *(int*)mapGet(&m, "store2"));
    TEST_ASSERT_EQUAL(3, *(int*)mapGet(&m, "store3"));

    mapDestroy(&m);
}
