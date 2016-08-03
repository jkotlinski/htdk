#include "scanner.h"

#include <cassert>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <string>

static int base = 10;

static bool is_integer(const char* s, int base) {
    if (*s == '-') {
        ++s;
        if (!*s) {
            return false;
        }
    }
    while (*s) {
        int n = *s;
        n -= '0';
        if (n > 9) {
            n += '0' - 'a' + 10;
        }
        if (n < 0 || n >= base) {
            return false;
        }
        ++s;
    }
    return true;
}

// Handles #nnnn.
static bool is_decimal_integer(const char* s) {
    if (!*s || *s != '#') {
        return false;
    }
    return is_integer(s + 1, 10);
}

// Handles $nnnn.
static bool is_hexadecimal_integer(const char* s) {
    if (!*s || *s != '$') {
        return false;
    }
    return is_integer(s + 1, 10);
}

static bool is_number(const char* s) {
    return is_decimal_integer(s) || is_hexadecimal_integer(s) || is_integer(s, base);
}

static int parse_number(const char* s, int base) {
    bool negate = false;
    if (*s == '-') {
        negate = true;
        ++s;
    }
    int i = 0;
    while (*s) {
        i *= base;
        char c = *s;
        c -= '0';
        if (c > '9') {
            c += '0' - 'a' + 10;
        }
        i += c;
        ++s;
    }
    return negate ? -i : i;
}

static int parse_number(const char* s) {
    if (*s == '$') {
        ++s;
        return parse_number(s, 16);
    }
    if (*s == '#') {
        ++s;
        return parse_number(s, 10);
    }
    return parse_number(s, base);
}

static void consumeWord(const char*& s) {
    while (*s && !isspace(*s)) {
        ++s;
    }
    while (*s && isspace(*s)) {
        ++s;
    }
}

static Token token(const char*& s) {
    while (isspace(*s)) {
        ++s;
    }
    std::string wordName;
    {
        const char* p = s;
        while (*p && !isspace(*p)) {
            char c = *p;
            if (c >= 'A' && c <= 'Z') {
                c += 'a' - 'A';
            }
            wordName += c;
            ++p;
        }
    }

    if (wordName == "\\") {  // Comment.
        while (*s != '\n') {
            ++s;
        }
        ++s;
        return token(s);
    } else if (wordName == "hex") {
        consumeWord(s);
        base = 16;
        return token(s);
    } else if (wordName == ":") {
        consumeWord(s);
        return Token(Colon);
    } else if (wordName == ";") {
        consumeWord(s);
        return Token(SemiColon);
    } else if (wordName == "s\"") {
        s += strlen("s\" ");
        while (isspace(*s)) {
            ++s;
        }
        std::string stringData;
        while (!isspace(s[0]) || s[1] != '"' || !isspace(s[2])) {
            stringData.push_back(*s);
            ++s;
        }
        s += 3;
        Token t(String);
        t.stringData = (char*)malloc(stringData.size() + 1);
        strcpy(t.stringData, stringData.c_str());
        return t;
    } else if (wordName == ":code") {
        s += strlen(":code ");
        while (isspace(*s)) {
            ++s;
        }
        std::string code;
        while (memcmp(";code", s, strlen(";code"))) {
            code.push_back(*s);
            ++s;
        }
        s += strlen(";code");
        while (isspace(*s)) {
            ++s;
        }
        Token t(Code);
        t.stringData = (char*)malloc(code.size() + 1);
        strcpy(t.stringData, code.c_str());
        return t;
    } else if (wordName == "if") {
        consumeWord(s);
        return Token(If);
    } else if (wordName == "then") {
        consumeWord(s);
        return Token(Then);
    } else if (wordName == "variable") {
        consumeWord(s);
        return Token(Variable);
    } else if (wordName == "allot") {
        consumeWord(s);
        return Token(Allot);
    } else if (wordName == "cells") {
        consumeWord(s);
        return Token(Cells);
    } else if (wordName == "create") {
        consumeWord(s);
        return Token(Create);
    } else if (wordName == "else") {
        consumeWord(s);
        return Token(Else);
    } else if (wordName == "!") {
        consumeWord(s);
        return Token(Store);
    } else if (wordName == "begin") {
        consumeWord(s);
        return Token(Begin);
    } else if (wordName == "again") {
        consumeWord(s);
        return Token(Again);
    } else if (wordName == "value") {
        consumeWord(s);
        return Token(Value);
    } else if (is_number(wordName.c_str())) {
        consumeWord(s);
        Token t(Number);
        t.intData = parse_number(wordName.c_str());
        return t;
    }

    Token t(WordName);
    t.stringData = (char*)malloc(wordName.length() + 1);
    assert(t.stringData);
    strcpy(t.stringData, wordName.c_str());
    consumeWord(s);
    return t;
}

Tokens scan(const char* s) {
    assert(s);
    Tokens tokens;
    while (*s) {
        tokens.push_back(token(s));
    }
    return tokens;
}
