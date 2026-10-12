.486p
MODEL	FLAT,CPP
LOCALS

include platform.inc

BEGIN_CODE_SEG
; align	4
;========================================================================

include types.inc
include stack.inc
include get_s.inc
;========================================================================


HazePresShift   equ	16
HazePres	equ	(1 SHL HazePresShift)
HazeInitZShift  equ	(HazePresShift - 16)

HazeLen equ 11
HazeMin equ 2
HazeColorCount  equ 16
HazeStartG	equ HazePres/HazeMin
HazeEndL	equ HazePres/(HazeMin+HazeLen)
HazedZ		equ (HazeMin*(HazeMin+HazeLen)*HazeColorCount)/(HazeLen)
;HazedZ		equ ((HazePres*HazeColorCount)/(HazeStartG - HazeEndL))
HazeColordZ	equ ((HazeColorCount)*HazePres)
;HazeTableSize	equ 8

;

;
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
align 8
ASM_GRSetZPrecision:
        Public ASM_GRSetZPrecision
SetZp proc zPrec:dword
        uses ebx

        mov     ecx,[zPrec]
        mov     eax,1
        mov     ebx,ecx
        mov     edx,ecx
        add     ecx,16
        fld1
        cmp     ebx,0
        jge     @@ZPrGt0
        xor     ebx,ebx
        neg     edx
        jmp     @@Ok
@@ZPrGt0:
        xor     edx,edx
@@Ok:
        fild    [_ZPres]
        shl     eax,cl
        mov     B_P ds:[?ZShiftRight],bl
        mov     B_P ds:[??ZShiftRight],bl
        mov     [_ZPres], eax
        ;
        fdivp   st(1),st
        mov     B_P ds:[?ZShiftLeft],dl
        mov     B_P ds:[??ZShiftLeft],dl

        fstp    [_iZPres]

        ret
endP

;

;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
ASM_GRSetHaze:
        Public ASM_GRSetHaze
SetHaze proc start:dword,len:dword,hazeDescr:dword
	uses	esi,edi,ebx

	mov	eax,HazePres
	mov	ebx,[start]
	cdq
	idiv	ebx
	mov	ds:[?HazeStartG],eax
	;
	mov	eax,HazePres
	mov	ecx,[len]
	mov	esi,ebx
	cdq
	add	esi,ecx
        ;
        mov     edi,[hazeDescr]
        ;
        idiv    esi
	mov	ds:[?HazeEndL],eax
	mov	B_P ds:[?HazePresShift],HazePresShift
	;mov	B_P ds:[??HazePresShift],HazePresShift
	;mov	B_P ds:[???HazePresShift],HazePresShift
	;
	mov	eax,HazeColorCount
	imul	eax,HazePres
	mov	ds:[?HazeColordZ],eax
	mov	ds:[??HazeColordZ],eax
	;
	mov	eax,ebx
	imul	eax,HazeColorCount
	imul    esi
	idiv	ecx
        ;
        mov     ebx,[edi]
        ;
	mov	ds:[?HazedZ],eax
	mov	ds:[??HazedZ],eax
	mov	ds:[???HazedZ],eax
	mov	ds:[????HazedZ],eax
	mov	ds:[?????HazedZ],eax
	mov	ds:[??????HazedZ],eax
	mov	ds:[???????HazedZ],eax
	mov	ds:[????????HazedZ],eax
	;

        mov     _gr_pHaze,ebx

        ret
endP

;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
align 8
ADrawHaze:
	Public ADrawHaze

	;jmp	END_DRAW_HAZE

	mov	ebp,[ESP_topVert]
	mov	eax,[_gr_polygon].textp
	mov	edx,[ESP_bottomVert]
	mov	ecx,_gr_pYCache
	mov	ebx,[ebp].y
	mov	esi,offset screen
	mov	ebp,[edx].y
	mov	edi,[ecx + ebx*4]	;scrPtr
	sub	ebp,ebx			;deltaY

	mov	[ESP_endVert],edi
	mov	[ESP_startVert],esi
	mov	[ESP_count],ebp


@@HazeLoopY:
        mov     eax,[esi].xl
        mov     ebx,[esi].xr
	add	edi,eax
	mov	[ESP_scr],edi

	sub	ebx,eax
	jl	@@End
	mov	[ESP_D_B],ebx
	mov	eax,ebx
	;inc	ebx
        ;cmp     ebx,1
        mov     ebx,[esi].izl
        GetDWPub   mov     edi,?HazeEndL
	;-----------------
        GetDBPub   sar     ebx,?ZShiftRight
	;-----------------
        mov     ecx,[esi].izr
        ;-----------------
        GetDBPub   sal     ebx,?ZShiftLeft
	;-----------------
        ;sal    ebx,HazeInitZShift
	;sal	ecx,HazeInitZShift
        GetDWPub   mov     esi,?HazeStartG
	;-----------------
        GetDB   sar     ecx,??ZShiftRight
	;-----------------
        ;-----------------
        GetDB   sal     ecx,??ZShiftLeft
	;-----------------
        ;jz     @@dOnePixel
	;cmp	[deltaX],0
	;-----------------
        or      eax,eax
	;------------------
        jz      @@dOneTwoPixels
	cmp	ecx,ebx
	jg	@@izGr
	jl	@@izLs
	cmp	ebx,esi
	jge	@@End
	;mov	z,234234
	cmp	ebx,edi
	jle	@@dHazeMaxZ
	;
	sub	ebx,edi
	mov	esi,_gr_pHaze
	GetDW	imul	ebx,?HazedZ
	GetDB	sar	ebx,?HazePresShift
	mov	edx,[ESP_scr]
	sal	ebx,8	;HazeTableSize
	mov	eax,[ESP_D_B]
	add	esi,ebx
	xor	ecx,ecx
	;
	;eax - deltaX
	;edx - screen
	;esi - hazeTable
@@dHazeOneZ:
        mov     cl,B_P [edx]
	inc	edx
	mov	bh,B_P [esi + ecx]
	dec	eax
	mov	B_P [edx - 1],bh
	jge	@@dHazeOneZ
	;
	jmp	@@End

	;
	;
	;	for one or/and two pixels
align 4
@@dOneTwoPixels:
	;       for one pixel
	cmp	ebx,esi
	jge	@@End
        sub     ebx,edi
	mov	esi,[ESP_scr]
	mov	edi,_gr_pHaze
	jg	@@dOneWScr
	mov	ah,B_P [edi]
	mov	B_P [esi],ah
	;
	jmp	@@End
	;
align 4
@@dOneWScr:
        ;sub     ebx,HazeEndL
        xor     eax,eax
        GetDW	imul    ebx,??HazedZ
        sar     ebx,16
        sal     ebx,8
        mov     al,B_P [esi]
        add     edi,ebx
	mov	ch,B_P [edi + eax]
	mov	B_P [esi],ch
	;
	jmp	@@End
	;
	;
	;
align 4
@@izLs:
	;ebx - izl
	;ecx - izr
	;esi - hStart
	;edi - hEnd
	cmp	ecx,esi
	jge	@@End	;    l-r  |++++++++++|
	cmp	ebx,edi
	jle	@@dHazeMaxZ ;     |++++++++++| l--r

	mov	eax,ecx
	GetDW	mov	edx,???HazedZ
	sub	eax,ebx
	imul	edx
	idiv	[ESP_D_B]

	GetDW	mov	edx,?HazeColordZ
	cmp	ebx,esi
	jl	@@LsCase1
	;		      l---|++++++++++|  r-?
	sub	ebx,edi
	GetDW	imul	ebx,????HazedZ
	cmp	ecx,edi
	jle	@@LsCase2
	;		      l---|----r+++++|
	;eax - B
	;ebx - A
	mov	esi,eax
	mov	eax,edx
	mov	[ESP_D_F],0
	sub	eax,ebx
	mov	edi,[ESP_scr]
	add	eax,esi
	mov	ecx,[ESP_D_B]
	;inc	eax
	cdq
	idiv	esi	; il = (N-A+B)/B

        cmp     eax,0
        jge     @@1
        mov     eax,0
@@1:
	add 	edi,eax
	sub	ecx,eax
	;
	imul	eax,esi
	add	ebx,eax
	;ebx - A
	;ecx - deltaX
	;esi - B
	;edi - scr
	;
	jmp	@@LsDraw
align 4
@@LsCase2:
	;		      l---|----------|--r
	mov	esi,eax
	mov	eax,ebx
	mov	edi,edx
	neg	eax
	cdq
	mov	ecx,[ESP_D_B]
	idiv	esi	;ir = -A/B

        cmp     eax,0
        jge     @@3
        mov     eax,0
@@3:
	;mov	edi,[scr]
	;mov	edx,ecx
	;sub	edx,eax
	sub	ecx,eax
	mov	[ESP_D_F],ecx
	mov	ecx,eax
	;
	;
	;mov	esi,eax
	mov	eax,edi
	;mov	[Ost],0
	sub	eax,ebx
	mov	edi,[ESP_scr]
	add	eax,esi
	;mov	ecx,[deltaX]
	;inc	eax
	cdq
	idiv	esi	; il = (N-A+B)/B

        cmp     eax,0
        jge     @@2
        mov     eax,0
@@2:
	add 	edi,eax
	sub	ecx,eax
	;
	imul	eax,esi
	add	ebx,eax
	jmp	@@LsDraw
align 4
@@LsCase1:
	sub	ebx,edi
	GetDW	imul	ebx,?????HazedZ
	cmp	ecx,edi
	jle	@@LsCase3
	;			  |+++l---r++|
	mov	[ESP_D_F],0
	mov	edi,[ESP_scr]
	mov	esi,eax
	mov	ecx,[ESP_D_B]
	jmp	@@LsDraw
align 4
@@LsCase3:
	;			  |+++l------|---r
	mov	esi,eax
	mov	eax,ebx
	neg	eax
	cdq
	idiv	esi	;ir = -A/B

        cmp     eax,0
        jge     @@4
        mov     eax,0
@@4:
        mov     ecx,[ESP_D_B]
	mov	edi,[ESP_scr]
	sub	ecx,eax
	mov	[ESP_D_F],ecx
	;mov	edx,ecx
	;sub	edx,eax
	mov	ecx,eax


@@LsDraw:
	;ebx - A
	;ecx - deltaX
	;esi - B
	;edi - scr
	;			  |++l----r++|
	mov	ebp,_gr_pHaze
	;push	ebp
	xor	eax,eax	;V-Pipe!

	;ebx - 1/z
	;esi - deltaZ
	;edi - screen
	;ecx - deltaX
	;edx - hazeTable
@@LsLoop:
	mov	edx,ebx
	add	ebx,esi
	;GetDB	sar	ebp,??HazePresShift
	;jl      @@LsOstLoop
        mov     dh,B_P [edi]
	;sal	ebp,8	;HazeTableSize
	inc	edi
	;add	ebp,eax
        ;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
        and     ebx,0FFFFFh
        ;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        sar     edx,8
        mov     ah,B_P [edx + ebp]
	dec	ecx
        mov     B_P [edi - 1],ah
	jge	@@LsLoop
	;
	;pop	ebp
	;ret

	mov	ecx,[ESP_D_F]
	cmp	ecx,0
	jle	@@End
	;


	;mov	ah,B_P [edx] Error!!!!!!!!!!!!!!!!!!!!!1
        mov     ah,B_P [ebp]
	;
@@LsOstLoop:
	mov	B_P [edi],ah
	inc	edi
	dec	ecx
	jg	@@LsOstLoop
	;pop	ebp
	;
	jmp	@@End
	;
	;
align 4
@@izGr:
	;ebx - izl
	;ecx - izr
	;esi - hStart
	;edi - hEnd
	cmp	ebx,esi
	jge	@@End	;    r-l  |++++++++++|
	cmp	ecx,edi
	jle	@@dHazeMaxZ ;     |++++++++++| r--l

	mov	eax,ecx
	GetDW	mov	edx,??????HazedZ
	sub	eax,ebx
	imul	edx
	idiv	[ESP_D_B]

	GetDW	mov	edx,??HazeColordZ
	cmp	ebx,edi
	jg	@@GrCase1
	;		          |++++++++++|---l  r-?
	sub	ebx,edi
	GetDW	imul	ebx,???????HazedZ
	cmp	ecx,esi
	jge	@@GrCase2
	;		          |++++r-----|---l
	;eax - B
	;ebx - A
	mov	esi,eax
	;mov	eax,eax
	;mov	[OstX],0
	sub	eax,ebx
	mov	edi,[ESP_scr]
	dec	eax
	mov	ecx,[ESP_D_B]
	;inc	eax
	cdq
	idiv	esi	; il = (-A+B-1)/B

        cmp     eax,0
        jge     @@5
        mov     eax,0
@@5:
	;add 	edi,eax
	mov	edx,_gr_pHaze
	sub	ecx,eax
        mov     dh,[edx]

@@GrBeginLoop:
	mov	B_P [edi],dh
	inc	edi
	add	ebx,esi
	dec	eax
	jg      @@GrBeginLoop
	;
	;imul	eax,esi
	;add	ebx,eax
	;ebx - A
	;ecx - deltaX
	;esi - B
	;edi - scr
	;
	jmp	@@GrDraw
align 4
@@GrCase2:
	;		      r---|----------|--l
	mov	esi,eax
	mov	eax,edx
	sub	eax,ebx
	dec	eax
	;mov	edi,edx
	;neg	eax
	cdq
	idiv	esi	;ir = (N-A-1)/B

        cmp     eax,0
        jge     @@6
        mov     eax,0
@@6:
	;mov	ecx,[deltaX]
	;mov	edi,[scr]
	;mov	edx,ecx
	;sub	edx,eax
	;mov	[OstX],edx
	mov	ecx,eax	;correct deltaX
	;
	;
	;mov	esi,eax
	mov	eax,esi
	;mov	[Ost],0
	sub	eax,ebx
	mov	edi,[ESP_scr]
	dec	eax
	;mov	ecx,[deltaX]
	;inc	eax
	cdq
	idiv	esi	; il = (-A+B-1)/B

        cmp     eax,0
        jge     @@7
        mov     eax,0
@@7:
        mov     edx,_gr_pHaze
	sub	ecx,eax
        mov     dh,B_P [edx]

@@GrBeginLoop1:
	mov	B_P [edi],dh
	inc	edi
	add	ebx,esi
	dec	eax
	jg      @@GrBeginLoop1
	;add 	edi,eax
	;sub	ecx,eax
	;
	;imul	eax,esi
	;add	ebx,eax
	jmp	@@GrDraw
align 4
@@GrCase1:
	sub	ebx,edi
	GetDW	imul	ebx,????????HazedZ
	cmp	ecx,esi
	jge	@@GrCase3
	;			  |+++r---l++|
	;mov	[OstX],0
	mov	edi,[ESP_scr]
	mov	esi,eax
	mov	ecx,[ESP_D_B]
	jmp	@@GrDraw
align 4
@@GrCase3:
	;		      r---|-------l++|
	mov	esi,eax
	mov	eax,edx
	sub	eax,ebx
	dec	eax
	cdq
	idiv	esi	;ir = (N-A-1)/B

        cmp     eax,0
        jge     @@8
        mov     eax,0
@@8:
	;mov	ecx,[deltaX]
	mov	edi,[ESP_scr]
	;mov	edx,ecx
	;sub	edx,eax
	;mov	[OstX],edx
	mov	ecx,eax


@@GrDraw:
	;ebx - A
	;ecx - deltaX
	;esi - B
	;edi - scr
	;			  |++l----r++|
	mov	ebp,_gr_pHaze
	;push	ebp
	xor	eax,eax	;V-Pipe!

	;ebx - 1/z
	;esi - deltaZ
	;edi - screen
	;ecx - deltaX
	;edx - hazeTable
@@GrLoop:
        mov     edx,ebx
	add	ebx,esi
	;GetDB	sar	ebp,???HazePresShift
	;jl      @@LsOstLoop
        mov     dh,B_P [edi]
        ;sal    ebp,8   ;HazeTableSize
	inc	edi
	;add	ebp,eax
        ;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
        and     ebx,0FFFFFh
        ;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        sar     edx,8
        mov     ah,B_P [edx + ebp]
	dec	ecx
        mov     B_P [edi - 1],ah
	jge	@@GrLoop
	;
	;pop	ebp
	;
	jmp	@@End

	;
align 4
@@dHazeMaxZ:
	mov	esi,_gr_pHaze
	mov	edi,[ESP_scr]
	mov	ebx,[ESP_D_B]
	mov	ch,B_P [esi]
	;
@@dHazeMaxLoop:
	mov	byte ptr [edi],ch
	inc	edi
	dec	ebx
	jge	@@dHazeMaxLoop
	;

@@End:
	mov	esi,[ESP_startVert]
	mov	edi,[ESP_endVert]
	add	esi,type Screen
	GetDWPub	add	edi,EXT_ScreenWidth8
	dec	[ESP_count]
	mov	[ESP_startVert],esi
	mov	[ESP_endVert],edi
	jge	@@HazeLoopY
	;
	jmp	END_DRAW_HAZE

;
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::;
;
unused dd  1024 DUP(?)
align 4
_ZPres dd ?
public _ZPres
;==========ASM extern Labels==============
align 4
extrn END_DRAW_HAZE:proc
extrn ENDD:proc
;==========ASM extern=====================
align 4
extrn _gr_vertices:dword
extrn interVert:dword
extrn _gr_polygon:dword
extrn screen:dword
extrn pYDivCache:dword
extrn p1DivCache:dword
extrn _iZPres:dword
;===========C(++) extern==================
align 4
extrn   _gr_pYCache:dword
extrn	_gr_nScreenWidth:dword
extrn	_gr_pHaze:dword
;=========================================
align 4
;$tables

END_CODE_SEG
end      