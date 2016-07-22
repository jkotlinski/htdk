#pragma once

const char* const oneplus = R"(
    inc LSB,x
    bne +
    inc MSB,x
+   rts)";

const char* const cfetch = R"(
    lda LSB,x
    sta + + 1
    lda MSB,x
    sta + + 2
+   lda $cafe
    sta LSB,x
    lda #0
    sta MSB,x
    rts)";

const char* const cstore = R"(
    lda LSB,x
    sta + + 1
    lda MSB,x
    sta + + 2
    lda LSB+1,x
+   sta $1234
    inx
    inx
    rts)";

const char* const lit = R"(
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
+   jmp $1234)";

const char* const rot = R"(
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
    rts)";
