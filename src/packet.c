#include "packet.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t getDataSize(int data_type) {
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
}

size_t getDataLen(int data_type, size_t sz) {
    assert(sz % getDataSize(data_type) == 0);
    return sz / getDataSize(data_type);
}

void *getDataElem(void *data, int data_type, size_t sz, size_t i) {
    assert(i < getDataLen(data_type, sz));
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
}

packet_t *packetPack(int data_type, const char *nm, void *data, size_t sz) {
    const size_t packet_sz = sizeof(packet_t) + strlen(nm) + 1 + sz;
    void *buf = calloc(1, packet_sz);
    if (buf == NULL) {
        puts("memory alloc failure\n");
        exit(1);
    }
    packet_t *p = buf;

    p->sz = packet_sz - ((intptr_t)p->data - (intptr_t)p);
    p->data_type = data_type;
    memcpy(p->data, nm, strlen(nm));
    memcpy(p->data + strlen(nm) + 1, data, sz);

    return p;
}

void packetUnpack(packet_t *packet, int *pdata_type, const char **pnm, void **pdata, size_t *pdata_len) {
    *pdata_type = packet->data_type;
    *pnm = (const char*)packet->data;
    const size_t nm_len = strlen(*pnm) + 1;
    *pdata = &packet->data[nm_len];
    *pdata_len = packet->sz - nm_len;
}
