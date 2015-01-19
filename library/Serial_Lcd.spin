''****************************************
''*  Parallax Serial LCD Driver v1.2     *
''*  Authors: Jon Williams, Jeff Martin  *
''*  Copyright (c) 2006 Parallax, Inc.   *
''*  See end of file for terms of use.   *
''****************************************
''
'' Driver for Parallax Serial LCDs (#27976, #27977, #27979)
''
'' v1.2 - March 26, 2008 - Updated by Jeff Martin to conform to Propeller object initialization standards.
'' v1.1 - April 29, 2006 - Updated by Jon Williams for consistency.
''
''
'' Serial LCD Switch Settings for Baud rate
''
''   ┌─────────┐   ┌─────────┐   ┌─────────┐
''   │   O N   │   │   O N   │   │   O N   │
''   │ ┌──┬──┐ │   │ ┌──┬──┐ │   │ ┌──┬──┐ │
''   │ │[]│  │ │   │ │  │[]│ │   │ │[]│[]│ │
''   │ │  │  │ │   │ │  │  │ │   │ │  │  │ │
''   │ │  │[]│ │   │ │[]│  │ │   │ │  │  │ │
''   │ └──┴──┘ │   │ └──┴──┘ │   │ └──┴──┘ │
''   │  1   2  │   │  1   2  │   │  1   2  │
''   └─────────┘   └─────────┘   └─────────┘
''      2400          9600          19200


CON

  LCD_BKSPC     = $08                                   ' move cursor left
  LCD_RT        = $09                                   ' move cursor right
  LCD_LF        = $0A                                   ' move cursor down 1 line
  LCD_CLS       = $0C                                   ' clear LCD (follow with 5 ms delay)
  LCD_CR        = $0D                                   ' move pos 0 of next line
  LCD_BL_ON     = $11                                   ' backlight on
  LCD_BL_OFF    = $12                                   ' backlight off
  LCD_OFF       = $15                                   ' LCD off
  LCD_ON1       = $16                                   ' LCD on; cursor off, blink off
  LCD_ON2       = $17                                   ' LCD on; cursor off, blink on
  LCD_ON3       = $18                                   ' LCD on; cursor on, blink off
  LCD_ON4       = $19                                   ' LCD on; cursor on, blink on
  LCD_LINE0     = $80                                   ' move to line 1, column 0
  LCD_LINE1     = $94                                   ' move to line 2, column 0
  LCD_LINE2     = $A8                                   ' move to line 3, column 0
  LCD_LINE3     = $BC                                   ' move to line 4, column 0

  #$F8, LCD_CC0, LCD_CC1, LCD_CC2, LCD_CC3
  #$FC, LCD_CC4, LCD_CC5, LCD_CC6, LCD_CC7 


VAR

  long  lcdLines, started 


OBJ

  serial : "Simple_Serial"                              ' bit-bang serial driver

  
PUB init(pin, baud, lines): okay

'' Qualifies pin, baud, and lines input
'' -- makes tx pin an output and sets up other values if valid

  started~                                              ' clear started flag
  if lookdown(pin : 0..27)                              ' qualify tx pin 
    if lookdown(baud : 2400, 9600, 19200)               ' qualify baud rate setting
      if lookdown(lines : 2, 4)                         ' qualify lcd size (lines)
        if serial.init(-1, pin, baud)                   ' tx pin only, true mode
          lcdLines := lines                             ' save lines size
          started~~                                     ' mark started flag true

  return started


PUB finalize

'' Finalizes serial object, disable LCD object

  if started
    serial.finalize
    started~                                            ' set to false


PUB putc(txByte) 

'' Transmit a byte

  serial.tx(txByte)
    

PUB str(strAddr)

'' Transmit z-string at strAddr

  serial.str(strAddr)


PUB cls

'' Clears LCD and moves cursor to home (0, 0) position

  if started
    putc(LCD_CLS)
    waitcnt(clkfreq / 200 + cnt)                        ' 5 ms delay 


PUB home

'' Moves cursor to 0, 0

  if started
    putc(LCD_LINE0)
  

PUB gotoxy(col, line) | pos

'' Moves cursor to col/line

  if started
    if lcdLines == 2                                    ' check lcd size
      if lookdown(line : 0..1)                          ' qualify line input
        if lookdown(col : 0..15)                        ' qualify column input
          putc(LinePos[line] + col)                     ' move to target position       
    else
      if lookdown(line : 0..3)
        if lookdown(col : 0..19)
          putc(LinePos[line] + col)                                                  


PUB clrln(line)

'' Clears line

  if started
    if lcdLines == 2                                    ' check lcd size
      if lookdown(line : 0..1)                          ' qualify line input
        putc(LinePos[line])                             ' move to that line         
        repeat 16
          putc(32)                                      ' clear line with spaces
        putc(LinePos[line])                             ' return to start of line    
    else
      if lookdown(line : 0..3)
        putc(LinePos[line])  
        repeat 20
          putc(32)
        putc(LinePos[line])                                                          
  

PUB cursor(type)

'' Selects cursor type
''   0 : cursor off, blink off  
''   1 : cursor off, blink on   
''   2 : cursor on, blink off  
''   3 : cursor on, blink on

  if started
    case type
      0..3 : putc(DispMode[type])                       ' get mode from table
      other : putc(LCD_ON3)                             ' use serial lcd power-up default


PUB displayOff

'' Blank the display (without clearing)

  if started
    putc(LCD_OFF) 


PUB displayOn

'' Restore the display (with cursor hidden)

  if started
    cursor(0)    
    
      
PUB custom(char, chrDataAddr)

'' Installs custom character map
'' -- chrDataAddr is address of 8-byte character definition array

  if started
    if lookdown(char : 0..7)                            ' make sure char in range
      putc(LCD_CC0 + char)                              ' write character code
      repeat 8
        putc(byte[chrDataAddr++])                       ' write character data


PUB backLight(status)

'' Enable (true) or disable (false) LCD backlight
'' -- works only with backlight-enabled displays

  if started
    status := status <> 0                               ' promote non-zero to -1 
    if status
      putc(LCD_BL_ON)
    else
      putc(LCD_BL_OFF)
  else
    status := false

  return status  
  

DAT

  LinePos     byte      LCD_LINE0, LCD_LINE1, LCD_LINE2, LCD_LINE3
  DispMode    byte      LCD_ON1, LCD_ON2, LCD_ON3, LCD_ON4
  
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