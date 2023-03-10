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
    int open;
    int pop_blocked;
    int push_blocked;
    pthread_mutex_t mx;
    pthread_cond_t cond;
} queue_t;

void queueInit(queue_t *q);

void queueClose(queue_t *q);

int queueIsPopBlocked(queue_t *q);

int queueIsPushBlocked(queue_t *q);

void queueDestroy(queue_t *q);

size_t queueFreeSpace(queue_t *q);

size_t queueLen(queue_t *q);

int queuePush(queue_t *q, void *e);

int queuePushBlock(queue_t *q, void *e);

int queuePop(queue_t *q, void **pe);

int queuePopBlock(queue_t *q, void **pe);

void *queuePeek(queue_t *q);

int queueIsOpen(queue_t *q);

#ifdef __cplusplus
}
#endif

#endif // QUEUE_H
