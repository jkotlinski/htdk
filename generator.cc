#include "generator.h"

#include <iostream>

void generateAsm(FILE* f, const Tokens& tokens) {
    for (auto it = tokens.begin(); it != tokens.end(); ++it) {
        switch (it->type) {
            case WordName:
                // printf("'%s'\n", (const char*)it->data);
                break;
            case Number:
                // printf("%i\n", (int)it->data);
                break;
            case Colon:
                ++it;
                if (it == tokens.end() || it->type != WordName) {
                    fprintf(stderr, ": must be followed by a word name!");
                    exit(1);
                }
                fprintf(f, "__%s:\n", (const char*)it->data);
                break;
            case SemiColon:
                fputs("\trts", f);
                break;
        }
    }
}
