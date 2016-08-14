#pragma once

#include <set>
#include <string>

class Dictionary {
    public:
        void markAsUsed(const std::string& word);
        void addWord(const std::string& word);

        const char* getMissingWord() const;
        void popMissingWord();

    private:
        std::set<std::string> missingWords;
        std::set<std::string> addedWords;
};
