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

static Token token(const char* s, const char** newS) {
    if (!strcmp(":", s)) {
        return Token(Colon);
    } else if (!strcmp(";", s)) {
        return Token(SemiColon);
    } else if (!strcmp(":code", s)) {
        s += strlen(":code ");
        while (isspace(*s)) {
            ++s;
        }
        std::string code;
        while (!isspace(*s)) {
            code.push_back(*s);
            ++s;
        }
        code += ':';
        while (memcmp(";code", s, strlen(";code"))) {
            code.push_back(*s);
            ++s;
        }
        code += "\trts\n";
        *newS = s + strlen(";code ");
        Token t(Code);
        char* p = (char*)malloc(code.size() + 1);
        strcpy(p, code.c_str());
        t.data = (size_t)p;
        return t;
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

        const char* newS = nullptr;
        tokens.push_back(token(word, &newS));

        if (newS) {
            s = (char*)newS;
        } else {
            ++s;
            while (*s && isspace(*s)) {
                ++s;
            }
        }
    }
    return tokens;
}
