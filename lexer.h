#pragma once

#include <list>

typedef std::list<const char*> Tokens;

Tokens tokenize(char* buffer);
