# Hack n' Trade Development Kit

Cross-compiled 6502 Forth, using [ACME assembler](https://sourceforge.net/projects/acme-crossass/). Non-standard, but matches [Forth 2012](http://forth-standard.org/standard/words) when possible.

Usage: `./htfc code.fs ; acme code.asm`

### Cross-compiled (target) words

 * ;
 * 1+ 2+
 * begin again
 * c! c@
 * rot

### Interpreted (host) words

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
