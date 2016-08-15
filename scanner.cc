#include "scanner.h"

#include <cassert>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <string>

#include "pettable.h"

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
    if (*s != '#') {
        return false;
    }
    return is_integer(s + 1, 10);
}

// Handles $nnnn.
static bool is_hexadecimal_integer(const char* s) {
    if (*s != '$') {
        return false;
    }
    return is_integer(s + 1, 16);
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
        if (c < 'a') {
            c -= '0';
        } else {
            c -= 'a';
            c += 10;
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

static std::string toLowerCase(const char* s) {
    std::string word;
    while (*s && !isspace(*s)) {
        char c = *s;
        if (c >= 'A' && c <= 'Z') {
            c += 'a' - 'A';
        }
        word += c;
        ++s;
    }
    return word;
}

static TokenType getSimpleToken(const std::string& wordName) {
    struct SimpleScannedWord {
        const char* name;
        TokenType tokenType;
    };
    static const SimpleScannedWord simpleScannedWords[] = {
        { ":", Colon },
        { ";", SemiColon },
        { "if", If },
        { "then", Then },
        { "variable", Variable },
        { "allot", Allot },
        { "cells", Cells },
        { "create", Create },
        { "else", Else },
        { "!", Store },
        { "begin", Begin },
        { "again", Again },
        { "value", Value },
        { "do", Do },
        { "drop", Drop },
        { "i", I },
        { "leave", Leave },
        { "loop", Loop },
        { "+loop", PlusLoop },
        { "repeat", Repeat },
        { "while", While },
        { "constant", Constant },
        { "here", Here },
        { ",", Comma },
        { "c,", CComma },
        { "chars", Nop },
        { "align", Nop },
        { "aligned", Nop },
        { "[']", Xt },
        { "until", Until },
        { "recurse", Recurse },
        { nullptr, None }
    };
    const SimpleScannedWord* it = simpleScannedWords;
    while (it->name) {
        if (wordName == it->name) {
            return it->tokenType;
        }
        ++it;
    }
    return None;
}

static Token token(const char*& s) {
    while (isspace(*s)) {
        ++s;
    }
    const std::string wordName = toLowerCase(s);

    {
        const TokenType tokenType = getSimpleToken(wordName);
        if (tokenType != None) {
            consumeWord(s);
            return Token(tokenType);
        }
    }

    if (wordName == "\\") {  // Comment.
        while (*s != '\n') {
            ++s;
        }
        ++s;
        return token(s);
    } else if (wordName == "(") {  // Comment.
        while (!isspace(s[0]) || s[1] != ')' || !isspace(s[2])) {
            ++s;
        }
        s += 3;
        return token(s);
    } else if (wordName == "hex") {
        consumeWord(s);
        base = 16;
        return token(s);
    } else if (wordName == "s\"") {
        s += strlen("s\" ");
        while (isspace(*s)) {
            ++s;
        }
        std::string stringData;
        while (*s != '"') {
            stringData.push_back(*s);
            ++s;
        }
        ++s;
        Token t(String);
        t.stringData = stringData;
        return t;
    } else if (wordName == ":code") {
        s += strlen(":code ");
        while (isspace(*s)) {
            ++s;
        }
        std::string code;
        while (!isspace(*s) || memcmp(";code", s + 1, strlen(";code"))) {
            code.push_back(*s);
            ++s;
        }
        s += strlen(" ;code");
        while (isspace(*s)) {
            ++s;
        }
        Token t(Code);
        t.stringData = code;
        return t;
    } else if (is_number(wordName.c_str())) {
        consumeWord(s);
        Token t(Number);
        t.intData = parse_number(wordName.c_str());
        return t;
    } else if (wordName == "char" || wordName == "[char]") {
		consumeWord(s);
		Token t(Number);
		t.intData = ascToPetTable[(unsigned char)*s];
		consumeWord(s);
		return t;
    }

    Token t(WordName);
    assert(!wordName.empty());
    t.stringData = wordName;
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
