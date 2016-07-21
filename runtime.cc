#include "runtime.h"

#include <cstdio>

const char* runtimeDefinition =
R"(; ACME Assembler
!cpu 6510
!to "%s", cbm
!ct pet

* = $801
!byte $b, $08, $a, 0, $9E, $32, $30, $36, $31, 0, 0, 0 ; basic header
jmp start
)";

void printRuntime(FILE* f, const char* prgPath) {
    fprintf(f, runtimeDefinition, prgPath);
}
