# Hack n' Trade Development Kit

Cross-compiled 6502 Forth, using [ACME assembler](https://sourceforge.net/projects/acme-crossass/).

Usage: `./htfc code.fs ; acme code.asm`

htfc is non-standard, but words follow the [Forth 2012](http://forth-standard.org/standard/words) definitions when possible.

### Cross-compiled (target) words

Following words are available within colon definitions:

 * ;
 * 1+ 2+
 * begin again
 * c! c@
 * rot

### Interpreted (host) words

Following words are available outside colon definitions:

 * :
 * :code ;code
 * value

### Number format

 * 1234 - decimal
 * #1234 - decimal
 * $d020 - hexadecimal

### Assembler

Code words are defined using ACME assembler:

    :code incbg
    -   inc $d020
        jmp - ;code
