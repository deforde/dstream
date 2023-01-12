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

static void *threadImpl(void *p) {
    thread_args_wrapper_t *wrapper = (thread_args_wrapper_t *)p;
    wrapper->func(wrapper->args);
    pthread_exit(NULL);
}

int threadStart(thread_t *th, thread_func_t func, void *args) {
    int ret;
    pthread_attr_t attr;
    pthread_t thread = {0};

    ret = pthread_attr_init(&attr);
    CHECK(ret, "pthread_attr_init");

    ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    CHECK(ret, "pthread_attr_setdetachstate");

    th->wrapper.func = func;
    th->wrapper.args = args;

    ret = pthread_create(&thread, &attr, threadImpl, (void *)&th->wrapper);
    CHECK(ret, "pthread_create");

    th->handle = thread;

    ret = pthread_attr_destroy(&attr);
    CHECK(ret, "pthread_attr_destroy");

    return 0;
}

int threadStop(thread_t th) {
    void *status;

    int ret = pthread_join(th.handle, &status);
    CHECK(ret, "pthread_join");

    return 0;
}
