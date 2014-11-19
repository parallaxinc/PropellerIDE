{{
  Welcome.spin
  
  Welcome to the OpenSource PropellerIDE.
  
  To run this program:
  
    * Make sure the FTDI Chip USB driver is installed.
    * Connect the USB Propeller board.
    * Debug (F8)

  The program should pause a moment, print Hello, and
  then toggle Propeller IO P27 repeatedly.
}}

CON

  _clkmode = XTAL1 + PLL16X
  _clkfreq = 80_000_000
  
OBJ
  ser : "Parallax Serial Terminal"

PUB start
  ser.start(115200)
  waitcnt(CLKFREQ/2+CNT)  '' Wait for start up
  ser.str(string($d,"Hello.",$d))
  blink
  
''
'' Blink ActivityBoard LED
''
PUB blink          '' The start method
  DIRA[27] |= 1    '' Set P27 to output
  repeat           '' Repeat forever
    OUTA[27] ^= 1  '' Toggle LED pin
    waitcnt(CLKFREQ/2+CNT)  '' Wait
                            
