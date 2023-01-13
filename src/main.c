#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>

#include "config.h"
#include "gui.h"
#include "queue.h"
#include "server.h"
#include "thread.h"
#include "util.h"

int main(void) {
    int ret;

    // char *cfg_content;
    // readFile("example/gdb/config.json", &cfg_content);
    // config_t cfg = configParse(cfg_content);
    // configGenGDBCommands(cfg);
    // configGenGDBExtensions();

    queue_t q;
    queueInit(&q);

    thread_t server_thread;
    ret = threadStart(&server_thread, serverThread, &q);
    if (ret == -1) {
        exit(1);
    }

    // const int chld = fork();
    // if (chld == -1) {
    //     perror("fork");
    //     exit(1);
    // } else if (!chld) {
    //     execvp(cfg.app, cfg.args);
    //     puts("execvp error");
    //     exit(1);
    // }

    thread_t gui_thread;
    ret = threadStart(&gui_thread, guiThread, &q);
    if (ret == -1) {
        exit(1);
    }

    ret = threadStop(gui_thread);
    if (ret == -1) {
        exit(1);
    }

    serverTerminate();
    ret = threadStop(server_thread);
    if (ret == -1) {
        exit(1);
    }

    void *packet;
    while (queuePop(&q, &packet) != -1) {
        free(packet);
    }
    queueDestroy(&q);

    // configDestroy(cfg);
    // free(cfg_content);

    return 0;
}
