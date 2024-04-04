; Copyright (C) 2015 Renesas Electronics Corporation. All rights reserved.
; File Name    : r_cg_adc_ASM.asm
; Version      : 1.00
; Device(s)    : None
; Tool-Chain   : CCRL

.PUBLIC _R_DSADC_Get_Result

_R_DSADC_Get_Result:
    PUSH    DE
    SUBW    SP,#0x04
    MOV     [SP+0x00],A
    MOVW    AX,BC
    MOVW    [SP+0x02],AX
   
    MOVW    DE, AX
   
    MOV     A, [SP+0x00]
    CMP0    A
    SKZ
    BR      $!CHECK_CH1

    MOVW    AX, !DSADCR0
    MOVW    [DE],AX
    MOV      A, !DSADCR0H
    MOV     [DE+0x02],A   
    BR      $!GET_SIGN
   
CHECK_CH1:
    MOV     A, [SP+0x00]
    DEC     A
    SKZ   
    BR      $!CHECK_CH2
   
    MOVW    AX, !DSADCR1
    MOVW    [DE],AX
    MOV     A, !DSADCR1H
    MOV     [DE+0x02],A   
    BR      $!GET_SIGN
   
CHECK_CH2:
    MOV     A, [SP+0x00]
    CMP     A, #0x02
    SKZ
    BR      $!CHECK_CH3
   
    MOVW    AX, !DSADCR2
    MOVW    [DE],AX
    MOV     A, !DSADCR2H
    MOV     [DE+0x02],A   
    BR      $!GET_SIGN
   
CHECK_CH3:
    MOV     A, [SP+0x00]
    CMP     A, #0x03
    SKZ
    BR      $!FINISH
   
    MOVW    AX, !DSADCR3
    MOVW    [DE],AX
    MOV     A, !DSADCR3H
    MOV     [DE+0x02],A   
    BR      $!GET_SIGN

GET_SIGN:
    MOV     [DE+0x03],#0x00
    AND     A,#0x80
    SKZ
    MOV     [DE+0x03],#0xFF

FINISH:
    POP     DE
    ADDW    SP,#0x04
    RET
    