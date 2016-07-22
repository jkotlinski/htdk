#include "runtime.h"

#include <cstdio>

const char* runtimeDefinition =
R"(; ACME Assembler
!cpu 6510
!ct pet

* = $801
!byte $b, $08, $a, 0, $9E, $32, $30, $36, $31, 0, 0, 0 ; basic header
    ldx #0
    jmp __start
)";

void printRuntime(FILE* f) {
    fprintf(f, runtimeDefinition);
}
