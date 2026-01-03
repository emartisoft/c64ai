        .export _sid_irq_start
        .export _sid_irq_stop
        .import _sid_play           ; C tarafında tanımlı SID çalma fonksiyonu
        .import _old_irq            ; mouse için IRQ
        .export _sid_play_inirq2
        .export _sid_pause_inirq2

.segment "LOWCODE"

_sid_irq_start:
        sei
        lda #<cia_irq
        sta $0314
        lda #>cia_irq
        sta $0315
        lda #$7f
        sta $dc0d                ; eski CIA IRQ’ları temizle
        lda #$81
        sta $dc0d                ; Timer A interrupt enable

        ;PAL CPU freq.: 985,248 Hz.
        ;Target freq: 50 Hz (SID frame rate for PAL).
        ;Timer = CPU / DesiredHz
        ;Timer = 985,248 / 50 = 19,704.96
        ;19,705 (dec) = 0x4CF9 (hex)
        ;985,248 / 19,705 ≈ 49.9998985 Hz  

        lda #$F9        
        sta $dc04
        lda #$4c        
        sta $dc05

        lda #%00010001
        sta $dc0e                ; Timer A: start + count mode
        cli
        rts

_sid_irq_stop:
        sei
        lda #%00000000
        sta $dc0e                ; Timer durdur
        lda #$7f
        sta $dc0d                ; IRQ clear
        lda #<(_old_irq)
        sta $0314
        lda #>(_old_irq)
        sta $0315
        cli
        rts

; IRQ rutini:
cia_irq:
        lda pause
        beq cont
        jsr _sid_play            ; her tetikte SID çalma fonksiyonu     
cont:
        lda $dc0d                ; IRQ flag clear (okuma)
        JMP (_old_irq)
        jmp $ea81                ; varsayılan KERNAL IRQ’ya dön

_sid_pause_inirq2:
        lda #$00
        sta pause
        rts

_sid_play_inirq2:
        lda #$01
        sta pause            
        rts
    
; 0 -> pause, 1 -> play
pause:
        .BYTE $01