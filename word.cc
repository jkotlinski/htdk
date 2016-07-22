#include "word.h"

void Word::print(FILE* f) const {
    fprintf(f, "\n%s:%s\n", label, definition);
}
