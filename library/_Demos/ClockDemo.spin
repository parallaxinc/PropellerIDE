{{
┌──────────────────────────────────────────────────────────────┐
│ Object File: Clock Demo.spin                                 │
│ Version:     1.01 (for Clock v1.2)                            │
│ Date:        July 16, 2012                                   │
│ Author:      Jeff Martin                                     │
│ Company:     Parallax Semiconductor                          │
│ Email:       jmartin@parallaxsemiconductor.com               │
│ Licensing:   MIT License - see end of file for terms of use. │
└──────────────────────────────────────────────────────────────┘

Description:
This is a demonstration of the Clock object.  It indicates the effects of clock source
changes (speed changes) through the use of eight LEDs on the Propeller Demo Board;
"scrolling" a lit LED back and forth across the eight-LED-display at a clock-dependant rate.
The clock-dependant rate is created by using waitcnt with a fixed count, rather than relying
on a factor of clkfreq (which changes according to clock speed).

When run, the eight-LED-display will flash when the clock source (and thus clock speed) has
changed, and then will scroll the LEDs back and forth to demonstrate the relative speed of
the clock.  The demo starts scrolling LEDs with a clock mode of xtal1+pll1x (5 MHz) then
progressively increases up to xtal1+pll16x (80 MHz), then progressively decreases down to
RCSLOW (≈20 MHz) and repeats the process again.

Notes:
 • At the slowest speed (RCSLOW), the LEDs almost never appear to "scroll," but if you
   watch the demo long enough, you'll sometimes catch it scrolling by 1 LED left/right
   during the RCSLOW phase.
 • It's not recommended to change the clock speed without communicating and "synchronizing"
   that pending event with other cogs which may be affected by it.  For example, if a cog
   is in the middle of a waitcnt with a clkfreq-based delay (a delay of a specific
   "time window" length), when another cog changes the clock frequency, the waiting cog
   will exit the waitcnt too early or too late because the time reference suddenly changed.

Schematic:
The effective circuit of the display used by this demo is represented by the schematic
below (standard Propeller connections assumed).

   ┌───────────┐   
   │ Propeller │
   │  P8X32A   │
   │           │  240 Ω  LEDs      
   │        P16├──────────┐         
   │        P17├──────────┫                    
   │        P18├──────────┫                    
   │        P19├──────────┫  
   │        P20├──────────┫
   │        P21├──────────┫
   │        P22├──────────┫
   │        P23├──────────┫
   │           │              │
   └───────────┘              

{{------------------------------------REVISION HISTORY-------------------------------------
 v1.01 - Updated 05/02/2013 to fix typos in description.
   
}}

CON
  _clkmode   = xtal1 + pll16x                           
  _xinfreq   = 5_000_000                                'Set to standard clock mode and frequency (80 MHz)

  SLED       = 16                                       'Starting LED (for scrolling display)
  ELED       = 23                                       'Ending LED (for scrolling display)
  SDELAY     = 115_000                                  'Scrolling LED delay
  SHIFTCOUNT = ||(ELED-SLED)                            'Scrolling LED shift count

VAR
  long  stack[9]                                        'Stack space for ScrollLeds method
  byte  cmIdx                                           'Clock Mode array index
  
OBJ                         
  clk           : "Clock"
  
PUB Main
{{Launch cog to scroll LEDs right/left and occasionally switch clock sources (indicated by
flash on all LEDs).}}
                                                                                                        
  clk.Init(5_000_000)                                   'Initialize Clock object
  dira[SLED..ELED]~~                                    'Drive LEDs
  cognew(ScrollLeds, @stack)                            'Launch cog to scroll time-dependant LEDs

  repeat                                                'Loop
    clk.SetMode(clockMode[cmIdx++])                     'Switch to new clockmode
    FlashLeds                                           '  Flash LEDs
    clk.PauseSec(3)                                     '  Wait before repeating
    if ~~clockMode[cmIdx] == true                       '  Check clock mode list; reached end?
      cmIdx~                                            '    Reset back to with first entry

PRI ScrollLeds 
{Scroll a single lit LED left/right across display at a clock-dependant speed.}

  clk.PauseMSec(10)                                     'Wait a little before driving LED
  dira[SLED..ELED]~~                                    'Drive LEDs
  outa[ELED]~~                                          'Turn on only last LED

  repeat                                                'Loop forever
    repeat SHIFTCOUNT                                   '  Loop to scroll left 
      waitcnt(SDELAY + cnt)                             '    Wait specific count (clock speed sensitive)
      outa[SLED..ELED] <<= 1                            '    Shift lit LED left
    repeat SHIFTCOUNT                                   '  Loop to scroll right               
      waitcnt(SDELAY + cnt)                             '    Wait specific count (clock speed sensitive)
      outa[SLED..ELED] >>= 1                            '    Shift lit LED right

PRI FlashLeds
{Flash all LEDs briefly.}

  outa[SLED..ELED]~~                                    'All LEDs on
  clk.PauseMSec(125)                                    'Pause
  outa[SLED..ELED]~                                     'LEDs back to normal
                                                        
DAT
{New clock mode table.  The traversal of this table by the code above takes the Propeller's
clock source from the slowest speed through the fastest speed, then back through slowest
speed again.}
   
  clockMode  word  clk#XTAL1_PLL1x                      '=  5_000_000 Hz
             word  clk#XTAL1_PLL2x                      '= 10_000_000 Hz
             word  clk#RCFAST_                          '~ 12_000_000 Hz
             word  clk#XTAL1_PLL4x                      '= 20_000_000 Hz
             word  clk#XTAL1_PLL8x                      '= 40_000_000 Hz
             word  clk#XTAL1_PLL16x                     '= 80_000_000 Hz
             word  clk#XTAL1_PLL8x                      '= 40_000_000 Hz
             word  clk#XTAL1_PLL4x                      '= 20_000_000 Hz
             word  clk#RCFAST_                          '~ 12_000_000 Hz
             word  clk#XTAL1_PLL2x                      '= 10_000_000 Hz
             word  clk#XTAL1_PLL1x                      '=  5_000_000 Hz
             word  clk#RCSLOW_                          '~     20_000 Hz
             word  true                                 '<end of list>
                                                        
{{

┌─────────────────────────────────────────────────────────────────────────────────────────┐
│                                TERMS OF USE: MIT License                                │                                                            
├─────────────────────────────────────────────────────────────────────────────────────────┤
│Permission is hereby granted, free of charge, to any person obtaining a copy of this     │
│software and associated documentation files (the "Software"), to deal in the Software    │
│without restriction, including without limitation the rights to use, copy, modify, merge,│
│publish, distribute, sublicense, and/or sell copies of the Software, and to permit       │
│persons to whom the Software is furnished to do so, subject to the following conditions: │
│                                                                                         │
│The above copyright notice and this permission notice shall be included in all copies or │
│substantial portions of the Software.                                                    │
│                                                                                         │
│THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESSED OR IMPLIED,    │
│INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR │
│PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE│
│FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR     │
│OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER   │
│DEALINGS IN THE SOFTWARE.                                                                │
└─────────────────────────────────────────────────────────────────────────────────────────┘
}}           