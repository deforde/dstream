#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    char **ids;
    char *loc;
} local_t;

typedef struct {
    char **ids;
    char *file;
} static_t;

typedef struct {
    char *app;
    char **args;
    char **globals;
    local_t **locals;
    static_t **statics;
} config_t;

config_t configParse(const char *cfg_content);
void configDestroy(config_t cfg);

#endif // CONFIG_H
