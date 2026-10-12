; May.22.1992 ú 08:36 ( Friday ) File :  MACRO.INC 
SetWord macro par1,par2
	org $-2
	ifnb	<par2>
par1 dw par2
	else
par1 dw 0FACEh
        endif
endm

SetByte macro par1,par2
	org $-1
	ifnb	<par2>
par1 db par2
	else
par1 db 0
        endif
endm

GetWord macro par1,par2,par3,par4
	par1	par2,0FACEh
        ifnb    <par4>
	SetWord par3,par4
	else
	SetWord par3
	endif
endm

GetByte macro par1,par2,par3,par4
	par1	par2,0
        ifnb    <par4>
	SetByte par3,par4
	else
	SetByte par3
	endif
endm

clr macro r
	xor	r,r
endm

move macro par1,par2,par3
	ifnb	<par3>
	mov	par3,par2
	mov	par1,par3
	else
	push	par2
	pop	par1
	endif
endm

GetDwrd macro par1,par2,par3
	par1	par2,0BEDAFACEh
	org	$-4
	par3	dd 0BEDAFACEh
	endm

SetDwrd macro par1
	org $-4
par1	dd 0FaceBedah
	endm

DecByte macro par,par2
	ifnb	<par2>
	org	$-par2
	else
	org	$-1
	endif
	par
	endm

ResetConv macro
	local matuta
	jmp	matuta
	matuta:
	endm
