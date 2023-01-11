#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>

#include "gui.h"
#include "queue.h"
#include "server.h"
#include "thread.h"

int main(void) {
    int ret;

    queue_t q;
    queueInit(&q);

    thread_t server_thread;
    ret = threadStart(&server_thread, serverThread, &q);
    if (ret == -1) {
        exit(1);
    }

    thread_t gui_thread;
    ret = threadStart(&gui_thread, guiThread, &q);
    if (ret == -1) {
        exit(1);
    }

    ret = threadStop(server_thread);
    if (ret == -1) {
        exit(1);
    }

    ret = threadStop(gui_thread);
    if (ret == -1) {
        exit(1);
    }

    queueDestroy(&q);

    return 0;
}
