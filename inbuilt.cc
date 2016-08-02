#include "inbuilt.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

static const char pushya[] = R"(:code pushya
    dex
    sta LSB,x
    sty MSB,x
+   rts ;code)";

static const char oneplus[] = R"(:code 1+
    inc LSB,x
    bne +
    inc MSB,x
+   rts ;code)";

static const char oneminus[] = R"(:code 1-
    lda LSB, x
    bne +
    dec MSB, x
+   dec LSB, x
    rts ;code)";

static const char plus[] = R"(:code +
    lda LSB, x
    clc
    adc LSB + 1, x
    sta LSB + 1, x

    lda MSB, x
    adc MSB + 1, x
    sta MSB + 1, x

    inx
    rts ;code)";

static const char minus[] = R"(:code -
    lda LSB + 1, x
    sec
    sbc LSB, x
    sta LSB + 1, x
    lda MSB + 1, x
    sbc MSB, x
    sta MSB + 1, x
    inx
    rts ;code)";

static const char twoplus[] = R"(: 2+ 1+ 1+ ;)";

static const char cfetch[] = R"(:code c@
    lda LSB,x
    sta + + 1
    lda MSB,x
    sta + + 2
+   lda $cafe
    sta LSB,x
    lda #0
    sta MSB,x
    rts ;code)";

static const char cstore[] = R"(:code c!
    lda LSB,x
    sta + + 1
    lda MSB,x
    sta + + 2
    lda LSB+1,x
+   sta $1234
    inx
    inx
    rts ;code)";

static const char swap[] = R"(:code swap
    ldy MSB, x
    lda MSB + 1, x
    sta MSB, x
    sty MSB + 1, x

    ldy LSB, x
    lda LSB + 1, x
    sta LSB, x
    sty LSB + 1, x
    rts ;code)";

static const char lit[] = R"(:code lit
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

static const char rot[] = R"(:code rot
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

static const char over[] = R"(:code over
    dex
    lda MSB + 2, x
    sta MSB, x
    lda LSB + 2, x
    sta LSB, x
    rts ;code)";

static const char dup[] = R"(:code dup
    dex
    lda MSB + 1, x
    sta MSB, x
    lda LSB + 1, x
    sta LSB, x
    rts ;code)";

static const char twodup[] = R"(: 2dup over over ;)";

// -----

const char* getDefinition(const char* wordName) {
    struct Pair {
        const char* name;
        const char* definition;
    };
    static const Pair defs[] = {
        { "dup", dup },
        { "2dup", twodup },
        { "over", over },
        { "swap", swap },
        { "rot", rot },
        { "c!", cstore },
        { "c@", cfetch },
        { "lit", lit },
        { "1+", oneplus },
        { "1-", oneminus },
        { "+", plus },
        { "-", minus },
        { "2+", twoplus },
        { "pushya", pushya },
        { nullptr, nullptr }
    };
    const Pair* it = defs;
    while (it->name) {
        if (!strcmp(it->name, wordName)) {
            return it->definition;
        }
        ++it;
    }
    fprintf(stderr, "Cannot find definition for '%s'\n", wordName);
    exit(1);
    return nullptr;
}
