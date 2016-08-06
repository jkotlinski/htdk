#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "dictionary.h"
#include "generator.h"
#include "inbuilt.h"
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

static size_t fsExtensionIt(const std::string& s) {
    size_t it = s.rfind(".fs");
    if (it == std::string::npos) {
        fprintf(stderr, "Input file name must end with .fs");
        exit(1);
    }
    return it;
}

static std::string asmPath(const char* fsPath) {
    std::string asmPath(fsPath);
    asmPath.replace(fsExtensionIt(asmPath), asmPath.back(), ".asm");
    return asmPath;
}

static std::string prgPath(const char* fsPath) {
    std::string prgPath(fsPath);
    prgPath.replace(fsExtensionIt(prgPath), prgPath.back(), ".prg");
    return prgPath;
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

    FILE* f = fopen(asmPath(fsPath).c_str(), "w");
    printHeader(f, prgPath(fsPath).c_str());
    Dictionary dictionary;
    generateAsm(f, scan(buffer), &dictionary);
    fputs("\n; --- inbuilt words\n", f);

    while (dictionary.getMissingWord()) {
        // FIXME add a tidier way to declare dependencies between code words
        if (!strcmp(dictionary.getMissingWord(), "um/mod")) {
            dictionary.markAsUsed("swap");
        }
        generateAsm(f, scan(getDefinition(dictionary.getMissingWord())), &dictionary);
    }
    fclose(f);

    free(buffer);

    return 0;
}
