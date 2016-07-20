#pragma once

#include <cstdlib>
#include <cstring>

enum TokenType {
    WordName,
    Number,
    Colon,
    SemiColon
};

struct Token {
    explicit Token(TokenType type) : type(type) {}

    TokenType type;
    size_t data = 0;  // Big enough to hold a pointer.
};
