#pragma once

struct Token {
public:
    virtual ~Token() {}
};

struct WordName : public Token {
    explicit WordName(std::string s) : s(s) {}

    std::string s;
};

struct Number : public Token {
    Number(const char* s) : n(atoi(s)) {}

    int n;
};

struct Colon : public Token {};
struct SemiColon : public Token {};

