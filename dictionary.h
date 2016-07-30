#pragma once

#include <map>
#include <set>
#include <string>

#include "word.h"

class Dictionary {
    public:
        void markAsUsed(const char* word);
        void addWord(const char* word);

        const char* getMissingWord() const;
        void popMissingWord();

    private:
        std::set<std::string> missingWords;
        std::set<std::string> addedWords;

        std::map<std::string, Word> words;
};
