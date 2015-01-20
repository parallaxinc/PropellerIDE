{{
┌───────────────────────────────────────────┬────────────────┬───────────────────────────────────┬───────────────┐
│ Real Random v1.2                          │ by Chip Gracey │ Copyright (c) 2007 Parallax, Inc. │ 23 March 2007 │
├───────────────────────────────────────────┴────────────────┴───────────────────────────────────┴───────────────┤
│                                                                                                                │
│ This object generates real random numbers by stimulating and tracking CTR PLL jitter. It requires one cog and  │
│ at least 20MHz.                                                                                                │
│                                                                                                                │
├────────────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│ Background and Detail:                                                                                         │
│                                                                                                                │
│ A real random number is impossible to generate within a closed digital system. This is because there are no    │
│ reliably-random states within such a system at power-up, and after power-up, it behaves deterministically.     │
│ Random values can only be 'earned' by measuring something outside of the digital system.                       │                                                                                     
│                                                                                                                │
│ In your programming, you might have used 'var?' to generate a pseudo-random sequence, but found the same       │
│ pattern playing every time you ran your program. You might have then used 'cnt' to 'randomly' seed the 'var'.  │
│ As long as you kept downloading to RAM, you saw consistently 'random' results. At some point, you probably     │
│ downloaded to EEPROM to set your project free. But what happened nearly every time you powered it up? You were │
│ probably dismayed to discover the same sequence playing each time! The problem was that 'cnt' was always       │
│ powering-up with the same initial value and you were then sampling it at a constant offset. This can make you  │
│ wonder, "Where's the end to this madness? And will I ever find true randomness?".                              │                                                              
│                                                                                                                │
│ In order to have real random numbers, either some external random signal must be input, or some analog system  │
│ must be used to generate random noise which can be measured. We're in luck here, because it turns out that the │
│ Propeller does have sufficiently-analog subsystems which can be exploited for this purpose -- each cog's CTR   │
│ PLLs. These can be exercised internally to good effect, without any I/O activity.                              │                                                                   
│                                                                                                                │
│ This object sets up a cog's CTRA PLL to run at the main clock's frequency. It then uses a pseudo-random        │
│ sequencer to modulate the PLL's target phase. The PLL responds by speeding up and slowing down in a an endless │
│ effort to lock. This results in very unpredictable frequency jitter which is fed back into the sequencer to    │
│ keep the bit salad tossing. The final output is a truly-random 32-bit unbiased value that is fully updated     │
│ every ~100us, with new bits rotated in every ~3us. This value can be sampled by your application whenever a    │ 
│ random number is needed.                                                                                       │
│                                                                                                                │
├────────────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│ Revision History                                                                   v1.0 released 21 March 2007 │
│                                                                                                                │ 
│ v1.1  Bias removal has been added to ensure true randomness. Released 22 March 2007.                           │
│ v1.2  Assembly code made more efficient. Documentation improved. Released 23 March 2007.                       │
│                                                                                                                │
└────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘

}}
VAR

  long  cog, random_value


PUB start : okay

'' Start real random driver - starts a cog
'' returns false if no cog available

  'Reset driver
  stop                                                    

  'Launch real random cog
  return cog := cognew(@entry, @random_value) + 1

  'allow 5ms to launch and randomize
  waitcnt(clkfreq / 200 + cnt)


PUB stop

'' Stop real random driver - frees a cog

  'If already running, stop real random cog
  if cog
    cogstop(cog~ -  1)


PUB random : value

'' Returns a new long random value

  'wait to insure new random (~200us at 80MHz system clock)
  waitcnt($4000 + cnt)
  
  return random_value


PUB random_ptr : ptr

'' Returns the address of the long which receives the random value
''
'' A random bit is rotated into the long every ~3us, resuling in a
'' new long every ~100us, on average, at 80MHz. You may want to double
'' these times, though, to be sure that you are getting new bits. The
'' timing uncertainty comes from the unbiasing algorithm which throws
'' away identical bit pairs, and only outputs the different ones.

  return @random_value


DAT

' ┌─────────────────────────┐
' │  Real Random Generator  │
' └─────────────────────────┘

                        org

entry                   movi    ctra,#%00001_111        'set ctra to internal pll mode, select x16 tap
                        movi    frqa,#$020              'set frqa to system clock frequency / 16
                        movi    vcfg,#$040              'set vcfg to discrete output, but without pins
                        mov     vscl,#70                'set vscl to 70 pixel clocks per waitvid

:twobits                waitvid 0,0                     'wait for next 70-pixel mark ± jitter time
                        test    phsa,#%10111    wc      'pseudo-randomly sequence phase to induce jitter
                        rcr     phsa,#1                 '(c holds random bit #1)
                        add     phsa,cnt                'mix PLL jitter back into phase 

                        rcl     par,#1          wz, nr  'transfer c into nz (par shadow register = 0)
                        wrlong  _random_value,par       'write random value back to spin variable

                        waitvid 0,0                     'wait for next 70-pixel mark ± jitter time           
                        test    phsa,#%10111    wc      'pseudo-randomly sequence phase to induce jitter        
                        rcr     phsa,#1                 '(c holds random bit #2)                                                        
                        add     phsa,cnt                'mix PLL jitter back into phase                    

        if_z_eq_c       rcl     _random_value,#1        'only allow different bits (removes bias)
                        jmp     #:twobits               'get next two bits


_random_value           res     1

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