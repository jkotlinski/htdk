#include "inbuilt.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

static const char count[] = ": count dup 1+ swap c@ ;";
static const char execute[] = R"(:code execute
    lda LSB,x
    sta W
    lda MSB,x
    sta W+1
    inx
    jmp (W) ;code)";

static const char charplus[] = ": char+ 1+ ;";
static const char cellplus[] = ": cell+ 2+ ;";
static const char twofetch[] = ": 2@ dup 2+ @ swap @ ;";
static const char twostore[] = ": 2! swap over ! 2+ ! ;";

static const char bl[] = "$20 value bl";

static const char plusstore[] = R"(:code +!
    lda LSB,x
    sta W
    lda MSB,x
    sta W+1
    ldy #0
    clc
    lda (W),y
    adc LSB+1,x
    sta (W),y
    iny
    lda (W),y
    adc MSB+1,x
    sta (W),y
    inx
    inx
    rts ;code)";

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

static const char litc[] = R"(:code (litc)
    dex

    pla
    sta W
    pla
    sta W + 1

    inc W
    bne +
    inc W + 1
+
    ldy #0
    lda (W), y
    sta LSB, x
    sty MSB, x

    inc W
    bne +
    inc W + 1
+   jmp (W) ;code)";

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

static const char within[] = ": within over - >r - r> u< ;";

static const char plusloop[] = R"(: (plusloop)
r> swap r> 2dup +
rot 0< if tuck swap else tuck then
r@ 1- rot rot within 0= if
>r >r else
r> 2drop 3 + >r then ;)";

static const char loop[] = R"(:code (loop)
    stx W
    tsx
    inc $103,x
    bne +
    inc $104,x
+   lda $103,x
    cmp $105,x
    beq +
-
	; loop not finished, return
    ldx W
	rts
+
	lda $104,x
	cmp $106,x
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

static const char twodrop[] = ": 2drop drop drop ;";

static const char unloop[] = ": unloop r> r> r> 2drop >r ;";

static const char or_[] = R"(:code or
    lda MSB,x
    ora MSB+1,x
    sta MSB+1,x
    lda LSB,x
    ora LSB+1,x
    sta LSB+1,x
    inx
    rts ;code)";

static const char xor_[] = R"(:code xor
    lda MSB,x
    eor MSB+1,x
    sta MSB+1,x
    lda LSB,x
    eor LSB+1,x
    sta LSB+1,x
    inx
    rts ;code)";

static const char twodiv[] = R"(:code 2/
    lda MSB,x
    cmp #$80
    ror MSB,x
    ror LSB,x
    rts ;code)";

static const char lshift[] = R"(:code lshift
    dec LSB,x
    bmi +
    asl LSB+1,x
    rol MSB+1,x
    jmp lshift
+   inx
    rts ;code)";

static const char rshift[] = R"(:code rshift
    dec LSB,x
    bmi +
    lsr MSB+1,x
    ror LSB+1,x
    jmp rshift
+   inx
    rts ;code)";

static const char lt[] = R"(:code <
    ldy #0
    sec
    lda LSB+1,x
    sbc LSB,x
    lda MSB+1,x
    sbc MSB,x
    bvc +
    eor #$80
+   bpl +
    dey
+   inx
    sty LSB,x
    sty MSB,x
    rts ;code)";

static const char gt[] = ": > swap < ;";

static const char max[] = ": max 2dup < if swap then drop ;";
static const char min[] = ": min 2dup > if swap then drop ;";

static const char uless[] = R"(:code u<
    ldy #0
    lda MSB, x
    cmp MSB + 1, x
    bcc .false
    bne .true
    ; ok, msb are equal...
    lda LSB + 1, x
    cmp LSB, x
    bcs .false
.true
    dey
.false
    inx
    sty MSB, x
    sty LSB, x
    rts ;code)";

static const char twoover[] = R"(:code 2over
    dex
    lda MSB+4,x
    sta MSB,x
    lda LSB+4,x
    sta LSB,x
    dex
    lda MSB+4,x
    sta MSB,x
    lda LSB+4,x
    sta LSB,x
    rts ;code)";

static const char twoswap[] = R"(:code 2swap
    lda LSB,x
    ldy LSB+2,x
    sty LSB,x
    sta LSB+2,x
    lda MSB,x
    ldy MSB+2,x
    sty MSB,x
    sta MSB+2,x
    lda LSB+1,x
    ldy LSB+3,x
    sty LSB+1,x
    sta LSB+3,x
    lda MSB+1,x
    ldy MSB+3,x
    sty MSB+1,x
    sta MSB+3,x
    rts ;code)";

static const char qnegate[] = ": ?negate 0< if negate then ;";
static const char abs_[] = ": abs dup ?negate ;";

static const char dnegate[] = ": dnegate invert >r invert r> 1 m+ ;";
static const char qdnegate[] = ": ?dnegate 0< if dnegate then ;";
static const char mmul[] = ": m* 2dup xor >r >r abs r> abs um* r> ?dnegate ;";
static const char sgtd[] = ": s>d dup 0< ;";

static const char mplus[] = R"(:code m+
    ldy #0
    lda MSB,x
    bpl +
    dey
+   clc
    lda LSB,x
    adc LSB+2,x
    sta LSB+2,x
    lda MSB,x
    adc MSB+2,x
    sta MSB+2,x
    tya
    adc LSB+1,x
    sta LSB+1,x
    tya
    adc MSB+1,x
    sta MSB+1,x
    inx
    rts ;code)";

static const char ummul[] = R"(:code um*
product = W
    lda #0
    sta product+2 ; clear upper bits of product
    sta product+3
    ldy #$10 ; set binary count to 16
.shift_r
    lsr MSB + 1, x ; multiplier+1 ; divide multiplier by 2
    ror LSB + 1, x ; multiplier
    bcc rotate_r
    lda product+2 ; get upper half of product and add multiplicand
    clc
    adc LSB, x ; multiplicand
    sta product+2
    lda product+3
    adc MSB, x ; multiplicand+1
rotate_r
    ror ; rotate partial product
    sta product+3
    ror product+2
    ror product+1
    ror product
    dey
    bne .shift_r

    lda product
    sta LSB + 1, x
    lda product + 1
    sta MSB + 1, x
    lda product + 2
    sta LSB, x
    lda product + 3
    sta MSB, x
    rts ;code)";

static const char slash[] = ": / /mod nip ;";
static const char slashmod[] = ": /mod >r s>d r> fm/mod ;";
static const char starslash[] = ": */ */mod nip ;";
static const char starslashmod[] = ": */mod >r m* r> fm/mod ;";
static const char mod[] = ": mod /mod drop ;";
static const char dabs[] = ": dabs dup ?dnegate ;";
static const char nip[] = ": nip swap drop ;";
static const char tuck[] = ": tuck swap over ;";

static const char fmmod[] = R"(: fm/mod
dup >r
dup 0< if negate >r dnegate r> then
over 0< if tuck + swap then
um/mod
r> 0< if swap negate swap then ;)";

static const char smrem[] = R"(: sm/rem
2dup xor >r over >r abs >r dabs
r> um/mod swap r> ?negate
swap r> ?negate ;)";

static const char ummod[] = R"(:code um/mod
; UM/MOD by Garth Wilson
; http://6502.org/source/integers/ummodfix/ummodfix.htm
; ( lsw msw divisor -- rem quot )
        N = W
        SEC
        LDA     LSB+1,X     ; Subtract hi cell of dividend by
        SBC     LSB,X     ; divisor to see if there's an overflow condition.
        LDA     MSB+1,X
        SBC     MSB,X
        BCS     oflo    ; Branch if /0 or overflow.

        LDA     #17     ; Loop 17x.
        STA     N       ; Use N for loop counter.
loop:   ROL     LSB+2,X     ; Rotate dividend lo cell left one bit.
        ROL     MSB+2,X
        DEC     N       ; Decrement loop counter.
        BEQ     end     ; If we're done, then branch to end.
        ROL     LSB+1,X     ; Otherwise rotate dividend hi cell left one bit.
        ROL     MSB+1,X
        lda     #0
        sta     N+1
        ROL     N+1     ; Rotate the bit carried out of above into N+1.

        SEC
        LDA     LSB+1,X     ; Subtract dividend hi cell minus divisor.
        SBC     LSB,X
        STA     N+2     ; Put result temporarily in N+2 (lo byte)
        LDA     MSB+1,X
        SBC     MSB,X
        TAY             ; and Y (hi byte).
        LDA     N+1     ; Remember now to bring in the bit carried out above.
        SBC     #0
        BCC     loop

        LDA     N+2     ; If that didn't cause a borrow,
        STA     LSB+1,X     ; make the result from above to
        STY     MSB+1,X     ; be the new dividend hi cell
        bcs     loop    ; and then branch up.

oflo:   LDA     #$FF    ; If overflow or /0 condition found,
        STA     LSB+1,X     ; just put FFFF in both the remainder
        STA     MSB+1,X
        STA     LSB+2,X     ; and the quotient.
        STA     MSB+2,X

end:    INX
        jmp swap ;code)";

static const char j[] = R"(:code j
    txa
    tsx
    ldy $107,x
    sty W
    ldy $108,x
    tax
    dex
    sty MSB,x
    lda W
    sta LSB,x
    rts ;code)";

// -----

const char* getDefinition(const char* wordName) {
    struct Pair {
        const char* name;
        const char* definition;
    };
    static const Pair defs[] = {
        { "um/mod", ummod },
        { "tuck", tuck },
        { "dabs", dabs },
        { "nip", nip },
        { "sm/rem", smrem },
        { "fm/mod", fmmod },
        { "/mod", slashmod },
        { "mod", mod },
        { "/", slash },
        { "*/mod", starslashmod },
        { "*/", starslash },
        { "um*", ummul },
        { "s>d", sgtd },
        { "m+", mplus },
        { "dnegate", dnegate },
        { "?dnegate", qdnegate },
        { "m*", mmul },
        { "?negate", qnegate },
        { "abs", abs_ },
        { "2swap", twoswap },
        { "2over", twoover },
        { "u<", uless },
        { "min", min },
        { "max", max },
        { "<", lt },
        { ">", gt },
        { "rshift", rshift },
        { "lshift", lshift },
        { "2/", twodiv },
        { "2drop", twodrop },
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
        { "unloop", unloop },
        { "or", or_ },
        { "xor", xor_ },
        { "2!", twostore },
        { "2@", twofetch },
        { "cell+", cellplus },
        { "char+", charplus },
        { "+!", plusstore },
        { "bl", bl },
        { "execute", execute },
        { "count", count },
        { "within", within },
        { "j", j },
        // internal
        { "(loop)", loop },
        { "(plusloop)", plusloop },
        { "(do)", do_ },
        { "(lit)", lit },
        { "(litc)", litc },
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
