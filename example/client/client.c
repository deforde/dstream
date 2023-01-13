#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "dstream_client.h"

#include <unistd.h>

int main(void) {
    dstreamClientConnect();

    float t_s = 0.f;
    float T_s = 0.1f;
    float freq = 1.f;

    float sin[100] = {0};
    float cos[100] = {0};
    for (;;) {
        for (size_t i = 0; i < 100; i++) {
            sin[i] = 0.5f + 0.5f * sinf(2.f * M_PI * freq * t_s);
            cos[i] = 0.5f + 0.5f * cosf(2.f * M_PI * freq * t_s);
            t_s += T_s / 100.f;
        }

        dstreamClientPackAndSend(F32, sin, sizeof(sin));
        dstreamClientPackAndSend(F32, cos, sizeof(cos));

        struct timespec ts = {
            .tv_sec = 0,
            .tv_nsec = T_s * 2000000000,
        };
        if (nanosleep(&ts, NULL) == -1) {
            perror("nanosleep");
            exit(1);
        }
    }

    dstreamClientDisconnect();

    return 0;
}
