#pragma once

#include <cstdio>

struct Word {
    Word() {}
    Word(const char* label, const char* definition, const char** dependencies)
        : label(label), definition(definition), dependencies(dependencies) {}

    void print(FILE* f) const;

    const char* label = nullptr;
    const char* definition = nullptr;
    const char** dependencies = nullptr;
};
