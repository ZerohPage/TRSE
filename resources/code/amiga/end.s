; exit gracefully - reverse everything done in init
	move.w #$7fff,DMACON
	move.w	olddmareq,DMACON
	move.w #$7fff,INTENA
	move.w	oldintena,INTENA
	move.w #$7fff,INTREQ
	move.w	oldintreq,INTREQ
	move.w #$7fff,ADKCON
	move.w	oldadkcon,ADKCON

	move.l	oldcopper,COP1LCH
	move.l 	gfxbase,a6
	move.l 	oldview,a1
	jsr -222(a6)	; LoadView
	jsr -270(a6)	; WaitTOF
	jsr -270(a6)	; WaitTOF
	move.l	$4,a6
	jsr -138(a6)	; Permit

	; end program
	movem.l (sp)+,d0-d7/a0-a6
	rts


;** PROCEDURES
; d6 = src shift
; d1 = dst x
; d2 = dst y
; d3 = modulo
; a0 = source
; a1 = dest
; d4 = blitter size
; d5 = bltmod
blitter:
;    lea     data_1,a0 ; Source
 ;   add.l   #80*120+10,a0
     add.l   d6,a0
;    lea     Screens_1,a1 ; Dest (x15)
    mulu.w  d3,d2
    add.l   d1,d2
    add.l   d2,a1
;    lea     ScreenTab(pc), a2 ; Save addresses here
;    move.w  #$09f0,d0 ; Goes in BLTCON0 (scroll first ) minterm
    move.w  #$09E0,d0 ; Goes in BLTCON0 (scroll first ) minterm
                        ; Leftshift 1, use channels A and D, copy A -> D
    moveq   #0,d2 
;    move.w  #(32*$40)*4+(2),d4 ; This is blitter size, y=256, x=44*8 // Størrelsen på området man kopierer *fra*

    move.l  #$ffffffff,BLTAFWM(a6) ; Set last word and first word mask BLTAFWM and BLTALWM
    move.w  d2,BLTCON1(a6) ;    issa 0   BLTCON1
.lp:

.litwait: ; Wait for blitter to be done
    btst    #14,DMACONR
    bne.s   .litwait

    move.w  d0,BLTCON0(a6)  ; Set registers; BLTCON0
    move.l  a0,BLTAPTH(a6) ; src 
    move.l  a1,BLTDPTH(a6) ; BLT Dest PTR
    move.w  d4,BLTSIZE(a6) ; BLTSIZE & Start blitter
    rts






; storage for 32-bit addresses and data
	CNOP 0,4
oldview:	dc.l 0
oldcopper:	dc.l 0
gfxbase:	dc.l 0
frame:          dc.l 0

; storage for 16-bit data
	CNOP 0,4
olddmareq:	dc.w 0
oldintreq:	dc.w 0
oldintena:	dc.w 0
oldadkcon:	dc.w 0

copper_index    dc.w 0
	CNOP 0,4
gfxname: dc.b 'graphics.library',0



  section datachip,data_c

Chip:

    even
cop:
    dc.w    $008e
diwstrt:
    dc.w    $2c81,$0090
diwstop:
    dc.w    $2cc1

    
    dc.w    $0092
ddfstrt:
; 
    dc.w    $0038,$0094
ddfstop:
    dc.w    $00d0
    dc.w    $0108
copper_mod_even:
;set bplmodulo here
    dc.w    0,$010a
copper_mod_odd:
;set bplmodulo here
    dc.w    0




CopSprites:
    
    dc.w $120,0, $122,0
    dc.w $124,0, $126,0
    dc.w $128,0, $12A,0
    dc.w $12C,0, $12E,0
    dc.w $130,0, $132,0
    dc.w $134,0, $136,0
    dc.w $138,0, $13A,0
    dc.w $13C,0, $13E,0

copper_palette:
    dc.w    $0180, $000
    dc.w    $0182, $fff
    dc.w    $0184, $236
    dc.w    $0186, $ba7
    dc.w    $0188, $836
    dc.w    $018a, $485
    dc.w    $018c, $723
    dc.w    $018e, $6cb
    dc.w    $0190, $246
    dc.w    $0192, $034
    dc.w    $0194, $569
    dc.w    $0196, $444
    dc.w    $0198, $666
    dc.w    $019a, $8d9
    dc.w    $019c, $b56
    dc.w    $019e, $999

    dc.w    $01A0, $000
    dc.w    $01A2, $fff
    dc.w    $01A4, $236
    dc.w    $01A6, $ba7
    dc.w    $01A8, $836
    dc.w    $01Aa, $485
    dc.w    $01Ac, $723
    dc.w    $01Ae, $6cb
    dc.w    $01B0, $246
    dc.w    $01B2, $034
    dc.w    $01B4, $569
    dc.w    $01B6, $444
    dc.w    $01B8, $666
    dc.w    $01Ba, $8d9
    dc.w    $01Bc, $b56
    dc.w    $01Be, $999





    dc.w    $e0
copper_bitplane0:
    dc.w    0,$e2
b0l:
    dc.w    0,$e4
copper_bitplane1:
    dc.w    0,$e6
b1l:
    dc.w    0,$e8
copper_bitplane2:
    dc.w    0,$ea
b2l:
    dc.w    0,$ec
copper_bitplane3:
    dc.w    0,$ee
b3l:
    dc.w    0,$f0
copper_bitplane4:
    dc.w    0,$f2
b4l:
    dc.w    0



copbplcon1:
    dc.l    $01020000
    dc.l    $2c01fffe
copbplcon0
; Set bitplane  B
   dc.w    $0100
copper_resolution
   dc.w     $4200 
copper_custom:
    dc.l    $fffffffe


;    dc.l    $2d01fffe, $01800000
 ;   dc.l    $8001fffe, $01002200
  ;  dc.l    $9001fffe, $01001200
;    dc.l    $ffdffffe
 ;   dc.l    $0d01fffe, $01000200
 blk.l   1024
    dc.l    $fffffffe


spritepointer:
    blk.b    256





