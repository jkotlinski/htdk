#pragma once

#include <list>
#include <memory>

#include "token.h"

typedef std::list<Token> Tokens;

void generate_asm(const Tokens& tokens);
