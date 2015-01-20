{{
*****************************************
* RCTIME background Demo v1.0.b         *
* Author: Beau Schwabe                  *
* Copyright (c) 2010 Parallax           *
* See end of file for terms of use.     *
*****************************************
}}
''
'' Usage:
''
'' background:  (This only needs to be run once for each Potentiometer you have connected)
'' RC.start(Pin,State,VariableAddress )     <<-- Variable will be updated with result and can be read at any time from the program.
''
''
''State = 1 (Preferred) - because this mode provides a slightly higher resolution due to the I/O threshold not being exactly Vdd/2 
''
''         220Ω  C
''I/O Pin ──┳── GND
''             │
''           ┌R
''           │ │
''           └─┻─── GND
''
''
''State = 0 
''
''         220Ω  C
''I/O Pin ──┳── GND
''             │
''           ┌R
''           │ │
''           └─┻─── +3.3V



''In my test setup I used I/O pin #0 and a 10MHz crystal with a PLL setting of x8 
''C = 0.1µF
''
''            approximate returned values from RCtime:
''
''R =  10M      4_380_000
''R = 4.7M      1_980_000
''R = 2.2M        939_000
''R =   1M        425_000
''R = 470K        200_000
''R = 220K         93_000
''R = 180K         76_000
''R = 150K         63_000
''R = 100K         43_000
''R =  58K         23_500
''R =  47K         19_500
''R =  15K          6_100
''R =  10K          4_000
''R = 4.7K          1_800
''R = 3.9K          1_450
''R = 3.3K          1_200
''R =   3K          1_100
''R = 2.7K            960
''R = 2.2K            750
''R = 1.5K            450
''R =   1K            240
''R = 470Ω             26
''R = 330Ω              0

CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000
  _stack = ($3000 + $3000 + 100) >> 2   'accomodate display memory and stack

  x_tiles = 16
  y_tiles = 12

  paramcount = 14       
  bitmap_base = $2000
  display_base = $5000

  lines = 5
  thickness = 2

VAR
  long tv_status    '0/1/2 = off/visible/invisible           read-only
  long  tv_enable     '0/? = off/on                            write-only
  long  tv_pins       '%ppmmm = pins                           write-only
  long  tv_mode       '%ccinp = chroma,interlace,ntsc/pal,swap write-only
  long  tv_screen     'pointer to screen (words)               write-only
  long  tv_colors     'pointer to colors (longs)               write-only               
  long  tv_hc         'horizontal cells                        write-only
  long  tv_vc         'vertical cells                          write-only
  long  tv_hx         'horizontal cell expansion               write-only
  long  tv_vx         'vertical cell expansion                 write-only
  long  tv_ho         'horizontal offset                       write-only
  long  tv_vo         'vertical offset                         write-only
  long  tv_broadcast  'broadcast frequency (Hz)                write-only
  long  tv_auralcog   'aural fm cog                            write-only

  long    colors[64]

  word screen[x_tiles * y_tiles]

  byte  x[lines]
  byte  y[lines]
  byte  xs[lines]
  byte  ys[lines]


  long RCValue
  
OBJ
  tv    : "TV"
  gr    : "Graphics"
  RC    : "RCTIME"
  Num   : "Numbers"

PUB start | i,dx,dy
  'start tv
  longmove(@tv_status, @tvparams, paramcount)
  tv_screen := @screen
  tv_colors := @colors
  tv.start(@tv_status)

  'init colors
  repeat i from 0 to 64
    colors[i] := $00001010 * (i+4) & $F + $2B060C02

  'init tile screen
  repeat dx from 0 to tv_hc - 1
    repeat dy from 0 to tv_vc - 1
      screen[dy * tv_hc + dx] := display_base >> 6 + dy + dx * tv_vc + ((dy & $3F) << 10)

  'start and setup graphics
  gr.start
  gr.setup(16, 12, 128, 96, bitmap_base)


  'Initialize RCValue
  RCValue := 0
  RC.start(0,1,@RCValue )

  repeat

    'clear bitmap
    gr.clear

    'draw text
    gr.textmode(1,1,7,5)
    gr.colorwidth(1,0)
    gr.text(0,90,string("Parallax RCTIME background demo"))

    'display text of RCtime value
    gr.text(0,50,Num.ToStr(RCValue, 10))
    'copy bitmap to display
    gr.copy(display_base)


DAT
tvparams                long    0               'status
                        long    1               'enable
                        
                        'long   %011_0000       'pins      Old Board 
                        long    %001_0101       'pins      New Board
                        
                        long    %0000           'mode
                        long    0               'screen
                        long    0               'colors
                        long    x_tiles         'hc
                        long    y_tiles         'vc
                        long    10              'hx
                        long    1               'vx
                        long    0               'ho
                        long    0               'vo
                        long    60_000_000'_xinfreq<<4 'broadcast
                        long    0               'auralcog

DAT
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