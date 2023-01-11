#include "test_packet.h"

#include <stdlib.h>
#include <string.h>

#include <unity.h>

#include "dstream_packet.h"

#define NAME "arr"

void testPacketBasic(void) {
    int32_t arr[] = {0, 1, 2, 3, 4};

    dstream_packet_t *p = dstreamPacketPack(I32, NAME, arr, sizeof(arr));

    int d_ty;
    const char *nm;
    void *data;
    size_t sz;
    dstreamPacketUnpack(p, &d_ty, &nm, &data, &sz);

    const size_t len = dstreamPacketGetDataLen(d_ty, sz);

    TEST_ASSERT_EQUAL(I32, d_ty);
    TEST_ASSERT_EQUAL(0, memcmp(nm, NAME, strlen(NAME)));
    TEST_ASSERT_EQUAL(sizeof(arr) / sizeof(*arr), len);

    for (size_t i = 0; i < len; i++) {
        TEST_ASSERT_EQUAL(arr[i], *(int32_t*)dstreamPacketGetDataElem(data, d_ty, sz, i));
    }

    free(p);
}
