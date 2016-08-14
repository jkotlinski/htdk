#include "label.h"

std::string label(const std::string& word_) {
    std::string s;
    const char* word = word_.c_str();
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

    const char* protected_words[] = {
        "and",
        nullptr
    };
    const char** it = protected_words;
    while (*it) {
        if (s == *it) {
            s += '_';
        }
        ++it;
    }

    return s;
}
