
AddVertex Macro  AddTwoPoints,End,Count
	local	@@TLessV,@@TEqualLR,@@TLessLR,@@TShiftR
	local   @@TEqualRL,@@TLessRL,@@TShiftL1,@@TAddTwoP

	inc	[nVert]
	mov	eax,[nVert]
	jle	End
	mov	esi,[iL1]
	mov	edi,[iR1]
	cmp	eax,[count]
	jz	AddTwoPoints
	mov	ebx,[iL2]
	mov	eax,[iR2]
	jl	@@TLessV
;---------------------------------
	cmp	ebx,eax
	jz	@@TEqualLR
	jl	@@TLessLR
	mov	esi,[endT]
@@TShiftR:
	mov	eax,[esi].x
	mov	[esi+type Vertex].x,eax
	mov	eax,[esi].y
	mov	[esi+type Vertex].y,eax
	IF	Count GT 2
	mov	eax,[esi+8]
	mov	[esi+type Vertex + 8],eax
	ENDIF
	IF	Count GT 3
	mov	eax,[esi+12]
	mov	[esi+type Vertex + 12],eax
	ENDIF
	IF	Count GT 4
	mov	eax,[esi+16]
	mov	[esi+type Vertex + 16],eax
	ENDIF
	sub	esi,type Vertex
	cmp	esi,ebx
	jge	@@TShiftR
	jmp	@@TAddTwoP
@@TLessLR:
	cmp	ebx,[startT]
	jz	@@TAddTwoP
	mov	[startT],ebx
	mov	edi,eax
	add	edi,type Vertex
	jmp	@@TAddTwoP
@@TEqualLR:
	cmp	ebx,[startT];esi
	jz	@@TAddTwoP
	mov	[startT],ebx
	mov	edi,ebx
	add	edi,type Vertex
	jmp	@@TAddTwoP
;----------Z---------------------------------
@@TLessV:
	cmp	ebx,eax
	jz	@@TEqualLR
	jl	@@TLessRL

	mov	edx,[iR1]
	sub	edx,ebx
	add	edx,(type Vertex)*2
	mov	esi,[endT]
@@TShiftL1:
	mov	eax,[ebx].x
	mov	[ebx+edx].x,eax
	mov	eax,[ebx].y
	mov	[ebx+edx].y,eax
	IF	Count GT 2
	mov	eax,[ebx+8]
	mov	[ebx+edx+8],eax
	ENDIF
	IF	Count GT 3
	mov	eax,[ebx+12]
	mov	[ebx+edx+12],eax
	ENDIF
	IF	Count GT 4
	mov	eax,[ebx+16]
	mov	[ebx+edx+16],eax
	ENDIF
	add	ebx,type Vertex
	cmp	ebx,esi
	jle	@@TShiftL1
	jmp	@@TAddTwoP
@@TLessRL:
	cmp	ebx,[startT]
	jz  @@TAddTwoP
	mov	[startT],ebx
	mov	edi,eax
	add	edi,type Vertex
;--------------------------------------
@@TAddTwoP:
	mov	esi,edi
	add	esi,type Vertex
endm

CalcCrossGor Macro Count,Source,Store
	local	@@Nax,@@Nax1,@@Store,@@Store1,@@NonZ,@@NonZ1,@@NonZ2,@@NonZ3
	;------------------------------- clip x & y
	mov	[Source&1],esi
	mov	[Source&2],edi
	mov	eax,[esi].x
	mov	ebx,[edi].x
	sub	eax,ebx
	shl	eax,DIVPOWER
	mov	edi,[edi].y
	mov	edx,edi
	sub	edx,ecx
	imul	edx
	sub     edi,[esi].y
	idiv	edi
	sar	eax,DIVPOWER
	add	eax,ebx
	mov	[Store],eax
	;------------------------------------------
	IF (Count GT 2)
	mov	edi,[Source&2]
	mov	eax,[esi+8]
	mov	ebx,[edi+8]
	sub	eax,ebx
	shl	eax,DIVPOWER
	mov	edi,[edi].y
	mov	edx,edi
	sub	edx,ecx
	imul	edx
	sub     edi,[esi].y
	idiv	edi
	sar	eax,DIVPOWER
	add	eax,ebx
	mov	[Store&1],eax
	ENDIF

	IF (Count GT 3)
	mov	edi,[Source&2]
	mov	eax,[esi+12]
	mov	ebx,[edi+12]
	sub	eax,ebx
	shl	eax,DIVPOWER
	mov	edi,[edi].y
	mov	edx,edi
	sub	edx,ecx
	imul	edx
	sub     edi,[esi].y
	idiv	edi
	sar	eax,DIVPOWER
	add	eax,ebx
	mov	[Store&2],eax
	ENDIF

	IF (Count GT 4)
	mov	edi,[Source&2]
	mov	eax,[esi+16]
	mov	ebx,[edi+16]
	sub	eax,ebx
	shl	eax,DIVPOWER
	mov	edi,[edi].y
	mov	edx,edi
	sub	edx,ecx
	imul	edx
	sub     edi,[esi].y
	idiv	edi
	sar	eax,DIVPOWER
	add	eax,ebx
	mov	[Store&3],eax
	ENDIF
endm

CalcCrossVer Macro Count,Source,Store
	local	@@Nax,@@Nax1,@@Store,@@Store1,@@NonZ,@@NonZ1,@@NonZ2,@@NonZ3

	mov	[Source&1],esi
	mov	[Source&2],edi
	mov	eax,[esi].y
	mov	ebx,[edi].y
	sub	eax,ebx
	shl	eax,DIVPOWER
	mov	edi,[edi].x
	mov	edx,edi
	sub	edx,ecx
	imul	edx
	sub     edi,[esi].x
	idiv	edi
	sar	eax,DIVPOWER
	add	eax,ebx
	mov	[Store],eax

	IF (Count GT 2)
	mov	edi,[Source&2]
	mov	eax,[esi+8]
	mov	ebx,[edi+8]
	sub	eax,ebx
	shl	eax,DIVPOWER
	mov	edi,[edi].x
	mov	edx,edi
	sub	edx,ecx
	imul	edx
	sub     edi,[esi].x
	idiv	edi
	sar	eax,DIVPOWER
	add	eax,ebx
	mov	[Store&1],eax
	ENDIF

	IF (Count GT 3)
	mov	edi,[Source&2]
	mov	eax,[esi+12]
	mov	ebx,[edi+12]
	sub	eax,ebx
	shl	eax,DIVPOWER
	mov	edi,[edi].x
	mov	edx,edi
	sub	edx,ecx
	imul	edx
	sub     edi,[esi].x
	idiv	edi
	sar	eax,DIVPOWER
	add	eax,ebx
	mov	[Store&2],eax
	ENDIF

	IF (Count GT 4)
	mov	edi,[Source&2]
	mov	eax,[esi+16]
	mov	ebx,[edi+16]
	sub	eax,ebx
	shl	eax,DIVPOWER
	mov	edi,[edi].x
	mov	edx,edi
	sub	edx,ecx
	imul	edx
	sub     edi,[esi].x
	idiv	edi
	sar	eax,DIVPOWER
	add	eax,ebx
	mov	[Store&3],eax
	ENDIF
endm

CrossTop Macro  Count,Exit,End
	local	@@2T,@@3T,@@4T,@@LoopV,@@1,@@2,@@3,@@4
	local	@@RTLoop,@@RT1,@@LT,@@TAddV,@@LTLoop,@@LT1
	local	@@TAddTwo,@@LoopVT,@@1T,@@15T



;----------------------------------------------
;----------------------TOP---------------------
;----------------------------------------------
	mov	ecx,[top]
	cmp	[pT],ecx
	jge	Exit

	mov	esi,[_pT]
	mov	edi,esi
@@RTLoop:
	sub	edi,type Vertex
	cmp	esi,[startT]
	jnz	@@RT1
	mov	edi,[endT]
@@RT1:
	cmp	[edi].y,ecx
	jge	@@LT
	mov	esi,edi
	dec	[nVert]
	jmp	@@RTLoop
;
@@LT:

	CalcCrossGor Count,iR,rew
;--------------------------LT--------------
	mov	esi,[_pT]
	mov	edi,esi
@@LTLoop:
	add	edi,type Vertex
	cmp	esi,[endT]
	jnz	@@LT1
	mov	edi,[startT]
@@LT1:
	cmp	[edi].y,ecx
	jge	@@TAddV
	mov	esi,edi
	dec	[nVert]
	jmp	@@LTLoop
;
@@TAddV:
	CalcCrossGor Count,iL,ff
;
;------------------------ADDV------------------
;
	AddVertex	@@TAddTwo,@@End,Count

@@TAddTwo:
	mov	[esi].y,ecx
	mov	[edi].y,ecx
	mov	[pT],ecx
	mov	ecx,[rew]
	mov	[edi].x,ecx
	mov	ecx,[ff]
	mov	[esi].x,ecx
	IF Count GT 2
	mov	ecx,[rew1]
	mov	[edi+8],ecx
	mov	ecx,[ff1]
	mov	[esi+8],ecx
	ENDIF
	IF Count GT 3
	mov	ecx,[rew2]
	mov	[edi+12],ecx
	mov	ecx,[ff2]
	mov	[esi+12],ecx
	ENDIF
	IF Count GT 4
	mov	ecx,[rew3]
	mov	[edi+16],ecx
	mov	ecx,[ff3]
	mov	[esi+16],ecx
	ENDIF
	mov	ecx,[nVert]
	mov	[count],ecx


	mov	ebx,[startT]
	mov	[_pT],ebx
	mov	[_pB],ebx
	mov	[_pL],ebx
	mov	[_pR],ebx

	mov	@X,[ebx].x;
	mov	@Y,[ebx].y;
;
	mov	[pB],@Y
	mov	[pT],@Y
	mov	[pL],@X
	mov	[pR],@X
	jmp	@@4T
;
@@LoopVT:
	mov	@X,[ebx].x;es
	mov	@Y,[ebx].y;es

@@1T:
	cmp	@Y,[pT]
	jge	@@15T
	mov	[pT],@Y
	mov	[_pT],ebx
	jmp	@@2T
@@15T:
	cmp	@Y,[pB]
	jle	@@2T
	mov	[pB],@Y
	mov	[_pB],ebx
@@2T:
	cmp	@X,[pR]
	jle	@@3T
	mov	[pR],@X
	mov	[_pR],ebx
	jmp	@@4T
@@3T:
	cmp	@X,[pL]
	jge	@@4T
	mov	[pL],@X
	mov	[_pL],ebx
@@4T:
	add	ebx,type Vertex
	dec	ecx
	jnz	@@LoopVT

	sub	ebx,type Vertex
	mov	[endT],ebx
;
;
;
	mov	edx,[left]
	mov	edi,[right]
;
	cmp	[pL],edi
	jg	End
	cmp	[pR],edx
	jl	End

endm

CrossLeft Macro Count,NoLast,Exit,End
	local	@@2L,@@3L,@@4L,@@LoopVL,@@LAddTwo,@@LAddV
	local	@@TLLoop,@@TL1,@@TL,@@BLLoop,@@BL1

;---------------------------------------------
;------------------LEFT-----------------------
;---------------------------------------------
	mov	ecx,[left]
	cmp	[pL],ecx
	jge	Exit

	mov	esi,[_pL]
	mov	edi,esi
@@TLLoop:
	sub	edi,type Vertex
	cmp	esi,[startT]
	jnz	@@TL1
	mov	edi,[endT]
@@TL1:
	cmp	[edi].x,ecx
	jge	@@TL
	mov	esi,edi
	dec	[nVert]
	jmp	@@TLLoop
;
@@TL:
	CalcCrossVer Count,iR,rew
;--------------------------------LB--------------
	mov	esi,[_pL]
	mov	edi,esi
@@BLLoop:
	add	edi,type Vertex
	cmp	esi,[endT]
	jnz	@@BL1
	mov	edi,[startT]
@@BL1:
	cmp	[edi].x,ecx
	jge	@@LAddV
	mov	esi,edi
	dec	[nVert]
	jmp	@@BLLoop
;
@@LAddV:
	CalcCrossVer Count,iL,ff
;
;------------------------ADDV------------------
;

	AddVertex	@@LAddTwo,@@End,Count

@@LAddTwo:
	mov	[esi].x,ecx
	mov	[edi].x,ecx
	IF	NoLast
	mov	[pL],ecx
	ENDIF
	mov	ecx,[rew]
	mov	[edi].y,ecx
	mov	ecx,[ff]
	mov	[esi].y,ecx
	IF Count GT 2
	mov	ecx,[rew1]
	mov	[edi+8],ecx
	mov	ecx,[ff1]
	mov	[esi+8],ecx
	ENDIF
	IF Count GT 3
	mov	ecx,[rew2]
	mov	[edi+12],ecx
	mov	ecx,[ff2]
	mov	[esi+12],ecx
	ENDIF
	IF Count GT 4
	mov	ecx,[rew3]
	mov	[edi+16],ecx
	mov	ecx,[ff3]
	mov	[esi+16],ecx
	ENDIF
	mov	ecx,[nVert]
	IF	NoLast
	mov	[count],ecx

	mov	[_pL],esi
	ENDIF
	mov	ebx,[startT]
	mov	[_pB],ebx
	mov	[_pT],ebx
	IF	NoLast
	mov	[_pR],ebx

	mov	@X,[ebx].x;
	ENDIF
	mov	@Y,[ebx].y;
;
	mov	[pB],@Y
	mov	[pT],@Y
	IF	NoLast
	mov	[pR],@X
	ENDIF
	jmp	@@4L
;
@@LoopVL:
	IF	NoLast
	mov	@X,[ebx].x;es
	ENDIF
	mov	@Y,[ebx].y;es

	cmp	@Y,[pB]
	jle	@@2L
	mov	[pB],@Y
	mov	[_pB],ebx
	jmp	@@3L
@@2L:
	cmp	@Y,[pT]
	jge	@@3L
	mov	[pT],@Y
	mov	[_pT],ebx
@@3L:
	IF	NoLast
	cmp	@X,[pR]
	jle	@@4L
	mov	[pR],@X
	mov	[_pR],ebx
	ENDIF
@@4L:
	add	ebx,type Vertex
	dec	ecx
	jnz	@@LoopVL

	sub	ebx,type Vertex
	mov	[endT],ebx
;
;
;
	mov	edx,[top]
	mov	edi,[bottom]
;
	cmp	[pT],edi
	jg	End
	cmp	[pB],edx
	jl	End

endm

CrossBottom Macro Count,Exit,End
	local	@@LBLoop,@@LB1,@@RB,@@RBLoop,@@RB1,@@BAddV
	local	@@BAddTwo,@@LoopVB,@@2B,@@3B,@@4B,@@1B,@@15B

;---------------------------------------------
;------------------BOTTOM-----------------------
;---------------------------------------------
	mov	ecx,[bottom]
	cmp	[pB],ecx
	jle	Exit

	mov	esi,[_pB]
	mov	edi,esi
@@LBLoop:
	sub	edi,type Vertex
	cmp	esi,[startT]
	jnz	@@LB1
	mov	edi,[endT]
@@LB1:
	cmp	[edi].y,ecx
	jle	@@RB
	mov	esi,edi
	dec	[nVert]
	jmp	@@LBLoop
;
@@RB:
	CalcCrossGor Count,iR,rew
;--------------------------------LT--------------
	mov	esi,[_pB]
	mov	edi,esi
@@RBLoop:
	add	edi,type Vertex
	cmp	esi,[endT]
	jnz	@@RB1
	mov	edi,[startT]
@@RB1:
	cmp	[edi].y,ecx
	jle	@@BAddV
	mov	esi,edi
	dec	[nVert]
	jmp	@@RBLoop
;
@@BAddV:
	CalcCrossGor Count,iL,ff
;
;------------------------ADDV------------------
;
	AddVertex	@@BAddTwo,@@End,Count

@@BAddTwo:
	mov	[esi].y,ecx
	mov	[edi].y,ecx
	mov	[pB],ecx
	mov	ecx,[rew]
	mov	[edi].x,ecx
	mov	ecx,[ff]
	mov	[esi].x,ecx
	IF Count GT 2
	mov	ecx,[rew1]
	mov	[edi+8],ecx
	mov	ecx,[ff1]
	mov	[esi+8],ecx
	ENDIF
	IF Count GT 3
	mov	ecx,[rew2]
	mov	[edi+12],ecx
	mov	ecx,[ff2]
	mov	[esi+12],ecx
	ENDIF
	IF Count GT 4
	mov	ecx,[rew3]
	mov	[edi+16],ecx
	mov	ecx,[ff3]
	mov	[esi+16],ecx
	ENDIF
	mov	ecx,[nVert]
	mov	[count],ecx

	mov	ebx,[startT]
	mov	[_pT],ebx
	mov	[_pB],ebx
	mov	[_pL],ebx
	mov	[_pR],ebx

	mov	@X,[ebx].x;
	mov	@Y,[ebx].y;
;
	mov	[pB],@Y
	mov	[pT],@Y
	mov	[pL],@X
	mov	[pR],@X
	jmp	@@4B
;
@@LoopVB:
	mov	@X,[ebx].x;es
	mov	@Y,[ebx].y;es

@@1B:
	cmp	@Y,[pT]
	jge	@@15B
	mov	[pT],@Y
	mov	[_pT],ebx
	jmp	@@2B
@@15B:
	cmp	@Y,[pB]
	jle	@@2B
	mov	[pB],@Y
	mov	[_pB],ebx
@@2B:
	cmp	@X,[pR]
	jle	@@3B
	mov	[pR],@X
	mov	[_pR],ebx
	jmp	@@4B
@@3B:
	cmp	@X,[pL]
	jge	@@4B
	mov	[pL],@X
	mov	[_pL],ebx
@@4B:
	add	ebx,type Vertex
	dec	ecx
	jnz	@@LoopVB

	sub	ebx,type Vertex
	mov	[endT],ebx
;
;
;
	mov	edx,[left]
	mov	edi,[right]
;
	cmp	[pL],edi
	jg	End
	cmp	[pR],edx
	jl	End

endm

CrossRight Macro Count,NoLast,Exit,End
	local	@@BRLoop,@@BR1,@@TR,@@TR1,@@TRLoop,@@RAddV
	local	@@RAddTwo,@@LoopVR,@@2R,@@3R,@@4R


;---------------------------------------------
;------------------RIGHT-----------------------
;---------------------------------------------
	mov	ecx,[right]
	cmp	[pR],ecx
	jle	Exit

	mov	esi,[_pR]
	mov	edi,esi
@@BRLoop:
	sub	edi,type Vertex
	cmp	esi,[startT]
	jnz	@@BR1
	mov	edi,[endT]
@@BR1:
	cmp	[edi].x,ecx
	jle	@@TR
	mov	esi,edi
	dec	[nVert]
	jmp	@@BRLoop
;
@@TR:
	CalcCrossVer Count,iR,rew
;--------------------------------LB--------------
	mov	esi,[_pR]
	mov	edi,esi
@@TRLoop:
	add	edi,type Vertex
	cmp	esi,[endT]
	jnz	@@TR1
	mov	edi,[startT]
@@TR1:
	cmp	[edi].x,ecx
	jle	@@RAddV
	mov	esi,edi
	dec	[nVert]
	jmp	@@TRLoop
;
@@RAddV:
	CalcCrossVer Count,iL,ff
;
;------------------------ADDV------------------
;
	AddVertex	@@RAddTwo,@@End,Count

@@RAddTwo:
	mov	[esi].x,ecx
	mov	[edi].x,ecx
	IF	NoLast
	mov	[pR],ecx
	ENDIF
	mov	ecx,[rew]
	mov	[edi].y,ecx
	mov	ecx,[ff]
	mov	[esi].y,ecx
	IF Count GT 2
	mov	ecx,[rew1]
	mov	[edi+8],ecx
	mov	ecx,[ff1]
	mov	[esi+8],ecx
	ENDIF
	IF Count GT 3
	mov	ecx,[rew2]
	mov	[edi+12],ecx
	mov	ecx,[ff2]
	mov	[esi+12],ecx
	ENDIF
	IF Count GT 4
	mov	ecx,[rew3]
	mov	[edi+16],ecx
	mov	ecx,[ff3]
	mov	[esi+16],ecx
	ENDIF
	mov	ecx,[nVert]
	IF	NoLast
	mov	[count],ecx

	mov	[_pR],esi
	ENDIF
	mov	ebx,[startT]
	mov	[_pB],ebx
	mov	[_pT],ebx
	IF	NoLast
	mov	[_pL],ebx
	ENDIF
	;mov	[_pR],ebx

	IF	NoLast
	mov	@X,[ebx].x
	mov	[pL],@X
	ENDIF

	mov	@Y,[ebx].y;
;
	mov	[pB],@Y
	mov	[pT],@Y
	;mov	[pR],@X
	jmp	@@4R
;
@@LoopVR:
	IF	NoLast
	mov	@X,[ebx].x;es
	ENDIF

	mov	@Y,[ebx].y;es

	cmp	@Y,[pB]
	jle	@@2R
	mov	[pB],@Y
	mov	[_pB],ebx
	jmp	@@3R
@@2R:
	cmp	@Y,[pT]
	jge	@@3R
	mov	[pT],@Y
	mov	[_pT],ebx
@@3R:
	IF	NoLast
	cmp	@X,[pL]
	jge	@@4R
	mov	[pL],@X
	mov	[_pL],ebx
	ENDIF
@@4R:
	add	ebx,type Vertex
	dec	ecx
	jnz	@@LoopVR

	sub	ebx,type Vertex
	mov	[endT],ebx
;
;
;
	mov	edx,[top]
	mov	edi,[bottom]
;
	cmp	[pT],edi
	jg	End
	cmp	[pB],edx
	jl	End

endm

