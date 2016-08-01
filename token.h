#pragma once

#include <cstdlib>
#include <cstring>

enum TokenType {
    WordName,
    Number,
    Colon,
    SemiColon,
    Begin,
    Again,
    Value,
    Code,
    Drop
};

struct Token {
    explicit Token(TokenType type) : type(type) {}

    TokenType type;
    char* stringData = nullptr;
    int intData = 0;
};
