#include <cassert>
#include <cstdio>
#include <cstdlib>

#include "dictionary.h"
#include "generator.h"
#include "runtime.h"
#include "scanner.h"

static char* readFile(const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Can't open %s", path);
        return 0;
    }
    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    rewind(f);

    char* buffer = (char*)malloc(len + 1);
    fread(buffer, 1, len, f);
    buffer[len] = '\0';
    fclose(f);
    return buffer;
}

static std::string asmPath(const char* fsPath) {
    std::string asmPath(fsPath);
    size_t it = asmPath.rfind(".fs");
    if (it == std::string::npos) {
        fprintf(stderr, "Input file name must end with .fs");
        exit(1);
    }
    asmPath.replace(it, asmPath.back(), ".asm");
    return asmPath;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        puts("Usage: htfc <infile.fs>");
        return 0;
    }

    const char* fsPath = argv[1];
    char* buffer = readFile(fsPath);
    if (!buffer) {
        return 1;
    }

    Tokens tokens = scan(buffer);

    FILE* f = fopen(asmPath(fsPath).c_str(), "w");
    printHeader(f);
    Dictionary dictionary;
    generateAsm(f, tokens, &dictionary);
    fputs("\n\n; --- inbuilt words\n", f);
    dictionary.printUsedWords(f);
    fclose(f);

    free(buffer);

    return 0;
}
