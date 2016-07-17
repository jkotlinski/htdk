#include "lexer.h"

#include <cstdio>
#include <cctype>

Tokens tokenize(char* s) {
    Tokens tokens;
    while (*s) {
        tokens.push_back(s);
        while (*s && !isspace(*s)) {
            ++s;
        }
        if (!*s) {
            break;
        }
        *s = 0;
        ++s;
        while (*s && isspace(*s)) {
            ++s;
        }
    }
    return tokens;
}
