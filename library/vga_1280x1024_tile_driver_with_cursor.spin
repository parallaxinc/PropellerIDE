{{
┌────────────────────────────────────────┬────────────────┬───────────────────────────────────┬──────────────────┐
│ VGA 1280x1024 Tile Driver v0.9         │ by Chip Gracey │ Copyright (c) 2006 Parallax, Inc. │ 22 November 2006 │
├────────────────────────────────────────┴────────────────┴───────────────────────────────────┴──────────────────┤
│                                                                                                                │
│ This object generates a 1280x1024 VGA display from a 80x64 array of 16x16-pixel 4-color tiles.  It requires    │
│ three cogs (or four with optional cursor enabled) and at least 80 MHz.                                         │
│                                                                                                                │
└────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
                        
}}
CON

' 1280 x 1024 @ 60Hz settings

  hp = 1280                     'horizontal pixels
  vp = 1024                     'vertical pixels
  hf = 48                       'horizontal front porch pixels
  hs = 112                      'horizontal sync pixels
  hb = 248                      'horizontal back porch pixels
  vf = 1                        'vertical front porch lines
  vs = 3                        'vertical sync lines
  vb = 38                       'vertical back porch lines
  pr = 100                      'pixel rate in MHz at 80MHz system clock (5MHz granularity)

  ht = hp + hf + hs + hb        'total scan line pixels

' Tile array

  xtiles = hp / 16
  ytiles = vp / 16


VAR

  long cog[4]
  
  long dira_                    '9 contiguous longs
  long dirb_
  long vcfg_
  long cnt_
  long array_ptr_
  long color_ptr_
  long cursor_ptr_
  long sync_ptr_
  long mode_ 
  

PUB start(base_pin, array_ptr, color_ptr, cursor_ptr, sync_ptr, mode) : okay | i, j

'' Start driver - starts two or three cogs
'' returns false if cogs not available
'' 
''     base_pin = First of eight VGA pins, must be a multiple of eight (0, 8, 16, 24, etc):
''                                           
''                    240Ω               240Ω                 240Ω                240Ω                  
''                +7 ───┳─ Red   +5 ───┳─ Green   +3 ───┳─ Blue   +1 ── H        
''                    470Ω │             470Ω │               470Ω │              240Ω                  
''                +6 ───┘         +4 ───┘           +2 ───┘          +0 ── V         
''  
''    array_ptr = Pointer to 5,120 long-aligned words, organized as 80 across by 64 down,
''                which will serve as the tile array. Each word specifies a tile bitmap and
''                a color palette for its tile area. The bottom 10 bits of each word hold
''                the base address of a 16-long tile bitmap, while the top 6 bits select a
''                color palette for the bitmap. For example, $B2E5 would specify the tile
''                bitmap spanning $B940..$B97F ($2E5<<6) and color palette $2C ($B2E5>>10).
''  
''    color_ptr = Pointer to 64 longs which will define the 64 color palettes. The RGB data
''                in each long is arranged as %%RGBx_RGBx_RGBx_RGBx with the sub-bytes 3..0
''                providing the color data for pixel values %11..%00, respectively:
''  
''                %%3330_0110_0020_3300: %11=white, %10=dark cyan, %01=blue, %00=gold
''  
''   cursor_ptr = Pointer to 4 longs which will control the cursor, or 0 to disable the
''                cursor. If a pointer is given, an extra cog will be started to generate
''                the cursor overlay. Here are the 4 longs that control the cursor:
'' 
''                cursor_x      - X position of cursor: ..0..1279.. (left to right)
''                cursor_y      - Y position of cursor: ..0..1023.. (bottom to top)
''
''                cursor_color  - Cursor color to be OR'd to background color as %%RGBx:
''                                %%3330=white, %%2220 or %%1110=translucent, %%0000=off
''
''                cursor_shape  - 0 for arrow, 1 for crosshair, or pointer to a cursor
''                                definition. A cursor definition consists of 32 longs
''                                containing a 32x32 pixel cursor image, followed by two
''                                bytes which define the X and Y center-pixel offsets
''                                within the image.
''
''     sync_ptr = Pointer to a long which will be set to -1 after each refresh, or 0 to
''                disable this function. This is useful in advanced applications where
''                awareness of display timing is important.
''  
''         mode = 0 for normal 16x16 pixel tiles or 1 for taller 16x32 pixel tiles. Mode 1
''                is useful for displaying the internal font while requiring half the array
''                memory; however, the 3-D bevel characters will not be usable because of
''                the larger vertical tile granularity of this mode.
                                                                                                   
  'If driver is already running, stop it
  stop

  'Ready i/o settings
  i := $FF << (base_pin & %011000)
  j := base_pin & %100000 == 0
  dira_ := i & j
  dirb_ := i & !j
  vcfg_ := $300000FF + (base_pin & %111000) << 6

  'Ready cnt value to sync cogs by
  cnt_ := cnt + $100000

  'Ready pointers and mode
  longmove(@array_ptr_, @array_ptr, 5)

  'Launch cogs, abort if error
  j := mode_
  repeat i from 0 to 3
    mode_ := j << 2 + i
    if i == 3                   'cursor cog?
      ifnot cursor_ptr          'cursor enabled?
        quit                    'if not, quit loop
      vcfg_ ^= $10000000        'else, set two-color mode
    ifnot cog[i] := cognew(@entry, @dira_) + 1
      stop
      return {false}
    waitcnt($8000 + cnt)        'allow cog to launch before modifying variables

  'Successful
  return true


PUB stop | i

'' Stop driver - frees cogs

  'If already running, stop any VGA cogs
  repeat i from 0 to 3
    if cog[i]
      cogstop(cog[i]~ - 1)              


DAT

' ┌─────────────────────────────┐
' │  Initialization - all cogs  │
' └─────────────────────────────┘

                        org

' Move field loop into position

entry                   mov     $1EF,$1EF - field + field_begin                                    
                        sub     entry,d0s0_             '(reverse move to avoid overwrite)             
                        djnz    regs,#entry

' Build line display code

                        mov     par,#xtiles
:wv                     mov     linecode+0,linecode_wv
                        add     :wv,d1
                        add     linecode_wv,#1
                        cmp     par,#1          wz
:sc     if_nz           mov     linecode+1,linecode_sc
        if_nz           add     :sc,d1
        if_nz           add     linecode_sc,d1
                        djnz    par,#:wv

' Acquire settings
                                                        'dira_        ─  dira
                        mov     regs,par                'dirb_        ─  dirb
:next                   movd    :read,sprs              'vcfg_        ─  vcfg
                        or      :read,d8_d4             'cnt_         ─  cnt
                        shr     sprs,#4                 'array_ptr_   ─  ctrb
:read                   rdlong  dira,regs               'color_ptr_   ─  frqb
                        add     regs,#4                 'cursor_ptr_  ─  vscl
                        tjnz    sprs,#:next             'sync_ptr_    ─  phsb
                        rdlong  regs,regs               'mode_        ─  regs

                        test    regs,#%100      wc      'if mode 1, set tile size to 16 x 32 pixels
        if_c            movs    tile_bytes,#32 * 4
        if_c            shr     array_bytes,#1
                                                        'adjust      cog index
                        test    regs,#%10       wc      'settings    0   1   2
                        test    regs,#%01       wz      '---------------------
        if_nc           add     build,#1                'build      +1  +1  +0                    
        if_nc_and_z     add     vf_lines,#2                                                           
        if_c_and_z      add     vf_lines,#4             'vf_lines   +2  +0  +4                                                    
        if_nc_and_z     sub     vb_lines,#4                                                                                          
        if_nc_and_nz    sub     vb_lines,#2             'vb_lines   -4  -2  -0                                                    
        if_nc_and_nz    movs    start_line,#2 * 4                                        
        if_c_and_z      movs    start_line,#4 * 4       'start_line +0  +2  +4
        
        if_c_or_z       mov     snop,#0                 'sync        N   Y   N                   

                        mov     regs,vscl               'save cursor pointer
                                                                                
' Synchronize all cogs' video circuits so that waitvid's will be pixel-locked
                                                                                              
                        movi    frqa,#(pr / 5) << 1     'set pixel rate (VCO runs at 1x)                     
                        mov     vscl,#1                 'set video shifter to reload on every pixel
                        waitcnt cnt,d8_d4               'wait for sync count, add ~3ms - cogs locked!
                        movi    ctra,#%00001_111        'enable PLLs now - NCOs locked!
                        waitcnt cnt,#0                  'wait ~3ms for PLLs to stabilize - PLLs locked!
                        mov     vscl,#100               'subsequent WAITVIDs will now be pixel-locked!

' Determine if this cog is to perform one of two field functions or the cursor function

        if_nc_or_z      jmp     #vsync                  'if cog index 0..2, jump to field function
                                                        'else, cursor function follows

' ┌─────────────────────────┐
' │  Cursor Loop - one cog  │
' └─────────────────────────┘

' Do vertical sync lines minus two

cursor                  mov     par,#vf + vs + vb - 5

:loop                   mov     vscl,vscl_line
:vsync                  waitvid ccolor,#0
                        djnz    par,#:vsync

' Do two lines minus horizontal back porch pixels to buy a big block of time

                        mov     vscl,vscl_two_lines_mhb
                        waitvid ccolor,#0   

' Get cursor data

                        rdlong  cx,regs                 'get cursor x
                        add     regs,#4
                        rdlong  cy,regs                 'get cursor y
                        add     regs,#4
                        rdlong  ccolor,regs             'get cursor color
                        add     regs,#4
                        rdlong  cshape,regs             'get cursor shape
                        sub     regs,#3 * 4

                        and     ccolor,#$FC             'trim and justify cursor color
                        shl     ccolor,#8

' Build cursor pixels

                        mov     par,#32                 'ready for 32 cursor segments
                        movd    :pix,#cpix
                        mov     cnt,cshape
                        
:pixloop                cmp     cnt,#1          wc, wz  'arrow, crosshair, or custom cursor?
        if_a            jmp     #:custom
        if_e            jmp     #:crosshair
        
                        cmp     par,#32         wz      'arrow
                        cmp     par,#32-21      wc      
        if_z            mov     cseg,h80000000
        if_nz_and_nc    sar     cseg,#1
        if_nz_and_c     shl     cseg,#2
                        mov     coff,#0
                        jmp     #:pix
                        
:crosshair              cmp     par,#32-15      wz      'crosshair
        if_ne           mov     cseg,h00010000
        if_e            neg     cseg,#2
                        cmp     par,#1          wz
        if_e            mov     cseg,#0
                        mov     coff,h00000F0F
                        jmp     #:pix
                        
:custom                 rdlong  cseg,cshape             'custom
                        add     cshape,#4
                        rdlong  coff,cshape
                        
:pix                    mov     cpix,cseg               'save segment into pixels
                        add     :pix,d0
                        
                        djnz    par,#:pixloop           'another segment?

' Compute cursor position

                        mov     cseg,coff               'apply cursor center-pixel offsets
                        and     cseg,#$FF
                        sub     cx,cseg
                        shr     coff,#8
                        and     coff,#$FF
                        add     cy,coff

                        cmps    cx,neg31        wc      'if x out of range, hide cursor via y
        if_nc           cmps    pixels_m1,cx    wc
        if_c            neg     cy,#1

                        mov     cshr,#0                 'adjust for left-edge clipping               
                        cmps    cx,#0           wc
        if_c            neg     cshr,cx
        if_c            mov     cx,#0
        
                        mov     cshl,#0                 'adjust for right-edge clipping
                        cmpsub  cx,pixels_m32   wc
        if_c            mov     cshl,cx
        if_c            mov     cx,pixels_m32

                        add     cx,#hb                  'bias x and y for display
                        sub     cy,lines_m1

' Do visible lines with cursor

                        mov     par,lines               'ready for visible scan lines

:line                   andn    cy,#$1F         wz, nr  'check if scan line in cursor range

        if_z            movs    :seg,cy                 'if in range, get cursor pixels
        if_z            add     :seg,#cpix
        if_nz           mov     cseg,#0                 'if out of range, use blank pixels
:seg    if_z            mov     cseg,cpix
        if_z            rev     cseg,#0                 'reverse pixels so they map sensibly
        if_z            shr     cseg,cshr               'perform any edge clipping on pixels
        if_z            shl     cseg,cshl

                        mov     vscl,cx                 'do left blank pixels (hb+cx)
                        waitvid ccolor,#0
                        
                        mov     vscl,vscl_cursor        'do cursor pixels (32)
                        waitvid ccolor,cseg
                        
                        mov     vscl,vscl_line_m32      'do right blank pixels (hp+hf+hs-32-cx)
                        sub     vscl,cx
                        waitvid ccolor,#0

                        add     cy,#1                   'another scan line?
                        djnz    par,#:line

' Do horizontal back porch pixels and loop

                        mov     vscl,#hb
                        waitvid ccolor,#0

                        mov     par,#vf + vs + vb - 2   'ready to do vertical sync lines
                        jmp     #:loop

' Cursor data

vscl_line               long    ht                      'total pixels per scan line
vscl_two_lines_mhb      long    ht * 2 - hb             'total pixels per two scan lines minus hb
vscl_line_m32           long    ht - 32                 'total pixels per scan line minus 32
vscl_cursor             long    1 << 12 + 32            '32 pixels per cursor with 1 clock per pixel
lines                   long    vp                      'visible scan lines
lines_m1                long    vp - 1                  'visible scan lines minus 1
pixels_m1               long    hp - 1                  'visible pixels minus 1
pixels_m32              long    hp - 32                 'visible pixels minus 32
neg31                   long    -31
                    
h80000000               long    $80000000               'arrow/crosshair cursor data
h00010000               long    $00010000
h00000F0F               long    $00000F0F

' Initialization data

d0s0_                   long    1 << 9 + 1              'd and s field increments
regs                    long    $1F0 - field            'number of registers in field loop space
sprs                    long    $DFB91E76               'phsb/vscl/frqb/ctrb/cnt/vcfg/dirb/dira nibbles
bit15                   long    $8000                   'bit15 mask used to differentiate cogs in par
d8_d4                   long    $0003E000               'bit8..bit4 mask for d field

field_begin                                             'field code begins at this offset

' Undefined cursor data

cx                      res     1
cy                      res     1
ccolor                  res     1
cshape                  res     1
coff                    res     1
cseg                    res     1
cshr                    res     1
cshl                    res     1
cpix                    res     32


' ┌───────────────────────────┐
' │  Field Loop - three cogs  │
' └───────────────────────────┘

                        org

' Allocate buffers

palettes                res     64                      'palettes of colors

pixels0                 res     xtiles                  'pixels for tile row line +0
pixels1                 res     xtiles                  'pixels for tile row line +1

linecode                res     xtiles * 2 - 1          'line display code
field                   mov     vscl,#hf
                        waitvid hv_sync,#0
linecode_sc             sumc    linecode,#xtiles        '(winds up as 'sumc field-1,#xtiles')
                        mov     ina,#1
                        jmp     #hsync

linecode_wv             waitvid palettes,pixels0

' Each cog alternately builds and displays two scan lines
                          
build                   mov     cnt,#vp / 6             'ready number of two-scan-line builds/displays
start_line              mov     tile_line,#0            'ready starting tile line (adjusted)
                        
' Build two scan lines during four scan lines

build_2y                movd    col,#linecode           'reset pointers for scan line buffers 
                        movd    pix0,#pixels0          
                        movd    pix1,#pixels1          

                        mov     ina,#2                  'two scan lines require two waitvid's

build_40x               mov     vscl,vscl_two_lines     'output lows for two scan lines so other cog
:zero                   waitvid :zero,#0                '..can display while this cog builds (twice)

                        mov     inb,#xtiles / 2         'build four scan lines for half a row

build_1x                rdword  vscl,ctrb               'get word from the tile array
                        add     ctrb,#2                  
                        ror     vscl,#10                'get color bits
col                     movd    linecode,vscl
                        add     col,d1
                        shr     vscl,#16                'get tile line address
                        add     vscl,tile_line                                   
pix0                    rdlong  pixels0,vscl            'get line +0 tile pixels
                        add     pix0,d0                                  
                        add     vscl,#4                                  
pix1                    rdlong  pixels1,vscl            'get line +1 tile pixels                 
                        add     pix1,d0                                  
                        djnz    inb,#build_1x           'loop for next tile (16 inst/loop)

                        djnz    ina,#build_40x          'if first half done, loop for 2nd waitvid
                        
                        sub     ctrb,#xtiles * 2        'back up to start of same row

' Display two scan lines

                        mov     inb,#2                  'ready for two scan lines
                        movs    hsync_ret,#linecode_ret

display                 test    inb,#1          wc
                        mov     vscl,vscl_tile          'set pixel rate for tiles
                        jmp     #linecode
linecode_ret            djnz    inb,#display            'another scan line?

' Another two scan lines?
                             
                        add     tile_line,#6 * 4        'advance six scan lines within tile row
tile_bytes              cmpsub  tile_line,#16 * 4 wc    'tile row done? (# doubled for mode 1)
        if_c            add     ctrb,#xtiles * 2        'if done, advance array pointer to next row

                        djnz    cnt,#build_2y           'another two scan lines?

                        sub     ctrb,array_bytes        'display done, reset array pointer to top row                  

' Visible section done, handle sync indicator

                        cmp     cnt,phsb        wz      'sync enabled? (cnt=0)
snop    if_nz           wrlong  neg1,phsb               'if so, write -1 to sync indicator (change to nop unless #3)

' Do vertical sync lines and loop
                        
vf_lines                mov     ina,#vf                 'do vertical front porch lines (adjusted +4|+2|+0)
                        call    #blank

vsync                   mov     ina,#vs                 'do vertical sync lines
                        call    #blank_vsync

vb_lines                mov     ina,#vb                 'do vertical back porch lines (adjusted -4|-2|-0)
                        movs    blank_vsync_ret,#build  '(loop to build, blank_vsync follows)

' Subroutine - do blank lines

blank_vsync             xor     hv_sync,#$0101          'flip vertical sync bits

blank                   mov     vscl,vscl_blank         'do horizontal blank pixels
                        waitvid hv_sync,#0

                        mov     vscl,#hf                'do horizontal front porch pixels
                        waitvid hv_sync,#0

hsync                   mov     vscl,#hs                'do horizontal sync pixels
                        waitvid hv_sync,#1

                        rdlong  vscl,frqb               'update another palette
                        and     vscl,color_mask
:palette                mov     palettes,vscl
                        add     :palette,d0
                        add     frqb,#4
                        add     par,count_64    wc      
        if_c            movd    :palette,#palettes
        if_c            sub     frqb,#64 * 4

                        mov     vscl,#hb                'do horizontal back porch pixels
                        waitvid hv_sync,#0
                        
                        djnz    ina,#blank              'another blank line?
hsync_ret           
blank_ret
blank_vsync_ret         ret

' Field data

d0s0                    long    1 << 9 + 1              'd and s field increments
d0                      long    1 << 9                  'd field increment
d1                      long    1 << 10                 'd field double increment

array_bytes             long    xtiles * ytiles * 2     'number of bytes in tile array

vscl_two_lines          long    ht * 2                  'total pixels per two scan lines
vscl_tile               long    1 << 12 + 16            '16 pixels per tile with 1 clock per pixel
vscl_blank              long    hp                      'visible pixels per scan line

hv_sync                 long    $0200                   '+/-H,-V states
count_64                long    $04000000               'addend that sets carry every 64th addition
color_mask              long    $FCFCFCFC               'mask to isolate R,G,B bits from H,V
neg1                    long    $FFFFFFFF               'negative 1 to be written to sync indicator

' Undefined field data

tile_line               res     1

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