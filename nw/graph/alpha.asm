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

;
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

TestCoordinates Macro

	push	eax
	push	ebx
	push	edx
	push	edi
	push	esi

	mov	ebx,[eax - 4] ;width
	mov	ecx,[eax - 8] ;height
	dec	ebx
	dec	ecx
	mov	edi,edx ;count
	mov	ebp,esi
@@Loop:
	mov	eax,[ebp].uzl
	mov	esi,[ebp].izl
	cdq
	idiv	esi
	cmp	eax,2
	jge	@@Ok
	int	3
@@Ok:
	cmp	eax,ebx
	jl	@@Ok1
	int	3
@@Ok1:

	mov	eax,[ebp].vzl
	cdq
	idiv	esi
	cmp	eax,2
	jge	@@Ok4
	int	3
@@Ok4:
	cmp	eax,ecx
	jl	@@Ok5
	int	3
@@Ok5:


	mov	eax,[ebp].uzr
	mov	esi,[ebp].izr
	cdq
	idiv	esi
	cmp	eax,2
	jge	@@Ok2
	int	3
@@Ok2:
	cmp	eax,ebx
	jl	@@Ok3
	int	3
@@Ok3:

	mov	eax,[ebp].vzr
	cdq
	idiv	esi
	cmp	eax,2
	jge	@@Ok6
	int	3
@@Ok6:
	cmp	eax,ecx
	jl	@@Ok7
	int	3
@@Ok7:

	add	ebp,type Screen
	dec	edi
	jge     @@Loop

	pop	esi
	pop	edi
	pop	edx
	pop	ebx
	pop	eax

endM
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::;
;
;

align 8
ADrawAlphaTexture32:
        Public ADrawAlphaTexture32

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

        mov     eax,[_gr_polygon].textp

        mov     [ESP_count],edx
	mov	[ESP_endVert],edi
	mov	[ESP_startVert],esi



        mov     ebx,[eax + 8]  ;cache
        mov     eax,[eax + 12] ;data

	;TestCoordinates


        mov     ds:[?TextureTable32],ebx
        mov     ds:[?Texture32],eax
        mov     ds:[??TextureTable32],ebx
        mov     edx,[ebx + 4]
        mov     ds:[???TextureTable32],ebx
        mov     ds:[????TextureTable32],ebx
        mov     ds:[??Texture32],eax
        mov     ds:[???Texture32],eax
        mov     eax,[_gr_polygon].color
	mov	ds:[?TextWidth32],edx
        mov     ds:[?AlphaMixTable],eax
        mov     ds:[??AlphaMixTable],eax
        mov     ds:[???AlphaMixTable],eax

@@TextLoopY:
	mov	eax,[esi].xl
	mov	ebx,[esi].xr
	add	edi,eax
	;dec	eax
	sub	ebx,eax
	jl	@@End

	mov	[ESP_scr],edi
	cmp	ebx,1
	jle	@@Ls3
	inc	ebx
;@@Gr3:
	fild	[esi].xr	;xr
	fisub	[esi].xl	;xr-xl
	fmul	[D_1Count32]
	;fmul	[D_Fix4]	;(xr-xl)/LI
	fld1			;1          (xr-xl)/LI
	fdiv	st,st(1)	;LI/(xr-xl)
	fild	[esi].vzl	;vzl        LI/(xr-xl)
	fild	[esi].vzr       ;vzr        vzl        LI/(xr-xl)
	fsub	st,st(1)	;vzr-vzl    vzl        LI/(xr-xl)
	fmul	st,st(2)	;D          vzl        LI/(xr-xl)
	;fmul	[D_Fix1616]
	fstp	[ESP_D_D]		;vzl        LI/(xr-xl)
	;fmul	[D_Fix]
	fild	[esi].uzl	;uzl        vzl        LI/(xr-xl)
	fild	[esi].uzr	;uzr        uzl        vzl        LI/(xr-xl)
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
	mov	ecx,ebx
	mov	[_deltaX],ebx

	GetDW	mov	esi,?Texture32
	mov	ebx,[ESP_u0]
	mov	eax,ebx
	mov     edx,[ESP_v0]
	sar	ebx,16
	and	eax,00000000000000001111111111111111b
	add	esi,ebx
	mov	ebx,edx
	shr	eax,1
	sar	edx,16
	sal	ebx,16
	add	esi,[0FACEBEDAh+4*edx] ;Texture Offset
	org	$-4
	?TextureTable32 dd 0FACEBEDAh

	add	eax,ebx
	mov	ebx,ecx
	;
	fld	st		;1/1zr   1/1zr   vzr     1zr     uzr     ul      vl
	fmul	st,st(2)	;vzr/1zr 1/1zr   vzr     1zr     uzr     ul      vl
	fxch	st(1)		;1/1zr   vr      vzr     1zr     uzr     ul      vl
	fmul	st,st(4)	;uzr/1zr vr      vzr     1zr     uzr     ul      vl
	;                       ;
	;                       ;ur   vr   vzr  1zr  uzr  ul   vl
	;shr	ebx,FIX_SHIFT_32
	;inc	ebx
        dec     ebx ;!!!!!!
	and	ebx,0FFFFFFE0h
	jz	@@Ost
	;shl	ebx,FIX_SHIFT_32
	add	ebx,edi
        GetDW   mov     edx,?AlphaMixTable
        mov     ds:[?EndLineAdr32],ebx
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
	mov	ecx,[ESP_du]	;(0(1))16.15
	;push	ebp
	mov	edi,ebx
	mov	ebp,ecx         ;
	sar	ebx,16		;[dv]
	and	ebp,00000000000000001111111111111111b	;{du}
	sar	ecx,16		;[du]
	sal	edi,16		;{dv}*65536
	shr	ebp,1
	add	ecx,[0FACEBEDAh + 4*ebx]	;[du]+[dv]*TextWidth
	org	$-4
	??TextureTable32	dd	0FACEBEDAh
	;pop	eax
	add	edi,ebp		;{dv}0{du}
        mov     [_deltaTextureW],ecx
        GetDW   add     ecx,?TextWidth32        ;Texture Width
        xor     ebx,ebx
        mov     [_deltaTextureWW],ecx
        mov     ecx,[ESP_scr]
        ;xor    eax,eax         ;
	;push	ebp		;U-PIPE!
	;
        ;draw 32 pixels of texture
        ;!eax - {v}0{u}
	;ebx - store pixels
        ;!edx - AlphaMixTable Pointer
        ; [du] + [dv]*TextureWidth
        ;ecx - Screen Pointer
        ;!esi - Texture Pointer
	;edi - {dv}0{du}
	;ebp - carry
	;

        InnerAlphaTextureLoop32    _deltaTextureW

	;
	;fdiv complete
	;

	fld	st		;1/1zr 1/1zr vzr  1zr  uzr  ul   vl
	fmul	st,st(2)	;vr    1/1zr vzr  1zr  uzr  ul   vl
	fxch	st(1)		;1/1zr vr    vzr  1zr  uzr  ul   vl
	fmul	st,st(4)	;ur    vr    vzr  1zr  uzr  ul   vl

        ;mov     edi,[ESP_scr]
        ;add     edi,32
        mov     [ESP_scr],ecx
        GetDW   cmp     ecx,?EndLineAdr32
        ;mov     [edi-4],ebx
	jnz	@@InnerLoop
	;pop	eax
	;
@@Ost:
        ;jmp   @@End
	;mov	ecx,[_deltaX]
	;mov	eax,ecx
	;and	ecx,NOT FIX_MASK_32
	;sub	eax,ecx
	;;jz	@@End
	;;inc	eax
	;mov	[_deltaX],eax
	;jz	@@End
	;
        mov     ebp,[_deltaX];!!!!!!!!!!!!!
	mov	ecx,[ESP_startVert] ;[scan]
        dec     ebp

        ;and     [_deltaX],1Fh!!!!!!!!!!!
        ;jz      @@End
        ;dec     [_deltaX]!!!!!!!!!!!!!

	;			;-    -    -    -    -    uL<  vL<
	fld	st(5)		;uL<  -    -    -    -    -    uL<  vL<
	fmul	[D_Count32]	;ul<  -    -    -    -    -    uL<  vL<
	fist	[ESP_u0]  	;ul<  -    -    -    -    -    uL<  vL<
	fstp	st(6)           ;-    -    -    -    -    ul<  vL<
	fld	st(6)		;vL<  -    -    -    -    -    ul<  vL<
	fmul	[D_Count32]	;vl<  -    -    -    -    -    ul<  vL<
	fst	st(7)		;vl<  -    -    -    -    -    ul<  vl<
	fistp	[ESP_v0]	;-    -    -    -    -    ul<  vl<
        ;
        and     ebp,1Fh ;!!!!!!!!!!!!!!!!!1
        ;
	fcompp			;-    -    -    ul<  vl<
	;
	fild	[ecx].uzr	;uzr< -	   -    -    ul<  vl<
	fild	[ecx].vzr	;vzr< uzr< -    -    -    ul<  vl<
	fild	[ecx].izr	;izr< vzr< uzr< -    -    -    ul<  vl<

        inc     ebp

	fdivr	[D_Fix1616]	;zr   vzr< uzr< -    -    -    ul<  vl<
        mov     [_deltaX],ebp
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
        ;GetDW   mov     esi,??Texture32
	mov	ebx,[ESP_u0]
	mov	edx,[ESP_v0]
        mov     edi,[ESP_scr]
        GetDW   mov     esi,??AlphaMixTable
        ;add    ebx,800h
	;add	edx,800h

@@OstLoop:
        mov     ecx,edx ;v
        mov     eax,ebx ;u
        sar     ecx,16  ;[v]
        add     edx,[ESP_dv];v+dv
        sar     eax,16  ;[u]
        inc     edi
        add     ebx,[ESP_du];u+du

        add     eax,[0FACEBEDAh + 4*ecx]        ;[u]+[v]*TextWidth
        org     $-4
	???TextureTable32	dd	07ACEBEDAh

        mov     ch,[0FACEBEDAh+eax]
        org     $-4
        ??Texture32       dd      07ACEBEDAh

        mov     cl,[edi-1]
        dec     ebp ;!!!!!!!!!!!!!!!!

        mov     ch,B_P [esi+ecx]
        mov     [edi-1],ch

        jge     @@OstLoop

@@End:
	mov	esi,[ESP_startVert]
	mov	edi,[ESP_endVert]
	add	esi,type Screen
        GetDWPub        add     edi,EXT_ScreenWidth16a
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


jmp	END_TYPE_DRAW
	;
	;
	;
@@Ls3:
	fild	[esi].izl
	fild	[esi].uzl
	fild	[esi].vzl	;vzl uzl 1zl
	fld1			;1 vzl uzl 1zl
	fdivrp	st(2),st        ;vzl uzl zl
	fmul	st,st(2)	;v uzl zl
	fxch	st(1)
	fmul	st,st(2)
	fxch	st(1)
	fistp	[ESP_v0]
	fistp	[ESP_v0]	;zl
	;
        mov     ecx,[ESP_v0]
        mov     eax,[ESP_u0]
        sar     ecx,16
        GetDW   mov     ebp,????TextureTable32
        sar     eax,16
        GetDW   mov     edx,???Texture32
        add     eax,[ebp+4*ecx]
        GetDW   mov     esi,???AlphaMixTable
        mov     cl,[edi]
        and     ebx,1
        mov     ch, B_P [edx+eax]
        mov     bl, B_P [esi+ecx]
        mov     B_P [edi],bl

        jz     @@Dr1P

        fild    [esi].vzr       ;vzl
        fild    [esi].uzr       ;uzl vzl
        fild    [esi].izr       ;1zl uzl vzl
        fdivr   [D_Fix1616]     ;zl  uzl vzl
        fmul    st(1),st        ;zl  u vzl
        fmulp   st(2),st        ;u v
        fxch    st(1)           ;v u
	fistp	[ESP_v0]
        fistp   [ESP_u0]        ;
        ;
        mov     ecx,[ESP_v0]
        mov     eax,[ESP_u0]
        sar     ecx,16
        mov     bl,[edi+1]
        sar     eax,16
        add     eax,[ebp+4*ecx]
        mov     bh, B_P [edx+eax]
        mov     cl, B_P [esi+ebx]
        mov     B_P [edi+1],cl
        ;
	;
@@Dr1P:
	;
	mov	esi,[ESP_startVert]
	mov	edi,[ESP_endVert]
	add	esi,type Screen
        GetDWPub        add     edi,EXT_ScreenWidth16b
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


jmp	END_TYPE_DRAW

;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::;
;
;

unused dd  1024 DUP(?)
;==========ASM extern Labels==============
align 4
extrn END_TYPE_DRAW:proc
extrn ENDD:proc
;==========ASM extern=====================
align 4
extrn _gr_vertices:dword
extrn interVert:dword
extrn _gr_polygon:dword
extrn screen:dword
extrn pYDivCache:dword
extrn p1DivCache:dword
;===========C(++) extern==================
align 4
extrn   _gr_pYCache:dword
extrn	_gr_nScreenWidth:dword
;=============Module Data=================
align 4
_oldFPUCW dw ?
align 4
_newFPUCW dw ?
align 4
_deltaX dd ?
align 4
_deltaTextureWW  dd ?
_deltaTextureW   dd ?
D_One	dd	1.0
D_Fix1616 dd 65536.0


D_1Count32	dd  0.03125 ;1.0/(2^FIX_SHIFT)
D_Count32	dd  32.0 ;(2^FIX_SHIFT)
D_FixCount32 dd 0.00048828125   ;(2^FIX_SHIFT)/65536.0

;D_1Count32	dd  0.030303030303 ;1.0/(2^FIX_SHIFT)
;D_Count32	dd  33.0 ;(2^FIX_SHIFT)
;D_FixCount32 dd 0.0005035400391   ;(2^FIX_SHIFT)/65536.0

END_CODE_SEG
end      