#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    float t_s = 0.f;
    float T_s = 0.001f;
    float freq = 1.f;

    // float data[100] = {0};
    __attribute__((unused)) float x = 0.f;
    for (;;) {
        x = 0.5f + 0.5f * sinf(2.f * M_PI * freq * t_s);
        t_s += T_s;

        struct timespec ts = {
            .tv_sec = 0,
            .tv_nsec = T_s / 1000000000,
        };
        if (nanosleep(&ts, NULL) == -1) {
            perror("nanosleep");
            exit(1);
        }
    }

    return 0;
}
