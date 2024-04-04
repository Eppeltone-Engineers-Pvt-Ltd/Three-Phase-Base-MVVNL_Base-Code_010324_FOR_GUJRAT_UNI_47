; Copyright (C) 2015 Renesas Electronics Corporation. All rights reserved.
; File Name    : r_sha.asm
; Version      : 1.00
; Device(s)    : None
; Tool-Chain   : CCRL

.PUBLIC _R_SHA1_DigestChunk_Asm

_R_SHA1_DigestChunk_Asm:
	;Stack
	;Local variable
	;[SP    ] 				: counter
	;[SP+0x01] --> [SP+0x03]  : <<no-use>>, alignment
	;[SP+0x04] --> [SP+0x07] 	: uint32_t temp
	;[SP+0x08] --> [SP+0x47]	: uint32_t W[16]
	;[SP+0x48] --> [SP+0x4B]	: uint32_t A;
	;[SP+0x4C] --> [SP+0x4F]	: uint32_t B;
	;[SP+0x50] --> [SP+0x53]	: uint32_t C;
	;[SP+0x54] --> [SP+0x57]	: uint32_t D;
	;[SP+0x58] --> [SP+0x5B]	: uint32_t E;
	
	;[SP+0x5E] --> [SP+0x5F]  : DE backup, dat_ptr
	;[SP+0x60] --> [SP+0x61]  : BC backup, blk_cnt
	;[SP+0x62] --> [SP+0x63]  : AX backup, sha1_info
	;[SP+0x64] --> [SP+0x65]  : HL backup
	;[SP+0x66] --> [SP+0x69]  : PC
	
	;Backup
	PUSH	HL
	PUSH	AX
	PUSH	BC
	PUSH	DE
	SUBW	SP, #0x5E
	MOVW	HL, SP
	
	;counter = 0;
	CLRB	A
	MOV	[HL], A
	
	;while (block > 0)
BLK_LOOP:	
	CLRW	AX
	CMPW	AX, [HL+0x60]
	SKNZ
	BR		$!BLK_COMPLETE
	
	;Init A,B,C,D,E
	MOVW	AX, [HL+0x62]
	MOVW	DE, AX			;Point DE to sha1_info
	
	MOVW	AX, [DE]		;A
	MOVW	[HL+0x48], AX
	MOVW	AX, [DE+0x02]
	MOVW	[HL+0x4A], AX
	MOVW	AX, [DE+0x04]	;B
	MOVW	[HL+0x4C], AX
	MOVW	AX, [DE+0x06]
	MOVW	[HL+0x4E], AX	
	MOVW	AX, [DE+0x08]	;C
	MOVW	[HL+0x50], AX
	MOVW	AX, [DE+0x0A]
	MOVW	[HL+0x52], AX	
	MOVW	AX, [DE+0x0C]	;D
	MOVW	[HL+0x54], AX
	MOVW	AX, [DE+0x0E]
	MOVW	[HL+0x56], AX		
	MOVW	AX, [DE+0x10]	;E
	MOVW	[HL+0x58], AX
	MOVW	AX, [DE+0x12]
	MOVW	[HL+0x5A], AX			
	
	;Reverse copy from sha1_info->msg.ptr to W 
	MOVW	AX, [HL+0x5E]
	MOVW	DE, AX
	
	MOVW	AX, [DE+0x02]
	XCH		A, X
	MOVW	[HL+0x08], AX
	MOVW	AX, [DE]
	XCH		A, X
	MOVW	[HL+0x0A], AX
	MOVW	AX, [DE+0x06]
	XCH		A, X
	MOVW	[HL+0x0C], AX
	MOVW	AX, [DE+0x04]
	XCH		A, X
	MOVW	[HL+0x0E], AX
	MOVW	AX, [DE+0x0A]
	XCH		A, X
	MOVW	[HL+0x10], AX
	MOVW	AX, [DE+0x08]
	XCH		A, X
	MOVW	[HL+0x12], AX
	MOVW	AX, [DE+0x0E]
	XCH		A, X
	MOVW	[HL+0x14], AX
	MOVW	AX, [DE+0x0C]
	XCH		A, X
	MOVW	[HL+0x16], AX
	MOVW	AX, [DE+0x12]
	XCH		A, X
	MOVW	[HL+0x18], AX
	MOVW	AX, [DE+0x10]
	XCH		A, X
	MOVW	[HL+0x1A], AX
	MOVW	AX, [DE+0x16]
	XCH		A, X
	MOVW	[HL+0x1C], AX
	MOVW	AX, [DE+0x14]
	XCH		A, X
	MOVW	[HL+0x1E], AX
	MOVW	AX, [DE+0x1A]
	XCH		A, X
	MOVW	[HL+0x20], AX
	MOVW	AX, [DE+0x18]
	XCH		A, X
	MOVW	[HL+0x22], AX
	MOVW	AX, [DE+0x1E]
	XCH		A, X
	MOVW	[HL+0x24], AX
	MOVW	AX, [DE+0x1C]
	XCH		A, X
	MOVW	[HL+0x26], AX
	MOVW	AX, [DE+0x22]
	XCH		A, X
	MOVW	[HL+0x28], AX
	MOVW	AX, [DE+0x20]
	XCH		A, X
	MOVW	[HL+0x2A], AX
	MOVW	AX, [DE+0x26]
	XCH		A, X
	MOVW	[HL+0x2C], AX
	MOVW	AX, [DE+0x24]
	XCH		A, X
	MOVW	[HL+0x2E], AX
	MOVW	AX, [DE+0x2A]
	XCH		A, X
	MOVW	[HL+0x30], AX
	MOVW	AX, [DE+0x28]
	XCH		A, X
	MOVW	[HL+0x32], AX
	MOVW	AX, [DE+0x2E]
	XCH		A, X
	MOVW	[HL+0x34], AX
	MOVW	AX, [DE+0x2C]
	XCH		A, X
	MOVW	[HL+0x36], AX
	MOVW	AX, [DE+0x32]
	XCH		A, X
	MOVW	[HL+0x38], AX
	MOVW	AX, [DE+0x30]
	XCH		A, X
	MOVW	[HL+0x3A], AX
	MOVW	AX, [DE+0x36]
	XCH		A, X
	MOVW	[HL+0x3C], AX
	MOVW	AX, [DE+0x34]
	XCH		A, X
	MOVW	[HL+0x3E], AX
	MOVW	AX, [DE+0x3A]
	XCH		A, X
	MOVW	[HL+0x40], AX
	MOVW	AX, [DE+0x38]
	XCH		A, X
	MOVW	[HL+0x42], AX
	MOVW	AX, [DE+0x3E]
	XCH		A, X
	MOVW	[HL+0x44], AX
	MOVW	AX, [DE+0x3C]
	XCH		A, X
	MOVW	[HL+0x46], AX
	
	;while (counter < 80)
	CLRB	A
	MOV		[HL], A
	
DIGEST_LOOP:	
	MOV		A, #0x50
	CMP		A, [HL]
	SKNZ	
	BR		$!FINISH
	
	;extend_counter = counter & 0x0F
	;MOVW	A, #0x0F
	;AND	A, [HL]
	;MOV	[HL+01H], A
	
	;if (counter > 15) -->extend the chunk
	MOV		A, #0x0F
	CMP		A, [HL]
	SKC	
	BR		$!ROUND_OPTION
	
EXTEND_CHUNK:
	
	;temp = W[(counter-3)&0x0F]
	MOV		A, [HL]
	SUB		A, #0x03
	AND		A, #0x0F
	CLRB	X
	SHL		A, 2
	XCH		A, X
	ADDW	AX, #0x08
	ADDW	AX, HL
	MOVW	DE, AX
	
	MOVW	AX, [DE]
	MOVW	[HL+0x04], AX
	MOVW	AX, [DE+0x02]
	MOVW	[HL+0x06], AX
	
	;temp ^= W[(counter-8)&0x0F]
	MOV		A, [HL]
	SUB		A, #0x08
	AND		A, #0x0F
	CLRB	X
	SHL		A, 2
	XCH		A, X
	ADDW	AX, #0x08
	ADDW	AX, HL
	MOVW	DE, AX
	
	MOVW	AX, [DE]
	XOR		A, [HL+0x05]
	XCH		A, X
	XOR		A, [HL+0x04]
	XCH		A, X
	MOVW	[HL+0x04], AX
	MOVW	AX, [DE+0x02]
	XOR		A, [HL+0x07]
	XCH		A, X
	XOR		A, [HL+0x06]
	XCH		A, X
	MOVW	[HL+0x06], AX

	;temp ^= W[(counter-14)&0x0F]
	MOV		A, [HL]
	SUB		A, #0x0E
	AND		A, #0x0F
	CLRB	X
	SHL		A, 2
	XCH		A, X
	ADDW	AX, #0x08
	ADDW	AX, HL
	MOVW	DE, AX
	
	MOVW	AX, [DE]
	XOR		A, [HL+0x05]
	XCH		A, X
	XOR		A, [HL+0x04]
	XCH		A, X
	MOVW	[HL+0x04], AX
	MOVW	AX, [DE+0x02]
	XOR		A, [HL+0x07]
	XCH		A, X
	XOR		A, [HL+0x06]
	XCH		A, X
	MOVW	[HL+0x06], AX
	
	;temp ^= W[(counter)&0x0F]
	MOV		A, [HL]
	AND		A, #0x0F
	CLRB	X
	SHL		A, 2
	XCH		A,X
	ADDW	AX, #0x08
	ADDW	AX, HL
	MOVW	DE, AX
	
	MOVW	AX, [DE]
	XOR		A, [HL+0x05]
	XCH		A, X
	XOR		A, [HL+0x04]
	XCH		A, X
	MOVW	[HL+0x04], AX
	MOVW	AX, [DE+0x02]
	XOR		A, [HL+0x07]
	XCH		A, X
	XOR		A, [HL+0x06]
	XCH		A, X
	MOVW	[HL+0x06], AX
	
	;W[counter&0x0F] = RotateLeft_1(temp)
	MOVW	BC, AX
	MOVW	AX, [HL+0x04]
	SHLW	AX, 1
	ROLWC	BC, 1
	SKNC
	ADDW	AX, #0x01
	MOVW	[DE], AX
	MOVW	AX, BC
	MOVW	[DE+0x02], AX
	
ROUND_OPTION:
	;temp = W[counter&0x0F] + E;
	MOV		A, [HL]
	AND		A, #0x0F
	SHL		A, 2
	CLRB	X
	XCH		A,X
	ADDW	AX, #0x08
	ADDW	AX, HL
	MOVW	DE, AX
	
	MOVW	AX, [DE]
	ADDW	AX, [HL+0x58]
	MOVW	[HL+0x04], AX 
	MOVW	AX, [DE+0x02]
	SKNC
	INCW	AX
	ADDW	AX, [HL+0x5A]
	MOVW	[HL+0x06], AX
		
	;temp += RotateLeft_5(A)
	MOVW	AX, [HL+0x48]
	MOVW	BC, AX
	MOVW	AX, [HL+0x4A]
	
	SHLW	AX, 1
	ROLWC	BC, 1
	SKNC
	ADDW	AX, #0x01
	
	SHLW	AX, 1
	ROLWC	BC, 1
	SKNC
	ADDW	AX, #0x01
	
	SHLW	AX, 1
	ROLWC	BC, 1
	SKNC
	ADDW	AX, #0x01

	SHLW	AX, 1
	ROLWC	BC, 1
	SKNC
	ADDW	AX, #0x01
	
	SHLW	AX, 1
	ROLWC	BC, 1
	SKNC
	ADDW	AX, #0x01
	
	XCHW	AX, BC
	ADDW	AX, [HL+0x04]
	MOVW	[HL+0x04], AX
	MOVW	AX, BC
	SKNC
	INCW	AX
	ADDW	AX, [HL+0x06]
	MOVW	[HL+0x06], AX

BELOW_ROUND_19:	
	MOV		A, #0x13
	CMP		A, [HL]
	SKNC
	BR		$!BELOW_ROUND_39
	;if (counter <= 19) then (temp += (D ^ (B & (C ^ D))) + 0x5a827999)
	MOVW	AX, [HL+0x50]
	MOVW	BC, AX
	MOVW	AX, [HL+0x52]
	MOVW	DE, AX
	MOVW	AX, [HL+0x54] ; AX = D low
	XOR		B, A
	XCH		A, X
	XOR		C, A
	MOVW	AX, [HL+0x56] ; AX = D high
	XOR		D, A
	XCH		A, X
	XOR		E, A
	MOVW	AX, [HL+0x4C] ; AX = B low
	AND		B, A
	XCH		A, X
	AND		C, A
	MOVW	AX, [HL+0x4E]; AX = B high
	AND		D, A
	XCH		A, X
	AND		E, A
	MOVW	AX, [HL+0x54] ; AX = D low
	XOR		B, A
	XCH		A, X
	XOR		C, A
	MOVW	AX, [HL+0x56] ; AX = D high
	XOR		D, A
	XCH		A, X
	XOR		E, A
	
	MOVW	AX, BC
	ADDW	AX, [HL+0x04]
	MOVW	[HL+0x04], AX
	MOVW	AX, DE
	SKNC
	INCW	AX
	ADDW	AX, [HL+0x06]
	MOVW	[HL+0x06], AX
	
	MOVW	AX, #0x7999
	ADDW	AX, [HL+0x04]
	MOVW	[HL+0x04], AX
	MOVW	AX, #0x5A82
	SKNC
	INCW	AX
	ADDW	AX, [HL+0x06]
	MOVW	[HL+0x06], AX
	
	BR		$!ROUND_COMMON
	
BELOW_ROUND_39:
	MOV		A, #0x27
	CMP		A, [HL]
	SKNC
	BR		$!BELOW_ROUND_59
	;if (20 <= counter <= 39) then (temp += (B ^ C ^ D) + 0x6ed9eba1 )
	MOVW	AX, [HL+0x54]	;D --> DE BC
	MOVW	BC, AX
	MOVW	AX, [HL+0x56]
	MOVW	DE, AX
	
	MOVW	AX, [HL+0x50]	;AX = C low
	XOR		B, A
	XCH		A, X
	XOR		C, A
	MOVW	AX, [HL+0x52]	;Ax = C high
	XOR		D, A
	XCH		A, X
	XOR		E, A
	MOVW	AX, [HL+0x4C] 	; AX = B low
	XOR		B, A
	XCH		A, X
	XOR		C, A
	MOVW	AX, [HL+0x4E]	; AX = B high
	XOR		D, A
	XCH		A, X
	XOR		E, A
	
	MOVW	AX, BC
	ADDW	AX, [HL+0x04]
	MOVW	[HL+0x04], AX
	MOVW	AX, DE
	SKNC
	INCW	AX
	ADDW	AX, [HL+0x06]
	MOVW	[HL+0x06], AX 

	MOVW	AX, #0xEBA1
	ADDW	AX, [HL+0x04]
	MOVW	[HL+0x04], AX
	MOVW	AX, #0x6ED9
	SKNC
	INCW	AX
	ADDW	AX, [HL+0x06]
	MOVW	[HL+0x06], AX	
	
	BR		$!ROUND_COMMON
	
BELOW_ROUND_59:
	MOV		A, #0x3B
	CMP		A, [HL]
	SKNC
	BR		$!BELOW_ROUND_79
	;if (counter <= 59) then (temp += ((B&C) ^ (B&D) ^ (C&D)) + 0x8f1bbcdc)
	MOVW	AX, [HL+0x54]	;D --> DE BC
	MOVW	BC, AX
	MOVW	AX, [HL+0x56]
	MOVW	DE, AX
	
	MOVW	AX, [HL+0x50]	;AX = C low
	AND		B, A
	XCH		A, X
	AND		C, A
	MOVW	AX, [HL+0x52]	;Ax = C high
	AND		D, A
	XCH		A, X
	AND		E, A
	
	;Backup C&D
	MOVW	AX, BC
	PUSH	AX
	MOVW	AX, DE
	PUSH	AX
	
	MOVW	AX, [HL+0x54]	;D --> DE BC
	MOVW	BC, AX
	MOVW	AX, [HL+0x56]
	MOVW	DE, AX
	
	MOVW	AX, [HL+0x4C]	;AX = B low
	AND		B, A
	XCH		A, X
	AND		C, A
	MOVW	AX, [HL+0x4E]	;Ax = B high
	AND		D, A
	XCH		A, X
	AND		E, A
	
	POP		AX
	XOR		D, A
	XCH		A, X
	XOR		E, A
	POP		AX
	XOR		B, A
	XCH		A, X
	XOR		C, A			;DE, BC : (B&D) ^ (C&D)
	
	;Backup (B&D) ^ (C&D)
	MOVW	AX, BC
	PUSH	AX
	MOVW	AX, DE
	PUSH	AX
	
	MOVW	AX, [HL+0x50]	;C --> DE BC
	MOVW	BC, AX
	MOVW	AX, [HL+0x52]
	MOVW	DE, AX
	
	MOVW	AX, [HL+0x4C]	;AX = B low
	AND		B, A
	XCH		A, X
	AND		C, A
	MOVW	AX, [HL+0x4E]	;Ax = B high
	AND		D, A
	XCH		A, X
	AND		E, A
	
	POP		AX
	XOR		D, A
	XCH		A, X
	XOR		E, A
	POP		AX
	XOR		B, A
	XCH		A, X
	XOR		C, A
	
	MOVW	AX, BC
	ADDW	AX, [HL+0x04]
	MOVW	[HL+0x04], AX
	MOVW	AX, DE
	SKNC	
	INCW	AX
	ADDW	AX, [HL+0x06]
	MOVW	[HL+0x06], AX
	
	MOVW	AX, #0xBCDC
	ADDW	AX, [HL+0x04]
	MOVW	[HL+0x04], AX
	MOVW	AX, #0x8F1B
	SKNC
	INCW	AX
	ADDW	AX, [HL+0x06]
	MOVW	[HL+0x06], AX	
	
	BR		$!ROUND_COMMON
	
BELOW_ROUND_79:
	;No need to check
	;if (counter <= 79) then (temp += (B ^ C ^ D) + 0xca62c1d6) 
	MOVW	AX, [HL+0x54]	;D --> DE BC
	MOVW	BC, AX
	MOVW	AX, [HL+0x56]
	MOVW	DE, AX
	
	MOVW	AX, [HL+0x50]	;AX = C low
	XOR		B, A
	XCH		A, X
	XOR		C, A
	MOVW	AX, [HL+0x52]	;Ax = C high
	XOR		D, A
	XCH		A, X
	XOR		E, A
	MOVW	AX, [HL+0x4C] 	; AX = B low
	XOR		B, A
	XCH		A, X
	XOR		C, A
	MOVW	AX, [HL+0x4E]	; AX = B high
	XOR		D, A
	XCH		A, X
	XOR		E, A
	
	MOVW	AX, BC
	ADDW	AX, [HL+0x04]
	MOVW	[HL+0x04], AX
	MOVW	AX, DE
	SKNC
	INCW	AX
	ADDW	AX, [HL+0x06]
	MOVW	[HL+0x06], AX 	

	MOVW	AX, #0xC1D6
	ADDW	AX, [HL+0x04]
	MOVW	[HL+0x04], AX
	MOVW	AX, #0xCA62
	SKNC
	INCW	AX
	ADDW	AX, [HL+0x06]
	MOVW	[HL+0x06], AX

ROUND_COMMON:
	;E=D
	MOVW	AX, [HL+0x54]
	MOVW	[HL+0x58], AX
	MOVW	AX, [HL+0x56]
	MOVW	[HL+0x5A], AX
	;D=C;
	MOVW	AX, [HL+0x50]
	MOVW	[HL+0x54], AX
	MOVW	AX, [HL+0x52]
	MOVW	[HL+0x56], AX
	;C=RotateLeft_30(B);
	;RotateLeft_30 = RotateRight_2
	MOVW	AX, [HL+0x4C]
	MOVW	BC, AX
	MOVW	AX, [HL+0x4E]
	SHRW	AX, 1
	XCHW	AX, BC			;1
	RORC	A, 1
	XCH		A, X
	RORC	A, 1
	XCH		A, X
	XCHW	AX, BC
	MOV1	A.7, CY
	SHRW	AX, 1
	XCHW	AX, BC			;2
	RORC	A, 1
	XCH		A, X
	RORC	A, 1
	XCH		A, X
	XCHW	AX, BC
	MOV1	A.7, CY
	
	MOVW	[HL+0x52], AX
	MOVW	AX, BC
	MOVW	[HL+0x50], AX
	
	;B=A;
	MOVW	AX, [HL+0x48]
	MOVW	[HL+0x4C], AX
	MOVW	AX, [HL+0x4A]
	MOVW	[HL+0x4E], AX
	;A=temp;
	MOVW	AX, [HL+0x04]
	MOVW	[HL+0x48], AX
	MOVW	AX, [HL+0x06]
	MOVW	[HL+0x4A], AX
	
	INC		[HL+0x0]
	BR		$!DIGEST_LOOP
	
FINISH:	
	MOVW	AX, [HL+0x62]
	MOVW	DE, AX
	
	;sha1_info->digest.A += A
	MOVW	AX, [DE]
	ADDW	AX, [HL+0x48]
	MOVW	[DE], AX
	MOVW	AX, [DE+0x02]
	SKNC
	INCW	AX
	ADDW	AX, [HL+0x4A]
	MOVW	[DE+0x02], AX
	
	;sha1_info->digest.B += B
	MOVW	AX, [DE+0x04]	
	ADDW	AX, [HL+0x4C]
	MOVW	[DE+0x04], AX
	MOVW	AX, [DE+0x06]
	SKNC
	INCW	AX
	ADDW	AX, [HL+0x4E]
	MOVW	[DE+0x06], AX

	;sha1_info->digest.C += C
	MOVW	AX, [DE+0x08]
	ADDW	AX, [HL+0x50]
	MOVW	[DE+0x08], AX
	MOVW	AX, [DE+0x0A]
	SKNC
	INCW	AX
	ADDW	AX, [HL+0x52]
	MOVW	[DE+0x0A], AX
	
	;sha1_info->digest.D += D
	MOVW	AX, [DE+0x0C]
	ADDW	AX, [HL+0x54]
	MOVW	[DE+0x0C], AX
	MOVW	AX, [DE+0x0E]
	SKNC
	INCW	AX
	ADDW	AX, [HL+0x56]
	MOVW	[DE+0x0E], AX	
	
	;sha1_info->digest.E += E
	MOVW	AX, [DE+0x10]
	ADDW	AX, [HL+0x58]
	MOVW	[DE+0x10], AX
	MOVW	AX, [DE+0x12]
	SKNC
	INCW	AX
	ADDW	AX, [HL+0x5B]
	MOVW	[DE+0x12], AX
	
	;Point to dat_ptr
	MOVW	AX, [HL+0x5E]
	ADDW	AX, #0x40	;Next 64byte block
	MOVW	[HL+0x5E], AX	;sha1_info->msg.ptr = AX
	DECW	[HL+0x60]
	BR		$!BLK_LOOP
	
BLK_COMPLETE:
	;Restore
	ADDW	SP, #0x5E
	POP		DE
	POP		BC
	POP		AX
	POP		HL
	
	RET