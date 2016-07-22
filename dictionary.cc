#include "dictionary.h"

#include <cassert>
#include <cstdio>

#include "def/rot.h"
#include "def/lit.h"

Dictionary::Dictionary() {
    words["rot"] = Word(rot, { 0 });
    words["lit"] = Word(lit, { 0 });
}

void Dictionary::markAsUsed(const char* word) {
    usedWords.insert(word);
}

void Dictionary::printUsedWords(FILE* f) const {
    for (auto it = usedWords.begin(); it != usedWords.end(); ++it) {
        const char* wordName = it->c_str();
        auto word = words.find(wordName);
        assert(word != words.end());
        fprintf(f, "\n\n%s:%s\n", wordName, word->second.definition);
    }
}
