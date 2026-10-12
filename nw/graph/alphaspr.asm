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
ASM_GRDrawAlphaSprite:
        Public ASM_GRDrawAlphaSprite
drawSmoke proc sm:dword
        local _delta:dword
        local _x0:dword,_y0:dword,_x1:dword,_y1:dword
        local _u0:dword,_u1:dword,_v0:dword,_v1:dword

        uses ebx,esi,edi

        mov     edi,[sm]
        mov     ebx,[edi].invz
        cmp     ebx,?HazeStartG
        jge     @@NoHaze

        ret
comment `
        mov     eax,[ebx].izl
        mov     cl,?ZShift
        mov     esi,?HazeStartG
        sar     eax,cl
        mov     ecx,_gr_pHaze
        mov     ebp,?HazeEndL

        add     ecx,256*15
        cmp     eax,esi
        jge     SaveHazePtr
        cmp     eax,ebp
        jle     FullHaze
        ;----compute haze
        sub     eax,esi ;(r-HS)
        mov     edx,15
        sub     ebp,esi ;(HE-HS)
        imul    edx;15
        idiv    ebp
        and     eax,0Fh
        sal     eax,8
        sub     ecx,eax
        jmp     SaveHazePtr

align 4
FullHaze:
        sub     ecx,256*15

SaveHazePtr:


comment
        push    D_P 0
        call    ASM_GRSetZPrecision
        add     esp,4

        mov     eax,[edi].colorS
        mov     ecx,[edi].hText
        mov     [_gr_polygon].typall,TEXTURE_ALPHA_TYPE
        mov     [_gr_polygon].addtyp,HAZE_ADDTYPE
        mov     [_gr_polygon].vertc,4
        mov     [_gr_polygon].color,eax
        mov     [_gr_polygon].textp,ecx

        mov     eax,[edi].xb
        mov     ecx,[edi].yb
        mov     edx,[edi].xe
        mov     esi,[edi].ye

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

        mov     ebx,[edi].us
        mov     ecx,[edi].vs
        mov     edx,[edi].ue
        mov     esi,[edi].ve

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
`
align 4
@@NoHaze:
        mov     ebx,[edi].xb
        mov     eax,[edi].xe

        cmp     ebx,_gr_ilyaRight
        jg      @@End
        cmp     eax,_gr_clipRect.left
        jl      @@End

        mov     ecx,[edi].yb
        mov     esi,[edi].ye

        cmp     ecx,_gr_ilyaBottom
        jg      @@End
        cmp     esi,_gr_clipRect.top
        jl      @@End

        push    ebp
        ;

        mov     [ESP_x1],eax
        mov     [ESP_x0],ebx
        mov     ebp,[edi].ue
        mov     edx,[edi].us
        mov     [ESP_u1],ebp
        sub     ebp,edx
        mov     [ESP_u0],edx
        mov     [ESP_delta],ebp

        cmp     ebx,_gr_clipRect.left
        jge     @@NoClipLeft

        sub     eax,ebx
        mov     edx,_gr_clipRect.left
        mov     ebp,eax
        mov     eax,edx
        sub     eax,ebx

        imul    [ESP_delta]
        idiv    ebp

        mov     ebx,_gr_clipRect.left
        add     [ESP_u0],eax
        mov     eax,[edi].xe
        mov     [ESP_x0],ebx

@@NoClipLeft:

        cmp     eax,_gr_ilyaRight
        jle     @@NoClipRight

        sub     eax,ebx
        mov     edx,_gr_ilyaRight
        mov     ebp,eax
        mov     eax,edx
        sub     eax,ebx

        imul    [ESP_delta]
        idiv    ebp

        mov     ebp,[edi].us
        mov     ebx,_gr_ilyaRight
        add     ebp,eax
        mov     [ESP_x1],ebx
        mov     [ESP_u1],ebp

@@NoClipRight:

        mov     [ESP_y1],esi
        mov     [ESP_y0],ecx
        mov     ebp,[edi].ve
        mov     edx,[edi].vs
        mov     [ESP_v1],ebp
        sub     ebp,edx
        mov     [ESP_v0],edx
        mov     [ESP_delta],ebp

        cmp     ecx,_gr_clipRect.top
        jge     @@NoClipTop

        mov     eax,_gr_clipRect.top
        sub     esi,ecx
        sub     eax,ecx

        imul    [ESP_delta]
        idiv    esi

        mov     ebp,_gr_clipRect.top
        add     [ESP_v0],eax
        mov     esi,[edi].ye
        mov     [ESP_y0],ebp

@@NoClipTop:

        cmp     esi,_gr_ilyaBottom
        jle     @@NoClipBottom

        mov     eax,_gr_ilyaBottom
        sub     esi,ecx
        sub     eax,ecx

        imul    [ESP_delta]
        idiv    esi

        mov     ebp,[edi].vs
        mov     esi,_gr_ilyaBottom
        add     ebp,eax
        mov     [ESP_y1],esi
        mov     [ESP_v1],ebp

@@NoClipBottom:

	mov	ecx,_gr_pYCache
        mov     esi,[ESP_y0]
        mov     eax,[edi].hText

        mov     ebp,[ecx + esi*4]       ;scrPtr

        mov     ebx,[edi].alpha

        mov     esi,[eax + 8]  ;cache
        mov     [ESP_delta],ebp
        shr     ebx,4
        mov     eax,[eax + 12] ;textp

        cmp     ebx,15
        jl      @@DrawWithAlpha

        ;Draw With Alpha 255

        mov     ds:[?TextureTable],esi
        mov     ds:[??TextureTable],esi
        mov     ecx,[esi+4]
        mov     ds:[?Texture],eax
        ;mov     ds:[??TextureTable],esi
        mov     eax,[edi].colorS
        mov     ds:[?TextureWidth],ecx
        mov     ds:[?TransparentTable],eax

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
        mov     edx,[0FACEBEDAh + 4*eax]    ;[dV]*TextWidth
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

        add     esi,[0FACEBEDAh + 4*ebx]    ;[u0]+[v0]*TextWidth
	org	$-4
        ??TextureTable dd 0FACEBEDAh
        ;esi - texture
        mov     ebp,edi
        mov     [ESP_x0],esi
        xor     ebx,ebx

@@LoopY:
        mov     ecx,edi
        GetDW   add     edi,?X0
        GetDW   add     ecx,?X1
        GetDW   mov     eax,?UFrack

@@LoopX:
        GetDW   add     eax,?dUFrack
        mov     bl,[edi]
        inc     edi
        mov     bh,[esi]
        adc     esi,edx
        mov     bl,[0FACEBEDAh+ebx] ;Transparent Table
	org	$-4
        ?TransparentTable dd 0FACEBEDAh
        cmp     edi,ecx
        mov     [edi-1],bl
        jl      @@LoopX

@@EndLoopX:
        mov     ecx,[ESP_x0]    ;texture
        mov     edi,[ESP_delta] ;{V}
        GetDWPub   add     ebp,EXT_ScreenWidth18a
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

        ret

;========================================

@@DrawWithAlpha:

        ;Draw With Alpha

        mov     ds:[?TextureTable_],esi
        mov     ds:[??TextureTable_],esi
        mov     ecx,[esi+4]
        mov     ds:[?Texture_],eax
        ;mov     ds:[??TextureTable],esi
        mov     eax,[edi].colorS
        mov     ebp,offset _MulTable16x16
        ;and     ebx,0F0h
        shl     ebx,4
        mov     ds:[?TextureWidth_],ecx
        mov     ds:[?TransparentTable_],eax
        add     ebp,ebx

        mov     eax,[ESP_u1]
        mov     ds:[?MulTable],ebp
        mov     ebp,[ESP_x1]
        mov     esi,[ESP_u0]
        mov     edi,[ESP_x0]
        sub     eax,esi
        mov     ds:[?X1_],ebp
        sub     ebp,edi
        mov     ds:[?X0_],edi
        jg      @@Gr0__
        mov     ebp,1
@@Gr0__:
        cdq
        idiv    ebp
        mov     edx,eax
        shl     eax,16          ;{dU}<<16
        sar     edx,16          ;[dU]
        mov     ds:[?dUFrack_],eax
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
        jg      @@Gr0___
        mov     ecx,1
@@Gr0___:
        cdq
        idiv    ecx
        mov     ecx,eax
        sar     eax,16
        shl     ecx,16  ;{dV}
        mov     edx,[?TextureTable_ + 4*eax]    ;[dV]*TextWidth
	org	$-4
        ?TextureTable_ dd 0FACEBEDAh

        mov     ds:[?dVFrack_],ecx
        mov     ecx,[ESP_v0]
        mov     ebx,ecx
        shl     ecx,16
        mov     ds:[_TextureVW_],edx
        mov     ds:[?UFrack_],ecx
        GetDW   add     edx,?TextureWidth_
        sar     esi,16
        mov     edi,[ESP_delta] ;screen
        mov     ds:[_TextureVWW_],edx
        GetDW   add     esi,?Texture_
        sar     ebx,16

        mov     edx,ebp         ;[dU]

        add     esi,[??TextureTable_ + 4*ebx]    ;[u0]+[v0]*TextWidth
	org	$-4
        ??TextureTable_ dd 0FACEBEDAh
        ;esi - texture
        mov     ebp,edi
        mov     [ESP_x0],esi
        xor     ebx,ebx

@@LoopY_:
        mov     ecx,edi
        GetDW   add     edi,?X0_
        GetDW   add     ecx,?X1_
        GetDW   mov     eax,?UFrack_
        mov     ds:[?EndLineAddress],ecx
        xor     ecx,ecx

@@LoopX_:
        GetDW   add     eax,?dUFrack_
        mov     cl,[esi]
        adc     esi,edx
        mov     bl,[edi]
        inc     edi
        mov     bh,[0FACEBEDAh+ecx]
        org     $-4
        ?MulTable dd 0FACEBEDAh

        mov     bl,[0FACEBEDAh+ebx] ;Transparent Table
	org	$-4
        ?TransparentTable_ dd 0FACEBEDAh

        GetDW   cmp     edi,?EndLineAddress
        mov     [edi-1],bl
        jl      @@LoopX_

;@@EndLoopX_:
        mov     ecx,[ESP_x0]    ;texture
        mov     edi,[ESP_delta] ;{V}
        GetDWPub   add     ebp,EXT_ScreenWidth18b
        GetDW   add     edi,?dVFrack_    ;{V} + {dV}
        sbb     eax,eax
        mov     [ESP_delta],edi
        add     ecx,[_TextureVW_ + eax*4]
        mov     edi,ebp
        mov     [ESP_x0],ecx
        mov     esi,ecx
        dec     [ESP_y0]
        jge     @@LoopY_

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
_TextureVWW_ dd ?
_TextureVW_  dd ?

_MulTable16x16 db 256 DUP (?) ;+16
Public _MulTable16x16

;=========================================
align 4
END_CODE_SEG
end