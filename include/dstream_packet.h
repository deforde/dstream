#ifndef DSTREAM_PACKET_H
#define DSTREAM_PACKET_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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
} dstream_data_type_e;

#ifndef __cplusplus
typedef struct {
    int data_type;
    size_t sz;
} dstream_packet_header_t;

typedef struct {
    dstream_packet_header_t hdr;
    uint8_t data[];
} dstream_packet_t;
#else
typedef struct dstream_packet_header_t dstream_packet_header_t;
typedef struct dstream_packet_t dstream_packet_t;
#endif

size_t dstreamPacketGetDataSize(int data_type);

size_t dstreamPacketGetDataLen(int data_type, size_t sz);

void *dstreamPacketGetDataElem(void *data, int data_type, size_t sz, size_t i);

dstream_packet_t *dstreamPacketPack(int data_type, const char *nm, void *data, size_t sz);

void dstreamPacketUnpack(dstream_packet_t *packet, int *pdata_type, const char **pnm, void **pdata, size_t *pdata_len);

size_t dstreamPacketGetTotalSize(dstream_packet_t *p);

dstream_packet_t *dstreamPacketAlloc(dstream_packet_header_t hdr);

#ifdef __cplusplus
}
#endif

#endif //DSTREAM_PACKET_H
