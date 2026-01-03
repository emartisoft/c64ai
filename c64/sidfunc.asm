.export _sid_init
.export _sid_play
.export _sid_mute
.export _sid_unmute
.export _load_A_File
.export _load_A_File_Border
.export _pseudosid
.export _set_sid_load_addr
.export _set_doc_load_addr

.segment "DATA"
.align 2
fileloadadr:
        .byte $84, $0f
border:
        .byte $00
loadmode:
        .byte $00

.segment "LOWCODE"

_set_sid_load_addr:
        lda #$84
        sta fileloadadr
        lda #$0f
        sta fileloadadr+1
        rts

_set_doc_load_addr:
        lda #$00
        sta fileloadadr
        lda #$B0
        sta fileloadadr+1
        rts

_pseudosid:
        LDA #$60
        STA $1000
        STA $1003
        LDA #$00
        STA sidinitaddr+1
        LDA #$03
        STA sidplayaddr+1
        LDA #$10
        STA sidinitaddr+2
        STA sidplayaddr+2
        RTS


_sid_init:
        ldx $0f8c
        stx sidinitaddr+2
        ldx $0f8d
        stx sidinitaddr+1
        ldx $0f8e
        stx sidplayaddr+2
        ldx $0f8f
        stx sidplayaddr+1

        ;lda #$00               ; musicindex (c'den parametre A'ya atanır)
sidinitaddr:
        jmp $9999               ; init music

_sid_play:
sidplayaddr:
        jmp $9999               ; play


.proc _sid_mute: near
        lda $d418
        and #$f0
        sta $d418               ; mute
        rts
.endproc

.proc _sid_unmute: near
        lda $d418
        ora #$0f
        sta $d418               ; mute
        rts
.endproc

.proc _load_A_File
        lda $fb                 ; dosya adı uzunluğu
        ldx $fc                 ; dosya adı başlangıç adresi
        ldy $fd
        jsr $ffbd               ; setnam

        lda #$01
        ldx $fe                 ; device number
        ldy #$00                ; yükleme kendimiz belirleyeceğiz
        jsr $ffba               ; setlfs

        ldx fileloadadr         ; $1000 load address SID dosyası başlık kısmından $7E
        ldy fileloadadr+1       ; sonrası kod başlar. Yüklemede 2 bayt es geçer
                                ; $1000-$7E+2=$0f84
        lda #$00                ; verify yapmadan yükle
        jsr $ffd5               ; load

        sta $fb                 ; yükleme sonucunu $fb'ye aktar
                                ; 0: yükleme run/stop ile durduruldu
                                ; 5: device not present
                                ; 4: file not found
                                ; $1d: load error
        rts
.endproc

L0328: 
        .BYTE $00, $00

.proc _load_A_File_Border
        sta loadmode            ; fonksiyonun parametresi A'ya yüklendiğinden loadmode'da saklanır
        sei

        LDA $0328
        STA L0328
        LDA $0329
        STA L0328+1

        lda #<flashload
        ldx #>flashload
        sta $0328
        stx $0329
        lda $d020
        sta border
        cli

        jsr _load_A_File
        sei
        lda L0328
        sta $0328
        lda L0328+1
        sta $0329
        cli
        lda border
        sta $d020
        rts
flashload: 
        inc $d020
        lda loadmode            ; loadmode $00 ise renkli border, değilse mono tek çizgi
        cmp #$00
        beq continue
        dec $d020
continue:
        jmp $f6fe

.endproc
