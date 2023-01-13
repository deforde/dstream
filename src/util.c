#include <stdio.h>
#include <stdlib.h>

void readFile(const char *filepath, char **pcontent) {
    FILE *f = fopen(filepath, "rb");
    if (f == NULL) {
        printf("fopen error: \"%s\"\n", filepath);
        exit(1);
    }
    fseek(f, 0, SEEK_END);
    const size_t sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *content = calloc(1, sz + 1);
    if (content == NULL) {
        puts("calloc failure");
        exit(1);
    }
    const size_t rsz = fread(content, 1, sz, f);
    fclose(f);
    if (rsz != sz) {
        free(content);
        puts("fread error");
        exit(1);
    }
    *pcontent = content;
}

void writeFile(const char *filepath, char *content, size_t sz) {
    FILE *f = fopen(filepath, "w");
    if (f == NULL) {
        printf("fopen error: \"%s\"\n", filepath);
        exit(1);
    }
    const size_t wsz = fwrite(content, 1, sz, f);
    fclose(f);
    if (wsz != sz) {
        puts("fwrite error");
        exit(1);
    }
}
