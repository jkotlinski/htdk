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
        const char* label(const char* word) const;
        void addWord(const char* word);

    private:
        std::set<std::string> usedWords;
        std::set<std::string> addedWords;

        std::map<std::string, Word> words;
};
