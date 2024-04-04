.EXTERN _r_dsadc_interrupt
.PUBLIC _r_dsadc_interrupt_asm
.SECTION    .text, TEXT

;_r_dsadc_interrupt_asm  .VECTOR     0x0044
;                        .SECTION    .text, TEXT

                        
_r_dsadc_interrupt_asm:

;    SEL     RB3
;    MOV     A,ES
;    MOV     X,A
;    MOV     A,CS
;    PUSH    AX
    
    CALL    !!_r_dsadc_interrupt
    
    SET1    !DTCEN1.5
    
;    POP     AX
;    MOV     CS,A
;    MOV     A,X
;    MOV     ES,A
		RET
;    RETI