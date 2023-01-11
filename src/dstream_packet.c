#include "dstream_packet.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t dstreamPacketGetDataSize(int data_type) {
    switch(data_type) {
    case U8:
        return sizeof(uint8_t);
    case U16:
        return sizeof(uint16_t);
    case U32:
        return sizeof(uint32_t);
    case U64:
        return sizeof(uint64_t);
    case I8:
        return sizeof(int8_t);
    case I16:
        return sizeof(int16_t);
    case I32:
        return sizeof(int32_t);
    case I64:
        return sizeof(int64_t);
    case F32:
        return sizeof(float);
    case F64:
        return sizeof(double);
    }
    assert(false);
    return 0;
}

size_t dstreamPacketGetDataLen(int data_type, size_t sz) {
    assert(sz % dstreamPacketGetDataSize(data_type) == 0);
    return sz / dstreamPacketGetDataSize(data_type);
}

void *dstreamPacketGetDataElem(void *data, int data_type, __attribute__((unused)) size_t sz, size_t i) {
    assert(i < dstreamPacketGetDataLen(data_type, sz));
    switch(data_type) {
    case U8:
        return &((uint8_t*)data)[i];
    case U16:
        return &((uint16_t*)data)[i];
    case U32:
        return &((uint32_t*)data)[i];
    case U64:
        return &((uint64_t*)data)[i];
    case I8:
        return &((int8_t*)data)[i];
    case I16:
        return &((int16_t*)data)[i];
    case I32:
        return &((int32_t*)data)[i];
    case I64:
        return &((int64_t*)data)[i];
    case F32:
        return &((float*)data)[i];
    case F64:
        return &((double*)data)[i];
    }
    assert(false);
    return NULL;
}

dstream_packet_t *dstreamPacketPack(int data_type, const char *nm, void *data, size_t sz) {
    const size_t dsz = dstreamPacketGetDataSize(data_type);

    size_t nmsz = strlen(nm) + 1;
    if (nmsz % dsz != 0) {
        nmsz = (nmsz / dsz + 1) * dsz;
    }

    const size_t packet_sz = sizeof(dstream_packet_t) + nmsz + sz;
    void *buf = calloc(1, packet_sz);
    if (buf == NULL) {
        puts("memory alloc failure");
        exit(1);
    }
    dstream_packet_t *p = buf;

    p->hdr.sz = packet_sz - sizeof(p->hdr);
    p->hdr.data_type = data_type;
    memcpy(p->data, nm, strlen(nm));
    memcpy(p->data + nmsz, data, sz);

    return p;
}

void dstreamPacketUnpack(dstream_packet_t *packet, int *pdata_type, const char **pnm, void **pdata, size_t *pdata_len) {
    *pdata_type = packet->hdr.data_type;
    *pnm = (const char*)packet->data;

    const size_t dsz = dstreamPacketGetDataSize(*pdata_type);
    size_t nmsz = strlen(*pnm) + 1;
    if (nmsz % dsz != 0) {
        nmsz = (nmsz / dsz + 1) * dsz;
    }

    *pdata = &packet->data[nmsz];
    *pdata_len = packet->hdr.sz - nmsz;
}

size_t dstreamPacketGetTotalSize(dstream_packet_t *p) {
    return sizeof(p->hdr) + p->hdr.sz;
}

dstream_packet_t *dstreamPacketAlloc(dstream_packet_header_t hdr) {
    dstream_packet_t *p = calloc(1, sizeof(hdr) + hdr.sz);
    if (p) {
        p->hdr = hdr;
    }
    return p;
}
