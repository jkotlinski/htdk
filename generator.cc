#include "generator.h"

#include <cassert>
#include <cstring>
#include <deque>
#include <iostream>
#include <map>

#include "dictionary.h"
#include "label.h"

static void compileCall(FILE* f, const char* wordName, bool tailCall, Dictionary* dictionary) {
    fprintf(f, tailCall ? "\tjmp %s\n" : "\tjsr %s\n", label(wordName).c_str());
    dictionary->markAsUsed(wordName);
}

void generateAsm(FILE* f, const Tokens& tokens, Dictionary* dictionary) {
    std::deque<int> stack;
    int localLabel = 0;
    bool state = false;
    std::set<int> undefinedVariables;
    int variableLabel = 0;
    std::map<std::string, int> variableLabels;

    for (auto it = tokens.begin(); it != tokens.end(); ++it) {
        switch (it->type) {
            case String:
                fprintf(f, "\tjsr litstring\n!byte %i\n!text \"%s\"\n",
                        (int)strlen(it->stringData), it->stringData);
                dictionary->markAsUsed("litstring");
                free(it->stringData);
                break;
            case Variable:
                ++it;
                if (it == tokens.end() || it->type != WordName) {
                    fprintf(stderr, "variable must be followed by a word name!");
                    exit(1);
                }
                fprintf(f, "\n%s:\n", label(it->stringData).c_str());
                dictionary->addWord(it->stringData);
                fprintf(f, "\tlda #<+\n\tldy #>+\n\tjmp pushya\n+\n!word vl_%i\n", variableLabel);
                undefinedVariables.insert(variableLabel);
                variableLabels[it->stringData] = variableLabel;
                ++variableLabel;
                dictionary->markAsUsed("pushya");
                free(it->stringData);
                break;
            case Store:
                if (!state) {
                    int variableLabel = stack.back();
                    stack.pop_back();
                    int value = stack.back();
                    stack.pop_back();
                    fprintf(f, "vl_%i = %i\n", variableLabel, value);
                    undefinedVariables.erase(variableLabel);
                    break;
                } else {
                    ++it;
                    bool tailCall = (it != tokens.end() && it->type == SemiColon);
                    --it;
                    compileCall(f, "!", tailCall, dictionary);
                }
                break;
            case WordName:
                // printf("WordName %s\n", it->stringData);
                assert(it->stringData);
                if (state) {
                    ++it;
                    bool tailCall = (it != tokens.end() && it->type == SemiColon);
                    --it;
                    compileCall(f, it->stringData, tailCall, dictionary);
                    if (tailCall) {
                        ++it;  // Skips ;
                    }
                    free(it->stringData);
                } else {
                    char* wordName = it->stringData;
                    if (variableLabels.find(wordName) != variableLabels.end()) {
                        stack.push_back(variableLabels[wordName]);
                        free(it->stringData);
                    } else {
                        fprintf(stderr, "Variable '%s' not defined\n", wordName);
                        exit(1);
                    }
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
                    fprintf(stderr, ": must be followed by a word name! (is type %i)\n", it->type);
                    exit(1);
                }
                fprintf(f, "\n%s:", label(it->stringData).c_str());
                // printf("Colon %s\n", it->stringData);
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
            case If:
                fprintf(f, "\tjsr ifcmp\n\tbeq .l%i\n", localLabel);
                dictionary->markAsUsed("ifcmp");
                stack.push_back(localLabel++);
                break;
            case Else:
                fprintf(f, "\tjmp .l%i\n.l%i:\n", localLabel, stack.back());
                stack.pop_back();
                stack.push_back(localLabel++);
                break;
            case Then:
                fprintf(f, ".l%i:\n", stack.back());
                stack.pop_back();
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
