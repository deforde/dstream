#include "queue.h"

void queueInit(queue_t *q) {
    q->head = 0;
    q->tail = 0;
    q->len = 0;
    q->open = 1;
    q->pop_blocked = 0;
    q->push_blocked = 0;
    pthread_cond_init(&q->cond, NULL);
    pthread_mutex_init(&q->mx, NULL);
}

void queueClose(queue_t *q) {
    pthread_mutex_lock(&q->mx);
    q->open = 0;
    pthread_mutex_unlock(&q->mx);
    pthread_cond_signal(&q->cond);
}

int queueIsPopBlocked(queue_t *q) {
    pthread_mutex_lock(&q->mx);
    int blocked = q->pop_blocked;
    pthread_mutex_unlock(&q->mx);
    return blocked;
}

int queueIsPushBlocked(queue_t *q) {
    pthread_mutex_lock(&q->mx);
    int blocked = q->push_blocked;
    pthread_mutex_unlock(&q->mx);
    return blocked;
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

int queuePushBlock(queue_t *q, void *e) {
    pthread_mutex_lock(&q->mx);
    while (q->len == QUEUE_CAPACITY && q->open) {
        q->push_blocked = 1;
        pthread_cond_wait(&q->cond, &q->mx);
    }
    q->push_blocked = 0;
    if (!q->open) {
        pthread_mutex_unlock(&q->mx);
        return -1;
    }
    q->store[q->tail] = e;
    q->tail = (q->tail + 1) % QUEUE_CAPACITY;
    q->len++;
    pthread_mutex_unlock(&q->mx);
    pthread_cond_signal(&q->cond);
    return 0;
}

int queuePopBlock(queue_t *q, void **pe) {
    pthread_mutex_lock(&q->mx);
    while (q->len == 0 && q->open) {
        q->pop_blocked = 1;
        pthread_cond_wait(&q->cond, &q->mx);
    }
    q->pop_blocked = 0;
    if (!q->open) {
        pthread_mutex_unlock(&q->mx);
        return -1;
    }
    *pe = q->store[q->head];
    q->head = (q->head + 1) % QUEUE_CAPACITY;
    q->len--;
    pthread_mutex_unlock(&q->mx);
    pthread_cond_signal(&q->cond);
    return 0;
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
