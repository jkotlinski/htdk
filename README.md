# Hack n' Trade Development Kit

Cross-compiled 6502 Forth, using [ACME assembler](https://sourceforge.net/projects/acme-crossass/). Non-standard, but matches [Forth 2012](http://forth-standard.org/standard/words) when possible.

Usage: `./htfc code.fs ; acme code.asm`

The resulting `code.prg` runs the user-defined word named `start`.

### Words

#### Compiling

: ; :code ;code value hex

#### Arithmetic

1+ 2+ 1- + -

#### Logic

= 0= and

#### Memory

! @ c! c@ fill

#### Stack manipulation

drop rot swap over dup 2dup

#### Control structures

begin again if else then

### Number format

 * 1234 - decimal
 * #1234 - decimal
 * $d020 - hexadecimal

### Assembler

 * MSB = top of MSB parameter stack
 * LSB = top of LSB parameter stack
 * W = zero-page work area cell

Code words are defined using ACME assembler:

    :code incbg
    -   inc $d020
        jmp - ;code
