#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

typedef void (*ThreadFuncPtr)(void);

typedef struct {
    pthread_t handle;
    ThreadFuncPtr func;
} Thread;

int threadStart(Thread *th);

int threadStop(Thread th);

#endif //THREAD_H
