#pragma once

#include <list>

#include "token.h"

typedef std::list<Token> Tokens;

Tokens scan(const char* buffer);
