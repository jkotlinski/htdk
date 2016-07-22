#include "word.h"

void Word::print(FILE* f) const {
    if (definition) {
        fprintf(f, "\n%s:%s\n", label, definition);
    }
}
