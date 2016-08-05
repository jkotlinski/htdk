\ From: John Hayes S1I
\ Subject: tester.fr
\ Date: Mon, 27 Nov 95 13:10:09 PST

\ (C) 1995 JOHNS HOPKINS UNIVERSITY / APPLIED PHYSICS LABORATORY
\ MAY BE DISTRIBUTED FREELY AS LONG AS THIS COPYRIGHT NOTICE REMAINS.
\ VERSION 1.2

\ 24/11/2015 Replaced Core Ext word <> with = 0=
\ 31/3/2015 Variable #ERRORS added and incremented for each error reported.
\ 22/1/09 The words { and } have been changed to T{ and }T respectively to
\ agree with the Forth 200X file ttester.fs. This avoids clashes with
\ locals using { ... } and the FSL use of }

HEX

\ SET THE FOLLOWING FLAG TO TRUE FOR MORE VERBOSE OUTPUT; THIS MAY
\ ALLOW YOU TO TELL WHICH TEST CAUSED YOUR SYSTEM TO HANG.
VARIABLE VERBOSE
   0 VERBOSE !

: EMPTY-STACK   \ ( ... -- ) EMPTY STACK: HANDLES UNDERFLOWED STACK TOO.
   DEPTH ?DUP IF DUP 0< IF NEGATE 0 DO 0 LOOP ELSE 0 DO DROP LOOP THEN THEN ;

VARIABLE #ERRORS 0 #ERRORS !

: ERROR      \ ( C-ADDR U -- ) DISPLAY AN ERROR MESSAGE
testing QUIT ;

VARIABLE ACTUAL-DEPTH         \ STACK RECORD
CREATE ACTUAL-RESULTS 20 CELLS ALLOT

: T{      \ ( -- ) SYNTACTIC SUGAR.
   ;

: ->      \ ( ... -- ) RECORD DEPTH AND CONTENT OF STACK.
   DEPTH DUP ACTUAL-DEPTH !      \ RECORD DEPTH
   ?DUP IF            \ IF THERE IS SOMETHING ON STACK
      0 DO ACTUAL-RESULTS I CELLS + ! LOOP \ SAVE THEM
   THEN ;

: }T      \ ( ... -- ) COMPARE STACK (EXPECTED) CONTENTS WITH SAVED
      \ (ACTUAL) CONTENTS.
   DEPTH ACTUAL-DEPTH @ = IF      \ IF DEPTHS MATCH
      DEPTH ?DUP IF         \ IF THERE IS SOMETHING ON THE STACK
         0  DO            \ FOR EACH STACK ITEM
           ACTUAL-RESULTS I CELLS + @   \ COMPARE ACTUAL WITH EXPECTED
           = 0= IF S" INCORRECT RESULT" ERROR LEAVE THEN
         LOOP
      THEN
   ELSE               \ DEPTH MISMATCH
      S" WRONG NUMBER OF RESULTS" ERROR
   THEN ;

: testing type cr ;

\ ====== Imported from core test suite

: BITSSET? IF 0 0 ELSE 0 THEN ;
: test-basic-assumptions
s" BASIC ASSUMPTIONS" testing
T{ -> }T               \ START WITH CLEAN SLATE
\ ( TEST IF ANY BITS ARE SET; ANSWER IN BASE 1 )
T{  0 BITSSET? -> 0 }T      ( ZERO IS ALL BITS CLEAR )
T{  1 BITSSET? -> 0 0 }T      ( OTHER NUMBER HAVE AT LEAST ONE BIT )
T{ -1 BITSSET? -> 0 0 }T ;

0    CONSTANT 0S
0 INVERT CONSTANT 1S

: test-booleans
s" BOOLEANS: INVERT AND OR XOR" testing
T{ 0 0 AND -> 0 }T
T{ 0 1 AND -> 0 }T
T{ 1 0 AND -> 0 }T
T{ 1 1 AND -> 1 }T

T{ 0 INVERT 1 AND -> 1 }T
T{ 1 INVERT 1 AND -> 0 }T

T{ 0S INVERT -> 1S }T
T{ 1S INVERT -> 0S }T

T{ 0S 0S AND -> 0S }T
T{ 0S 1S AND -> 0S }T
T{ 1S 0S AND -> 0S }T
T{ 1S 1S AND -> 1S }T

T{ 0S 0S OR -> 0S }T
T{ 0S 1S OR -> 1S }T
T{ 1S 0S OR -> 1S }T
T{ 1S 1S OR -> 1S }T

T{ 0S 0S XOR -> 0S }T
T{ 0S 1S XOR -> 1S }T
T{ 1S 0S XOR -> 1S }T
T{ 1S 1S XOR -> 0S }T ;

8000 CONSTANT MSB
: test-shift
s" 2* 2/ LSHIFT RSHIFT" testing

( WE TRUST 1S, INVERT, AND BITSSET?; WE WILL CONFIRM RSHIFT LATER )
T{ MSB BITSSET? -> 0 0 }T

T{ 0S 2* -> 0S }T
T{ 1 2* -> 2 }T
T{ 4000 2* -> 8000 }T
T{ 1S 2* 1 XOR -> 1S }T
T{ MSB 2* -> 0S }T

T{ 0S 2/ -> 0S }T
T{ 1 2/ -> 0 }T
T{ 4000 2/ -> 2000 }T
T{ 1S 2/ -> 1S }T            \ MSB PROPOGATED
T{ 1S 1 XOR 2/ -> 1S }T
T{ MSB 2/ MSB AND -> MSB }T

T{ 1 0 LSHIFT -> 1 }T
T{ 1 1 LSHIFT -> 2 }T
T{ 1 2 LSHIFT -> 4 }T
T{ 1 F LSHIFT -> 8000 }T         \ BIGGEST GUARANTEED SHIFT
T{ 1S 1 LSHIFT 1 XOR -> 1S }T
T{ MSB 1 LSHIFT -> 0 }T

T{ 1 0 RSHIFT -> 1 }T
T{ 1 1 RSHIFT -> 0 }T
T{ 2 1 RSHIFT -> 1 }T
T{ 4 2 RSHIFT -> 1 }T
T{ 8000 F RSHIFT -> 1 }T         \ BIGGEST
T{ MSB 1 RSHIFT MSB AND -> 0 }T      \ RSHIFT ZERO FILLS MSBS
T{ MSB 1 RSHIFT 2* -> MSB }T ;

: start
$16 $d018 c! \ upper/lowercase
test-basic-assumptions
test-booleans
test-shift
cr s" OK" testing ;
