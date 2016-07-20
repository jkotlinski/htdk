#include "scanner.h"

#include <cctype>
#include <cstdio>
#include <cstring>

static bool is_decimal_integer(const char* s) {
    if (!*s) {
        return false;
    }
    if (*s == '-') {
        ++s;
        if (!*s) {
            return false;
        }
    }
    while (*s) {
        if (!isdigit(*s)) {
            return false;
        }
        ++s;
    }
    return true;
}

static bool is_number(const char* s) {
    return is_decimal_integer(s);
}

static Token token(const char* s) {
    if (s[0] == ':' && s[1] == '\0') {
        return Token(TokenType::Colon);
    } else if (s[0] == ';' && s[1] == '\0') {
        return Token(TokenType::SemiColon);
    } else if (is_number(s)) {
        Token t(TokenType::Number);
        t.data = (size_t)atoi(s);
        return t;
    }
    Token t(TokenType::WordName);
    t.data = (size_t)s;
    return t;
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
