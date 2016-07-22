#include "dictionary.h"

#include <cassert>
#include <cstdio>

#include "defs.h"

Dictionary::Dictionary() {
    words["1+"] = Word("oneplus", oneplus, { 0 });
    words["c@"] = Word("cfetch", cfetch, { 0 });
    words["c!"] = Word("cstore", cstore, { 0 });
    words["rot"] = Word("rot", rot, { 0 });
    words["lit"] = Word("lit", lit, { 0 });
}

void Dictionary::markAsUsed(const char* word) {
    usedWords.insert(word);
}

const char* Dictionary::label(const char* word) const {
    auto wordIt = words.find(word);
    if (wordIt == words.end()) {
        fprintf(stderr, "Word '%s' not found!\n", word);
        exit(1);
    }
    return wordIt->second.label;
}

void Dictionary::printUsedWords(FILE* f) const {
    for (auto it = usedWords.begin(); it != usedWords.end(); ++it) {
        auto wordIt = words.find(it->c_str());
        assert(wordIt != words.end());
        wordIt->second.print(f);
    }
}
