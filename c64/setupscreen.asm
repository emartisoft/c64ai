.export _setup_screen
.export _show_sprite17
.export _hide_sprite17
.export _changePaperColor
.export _printC64AI
.import _spritepointer

.segment "CODE"

pcolorptr:
        .byte $0a
pcolor:
        .byte $01, $03, $04, $05, $07, $08, $09, $0a, $0d, $0e, $0f, $01
_changePaperColor:
        lda pcolorptr
        tax
        cmp #$0b
        bne L1 
        ldx #$00
        stx pcolorptr
L1:
        inc pcolorptr
        lda pcolor,x
        sta $d021
        inx
        lda pcolor,x
        sta $d029
        rts

_show_sprite17:
        lda $d015
        ora #$81
        sta $d015
        rts

_hide_sprite17:
        lda $d015
        and #$7e
        sta $d015
        rts

_setup_screen:
        ldx #$00
loop:
        lda buffer,x
        sta $0400,x
        lda buffer+$0100,x
        sta $0500,x
        lda buffer+$0200,x
        sta $0600,x
        lda buffer+$0300,x
        sta $0700,x
        inx
        bne loop
        jsr _spritepointer
        lda #$06        ; BLUE
loop2:
        sta $d800,x
        sta $d900,x
        sta $da00,x
        sta $db00,x
        inx
        bne loop2
        rts
buffer:
        .incbin "map40x25.bin"

_printC64AI:
        ldx #$00
loop3:
        lda line1,x
        sta $054A,x
        lda line2,x
        sta $0572,x
        lda line3,x
        sta $059A,x
        lda line4,x
        sta $05C2,x
        lda line5,x
        sta $05EA,x
        inx
        cpx #17
        bne loop3
        rts
line1:        
        .BYTE $DC, $DC, $DC, $20, $DC, $DC, $DC, $20, $DC, $20, $DC, $20, $DB, $DB, $DB, $20, $DB
line2:
        .BYTE $DC, $20, $20, $20, $DC, $20, $20, $20, $DC, $20, $DC, $20, $DB, $20, $DB, $20, $DB
line3:
        .BYTE $DC, $20, $20, $20, $DC, $DC, $DC, $20, $DC, $DC, $DC, $20, $DB, $DB, $DB, $20, $DB
line4:      
        .BYTE $DC, $20, $20, $20, $DC, $20, $DC, $20, $20, $20, $DC, $20, $DB, $20, $DB, $20, $DB
line5:      
        .BYTE $DC, $DC, $DC, $20, $DC, $DC, $DC, $20, $20, $20, $DC, $20, $DB, $20, $DB, $20, $DB
        
        