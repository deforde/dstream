#include "config.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"

#define GDB_EXTS_FILEPATH "gdb_extensions.py"
#define GDB_CMDS_FILEPATH "/tmp/gdb_cmds"

static void buildStringList(cJSON *obj, const char *key, char ***pout) {
    cJSON *list = cJSON_GetObjectItemCaseSensitive(obj, key);
    if (!cJSON_IsArray(list)) {
        printf("config error for key: \"%s\"\n", key);
        exit(1);
    }
    cJSON *item;
    size_t i = 0;
    cJSON_ArrayForEach(item, list) { i++; }
    *pout = calloc(1, (i + 1) * sizeof(**pout));
    i = 0;
    cJSON_ArrayForEach(item, list) {
        if (!cJSON_IsString(item) || item->valuestring == NULL) {
            printf("config error for key: \"%s\", at index: %zu\n", key, i);
            exit(1);
        }
        (*pout)[i] = strdup(item->valuestring);
        i++;
    }
}

config_t configParse(const char *cfg_content) {
    config_t cfg = {0};

    cJSON *cjson = cJSON_ParseWithLength(cfg_content, strlen(cfg_content));

    cJSON *app = cJSON_GetObjectItemCaseSensitive(cjson, "app");
    if (!cJSON_IsString(app) || app->valuestring == NULL) {
        puts("config error for key: \"app\"");
        exit(1);
    }
    cfg.app = strdup(app->valuestring);

    buildStringList(cjson, "args", &cfg.args);
    buildStringList(cjson, "globals", &cfg.globals);

    //=========================================================================

    cJSON *locals = cJSON_GetObjectItemCaseSensitive(cjson, "locals");
    if (!cJSON_IsArray(locals)) {
        puts("config error for key: \"locals\"");
        exit(1);
    }
    cJSON *local;
    size_t i = 0;
    cJSON_ArrayForEach(local, locals) { i++; }
    cfg.locals = calloc(1, (i + 1) * sizeof(*cfg.locals));
    i = 0;
    cJSON_ArrayForEach(local, locals) {
        if (!cJSON_IsObject(local)) {
            printf("config error for key: \"locals\", at index: %zu\n", i);
            exit(1);
        }

        cfg.locals[i] = calloc(1, sizeof(**cfg.locals));

        buildStringList(local, "ids", &cfg.locals[i]->ids);

        cJSON *loc = cJSON_GetObjectItemCaseSensitive(local, "loc");
        if (!cJSON_IsString(loc) || loc->valuestring == NULL) {
            puts("config error for key: \"loc\"");
            exit(1);
        }
        cfg.locals[i]->loc = strdup(loc->valuestring);

        i++;
    }

    //=========================================================================

    cJSON *statics = cJSON_GetObjectItemCaseSensitive(cjson, "statics");
    if (!cJSON_IsArray(statics)) {
        puts("config error for key: \"statics\"");
        exit(1);
    }
    cJSON *sttc;
    i = 0;
    cJSON_ArrayForEach(sttc, statics) { i++; }
    cfg.statics = calloc(1, (i + 1) * sizeof(*cfg.statics));
    i = 0;
    cJSON_ArrayForEach(sttc, statics) {
        if (!cJSON_IsObject(sttc)) {
            printf("config error for key: \"statics\", at index: %zu\n", i);
            exit(1);
        }

        cfg.statics[i] = calloc(1, sizeof(**cfg.statics));

        buildStringList(sttc, "ids", &cfg.statics[i]->ids);

        cJSON *file = cJSON_GetObjectItemCaseSensitive(sttc, "file");
        if (!cJSON_IsString(file) || file->valuestring == NULL) {
            puts("config error for key: \"file\"");
            exit(1);
        }
        cfg.statics[i]->file = strdup(file->valuestring);

        i++;
    }

    //=========================================================================

    cJSON_Delete(cjson);

    return cfg;
}

void configDestroy(config_t cfg) {
    free(cfg.app);

    for (size_t i = 0; cfg.args[i]; i++) {
        free(cfg.args[i]);
    }
    free(cfg.args);

    for (size_t i = 0; cfg.globals[i]; i++) {
        free(cfg.globals[i]);
    }
    free(cfg.globals);

    for (size_t i = 0; cfg.locals[i]; i++) {
        for (size_t j = 0; cfg.locals[i]->ids[j]; j++) {
            free(cfg.locals[i]->ids[j]);
        }
        free(cfg.locals[i]->ids);
        free(cfg.locals[i]->loc);
        free(cfg.locals[i]);
    }
    free(cfg.locals);

    for (size_t i = 0; cfg.statics[i]; i++) {
        for (size_t j = 0; cfg.statics[i]->ids[j]; j++) {
            free(cfg.statics[i]->ids[j]);
        }
        free(cfg.statics[i]->ids);
        free(cfg.statics[i]->file);
        free(cfg.statics[i]);
    }
    free(cfg.statics);
}

void configGenGDBCommands(config_t cfg) {
    FILE *f = fopen(GDB_CMDS_FILEPATH, "w");
    if (f == NULL) {
        printf("fopen error: \"%s\"\n", GDB_CMDS_FILEPATH);
        exit(1);
    }

    fprintf(f,
            "source %s\n"
            "set breakpoint pending on\n"
            "set print elements unlimited\n"
            "set print repeats unlimited\n"
            "set pagination off\n",
            GDB_EXTS_FILEPATH);

    for (size_t i = 0; cfg.globals[i]; i++) {
        fprintf(f,
                "watch %s\n"
                "commands\n"
                "silent\n"
                "trace_data {\"id\": \"%s\"}\n"
                "c\n"
                "end\n",
                cfg.globals[i], cfg.globals[i]);
    }

    for (size_t i = 0; cfg.locals[i]; i++) {
        fprintf(f,
                "b %s\n"
                "commands\n"
                "silent\n",
                cfg.locals[i]->loc);
        for (size_t j = 0; cfg.locals[i]->ids[j]; j++) {
            fprintf(f, "trace_data {\"id\": \"%s\"}\n", cfg.locals[i]->ids[j]);
        }
        fprintf(f, "c\n"
                   "end\n");
    }

    for (size_t i = 0; cfg.statics[i]; i++) {
        for (size_t j = 0; cfg.statics[i]->ids[j]; j++) {
            fprintf(f,
                    "watch %s::%s\n"
                    "commands\n"
                    "silent\n"
                    "trace_data {\"id\": \"%s\"}\n"
                    "c\n"
                    "end\n",
                    cfg.statics[i]->file, cfg.statics[i]->ids[j],
                    cfg.statics[i]->ids[j]);
        }
    }

    fprintf(f, "r\n"
               "q\n");

    fclose(f);
}
