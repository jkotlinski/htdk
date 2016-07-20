#include "generator.h"

#include <iostream>

void generate_asm(const Tokens& tokens) {
    for (auto it = tokens.begin(); it != tokens.end(); ++it) {
        switch (it->type) {
            case WordName:
                printf("'%s'\n", (const char*)it->data);
                break;
            case Number:
                printf("%i\n", (int)it->data);
                break;
            case Colon:
            case SemiColon:
                break;
        }
    }
}
