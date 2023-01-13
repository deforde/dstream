#ifndef DSTREAM_CLIENT_H
#define DSTREAM_CLIENT_H

#include <stddef.h>

#include "dstream_sock.h"

extern int dstream_socket;

void dstreamClientConnect(void);
void dstreamClientDisconnect(void);

#define dstreamClientPackAndSend(data_type, data, sz)                          \
    {                                                                          \
        if (dstream_socket != -1) {                                            \
            dstream_packet_t *p = dstreamPacketPack(data_type, #data, data, sz);    \
            const ssize_t ssz = dstreamSockSend(dstream_socket, p,             \
                                                dstreamPacketGetTotalSize(p)); \
            assert(ssz == (ssize_t)dstreamPacketGetTotalSize(p));              \
            free(p);                                                           \
        }                                                                      \
    }

#endif // DSTREAM_CLIENT_H
