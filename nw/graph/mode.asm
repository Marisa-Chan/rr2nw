include platform.inc
; MODE.ASM
;	Auxiliary assembler routines to tackle mode switches
;	including MCGA X mode (320x240x8bpp).
;	Source of MCGA X mode assembler code and documentation:
;		1. Dr.Dobb's Journal, July 1991 (continued in next issues)
;		2. Russian Dr.Dobb's Journal,  N 3' 1991.

	.386p
	MODEL	FLAT,CPP
	LOCALS

BEGIN_CODE_SEG

GetDW	Macro	oper,reg,var
		oper	reg,0FACEBEDAh
		org		$-4
	var	dd		0FACEBEDAh
			EndM




SC_INDEX	equ		03C4h	;Sequence Controller Index
CRTC_INDEX	equ		03D4h	;CRT Controller Index
ISR1_INDEX	equ		03DAh	;Input status register 1
MISC_OUTPUT	equ		03C2h	;Miscellaneous output register
MOR_READ	equ		03CCh	;Miscellaneous output register read
SCREEN_ADDR	equ		0A0000h	;32-bit address of display memory in mode X

MAP_MASK	equ		02h		;Index in SC of Map Mask register

CRTC_SARH	equ		0Ch		;CRTC Start Address Register high byte
CRTC_SARL	equ		0Dh		;CRTC Start Address Register low byte





;	void GREnableMCGAMode();
;	This procedure forces the VGA display adapter into 320x200x256
;	(13h) mode.

GREnableMCGAMode	Proc

	uses	ebx,esi,edi,ebp

	mov		ax,13h
	int		10h
	IFNDEF _WINNT
		mov		dword ptr _gr_bIsVESA2Mode,0
		mov		dword ptr _gr_nVisualPage,0
	ENDIF
	ret

GREnableMCGAMode	EndP


;	Index/data pairs for CRT Controller registers that differ between
;	mode 13h and mode X

CRTParms	label	word
	dw	00D06h	;vertical total
	dw	03E07h	;overflow (bit 8 of vertical counts)
	dw	04109h;	;cell height (2 to double-scan)
	dw	0EA10h	;v sync start
	dw	0AC11h	;v sync end and protect cr0-cr7
	dw	0DF12h	;vertical displayed
	dw	00014h	;turn off dword mode
	dw	0E715h	;v blank start
	dw	00616h	;v blank end
	dw	0E317h	;turn on byte mode
CRTParmsEnd	label	word
CRT_PARMS_LENGTH 	equ		(((offset CRTParmsEnd)-(offset CRTParms))/2)

;	void GREnableMCGAXMode();
;	This procedure forces the VGA display adapter into 320x240x256
;	mode (mode X).

currentPageAddr		dd	?
currentPageNum		dd	?


GREnableMCGAXMode	 Proc

	uses	ebx,esi,edi,ebp

	mov		ax,13h
	int		10h

	mov		dx,SC_INDEX
	mov		ax,0604h
	out		dx,ax	;disable chain4 mode
	mov		ax,0100h
	out		dx,ax	;synchronous reset while switching clocks

	mov		dx,MISC_OUTPUT
	mov		al,0E3h
	out		dx,al	;select 28 MHz clock & 60 Hz scanning rate

	mov		dx,SC_INDEX
	mov		ax,0300h
	out		dx,ax	;undo reset (restart sequencer)

	mov		dx,CRTC_INDEX	;reprogram the CRT Controller
	mov		al,11h	;VSync End reg contains register write
	out		dx,al	; protect but
	inc		dx		;CRT Controller Data register
	in		al,dx	;get current VSync End register setting
	and		al,7Fh	;remove write protect on various
	out		dx,al	; CRTC registers
	dec		dx		;CRT Controller Index
	cld
	mov		esi,offset CRTParms	;point to CRT parameter table
	mov		cx,CRT_PARMS_LENGTH	;number of table entries
@@SetCRTParmsLoop:
	mov	ax,[esi]	;get the next CRT Index/Data pair
	add	esi,2
	out		dx,ax		;set the next CRT Index/Data pair
	dec	cx
	jnz	@@SetCRTParmsLoop

	mov		dx,SC_INDEX
	mov		ax,0F02h
	out		dx,ax	;enable writes to all four planes
	mov		edi,SCREEN_ADDR		;point ES:DI to display memory
	sub		eax,eax				;clear to zero-value pixels
	mov		ecx,4000h			;number of dwords in display memory
								;clear all of display memory
@@ClearVRAMLoop:
	mov		[edi],eax
	add		edi,4
	dec		ecx
	jnz		@@ClearVRAMLoop

	mov		currentPageNum,0
	mov		currentPageAddr,0A0000h
	IFNDEF _WINNT
		mov		dword ptr _gr_bIsVESA2Mode,0
		mov		dword ptr _gr_nVisualPage,0
	ENDIF

	ret

GREnableMCGAXMode	 EndP


;	void GREnableTextMode();
;	This procedure forces the VGA display adapter into text (3h)
;	mode.

GREnableTextMode	Proc

	uses	ebx,esi,edi,ebp

	mov		ax,3
	int		10h
	IFNDEF _WINNT
		mov		dword ptr _gr_bIsVESA2Mode,0
		mov		dword ptr _gr_nVisualPage,0
	ENDIF
	ret

GREnableTextMode	EndP

; ------------------------------------------------------------------------
DumpXScreenCopyPlan	Macro	?Plan
		Local	?@@0

		mov		ax,MAP_MASK+(100h SHL ?Plan)
		mov		dx,SC_INDEX
		out		dx,ax

		mov		esi,_gr_pScreen
		mov		edi,currentPageAddr
		IF ?Plan GT 0
			add		esi,?Plan
		ENDIF
		mov		ecx,320*240/16
?@@0:	mov		al,ds:[esi+8]
		mov		ah,ds:[esi+12]
		shl		eax,16
		mov		al,ds:[esi]
		mov		ah,ds:[esi+4]
		mov		ds:[edi],eax
		add		esi,16
		add		edi,4
		dec		ecx
		jnz		?@@0

		EndM


		extrn		_gr_pScreen:dword


GRDumpXScreen		Proc
		Uses	esi,edi

		add		currentPageAddr,05000h
		inc		currentPageNum
		cmp		currentPageNum,3
		jb		@@0
		mov		currentPageNum,0
		mov		currentPageAddr,0A0000h
@@0:
		DumpXScreenCopyPlan	0
		DumpXScreenCopyPlan	1
		DumpXScreenCopyPlan	2
		DumpXScreenCopyPlan	3

		IF	0		; Wait retrace
			mov		edx,ISR1_INDEX
			@@10:
				in		al,dx
				test	al,8
				jnz		@@10
			@@11:
				in		al,dx
				test	al,8
				jz		@@11
		ENDIF

		mov		ecx,currentPageAddr
		mov		edx,CRTC_INDEX

		mov		ah,ch
		mov		al,CRTC_SARH
		out		dx,ax

		mov		ah,cl
		mov		al,CRTC_SARL
		out		dx,ax

		ret

GRDumpXScreen		EndP


GRSetPalette		Proc	palette:dword
		Uses	ebx

		mov		eax,1012h
		mov		ebx,0
		mov		ecx,256
		mov		edx,palette
		push	esi
		push	edi
		push	ebp
		int		10h
		pop		ebp
		pop		edi
		pop		esi
		ret

GRSetPalette		EndP

	.486p

GRDumpBlurredScreen	Proc src:dword,dst:dword,tbl:dword
		Uses	esi,edi,ebx

		mov		esi,src
		mov		edi,dst
		mov		ebx,tbl
		xor		eax,eax
		push	ebp
		mov		ebp,320*200/4
@@0:
		mov	edx,ds:[esi]
		mov	ecx,ds:[edi]
;
			mov	ah,dl
			mov	al,cl
			mov	cl , ds:[ebx+eax]
;
			mov	ah,dh
			mov	al,ch
			mov	ch , ds:[ebx+eax]
;
			bswap	ecx
			bswap	edx
;
			mov	ah,dl
			mov	al,cl
			mov	cl , ds:[ebx+eax]
;
			mov	ah,dh
			mov	al,ch
			mov	ch , ds:[ebx+eax]
;
			bswap	ecx
;
		mov	ds:[edi],ecx
		add	esi,4
		add	edi,4

	dec ebp
	jnz	@@0
	pop ebp
	ret

EndP

;=================================================================
;
GRDrawPanel:
	public GRDrawPanel

DrawPanel  proc panel:dword,xs:dword,ys:dword,transp:dword
	uses	esi,edi,ebx;

	mov	eax,[ys]
	imul	_gr_nScreenWidth
	add	eax,[xs]
	add	eax,_gr_pOrigin

	xor	ecx,ecx
        xor     ebx,ebx
        xor     edx,edx
	mov	esi,offset [panel]
	mov	edi,offset [transp]
        push    ebp
@@Loop:
	mov	bl,byte ptr [esi]
	mov	cx,word ptr [esi+1]
	or	ecx,ecx
	jz	@@End
	;shl	ebx,2
	add	esi,3
	jmp     [@@switch][ebx*4]

@@caseSp:
	add	eax,ecx
	jmp	@@Loop
@@caseTr:
        mov     dh,byte ptr [esi]
        mov     dl, byte ptr [eax]
	mov	dl,byte ptr [edx+edi]
	inc	esi
	mov	byte ptr [eax],dl
	inc	eax
	dec	ecx
	jnz	@@caseTr
	jmp	@@Loop
@@caseFl:
	mov	ebx,ecx
	shr	ebx,2
	jz	@@FlByte
@@FlDWord:
	mov	ebp,[esi]
	add	esi,4
	mov	[eax],ebp
	add	eax,4
	dec	ebx
	jnz	@@FlDWord
	and	ecx,3
	jz	@@FlEnd
@@FlByte:
	mov	dl,byte ptr [esi]
	inc	esi
	mov	byte ptr [eax],dl
	inc	eax
	dec	ecx
	jnz	@@FlByte
@@FlEnd:
        xor     ebx,ebx
	jmp	@@Loop
@@End:
	pop ebp
	ret

@@switch dd @@caseSp,@@caseTr,@@caseFl
	endp

; ------------------------------------------------------

GRPrintString:
	public GRPrintString

PrintString  proc Str:dword,xs:dword,ys:dword,FontD:dword,FontH:dword,SmTable:dword,Font:dword
	uses	esi,edi,ebx;

	mov	eax,[ys]
	imul	_gr_nScreenWidth
	add	eax,[xs]
	add	eax,_gr_pOrigin

	mov	esi,offset [Str]
	dec	esi
@@5:
	inc	esi
	movzx	edx,byte ptr [esi]
	shl	edx,3
	jz	@@End
	mov	ecx,[FontH]
	mov	edi,offset [SmTable]
	mov	ebx,[edi+edx]
	or	ebx,ebx
	jz	@@5
	mov	edx,[edi+edx+4]
	add	edx,offset [Font]
	mov	edi,eax
@@4:
	mov	bh,bl
@@3:
	mov	ch,byte ptr [edx]
	test	ch,0f0h
	jnz	@@2
	mov	byte ptr [edi],ch
@@2:
	inc	edx
	inc	edi
	dec	bh
	jnz	@@3
	add	edx,[FontD]
	sub	edx,ebx
	add	edi,_gr_nScreenWidth
	sub	edi,ebx
	dec	cl
	jnz	@@4
	add	eax,ebx
	jmp	@@5
@@End:
	ret
;
	endp
;
;=================================================================
;
GRCPrintString:
	public GRCPrintString

ClippPrintString  proc Str:dword,xs:dword,ys:dword,FontD:dword,FontH:dword,SmTable:dword,Font:dword
	local	xSm:dword,yTop:dword;,Scr1,ShL1,ShL2,ShL3
	;local	ShM1,ShM2,ShM3,ShR1,ShR2,ShR3
	uses	esi,edi,ebx;

	mov	[yTop],0
	mov	eax,_gr_clipRect._top
	mov	ecx,eax
	sub	eax,[ys]
	jle	@@NoTop
	sub	[FontH],eax
	jle	@@End
	imul	[FontD]
	mov	[yTop],eax
	mov	[ys],ecx
	jmp	@@LeftOts
@@NoTop:
	mov	eax,[ys]
	add	eax,[FontH]
	sub	eax,_gr_clipRect._bottom
	jle	@@LeftOts
	sub	[FontH],eax
	jl	@@End
@@LeftOts:
	mov	esi,[Str]
	mov	eax,[ys]
	imul	_gr_nScreenWidth
	add	eax,_gr_pOrigin
	mov	ds:[Scr1],eax
	mov	ecx,[xs]
	mov	[xSm],ecx
	mov	edx,_gr_clipRect._left
	cmp     ecx,edx
	jge	@@Midd
@@LoopLeft:
	movzx	ebx,byte ptr [esi]
	shl	ebx,3
	jz	@@End
	add	ebx,[SmTable]
	mov	edi,[ebx]
	or	edi,edi
	jz	@@NextChL
	add	ecx,edi
	cmp	ecx,edx
	jg	@@DrawLeft
@@NextChL:
	inc	esi
	jmp	@@LoopLeft
@@DrawLeft:
	mov	[xSm],ecx
	sub	ecx,edx
	add	eax,edx
	mov	edx,[ebx]
	sub	edx,ecx
	mov	edi,[ebx+4]
	add	edi,[Font]
	add	edi,edx
	add	edi,[yTop]
	mov	ds:[ShL1],ecx
	mov	ds:[ShL2],ecx
	mov	ds:[ShL3],ecx
	mov	ebx,[FontH]
	jmp	@@LoopVL
@@LoopVnL:
	GetDW	mov	ecx,ShL1
@@LoopVL:
	mov	dl,byte ptr [edi]
	test	dl,0f0h
	jnz	@@NoDrawL
	mov	byte ptr [eax],dl
@@NoDrawL:
	inc	edi
	inc	eax
	dec	ecx
	jnz	@@LoopVL
	add	eax,_gr_nScreenWidth
	GetDW	sub	eax,ShL2
	add	edi,[FontD]
	GetDW	sub	edi,ShL3
	dec	ebx
	jnz	@@LoopVnL
	inc	esi
@@Midd:
	GetDW	mov	eax,Scr1
	mov	edx,[xSm]
	add	eax,edx
	movzx	ebx,byte ptr [esi]
	shl	ebx,3
	jz	@@End
	add	ebx,[SmTable]
	mov	ecx,[ebx]
	or	ecx,ecx
	jz	@@NextChM
	cmp	edx,_gr_clipRect._right
	jge	@@End
	add	edx,ecx
	cmp	edx,_gr_clipRect._right
	jg	@@Right
	mov	[xSm],edx
	mov	edi,[ebx+4]
	add	edi,[Font]
	add	edi,[yTop]
	mov	ds:[ShM1],ecx
	mov	ds:[ShM2],ecx
	mov	ds:[ShM3],ecx
	mov	ebx,[FontH]
	jmp	@@LoopVM
@@LoopVnM:
	GetDW	mov	ecx,ShM1
@@LoopVM:
	mov	dl,byte ptr [edi]
	test	dl,0f0h
	jnz	@@NoDrawM
	mov	byte ptr [eax],dl
@@NoDrawM:
	inc	edi
	inc	eax
	dec	ecx
	jnz	@@LoopVM
	add	eax,_gr_nScreenWidth
	GetDW	sub	eax,ShM2
	add	edi,[FontD]
	GetDW	sub	edi,ShM3
	dec	ebx
	jnz	@@LoopVnM
@@NextChM:
	inc	esi
	jmp	@@Midd
@@Right:
	sub	edx,_gr_clipRect._right
	sub	ecx,edx
	mov	edi,[ebx+4]
	add	edi,[Font]
	add	edi,[yTop]
	mov	ds:[ShR1],ecx
	mov	ds:[ShR2],ecx
	mov	ds:[ShR3],ecx
	mov	ebx,[FontH]
	jmp	@@LoopVR
@@LoopVnR:
	GetDW	mov	ecx,ShR1
@@LoopVR:
	mov	dl,byte ptr [edi]
	test	dl,0f0h
	jnz	@@NoDrawR
	mov	byte ptr [eax],dl
@@NoDrawR:
	inc	edi
	inc	eax
	dec	ecx
	jnz	@@LoopVR
	add	eax,_gr_nScreenWidth
	GetDW	sub	eax,ShR2
	add	edi,[FontD]
	GetDW	sub	edi,ShR3
	dec	ebx
	jnz	@@LoopVnR
@@End:
	ret
;
	endp
;
;=================================================================
;
GRCPrintStringColor:
	public GRCPrintStringColor

ClippPrintStringColor  proc Str:dword,xs:dword,ys:dword,FontD:dword,FontH:dword,SmTable:dword,Font:dword,color:dword
	local	xSm:dword,yTop:dword;,Scr1,ShL1,ShL2,ShL3
	;local	ShM1,ShM2,ShM3,ShR1,ShR2,ShR3
	uses	esi,edi,ebx;

	mov	[yTop],0
	mov	eax,_gr_clipRect._top
	mov	ecx,eax
	sub	eax,[ys]
	jle	@@NoTop
	sub	[FontH],eax
	jle	@@End
	imul	[FontD]
	mov	[yTop],eax
	mov	[ys],ecx
	jmp	@@LeftOts
@@NoTop:
	mov	eax,[ys]
	add	eax,[FontH]
	sub	eax,_gr_clipRect._bottom
	jle	@@LeftOts
	sub	[FontH],eax
	jle	@@End
@@LeftOts:
	mov	esi,[Str]
	mov	eax,[ys]
	imul	_gr_nScreenWidth
	add	eax,_gr_pOrigin
	mov	ds:[Scr11],eax
	mov	ecx,[xs]
	mov	[xSm],ecx
	mov	edx,_gr_clipRect._left
	cmp     ecx,edx
	jge	@@Midd
@@LoopLeft:
	movzx	ebx,byte ptr [esi]
	shl	ebx,3
	jz	@@End
	add	ebx,[SmTable]
	mov	edi,[ebx]
	or	edi,edi
	jz	@@NextChL
	add	ecx,edi
	cmp	ecx,edx
	jg	@@DrawLeft
@@NextChL:
	inc	esi
	jmp	@@LoopLeft
@@DrawLeft:
	mov	[xSm],ecx
	sub	ecx,edx
	add	eax,edx
	mov	edx,[ebx]
	sub	edx,ecx
	mov	edi,[ebx+4]
	add	edi,[Font]
	add	edi,edx
	add	edi,[yTop]
	mov	ds:[ShL11],ecx
	mov	ds:[ShL21],ecx
	mov	ds:[ShL31],ecx
	mov	ebx,[FontH]
	jmp	@@LoopVL
@@LoopVnL:
	GetDW	mov	ecx,ShL11
@@LoopVL:
	mov	dl,byte ptr [edi]
	test	dl,0f0h
	jnz	@@NoDrawL
	mov	edx,[color]
	mov	byte ptr [eax],dl
@@NoDrawL:
	inc	edi
	inc	eax
	dec	ecx
	jnz	@@LoopVL
	add	eax,_gr_nScreenWidth
	GetDW	sub	eax,ShL21
	add	edi,[FontD]
	GetDW	sub	edi,ShL31
	dec	ebx
	jnz	@@LoopVnL
	inc	esi
@@Midd:
	GetDW	mov	eax,Scr11
	mov	edx,[xSm]
	add	eax,edx
	movzx	ebx,byte ptr [esi]
	shl	ebx,3
	jz	@@End
	add	ebx,[SmTable]
	mov	ecx,[ebx]
	or	ecx,ecx
	jz	@@NextChM
	cmp	edx,_gr_clipRect._right
	jge	@@End
	add	edx,ecx
	cmp	edx,_gr_clipRect._right
	jg	@@Right
	mov	[xSm],edx
	mov	edi,[ebx+4]
	add	edi,[Font]
	add	edi,[yTop]
	mov	ds:[ShM11],ecx
	mov	ds:[ShM21],ecx
	mov	ds:[ShM31],ecx
	mov	ebx,[FontH]
	jmp	@@LoopVM
@@LoopVnM:
	GetDW	mov	ecx,ShM11
@@LoopVM:
	mov	dl,byte ptr [edi]
	test	dl,0f0h
	jnz	@@NoDrawM
	mov	edx,[color]
	mov	byte ptr [eax],dl
@@NoDrawM:
	inc	edi
	inc	eax
	dec	ecx
	jnz	@@LoopVM
	add	eax,_gr_nScreenWidth
	GetDW	sub	eax,ShM21
	add	edi,[FontD]
	GetDW	sub	edi,ShM31
	dec	ebx
	jnz	@@LoopVnM
@@NextChM:
	inc	esi
	jmp	@@Midd
@@Right:
	sub	edx,_gr_clipRect._right
	sub	ecx,edx
	mov	edi,[ebx+4]
	add	edi,[Font]
	add	edi,[yTop]
	mov	ds:[ShR11],ecx
	mov	ds:[ShR21],ecx
	mov	ds:[ShR31],ecx
	mov	ebx,[FontH]
	jmp	@@LoopVR
@@LoopVnR:
	GetDW	mov	ecx,ShR11
@@LoopVR:
	mov	dl,byte ptr [edi]
	test	dl,0f0h
	jnz	@@NoDrawR
	mov	edx,[color]
	mov	byte ptr [eax],dl
@@NoDrawR:
	inc	edi
	inc	eax
	dec	ecx
	jnz	@@LoopVR
	add	eax,_gr_nScreenWidth
	GetDW	sub	eax,ShR21
	add	edi,[FontD]
	GetDW	sub	edi,ShR31
	dec	ebx
	jnz	@@LoopVnR
@@End:
	ret
;
	endp



;local	rew1:dword,rew2:dword,ff1:dword,ff2:dword
	Clipping	struc
		_left	dd ?
		_top	dd ?
		_right	dd ?
		_bottom	dd ?

	Clipping	ends

align	4
extrn	_gr_clipRect:Clipping


; ------------------------------------------------------

extrn  _gr_pOrigin:dword
extrn  _gr_nScreenWidth:dword
IFNDEF _WINNT
	extrn  _gr_bIsVESA2Mode:dword
	extrn  _gr_nVisualPage:dword
ENDIF

	public	GREnableMCGAMode
	public	GREnableMCGAXMode
	public	GREnableTextMode
	public	GRDumpXScreen
	public	GRSetPalette
	public	GRDumpBlurredScreen


END_CODE_SEG

	End
