#include "runtime.h"

#include <cstdio>

const char* runtimeDefinition =
R"(; ACME Assembler
!cpu 6510
!ct pet

MSB = $73 ; msb stack is [$3b, $72]
LSB = $3b ; lsb stack is [3, $3a]

W = $8b ; rnd seed

* = $801
!byte $b, $08, $a, 0, $9E, $32, $30, $36, $31, 0, 0, 0 ; basic header
    ldx #0
    jmp __start

lit:
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
+   jmp $1234
)";

void printRuntime(FILE* f) {
    fprintf(f, runtimeDefinition);
}
