#include "label.h"

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
