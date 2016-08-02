#pragma once

#include <set>
#include <string>

class Dictionary {
    public:
        void markAsUsed(const char* word);
        void addWord(const char* word);

        const char* getMissingWord() const;
        void popMissingWord();

    private:
        std::set<std::string> missingWords;
        std::set<std::string> addedWords;
};
