; Test.as

.entry 	LIST
.extern W
MAIN:	add r3, LIST
LOOP:	prn #48
macro m1
	lea W, r6
        inc r6
	mov S1.2 ,r3
endmacro
	mov r3, K
	sub r1, r4
m1
	bne END
        dec W
.entry MAIN
        jmp LOOP
        add L3, L3
END:	hlt
STR:	.string "abcdef"
LIST:	.data 6,-9,15
        .data -100
K:	.data 8
S1:	.struct 22, "rotem"
.extern L3
