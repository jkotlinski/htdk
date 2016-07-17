#include "words.h"

#include <cstdio>
#include <cstdlib>

static char* readFile(const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Can't open %s", path);
        return 0;
    }
    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    rewind(f);

    char* buffer = (char*)malloc(len);
    fread(buffer, 1, len, f);
    fclose(f);
    return buffer;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        puts("Usage: htfc <infile.fs>");
        return 0;
    }

    char* buffer = readFile(argv[1]);
    if (!buffer) {
        return 1;
    }

    free(buffer);
    return 0;
}
