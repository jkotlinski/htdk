#pragma once

struct Word {
    Word() {}
    Word(const char* definition, const char** dependencies)
        : definition(definition), dependencies(dependencies) {}

    const char* definition = nullptr;
    const char** dependencies = nullptr;
};
