''****************************************
''*  Debug_Lcd Test (4x20 LCD) v1.2      *
''*  Authors: Jon Williams, Jeff Martin  *
''*  Copyright (c) 2006 Parallax, Inc.   *
''*  See end of file for terms of use.   *
''***************************************
''
'' v1.2 - March 26, 2008 - Updated by Jeff Martin to use updated Debug_LCD.
'' v1.1 - April 29, 2006 - Updated by Jon Williams for consistency.
''
'' Uses the Parallax 4x20 serial LCD to display a counter in decimal, hex, and binary formats.


CON

  _clkmode = xtal1 + pll16x                             ' use crystal x 16
  _xinfreq = 5_000_000                                  ' 5 MHz cyrstal (sys clock = 80 MHz)

  LCD_PIN   = 0                                         ' for Parallax 4x20 serial LCD on P0
  LCD_BAUD  = 19_200
  LCD_LINES = 4    


OBJ

  lcd : "Debug_Lcd"


PUB main | idx
                                                
  if lcd.init(LCD_PIN, LCD_BAUD, LCD_LINES)             ' start lcd
    lcd.cursor(0)                                       ' cursor off
    lcd.backLight(true)                                 ' backlight on (if available)
    lcd.custom(0, @Bullet)                              ' create custom character 0
    lcd.cls                                             ' clear the lcd
    lcd.str(string("LCD DEBUG", 13))
    lcd.putc(0)                                         ' display custom bullet character
    lcd.str(string(" Dec", 13))
    lcd.putc(0)
    lcd.str(string(" Hex", 13)) 
    lcd.putc(0)
    lcd.str(string(" Bin"))     

    repeat
      repeat idx from 0 to 255
        updateLcd(idx)
        waitcnt(clkfreq / 5 + cnt)                      ' pad with 1/5 sec

      repeat idx from -255 to 0
        updatelcd(idx)
        waitcnt(clkfreq / 5 + cnt)
        

PRI updateLcd(value)

  lcd.gotoxy(12, 1)
  lcd.decf(value, 8)                                    ' print right-justified decimal value
  lcd.gotoxy(11, 2)
  lcd.ihex(value, 8)                                    ' print indicated (with $) hex
  lcd.gotoxy(7, 3)
  lcd.ibin(value, 12)                                   ' print indicated (with %) binary                
                
      
DAT

  Bullet      byte      $00, $04, $0E, $1F, $0E, $04, $00, $00

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