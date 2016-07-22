#include "generator.h"

#include <cassert>
#include <deque>
#include <iostream>

#include "dictionary.h"

void generateAsm(FILE* f, const Tokens& tokens, Dictionary* dictionary) {
    std::deque<int> stack;
    int localLabel = 0;

    for (auto it = tokens.begin(); it != tokens.end(); ++it) {
        switch (it->type) {
            case WordName:
                {
                    const char* wordName = (const char*)it->data;
                    ++it;
                    if (it != tokens.end() && it->type == SemiColon) {
                        // Tail call.
                        fprintf(f, "\tjmp %s\n", dictionary->label(wordName));
                    } else {
                        --it;
                        fprintf(f, "\tjsr %s\n", dictionary->label(wordName));
                    }
                    dictionary->markAsUsed(wordName);
                }
                break;
            case Number:
                dictionary->markAsUsed("lit");
                fprintf(f, "\tjsr lit\n\t!word %i\n", (int)it->data);
                break;
            case Colon:
                ++it;
                if (it == tokens.end() || it->type != WordName) {
                    fprintf(stderr, ": must be followed by a word name!");
                    exit(1);
                }
                fprintf(f, "\n%s:\n", (const char*)it->data);
                dictionary->addWord((const char*)it->data);
                break;
            case SemiColon:
                fputs("\trts\n", f);
                break;
            case Begin:
                stack.push_back(localLabel);
                fprintf(f, ".l%i:\n", localLabel++);
                break;
            case Again:
                fprintf(f, "\tjmp .l%i\n", stack.back());
                stack.pop_back();
                break;
        }
    }
}
