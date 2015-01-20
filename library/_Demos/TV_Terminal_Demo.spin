''***************************************
''*  TV Terminal Demo v1.0.1            *
''*  Author: Chip Gracey                *
''*  Copyright (c) 2004 Parallax, Inc.  *
''*  See end of file for terms of use.  *
''***************************************

{-----------------REVISION HISTORY-----------------
 v1.0.1 - Updated 5/15/2006 to use TV_Terminal v1.1}
 
CON

        _clkmode        = xtal1 + pll16x
        _xinfreq        = 5_000_000

VAR

        long    testerror

        long    vlong
        word    vword
        byte    vbyte

OBJ

        term    : "TV_Terminal"


PUB start | i

  'start the tv terminal
  term.start(12)

  'print a string
  term.str(@title)

  'change to green
  term.out(2)

  'print some small decimal numbers
  repeat i from -6 to 6
    term.dec(i)
    term.out(" ")
  term.out(13)

  'print the extreme decimal numbers
  term.dec($7FFFFFFF)
  term.out(9)
  term.dec($80000001)
  term.out(13)

  'change to red
  term.out(3)

  'print some hex numbers
  repeat i from -6 to 6
    term.hex(i, 2)
    term.out(" ")
  term.out(13)

  'print some binary numbers
  repeat i from 0 to 7
    term.bin(i, 3)
    term.out(" ")


DAT

title   byte    "TV Terminal Demo",13,13,0

{{

┌──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                                   TERMS OF USE: MIT License                                                  │                                                            
├──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation    │ 
│files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,    │
│modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software│
│is furnished to do so, subject to the following conditions:                                                                   │
│                                                                                                                              │
│The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.│
│                                                                                                                              │
│THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE          │
│WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR         │
│COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,   │
│ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                         │
└──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
}}