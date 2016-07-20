#pragma once

#include <list>
#include <memory>

#include "token.h"

typedef std::list<Token> Tokens;

Tokens scan(char* buffer);
