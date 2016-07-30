#include "dictionary.h"

#include <cassert>
#include <cstdio>

void Dictionary::addWord(const char* word) {
    if (words.find(word) != words.end()) {
        fprintf(stderr, "Redefining word '%s' is not allowed\n", word);
        exit(1);
    }
    words[word] = Word(word, nullptr, { 0 });
    addedWords.insert(word);

    auto missingIt = missingWords.find(word);
    if (missingIt != missingWords.end()) {
        missingWords.erase(missingIt);
    }
}

void Dictionary::markAsUsed(const char* word) {
    std::string l = label(word);
    auto addedIt = addedWords.find(l.c_str());
    if (addedIt == addedWords.end()) {
        missingWords.insert(l.c_str());
    }
}

std::string label(const char* word) {
    std::string s;
    while (*word) {
        unsigned char c = *word;
        if (isalpha(c)) {
            s += c;
        } else {
            char buf[20];
            sprintf(buf, "_%02x", c);
            s += buf;
        }
        ++word;
    }
    return s;
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
