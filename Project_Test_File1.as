; Project_Test_File1.as


.extern L3
.extern W
MAIN:	mov S1.1, W
		move S2.1, w
		add r2,STR
		inc K
		mov S1.2 ,r3
		prn #-5
		sub r1, r4
		bne L3
END:	rts
STR:	.string "abcdef"
LENGTH:	.data 6,-9,15
K:		.data 22
S1:		.struct 8, "ab"