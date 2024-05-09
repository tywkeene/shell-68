        section .text
        align   4

        global  asm_get_fp
        section .text.asm_get_fp
asm_get_fp::
        move.l  a6,d0
        rts

        global  asm_get_sp
        section .text.asm_get_sp
asm_get_sp::
        move.l  a7,d0
        rts
