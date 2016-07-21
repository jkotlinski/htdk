#pragma once

#include <list>
#include <memory>

#include "token.h"

typedef std::list<Token> Tokens;

void generateAsm(FILE* f, const Tokens& tokens);
