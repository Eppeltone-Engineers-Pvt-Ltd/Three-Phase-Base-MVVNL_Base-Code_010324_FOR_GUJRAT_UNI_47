.PUBLIC _CONV_ftod_asm

_CONV_ftod_asm:
	;Register and stack allocation as below
	;AX		: void *d
	;BC		: f low (B LSB)
	;DE		: f high (D LSB)
		
	;Assign HL address of d
	MOVW	HL, AX
	CLRW	AX
	MOVW	[HL+0x00], AX
	
	;Shift all 4 bytes left 1 bit to get exponent into 1 byte and store sign bit.
	XCHW	AX, DE
	SHLW	BC, 1				;Shift low-part
	ROLWC	AX, 1				;Shift high-part
	SKNC						
	MOV		D, #0x80			;Store sign bit to D
	
	;All zeros cases:
ALL_ZERO_CHECK:
	SUBW	AX, BC				;Check if there's all zeros
	SKZ
	BR		!!INFINITY_CHECK
	MOVW	[HL+0x02], AX
	MOVW	[HL+0x04], AX
	MOVW	[HL+0x06], AX
	RET
	
	;If exp=7f ==> infinity
INFINITY_CHECK:
	ADDW	AX, BC				;Restore result of AX
	SUB		A, #0xFF			;Check 0xFF
	
	SKZ
	BR		!!DENORMAL_CHECK
	MOVW	[HL+0x02], AX
	MOVW	[HL+0x04], AX
	MOVW	AX, #0x7FF0
	OR 		A, D
	MOVW	[HL+0x06], AX
	RET
	
	;If exp=00 ==> DENORMAL value, special handling before go to normal handling
DENORMAL_CHECK:
	ADD		A, #0xFF				;Restore result of A
	CMP0	A
	SKZ
	BR		!!FTOD_CONVERSION
	
	;EXPLAIN THE LOOP PROCESS....
	
	XCH		A, X					;8 high bits of fraction --> A
DENORMAL_LOOP:	
	SHLW	BC, 0x01
	ROLC	A, 1
	SKNC
	BR		!!EXIT_DENORMAL_LOOP
	DEC		X						;exponent--
	BR		!!DENORMAL_LOOP
	
EXIT_DENORMAL_LOOP:
	XCH		A, X					;If exponent != 0 ==> 4 high exponent bit is 1s
	CMP0	A
	SKZ
	MOV		E, #0xF0
	
	;NORMALIZED value conversion
FTOD_CONVERSION:
	PUSH	AX						;Backup, high
	PUSH	BC						;Backup, low

	XCHW	AX, BC					;Move f 4 last nibbles to d byte #2, 3, BC high
	SHLW	AX, 0x0C				;Note: f shift left before for this purpose
	MOVW	[HL+0x02], AX	
	
	POP		AX						;Restore backup BC-->AX
	SHRW	AX, 0x04				; low >> 4 | high << 12
	SHLW	BC, 0x0C
	ADDW	AX, BC
	MOVW	[HL+0x04], AX
	
	POP		AX						;Restore backup AX-->AX
	SHRW	AX, 0x04
	OR		A, E					;From denormal processing
	ADD		A, #0x38
	OR		A, D
	MOVW	[HL+0x06], AX
	
	RET
	