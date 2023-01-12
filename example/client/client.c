#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "dstream_packet.h"
#include "dstream_sock.h"

#include <unistd.h>

int main(void) {
    int s = dstreamConnectToServer();
    if (s == -1) {
        exit(1);
    }

    float t_s = 0.f;
    float T_s = 0.1f;
    float freq = 1.f;

    float data[100] = {0};
    for (;;) {
        for (size_t i = 0; i < 100; i++) {
            data[i] = 0.5f + 0.5f * sinf(2.f * M_PI * freq * t_s);
            t_s += T_s / 100.f;
        }

        dstream_packet_t *p =
            dstreamPacketPack(F32, "test", data, sizeof(data));

        const ssize_t ssz = dstreamSockSend(s, p, dstreamPacketGetTotalSize(p));
        assert(ssz == (ssize_t)dstreamPacketGetTotalSize(p));

        free(p);

        struct timespec ts = {
            .tv_sec = 0,
            .tv_nsec = T_s * 2000000000,
        };
        if (nanosleep(&ts, NULL) == -1) {
            perror("nanosleep");
            exit(1);
        }
    }

    close(s);

    return 0;
}
