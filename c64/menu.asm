.export _menu
.export _spritepointer

.segment "CODE"
_spritepointer:
;----------------------------------------
; Sprite pointer tablosu ($07F8..$07FF)
;----------------------------------------
        ldy #$31
        ldx #$00
LPOINTER:
        tya
        sta $07F9,x
        inx
        iny
        cpx #$07
        bne LPOINTER
        lda #$0d
        sta $07F8       ; mouse pointer
        rts
_menu:
;----------------------------------------
; Renk ayarları
;----------------------------------------
        jsr _spritepointer

        ;lda #$00
        ;sta $D025      ; multicolor1 = siyah
        ;lda #$01
        ;sta $D026      ; multicolor2 = beyaz

        lda #$02       ; 
        sta $D028      ; sprite1 color
        ;sta $D029      ; sprite2

        lda #$05
        sta $D02A      ; sprite3
        sta $D02B      ; sprite4
        sta $D02C      ; sprite5

        lda #$07
        sta $D02D      ; sprite6

        lda #$00
        sta $d02e

;----------------------------------------
; Sprite konumları
;----------------------------------------

        lda #$00
        sta $d00e
        sta $d00f

        lda #65
        sta $D002      ; sprite1 X
        sta $D004      ; sprite2 X
        sta $D006      ; sprite3 X
        sta $D008      ; sprite4 X
        sta $D00A      ; sprite5 X
        sta $D00C      ; sprite6 X

        lda $d010
        ora #%01111110 ; bit1–6 X MSB set (sprite1=bit1 ... sprite6=bit6)
        sta $D010

        ; Y konumları: 74, 98, 130, 154, 178, 210
        lda #58
        sta $D003      ; sprite1
        lda #82
        sta $D005      ; sprite2
        lda #122
        sta $D007      ; sprite3
        lda #146
        sta $D009      ; sprite4
        lda #170
        sta $D00B      ; sprite5
        lda #202
        sta $D00D      ; sprite6

;----------------------------------------
; Multicolor mod (bit1–6 = 1)
;----------------------------------------
        lda $d01c
        ora #%01111110
        sta $D01C

;----------------------------------------
; Sprite’ları ekranda görünür yap (bit1–6 = 1)
;----------------------------------------
        lda $d015
        ora #%11111110
        sta $D015
        RTS

.segment "SPRITES"
        .incbin "sprites.raw"
