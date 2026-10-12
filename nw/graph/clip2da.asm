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
;
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
AMClipHorizontal Macro typePol,typeAdd,bLast

	mov	[edi].y,ebx
	push	esi
	push	ebx
	IF	bLast
	mov	eax,[ecx].x
	ELSE
	mov	eax,[ebp + type Vertex].x	;x
	ENDIF
	mov	edx,[ebp].y			;yp
	sub	eax,[ebp].x			;x-xp
	sub	ebx,edx				;yPer - yp
	IF	bLast
	mov	esi,[ecx].y
	ELSE
	mov	esi,[ebp + type Vertex].y	;y
	ENDIF
	sal	eax,10
	sub	esi,edx				;y-yp
	imul	ebx                             ;(x-xp)*(yPer-yp)
	idiv	esi
	sar	eax,10
	add	eax,[ebp].x
	mov	[edi].x,eax
	;
        IF ((typePol EQ TEXTURE_P_TYPE) OR (typePol EQ TEXTURE_ALPHA_TYPE) OR (typePol EQ TEXTURE_P_TRSP_TYPE) OR (typeAdd AND HAZE_ADDTYPE) OR (typeAdd AND BUMP_ADDTYPE))
	IF	bLast
	mov	eax,[ecx].iz
	ELSE
	mov	eax,[ebp + type Vertex].iz
	ENDIF
	sub	eax,[ebp].iz
	imul	ebx
	idiv	esi
	add	eax,[ebp].iz
	mov	[edi].iz,eax
	ENDIF
        ;
        IF (typePol EQ GOURAUD_TYPE)
	IF	bLast
        mov     eax,[ecx].col
	ELSE
        mov     eax,[ebp + type Vertex].col
	ENDIF
        sub     eax,[ebp].col
	imul	ebx
	idiv	esi
        add     eax,[ebp].col
        mov     [edi].col,eax
	ENDIF
        ;
        IF (typePol GT GOURAUD_TYPE)
	IF	bLast
	mov	eax,[ecx].u
	ELSE
	mov	eax,[ebp + type Vertex].u
	ENDIF
	sub	eax,[ebp].u
	imul	ebx
	idiv	esi
	add	eax,[ebp].u
	mov	[edi].u,eax
        ;
	IF	bLast
        mov     eax,[ecx].v
	ELSE
        mov     eax,[ebp + type Vertex].v
	ENDIF
	sub	eax,[ebp].v
	imul	ebx
	idiv	esi
	add	eax,[ebp].v
	mov	[edi].v,eax
	ENDIF
	;
        IF (typeAdd AND BUMP_ADDTYPE)
	IF	bLast
	mov	eax,[ecx].ub
	ELSE
	mov	eax,[ebp + type Vertex].ub
	ENDIF
	sub	eax,[ebp].ub
	imul	ebx
	idiv	esi
	add	eax,[ebp].ub
	mov	[edi].ub,eax
	;
	IF	bLast
	mov	eax,[ecx].vb
	ELSE
	mov	eax,[ebp + type Vertex].vb
	ENDIF
	sub	eax,[ebp].vb
	imul	ebx
	idiv	esi
	add	eax,[ebp].vb
	mov	[edi].vb,eax
	ENDIF
	;
	pop	ebx
	pop	esi
endM

;
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
AMClipVertical Macro typePol,typeAdd,bLast

	mov	[edi].x,ebx
	push	esi
	push	ebx
	IF	bLast
	mov	eax,[ecx].y
	ELSE
	mov	eax,[ebp + type Vertex].y	;y
	ENDIF
	mov	edx,[ebp].x			;xp
	sub	eax,[ebp].y			;y-yp
	sub	ebx,edx				;xPer - xp
	IF	bLast
	mov	esi,[ecx].x
	ELSE
	mov	esi,[ebp + type Vertex].x	;x
	ENDIF
	sal	eax,10
	sub	esi,edx				;x-xp
	imul	ebx                             ;(y-yp)*(xPer-xp)
	idiv	esi
	sar	eax,10
	add	eax,[ebp].y
	mov	[edi].y,eax
	;
        IF ((typePol EQ TEXTURE_P_TYPE) OR (typePol EQ TEXTURE_ALPHA_TYPE) OR (typePol EQ TEXTURE_P_TRSP_TYPE) OR (typeAdd AND HAZE_ADDTYPE) OR (typeAdd AND BUMP_ADDTYPE))
	IF	bLast
	mov	eax,[ecx].iz
	ELSE
	mov	eax,[ebp + type Vertex].iz
	ENDIF
	sub	eax,[ebp].iz
	imul	ebx
	idiv	esi
	add	eax,[ebp].iz
	mov	[edi].iz,eax
	ENDIF
	;
        IF (typePol EQ GOURAUD_TYPE)
	IF	bLast
        mov     eax,[ecx].col
	ELSE
        mov     eax,[ebp + type Vertex].col
	ENDIF
        sub     eax,[ebp].col
	imul	ebx
	idiv	esi
        add     eax,[ebp].col
        mov     [edi].col,eax
	ENDIF
	;
        IF (typePol GT GOURAUD_TYPE)
	IF	bLast
	mov	eax,[ecx].u
	ELSE
	mov	eax,[ebp + type Vertex].u
	ENDIF
	sub	eax,[ebp].u
	imul	ebx
	idiv	esi
	add	eax,[ebp].u
	mov	[edi].u,eax
	;
	IF	bLast
	mov	eax,[ecx].v
	ELSE
	mov	eax,[ebp + type Vertex].v
	ENDIF
	sub	eax,[ebp].v
	imul	ebx
	idiv	esi
	add	eax,[ebp].v
	mov	[edi].v,eax
	ENDIF
	;
        IF (typeAdd AND BUMP_ADDTYPE)
	IF	bLast
	mov	eax,[ecx].ub
	ELSE
	mov	eax,[ebp + type Vertex].ub
	ENDIF
	sub	eax,[ebp].ub
	imul	ebx
	idiv	esi
	add	eax,[ebp].ub
	mov	[edi].ub,eax
	;
	IF	bLast
	mov	eax,[ecx].vb
	ELSE
	mov	eax,[ebp + type Vertex].vb
	ENDIF
	sub	eax,[ebp].vb
	imul	ebx
	idiv	esi
	add	eax,[ebp].vb
	mov	[edi].vb,eax
	ENDIF
	;
	pop	ebx
	pop	esi
endM
;
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
AMCopyVertex Macro typePol,typeAdd
	mov	eax,[ebp].x
	mov	edx,[ebp].y
	mov	[edi].x,eax

        IF ((typePol EQ TEXTURE_P_TYPE) OR (typePol EQ TEXTURE_ALPHA_TYPE) OR (typePol EQ TEXTURE_P_TRSP_TYPE) OR (typeAdd AND HAZE_ADDTYPE) OR (typeAdd AND BUMP_ADDTYPE))
	mov	eax,[ebp].iz
	ENDIF

	mov	[edi].y,edx

        IF ((typePol EQ TEXTURE_P_TYPE) OR (typePol EQ TEXTURE_ALPHA_TYPE) OR (typePol EQ TEXTURE_P_TRSP_TYPE) OR (typeAdd AND HAZE_ADDTYPE) OR (typeAdd AND BUMP_ADDTYPE))
	mov	[edi].iz,eax
	ENDIF

        IF (typePol EQ GOURAUD_TYPE)
        mov     eax,[ebp].col
        mov     [edi].col, eax
        ENDIF

        IF (typePol GT GOURAUD_TYPE)
	mov	eax,[ebp].u
        mov     edx,[ebp].v
	mov	[edi].u,eax
        mov     [edi].v,edx
	ENDIF

	;
	IF (typeAdd EQ BUMP_ADDTYPE)
	mov	eax,[ebp].ub
	mov	edx,[ebp].vb
	mov	[edi].ub,eax
	mov	[edi].vb,edx
	ENDIF
endM
;
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
AMClip2D Macro  typePol,typeAdd
	local	@@TopLoop,@@NoCopyTop,@@EndTopLoop,@@NoCopyTop1,@@EndTop
	local	@@NoTop,@@BottomLoop,@@NoCopyBottom,@@EndBottomLoop
	local	@@NoCopyBottom1,@@EndBottom,@@BottomS,@@NoBottom
	local	@@LeftLoop,@@NoCopyLeft,@@EndLeftLoop,@@NoCopyLeft1,@@EndLeft
	local	@@LeftS,@@NoLeft,@@RightLoop,@@NoCopyRight,@@EndRightLoop
	local	@@NoCopyRight1,@@EndRight,@@RightS,@@NoRight
	local	@@FindTBVertLoop,@@NoTopVert,@@EndFindLoop
	local	?Svertex,?Svertex1,?Dvertex,??Svertex,??Svertex1,??Dvertex
	local	???Svertex,???Dvertex
	;
	;esi - RLBT
	mov	ebp,offset _gr_vertices
	mov	edi,offset interVert
        test    esi,1
	mov	eax,[ESP_vertC]
	jz	@@NoTop

	mov	ebx,_gr_clipRect.top
	cmp	[ebp].y,ebx
	mov	[ESP_num],-1
	setge	ch
	mov	[ESP_count],eax
@@TopLoop:
        and     ch,ch
	jz	@@NoCopyTop

	AMCopyVertex	typePol,typeAdd

	add	edi,type Vertex
	inc	[ESP_num]

@@NoCopyTop:
	cmp	[ebp + type Vertex].y,ebx
	setge	cl
	cmp	ch,cl
	jz	@@EndTopLoop

	AMClipHorizontal	typePol,typeAdd,FALSE

	add	edi,type Vertex
	inc	[ESP_num]

@@EndTopLoop:
	shl     ecx,8
	add	ebp,type Vertex
	dec	[ESP_count]
	jnz	@@TopLoop
	;
        and     ch,ch
	jz	@@NoCopyTop1

	AMCopyVertex	typePol,typeAdd

	add	edi,type Vertex
	inc	[ESP_num]

@@NoCopyTop1:
	cmp	[_gr_vertices].y,ebx
	setge	cl
	cmp	ch,cl
	jz	@@EndTop

	mov	ecx,offset _gr_vertices
	AMClipHorizontal	typePol,typeAdd,TRUE

	inc	[ESP_num]

@@EndTop:
	mov	eax,[ESP_num]
	cmp	eax,2
	jl	ENDD
	mov	ebp,offset interVert
	mov	edi,offset _gr_vertices
	;
	;Bottom Side
	;
@@NoTop:
	test	esi,2
	jz	@@NoBottom

	mov	ds:[?Svertex],ebp
	mov	ebx,_gr_ilyaBottom
	mov	ds:[?Dvertex],edi
	cmp	[ebp].y,ebx
	mov	[ESP_num],-1
	setle	ch
	mov	ds:[?Svertex1],ebp
	mov	[ESP_count],eax
@@BottomLoop:
        and     ch,ch
	jz	@@NoCopyBottom

	AMCopyVertex	typePol,typeAdd

	add	edi,type Vertex
	inc	[ESP_num]

@@NoCopyBottom:
	cmp	[ebp + type Vertex].y,ebx
	setle	cl
	cmp	ch,cl
	jz	@@EndBottomLoop

	AMClipHorizontal	typePol,typeAdd,FALSE

	add	edi,type Vertex
	inc	[ESP_num]

@@EndBottomLoop:
	shl     ecx,8
	add	ebp,type Vertex
	dec	[ESP_count]
	jnz	@@BottomLoop
	;
        and     ch,ch
	jz	@@NoCopyBottom1

	AMCopyVertex	typePol,typeAdd

	add	edi,type Vertex
	inc	[ESP_num]

@@NoCopyBottom1:
	GetDW	mov	edx,?Svertex
	cmp	[edx].y,ebx
	setle	cl
	cmp	ch,cl
	jz	@@EndBottom

	mov	ecx,edx
	AMClipHorizontal	typePol,typeAdd,TRUE

	inc	[ESP_num]

@@EndBottom:
	mov	eax,[ESP_num]
	cmp	eax,2
	jl	ENDD
	GetDW	mov	ebp,?Dvertex
	GetDW	mov	edi,?Svertex1
	;
	;Left Side
	;
@@NoBottom:
	test	esi,4
	jz	@@NoLeft

	mov	ds:[??Svertex],ebp
	mov	ebx,_gr_clipRect.left
	mov	ds:[??Dvertex],edi
	cmp	[ebp].x,ebx
	mov	[ESP_num],-1
	setge	ch
	mov	ds:[??Svertex1],ebp
	mov	[ESP_count],eax
@@LeftLoop:
        and     ch,ch
	jz	@@NoCopyLeft

	AMCopyVertex	typePol,typeAdd

	add	edi,type Vertex
	inc	[ESP_num]

@@NoCopyLeft:
	cmp	[ebp + type Vertex].x,ebx
	setge	cl
	cmp	ch,cl
	jz	@@EndLeftLoop

	AMClipVertical	typePol,typeAdd,FALSE

	add	edi,type Vertex
	inc	[ESP_num]

@@EndLeftLoop:
	shl     ecx,8
	add	ebp,type Vertex
	dec	[ESP_count]
	jnz	@@LeftLoop
	;
        and     ch,ch
	jz	@@NoCopyLeft1

	AMCopyVertex	typePol,typeAdd

	add	edi,type Vertex
	inc	[ESP_num]

@@NoCopyLeft1:
	GetDW	mov	edx,??Svertex
	cmp	[edx].x,ebx
	setge	cl
	cmp	ch,cl
	jz	@@EndLeft

	mov	ecx,edx
	AMClipVertical	typePol,typeAdd,TRUE

	inc	[ESP_num]

@@EndLeft:
	mov	eax,[ESP_num]
	cmp	eax,2
	jl	ENDD
	GetDW	mov	ebp,??Dvertex
	GetDW	mov	edi,??Svertex1
	;
	;Right Side
	;
@@NoLeft:
	test	esi,8
	jz	@@NoRight

	mov	ds:[???Svertex],ebp
	mov	ebx,_gr_ilyaRight
	mov	ds:[???Dvertex],edi
	cmp	[ebp].x,ebx
	mov	[ESP_num],-1
	setle	ch
	mov	[ESP_count],eax
@@RightLoop:
        and     ch,ch
	jz	@@NoCopyRight

	AMCopyVertex	typePol,typeAdd

	add	edi,type Vertex
	inc	[ESP_num]

@@NoCopyRight:
	cmp	[ebp + type Vertex].x,ebx
	setle	cl
	cmp	ch,cl
	jz	@@EndRightLoop

	AMClipVertical	typePol,typeAdd,FALSE

	add	edi,type Vertex
	inc	[ESP_num]

@@EndRightLoop:
	shl     ecx,8
	add	ebp,type Vertex
	dec	[ESP_count]
	jnz	@@RightLoop
	;
        and     ch,ch
	jz	@@NoCopyRight1

	AMCopyVertex	typePol,typeAdd

	add	edi,type Vertex
	inc	[ESP_num]

@@NoCopyRight1:
	GetDW	mov	edx,???Svertex
	cmp	[edx].x,ebx
	setle	cl
	cmp	ch,cl
	jz	@@EndRight

	mov	ecx,edx
	AMClipVertical	typePol,typeAdd,TRUE

	inc	[ESP_num]

@@EndRight:
	mov	eax,[ESP_num]
	cmp	eax,2
	jl	ENDD
	GetDW	mov	ebp,???Dvertex
	;
	;
	;
@@NoRight:
	mov	edx,ebp
	mov	ebx,[ebp].y
	mov	[ESP_startVert],ebp
	mov	esi,ebp
	mov	ecx,ebx
	;ebx,ecx - top & bottom
	;edx,esi - topV & botV
@@FindTBVertLoop:
	add	ebp,type Vertex

	cmp	[ebp].y,ebx
	jge	@@NoTopVert
	mov	ebx,[ebp].y
	dec	eax
	mov	edx,ebp
	jg	@@FindTBVertLoop
	;
	mov	[ESP_endVert],ebp
	mov	[ESP_topVert],edx
	mov	[ESP_bottomVert],esi
	jmp	END_CLIP
	;
@@NoTopVert:
	cmp	[ebp].y,ecx
	jle	@@EndFindLoop
	mov	ecx,[ebp].y
	dec     eax
	mov	esi,ebp
	jg	@@FindTBVertLoop
	;
	mov	[ESP_endVert],ebp
	mov	[ESP_topVert],edx
	mov	[ESP_bottomVert],esi
	jmp	END_CLIP
	;
@@EndFindLoop:
	dec     eax
	jg	@@FindTBVertLoop
	;
	mov	[ESP_endVert],ebp
	mov	[ESP_topVert],edx
	mov	[ESP_bottomVert],esi
	;jmp	END_CLIP

endm
;-------------------------------------------------
;-----------Function on AMClip2D------------------
;-------------------------------------------------
align 8
G_Clip:
public G_Clip
	AMClip2D FLAT_TYPE,NO_ADDTYPE
jmp	END_CLIP
;-------------------------------------------------
align 8
G_ClipZ:
public G_ClipZ
	AMClip2D FLAT_TYPE,HAZE_ADDTYPE
jmp	END_CLIP
;-------------------------------------------------
align 8
G_ClipUV:
public G_ClipUV
	AMClip2D TEXTURE_L_TYPE,NO_ADDTYPE
jmp	END_CLIP
;-------------------------------------------------
align 8
G_ClipZU:
public G_ClipZU
        AMClip2D GOURAUD_TYPE,HAZE_ADDTYPE
jmp	END_CLIP
;-------------------------------------------------
align 8
G_ClipZUV:
public G_ClipZUV
	AMClip2D TEXTURE_L_TYPE,HAZE_ADDTYPE
jmp	END_CLIP
;-------------------------------------------------
;BUMP
align 8
G_ClipB:
public G_ClipB
	AMClip2D FLAT_TYPE,BUMP_ADDTYPE
jmp	END_CLIP
;-------------------------------------------------
align 8
G_ClipUVB:
public G_ClipUVB
	AMClip2D TEXTURE_L_TYPE,BUMP_ADDTYPE
jmp	END_CLIP
;-------------------------------------------------
align 8
G_ClipUB:
public G_ClipUB
	AMClip2D GOURAUD_TYPE,BUMP_ADDTYPE
jmp	END_CLIP
;-------------------------------------------------
;-------------------------------------------------
;-------------------------------------------------
;
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::;
;

AMCalcInvZ Macro typePol,typeAdd
	local @@CalcInvZLoop

	;don't use esi
	;ebp - endVert
	;Vertex - startVert

        IF (typePol NE FLAT_TYPE) AND (typePol NE TRANSPARENT_TYPE)
	mov	ecx,offset _gr_vertices
@@CalcInvZLoop:

	mov	ebx,[ecx].iz
        IF (typePol EQ TEXTURE_P_TYPE) OR (typePol EQ TEXTURE_P_TRSP_TYPE) OR (typePol EQ TEXTURE_ALPHA_TYPE)
	mov	eax,[ecx].u
	imul	ebx
	shrd	eax,edx,16


        mov     [ecx].u,eax
        mov     eax,[ecx].v
        imul    ebx
        shrd    eax,edx,16
        mov     [ecx].v,eax
	ENDIF

        ;IF (typePol EQ GOURAUD_TYPE)
        ;        mov     [ecx].u,eax
        ;ENDIF
        ;ENDIF
	;
        IF (typeAdd AND BUMP_ADDTYPE)
	mov	eax,[ecx].ub
	imul	ebx
	shrd	eax,edx,16
	mov	[ecx].ub,eax
	mov	eax,[ecx].vb
	imul	ebx
	shrd	eax,edx,16
	mov	[ecx].vb,eax
	ENDIF

	add	ecx,type Vertex
	cmp	ecx,ebp
	jle	@@CalcInvZLoop
	;
	ENDIF
endM
;-------------------------------------------------
;-----------Function on AMCalcInvZ----------------
;-------------------------------------------------
;
align 8
G_InvU:
public	G_InvU
	AMCalcInvZ GOURAUD_TYPE,0
jmp	END_CALC_IZ
;
;--------------------------------------------------
;
align 8
G_InvUV:
public	G_InvUV
	AMCalcInvZ TEXTURE_P_TYPE,0
jmp	END_CALC_IZ
;-------------------------------------------------
;
align 8
G_InvB:
public	G_InvB
	AMCalcInvZ GOURAUD_TYPE,BUMP_ADDTYPE
jmp	END_CALC_IZ
;-------------------------------------------------
;
align 8
G_InvUB:
public	G_InvUB
	AMCalcInvZ GOURAUD_TYPE,BUMP_ADDTYPE
jmp	END_CALC_IZ
;-------------------------------------------------
;
align 8
G_InvUVB:
public	G_InvUVB
	AMCalcInvZ TEXTURE_P_TYPE,BUMP_ADDTYPE
jmp	END_CALC_IZ
;-------------------------------------------------
;-------------------------------------------------
;-------------------------------------------------
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::;
;
unused dd  1024 DUP(?)
;==========ASM Data=======================
align 4
_gr_ilyaBottom dd ?
public _gr_ilyaBottom
_gr_ilyaRight  dd ?
public _gr_ilyaRight
;==========ASM extern Labels==============
align 4
extrn ENDD:proc
extrn END_CLIP:proc
extrn END_CALC_IZ:proc
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
;extrn	_gr_pScreenWidth:dword
extrn	_gr_clipRect:Clip

END_CODE_SEG
end

