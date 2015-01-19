''********************************************
''*  VGA 512x384 2-Color Bitmap Driver v1.0  *
''*  Author: Chip Gracey                     *
''*  Copyright (c) 2006 Parallax, Inc.       *
''*  See end of file for terms of use.       *
''********************************************

CON

  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

  tiles    = vga#xtiles * vga#ytiles
  tiles32  = tiles * 32


OBJ

  vga : "VGA_512x384_Bitmap"


VAR

  long  sync, pixels[tiles32]
  word  colors[tiles]


PUB start | h, i, j, k, x, y

  'start vga
  vga.start(16, @colors, @pixels, @sync)

  'init colors to cyan on blue
  repeat i from 0 to tiles - 1
    colors[i] := $2804

  'draw some lines  
  repeat y from 1 to 8
    repeat x from 0 to 511
      plot(x, x/y)

  'wait ten seconds
  waitcnt(clkfreq * 10 + cnt)

  'randomize the colors and pixels
  repeat
    colors[||?h // tiles] := ?i
    repeat 100
      pixels[||?j // tiles32] := k?


PRI plot(x,y) | i

  if x => 0 and x < 512 and y => 0 and y < 384
    pixels[y << 4 + x >> 5] |= |< x  

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