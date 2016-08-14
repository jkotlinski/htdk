#pragma once

#include <string>

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
    Cells,
    Do,
    I,
    Leave,
    Loop,
    Repeat,
    While,
    Constant,
    Here,
    Comma,
    CComma,
    Nop
};

struct Token {
    explicit Token(TokenType type) : type(type) {}

    TokenType type;
    std::string stringData;
    int intData = 0;
};
