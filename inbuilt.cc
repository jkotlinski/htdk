#include "inbuilt.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

static const char pushya[] = R"(:code (pushya)
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

static const char lit[] = R"(:code (lit)
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

static const char qdup[] = R"(:code ?dup
    lda MSB,x
    ora LSB,x
    beq +
    dex
    lda MSB + 1, x
    sta MSB, x
    lda LSB + 1, x
    sta LSB, x
+   rts ;code)";

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

static const char ifcmp[] = R"(:code (if)
    inx
    lda MSB-1,x
    ora LSB-1,x
    rts ;code)";

static const char mul[] = ": * m* drop ;";

static const char zless[] = R"(:code 0<
    lda MSB,x
    and #$80
    beq +
    lda #$ff
+   sta MSB,x
    sta LSB,x
    rts ;code)";

static const char twomul[] = R"(:code 2*
    asl LSB,x
    rol MSB,x
    rts ;code)";

static const char depth[] = R"(:code depth
    txa
    eor #$ff
    tay
    iny
    dex
    sty LSB,x
    lda #0
    sta MSB,x
    rts ;code)";

static const char do_[] = R"(:code (do) ; ( limit first -- )
    pla
    sta W
    pla
    tay

    lda MSB+1,x
    pha
    lda LSB+1,x
    pha
    lda MSB,x
    pha
    lda LSB,x
    pha
    inx
    inx

    tya
    pha
    lda W
    pha
    rts ;code)";

static const char rat[] = R"(:code r@
    txa
    tsx
    ldy $103,x
    sty W
    ldy $104,x
    tax
    dex
    sty MSB,x
    lda W
    sta LSB,x
    rts ;code)";

static const char cr[] = ": cr $d emit ;";

static const char emit[] = R"(:code emit
    lda LSB,x
    inx
    jmp $ffd2 ; PUTCHR ;code)";

static const char litstring[] =
    ": (litstring) r> 1+ dup 1+ swap c@ 2dup + 1- >r ;";

static const char tor[] = R"(:code >r
    pla
    sta W
    pla
    sta W+1
    inc W
    bne +
    inc W+1
+
    lda MSB,x
    pha
    lda LSB,x
    pha
    inx
    jmp (W) ;code)";

static const char rto[] = R"(:code r>
    pla
    sta W
    pla
    sta W+1
    inc W
    bne +
    inc W+1
+
    dex
    pla
    sta LSB,x
    pla
    sta MSB,x
    jmp (W) ;code)";

static const char type[] = R"(: type
    \ 0 d4 c! ( quote mode off )
    begin ?dup while
    swap dup c@ emit 1+ swap 1-
    repeat drop ;)";

static const char branch[] = R"(:code (branch)
    pla
    sta W
    pla
    sta W + 1

    ldy #2
    lda (W), y
    sta + + 2
    dey
    lda (W), y
    sta + + 1
+   jmp $1234
;code)";

static const char loop[] = R"(:code (loop)
    stx W
    tsx
    inc 103,x
    bne +
    inc 104,x
+   lda 103,x
    cmp 105,x
    beq +
-
	; loop not finished, return
    ldx W
	rts
+
	lda 104,x
	cmp 106,x
	bne -

	; loop done
	pla
	clc
	adc #4 ; skip loop jmp in calling code
	sta W2
	pla
	adc #0
	sta W2+1
	txa
	clc
	adc #6 ; pop loop counters
	tax
	txs
	ldx W
	jmp (W2) ;code)";

static const char negate[] = ": negate invert 1+ ;";

static const char invert[] = R"(:code invert
    lda MSB,x
    eor #$ff
    sta MSB,x
    lda LSB,x
    eor #$ff
    sta LSB,x
    rts ;code)";

static const char quit[] = R"(:code quit
    ; Returns to the address on the bottom of the return stack.
    ldx #$fd
    txs
    rts ;code)";

// -----

const char* getDefinition(const char* wordName) {
    struct Pair {
        const char* name;
        const char* definition;
    };
    static const Pair defs[] = {
        { "type", type },
        { "quit", quit },
        { "invert", invert },
        { ">r", tor },
        { "r>", rto },
        { "emit", emit },
        { "cr", cr },
        { "r@", rat },
        { "depth", depth },
        { "2*", twomul },
        { "*", mul },
        { "!", store },
        { "+", plus },
        { "-", minus },
        { "0=", zequal },
        { "0<", zless },
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
        { "?dup", qdup },
		{ "fill", fill },
        { "over", over },
        { "rot", rot },
        { "swap", swap },
        { "negate", negate },
        // internal
        { "(loop)", loop },
        { "(do)", do_ },
        { "(lit)", lit },
        { "(litstring)", litstring },
        { "(if)", ifcmp },
        { "(pushya)", pushya },
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
