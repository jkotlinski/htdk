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

static const char equal[] = R"(:code =
    ldy #0
    lda LSB, x
    cmp LSB + 1, x
    bne +
    lda MSB, x
    cmp MSB + 1, x
    bne +
    dey
+   inx
    sty MSB, x
    sty LSB, x
    rts ;code)";

static const char zequal[] = R"(:code 0=
    ldy #0
    lda MSB, x
    bne +
    lda LSB, x
    bne +
    dey
+   sty MSB, x
    sty LSB, x
    rts ;code)";

static const char and_[] = R"(:code and
    lda MSB, x
    and MSB + 1, x
    sta MSB + 1, x

    lda LSB, x
    and LSB + 1, x
    sta LSB + 1, x

    inx
    rts ;code)";

static const char store[] = R"(:code !
    lda LSB, x
    sta W
    lda MSB, x
    sta W + 1

    ldy #0
    lda LSB+1, x
    sta (W), y
    iny
    lda MSB+1, x
    sta (W), y

    inx
    inx
    rts ;code)";

static const char fetch[] = R"(:code @
    lda LSB,x
    sta W
    lda MSB,x
    sta W+1

    ldy #0
    lda (W),y
    sta LSB,x
    iny
    lda (W),y
    sta MSB,x
    rts ;code)";

static const char fill[] = R"(:code fill
    lda LSB, x
    tay
    lda LSB + 2, x
    sta .fdst
    lda MSB + 2, x
    sta .fdst + 1
    lda LSB + 1, x
    eor #$ff
    sta W
    lda MSB + 1, x
    eor #$ff
    sta W + 1
    inx
    inx
    inx
-
    inc W
    bne +
    inc W + 1
    bne +
    rts
+
.fdst = * + 1
    sty $ffff ; overwrite

    ; advance
    inc .fdst
    bne -
    inc .fdst + 1
    jmp -)";

static const char ifcmp[] = R"(:code ifcmp
    inx
    lda MSB-1,x
    ora LSB-1,x
    rts ;code)";

static const char mul[] = ": * m* drop ;";

// -----

const char* getDefinition(const char* wordName) {
    struct Pair {
        const char* name;
        const char* definition;
    };
    static const Pair defs[] = {
        { "*", mul },
        { "!", store },
        { "+", plus },
        { "-", minus },
        { "0=", zequal },
        { "1+", oneplus },
        { "1-", oneminus },
        { "2+", twoplus },
        { "2dup", twodup },
        { "=", equal },
        { "@", fetch },
        { "and", and_ },
        { "c!", cstore },
        { "c@", cfetch },
        { "dup", dup },
		{ "fill", fill },
        { "lit", lit },
        { "over", over },
        { "rot", rot },
        { "swap", swap },
        // internal
        { "ifcmp", ifcmp },
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
