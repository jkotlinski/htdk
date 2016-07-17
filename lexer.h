#pragma once

#include <list>
#include <string>

typedef std::list<std::string> Tokens;

Tokens tokenize(const char* buffer);
