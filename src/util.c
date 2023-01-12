#include <stdio.h>
#include <stdlib.h>

void readFile(const char *filepath, char **pcontent, size_t *psz) {
    FILE *f = fopen(filepath, "rb");
    if (f == NULL) {
        printf("fopen error: \"%s\"\n", filepath);
        exit(1);
    }
    fseek(f, 0, SEEK_END);
    const size_t sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *content = calloc(1, sz);
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
    *psz = sz;
}
