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
    PlusLoop,
    Repeat,
    While,
    Constant,
    Here,
    Comma,
    CComma,
    Xt,
    Until,
    Recurse,
    Exit,
    Nop
};

struct Token {
    explicit Token(TokenType type) : type(type) {}

    TokenType type;
    std::string stringData;
    int intData = 0;
};
