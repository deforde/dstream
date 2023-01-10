#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

typedef void (*thread_func_t)(void *);

typedef struct {
    thread_func_t func;
    void *args;
} thread_args_wrapper_t;

typedef struct {
    pthread_t handle;
    thread_args_wrapper_t wrapper;
} thread_t;

int threadStart(thread_t *th, thread_func_t func, void *args);

int threadStop(thread_t th);

#endif //THREAD_H
