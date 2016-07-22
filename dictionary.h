#pragma once

#include <map>
#include <set>
#include <string>

#include "word.h"

class Dictionary {
    public:
        Dictionary();

        void markAsUsed(const char* word);
        void printUsedWords(FILE* f) const;

    private:
        std::set<std::string> usedWords;

        std::map<std::string, Word> words;
};
