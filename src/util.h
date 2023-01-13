#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

void readFile(const char *filepath, char **pcontent);
void writeFile(const char *filepath, char *content, size_t sz);

#endif // UTIL_H
