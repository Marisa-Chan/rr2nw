.486p
MODEL	FLAT,CPP
LOCALS
; align	4

include platform.inc

BEGIN_CODE_SEG

;========================================================================
include types.inc
include stack.inc
include get_s.inc

include subtloop.inc



FIX_SHIFT_32	equ 5
FIX_MASK_32	equ 31

BUMP_SHIFT equ 8
BUMP_PRES  equ 256

;
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

align 8
ASM_GRSetBump:
        Public ASM_GRSetBump
SetBump proc start:dword, end:dword

        fild    [start]
        fild    [end]
        fld1                    ;1 e s
        fdivr   st(2),st        ;1 e is
        fdivrp  st(1),st        ;ie is
        fst     [_iBumpEnd]     ;
        fxch    st(1)           ;is ie
        fst     [_iBumpStart]
        fsubrp  st(1),st        ;is-ie
        fld     [_C255Pres]     ;255 is-ie
        fdivrp  st(1),st        ;255/(is-ie)
        fstp    [_iBumpDelta]

        ret
endP





align 8
ADrawBump32:
	Public ADrawBump32

	fstcw   [_oldFPUCW]          ; store copy of CW
	mov     ax,_oldFPUCW         ; get it in ax
	and     eax,NOT 1100000000b ; 24 bit precision
	mov     [_newFPUCW],ax       ; store it
	fldcw   [_newFPUCW]          ; load the FPU

	mov	edx,[ESP_topVert]
	mov	esi,[ESP_bottomVert]
	mov	ecx,_gr_pYCache
	mov	ebx,[edx].y
	mov	edx,[esi].y
	mov	esi,offset screen
	sub	edx,ebx			;deltaY
	mov	edi,[ecx + ebx*4]	;scrPtr

        mov     eax,[_gr_polygon].bumpp

        mov     [ESP_count],edx
	mov	[ESP_endVert],edi
	mov	[ESP_startVert],esi


        mov     ebx,[eax + 8]  ;cache
        mov     eax,[eax + 12] ;data



	;TestCoordinates


	mov	ds:[?TextureTable32],ebx
	mov	ds:[??TextureTable32],ebx
        mov     ds:[???TextureTable32],ebx
	mov	edx,[ebx + 4]
	mov	ds:[?Texture32],eax
        mov     ds:[??Texture32],eax
	mov	ds:[?TextWidth32],edx
        ;mov     ds:[??TextWidth32],edx

@@TextLoopY:
	mov	eax,[esi].xl
	mov	ebx,[esi].xr
	add	edi,eax
	;dec	eax
	sub	ebx,eax
	jl	@@End

        ;mov     [ESP_scr],edi
	inc	ebx

        fild    [esi].izl       ;izl
        fld     [_iZPres]       ;izp izl
        fmul    st(1),st        ;izp izL
        fxch    st(1)           ;izL izp
        fst     [_FloatTmp]      ;
        mov     eax,[_FloatTmp]
        cmp     eax,[_iBumpStart] ;
        jl      @@NextCompareL
        ;Z = 255
        mov     ecx,255*BUMP_PRES
        jmp     @@Right
align 4
@@NextCompareL:
        cmp     eax,[_iBumpEnd]
        jg      @@ComputeZL
        ;Z = 0
        xor     ecx,ecx
        jmp     @@Right
align 4
@@ComputeZL:
        fld     [_iBumpEnd]     ;ibe izL izp
        fld     [_iBumpDelta]   ;ibd ibe izL izp
        fxch    st(1)           ;ibe ibd izL izp
        fsubp   st(2),st        ;ibd izL-ibe izp
        fmulp   st(1),st        ;ibd*(izL-ibe) izp
        fist    [_IntTmp]
        mov     ecx,[_IntTmp]
        ;
@@Right:
        fild    [esi].izr       ;izr ? izp
        fmulp   st(2),st        ;? izR
        fxch    st(1)           ;izR ?
        fst     [_FloatTmp]
        mov     eax,[_FloatTmp]
        cmp     eax,[_iBumpStart] ;
        jl      @@NextCompareR
        ;Z = 255
        mov     edx,255*BUMP_PRES
        jmp     @@SimpleClip
align 4
@@NextCompareR:
        cmp     eax,[_iBumpEnd]
        jg      @@ComputeZR
        ;Z = 0
        and     ch,ch
        jz      @@End
        xor     edx,edx
        jmp     @@SimpleClip
align 4
@@ComputeZR:
        fld     [_iBumpEnd]     ;ibe izR ?
        fld     [_iBumpDelta]   ;ibd ibe izR ?
        fxch    st(1)           ;ibe ibd izR ?
        fsubp   st(2),st        ;ibd izR-ibe ?
        fmulp   st(1),st        ;ibd*(izR-ibe) ?
        fist    [_IntTmp]
        mov     edx,[_IntTmp]
        ;
@@SimpleClip:

        ;ecx - L  8.8
        ;edx - R  8.8
        ;ebx - delat
        fcompp
        mov     [_deltaX],ebx

        mov     eax,ecx
        sub     eax,edx
        xor     ebx,0FFFFFFFFh
        sar     eax,BUMP_SHIFT
        imul    D_P [_gr_pDivCache + ebx*4 + 4] ;neg + 1

        mov     ebp,eax
        shld    edx,eax,32-Y_DIV_CACHE_PRES
        ;eax
        mov     eax,edx
        mov     edx,ecx
        shl     ebp,32 - Y_DIV_CACHE_PRES
        shl     edx,24
        and     ebp,0FFFF0000h
        mov     dl,ch
        mov     dh,al


;       cmp     ebx,3
;	jg	@@Gr3
;@@Gr3:
	fild	[esi].xr	;xr
	fisub	[esi].xl	;xr-xl
	fmul	[D_1Count32]
	;fmul	[D_Fix4]	;(xr-xl)/LI
	fld1			;1          (xr-xl)/LI
	fdiv	st,st(1)	;LI/(xr-xl)
        fild    [esi].vbzl       ;vzl        LI/(xr-xl)
        fild    [esi].vbzr       ;vzr        vzl        LI/(xr-xl)
	fsub	st,st(1)	;vzr-vzl    vzl        LI/(xr-xl)
	fmul	st,st(2)	;D          vzl        LI/(xr-xl)
	;fmul	[D_Fix1616]
	fstp	[ESP_D_D]		;vzl        LI/(xr-xl)
	;fmul	[D_Fix]
        fild    [esi].ubzl       ;uzl        vzl        LI/(xr-xl)
        fild    [esi].ubzr       ;uzr        uzl        vzl        LI/(xr-xl)
	fsub	st,st(1)	;uzr-uzl    uzl        vzl        LI/(xr-xl)
	fmul	st,st(3)	;B          uzl        vzl        LI/(xr-xl)
	;fmul	[D_Fix1616]
	fstp	[ESP_D_B]		;uzl        vzl        LI/(xr-xl)
	;fmul	[D_Fix]
	fild	[esi].izl	;1zl        uzl        vzl        LI/(xr-xl)
	fild	[esi].izr	;1zr        1zl        uzl        vzl        LI/(xr-xl)
	fsub	st,st(1)	;1zr-1zl    1zl        uzl        vzl        LI/(xr-xl)
	fmul	st,st(4)	;F          1zl        uzl        vzl        LI/(xr-xl)
	fmul	[D_FixCount32]
	;fmul	[D_Fix]
	fstp	[ESP_D_F]		;1zl        uzl        vzl
	fmul	[D_FixCount32]
	;fmul	[D_Fix]
	;
	fld1			;1       1zl     uzl     vzl
	fdiv	st,st(1)	;1/1zl   1zl     uzl     vzl
	fld	st		;1/1zl   1/1zl   1zl     uzl     vzl
	fmul	st,st(4)	;vzl/1zl 1/1zl   1zl     uzl     vzl
	fxch	st(1)		;1/1zl   vl      1zl     uzl     vzl
	fmul	st,st(3)	;ul      vl      1zl     uzl     vzl
	fst	st(5)
	fmul	[D_Count32]
	;fmul	[D_Fix1615]
	fistp	[ESP_u0]		;
	;fstp	st(5)		;vl      1zl     uzl     vzl     ul
	fst	st(5)
	;fmul	[D_Fix1616]
	fmul	[D_Count32]
	fistp	[ESP_v0]		;
	;fstp	st(5)		;1zl     uzl     vzl     ul      vl
	;
	;
	fadd	[ESP_D_F]           ;1zr     uzl     vzl     ul      vl
	fxch	st(1)		;uzl     1zr     vzl     ul      vl
	fadd	[ESP_D_B]           ;uzr     1zr     vzl     ul      vl
	fxch	st(2)		;vzl     1zr     uzr     ul      vl
	fadd	[ESP_D_D]		;vzr     1zr     uzr     ul      vl
	;
	fld1			;1       vzr     1zr     uzr     ul      vl
	fdiv	st,st(2)	;1/1zr   vzr     1zr     uzr     ul      vl
	;
	;sub	ebx,eax
	;jl	@@End
        ;mov     ecx,ebx
        ;mov     [_deltaX],ebx
        ;eax - {Vb}0{Ub}
        ;fracDelta - {dVb}0{dUb}
	;esi - Screen
        ;ecx - ptr

        mov     esi,edi
        GetDW   mov     ecx,?Texture32
	mov	ebx,[ESP_u0]
	mov	eax,ebx
        mov     edi,[ESP_v0]
	sar	ebx,16
	and	eax,00000000000000001111111111111111b
        add     ecx,ebx
        mov     ebx,edi
	shr	eax,1
        sar     edi,16
	sal	ebx,16
        add     ecx,[0FACEBEDAh+4*edi] ;Texture Offset
	org	$-4
	?TextureTable32 dd 0FACEBEDAh

        or      eax,ebx
        mov     ebx,[_deltaX]
	;
	fld	st		;1/1zr   1/1zr   vzr     1zr     uzr     ul      vl
	fmul	st,st(2)	;vzr/1zr 1/1zr   vzr     1zr     uzr     ul      vl
	fxch	st(1)		;1/1zr   vr      vzr     1zr     uzr     ul      vl
	fmul	st,st(4)	;uzr/1zr vr      vzr     1zr     uzr     ul      vl
	;                       ;
	;                       ;ur   vr   vzr  1zr  uzr  ul   vl
	;shr	ebx,FIX_SHIFT_32
	;inc	ebx
	and	ebx,0FFFFFFE0h
	jz	@@Ost
	;shl	ebx,FIX_SHIFT_32
	;add	ebx,[ESP_scr]
        ;mov     edx,edi
        add     ebx,esi
        ;GetDW   mov     [ESP_u0],?TextWidth32   ;Texture Width
	mov	ds:[?EndLineAdr32],ebx
	;mov	ds:[?TextureTable],ecx

	;push	eax
@@InnerLoop:
	;			;ur    vr   vzr  1zr  uzr  ul   vl
	fsubr	st(5),st	;ur    vr   vzr  1zr  uzr  du   vl
	fxch	st(1)		;vr    ur   vzr  1zr  uzr  du   vl
	fsubr	st(6),st	;vr    ur   vzr  1zr  uzr  du   dv
	fxch	st(6)		;dv    ur   vzr  1zr  uzr  du   vr
	;fmul	[D_Fix4]      ;dV    ur   vzr  1zr  uzr  du   vr
	fistp	[ESP_dv]		;ur    vzr  1zr  uzr  du   vr
	fxch	st(4)		;du    vzr  1zr  uzr  ur   vr
	;fmul	[D_Fix4_1]	;dU    vzr  1zr  uzr  ur   vr
	fistp	[ESP_du]		;vzr   1zr  uzr  ur   vr
	;calculate next u and v coordinates
				;vzl   1zl  uzl  ul   vl
	fadd	[ESP_D_D]		;vzr   1zl  uzl  ul   vl
	fxch	st(1)		;1zl   vzr  uzl  ul   vl
	fadd	[ESP_D_F]		;1zr   vzr  uzl  ul   vl
	fxch	st(2)		;uzl   vzr  1zr  ul   vl
	fadd	[ESP_D_B]		;uzr   vzr  1zr  ul   vl
	fxch	st(2)		;1zr   vzr  uzr  ul   vl
	fxch	st(1)		;vzr   1zr  uzr  ul   vl
	fld1			;1     vzr  1zr  uzr  ul   vl
	fdiv	st,st(2)	;1/1zr vzr  1zr  uzr  ul   vl
	;
	;
	mov	ebx,[ESP_dv]	;16.16
        mov     edi,[ESP_du]    ;(0(1))16.15

        sal     ebx,16
        and     edi,00000000000000001111111111111111b   ;{du}
        shr     edi,1
        or      ebx,edi
        mov     edi,[ESP_du]
        mov     [_fracDelta],ebx
        mov     ebx,[ESP_dv]
        sar     edi,16
        sar     ebx,16
        add     edi,[0FACEBEDAh + 4*ebx]        ;[du]+[dv]*TextWidth
	org	$-4
	??TextureTable32	dd	0FACEBEDAh

        mov     [_deltaTextureW],edi
        GetDW   add     edi,?TextWidth32   ;Texture Width
        ;add     edi,[ESP_u0]
        xor     ebx,ebx
        mov     [_deltaTextureWW],edi

	;xor	eax,eax		;fuck me!
	;push	ebp		;U-PIPE!
	;
        ;draw 4 pixels of bump texture
        ;eax - {Vb}0{Ub}
        ;fracDelta - {dVb}0{dUb}
	;esi - Screen
        ;ecx - ptr
	;edx - {Z}[dZ][Z]
	;ebp - {dZ}<16Bit0>
	;ebx - Tmp
        ;Delta - ( [dUb]+([dVb]+1)*256, [dUb]+[dVb]*256 ) <-pointer

        InnerBumpLoop4New 0 ,[_fracDelta],_deltaTextureW
        InnerBumpLoop4New 4 ,[_fracDelta],_deltaTextureW
        InnerBumpLoop4New 8 ,[_fracDelta],_deltaTextureW
        InnerBumpLoop4New 12,[_fracDelta],_deltaTextureW
        InnerBumpLoop4New 16,[_fracDelta],_deltaTextureW
        InnerBumpLoop4New 20,[_fracDelta],_deltaTextureW
        InnerBumpLoop4New 24,[_fracDelta],_deltaTextureW
        InnerBumpLoop4New 28,[_fracDelta],_deltaTextureW



        add     esi,32
	;
	;fdiv complete
	;

	fld	st		;1/1zr 1/1zr vzr  1zr  uzr  ul   vl
	fmul	st,st(2)	;vr    1/1zr vzr  1zr  uzr  ul   vl
	fxch	st(1)		;1/1zr vr    vzr  1zr  uzr  ul   vl
	fmul	st,st(4)	;ur    vr    vzr  1zr  uzr  ul   vl

	;mov	edi,[ESP_scr]
	;mov	[ESP_scr],edi
        GetDW   cmp     esi,?EndLineAdr32
	;mov	[edi-4],ebx
	jnz	@@InnerLoop
        ;mov     edi,esi
	;pop	eax
	;
@@Ost:
	;;jmp	@@End
	;mov	ecx,[_deltaX]
	;mov	eax,ecx
	;and	ecx,NOT FIX_MASK_32
	;sub	eax,ecx
	;;jz	@@End
	;;inc	eax
	;mov	[_deltaX],eax
	;jz	@@End
	;

        mov     ebx,[ESP_startVert] ;[scan]

	and	[_deltaX],1Fh
	jz	@@End
	dec	[_deltaX]

        ;                       ;-    -    -    -    -    uL<  vL<
	fld	st(5)		;uL<  -    -    -    -    -    uL<  vL<
	fmul	[D_Count32]	;ul<  -    -    -    -    -    uL<  vL<
	fist	[ESP_u0]  	;ul<  -    -    -    -    -    uL<  vL<
	fstp	st(6)           ;-    -    -    -    -    ul<  vL<
	fld	st(6)		;vL<  -    -    -    -    -    ul<  vL<
	fmul	[D_Count32]	;vl<  -    -    -    -    -    ul<  vL<
	fst	st(7)		;vl<  -    -    -    -    -    ul<  vl<
	fistp	[ESP_v0]	;-    -    -    -    -    ul<  vl<
	;
	fcompp			;-    -    -    ul<  vl<
	;
        fild    [ebx].ubzr       ;uzr< -    -    -    ul<  vl<
        fild    [ebx].vbzr       ;vzr< uzr< -    -    -    ul<  vl<
        fild    [ebx].izr       ;izr< vzr< uzr< -    -    -    ul<  vl<

	fdivr	[D_Fix1616]	;zr   vzr< uzr< -    -    -    ul<  vl<
	fmul	st(1),st	;zr   vr<  uzr< -    -    -    ul<  vl<
	fmulp	st(2),st	;vr<  ur<  -    -    -    ul<  vl<

	fsub	st,st(6)	;dv<  ur<  -    -    -    ul<  vl<
	fxch	st(1)		;ur<  dv<  -    -    -    ul<  vl<
	fsub	st,st(5)	;du<  dv<  -    -    -    ul<  vl<

	fidiv	[_deltaX]
	fistp	[ESP_du]

	fidiv	[_deltaX]
	fist	[ESP_dv]
	;
        ;GetDW   mov     ecx,??Texture32
        ;mov     [_fracDelta],ebp
	mov	ebx,[ESP_u0]
        mov     edi,[ESP_v0]
	;add	ebx,800h
	;add	edx,800h

@@OstLoop:
        ;eax - tmp
        ;ebx - u 16.16
        ;ecx - tmp
        ;edx - {Z}[dZ][Z]
        ;esi - Screen
        ;edi - v 16.16
        ;ebp - {dZ}<16Bit0>
        ;Delta - ( [dUb]+([dVb]+1)*256, [dUb]+[dVb]*256 ) <-pointer
        mov     ecx,edi ;v
	mov	eax,ebx	;u
	sar	ecx,16	;[v]
        add     edi,[ESP_dv];v+dv
	sar	eax,16	;[u]
	add	ebx,[ESP_du];u+du

	add	eax,[0FACEBEDAh + 4*ecx]	;[u]+[v]*TextWidth
	org	$-4
	???TextureTable32	dd	07ACEBEDAh

        mov     ch,dl

        mov     cl,[0FACEBEDAh+eax]
        org     $-4
        ??Texture32   dd 0FACEBEDAh

        inc     esi
        mov     ch,B_P [ecx + _gr_BumpMulTable] ;U
        add     edx,ebp
        adc     dl,dh
        mov     cl,[esi-1]
        mov     ah,B_P [ecx + _gr_BumpMixColorTable]
        dec     [_deltaX]
        mov     [esi-1],ah

        jge     @@OstLoop


@@End:
	mov	esi,[ESP_startVert]
	mov	edi,[ESP_endVert]
	add	esi,type Screen
	GetDWPub	add	edi,EXT_ScreenWidth11
	dec	[ESP_count]
	mov	[ESP_startVert],esi
	mov	[ESP_endVert],edi
	ffree   st(0)
	ffree   st(1)
	ffree   st(2)
	ffree   st(3)
	ffree   st(4)
	ffree   st(5)
	ffree   st(6)
	jge	@@TextLoopY
	;
	;
	fldcw   [_oldFPUCW]	; restore the FPU

        mov     eax,[_gr_polygon].addtyp

jmp     END_DRAW_BUMP
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::;
;
;

unused dd  1024 DUP(?)
;==========ASM extern Labels==============
align 4
extrn END_TYPE_DRAW:proc
extrn END_DRAW_BUMP:proc
extrn ENDD:proc
;==========ASM extern=====================
align 4
extrn _gr_vertices:dword
extrn interVert:dword
extrn _gr_polygon:dword
extrn screen:dword
extrn pYDivCache:dword
;===========C(++) extern==================
align 4
extrn   _gr_pYCache:dword
extrn p1DivCache:dword
extrn	_gr_nScreenWidth:dword
;=============Module Data=================
align 4
_oldFPUCW dw ?
align 4
_newFPUCW dw ?

align 4
_deltaX dd ?

align 4
D_One           dd 1.0
D_Fix1616       dd 65536.0

_fracDelta      dd ?
_deltaTextureWW dd ?
_deltaTextureW  dd ?
_gr_BumpMulTable   db 1 DUP (?) ;65536 DUP (?)
public _gr_BumpMulTable
_gr_BumpMixColorTable      db 1 DUP (?) ;65536 DUP (?)
public _gr_BumpMixColorTable


;_C255Pres       dd 32512.0      ;(127*BUMP_PRES)
_C255Pres       dd 65280.0      ;(255*BUMP_PRES)
_iBumpStart     dd ?
_iBumpEnd       dd ?
_iBumpDelta     dd ?
_iZPres         dd ?
public  _iZPres
_IntTmp         dd ?
_FloatTmp       dd ?

D_1Count32      dd 0.03125 ;1.0/(2^FIX_SHIFT)
D_Count32       dd 32.0 ;(2^FIX_SHIFT)
D_FixCount32    dd 0.00048828125   ;(2^FIX_SHIFT)/65536.0



D_1Count4	dd  0.25 ;1.0/(2^FIX_SHIFT)
D_Count4	dd  4.0 ;(2^FIX_SHIFT)
D_FixCount4     dd 0.00006103515625   ;(2^FIX_SHIFT)/65536.0

D_1Count8	dd  0.125 ;1.0/(2^FIX_SHIFT)
D_Count8	dd  8.0 ;(2^FIX_SHIFT)
D_FixCount8     dd 0.0001220703125   ;(2^FIX_SHIFT)/65536.0

D_1Count16	dd  0.0625 ;1.0/(2^FIX_SHIFT)
D_Count16	dd  16.0 ;(2^FIX_SHIFT)
D_FixCount16    dd 0.000244140625   ;(2^FIX_SHIFT)/65536.0


;D_1Count32	dd  0.030303030303 ;1.0/(2^FIX_SHIFT)
;D_Count32	dd  33.0 ;(2^FIX_SHIFT)
;D_FixCount32 dd 0.0005035400391   ;(2^FIX_SHIFT)/65536.0

END_CODE_SEG
end      