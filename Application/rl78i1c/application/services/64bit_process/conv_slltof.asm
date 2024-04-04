.PUBLIC _CONV_slltof_asm

_CONV_slltof_asm:
	;Stack allocation:
	; [SP+0x00] :	Backup of AX
	; [SP+0x02] : 	Backup of DE
	; [SP+0x04]	:	Backup of HL
	; [SP+0x06] : 	sl_input - ll
	; [SP+0x08] : 	sl_input - lh
	; [SP+0x0A] : 	sl_input - hl
	; [SP+0x0B] : 	sl_input - hh
	
	; AX: f_output pointer
	; BC: sl_input pointer
	; Also need to push AX to store output pointer
	SUBW	SP, #0x08
	PUSH	HL
	PUSH	DE
	PUSH	AX
	
	;Copy input to stack
	MOVW	AX, BC
	MOVW	HL, AX
	MOVW	AX, SP
	ADDW	AX, #0x06
	MOVW	DE, AX
	
	MOVW	AX, [HL+0x00]
	MOVW	[DE], AX
	MOVW	AX, [HL+0x02]
	MOVW	[DE+0x02], AX
	MOVW	AX, [HL+0x04]
	MOVW	[DE+0x04], AX
	MOVW	AX, [HL+0x06]
	MOVW	[DE+0x06], AX
	
	;Assign the copied stack pointer to HL
	MOVW	AX, DE
	MOVW	HL, AX
	
	;Store the sign
	;Check if input is negative or not
	MOV		E, #0x00
	MOVW	AX, #0x8000
	AND		A, [HL+0x07]
	SKNZ
	BR		!!_CONV_SLL_Convert

_CONV_SLL_Rev:
	;sl_input = -sl_input
	CLRW	AX
	SUBW	AX, [HL+0x00]
	MOVW	[HL+0x00], AX
	
	CLRW	AX
	SUBC	A, [HL+0x02]
	MOV		[HL+0x02], A
	XCH		A, X
	SUBC	A, [HL+0x03]
	MOV		[HL+0x03], A
	
	CLRW	AX
	SUBC	A, [HL+0x04]
	MOV		[HL+0x04], A
	XCH		A, X
	SUBC	A, [HL+0x05]
	MOV		[HL+0x05], A
	
	CLRW	AX
	SUBC	A, [HL+0x06]
	MOV		[HL+0x06], A
	XCH		A, X
	SUBC	A, [HL+0x07]
	MOV		[HL+0x07], A	
	MOV		E, #0x01
	
_CONV_SLL_Convert:
	;Check if bit(32-->64) == 0?
	MOV		A, [HL+0x07]
	OR		A, [HL+0x06]
	OR		A, [HL+0x05]
	OR		A, [HL+0x04]
	;Store this result to C
	MOV		C, A
	;Continue check whether all is zero?
	OR		A, [HL+0x03]
	OR		A, [HL+0x02]
	OR		A, [HL+0x01]
	OR		A, [HL+0x00]
	SKZ
	BR		!!_CONV_SLL_SetExponent
	CLRW	AX
	CLRW	BC
	BR		!!_CONV_SLL_SetOutputPointer

_CONV_SLL_SetExponent:
	;Exponent setting procedure:
	;	Maximum value of signed long long is 2^63
	;	Store this maximum to D
	;	Shift byte left as below:
	; 	In check0: 
	; 	In check1: 
	;	In loop:
	
	MOV		D, #0x3F
	
_CONV_SLL_SetExp_Check0:
	; If bit (32 --> 64) == 0 (C is holding this setting)
	; Multiply for 2^32, then D is substract by 32
	CMP0	C
	SKZ
	BR		!!_CONV_SLL_SetExp_Check1
	MOVW	AX, [HL+0x02]
	MOVW	[HL+0x06], AX
	MOVW	AX, [HL+0x00]
	MOVW	[HL+0x04], AX
	CLRW	AX
	MOVW	[HL+0x02], AX
	MOVW	[HL+0x00], AX
	MOV		A, #0x20
	SUB		D, A
	
_CONV_SLL_SetExp_Check1:
	; If bit (16 --> 31) == 0 (original value)
	; Multiply for 2^16, then D is substract by 16
	MOV		A, [HL+0x07]
	OR		A, [HL+0x06]
	SKZ
	BR		!!_CONV_SLL_SetExp_LoopBegin
	MOVW	AX, [HL+0x04]
	MOVW	[HL+0x06], AX
	MOVW	AX, [HL+0x02]
	MOVW	[HL+0x04], AX
	MOVW	AX, [HL+0x00]
	MOVW	[HL+0x02], AX
	CLRW	AX
	MOVW	[HL+0x00], AX
	MOV		A, #0x10
	SUB		D, A
	
_CONV_SLL_SetExp_LoopBegin:
	MOV		A, [HL+0x07]
	MOV1	CY, A.7
	SKNC
	BR		!!_CONV_SLL_SetExp_LoopEnd
	
_CONV_SLL_SetExp_Loop:
	; Decrease D by 1 each time
	; Add low16 to low16
	; Rotate whole 8byte left each time
	; If bit 7 of MSB is not 1, loop again
	DEC		D
	MOVW	AX, [HL+0x00]
	ADDW	AX, AX
	MOVW	[HL+0x00], AX
	MOVW	AX, [HL+0x02]
	ROLWC	AX, 1
	MOVW	[HL+0x02], AX
	MOVW	AX, [HL+0x04]
	ROLWC	AX, 1
	MOVW	[HL+0x04], AX
	MOVW	AX, [HL+0x06]
	ROLWC	AX, 1
	MOVW	[HL+0x06], AX
	MOV1	CY, A.7
	SKC
	BR		!!_CONV_SLL_SetExp_Loop
	
_CONV_SLL_SetExp_LoopEnd:
	;Exponent = calculated + 127
	MOV		A, #0x7F
	ADD		D, A
	
_CONV_SLL_Finishing_Check0:	
	; Rounding?
	; If byte(0 --> 3) != 0
	; then byte(4) |= 1
	MOV		A, [HL+0x00]
	OR		A, [HL+0x01]
	OR		A, [HL+0x02]
	OR		A, [HL+0x03]
	SKNZ
	BR		!!_CONV_SLL_Finishing_Check1
	MOV		A, [HL+0x04]
	OR		A, #0x01
	MOV		[HL+0x04], A
	
_CONV_SLL_Finishing_Check1:
	; Rounding?
	; Switch (byte4)
	; case < 0x80
	; 	set output
	; case == 0x80  check2
	; 	if (byte5), bit0 == 0
	;		set output
	;	else
	;		check3
	; case > 0x80: check3
	;	
	MOV		A, [HL+0x04]
	CMP		A, #0x80
	SKNC
	BR		!!_CONV_SLL_SetOut
	SKZ
	BR		!!_CONV_SLL_Finishing_Check3
	
_CONV_SLL_Finishing_Check2:
	MOV		A, [HL+0x05]
	MOV1	CY, A.0
	SKC
	BR		!!_CONV_SLL_SetOut
	
_CONV_SLL_Finishing_Check3:
	INC		[HL+0x05]
	SKZ
	BR		!!_CONV_SLL_SetOut
	MOVW	AX, [HL+0x06]
	ADDW	AX, #0x01
	MOVW	[HL+0x06], AX
	SKNC
	INC		D
	
_CONV_SLL_SetOut:
	; MSB  LSB (not follow compiler format)
	; AX , BC will hold output value
	; exponent: D (from beginnning)
	; byte 5 --> C
	; byte 6 --> B
	; byte 7 --> A <--> E (A will content the sign)
	; 
	; 
	MOV		A, [HL+0x05]
	MOV		C, A
	MOVW	AX, [HL+0x06]
	
	;Store the sign first by transfer A <--> E
	;This sign will be shifted out by Rotate instruction below
	XCH		A, E
	MOV1	CY, A.0
	;Now move byte6 to B
	MOV		A, X
	MOV		B, A
	;Shift the exponent part right 1 bit to follow IEEE format
	;restore the carry of sign bit
	;store the carry of fraction
	MOVW	AX, DE
	RORC	A, 1
	;Restore carry of fraction
	XCH		A, X
	MOV1	A.7, CY
	XCH		A, X
	
	;Done, now:
	;AX: high part of float
	;BC: low part of float
	
_CONV_SLL_SetOutputPointer:
	;Pop out value to HL
	POP		HL
	MOVW	[HL+0x02], AX
	XCHW	AX, BC
	MOVW	[HL+0x00], AX
	XCHW	AX, HL
	
_CONV_SLL_End:
	POP		DE
	POP		HL
	ADDW	SP, #0x08
	RET
	