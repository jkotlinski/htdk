#include "dictionary.h"

#include <cassert>

void Dictionary::addWord(const std::string& word) {
    if (addedWords.find(word) != addedWords.end()) {
        fprintf(stderr, "Redefining word '%s' is not allowed\n", word.c_str());
        exit(1);
    }
    addedWords.insert(word);

    auto missingIt = missingWords.find(word);
    if (missingIt != missingWords.end()) {
        missingWords.erase(missingIt);
    }
}

void Dictionary::markAsUsed(const std::string& word) {
    if (addedWords.find(word) == addedWords.end()) {
        missingWords.insert(word);
    }
}

const char* Dictionary::getMissingWord() const {
    if (missingWords.empty()) {
        return nullptr;
    }
    return missingWords.begin()->c_str();
}

void Dictionary::popMissingWord() {
    missingWords.erase(missingWords.begin());
}
