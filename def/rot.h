#pragma once

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
