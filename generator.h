#pragma once

#include <list>
#include <cstdio>  // FILE

#include "token.h"

typedef std::list<Token> Tokens;
class Dictionary;

void generateAsm(FILE* f, const Tokens& tokens, Dictionary* dictionary);
