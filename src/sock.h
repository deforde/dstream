#ifndef SOCK_H
#define SOCK_H

#include <stddef.h>
#include <stdint.h>

#include <sys/types.h>

int acceptClientConnection(void);

int connectToServer(void);

int sockSend(int s, void *buf, size_t sz);

ssize_t sockRecv(int s, void *buf, size_t sz);

#endif //SOCK_H
