#pragma once

#include <cstdio>
#include <string>

struct Word {
    Word() {}
    Word(const char* label, const char* definition, const char** dependencies)
        : label(label), definition(definition), dependencies(dependencies) {}

    void print(FILE* f) const;

    std::string label;
    const char* definition = nullptr;
    const char** dependencies = nullptr;
};
