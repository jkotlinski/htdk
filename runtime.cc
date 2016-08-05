#include "runtime.h"

#include <cstdio>

#include "dictionary.h"

const char* runtimeDefinition =
R"(; ACME Assembler
!cpu 6510
!ct pet
!to "%s",cbm

MSB = $73 ; msb stack is [$3b, $72]
LSB = $3b ; lsb stack is [3, $3a]

W = $8b ; rnd seed
W2 = $8d ; rnd seed

* = $801
!byte $b, $08, $a, 0, $9E, $32, $30, $36, $31, 0, 0, 0 ; basic header
    ldx #0
    jmp start
)";

void printHeader(FILE* f, const char* prgPath) {
    fprintf(f, runtimeDefinition, prgPath);
}
