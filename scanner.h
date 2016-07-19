#pragma once

#include <list>
#include <memory>

#include "token.h"

typedef std::list<std::unique_ptr<Token>> Tokens;

Tokens scan(char* buffer);
