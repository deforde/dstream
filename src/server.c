#include "server.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <unistd.h>

#include "dstream_packet.h"
#include "dstream_sock.h"
#include "queue.h"

void serverThread(void *p) {
    queue_t *q = p;

    int s = dstreamAcceptClientConnection();
    if (s == -1) {
        exit(1);
    }

    for(;;) {
        ssize_t rsz = 0;
        dstream_packet_header_t hdr = {0};

        rsz = dstreamSockRecv(s, &hdr, sizeof(hdr));
        if (rsz == 0) {
            puts("remote endpoint disconnected");
            queueClose(q);
            break;
        }
        if (rsz != sizeof(hdr)) {
            puts("failed to read packet header");
            exit(1);
        }

        dstream_packet_t *packet = dstreamPacketAlloc(hdr);
        rsz = dstreamSockRecv(s, packet->data, packet->hdr.sz);
        if (rsz != (ssize_t)packet->hdr.sz) {
            puts("failed to read packet data");
            exit(1);
        }

        if (queuePushBlock(q, packet) == -1) {
            free(packet);
            break;
        }
    }

    close(s);
}
