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

    int32_t data[] = {0, 1};
    for(;;) {
        dstream_packet_t *p = dstreamPacketPack(I32, "test", data, sizeof(data));
        data[0]++;
        data[1]++;
        const ssize_t ssz = dstreamSockSend(s, p, dstreamPacketGetTotalSize(p));
        assert(ssz == (ssize_t)dstreamPacketGetTotalSize(p));
        free(p);
        puts("Sent test packet...");
        sleep(1);
    }

    close(s);

    return 0;
}
