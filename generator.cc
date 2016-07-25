#include "generator.h"

#include <cassert>
#include <deque>
#include <iostream>

#include "dictionary.h"

void generateAsm(FILE* f, const Tokens& tokens, Dictionary* dictionary) {
    std::deque<int> stack;
    int localLabel = 0;
    bool state = false;

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
                if (state) {
                    dictionary->markAsUsed("lit");
                    fprintf(f, "\tjsr lit\n\t!word %i\n", (int)it->data);
                } else {
                    stack.push_back((int)it->data);
                }
                break;
            case Code:
                {
                    const char* p = (const char*)it->data;
                    std::string wordName;
                    while (*p != ':') {
                        wordName.push_back(*p);
                        ++p;
                    }
                    dictionary->addWord(wordName.c_str());
                }
                fputs((const char*)it->data, f);
                break;
            case Colon:
                ++it;
                if (it == tokens.end() || it->type != WordName) {
                    fprintf(stderr, ": must be followed by a word name!");
                    exit(1);
                }
                fprintf(f, "\n%s:\n", (const char*)it->data);
                dictionary->addWord((const char*)it->data);
                state = true;
                break;
            case SemiColon:
                fputs("\trts\n", f);
                state = false;
                break;
            case Begin:
                stack.push_back(localLabel);
                fprintf(f, ".l%i:\n", localLabel++);
                break;
            case Again:
                fprintf(f, "\tjmp .l%i\n", stack.back());
                stack.pop_back();
                break;
            case Value:
                ++it;
                if (it == tokens.end() || it->type != WordName) {
                    fprintf(stderr, "value must be followed by a word name!");
                    exit(1);
                }
                fprintf(f, "\n%s:\n", (const char*)it->data);
                dictionary->addWord((const char*)it->data);
                fprintf(f, "\tlda #%i\n", stack.back() & 0xff);
                fprintf(f, "\tldy #%i\n", stack.back() >> 8);
                fprintf(f, "\tjmp pushya\n");
                stack.pop_back();
                dictionary->markAsUsed("pushya");
                break;
        }
    }
}
