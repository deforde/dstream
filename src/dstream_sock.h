#ifndef SOCK_H
#define SOCK_H

#include <stddef.h>
#include <stdint.h>

#include <sys/types.h>

int dstreamAcceptClientConnection(void);

int dstreamConnectToServer(void);

int dstreamSockSend(int s, void *buf, size_t sz);

ssize_t dstreamSockRecv(int s, void *buf, size_t sz);

#endif //SOCK_H
