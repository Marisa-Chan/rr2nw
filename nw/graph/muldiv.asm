.486p
MODEL	FLAT,CPP
LOCALS
; align	4

include platform.inc

BEGIN_CODE_SEG
;========================================================================

MulDivL:
	Public MulDivL
MD proc a:dword,b:dword,c:dword

	mov	eax,[a]
	imul	[b]
	idiv	[c]
	ret
endp
;----------------------------------------------------------

MulDivF:
	Public MulDivF
MDF proc a:dword,b:dword,c:dword
	local	res:dword

	fld	[a]
	fild	[c]		;c a
	fild	[b]		;b c a
	fmul	st,st(2)	;b*a c a
	fdiv	st,st(1)
	fxch	st(2)		;a c res
	fcompp
	fstp	[res]
	mov	eax,[res]
	ret
endp
;----------------------------------------------------------

CalcPerPar:
	Public CalcPerPar
CPrP proc a:dword,az:dword,b:dword,bz:dword,d0:dword,d1:dword,nz:dword
	local	res:dword
	;(((a*az-b*bz)*d0)/d1+b*bz)/nz
	;(a*az*d0 + b*bz*(d1-d0))/(d1*nz)

	fild	[d0]		;d0
	fild	[d1]		;d1 d0
	fld	st(0)		;d1 d1 d0
	fsub	st,st(2)	;d1-d0 d1 d0
	fild	[a]		;a d1-d0 d1 d0
	fld	[az]		;az a d1-d0 d1 d0
	fmul	st,st(4)	;az*d0 a d1-d0 d1 d0
	fxch	st(1)		;a az*d0 d1-d0 d1 d0
	fmul	st,st(1)	;a*az*d0 az*d0 d1-d0 d1 d0
	fxch	st(4)		;d0 az*d0 d1-d0 d1 a*az*d0
	fcompp			;d1-d0 d1 a*az*d0
	fild	[b]		;b d1-d0 d1 a*az*d0
	fld	[bz]		;bz b d1-d0 d1 a*az*d0
	fmul	st,st(2)	;bz*(d1-d0) b d1-d0 d1 a*az*d0
	fxch	st(1)		;b bz*(d1-d0) d1-d0 d1 a*az*d0
	fmul	st,st(1)	;b*bz*(d1-d0)  bz*(d1-d0) d1-d0 d1 a*az*d0
	fadd	st,st(4)
	fxch	st(4)
        fcompp                  ;d1-d0 d1 del
        fld    [nz]            ;nz d1-d0 d1 del
        fmul    st,st(2)        ;nz*d1 d1-d0 d1 del
        fdivp    st(3),st       ;d1-d0 d1 Res
	fcompp
	fistp	[res]
	mov	eax,[res]
	ret
endp
;----------------------------------------------------------

CalcPerParF:
	Public CalcPerParF
CPrPF proc a:dword,az:dword,b:dword,bz:dword,d0:dword,d1:dword,nz:dword
	local	res:dword
	;(((a*az-b*bz)*d0)/d1+b*bz)/nz
	;(a*az*d0 + b*bz*(d1-d0))/(d1*nz)

	fild	[d0]		;d0
	fild	[d1]		;d1 d0
	fld	st(0)		;d1 d1 d0
	fsub	st,st(2)	;d1-d0 d1 d0
	fld	[a]		;a d1-d0 d1 d0
	fld	[az]		;az a d1-d0 d1 d0
	fmul	st,st(4)	;az*d0 a d1-d0 d1 d0
	fxch	st(1)		;a az*d0 d1-d0 d1 d0
	fmul	st,st(1)	;a*az*d0 az*d0 d1-d0 d1 d0
	fxch	st(4)		;d0 az*d0 d1-d0 d1 a*az*d0
	fcompp			;d1-d0 d1 a*az*d0
	fld	[b]		;b d1-d0 d1 a*az*d0
	fld	[bz]		;bz b d1-d0 d1 a*az*d0
	fmul	st,st(2)	;bz*(d1-d0) b d1-d0 d1 a*az*d0
	fxch	st(1)		;b bz*(d1-d0) d1-d0 d1 a*az*d0
	fmul	st,st(1)	;b*bz*(d1-d0)  bz*(d1-d0) d1-d0 d1 a*az*d0
	fadd	st,st(4)
	fxch	st(4)
        fcompp                  ;d1-d0 d1 del
        fld    [nz]            ;nz d1-d0 d1 del
        fmul    st,st(2)        ;nz*d1 d1-d0 d1 del
        fdivp    st(3),st       ;d1-d0 d1 Res
	fcompp
	fstp	[res]
	mov	eax,[res]
	ret
endp

END_CODE_SEG
end
