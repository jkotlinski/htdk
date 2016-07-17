const char* rot =
"    ldy MSB+2, x\n"
"    lda MSB+1, x\n"
"    sta MSB+2, x\n"
"    lda MSB, x\n"
"    sta MSB+1, x\n"
"    sty MSB, x\n"
"    ldy LSB+2, x\n"
"    lda LSB+1, x\n"
"    sta LSB+2, x\n"
"    lda LSB, x\n"
"    sta LSB+1, x\n"
"    sty LSB, x\n"
"    rts\n";
