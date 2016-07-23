#include "scanner.h"

#include <cctype>
#include <cstdio>
#include <cstring>

static bool is_decimal_integer(const char* s) {
    if (!*s) {
        return false;
    }
    if (*s == '#') {
        ++s;
        if (!*s) {
            return false;
        }
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

static bool is_hexadecimal_integer(const char* s) {
    if (!*s) {
        return false;
    }
    if (*s != '$') {
        return false;
    }
    ++s;
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
        if (!isdigit(*s) && (*s < 'a' || *s > 'f')) {
            return false;
        }
        ++s;
    }
    return true;
}

static bool is_number(const char* s) {
    return is_decimal_integer(s) || is_hexadecimal_integer(s);
}

static int parse_hexadecimal_number(const char* s) {
    ++s;  // Skip $
    bool negate = false;
    if (*s == '-') {
        negate = true;
        ++s;
    }
    int i = 0;
    while (*s) {
        i *= 16;
        if (isdigit(*s)) {
            i += *s - '0';
        } else {
            i += *s - 'a' + 10;
        }
        ++s;
    }
    return negate ? -i : i;
}

static int parse_number(const char* s) {
    if (*s == '$') {
        return parse_hexadecimal_number(s);
    }
    return atoi(s);
}

static Token token(const char* s) {
    if (s[0] == ':' && s[1] == '\0') {
        return Token(Colon);
    } else if (s[0] == ';' && s[1] == '\0') {
        return Token(SemiColon);
    } else if (!strcmp("begin", s)) {
        return Token(Begin);
    } else if (!strcmp("again", s)) {
        return Token(Again);
    } else if (!strcmp("value", s)) {
        return Token(Value);
    } else if (is_number(s)) {
        Token t(Number);
        t.data = (size_t)parse_number(s);
        return t;
    }
    Token t(WordName);
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
