#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>

#include <pthread.h>

#define QUEUE_CAPACITY 10

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *store[QUEUE_CAPACITY];
    size_t head;
    size_t tail;
    size_t len;
    pthread_mutex_t mx;
} queue_t;

void queueInit(queue_t *q);

void queueDestroy(queue_t *q);

size_t queueFreeSpace(queue_t *q);

int queuePush(queue_t *q, void *e);

int queuePop(queue_t *q, void **pe);

void *queuePeek(queue_t *q);

#ifdef __cplusplus
}
#endif

#endif //QUEUE_H
