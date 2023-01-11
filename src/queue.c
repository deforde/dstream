#include "queue.h"

void queueInit(queue_t *q) {
    q->head = 0;
    q->tail = 0;
    q->len = 0;
}

size_t queueFreeSpace(queue_t *q) {
    return QUEUE_CAPACITY - q->len;
}

int queuePush(queue_t *q, void *e) {
    if (queueFreeSpace(q) == 0) {
        return -1;
    }
    q->store[q->tail] = e;
    q->tail = (q->tail + 1) % QUEUE_CAPACITY;
    q->len++;
    return 0;
}

int queuePop(queue_t *q, void **pe) {
    if (q->len == 0) {
        return -1;
    }
    *pe = q->store[q->head];
    q->head = (q->head + 1) % QUEUE_CAPACITY;
    q->len--;
    return 0;
}

void *queuePeek(queue_t *q) {
    if (q->len == 0) {
        return NULL;
    }
    return q->store[q->head];
}
