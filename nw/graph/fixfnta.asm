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



ASM_PrintAt:
        public ASM_PrintAt

PrintString  proc Str:dword,xs:dword,ys:dword,FontD:dword,FontH:dword,SmTable:dword,Font:dword
	uses	esi,edi,ebx;

        mov     ecx,_gr_pYCache
        mov     ebx,[ys]
        mov     eax,[xs]

        mov     esi,offset [Str]

        add     eax,[ecx + ebx*4]       ;scrPtr

        ;mov     eax,[ys]
        ;imul    _gr_nScreenWidth
        ;add     eax,[xs]
        ;mov     esi,offset [Str]
        ;add     eax,_gr_pOrigin
        dec     esi
@@5:
        inc     esi
        xor     edx,edx
        mov     dl,byte ptr [esi]
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
        and     ch,ch
        jz     @@2
	mov	byte ptr [edi],ch
@@2:
	inc	edx
	inc	edi
	dec	bh
	jnz	@@3
	add	edx,[FontD]
	sub	edx,ebx
        GetDWPub        add     edi,EXT_ScreenWidth15a
	sub	edi,ebx
	dec	cl
	jnz	@@4
	add	eax,ebx
	jmp	@@5
@@End:
	ret
;
	endp

;======================================================================
;======================================================================
;======================================================================
ASM_PrintColorAt:
        public ASM_PrintColorAt

PrintColorString  proc Str:dword,xs:dword,ys:dword,FontD:dword,FontH:dword,SmTable:dword,Font:dword,colorF:dword
	uses	esi,edi,ebx;

        mov     ecx,_gr_pYCache
        mov     ebx,[ys]
        mov     eax,[xs]

        mov     esi,offset [Str]
        mov     edx,[colorF]

        add     eax,[ecx + ebx*4]       ;scrPtr

        ;mov     eax,[ys]
        ;imul    _gr_nScreenWidth
        ;add     eax,[xs]
        ;mov     esi,offset [Str]
        ;add     eax,_gr_pOrigin

        dec     esi
        mov     ds:[?Color],dl
@@5:
        inc     esi
        xor     edx,edx
        mov     dl,byte ptr [esi]
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
        and     ch,ch
        jz      @@2
        mov     byte ptr [edi],0FFh
                org             $-1
                ?Color     db              0FFh

@@2:
	inc	edx
	inc	edi
	dec	bh
	jnz	@@3
	add	edx,[FontD]
	sub	edx,ebx
        GetDWPub        add     edi,EXT_ScreenWidth15b
	sub	edi,ebx
	dec	cl
	jnz	@@4
	add	eax,ebx
	jmp	@@5
@@End:
	ret
;
	endp

;======================================================================
;======================================================================
;======================================================================
ASM_PrintClipAt:
        public ASM_PrintClipAt

ClippPrintString  proc Str:dword,xs:dword,ys:dword,FontD:dword,FontH:dword,SmTable:dword,Font:dword
	local	xSm:dword,yTop:dword;,Scr1,ShL1,ShL2,ShL3
	;local	ShM1,ShM2,ShM3,ShR1,ShR2,ShR3
	uses	esi,edi,ebx;

	mov	[yTop],0
        mov     eax,_gr_clipRect.top
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
        sub     eax,_gr_clipRect.bottom
	jle	@@LeftOts
	sub	[FontH],eax
	jl	@@End
@@LeftOts:
	mov	esi,[Str]
	mov	eax,[ys]
        mov     ecx,_gr_pYCache
        mov     eax,[ecx+eax*4]
	mov	ds:[Scr1],eax
	mov	ecx,[xs]
	mov	[xSm],ecx
        mov     edx,_gr_clipRect.left
	cmp     ecx,edx
	jge	@@Midd
@@LoopLeft:
        xor     ebx,ebx
        mov     bl,byte ptr [esi]
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
        and     dl,dl
        jz      @@NoDrawL
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
        xor     ebx,ebx
	add	eax,edx
        mov     bl,byte ptr [esi]
	shl	ebx,3
	jz	@@End
	add	ebx,[SmTable]
	mov	ecx,[ebx]
	or	ecx,ecx
	jz	@@NextChM
        cmp     edx,_gr_clipRect.right
	jge	@@End
	add	edx,ecx
        cmp     edx,_gr_clipRect.right
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
        and     dl,dl
        jz      @@NoDrawM
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
        sub     edx,_gr_clipRect.right
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
        and     dl,dl
        jz      @@NoDrawR
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

;======================================================================
;======================================================================
;======================================================================
ASM_PrintClipColorAt:
        public ASM_PrintClipColorAt

ClippPrintStringColor  proc Str:dword,xs:dword,ys:dword,FontD:dword,FontH:dword,SmTable:dword,Font:dword,colorF:dword
	local	xSm:dword,yTop:dword;,Scr1,ShL1,ShL2,ShL3
	;local	ShM1,ShM2,ShM3,ShR1,ShR2,ShR3
	uses	esi,edi,ebx;

	mov	[yTop],0
        mov     eax,_gr_clipRect.top
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
        sub     eax,_gr_clipRect.bottom
	jle	@@LeftOts
	sub	[FontH],eax
	jle	@@End
@@LeftOts:
	mov	esi,[Str]
	mov	eax,[ys]
        mov     ecx,_gr_pYCache
        mov     eax,[ecx+eax*4]
	mov	ds:[Scr11],eax
	mov	ecx,[xs]
	mov	[xSm],ecx
        mov     edx,_gr_clipRect.left
	cmp     ecx,edx
	jge	@@Midd
@@LoopLeft:
        xor     ebx,ebx
        mov     bl,byte ptr [esi]
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
        and     dl,dl
        jz      @@NoDrawL
        mov     edx,[colorF]
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
        xor     ebx,ebx
	add	eax,edx
        mov     bl,byte ptr [esi]
	shl	ebx,3
	jz	@@End
	add	ebx,[SmTable]
	mov	ecx,[ebx]
	or	ecx,ecx
	jz	@@NextChM
        cmp     edx,_gr_clipRect.right
	jge	@@End
	add	edx,ecx
        cmp     edx,_gr_clipRect.right
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
        and     dl,dl
        jz      @@NoDrawM
        mov     edx,[colorF]
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
        sub     edx,_gr_clipRect.right
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
        and     dl,dl
        jz      @@NoDrawR
        mov     edx,[colorF]
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

unused dd  1024 DUP(?)
;==========ASM extern=====================
;===========C(++) extern==================
align 4
extrn   _gr_pYCache:dword
extrn   _gr_pOrigin:dword
extrn   p1DivCache:dword
extrn	_gr_nScreenWidth:dword
extrn	_gr_clipRect:Clip

extrn pYDivCache:dword

;=============Module Data=================

END_CODE_SEG
end