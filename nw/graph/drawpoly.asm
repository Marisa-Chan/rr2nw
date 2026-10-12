.486p
MODEL	FLAT,CPP
LOCALS
; align	4

include platform.inc

BEGIN_CODE_SEG
;========================================================================
include get_s.inc
include types.inc
include stack.inc

;
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::;
;
AMCalcParameters Macro parIn,parOut,bPCCW
	local	FindLoop,NoLeftVert,NoRightVert,EndFindLoop
	local	LoopVertexLeft,OkLeft,LoopEdgeLeft,EndLeft
	local   LoopVertexRight,OkRight,LoopEdgeRight,EndRight
	local	EndCalcPar
	local	??EndVertL, ??EndVertR

	;mov	esi,[ESP_startVert]
	mov	esi,[ESP_topVert]
	mov	edi,offset screen

	mov	ds:[??EndVertL],ebp
	;mov	ebp,[ESP_bottomVert]
	;mov	ebp,[ESP_endVert]
	;
	;mov	ds:[@@?Bottom],ebx
	;mov	ds:[@@?Bottom1],ebx
	cmp	esi,ebp
	mov	ds:[??EndVertR],ebp
	jnz	LoopVertexLeft
	;
	mov	esi,[ESP_startVert]
	mov	edi,[ESP_endVert]
	mov	eax,[esi].x
	mov	ebx,[esi].&parIn
	add	esi,type Vertex
	mov	ecx,eax
	mov	edx,ebx
	;eax - xl
	;ebx - parL
	;ecx - xr
	;edx - parR
	;esi - vert
FindLoop:
	cmp	[esi].x,eax
	jge	NoLeftVert
	mov	eax,[esi].x
	mov	ebx,[esi].&parIn
	cmp	esi,edi
	jz	EndFindLoop
	add	esi,type Vertex
	jmp	FindLoop
align 4
NoLeftVert:
	cmp	[esi].x,ecx
	jle	NoRightVert
        mov     ecx,[esi].x
	mov	edx,[esi].&parIn
NoRightVert:
	cmp	esi,edi
	jz	EndFindLoop
	add	esi,type Vertex
	jmp	FindLoop
align 4
EndFindLoop:
	mov	[screen].&parOut&l,ebx
	mov	[screen].&parOut&r,edx
	mov	esi,[ESP_bottomVert]
	jmp	EndCalcPar
	;
	;Left Side
	;
LoopVertexLeft:
	mov	ebx,[esi].y
	mov	eax,[esi].&parIn

	add	esi,type Vertex
	mov	ecx,eax
	cmp	esi,[ESP_endVert]
	jle	OkLeft
	mov	esi,[ESP_startVert]
	;
OkLeft:
	;sal	ecx,pres

	sub	ebx,[esi].y	;-dy
	jz	EndLeft
	sub	eax,[esi].&parIn	;-dPar

	;sal	eax,pres

	imul	D_P [_gr_pDivCache - ebx*4]
	;shrd	eax,edx,Y_DIV_CACHE_PRES
	shld	edx,eax,32-Y_DIV_CACHE_PRES
	;edx - deltaPar[]

	shl	eax,32-Y_DIV_CACHE_PRES
	xor	ebp,ebp

	;cdq
	;idiv	ebx

	;edx - deltaPar[]
	;ebx - -dy
	;ecx - Par0[]
	;ebp - Par0{}
	;esi - vertex
	;edi - screen
	;eax - deltaPar{}
LoopEdgeLeft:

	mov	[edi].&parOut&l,ecx
	add	ebp,eax
	adc	ecx,edx
	add	edi,type Screen
	inc	ebx
	jl	LoopEdgeLeft

	;sub	edi,type Screen
	mov	edx,[esi].&parIn
	mov	[edi].&parOut&l,edx

EndLeft:

	GetDW	cmp     esi,??EndVertL
	jnz	LoopVertexLeft
	;
	;Right Side
	;
	mov	edi,offset screen
	mov	esi,[ESP_topVert]

LoopVertexRight:
	mov	ebx,[esi].y
	mov	eax,[esi].&parIn

	IF	(bPCCW NE TRUE)
	add	esi,type Vertex
	mov	ecx,eax
	cmp	esi,[ESP_endVert]
	jle	OkRight
	mov	esi,[ESP_startVert]
	ELSE
	sub	esi,type Vertex
	mov	ecx,eax
	cmp	esi,[ESP_startVert]
	jge	OkRight
	mov	esi,[ESP_endVert]
	ENDIF
	;
OkRight:
	;sal	ecx,pres

	sub	ebx,[esi].y	;-dy
	jz	EndRight
	sub	eax,[esi].&parIn	;-dPar

	;sal	eax,pres

	imul	D_P [_gr_pDivCache - ebx*4]
	;shrd	eax,edx,Y_DIV_CACHE_PRES
	shld	edx,eax,32-Y_DIV_CACHE_PRES
	;edx - deltaPar[]

	shl	eax,32-Y_DIV_CACHE_PRES
	xor	ebp,ebp
	;cdq
	;idiv	ebx

	;edx - deltaPar[]
	;ebx - -dy
	;ecx - Par0[]
	;ebp - Par0{}
	;esi - vertex
	;edi - screen
	;eax - deltaPar{}
LoopEdgeRight:

	mov	[edi].&parOut&r,ecx
	add	ebp,eax
	adc	ecx,edx
	add	edi,type Screen
	inc	ebx
	jl	LoopEdgeRight

	mov	edx,[esi].&parIn
	mov	[edi].&parOut&r,edx
	;sub	edi,type Screen

EndRight:

	GetDW	cmp     esi,??EndVertR
	jnz	LoopVertexRight
	;
EndCalcPar:
	mov	ebp,esi
endM


comment `

AMCalcParameters Macro parIn,parOut,bPCCW,pres
	local	FindLoop,NoLeftVert,NoRightVert,EndFindLoop
	local	LoopVertexLeft,OkLeft,LoopEdgeLeft,EndLeft
	local   LoopVertexRight,OkRight,LoopEdgeRight,EndRight
	local	EndCalcPar

	;mov	esi,[ESP_startVert]
	mov	esi,[ESP_topVert]
	mov	edi,offset screen
	;mov	ebp,[ESP_bottomVert]
	;mov	ebp,[ESP_endVert]
	;
	;mov	ds:[@@?Bottom],ebx
	;mov	ds:[@@?Bottom1],ebx
	cmp	esi,ebp
	jnz	LoopVertexLeft
	;
	mov	esi,[ESP_startVert]
	mov	edi,[ESP_endVert]
	mov	eax,[esi].x
	mov	ebx,[esi].&parIn
	add	esi,type Vertex
	mov	ecx,eax
	mov	edx,ebx
	;eax - xl
	;ebx - parL
	;ecx - xr
	;edx - parR
	;esi - vert
FindLoop:
	cmp	[esi].x,eax
	jge	NoLeftVert
	mov	eax,[esi].x
	mov	ebx,[esi].&parIn
	cmp	esi,edi
	jz	EndFindLoop
	add	esi,type Vertex
	jmp	FindLoop
align 4
NoLeftVert:
	cmp	[esi].x,ecx
	jle	NoRightVert
	mov	ecx,[esi].x
	mov	edx,[esi].&parIn
NoRightVert:
	cmp	esi,edi
	jz	EndFindLoop
	add	esi,type Vertex
	jmp	FindLoop
align 4
EndFindLoop:
	mov	[screen].&parOut&l,ebx
	mov	[screen].&parOut&r,edx
	jmp	EndCalcPar
	;
	;Left Side
	;
LoopVertexLeft:
	mov	ebx,[esi].y
	mov	eax,[esi].&parIn

	IF	bPCCW
	add	esi,type Vertex
	mov	ecx,eax
	cmp	esi,[ESP_endVert]
	jle	OkLeft
	mov	esi,[ESP_startVert]
	ELSE
	sub	esi,type Vertex
	mov	ecx,eax
	cmp	esi,[ESP_startVert]
	jge	OkLeft
	mov	esi,[ESP_endVert]
	ENDIF
	;
OkLeft:
	IF	(pres GT 0)
	sal	ecx,pres
	ENDIF

	sub	ebx,[esi].y	;-dy
	jz	EndLeft
	sub	eax,[esi].&parIn	;-dPar

	IF	(pres GT 0)
	sal	eax,pres
	ENDIF

	imul	D_P [_gr_pDivCache - ebx*4]
	shrd	eax,edx,Y_DIV_CACHE_PRES

	;cdq
	;idiv	ebx

	;eax - deltaPar
	;ebx - -dy
	;ecx - Par0
	;esi - vertex
	;edi - screen
	;ebp - bottomVert
LoopEdgeLeft:

	IF	(pres GT 0)
	mov	edx,ecx
	add	edi,type Screen
	sar	edx,pres
	add	ecx,eax
	inc	ebx
	mov	[edi - type Screen].&parOut&l,edx
	ELSE
	mov	[edi].&parOut&l,ecx
	add	ecx,eax
	add	edi,type Screen
	inc	ebx
	ENDIF

	jl	LoopEdgeLeft
	;sub	edi,type Screen
	mov	edx,[esi].&parIn
	mov	[edi].&parOut&l,edx

EndLeft:

	cmp     esi,ebp
	jnz	LoopVertexLeft
	;
	;Right Side
	;
	mov	edi,offset screen
	mov	esi,[ESP_topVert]

LoopVertexRight:
	mov	ebx,[esi].y
	mov	eax,[esi].&parIn

	IF	(bPCCW NE TRUE)
	add	esi,type Vertex
	mov	ecx,eax
	cmp	esi,[ESP_endVert]
	jle	OkRight
	mov	esi,[ESP_startVert]
	ELSE
	sub	esi,type Vertex
	mov	ecx,eax
	cmp	esi,[ESP_startVert]
	jge	OkRight
	mov	esi,[ESP_endVert]
	ENDIF
	;
OkRight:
	IF	(pres GT 0)
	sal	ecx,pres
	ENDIF

	sub	ebx,[esi].y	;-dy
	jz	EndRight
	sub	eax,[esi].&parIn	;-dPar

	IF	(pres GT 0)
	sal	eax,pres
	ENDIF

	imul	D_P [_gr_pDivCache - ebx*4]
	shrd	eax,edx,Y_DIV_CACHE_PRES

	;cdq
	;idiv	ebx

	;eax - deltaPar
	;ebx - -dy
	;ecx - Par0
	;esi - vertex
	;edi - screen
	;ebp - bottomVert
LoopEdgeRight:

	IF	(pres GT 0)
	mov	edx,ecx
	add	edi,type Screen
	sar	edx,pres
	add	ecx,eax
	inc	ebx
	mov	[edi - type Screen].&parOut&r,edx
	ELSE
	mov	[edi].&parOut&r,ecx
	add	ecx,eax
	add	edi,type Screen
	inc	ebx
	ENDIF

	jl	LoopEdgeRight
	;sub	edi,type Screen
	mov	edx,[esi].&parIn
	mov	[edi].&parOut&r,edx

EndRight:

	cmp     esi,ebp
	jnz	LoopVertexRight
	;
EndCalcPar:
endM
`

;
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
ASM_GR_SetClipRect:
        Public ASM_GR_SetClipRect
SetClipping proc

	mov	eax,_gr_clipRect.top
	mov	ds:[?Top],eax
	mov	ds:[??Top],eax
	mov	eax,_gr_clipRect.left
	mov	ds:[?Left],eax
	mov	ds:[??Left],eax
	mov	eax,_gr_clipRect.bottom
	mov	ds:[?Bottom],eax
	mov	ds:[??Bottom],eax
	dec	eax
	mov	_gr_ilyaBottom,eax
	mov	eax,_gr_clipRect.right
	mov	ds:[?Right],eax
	mov	ds:[??Right],eax
	dec	eax
	mov	_gr_ilyaRight,eax

	ret
endP
;
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
align 8
ASM_GRDrawPolygonPCCW:
        Public ASM_GRDrawPolygonPCCW
DrawPolygonPCCW proc
	local	u0:dword,v0:dword,du:dword,dv:dword
	local	D_D:dword,D_F:dword,D_B:dword,num:dword,scr:dword
	local	startVert:dword,endVert:dword,topVert:dword,bottomVert:dword
	local	vertC:dword,count:dword
	uses	esi,edi,ebx


	mov	esi,offset _gr_vertices
	mov	ecx,[_gr_polygon].vertc
	mov	edi,esi
	dec	ecx
	mov	edx,[esi].x
	mov	[vertC],ecx
	mov	[count],ecx
	mov	ecx,[esi].y
	mov	ebx,edx
	push	ebp
	;
	;Can`t access to stack frame
	;
	mov	eax,ecx
	mov	ebp,esi
	;eax,ecx - topY & bottomY
	;ebx,edx - leftX & rightX
	;esi,edi - topV & bottomV
	;ebp - currVertex
@@Loop:
	add	ebp,type Vertex

	cmp	eax,[ebp].y
	jle	@@BotY
	mov	eax,[ebp].y
	mov	esi,ebp
	jmp	@@LefX
align 4
@@BotY:
	cmp	ecx,[ebp].y
	jge	@@LefX
	mov	ecx,[ebp].y
	mov	edi,ebp
@@LefX:
	cmp	ebx,[ebp].x
	jle	@@RigX
	mov	ebx,[ebp].x
	jmp	@@EndLoop
align 4
@@RigX:
	cmp	edx,[ebp].x
	jge	@@EndLoop
	mov	edx,[ebp].x
@@EndLoop:
	dec	[ESP_count]
	jnz	@@Loop
	;
	GetDW	cmp	ecx,?Top
	jl	ENDD
	GetDW	cmp	eax,?Bottom
	jge	ENDD
	GetDW	cmp	ebx,?Right
	jge	ENDD
	GetDW	cmp	edx,?Left
	jl	ENDD

	mov	[ESP_topVert],esi
	mov	esi,[_gr_polygon].typall ;V
	;
	add	esi,[_gr_polygon].addtyp
	mov	[ESP_bottomVert],edi
	mov	edi,esi
	mov	[_gr_polygon].typall,esi
	add	esi,offset CalcInvZTable
	mov	[ESP_endVert],ebp
	mov	ds:[?TypeAll],esi
	mov	esi,edi
	add	edi,offset Clip2DTable
	mov	[ESP_startVert],offset _gr_vertices
	mov	ds:[??TypeAll],edi
	mov	edi,esi
	add	esi,offset ATypeScrCalcTable
	mov	ds:[???TypeAll],esi
	xor	esi,esi

	;eax,ecx - topY & bottomY
	;ebx,edx - leftX & rightX
	;esi,edi - topV & bottomV
	GetDW	cmp	eax,??Top
	jge	@@OkTop
	inc	esi
@@OkTop:
	GetDW	cmp	ecx,??Bottom
	jl	@@OkBottom
	add	esi,2
@@OkBottom:
	GetDW	cmp	ebx,??Left
	jge	@@OkLeft
	add	esi,4
@@OkLeft:
	GetDW	cmp	edx,??Right
	jl	@@OkRight
	add	esi,8
@@OkRight:
	;==============================================
	;don't use esi
	;ebp - endVert
	;Vertex - startVert
	;mov	eax,[_gr_polygon].typall
	jmp	D_P [CalcInvZTable + 0FACEBEDAh]
	org	$-4
	?TypeAll	dd 0FACEBEDAh
END_CALC_IZ:
public END_CALC_IZ
	;==============================================

	;==============================================
	;esi - RLBT
	;eax - typeAll
	test	esi,esi
	;mov	eax,[_gr_polygon].typall
	jz	END_CLIP
	jmp	D_P [Clip2DTable + 0FACEBEDAh]
	org	$-4
	??TypeAll	dd 0FACEBEDAh
align 4
END_CLIP:
public END_CLIP
	;==============================================
	;
	;

	mov 	ebp,[ESP_bottomVert]
	AMCalcParameters x,x,PCCW,16

	;ebp - BottomVertex
	;            x
	;mov	eax,[_gr_polygon].typall
	jmp	D_P [ATypeScrCalcTable + 0FACEBEDAh]
	org	$-4
	???TypeAll	dd 0FACEBEDAh
align 4
END_CALC_PAR:
public END_CALC_PAR
	;
	;
	;Draw Polygon
	;
	mov	eax,[_gr_polygon].typall
	jmp	D_P [ATypeDrawTable + eax]
align 4
END_TYPE_DRAW:
public END_TYPE_DRAW
	;
        ;mov     eax,[_gr_polygon].addtyp
        ;test    eax,eax
        ;jz      ENDD
	;
        ;test    eax,BUMP_ADDTYPE
        ;jnz     ADrawBump32

END_DRAW_BUMP:
public END_DRAW_BUMP
        ;
        ;
        cmp     [_gr_polygon].lights,0
        jz      NO_DRAW_LIGHT_1
        ;test    eax,LIGHT_ADDTYPE
        ;jz      NO_DRAW_LIGHT
        ;
        ;mov     ebp,esp
        ;push    [_gr_polygon].lightC
        ;push    [_gr_polygon].light
        ;push    [ebp + 32] ;startVert
        ;push    [ebp + 36] ;endVert
        ;call    ASM_PreparePolygonForLight
        ;add     esp,4*4
        ;
        ;mov     ebp,[ESP_bottomVert]
        ;AMCalcParameters u,uz,PCCW,6
        ;AMCalcParameters v,vz,PCCW,6
        ;
        mov     ebp,[ESP_topVert]
        ;mov     edi,[_gr_polygon].lights
        ;mov     ebx,_LightTextH
        mov     esi,offset _gr_pLights
        ;mov     [_gr_polygon].textp,ebx
        mov     ebp,[ebp].y
        mov     edi,[_gr_polygon].lights
        ;mov     esi,[edi]
        mov     [_minY],ebp
        mov     [_LightMask],edi
@@LightLoop:
        test    edi,1
        jz      NO_DRAW_CURR_LIGHT
        test    D_P [esi],1
        jnz     NO_DRAW_CURR_LIGHT      ;draw SHADOW
        mov     [_LightPtr],esi
        push    esi
        call    ASM_PrepareLightSource
        pop     esi
        mov     ebp,[esi+4] ;get light->colorTable
        cmp     ebp,0
        jz      END_DRAW_LIGHT
        mov     [_gr_polygon].color,ebp

        ;add     esp,4
        ;
        ;Draw Light
        ;jmp     ADrawLight
        jmp     ADrawLightPer8
        ;
align 4
END_DRAW_LIGHT:
public END_DRAW_LIGHT
        ;
        mov     edi,[_LightMask]
        mov     esi,[_LightPtr]

NO_DRAW_CURR_LIGHT:

        shr     edi,1
        jz      NO_DRAW_LIGHT_1
        add     esi,2100     ;sizeof(SGRLight)
        mov     [_LightMask],edi
        jmp      @@LightLoop
        ;
NO_DRAW_LIGHT_1:
        mov     eax,[_gr_polygon].addtyp
        ;
NO_DRAW_LIGHT:
public NO_DRAW_LIGHT
        ;
        and     eax,eax
        jz      ENDD
        ;
        and     eax,HAZE_ADDTYPE
	jnz	ADrawHaze
;align 4
END_DRAW_HAZE:
public END_DRAW_HAZE
	;
;	test	[Polygon].addtyp,LIGHT_ADDTYPE
;	jnz	G_ADrawLight
;@@EndDrawLight:
;	;
;	test	[Polygon].addtyp,SHADOW_ADDTYPE
;	jnz	ADrawShadow
;@@EndDrawLight:
	;
ENDD:
public ENDD
	pop	ebp
        mov     eax,1
	ret
endp

;-------------------------------------------------
;-----------Function on AMCalcParameters----------
;-------------------------------------------------
align 8
G_CalcParZ:
	AMCalcParameters iz,iz,PCCW,6
jmp	END_CALC_PAR
;-------------------------------------------------
align 8
G_CalcParUV:
	AMCalcParameters u,uz,PCCW,6
	AMCalcParameters v,vz,PCCW,6
jmp	END_CALC_PAR
;-------------------------------------------------
align 8
G_CalcParZU:
	AMCalcParameters iz,iz,PCCW,6
        AMCalcParameters col,uz,PCCW,6
jmp	END_CALC_PAR
;-------------------------------------------------
align 8
G_CalcParZUV:
	AMCalcParameters iz,iz,PCCW,6
	AMCalcParameters u,uz,PCCW,6
	AMCalcParameters v,vz,PCCW,6
jmp	END_CALC_PAR
;-------------------------------------------------
;BUMP
align 8
G_CalcParB:
	AMCalcParameters iz,iz,PCCW,6
	AMCalcParameters ub,ubz,PCCW,6
	AMCalcParameters vb,vbz,PCCW,6
jmp	END_CALC_PAR
;-------------------------------------------------
align 8
G_CalcParUVB:
	AMCalcParameters u,uz,PCCW,6
	AMCalcParameters v,vz,PCCW,6
	AMCalcParameters iz,iz,PCCW,6
	AMCalcParameters ub,ubz,PCCW,6
	AMCalcParameters vb,vbz,PCCW,6
jmp	END_CALC_PAR
;-------------------------------------------------
align 8
G_CalcParUB:
	AMCalcParameters iz,iz,PCCW,6
        AMCalcParameters col,uz,PCCW,6
	AMCalcParameters ub,ubz,PCCW,6
	AMCalcParameters vb,vbz,PCCW,6
jmp	END_CALC_PAR
;-------------------------------------------------
;-------------------------------------------------
;-------------------------------------------------
;
;
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::;
;
ASM_GRInitTables:
        Public ASM_GRInitTables
InitTables proc mmx:dword
        uses    esi,edi

	mov	eax,offset CalcInvZTable
	mov	D_P [eax + FLAT_TYPE + NO_ADDTYPE],offset END_CALC_IZ
	mov	D_P [eax + FLAT_TYPE + HAZE_ADDTYPE],offset END_CALC_IZ
        mov     D_P [eax + FLAT_TYPE + BUMP_ADDTYPE],offset END_CALC_IZ
	mov	D_P [eax + FLAT_TYPE + LIGHT_ADDTYPE],offset END_CALC_IZ
        mov     D_P [eax + FLAT_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset END_CALC_IZ
	mov	D_P [eax + FLAT_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset END_CALC_IZ
        mov     D_P [eax + FLAT_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset END_CALC_IZ
        mov     D_P [eax + FLAT_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE], offset END_CALC_IZ
	;
	mov	D_P [eax + TRANSPARENT_TYPE + NO_ADDTYPE],offset END_CALC_IZ
	mov	D_P [eax + TRANSPARENT_TYPE + HAZE_ADDTYPE],offset END_CALC_IZ
        mov     D_P [eax + TRANSPARENT_TYPE + BUMP_ADDTYPE],offset END_CALC_IZ
	mov	D_P [eax + TRANSPARENT_TYPE + LIGHT_ADDTYPE],offset END_CALC_IZ
        mov     D_P [eax + TRANSPARENT_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset END_CALC_IZ
	mov	D_P [eax + TRANSPARENT_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset END_CALC_IZ
        mov     D_P [eax + TRANSPARENT_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset END_CALC_IZ
        mov     D_P [eax + TRANSPARENT_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset END_CALC_IZ
	;
	mov	D_P [eax + GOURAUD_TYPE + NO_ADDTYPE],offset G_InvU
	mov	D_P [eax + GOURAUD_TYPE + HAZE_ADDTYPE],offset G_InvU
        mov     D_P [eax + GOURAUD_TYPE + BUMP_ADDTYPE],offset G_InvU
	mov	D_P [eax + GOURAUD_TYPE + LIGHT_ADDTYPE],offset G_InvU
        mov     D_P [eax + GOURAUD_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset G_InvU
	mov	D_P [eax + GOURAUD_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset G_InvU
        mov     D_P [eax + GOURAUD_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_InvU
        mov     D_P [eax + GOURAUD_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_InvU
	;
	mov	D_P [eax + TEXTURE_P_TYPE + NO_ADDTYPE],offset G_InvUV
	mov	D_P [eax + TEXTURE_P_TYPE + HAZE_ADDTYPE],offset G_InvUV
        mov     D_P [eax + TEXTURE_P_TYPE + BUMP_ADDTYPE],offset G_InvUV
	mov	D_P [eax + TEXTURE_P_TYPE + LIGHT_ADDTYPE],offset G_InvUV
        mov     D_P [eax + TEXTURE_P_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset G_InvUV
	mov	D_P [eax + TEXTURE_P_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset G_InvUV
        mov     D_P [eax + TEXTURE_P_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_InvUV
        mov     D_P [eax + TEXTURE_P_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_InvUV
	;
	mov	D_P [eax + TEXTURE_L_TYPE + NO_ADDTYPE],offset END_CALC_IZ
	mov	D_P [eax + TEXTURE_L_TYPE + HAZE_ADDTYPE],offset END_CALC_IZ
        mov     D_P [eax + TEXTURE_L_TYPE + BUMP_ADDTYPE],offset END_CALC_IZ
	mov	D_P [eax + TEXTURE_L_TYPE + LIGHT_ADDTYPE],offset END_CALC_IZ
        mov     D_P [eax + TEXTURE_L_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset END_CALC_IZ
	mov	D_P [eax + TEXTURE_L_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset END_CALC_IZ
        mov     D_P [eax + TEXTURE_L_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset END_CALC_IZ
        mov     D_P [eax + TEXTURE_L_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset END_CALC_IZ
	;
	mov	D_P [eax + TEXTURE_P_TRSP_TYPE + NO_ADDTYPE],offset G_InvUV
	mov	D_P [eax + TEXTURE_P_TRSP_TYPE + HAZE_ADDTYPE],offset G_InvUV
        mov     D_P [eax + TEXTURE_P_TRSP_TYPE + BUMP_ADDTYPE],offset G_InvUV
	mov	D_P [eax + TEXTURE_P_TRSP_TYPE + LIGHT_ADDTYPE],offset G_InvUV
        mov     D_P [eax + TEXTURE_P_TRSP_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset G_InvUV
	mov	D_P [eax + TEXTURE_P_TRSP_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset G_InvUV
        mov     D_P [eax + TEXTURE_P_TRSP_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_InvUV
        mov     D_P [eax + TEXTURE_P_TRSP_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_InvUV
        ;
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + NO_ADDTYPE],offset G_InvUV
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + HAZE_ADDTYPE],offset G_InvUV
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + BUMP_ADDTYPE],offset G_InvUV
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + LIGHT_ADDTYPE],offset G_InvUV
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset G_InvUV
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset G_InvUV
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_InvUV
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_InvUV
        ;
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + NO_ADDTYPE],offset END_CALC_IZ
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + HAZE_ADDTYPE],offset END_CALC_IZ
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + BUMP_ADDTYPE],offset END_CALC_IZ
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + LIGHT_ADDTYPE],offset END_CALC_IZ
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset END_CALC_IZ
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset END_CALC_IZ
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset END_CALC_IZ
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset END_CALC_IZ
        ;
        ;------------------------------------------------------------------
	mov	eax,offset Clip2DTable
	mov	D_P [eax + FLAT_TYPE + NO_ADDTYPE],offset G_Clip
	mov	D_P [eax + FLAT_TYPE + HAZE_ADDTYPE],offset G_ClipZ
        mov     D_P [eax + FLAT_TYPE + BUMP_ADDTYPE],offset G_Clip
        mov     D_P [eax + FLAT_TYPE + LIGHT_ADDTYPE],offset G_Clip
        mov     D_P [eax + FLAT_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset G_ClipZ
        mov     D_P [eax + FLAT_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset G_ClipZ
        mov     D_P [eax + FLAT_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_Clip
        mov     D_P [eax + FLAT_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_ClipZ
	;
	mov	D_P [eax + TRANSPARENT_TYPE + NO_ADDTYPE],offset G_Clip
	mov	D_P [eax + TRANSPARENT_TYPE + HAZE_ADDTYPE],offset G_ClipZ
        mov     D_P [eax + TRANSPARENT_TYPE + BUMP_ADDTYPE],offset G_Clip
        mov     D_P [eax + TRANSPARENT_TYPE + LIGHT_ADDTYPE],offset G_Clip
        mov     D_P [eax + TRANSPARENT_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset G_ClipZ
        mov     D_P [eax + TRANSPARENT_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset G_ClipZ
        mov     D_P [eax + TRANSPARENT_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_Clip
        mov     D_P [eax + TRANSPARENT_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_ClipZ
	;
	mov	D_P [eax + GOURAUD_TYPE + NO_ADDTYPE],offset G_ClipZU
	mov	D_P [eax + GOURAUD_TYPE + HAZE_ADDTYPE],offset G_ClipZU
        mov     D_P [eax + GOURAUD_TYPE + BUMP_ADDTYPE],offset G_ClipZU
	mov	D_P [eax + GOURAUD_TYPE + LIGHT_ADDTYPE],offset G_ClipZU
        mov     D_P [eax + GOURAUD_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset G_ClipZU
	mov	D_P [eax + GOURAUD_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset G_ClipZU
        mov     D_P [eax + GOURAUD_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_ClipZU
        mov     D_P [eax + GOURAUD_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_ClipZU
	;
	mov	D_P [eax + TEXTURE_P_TYPE + NO_ADDTYPE],offset G_ClipZUV
	mov	D_P [eax + TEXTURE_P_TYPE + HAZE_ADDTYPE],offset G_ClipZUV
        mov     D_P [eax + TEXTURE_P_TYPE + BUMP_ADDTYPE],offset G_ClipZUV
	mov	D_P [eax + TEXTURE_P_TYPE + LIGHT_ADDTYPE],offset G_ClipZUV
        mov     D_P [eax + TEXTURE_P_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset G_ClipZUV
	mov	D_P [eax + TEXTURE_P_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset G_ClipZUV
        mov     D_P [eax + TEXTURE_P_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_ClipZUV
        mov     D_P [eax + TEXTURE_P_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_ClipZUV
	;
	mov	D_P [eax + TEXTURE_L_TYPE + NO_ADDTYPE],offset G_ClipUV
	mov	D_P [eax + TEXTURE_L_TYPE + HAZE_ADDTYPE],offset G_ClipZUV
        mov     D_P [eax + TEXTURE_L_TYPE + BUMP_ADDTYPE],offset G_ClipUV
        mov     D_P [eax + TEXTURE_L_TYPE + LIGHT_ADDTYPE],offset G_ClipUV
        mov     D_P [eax + TEXTURE_L_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset G_ClipZUV
	mov	D_P [eax + TEXTURE_L_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset G_ClipZUV
        mov     D_P [eax + TEXTURE_L_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_ClipUV
        mov     D_P [eax + TEXTURE_L_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_ClipZUV
	;
	mov	D_P [eax + TEXTURE_P_TRSP_TYPE + NO_ADDTYPE],offset G_ClipZUV
	mov	D_P [eax + TEXTURE_P_TRSP_TYPE + HAZE_ADDTYPE],offset G_ClipZUV
        mov     D_P [eax + TEXTURE_P_TRSP_TYPE + BUMP_ADDTYPE],offset G_ClipZUV
	mov	D_P [eax + TEXTURE_P_TRSP_TYPE + LIGHT_ADDTYPE],offset G_ClipZUV
        mov     D_P [eax + TEXTURE_P_TRSP_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset G_ClipZUV
	mov	D_P [eax + TEXTURE_P_TRSP_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset G_ClipZUV
        mov     D_P [eax + TEXTURE_P_TRSP_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_ClipZUV
        mov     D_P [eax + TEXTURE_P_TRSP_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_ClipZUV
        ;
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + NO_ADDTYPE],offset G_ClipZUV
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + HAZE_ADDTYPE],offset G_ClipZUV
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + BUMP_ADDTYPE],offset G_ClipZUV
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + LIGHT_ADDTYPE],offset G_ClipZUV
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset G_ClipZUV
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset G_ClipZUV
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_ClipZUV
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_ClipZUV
        ;
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + NO_ADDTYPE],offset G_ClipUV
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + HAZE_ADDTYPE],offset G_ClipZUV
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + BUMP_ADDTYPE],offset G_ClipUV
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + LIGHT_ADDTYPE],offset G_ClipUV
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset G_ClipZUV
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset G_ClipZUV
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_ClipUV
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_ClipZUV
        ;
        ;--------------------------------------------------------------------
	mov	eax,offset ATypeScrCalcTable
	mov	D_P [eax + FLAT_TYPE + NO_ADDTYPE],offset END_CALC_PAR
	mov	D_P [eax + FLAT_TYPE + HAZE_ADDTYPE],offset G_CalcParZ
        mov     D_P [eax + FLAT_TYPE + BUMP_ADDTYPE],offset END_CALC_PAR
        mov     D_P [eax + FLAT_TYPE + LIGHT_ADDTYPE],offset END_CALC_PAR
        mov     D_P [eax + FLAT_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset G_CalcParZ
	mov	D_P [eax + FLAT_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset G_CalcParZ
        mov     D_P [eax + FLAT_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset END_CALC_PAR
        mov     D_P [eax + FLAT_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_CalcParZ
	;
	mov	D_P [eax + TRANSPARENT_TYPE + NO_ADDTYPE],offset END_CALC_PAR
	mov	D_P [eax + TRANSPARENT_TYPE + HAZE_ADDTYPE],offset G_CalcParZ
        mov     D_P [eax + TRANSPARENT_TYPE + BUMP_ADDTYPE],offset END_CALC_PAR
        mov     D_P [eax + TRANSPARENT_TYPE + LIGHT_ADDTYPE],offset END_CALC_PAR
        mov     D_P [eax + TRANSPARENT_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset G_CalcParZ
	mov	D_P [eax + TRANSPARENT_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset G_CalcParZ
        mov     D_P [eax + TRANSPARENT_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset END_CALC_PAR
        mov     D_P [eax + TRANSPARENT_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_CalcParZ
	;
	mov	D_P [eax + GOURAUD_TYPE + NO_ADDTYPE],offset G_CalcParZU
	mov	D_P [eax + GOURAUD_TYPE + HAZE_ADDTYPE],offset G_CalcParZU
        mov     D_P [eax + GOURAUD_TYPE + BUMP_ADDTYPE],offset G_CalcParZU
	mov	D_P [eax + GOURAUD_TYPE + LIGHT_ADDTYPE],offset G_CalcParZU
        mov     D_P [eax + GOURAUD_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset G_CalcParZU
	mov	D_P [eax + GOURAUD_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset G_CalcParZU
        mov     D_P [eax + GOURAUD_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_CalcParZU
        mov     D_P [eax + GOURAUD_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_CalcParZU
	;
	mov	D_P [eax + TEXTURE_P_TYPE + NO_ADDTYPE],offset G_CalcParZUV
	mov	D_P [eax + TEXTURE_P_TYPE + HAZE_ADDTYPE],offset G_CalcParZUV
        mov     D_P [eax + TEXTURE_P_TYPE + BUMP_ADDTYPE],offset G_CalcParZUV
	mov	D_P [eax + TEXTURE_P_TYPE + LIGHT_ADDTYPE],offset G_CalcParZUV
        mov     D_P [eax + TEXTURE_P_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset G_CalcParZUV
	mov	D_P [eax + TEXTURE_P_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset G_CalcParZUV
        mov     D_P [eax + TEXTURE_P_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_CalcParZUV
        mov     D_P [eax + TEXTURE_P_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_CalcParZUV
	;
	mov	D_P [eax + TEXTURE_L_TYPE + NO_ADDTYPE],offset G_CalcParUV
	mov	D_P [eax + TEXTURE_L_TYPE + HAZE_ADDTYPE],offset G_CalcParZUV
        mov     D_P [eax + TEXTURE_L_TYPE + BUMP_ADDTYPE],offset G_CalcParUV
        mov     D_P [eax + TEXTURE_L_TYPE + LIGHT_ADDTYPE],offset G_CalcParUV
        mov     D_P [eax + TEXTURE_L_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset G_CalcParZUV
	mov	D_P [eax + TEXTURE_L_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset G_CalcParZUV
        mov     D_P [eax + TEXTURE_L_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_CalcParUV
        mov     D_P [eax + TEXTURE_L_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_CalcParZUV
	;
	mov	D_P [eax + TEXTURE_P_TRSP_TYPE + NO_ADDTYPE],offset G_CalcParZUV
	mov	D_P [eax + TEXTURE_P_TRSP_TYPE + HAZE_ADDTYPE],offset G_CalcParZUV
        mov     D_P [eax + TEXTURE_P_TRSP_TYPE + BUMP_ADDTYPE],offset G_CalcParZUV
	mov	D_P [eax + TEXTURE_P_TRSP_TYPE + LIGHT_ADDTYPE],offset G_CalcParZUV
        mov     D_P [eax + TEXTURE_P_TRSP_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset G_CalcParZUV
	mov	D_P [eax + TEXTURE_P_TRSP_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset G_CalcParZUV
        mov     D_P [eax + TEXTURE_P_TRSP_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_CalcParZUV
        mov     D_P [eax + TEXTURE_P_TRSP_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_CalcParZUV
        ;
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + NO_ADDTYPE],offset G_CalcParZUV
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + HAZE_ADDTYPE],offset G_CalcParZUV
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + BUMP_ADDTYPE],offset G_CalcParZUV
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + LIGHT_ADDTYPE],offset G_CalcParZUV
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset G_CalcParZUV
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset G_CalcParZUV
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_CalcParZUV
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_CalcParZUV
        ;
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + NO_ADDTYPE],offset G_CalcParUV
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + HAZE_ADDTYPE],offset G_CalcParZUV
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + BUMP_ADDTYPE],offset G_CalcParUV
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + LIGHT_ADDTYPE],offset G_CalcParUV
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset G_CalcParZUV
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset G_CalcParZUV
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_CalcParUV
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset G_CalcParZUV
        ;
        ;------------------------------------------------------------------
	mov	eax,offset ATypeDrawTable
	mov	D_P [eax + FLAT_TYPE + NO_ADDTYPE],offset ADrawFlat
	mov	D_P [eax + FLAT_TYPE + HAZE_ADDTYPE],offset ADrawFlat
	mov	D_P [eax + FLAT_TYPE + BUMP_ADDTYPE],offset ADrawFlat
	mov	D_P [eax + FLAT_TYPE + LIGHT_ADDTYPE],offset ADrawFlat
	mov	D_P [eax + FLAT_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset ADrawFlat
	mov	D_P [eax + FLAT_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset ADrawFlat
	mov	D_P [eax + FLAT_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset ADrawFlat
	mov	D_P [eax + FLAT_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset ADrawFlat
	;
	mov	D_P [eax + TRANSPARENT_TYPE + NO_ADDTYPE],offset ADrawTransparent
	mov	D_P [eax + TRANSPARENT_TYPE + HAZE_ADDTYPE],offset ADrawTransparent
	mov	D_P [eax + TRANSPARENT_TYPE + BUMP_ADDTYPE],offset ADrawTransparent
	mov	D_P [eax + TRANSPARENT_TYPE + LIGHT_ADDTYPE],offset ADrawTransparent
	mov	D_P [eax + TRANSPARENT_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset ADrawTransparent
	mov	D_P [eax + TRANSPARENT_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset ADrawTransparent
	mov	D_P [eax + TRANSPARENT_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset ADrawTransparent
	mov	D_P [eax + TRANSPARENT_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset ADrawTransparent
	;
	mov	D_P [eax + GOURAUD_TYPE + NO_ADDTYPE],offset ADrawGouraud
	mov	D_P [eax + GOURAUD_TYPE + HAZE_ADDTYPE],offset ADrawGouraud
	mov	D_P [eax + GOURAUD_TYPE + BUMP_ADDTYPE],offset ADrawGouraud
	mov	D_P [eax + GOURAUD_TYPE + LIGHT_ADDTYPE],offset ADrawGouraud
	mov	D_P [eax + GOURAUD_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset ADrawGouraud
	mov	D_P [eax + GOURAUD_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset ADrawGouraud
	mov	D_P [eax + GOURAUD_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset ADrawGouraud
	mov	D_P [eax + GOURAUD_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset ADrawGouraud
	;
	mov	D_P [eax + TEXTURE_P_TYPE + NO_ADDTYPE],offset ADrawTexturePer32
	mov	D_P [eax + TEXTURE_P_TYPE + HAZE_ADDTYPE],offset ADrawTexturePer32
        mov     D_P [eax + TEXTURE_P_TYPE + BUMP_ADDTYPE],offset ADrawDiserTexture32
	mov	D_P [eax + TEXTURE_P_TYPE + LIGHT_ADDTYPE],offset ADrawTexturePer32
        mov     D_P [eax + TEXTURE_P_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset ADrawDiserTexture32
	mov	D_P [eax + TEXTURE_P_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset ADrawTexturePer32
        mov     D_P [eax + TEXTURE_P_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset ADrawDiserTexture32
        mov     D_P [eax + TEXTURE_P_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset ADrawDiserTexture32
	;
	mov	D_P [eax + TEXTURE_L_TYPE + NO_ADDTYPE],offset ADrawLinTexture
	mov	D_P [eax + TEXTURE_L_TYPE + HAZE_ADDTYPE],offset ADrawLinTexture
	mov	D_P [eax + TEXTURE_L_TYPE + BUMP_ADDTYPE],offset ADrawLinTexture
	mov	D_P [eax + TEXTURE_L_TYPE + LIGHT_ADDTYPE],offset ADrawLinTexture
	mov	D_P [eax + TEXTURE_L_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset ADrawLinTexture
	mov	D_P [eax + TEXTURE_L_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset ADrawLinTexture
	mov	D_P [eax + TEXTURE_L_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset ADrawLinTexture
	mov	D_P [eax + TEXTURE_L_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset ADrawLinTexture
	;
	mov	D_P [eax + TEXTURE_P_TRSP_TYPE + NO_ADDTYPE],offset ADrawSprite32
	mov	D_P [eax + TEXTURE_P_TRSP_TYPE + HAZE_ADDTYPE],offset ADrawSprite32
	mov	D_P [eax + TEXTURE_P_TRSP_TYPE + BUMP_ADDTYPE],offset ADrawSprite32
	mov	D_P [eax + TEXTURE_P_TRSP_TYPE + LIGHT_ADDTYPE],offset ADrawSprite32
	mov	D_P [eax + TEXTURE_P_TRSP_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset ADrawSprite32
	mov	D_P [eax + TEXTURE_P_TRSP_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset ADrawSprite32
	mov	D_P [eax + TEXTURE_P_TRSP_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset ADrawSprite32
	mov	D_P [eax + TEXTURE_P_TRSP_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset ADrawSprite32
        ;
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + NO_ADDTYPE],offset ADrawAlphaTexture32
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + HAZE_ADDTYPE],offset ADrawAlphaTexture32
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + BUMP_ADDTYPE],offset ADrawAlphaTexture32
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + LIGHT_ADDTYPE],offset ADrawAlphaTexture32
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset ADrawAlphaTexture32
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset ADrawAlphaTexture32
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset ADrawAlphaTexture32
        mov     D_P [eax + TEXTURE_ALPHA_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset ADrawAlphaTexture32
        ;
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + NO_ADDTYPE],offset ADrawLinSprite
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + HAZE_ADDTYPE],offset ADrawLinSpriteHaze
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + BUMP_ADDTYPE],offset ADrawLinSprite
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + LIGHT_ADDTYPE],offset ADrawLinSprite
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE],offset ADrawLinSpriteHaze
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + HAZE_ADDTYPE + LIGHT_ADDTYPE],offset ADrawLinSpriteHaze
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset ADrawLinSprite
        mov     D_P [eax + TEXTURE_L_TRSP_TYPE + HAZE_ADDTYPE + BUMP_ADDTYPE + LIGHT_ADDTYPE],offset ADrawLinSpriteHaze
        ;
        ;-------------Set Transparency Mul Table--------------------
        mov     ecx,offset _MulTable16x16
        mov     edi,0
@@LoopY:
        mov     esi,1
@@LoopX:
        mov     eax,edi
        imul    esi
        shr     eax,4
        mov     B_P [ecx],al
        inc     esi
        inc     ecx
        cmp     esi,16
        jle     @@LoopX

        inc     edi
        cmp     edi,15
        jle     @@LoopY

        ret
endp

;
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::;
;
;
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::;
;
ASM_GRCalcYDivCache:
        Public ASM_GRCalcYDivCache
InitYDiv proc
	uses	ebx,esi,edi

	mov	esi,_gr_pDivCache - 4
	mov	ecx,_gr_nScreenWidth
	mov	edi,_gr_p1DivCache + 4
	cmp	ecx,_gr_nScreenHeight
	jge	@@Ok
	mov	ecx,_gr_nScreenHeight
@@Ok:
	;mov	ecx,ebx
	;shl	ebx,2
	;sub	esi,ebx
	;mov	ebx,ecx
	mov	ebx,-1
@@LoopY:
	test	ebx,ebx
	jz	@@NoDiv
	mov	eax,(Y_DIV_CACHE)
	cdq
	idiv	ebx
;@@NoDiv:
	mov	[esi],eax
	sub	esi,4

	mov	eax, -(Y_DIV_CACHE)
	cdq
	idiv	ebx
	mov	[edi],eax
	add	edi,4
	jmp	@@lllll
@@NoDiv:
	sub	esi,4
	add	edi,4
@@lllll:
	dec	ebx
	dec	ecx
	jge	@@LoopY
	;
	mov	ecx,_gr_nScreenWidth
	mov	ds:[EXT_ScreenWidth],ecx
	mov	ds:[EXT_ScreenWidth1],ecx
	mov	ds:[EXT_ScreenWidth2],ecx
	mov	ds:[EXT_ScreenWidth3],ecx
	mov	ds:[EXT_ScreenWidth4],ecx
	mov	ds:[EXT_ScreenWidth5],ecx
	mov	ds:[EXT_ScreenWidth5_],ecx
	mov	ds:[EXT_ScreenWidth6],ecx
	mov	ds:[EXT_ScreenWidth7],ecx
	mov	ds:[EXT_ScreenWidth8],ecx
	mov	ds:[EXT_ScreenWidth9],ecx
	mov	ds:[EXT_ScreenWidth10],ecx
	mov	ds:[EXT_ScreenWidth11],ecx
        ;mov     ds:[EXT_ScreenWidth12],ecx
        mov     ds:[EXT_ScreenWidth13],ecx
        mov     ds:[EXT_ScreenWidth13a],ecx
        mov     ds:[EXT_ScreenWidth13b],ecx
        mov     ds:[EXT_ScreenWidth13c],ecx
        mov     ds:[EXT_ScreenWidth14a],ecx
        mov     ds:[EXT_ScreenWidth14b],ecx
        mov     ds:[EXT_ScreenWidth15a],ecx
        mov     ds:[EXT_ScreenWidth15b],ecx
        mov     ds:[EXT_ScreenWidth16a],ecx
        mov     ds:[EXT_ScreenWidth16b],ecx
        mov     ds:[EXT_ScreenWidth17],ecx
        mov     ds:[EXT_ScreenWidth18a],ecx
        mov     ds:[EXT_ScreenWidth18b],ecx
        mov     ds:[EXT_ScreenWidth19a],ecx
        mov     ds:[EXT_ScreenWidth19b],ecx
        mov     ds:[EXT_ScreenWidth20a],ecx
        mov     ds:[EXT_ScreenWidth20b],ecx
        ret
endp

;
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::;
;
unused dd  1024 DUP(?)
;==============Tables=====================
align	4
CalcInvZTable		dd TABLE_SIZE DUP(?)
align	4
Clip2DTable		dd TABLE_SIZE DUP(?)
align	4
ATypeScrCalcTable	dd TABLE_SIZE DUP(?)
align	4
ATypeDrawTable		dd TABLE_SIZE DUP(?)
;===========ASM Data======================
align 4
_gr_vertices    Vertex 64 DUP(?)
public _gr_vertices
interVert	Vertex 64 DUP(?)
public interVert
align 4
_gr_polygon Polygon 1 DUP(?)
public _gr_polygon
align 4
screen Screen 2048 DUP (?)
public screen
;
align 4
pYDivCache dd 2048 DUP (?) ;+16
public pYDivCache
align 4
p1DivCache dd 2048 DUP (?) ;+16
public p1DivCache

align 4
_LightMask dd ?
_LightPtr   dd ?
_minY       dd ?
public  _minY
;===========C(++) extern==================
align 4
extrn   _gr_pYCache:dword
extrn	_gr_nScreenWidth:dword
extrn	_gr_nScreenHeight:dword
;extrn   _LightTextH:dword
extrn	_gr_clipRect:Clip
;extrn   ASM_PreparePolygonForLight:proc
;extrn   ASM_CreateLightTexture:proc
extrn   ASM_PrepareLightSource:proc
extrn   _gr_pLights:dword
;===========ASM extern=====================
align 4
extrn _gr_ilyaBottom:dword
extrn _gr_ilyaRight:dword

extrn _MulTable16x16:dword
extrn EXT_ScreenWidth:dword
extrn EXT_ScreenWidth1:dword
extrn EXT_ScreenWidth2:dword
extrn EXT_ScreenWidth3:dword
extrn EXT_ScreenWidth4:dword
extrn EXT_ScreenWidth5:dword
extrn EXT_ScreenWidth5_:dword
extrn EXT_ScreenWidth6:dword
extrn EXT_ScreenWidth7:dword
extrn EXT_ScreenWidth8:dword
extrn EXT_ScreenWidth9:dword
extrn EXT_ScreenWidth10:dword
extrn EXT_ScreenWidth11:dword
;extrn EXT_ScreenWidth12:dword
extrn EXT_ScreenWidth13:dword
extrn EXT_ScreenWidth13a:dword
extrn EXT_ScreenWidth13b:dword
extrn EXT_ScreenWidth13c:dword
extrn EXT_ScreenWidth14a:dword
extrn EXT_ScreenWidth14b:dword
extrn EXT_ScreenWidth15a:dword
extrn EXT_ScreenWidth15b:dword
extrn EXT_ScreenWidth16a:dword
extrn EXT_ScreenWidth16b:dword
extrn EXT_ScreenWidth17:dword
extrn EXT_ScreenWidth18a:dword
extrn EXT_ScreenWidth18b:dword
extrn EXT_ScreenWidth19a:dword
extrn EXT_ScreenWidth19b:dword
extrn EXT_ScreenWidth20a:dword
extrn EXT_ScreenWidth20b:dword

;==========================================
extrn	G_Clip:proc
extrn	G_ClipZ:proc
extrn	G_ClipUV:proc
extrn	G_ClipZU:proc
extrn	G_ClipZUV:proc
extrn	G_ClipB:proc
extrn	G_ClipUVB:proc
extrn	G_ClipUB:proc
;------------------------------------------
extrn	G_InvU:proc
extrn	G_InvUV:proc
extrn	G_InvB:proc
extrn	G_InvUVB:proc
extrn	G_InvUB:proc
;---------------Draw-----------------------
extrn	ADrawFlat:proc
extrn	ADrawTexturePer4:proc
extrn	ADrawTexturePer8:proc
extrn	ADrawTexturePer16:proc
extrn	ADrawTexturePer32:proc
extrn	ADrawSprite32:proc
extrn	ADrawLinTexture:proc
extrn   ADrawLinSprite:proc
;extrn   ADrawLinTexturePPro:proc
extrn	ADrawTransparent:proc
extrn	ADrawHaze:proc
extrn   ADrawGouraud:proc
;extrn   ADrawLight:proc
extrn   ADrawAlphaTexture32:proc
;extrn   ADrawAlphaRGBTexture32:proc
extrn   ADrawLightPer8:proc
extrn   ADrawBump32:proc
extrn   ADrawLinSpriteHaze:proc
extrn   ADrawDiserTexture32:proc
;==========================================
END_CODE_SEG
end