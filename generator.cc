#include "generator.h"

#include <cassert>
#include <cstring>
#include <deque>
#include <iostream>
#include <map>

#include "dictionary.h"
#include "label.h"

#define LPAREN "_28"
#define RPAREN "_29"

static void compileCall(FILE* f, const char* wordName, const Tokens& tokens,
        Tokens::const_iterator* it, bool* state, Dictionary* dictionary) {
    static const char* noTailCallEliminationWords[] = {
        "r@",
        "r>",
        "unloop",
        ">r",
        nullptr
    };
    ++*it;
    bool tailCall = (*it != tokens.end() && (*it)->type == SemiColon);
    const char** tceIt = noTailCallEliminationWords;
    while (tailCall && *tceIt) {
        if (!strcmp(wordName, *tceIt)) {
            tailCall = false;
        }
        ++tceIt;
    }
    --*it;
    fprintf(f, tailCall ? "\tjmp %s\n" : "\tjsr %s\n", label(wordName).c_str());
    if (tailCall) {
        ++*it;  // Skips ;
        *state = false;
    }
    dictionary->markAsUsed(wordName);
}

static std::string create(FILE* f, const Tokens& tokens, Tokens::const_iterator* it, Dictionary* dictionary) {
    ++*it;
    if (*it == tokens.end() || (*it)->type != WordName) {
        fprintf(stderr, "create must be followed by a word name!");
        exit(1);
    }
    fprintf(f, "\n%s:\n", label((*it)->stringData).c_str());
    fprintf(f, "\tlda #<+\n\tldy #>+\n\tjmp " LPAREN "pushya" RPAREN "\n+\n");
    const std::string name = (*it)->stringData;
    dictionary->addWord((*it)->stringData);
    dictionary->markAsUsed("(pushya)");
    free((*it)->stringData);
    return name;
}

void generateAsm(FILE* f, const Tokens& tokens, Dictionary* dictionary) {
    static int loopCounter;
    static int localLabel;
    static int variableLabel;
    static int hereLabel;

    const int HERE_MASK = 1 << 31;

    std::deque<int> stack;
    std::deque<int> loopStack;
    bool state = false;
    std::set<int> undefinedVariables;
    std::map<std::string, int> variableLabels;

    for (auto it = tokens.begin(); it != tokens.end(); ++it) {
        switch (it->type) {
            case Comma:
                fprintf(f, "\t!word %i\n", stack.back());
                stack.pop_back();
                break;
            case Here:
                stack.push_back(hereLabel | HERE_MASK);
                fprintf(f, "here_%i = *\n", hereLabel++);
                break;
            case While:
                fprintf(f, "\tjsr " LPAREN "if" RPAREN "\n\tbeq .l%i\n", localLabel);
                dictionary->markAsUsed("(if)");
                {
                    const int tmp = stack.back();
                    stack.pop_back();
                    stack.push_back(localLabel++);
                    stack.push_back(tmp);
                }
                break;
            case Repeat:
                fprintf(f, "\tjmp .l%i\n", stack.back());
                stack.pop_back();
                fprintf(f, ".l%i:\n", stack.back());
                stack.pop_back();
                break;
            case Loop:
                fprintf(f, "\tjsr " LPAREN "loop" RPAREN "\n\tjmp .l%i\n.loopexit_%i:\n",
                        stack.back(),
                        loopStack.back());
                stack.pop_back();
                loopStack.pop_back();
                dictionary->markAsUsed("(loop)");
                break;
            case Leave:
                fprintf(f, "\tjsr unloop\n\tjmp .loopexit_%i\n", loopStack.back());
                dictionary->markAsUsed("unloop");
                break;
            case I:
                assert(state);
                fprintf(f, "\tjsr r_40\t; i\n");
                dictionary->markAsUsed("r@");
                break;
            case None:
                assert(!"Invalid token");
                break;
            case Do:
                assert(state);
                fprintf(f, "\tjsr " LPAREN "do" RPAREN "\t; loop %i\n.l%i:\n", loopCounter, localLabel);
                dictionary->markAsUsed("(do)");
                loopStack.push_back(loopCounter++);
                stack.push_back(localLabel++);
                break;
            case Cells:
                if (state) {
                    compileCall(f, "2*", tokens, &it, &state, dictionary);
                } else {
                    stack.back() *= 2;
                }
                break;
            case Allot:
                assert(!stack.empty());
                fprintf(f, "* = * + %i\n", stack.back());
                stack.pop_back();
                break;
            case Create:
                create(f, tokens, &it, dictionary);
                break;
            case String:
                fprintf(f, "\tjsr " LPAREN "litstring" RPAREN "\n!byte %i\n!text \"%s\"\n",
                        (int)strlen(it->stringData), it->stringData);
                dictionary->markAsUsed("(litstring)");
                free(it->stringData);
                break;
            case Variable:
                {
                    const std::string name = create(f, tokens, &it, dictionary);
                    fprintf(f, "!word vl_%i\n", variableLabel);
                    undefinedVariables.insert(variableLabel);
                    variableLabels[name] = variableLabel++;
                }
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
                    compileCall(f, "!", tokens, &it, &state, dictionary);
                }
                break;
            case WordName:
                // printf("WordName %s\n", it->stringData);
                assert(it->stringData);
                if (state) {
                    char* wordName = it->stringData;
                    compileCall(f, wordName, tokens, &it, &state, dictionary);
                    free(wordName);
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
                // puts("Number");
                if (state) {
                    dictionary->markAsUsed("(lit)");
                    fprintf(f, "\tjsr " LPAREN "lit" RPAREN "\n\t!word %i\n", it->intData);
                } else {
                    stack.push_back(it->intData);
                }
                break;
            case Code:
                // puts("Code");
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
                fprintf(f, "\tjsr " LPAREN "if" RPAREN "\n\tbeq .l%i\n", localLabel);
                dictionary->markAsUsed("(if)");
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
            case Constant:
            case Value:
                ++it;
                if (it == tokens.end() || it->type != WordName) {
                    fprintf(stderr, "value must be followed by a word name!");
                    exit(1);
                }
                fprintf(f, "\n%s:\n", label(it->stringData).c_str());
                dictionary->addWord(it->stringData);
                if (stack.back() & HERE_MASK) {
                    fprintf(f, "\tlda #<here_%i\n", stack.back() ^ HERE_MASK);
                    fprintf(f, "\tldy #>here_%i\n", stack.back() ^ HERE_MASK);
                } else {
                    fprintf(f, "\tlda #%i\n", stack.back() & 0xff);
                    fprintf(f, "\tldy #%i\n", stack.back() >> 8);
                }
                fprintf(f, "\tjmp " LPAREN "pushya" RPAREN "\n");
                stack.pop_back();
                dictionary->markAsUsed("(pushya)");
                free(it->stringData);
                break;
        }
    }

    while (!undefinedVariables.empty()) {
        fprintf(f, "vl_%i = 0 ; undefined\n", *undefinedVariables.begin());
        undefinedVariables.erase(undefinedVariables.begin());
    }
}
