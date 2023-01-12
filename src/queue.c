#include "queue.h"

void queueInit(queue_t *q) {
    q->head = 0;
    q->tail = 0;
    q->len = 0;
    pthread_cond_init(&q->cond, NULL);
    pthread_mutex_init(&q->mx, NULL);
}

void queueDestroy(queue_t *q) {
    pthread_cond_destroy(&q->cond);
    pthread_mutex_destroy(&q->mx);
}

size_t queueFreeSpace(queue_t *q) {
    pthread_mutex_lock(&q->mx);
    const size_t fs = QUEUE_CAPACITY - q->len;
    pthread_mutex_unlock(&q->mx);
    return fs;
}

int queuePush(queue_t *q, void *e) {
    int ret = -1;
    pthread_mutex_lock(&q->mx);
    if (q->len != QUEUE_CAPACITY) {
        q->store[q->tail] = e;
        q->tail = (q->tail + 1) % QUEUE_CAPACITY;
        q->len++;
        ret = 0;
    }
    pthread_mutex_unlock(&q->mx);
    return ret;
}

int queuePop(queue_t *q, void **pe) {
    int ret = -1;
    pthread_mutex_lock(&q->mx);
    if (q->len != 0) {
        *pe = q->store[q->head];
        q->head = (q->head + 1) % QUEUE_CAPACITY;
        q->len--;
        ret = 0;
    }
    pthread_mutex_unlock(&q->mx);
    return ret;
}

void queuePushBlock(queue_t *q, void *e) {
    pthread_mutex_lock(&q->mx);
    while (q->len == QUEUE_CAPACITY) {
        pthread_cond_wait(&q->cond, &q->mx);
    }
    q->store[q->tail] = e;
    q->tail = (q->tail + 1) % QUEUE_CAPACITY;
    q->len++;
    pthread_mutex_unlock(&q->mx);
    pthread_cond_signal(&q->cond);
}

void queuePopBlock(queue_t *q, void **pe) {
    pthread_mutex_lock(&q->mx);
    while (q->len == 0) {
        pthread_cond_wait(&q->cond, &q->mx);
    }
    *pe = q->store[q->head];
    q->head = (q->head + 1) % QUEUE_CAPACITY;
    q->len--;
    pthread_mutex_unlock(&q->mx);
    pthread_cond_signal(&q->cond);
}

void *queuePeek(queue_t *q) {
    void *ret = NULL;
    pthread_mutex_lock(&q->mx);
    if (q->len != 0) {
        ret = q->store[q->head];
    }
    pthread_mutex_unlock(&q->mx);
    return ret;
}
