#ifndef DSTREAM_CLIENT_H
#define DSTREAM_CLIENT_H

#include <stddef.h>

#include "dstream_packet.h"
#include "dstream_sock.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif // DSTREAM_CLIENT_H
