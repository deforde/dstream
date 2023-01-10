#include "thread.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>

#define CHECK(ret, func_name)                                                  \
    {                                                                          \
        if (ret != 0) {                                                        \
            errno = ret;                                                       \
            perror(func_name);                                                 \
            return -1;                                                         \
        }                                                                      \
    }

static void *threadImpl(void * p) {
    ThreadFuncPtr func = (ThreadFuncPtr)((uintptr_t)p);
    func();
    pthread_exit(NULL);
}

int threadStart(Thread *th) {
    int ret;
    pthread_attr_t attr;
    pthread_t thread = {0};

    ret = pthread_attr_init(&attr);
    CHECK(ret, "pthread_attr_init");

    ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    CHECK(ret, "pthread_attr_setdetachstate");

    ret = pthread_create(&thread, &attr, threadImpl, (void*)((uintptr_t)th->func));
    CHECK(ret, "pthread_create");

    ret = pthread_attr_destroy(&attr);
    CHECK(ret, "pthread_attr_destroy");

    th->handle = thread;

    return 0;
}

int threadStop(Thread th) {
    void *status;

    int ret = pthread_join(th.handle, &status);
    CHECK(ret, "pthread_join");

    return 0;
}
