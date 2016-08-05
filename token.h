#pragma once

enum TokenType {
    None,
    WordName,
    Number,
    Colon,
    SemiColon,
    Begin,
    Again,
    Value,
    Code,
    Drop,
    If,
    Then,
    Else,
    Variable,
    Store,
    String,
    Create,
    Allot,
    Cells
};

struct Token {
    explicit Token(TokenType type) : type(type) {}

    TokenType type;
    char* stringData = nullptr;
    int intData = 0;
};
