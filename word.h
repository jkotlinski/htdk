#pragma once

#include <cstdio>
#include <string>

struct Word {
    Word() {}
    Word(const char* label, const char* definition, const char** dependencies)
        : label(label), definition(definition), dependencies(dependencies) {}

    std::string label;
    const char* definition = nullptr;
    const char** dependencies = nullptr;
};
