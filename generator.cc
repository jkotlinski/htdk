#include "generator.h"

#include <cassert>
#include <deque>
#include <iostream>

#include "dictionary.h"
#include "label.h"

void generateAsm(FILE* f, const Tokens& tokens, Dictionary* dictionary) {
    std::deque<int> stack;
    int localLabel = 0;
    bool state = false;

    for (auto it = tokens.begin(); it != tokens.end(); ++it) {
        switch (it->type) {
            case WordName:
                {
                    char* wordName = it->stringData;
                    ++it;
                    if (it != tokens.end() && it->type == SemiColon) {
                        // Tail call.
                        fprintf(f, "\tjmp %s\n", label(wordName).c_str());
                    } else {
                        --it;
                        fprintf(f, "\tjsr %s\n", label(wordName).c_str());
                    }
                    dictionary->markAsUsed(wordName);
                    free(wordName);
                }
                break;
            case Number:
                if (state) {
                    dictionary->markAsUsed("lit");
                    fprintf(f, "\tjsr lit\n\t!word %i\n", it->intData);
                } else {
                    stack.push_back(it->intData);
                }
                break;
            case Code:
                {
                    char* p = it->stringData;
                    std::string wordName;
                    while (!isspace(*p)) {
                        wordName.push_back(*p);
                        ++p;
                    }
                    dictionary->addWord(wordName.c_str());
                    if (*p == '\n') {
                        ++p;
                    }
                    if (label(wordName.c_str()) != wordName) {
                        fprintf(f, "%s:\t; %s\n%s\n", label(wordName.c_str()).c_str(),
                                wordName.c_str(), p);
                    } else {
                        fprintf(f, "%s:\n%s\n", wordName.c_str(), p);
                    }
                }
                free(it->stringData);
                break;
            case Colon:
                ++it;
                if (it == tokens.end() || it->type != WordName) {
                    fprintf(stderr, ": must be followed by a word name!");
                    exit(1);
                }
                fprintf(f, "\n%s:", label(it->stringData).c_str());
                dictionary->addWord(it->stringData);
                if (it->stringData != label(it->stringData)) {
                    fprintf(f, "\t; %s", it->stringData);
                }
                fprintf(f, "\n");
                free(it->stringData);
                state = true;
                break;
            case Drop:
                fputs("\tinx\t; drop\n", f);
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
                fprintf(f, "\n%s:\n", label(it->stringData).c_str());
                dictionary->addWord(it->stringData);
                fprintf(f, "\tlda #%i\n", stack.back() & 0xff);
                fprintf(f, "\tldy #%i\n", stack.back() >> 8);
                fprintf(f, "\tjmp pushya\n");
                stack.pop_back();
                dictionary->markAsUsed("pushya");
                free(it->stringData);
                break;
        }
    }
}
