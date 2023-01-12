#ifndef SOCK_H
#define SOCK_H

#include <stddef.h>
#include <stdint.h>

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

int dstreamAcceptClientConnection(void);

int dstreamConnectToServer(void);

ssize_t dstreamSockSend(int s, void *buf, size_t sz);

ssize_t dstreamSockRecv(int s, void *buf, size_t sz);

#ifdef __cplusplus
}
#endif

#endif // SOCK_H
