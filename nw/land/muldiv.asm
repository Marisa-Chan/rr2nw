.486p
MODEL	FLAT,CPP
LOCALS
; align	4
;_TEXT   Segment public use32 'CODE'
include platform.inc
BEGIN_CODE_SEG
;========================================================================

;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

MulDiv:
        Public MulDiv
MD proc a:dword,b:dword,c:dword  ;(a*b)/c

        mov     eax,[a]
        imul    dword ptr [b]
        idiv    dword ptr [c]
        ret
endP
END_CODE_SEG
end      