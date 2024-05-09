        section .text
        align   4
; extern uint8_t sh_get_atomic_uint8(uint8_t *p);
        global  asm_get_atomic_uint8
        section .text.asm_get_atomic_uint8
asm_get_atomic_uint8::
        move.l  (4,a7),d0

        jsr     acquire
        move.l  d0,a0
        move.l  (a0),d0
        jsr     release
        rts

; extern uint8_t asm_set_atomic_uint32(uint8_t *p, uint8_t new);
        global  asm_set_atomic_uint8
        section .text.asm_set_atomic_uint8
asm_set_atomic_uint8::
        move.l  (4,a7),d0                       ; sh_atomic_uint32_t *p: pointer to atomic value
        move.b  (1,a7),d1                       ; uint32_t new: value to set

        jsr     acquire                         ; acquire spin lock on a0
        move.l  d0,a0
        move.b  (a0),d0
        move.b  d1,(a0)
        jsr     release
        rts

; extern uint32_t sh_get_atomic_uint32(uint32_t *p);
        global  asm_get_atomic_uint32
        section .text.asm_get_atomic_uint32
asm_get_atomic_uint32::
        move.l  (4,a7),d0

        jsr     acquire
        move.l  d0,a0
        move.l  (a0),d0
        jsr     release
        rts

; extern uint32_t asm_set_atomic_uint32(uint32_t *p, uint32_t new);
        global  asm_set_atomic_uint32
        section .text.asm_set_atomic_uint32
asm_set_atomic_uint32::
        move.l  (4,a7),d0                       ; sh_atomic_uint32_t *p: pointer to atomic value
        move.l  (8,a7),d1                       ; uint32_t new: value to set

        jsr     acquire                         ; acquire spin lock on a0
        move.l  d0,a0
        move.l  (a0),d0
        move.l  d1,(a0)
        jsr     release
        rts

acquire:
SPIN:
        eor.l   #$FFFFFFFF,spin_lock
        lea.l   spin_lock(pc),a0
        tas     (a0)
        bne.s   SPIN
        rts

release:
        and.l   #$7FFFFFFF,spin_lock
        rts

        section .data
spin_lock:
        dc.b    0
