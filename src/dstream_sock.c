#include "dstream_sock.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCK_PATH "dstream_socket"

int dstreamAcceptClientConnection(void) {
    const int s = socket(AF_UNIX, SOCK_STREAM, 0);
    if (s == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_un local = {
        .sun_family = AF_UNIX,
    };
    strcpy(local.sun_path, SOCK_PATH);
    unlink(local.sun_path);

    const socklen_t len = strlen(local.sun_path) + sizeof(local.sun_family);
    if (bind(s, (struct sockaddr *)&local, len) == -1) {
        perror("bind");
        return -1;
    }

    if (listen(s, 5) == -1) {
        perror("listen");
        return -1;
    }

    struct sockaddr_un remote;
    socklen_t sz = sizeof(remote);
    const int s2 = accept(s, (struct sockaddr *)&remote, &sz);
    if (s2 == -1) {
        perror("accept");
        return -1;
    }

    return s2;
}

int dstreamConnectToServer(void) {
    const int s = socket(AF_UNIX, SOCK_STREAM, 0);
    if (s == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_un remote = {
        .sun_family = AF_UNIX,
    };
    strcpy(remote.sun_path, SOCK_PATH);

    const socklen_t len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(s, (struct sockaddr *)&remote, len) == -1) {
        perror("connect");
        return -1;
    }

    return s;
}

int dstreamSockSend(int s, void *buf, size_t sz) {
    if(send(s, buf, sz, 0) == -1) {
        perror("send");
        return -1;
    }
    return 0;
}

ssize_t dstreamSockRecv(int s, void *buf, size_t sz) {
    ssize_t rsz = recv(s, buf, sz, 0);
    if (rsz == -1) {
        perror("recv");
    }
    return rsz;
}
