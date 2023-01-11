#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "dstream_packet.h"
#include "dstream_sock.h"

#include <unistd.h>

int main(void) {
    int s = dstreamConnectToServer();
    if (s == -1) {
        exit(1);
    }

    int32_t i = 0;
    for(;;) {
        dstream_packet_t *p = dstreamPacketPack(I32, "test", &i, sizeof(i));
        i++;
        const ssize_t ssz = dstreamSockSend(s, p, dstreamPacketGetTotalSize(p));
        assert(ssz == (ssize_t)dstreamPacketGetTotalSize(p));
        free(p);
        puts("Sent test packet...");
        sleep(1);
    }

    close(s);

    return 0;
}
