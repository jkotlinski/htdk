#include "dictionary.h"

#include <cassert>
#include <cstdio>

#include "defs.h"

Dictionary::Dictionary() {
    words["1+"] = Word("oneplus", oneplus, { 0 });
    words["c@"] = Word("cfetch", cfetch, { 0 });
    words["c!"] = Word("cstore", cstore, { 0 });
    words["lit"] = Word("lit", lit, { 0 });
    words["pushya"] = Word("pushya", pushya, { 0 });
    words["rot"] = Word("rot", rot, { 0 });
}

void Dictionary::addWord(const char* word) {
    if (words.find(word) != words.end()) {
        fprintf(stderr, "Redefining word '%s' is not allowed\n", word);
        exit(1);
    }
    words[word] = Word(word, nullptr, { 0 });
    addedWords.insert(word);
}

void Dictionary::markAsUsed(const char* word) {
    usedWords.insert(word);
}

const char* Dictionary::label(const char* word) const {
    auto wordIt = words.find(word);
    if (wordIt == words.end()) {
        fprintf(stderr, "Undefined word: %s\n", word);
        exit(1);
    }
    return wordIt->second.label.c_str();
}

void Dictionary::printUsedWords(FILE* f) const {
    for (auto it = usedWords.begin(); it != usedWords.end(); ++it) {
        auto wordIt = words.find(it->c_str());
        if (wordIt == words.end()) {
            fprintf(stderr, "Undefined word: %s\n", it->c_str());
            exit(1);
        }
        wordIt->second.print(f);
    }
}
