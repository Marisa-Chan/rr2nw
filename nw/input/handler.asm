
_PROFILE	equ	1

		.386

		locals

GetDW	Macro	oper,reg,var
		oper	reg,0FACEBEDAh
		org		$-4
	var	dd		0FACEBEDAh
			EndM

UseDW	Macro	oper,arg1,arg2,var
		oper	arg1,arg2
		org		$-4
	var	dd		0FACEBEDAh
			EndM

FarPtr16	Struc
	Offs16	dw	?
	Seg16	dw	?
FarPtr16	EndS

FarPtr32	Struc
	Offs32	dd	?
	Seg32	dw	?
FarPtr32	EndS

DPMIRMCS	Struc
	RMCS_edi	dd	0
	RMCS_esi	dd	0
	RMCS_ebp	dd	0
	RMCS_res0	dd	0
	RMCS_ebx	dd	0
	RMCS_edx	dd	0
	RMCS_ecx	dd	0
	RMCS_eax	dd	0
	RMCS_flags	dw	0
	RMCS_es		dw	0
	RMCS_ds		dw	0
	RMCS_fs		dw	0
	RMCS_gs		dw	0
	RMCS_ip		dw	0
	RMCS_cs		dw	0
	RMCS_sp		dw	0
	RMCS_ss		dw	0
DPMIRMCS	EndS

PITStatus	Struc
	PITCount		dd	?
	PITNewCommon	dd	0
	PITNewOwn		dd	0
PITStatus	EndS

PIT_FACTOR	equ		64


IF _PROFILE
	_TEXT	Segment	Public use32 'CODE'
	ProfileBase:
		public	ProfileBase
	_TEXT	EndS
ENDIF


_TEXT16	Segment	Public use16 'CODE'

		Assume	cs:_TEXT16, ds:Nothing, es:Nothing

		Org		0


RM_BeginTEXT16:

RM_PITStatus	PITStatus	<>

RM_EndTEXT16:

_TEXT16	EndS


_DATA	Segment	Public use32 'DATA'

IF	_PROFILE
extrn _prof_dwLowAddr:dword
extrn _prof_dwAddrSpan:dword
extrn _prof_nShiftStep:byte
extrn _prof_pdwTable:dword
extrn _prof_dwExtraCnt:dword
extrn _prof_dwExtraSegCnt:dword

;extrn _prof_stack:dword
ENDIF

_DATA	EndS


_TEXT	Segment	Public use32 'CODE'


	Assume	cs:_TEXT, ds:_TEXT, es:Nothing


PM_TEXT16_Seg	dw	?
PM_TEXT16_Sel	dw	?

PM_Handlers_Begin:

PM_TEXT16_Addr	dd	?
PM_OldInt8		FarPtr32	<>
PM_OldIntAccum	dd	?
PM_CallBackAccum	dd	?
PM_CallBackPtr		dd	?
PM_CallBackOldStk	FarPtr32	<>
PM_EntryFlag	dd	0

PM_Int8:
	pushf
	push	eax
	push	ds
	mov		ax,_DATA
	mov		ds,ax

	IF	_PROFILE
		Assume	ds:_DATA
		push	es
		push	ecx

		mov		es,[esp+10h+8]	; ret seg 0x80
		mov		eax,[esp+0Ch+8]	; ret addr 0xF16
		cmp		eax,0F16h
		jne		@@prof_extraseg
		mov		es,es:[eax+6]	; thunk stack seg [0x80:0xF1C] = 0x98
		movzx	eax,word ptr es:[esp+18h+8]	; thunk caller frame = 0x4CDC

		movzx	eax,word ptr es:[eax]	; caller bp
		cmp		word ptr es:[eax+2],6BEh ; caller2
		jne		@@prof_extraseg

		movzx	eax,word ptr es:[eax]	; caller2 bp
		mov		ecx,es:[eax+18h]
		mov		es,es:[eax+2]		; interrupt thunk frame

		mov		ax,cs
		cmp		es:[ecx-8],ax	; selector
		jne		@@prof_extraseg
		mov		eax,es:[ecx-12]	; offset
		sub		eax,_prof_dwLowAddr
		jb		@@prof_extra
		cmp		eax,_prof_dwAddrSpan
		jae		@@prof_extra
		mov		cl,_prof_nShiftStep
		shr		eax,cl
		shl		eax,2
		add		eax,_prof_pdwTable
		inc		dword ptr [eax]
		jmp		@@prof_end
	@@prof_extra:
		inc		_prof_dwExtraCnt
		jmp		@@prof_end
	@@prof_extraseg:
		inc		_prof_dwExtraSegCnt
	@@prof_end:
		pop		ecx
		pop		es
	@@P0:
		Assume	ds:_TEXT
	ENDIF

	mov		eax,PM_TEXT16_Addr
	inc		dword ptr ds:[eax+offset RM_PITStatus.PITCount-offset RM_BeginTEXT16]
GetDW sub	PM_CallBackAccum,PM_CallBackStep
	jg		@@DontCallBack
	cmp		PM_EntryFlag,0
	jne		@@DontCallBack
	mov		PM_EntryFlag,1
GetDW sub	PM_OldIntAccum,PM_OldIntStep1
	jg		@@2
	add	PM_OldIntAccum,10000h
	pushf
	call	fword ptr cs:[PM_OldInt8]
	jmp		short @@3
@@2:mov		al,20h
	out		20h,al
@@3:
GetDW add	PM_CallBackAccum,PM_CallBackThreshold
	push	ebx
	push	ecx
	push	edx
	push	es
	mov		PM_CallBackOldStk.Seg32,ss
	mov		PM_CallBackOldStk.Offs32,esp
	mov	ax,_DATA
	mov	es,ax
	mov	ss,ax
	mov		esp,offset ExtraStack
	call	dword ptr PM_CallBackPtr
	mov		ss,PM_CallBackOldStk.Seg32
	mov		esp,PM_CallBackOldStk.Offs32
	pop	es
	pop		edx
	pop		ecx
	pop		ebx
	cli
	mov		PM_EntryFlag,0
	pop		ds
	pop		eax
	popf
	iretd
@@DontCallBack:
GetDW sub	PM_OldIntAccum,PM_OldIntStep
	jg		@@0
	add	PM_OldIntAccum,10000h
	pop		ds
	pop		eax
	popf
	jmp		fword ptr cs:[PM_OldInt8]
@@0:mov		al,20h
	out		20h,al
	pop		ds
	pop		eax
	popf
	iretd



GetPITCount	Proc

	mov		eax,PM_TEXT16_Addr
	mov		eax,ds:[eax+offset RM_PITStatus.PITCount-offset RM_BeginTEXT16]
	ret

GetPITCount	EndP


GetPITGlobalLocal	Proc

	mov		edx,PM_TEXT16_Addr
	cli
	mov		eax,ds:[edx+offset RM_PITStatus.PITNewCommon-offset RM_BeginTEXT16]
	sub		eax,ds:[edx+offset RM_PITStatus.PITNewOwn-offset RM_BeginTEXT16]
	sti
	ret

GetPITGlobalLocal	EndP


GetPITStatusAddr	Proc

	mov		eax,PM_TEXT16_Addr
	add		eax,LARGE(offset RM_PITStatus-offset RM_BeginTEXT16)
	ret

GetPITStatusAddr	EndP

	db	4096 dup (?)
ExtraStack	label	dword

PM_Handlers_End:


HookPIT	Proc		; bool HookPit(dword dwDivider)

	push	ebp
	mov		ebp,esp
	push	ebx
	push	esi
	push	edi


	mov		PM_CallBackPtr,0
	mov		PM_CallBackAccum,1
	mov		PM_CallBackStep,0
	mov		PM_OldIntAccum,10000h
	mov		eax,[ebp+8]	; dwDivider
	mov		PM_OldIntStep,eax
	mov		PM_OldIntStep1,eax

	mov		PM_EntryFlag,0

						; Allocate RM handler memory

	mov		ax,100h
	mov		bx,(offset RM_EndTEXT16-offset RM_BeginTEXT16+15)/16
	int		31h
	jnc		@@0
	mov		eax,0
	jmp		@@Ret
@@0:mov		PM_TEXT16_Sel,dx
	mov		PM_TEXT16_Seg,ax
	and		eax,0FFFFh
	shl		eax,4
	mov		PM_TEXT16_Addr,eax

						; Lock handlers memory regions
	mov		ax,600h
	mov		ebx,offset PM_Handlers_Begin
	mov		ecx,ebx
	shr		ebx,16
	mov		edi,offset PM_Handlers_End-offset PM_Handlers_Begin
	mov		esi,edi
	shr		esi,16
	int		31h

	mov		ax,600h
	mov		ebx,PM_TEXT16_Addr
	mov		ecx,ebx
	shr		ebx,16
	mov		edi,offset RM_EndTEXT16-offset RM_BeginTEXT16
	mov		esi,edi
	shr		esi,16
	int		31h

								; Clear PIT counter
	mov		eax,PM_TEXT16_Addr
	mov		dword ptr ds:[eax+offset RM_PITStatus.PITCount-offset RM_BeginTEXT16],0
	mov		dword ptr ds:[eax+offset RM_PITStatus.PITNewCommon-offset RM_BeginTEXT16],0
	mov		dword ptr ds:[eax+offset RM_PITStatus.PITNewOwn-offset RM_BeginTEXT16],0

						; Store previous handler
	push	es
	mov		ax,3508h
	int		21h
	mov		PM_OldInt8.Offs32,ebx
	mov		PM_OldInt8.Seg32,es
	pop		es

	cli
						; Install 8 handler (PIT)
	mov		ax,2508h
	push	ds
	mov		cx,cs
	mov		ds,cx
	mov		edx,offset PM_Int8
	int		21h
	pop		ds

								; Reprogram PIT
	mov		al,36h
	out		43h,al
	jmp		$+2
	jmp		$+2
	;mov		al,LOW(10000h/PIT_FACTOR)
	mov		eax,[ebp+8]	; dwDivider
	out		40h,al
	jmp		$+2
	jmp		$+2
	;mov		al,HIGH(10000h/PIT_FACTOR)
	mov		al,ah
	out		40h,al

	sti
	mov		eax,1

@@Ret:
	pop		edi
	pop		esi
	pop		ebx
	mov		esp,ebp
	pop		ebp
	ret

HookPIT	EndP


UnhookPIT	Proc Near


	push	ebp
	mov		ebp,esp
	pushf
	push	ebx
	push	esi
	push	edi


	cli

						; Reprogram PIT to original state
	mov		al,36h
	out		43h,al
	jmp		$+2
	jmp		$+2
	mov		al,0
	out		40h,al
	jmp		$+2
	jmp		$+2
	out		40h,al

						; Restore int 8 (PIT)
	mov		ax,2508h
	push	ds
	mov		edx,PM_OldInt8.Offs32
	mov		ds,PM_OldInt8.Seg32
	int		21h
	pop		ds

	sti
						; Unlock handlers memory regions
	mov		ax,601h
	mov		ebx,offset PM_Handlers_Begin
	mov		ecx,ebx
	shr		ebx,16
	mov		edi,offset PM_Handlers_End-offset PM_Handlers_Begin
	mov		esi,edi
	shr		esi,16
	int		31h

	mov		ax,601h
	mov		ebx,PM_TEXT16_Addr
	mov		ecx,ebx
	shr		ebx,16
	mov		edi,offset RM_EndTEXT16-offset RM_BeginTEXT16
	mov		esi,edi
	shr		esi,16
	int		31h

						; Free DOS Mem
	mov		ax,101h
	mov		dx,PM_TEXT16_Sel
	int		31h

	pop		edi
	pop		esi
	pop		ebx
	popf
	mov		esp,ebp
	pop		ebp
	ret

UnhookPIT	EndP


SetPITCallBack	Proc	Near	; int SetPITCallBack(pCallBack,dwDivider)
								; returns actual divider
	cli
	mov		eax,[esp+4]
	mov		PM_CallBackPtr,eax
	or		eax,eax
	jz		@@0
	mov		eax,PM_OldIntStep
	mov		PM_CallBackStep,eax
	mov		eax,[esp+8]
	cmp		eax,PM_OldIntStep
	jge		@@1
	mov		eax,PM_OldIntStep	; Must Threshold >= Step!!!
@@1:mov		PM_CallBackThreshold,eax
	mov		PM_CallBackAccum,eax
	sti
	ret
@@0:mov		PM_CallBackStep,0
	mov		PM_CallBackAccum,1
	sti
	ret

SetPITCallBack	EndP

GetPITCallBack	Proc	Near

	mov		eax,PM_CallBackPtr
	ret

GetPITCallBack	EndP


	public	GetPITCount
	public	HookPIT
	public	UnhookPIT
	public	GetPITGlobalLocal
	public	GetPITStatusAddr
	public	SetPITCallBack
	public	GetPITCallBack



_TEXT	EndS

	End
