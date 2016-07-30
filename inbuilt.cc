#include "inbuilt.h"

#include <cstring>

#include "dictionary.h"

static const char* const pushya = R"(
:code pushya
    dex
    sta LSB,x
    sty MSB,x
+   rts ;code)";

static const char* const oneplus = R"(
:code 1+
    inc LSB,x
    bne +
    inc MSB,x
+   rts ;code)";

static const char* const cfetch = R"(
:code c@
    lda LSB,x
    sta + + 1
    lda MSB,x
    sta + + 2
+   lda $cafe
    sta LSB,x
    lda #0
    sta MSB,x
    rts ;code)";

static const char* const cstore = R"(
:code c!
    lda LSB,x
    sta + + 1
    lda MSB,x
    sta + + 2
    lda LSB+1,x
+   sta $1234
    inx
    inx
    rts ;code)";

static const char* const lit = R"(
:code lit
    dex
    pla
    sta W
    pla
    sta W + 1
    ldy #1
    lda (W), y
    sta LSB, x
    iny
    lda (W), y
    sta MSB, x
    lda W
    clc
    adc #3
    sta + + 1
    lda W + 1
    adc #0
    sta + + 2
+   jmp $1234 ;code)";

static const char* const rot = R"(
:code rot
    ldy MSB+2, x
    lda MSB+1, x
    sta MSB+2, x
    lda MSB, x
    sta MSB+1, x
    sty MSB, x
    ldy LSB+2, x
    lda LSB+1, x
    sta LSB+2, x
    lda LSB, x
    sta LSB+1, x
    sty LSB, x
    rts ;code)";

const char* getDefinition(const char* wordName) {
    struct Pair {
        const char* name;
        const char* definition;
    };
    const Pair defs[] = {
        { "rot", rot },
        { "c!", cstore },
        { "c@", cfetch },
        { "lit", lit },
        { "1+", oneplus },
        { "pushya", pushya },
        { nullptr, nullptr }
    };
    const Pair* it = defs;
    while (it->name) {
        if (!strcmp(label(it->name).c_str(), wordName)) {
            return it->definition;
        }
        ++it;
    }
    fprintf(stderr, "Cannot find definition for '%s'\n", wordName);
    exit(1);
    return nullptr;
}
