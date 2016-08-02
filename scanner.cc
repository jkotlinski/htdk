#include "scanner.h"

#include <cassert>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <string>

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
            wordName += *p;
            ++p;
        }
    }

    if (wordName == ":") {
        consumeWord(s);
        return Token(Colon);
    } else if (wordName == ";") {
        consumeWord(s);
        return Token(SemiColon);
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

    const char* p = s;
    std::string str;
    while (!isspace(*p)) {
        str += *p;
        ++p;
    }
    Token t(WordName);
    t.stringData = (char*)malloc(str.length() + 1);
    strcpy(t.stringData, str.c_str());
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
