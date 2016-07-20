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
    ~Token() { free(data); }

    TokenType type;
    void* data = nullptr;
    size_t dataSize = 0;

    Token(const Token& other) {
        type = other.type;
        dataSize = other.dataSize;
        data = malloc(dataSize);
        memcpy(data, other.data, dataSize);
    }
    Token(Token& other) {
        type = other.type;
        dataSize = other.dataSize;
        data = malloc(dataSize);
        memcpy(data, other.data, dataSize);
    }
};
