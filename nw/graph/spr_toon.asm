;$align 4
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


FIX_SHIFT_4	equ 2
FIX_MASK_4	equ 3


ESP_v1       equ dword ptr [esp + 16]
ESP_v0       equ dword ptr [esp + 20]
ESP_u1       equ dword ptr [esp + 24]
ESP_u0       equ dword ptr [esp + 28]
ESP_y1       equ dword ptr [esp + 32]
ESP_x1       equ dword ptr [esp + 36]
ESP_y0       equ dword ptr [esp + 40]
ESP_x0       equ dword ptr [esp + 44]
ESP_delta    equ dword ptr [esp + 48]

;
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


align 8
ASM_GRDrawSprite:
        Public ASM_GRDrawSprite
draw4sprite proc x0:dword, y0:dword, x1:dword, y1:dword, u0:dword, v0:dword, u1:dword, v1:dword, izi:dword, hTex:dword
        local _delta:dword
        local _x0:dword,_y0:dword,_x1:dword,_y1:dword
        local _u0:dword,_u1:dword,_v0:dword,_v1:dword

        uses ebx,esi,edi

        mov     ebx,[izi]
        cmp     ebx,?HazeStartG
        jge     @@NoHaze

        push    D_P 0
        call    ASM_GRSetZPrecision
        add     esp,4

        mov     ecx,[hTex]
        mov     [_gr_polygon].typall,TEXTURE_L_TRSP_TYPE
        mov     [_gr_polygon].addtyp,HAZE_ADDTYPE
        mov     [_gr_polygon].vertc,4
        mov     [_gr_polygon].textp,ecx

        mov     eax,[x0]
        mov     ecx,[y0]
        mov     edx,[x1]
        mov     esi,[y1]

        mov     [_gr_vertices].x,eax
        mov     [_gr_vertices].y,ecx
        mov     [_gr_vertices].iz,ebx

        mov     [_gr_vertices + (type Vertex)*1].x,eax
        mov     [_gr_vertices + (type Vertex)*1].y,esi
        mov     [_gr_vertices + (type Vertex)*1].iz,ebx

        mov     [_gr_vertices + (type Vertex)*2].x,edx
        mov     [_gr_vertices + (type Vertex)*2].y,esi
        mov     [_gr_vertices + (type Vertex)*2].iz,ebx

        mov     [_gr_vertices + (type Vertex)*3].x,edx
        mov     [_gr_vertices + (type Vertex)*3].y,ecx
        mov     [_gr_vertices + (type Vertex)*3].iz,ebx

        mov     ebx,[u0]
        mov     ecx,[v0]
        mov     edx,[u1]
        mov     esi,[v1]

        mov     [_gr_vertices].u,ebx
        mov     [_gr_vertices].v,ecx

        mov     [_gr_vertices + (type Vertex)*1].u,ebx
        mov     [_gr_vertices + (type Vertex)*1].v,esi

        mov     [_gr_vertices + (type Vertex)*2].u,edx
        mov     [_gr_vertices + (type Vertex)*2].v,esi

        mov     [_gr_vertices + (type Vertex)*3].u,edx
        mov     [_gr_vertices + (type Vertex)*3].v,ecx

        call    ASM_GRDrawPolygonPCCW

        ret
align 4
@@NoHaze:
        mov     ebx,[x0]
        mov     eax,[x1]

        cmp     ebx,_gr_ilyaRight
        jg      @@End
        cmp     eax,_gr_clipRect.left
        jl      @@End

        mov     ecx,[y0]
        mov     esi,[y1]

        cmp     ecx,_gr_ilyaBottom
        jg      @@End
        cmp     esi,_gr_clipRect.top
        jl      @@End


        mov     [_x1],eax
        mov     [_x0],ebx
        mov     edi,[u1]
        mov     edx,[u0]
        mov     [_u1],edi
        sub     edi,edx         ;u1 - u0
        mov     [_u0],edx
        mov     [_delta],edi

        cmp     ebx,_gr_clipRect.left
        jge     @@NoClipLeft

        sub     eax,ebx                 ;x1-x0
        mov     edx,_gr_clipRect.left
        mov     edi,eax                 ;x1-x0
        mov     eax,edx                 ;left
        sub     eax,ebx                 ;left-x0

        imul    [_delta]                ;(left-x0)*(u1-u0)
        idiv    edi                     ;(left-x0)*(u1-u0)/(x1-x0)

        mov     ebx,_gr_clipRect.left
        add     [_u0],eax
        mov     eax,[x1]
        mov     [_x0],ebx

@@NoClipLeft:

        cmp     eax,_gr_ilyaRight
        jle     @@NoClipRight

        sub     eax,ebx
        mov     edx,_gr_ilyaRight
        mov     edi,eax
        mov     eax,edx
        sub     eax,ebx

        imul    [_delta]
        idiv    edi

        mov     edi,[u0]
        mov     ebx,_gr_ilyaRight
        add     edi,eax
        mov     [_x1],ebx
        mov     [_u1],edi

@@NoClipRight:

        mov     [_y1],esi
        mov     [_y0],ecx
        mov     edi,[v1]
        mov     edx,[v0]
        mov     [_v1],edi
        sub     edi,edx
        mov     [_v0],edx
        mov     [_delta],edi

        cmp     ecx,_gr_clipRect.top
        jge     @@NoClipTop

        mov     eax,_gr_clipRect.top
        sub     esi,ecx
        sub     eax,ecx

        imul    [_delta]
        idiv    esi

        mov     edi,_gr_clipRect.top
        add     [_v0],eax
        mov     esi,[y1]
        mov     [_y0],edi

@@NoClipTop:

        cmp     esi,_gr_ilyaBottom
        jle     @@NoClipBottom

        mov     eax,_gr_ilyaBottom
        sub     esi,ecx
        sub     eax,ecx

        imul    [_delta]
        idiv    esi

        mov     edi,[v0]
        mov     esi,_gr_ilyaBottom
        add     edi,eax
        mov     [_y1],esi
        mov     [_v1],edi

@@NoClipBottom:

	mov	ecx,_gr_pYCache
        mov     esi,[_y0]
        mov     eax,[hTex]

        mov     edi,[ecx + esi*4]       ;scrPtr

        mov     esi,[eax + 8]  ;cache
        mov     [_delta],edi
        mov     eax,[eax + 12] ;textp

        mov     ds:[?TextureTable],esi
        mov     ds:[??TextureTable],esi
        mov     ecx,[esi+4]
        mov     ds:[?Texture],eax
        ;mov     ds:[??TextureTable],esi
        mov     ds:[?TextureWidth],ecx

        push    ebp

        mov     eax,[ESP_u1]
        mov     ebp,[ESP_x1]
        mov     esi,[ESP_u0]
        mov     edi,[ESP_x0]
        sub     eax,esi
        mov     ds:[?X1],ebp
        sub     ebp,edi
        mov     ds:[?X0],edi
        jg      @@Gr0
        mov     ebp,1
@@Gr0:
        cdq
        idiv    ebp
        mov     edx,eax
        shl     eax,16          ;{dU}<<16
        sar     edx,16          ;[dU]
        mov     ds:[?dUFrack],eax
        ;
        ;esi - u0
        ;edi - x0
        ;edx - [dU]
        ;
        mov     ebp,edx

        mov     eax,[ESP_v1]
        mov     ecx,[ESP_y1]
        mov     ebx,[ESP_y0]
        sub     eax,[ESP_v0]
        sub     ecx,ebx
        mov     [ESP_y0],ecx

        jg      @@Gr0_
        mov     ecx,1
@@Gr0_:
        cdq
        idiv    ecx
        mov     ecx,eax
        sar     eax,16
        shl     ecx,16  ;{dV}
        mov     edx,[?TextureTable + 4*eax]    ;[dV]*TextWidth
	org	$-4
        ?TextureTable dd 0FACEBEDAh

        mov     ds:[?dVFrack],ecx
        mov     ecx,[ESP_v0]
        mov     ebx,ecx
        shl     ecx,16
        mov     ds:[_TextureVW],edx
        mov     ds:[?UFrack],ecx
        GetDW   add     edx,?TextureWidth
        sar     esi,16
        mov     edi,[ESP_delta] ;screen
        mov     ds:[_TextureVWW],edx
        GetDW   add     esi,?Texture
        sar     ebx,16

        mov     edx,ebp         ;[dU]

        add     esi,[??TextureTable + 4*ebx]    ;[u0]+[v0]*TextWidth
	org	$-4
        ??TextureTable dd 0FACEBEDAh
        ;esi - texture
        mov     ebp,edi
        mov     [ESP_x0],esi

@@LoopY:
        mov     ecx,edi
        GetDW   add     edi,?X0
        GetDW   add     ecx,?X1
        GetDW   mov     eax,?UFrack

@@LoopX:
        cmp     edi,ecx
        jg      @@EndLoopX
        GetDW   add     eax,?dUFrack
        mov     bl,[esi]
        adc     esi,edx
        inc     edi
        or      bl,bl
        jz      @@LoopX
        mov     [edi-1],bl
        jmp     @@LoopX
align 4
@@EndLoopX:
        mov     ecx,[ESP_x0]    ;texture
        mov     edi,[ESP_delta] ;{V}
        GetDWPub   add     ebp,EXT_ScreenWidth17
        GetDW   add     edi,?dVFrack    ;{V} + {dV}
        sbb     eax,eax
        mov     [ESP_delta],edi
        add     ecx,[_TextureVW + eax*4]
        mov     edi,ebp
        mov     [ESP_x0],ecx
        mov     esi,ecx
        dec     [ESP_y0]
        jge     @@LoopY

        pop     ebp
@@End:

        ret
endP

;=============================================================
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::;
;
unused dd  1024 DUP(?)
;==========ASM extern Labels==============
;align 4
;extrn END_TYPE_DRAW:proc
;extrn ENDD:proc
;==========ASM extern=====================
align 4
extrn _gr_ilyaBottom:dword
extrn _gr_ilyaRight:dword
extrn _gr_vertices:dword
;extrn interVert:dword
extrn _gr_polygon:dword
;extrn screen:dword
extrn pYDivCache:dword
extrn ?HazeStartG:dword
extrn ASM_GRDrawPolygonPCCW:proc
extrn ASM_GRSetZPrecision:proc
;===========C(++) extern==================
align 4
extrn   _gr_pYCache:dword
extrn   _gr_clipRect:Clip
extrn   p1DivCache:dword
;extrn   _gr_nScreenWidth:dword
;=============Module Data=================
align 4
_TextureVWW dd ?
_TextureVW  dd ?
;=========================================
align 4
END_CODE_SEG
end
