    ;Ouput past ANGEL 2.0 preprocessor
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
    ;========================================================================
DrawOst Macro
    local sw$0$7
    local sw$0$6
    local sw$0$5
    local sw$0$4
    local sw$0$3
    local sw$0$2
    local sw$0$1
    local sw$0$0
    local swt$0
    local es$0
        local if$5
        local if$4
        local if$3
        local if$2
        local if$1
        local if$0
    local   EndOst,BrG
    mov     ebx,[_deltaX]
    fistp   [ESP_u0]
    and     ebx,07h
    jz      EndOst
    ;ecx - [L]
    ;ebx - [dL]
    ;ebp - {L}<<16
    ;edx - {dL}<<16
    ;FIXME in switch ecx 60030000-????????????????otkuda 6?????????
    ;mov     [_ECX],ecx
    ;mov     [_EBP],ebp
    shl     ecx,16
    ;Le
    mov     edx,[ESP_u0]        
    ;0|{L}
    shr     ebp,16          
    or      edx,edx
    jle     BrG
    and     edx,1FFFFFh
    BrG:
    ;[L]|{L}
    or      ecx,ebp         
    sub     edx,ecx
    sar     edx,3
    ;--- switch ---
    jmp swt$0[ebx*4-0]
    align 4
    swt$0 label dword
    dd sw$0$0
    dd sw$0$1
    dd sw$0$2
    dd sw$0$3
    dd sw$0$4
    dd sw$0$5
    dd sw$0$6
    dd sw$0$7
    sw$0$7 equ $
        ;L16.16
        mov     eax,ecx 
        ;L+dL
        add     ecx,edx 
        sar     eax,8
        jle if$0
            and     eax,0FFFFh
            mov     al,[edi]
            mov     al,[esi+eax]
            mov     [edi],al
        if$0:
        inc     edi
    sw$0$6 equ $
        ;L16.16
        mov     eax,ecx 
        ;L+dL
        add     ecx,edx 
        sar     eax,8
        jle if$1
            and     eax,0FFFFh
            mov     al,[edi]
            mov     al,[esi+eax]
            mov     [edi],al
        if$1:
        inc     edi
    sw$0$5 equ $
        ;L16.16
        mov     eax,ecx 
        ;L+dL
        add     ecx,edx 
        sar     eax,8
        jle if$2
            and     eax,0FFFFh
            mov     al,[edi]
            mov     al,[esi+eax]
            mov     [edi],al
        if$2:
        inc     edi
    sw$0$4 equ $
        ;L16.16
        mov     eax,ecx 
        ;L+dL
        add     ecx,edx 
        sar     eax,8
        jle if$3
            and     eax,0FFFFh
            mov     al,[edi]
            mov     al,[esi+eax]
            mov     [edi],al
        if$3:
        inc     edi
    sw$0$3 equ $
        ;L16.16
        mov     eax,ecx 
        ;L+dL
        add     ecx,edx 
        sar     eax,8
        jle if$4
            and     eax,0FFFFh
            mov     al,[edi]
            mov     al,[esi+eax]
            mov     [edi],al
        if$4:
        inc     edi
    sw$0$2 equ $
        ;L16.16
        mov     eax,ecx 
        ;L+dL
        add     ecx,edx 
        sar     eax,8
        jle if$5
            and     eax,0FFFFh
            mov     al,[edi]
            mov     al,[esi+eax]
            mov     [edi],al
        if$5:
        inc     edi
    sw$0$1 equ $
        ;L16.16
        mov     eax,ecx 
        or      ecx,ecx
        jle     EndOst
        ;L+dL
        add     ecx,edx 
        sar     eax,8
        and     eax,0FFFFh
        ;$break <=0
        ;jle     EndOst
        ;$if $(or eax,eax) <0
        ;    int 3
        ;$endif
        mov     al,[edi]
        mov     al,[esi+eax]
        mov     [edi],al
    sw$0$0 equ $
 es$0:
    EndOst:
ENDM
    
    
    ;----------------------------------------------------------------------
EndLineDraw Macro name
    fcompp
    fcompp
    fcompp
    ;ffree   st(0)
    ;pdb
    fld     [PDB]   
    ;pdc0 pdb
    fld     [PDC0]  
    ;pdc pdc0 pdb
    fld     [PDC]   
    ;pdb  pdc0 pdc
    fxch    st(2)   
    ;pdb` pdc0 pdc
    fadd    [PDB0]  
    ;pdc0 pdb` pdc
    fxch    st(1)   
    ;pdc0 pdb` pdc`
    fadd    st(2),st
    ;pdc0` pdb` pdc`
    fadd    [PDC1]  
    ;pdb` pdc0` pdc`
    fxch    st(1)   
    fstp    [PDB]
    fxch    st(1)
    fstp    [PDC]
    fstp    [PDC0]
    ;pdb
    fld     [PUB]   
    ;pdc0 pdb
    fld     [PUC0]  
    ;pdc pdc0 pdb
    fld     [PUC]   
    ;pdb pdc0 pdc
    fxch    st(2)   
    ;pdb` pdc0 pdc
    fadd    [PUB0]  
    ;pdc0 pdb` pdc
    fxch    st(1)   
    ;pdc0 pdb` pdc`
    fadd    st(2),st
    ;pdc0` pdb` pdc`
    fadd    [PUC1]  
    ;pdb` pdc0` pdc`
    fxch    st(1)   
    fstp    [PUB]
    fxch    st(1)
    fstp    [PUC]
    fstp    [PUC0]
    mov     ebp,[ESP_startVert]
    mov	edi,[ESP_endVert]
    add     ebp,type Screen
    GetDWPub        add     edi,name
    dec	[ESP_count]
    mov     [ESP_startVert],ebp
    mov	[ESP_endVert],edi
    ;ffree   st(0)
    ;ffree   st(1)
    ;ffree   st(2)
    ;ffree   st(3)
    ;ffree   st(4)
    ;ffree   st(5)
    ;ffree   st(6)
    jge     LightLoopY
    ;
    ;
    ; restore the FPU
    fldcw   [_oldFPUCW]	
    jmp     END_DRAW_LIGHT
ENDM
    
    
    ;========================================================================
    align 8
    ADrawLightPer8:
    Public ADrawLightPer8
    ; store copy of CW
    fstcw   [_oldFPUCW]          
    ; get it in ax
    mov     ax,_oldFPUCW         
    ; 24 bit precision
    and     eax,NOT 1100000000b 
    ; store it
    mov     [_newFPUCW],ax       
    ; load the FPU
    fldcw   [_newFPUCW]          
    mov	edx,[ESP_topVert]
    mov	esi,[ESP_bottomVert]
    mov	ecx,_gr_pYCache
    mov	ebx,[edx].y
    mov	edx,[esi].y
    mov     ebp,offset screen
    ;deltaY
    sub	edx,ebx			
    ;scrPtr
    mov	edi,[ecx + ebx*4]	
    ;mov     eax,[_gr_polygon].textp
    mov     [ESP_count],edx
    mov	[ESP_endVert],edi
    mov     [ESP_startVert],ebp
    ;mov     ebx,[eax + 8]  ;cache
    ;mov     eax,[eax + 12] ;data
    ;TestCoordinates
    ;mov     ds:[?TextureTable32],ebx
    ;mov     ds:[?Texture32],eax
    ;mov     ds:[??TextureTable32],ebx
    ;mov     edx,[ebx + 4]
    ;mov     ds:[???TextureTable32],ebx
    ;mov     ds:[??Texture32],eax
    mov     esi,[_gr_polygon].color
    ;mov     ds:[?TextWidth32],edx
    ;mov     ds:[?AlphaMixTable],eax
    LightLoopY:
    mov     eax,[ebp].xl
    mov     ebx,[ebp].xr
    add	edi,eax
    ;dec	eax
    sub	ebx,eax
    jl      EndDraw
    ;mov     [ESP_scr],edi
    ;cmp     ebx,1
    ;jle     @@Ls3
    inc	ebx
    ;
    ;xl
    fild    [ebp].xl
    ;pda xl
    fld     [PDA]   
    ;N pda xl
    fld     [N]     
    ;xl pda N
    fxch    st(2)   
    ;xl xl pda N
    fld     st      
    ;xl*pda xl pda N
    fmul    st,st(2)
    ;pdb xl*pda xl pda N
    fld     [PDB]   
    ;pda xl*pda xl pdb N
    fxch    st(3)   
    ;pda*N xl*pda xl pdb N
    fmul    st,st(4)
    ;xl*pda pda*N xl pdb N
    fxch    st(1)   
    ;xl*pda xl*pda pda*N xl pdb N
    fld     st      
    ;xl*pda pda*N xl pdb+xl*pda N
    faddp   st(4),st
    ;pdc xl*pda pda*N xl pdb+xl*pda N
    fld     [PDC]   
    ;xl*pda pdc pda*N xl pdb+xl*pda N
    fxch    st(1)   
    ;pda*(N+xl) pdc pda*N xl pdb+xl*pda N
    fadd    st,st(2)
    ;pdb+xl*pda pda*(N+xl) pdc pda*N xl pdb+xl*pda N
    fld     st(4)   
    ;(pdb+xl*pda)*xl pda*(N+xl) pdc pda*N xl pdb+xl*pda N
    fmul    st,st(4)
    ;pda*(N+xl) (pdb+xl*pda)*xl pdc pda*N xl pdb+xl*pda N
    fxch    st(1)   
    ;(pdb+xl*pda)*xl pdc pda*N xl pda*(2*xl+N)+pdb N
    faddp   st(5),st
    ;pda*N pdc (pdb+xl*pda)*xl xl pda*(2*xl+N)+pdb N
    fxch    st(2)   
    ;pda*N*N pdc (pdb+xl*pda)*xl xl pda*(2*xl+N)+pdb N
    fmul    st,st(5)
    ;pdc pda*N*N (pdb+xl*pda)*xl xl pda*(2*xl+N)+pdb N
    fxch    st(1)   
    ;pda*N*N PD xl pda*(2*xl+N)+pdb N
    faddp   st(2),st
    ;pda*(2*xl+N)+pdb PD xl pda*N*N N
    fxch    st(3)   
    ;PD0 PD xl pda*N*N N
    fmul    st,st(4)
    ;pda*N*N PD0 PD xl pda*N*N N
    fld     st(3)   
    ;PD PD0 pda*N*N xl pda*N*N N
    fxch    st(2)   
    ;PD0 pda*N*N xl pda*N*N N
    fstp    [PD]    
    ;pda*N*N PD0 xl pda*N*N N
    fxch    st(1)   
    ;PD0 xl PD1 N
    faddp   st(3),st
    ;pua PD0 xl PD1 N
    fld     [PUA]   
    ;PD0 pua xl PD1 N
    fxch    st(1)   
    ;pua xl PD1 N
    fstp    [PD0]   
    ;PD1 xl pua N
    fxch    st(2)   
    ;xl pua N
    fstp    [PD1]   
    ;-------------------------------------------
    ;xl xl pua N
    fld     st      
    ;xl*pua xl pua N
    fmul    st,st(2)
    ;pub xl*pua xl pua N
    fld     [PUB]   
    ;pua xl*pua xl pub N
    fxch    st(3)   
    ;pua*N xl*pua xl pub N
    fmul    st,st(4)
    ;xl*pua pua*N xl pub N
    fxch    st(1)   
    ;xl*pua xl*pua pua*N xl pub N
    fld     st      
    ;xl*pua pua*N xl pub+xl*pua N
    faddp   st(4),st
    ;puc xl*pua pua*N xl pub+xl*pua N
    fld     [PUC]   
    ;xl*pua puc pua*N xl pub+xl*pua N
    fxch    st(1)   
    ;pua*(N+xl) puc pua*N xl pub+xl*pua N
    fadd    st,st(2)
    ;pub+xl*pua pua*(N+xl) puc pua*N xl pub+xl*pua N
    fld     st(4)   
    ;pua*(N+xl) puc pua*N (pub+xl*pua)*xl pub+xl*pua N
    fmulp   st(4),st
    ;puc pua*N (pub+xl*pua)*xl pua*(2*xl+N)+pub N
    faddp   st(4),st
    ;pua*N puc (pub+xl*pua)*xl pua*(2*xl+N)+pub N
    fxch    st(1)   
    ;pua*N*N puc (pub+xl*pua)*xl pua*(2*xl+N)+pub N
    fmul    st,st(4)
    ;puc pua*N*N (pub+xl*pua)*xl pua*(2*xl+N)+pub N
    fxch    st(1)   
    ;pua*N*N PU pua*(2*xl+N)+pub N
    faddp   st(2),st
    ;pua*(2*xl+N)+pub PU pua*N*N N
    fxch    st(2)   
    ;PU pua*N*N PU0
    fmulp   st(3),st
    ;pua*N*N PU pua*N*N PU0
    fld     st(1)   
    ;PU PU1 PU0
    faddp   st(2),st
    ;PU1 PU PU0
    fxch    st(1)   
    ;-----------------------------------------
    ;L0 PU1 PU PU0
    fld     [L0]    
    ;PD L0 PU1 PU PU0
    fld     [PD]    
    ;PU1 L0 PD PU PU0
    fxch    st(2)   
    ;PU1 L0 PD PU PU0
    fst     [PU1]   
    ;
    ;PU PU1 L0 PD PU PU0
    fld     st(3)   
    ;PU/PD PU1 L0 PD PU PU0
    fdiv    st,st(3)
    ;PU PU1 L0 PD PU/PD PU0
    fxch    st(4)   
    ;PU` PU1 L0 PD PU/PD PU0
    fadd    st,st(5)
    ;PU/PD PU1 L0 PD PU` PU0
    fxch    st(4)   
    ;L    PU1  L0  PD  PU`  PU0
    fsub    st,st(2)
    ;PU0  PU1  L0  PD  PU`  L
    fxch    st(5)   
    ;PU0` PU1  L0  PD  PU`  L
    fadd    st,st(1)
    ;PD0  PU0` PU1 L0  PD   PU` L
    fld     [PD0]   
    ;L    PU0` PU1 L0  PD   PU` PD0
    fxch    st(6)   
    ;PU0` PU1  L0  PD  PU`  PD0
    fistp   [ESP_u0]
    ;PD0  PU1  L0  PD  PU`  PU0`
    fxch    st(5)   
    ;PD0  PU1  L0  PD` PU`  PU0`
    fadd    st(3),st
    ;PD0` PU1  L0  PD` PU`  PU0`
    fadd    [PD1]   
    ;PD`  PU1  L0 PD0` PU`  PU0`
    fxch    st(3)   
    ;PU`  PD` PU1  L0  PD0` PU` PU0`
    fld     st(4)   
    ;
    fdiv    st,st(1)
    mov     ecx,[ESP_u0]
    mov     [_deltaX],ebx
    mov     ebp,ecx
    cmp     ecx,0
    jle     NOCLIP
    and     ecx,1F0000h
    NOCLIP:
    ;[L]
    sar     ecx,16   
    ;{L}<<16
    shl     ebp,16  
    and     ebx,0FFFFFFF8h
    ;L pd pu1 l0 pd0 pu pu0
    fsub    st,st(3)
    jz      Ost
    add     ebx,edi
    mov     ds:[?EndLineAdr32],ebx
    mov     ds:[?EndLineAdr32a],ebx
    mov     ds:[?EndLineAdr32b],ebx
    InnerLoop:
    ;L pd pu1 l0 pd0 pu pu0
    ;pd   pu1 l0 pd0  pu  pu0
    fistp   [ESP_u0]        
    ;pd`  pu1 l0 pd0  pu  pu0
    fadd    st,st(3)        
    ;pd0  pu1 l0 pd`  pu  pu0
    fxch    st(3)           
    ;pd0` pu1 l0 pd`  pu  pu0
    fadd    [PD1]           
    ;pu0  pu1 l0 pd`  pu  pd0`
    fxch    st(5)           
    ;pu0  pu1 l0 pd`  pu` pd0`
    fadd    st(4),st        
    ;pu0` pu1 l0 pd`  pu` pd0`
    fadd    st,st(1)        
    ;pd0` pu1 l0 pd`  pu` pu0`
    fxch    st(5)           
    ;pd`  pu1 l0 pd0` pu` pu0`
    fxch    st(3)           
    ;pu`  pd` pu1 l0 pd0` pu` pu0`
    fld     st(4)           
    ;
    fdiv    st,st(1)        
    ;edi - screen
    ;ecx - [L]
    ;ebp - {L}<<16
    ;edx - Lend
    ;esi - LightTable
    cmp     ecx,0
    mov     edx,[ESP_u0]
    jle     StL0
    ;---------Ls > 0
    cmp     edx,0
    jle     EnL0
    ;---------Ls > 0, Le > 0
    and     edx,1FFFFFh
    ;u
    mov     ebx,edx  
    ;u {Le}<<16
    shl     edx,16   
    ;u [Le]
    sar     ebx,16   
    ;v{dL}<<16
    sub     edx,ebp  
    ;u
    sbb     eax,eax  
    ;v[dL]
    sub     ebx,ecx  
    ;u
    add     ebx,eax  
    xor     eax,eax
    ; v store [L]
    mov     ah,cl 
    ;u
    shrd    edx,ebx,3
    ;u
    sar     ebx,3    
    ;v get first pixel
    mov     al,[edi] 
    ;u
    inc     edi      
    ;v {L} + {dL}
    add     ebp,edx  
    ;u [L] + [dL]
    adc     ecx,ebx  
    ;v
    mov     al,[esi+eax]
    ;u
    mov     [edi-1],al    
    OFF = 0
    REPT    7
    ;v
    mov     al,[edi+OFF]
    ;u
    mov     ah,cl   
    ;v
    add     ebp,edx 
    ;u
    adc     ecx,ebx 
    ;v+1
    mov     al,[esi+eax]
    ;u
    mov     [edi+OFF],al
    OFF = OFF + 1
ENDM
    
    
    add     edi,7
    ;L pd pu1 l0 pd0 pu pu0
    fsub    st,st(3) 
    GetDW   cmp     edi,?EndLineAdr32
    jl      InnerLoop
    Ost:
    DrawOst
    EndDraw:
    EndLineDraw EXT_ScreenWidth13
    ;==========================================================
    ;---------------
    EnL0:
    ;---------Ls > 0, Le <= 0
    ;u
    mov     ebx,edx  
    ;u {Le}<<16
    shl     edx,16   
    ;u [Le]
    sar     ebx,16   
    ;v{dL}<<16
    sub     edx,ebp  
    ;u
    sbb     eax,eax  
    ;v[dL]
    sub     ebx,ecx  
    ;u
    add     ebx,eax  
    xor     eax,eax
    ; v store [L]
    mov     ah,cl 
    ;u
    shrd    edx,ebx,3
    ;u
    sar     ebx,3    
    ;v get first pixel
    mov     al,[edi] 
    ;u
    inc     edi      
    ;v {L} + {dL}
    add     ebp,edx  
    ;u [L] + [dL]
    adc     ecx,ebx  
    ;v
    mov     al,[esi+eax]
    ;u
    mov     [edi-1],al    
    ;OFF = 0
    REPT    7
    local   NoDraw
    jle     NoDraw
    ;v
    mov     ah,cl   
    ;u
    mov     al,[edi]
    ;u+1
    mov     al,[esi+eax]
    ;u
    mov     [edi],al
    NoDraw:
    ;u
    inc     edi     
    ;v
    add     ebp,edx 
    ;u
    adc     ecx,ebx 
ENDM
    
    
    ;add     edi,7
    ;fsub    st,st(3) ;L pd pu1 l0 pd0 pu pu0
    ;GetDW   cmp     edi,?EndLineAdr32
    ;jnz     @@InnerLoop
    ;DrawOst
    ;clear ST(0)
    fcomp   
    EndLineDraw EXT_ScreenWidth13a
    ;==========================================================
    ;------------------
    StL0:
    ;-------Ls <= 0
    cmp     edx,0
    jle     EnL01
    ;-------Ls <= 0, Le > 0
    and     edx,1FFFFFh
    ;u
    mov     ebx,edx  
    ;u {Le}<<16
    shl     edx,16   
    ;u [Le]
    sar     ebx,16   
    ;v{dL}<<16
    sub     edx,ebp  
    ;u
    sbb     eax,eax  
    ;v[dL]
    sub     ebx,ecx  
    ;u
    add     ebx,eax  
    ;v
    inc     edi      
    ;mov     eax,ecx ; v store [L]
    xor     eax,eax
    ;u
    shrd    edx,ebx,3
    ;u
    sar     ebx,3    
    ;mov     ah,[edi] ;v get first pixel
    ;v {L} + {dL}
    add     ebp,edx  
    ;u [L] + [dL]
    adc     ecx,ebx  
    ;mov     al,[esi+eax];v
    ;mov     [edi-1],al    ;u
    ;OFF = 0
    REPT    7
    local   NoDraw
    jle     NoDraw
    ;v
    mov     ah,cl   
    ;u
    mov     al,[edi]
    ;u+1
    mov     al,[esi+eax]
    ;u
    mov     [edi],al
    NoDraw:
    ;u
    inc     edi     
    ;v
    add     ebp,edx 
    ;u
    adc     ecx,ebx 
ENDM
    
    
    fsub    st,st(3)
    GetDW   cmp     edi,?EndLineAdr32a
    jl      InnerLoop
    DrawOst
    EndLineDraw EXT_ScreenWidth13b
    EnL01:
    ;-------Ls <= 0, Le <= 0
    add     edi,8
    mov     ecx,edx
    mov     ebp,edx
    sar     ecx,16
    shl     ebp,16
    fsub    st,st(3)
    GetDW   cmp     edi,?EndLineAdr32b
    jl      InnerLoop
    DrawOst
    EndLineDraw EXT_ScreenWidth13c
    ;==========================================================
    ;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::;
    ;
    ;
    unused dd  1024 DUP(?)
    ;==========ASM extern Labels==============
    align 4
    extrn END_DRAW_LIGHT:proc
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
    ;_ECX    dd ?
    ;_EBP    dd ?
    align 4
    N dd 8.0
    L0 dd ?
    public L0
    PDA dd ?
    public PDA
    PDB dd ?
    public PDB
    PDC dd ?
    public PDC
    PDB0 dd ?
    public PDB0
    PDC0 dd ?
    public PDC0
    PDC1 dd ?
    public PDC1
    PUA dd ?
    public PUA
    PUB dd ?
    public PUB
    PUC dd ?
    public PUC
    PUB0 dd ?
    public PUB0
    PUC0 dd ?
    public PUC0
    PUC1 dd ?
    public PUC1
    PD dd ?
    public PD
    PD0 dd ?
    public PD0
    PD1 dd ?
    public PD1
    PU dd ?
    public PU
    PU0 dd ?
    public PU0
    PU1 dd ?
    public PU1
    END_CODE_SEG
    end
