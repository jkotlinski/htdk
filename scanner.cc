#include "scanner.h"

#include <cstdio>
#include <cctype>

static bool is_decimal_integer(const char* s) {
    if (!*s) {
        return false;
    }
    while (*s) {
        if (!isdigit(*s)) {
            return false;
        }
        ++s;
    }
    return true;
}

static std::unique_ptr<Token> token(const char* s) {
    if (s[0] == ':' && s[1] == '\0') {
        return std::make_unique<Colon>();
    } else if (s[0] == ';' && s[1] == '\0') {
        return std::make_unique<SemiColon>();
    } else if (is_decimal_integer(s)) {
        return std::make_unique<Number>(s);
    }
    return std::make_unique<WordName>(s);
}

Tokens scan(char* s) {
    Tokens tokens;
    while (*s) {
        const char* word = s;
        while (*s && !isspace(*s)) {
            ++s;
        }
        if (!*s) {
            break;
        }
        *s = 0;

        tokens.push_back(token(word));

        ++s;
        while (*s && isspace(*s)) {
            ++s;
        }
    }
    return tokens;
}
