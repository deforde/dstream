#include "dstream_client.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <unistd.h>

int dstream_socket = -1;

void dstreamClientConnect(void) {
    while (dstream_socket == -1) {
        dstream_socket = dstreamConnectToServer();
        struct timespec ts = {
            .tv_sec = 1,
            .tv_nsec = 0,
        };
        if (nanosleep(&ts, NULL) == -1) {
            perror("nanosleep");
            exit(1);
        }
    }
}

void dstreamClientDisconnect(void) {
    close(dstream_socket);
}
