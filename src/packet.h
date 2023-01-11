#ifndef PACKET_H
#define PACKET_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    U8,
    U16,
    U32,
    U64,
    I8,
    I16,
    I32,
    I64,
    F32,
    F64,
} data_type_e;

typedef struct {
    int data_type;
    size_t sz;
    uint8_t data[];
} packet_t;

size_t getDataSize(int data_type);

size_t getDataLen(int data_type, size_t sz);

void *getDataElem(void *data, int data_type, size_t sz, size_t i);

packet_t *packetPack(int data_type, const char *nm, void *data, size_t sz);

void packetUnpack(packet_t *packet, int *pdata_type, const char **pnm, void **pdata, size_t *pdata_len);

#endif //PACKET_H
