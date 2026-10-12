InnerTextureLoop4 Macro textureW
	;4.5 ticks/pixel
	;
	;draw 4 pixels of texture
	;eax - {v}0{u}
	;ebx - store pixels
	;ecx - [du] + [dv]*TextureWidth
	;edx - TextureWidth
	;esi - Texture Pointer
	;edi - {dv}0{du}
	;ebp - carry


	mov	bl,[esi]        ;U
	add	eax,edi		; V {v}+{dv}|{u}+{du}
	sbb	ebp,ebp		;U FFFFFFFF if need add TextureWidth(v)
	add	ah,80h		; V set carry if need add 1(u)
	adc	esi,ecx         ;U
	and	ebp,textureW		; V 0 or TextureWidth
	add	esi,ebp         ;U
	and	ah,7Fh		; V restore eax and set 15 bit to zero
    				;
	add	eax,edi		;U {v}+{dv}|{u}+{du}
	sbb	ebp,ebp		;U FFFFFFFF if need add TextureWidth(v)
	mov	bh,[esi]        ; V
	and	ebp,textureW		;U 0 or TextureWidth
	add	ah,80h          ; V
	adc	esi,ecx         ;U
	and	ah,7Fh		; V restore eax and set 15 bit to zero
	add	esi,ebp         ;U
    				;
	add	eax,edi		; V {v}+{dv}|{u}+{du}
	sbb	ebp,ebp		;U FFFFFFFF if need add TextureWidth(v)
	shl	ebx,16          ; V
	mov	bl,[esi]        ;U
	add	ah,80h		; V set carry if need add 1(u)
	adc	esi,ecx         ;U
	and	ebp,textureW		; V 0 or TextureWidth
	add	esi,ebp         ;U
	and	ah,7Fh		; V restore eax and set 15 bit to zero
    				;
	add	eax,edi		;U {v}+{dv}|{u}+{du}
	sbb	ebp,ebp		;U FFFFFFFF if need add TextureWidth(v)
	mov	bh,[esi]        ; V
   	and	ebp,textureW		;U 0 or TextureWidth
	add	ah,80h		; V set carry if need add 1(u)
   	adc	esi,ecx         ;U
	and	ah,7Fh          ; V
   	rol	ebx,16          ;U
   	add	esi,ebp         ; V
endM

InnerTextureLoop4Mid Macro textureW,screenOff
	;5 ticks/pixel

	mov	ebp,[ESP_scr]   ;U
	add	eax,edi		; V {v}+{dv}|{u}+{du}
	mov	[ebp+screenOff],ebx       ;U+1
	mov	bl,[esi]        ; V
	sbb	ebp,ebp         ;U
	add	ah,80h		; V set carry if need add 1(u)
	adc	esi,ecx         ;U
	and	ebp,textureW		; V 0 or TextureWidth
	add	esi,ebp         ;U
	and	ah,7Fh		; V restore eax and set 15 bit to zero
    				;
	add	eax,edi		;U {v}+{dv}|{u}+{du}
	sbb	ebp,ebp		;U FFFFFFFF if need add TextureWidth(v)
	mov	bh,[esi]        ; V
	and	ebp,textureW		;U 0 or TextureWidth
	add	ah,80h          ; V
	adc	esi,ecx         ;U
	and	ah,7Fh		; V restore eax and set 15 bit to zero
	add	esi,ebp         ;U
    				;
	add	eax,edi		; V {v}+{dv}|{u}+{du}
	sbb	ebp,ebp		;U FFFFFFFF if need add TextureWidth(v)
	shl	ebx,16          ; V
	mov	bl,[esi]        ;U
	add	ah,80h		; V set carry if need add 1(u)
	adc	esi,ecx         ;U
	and	ebp,textureW		; V 0 or TextureWidth
	add	esi,ebp         ;U
	and	ah,7Fh		; V restore eax and set 15 bit to zero
    				;
	add	eax,edi		;U {v}+{dv}|{u}+{du}
	sbb	ebp,ebp		;U FFFFFFFF if need add TextureWidth(v)
	mov	bh,[esi]        ; V
   	and	ebp,textureW		;U 0 or TextureWidth
	add	ah,80h		; V set carry if need add 1(u)
   	adc	esi,ecx         ;U
	and	ah,7Fh          ; V
   	rol	ebx,16          ;U
   	add	esi,ebp         ; V

endM

InnerTextureLoop4PPro Macro textureW,screenOff
	;4.5 ticks/pixel
	;
	;draw 4 pixels of texture
	;eax - {v}0{u}
	;ebx - store pixels
	;ecx - [du] + [dv]*TextureWidth
	;edx - TextureWidth
	;esi - Texture Pointer
	;edi - {dv}0{du}
	;ebp - carry

	mov	bl,[esi]        ;U
	add	eax,edi		; V {v}+{dv}|{u}+{du}
	sbb	ebp,ebp		;U FFFFFFFF if need add TextureWidth(v)
	rol	eax,17		; V set carry if need add 1(u)
	adc	esi,ecx         ;U
	and	ebp,textureW		; V 0 or TextureWidth
	and	eax,0FFFFFFFEh
	add	esi,ebp         ;U
	ror	eax,17
	mov	[edx+screenOff],bl
    				;
	mov	bl,[esi]        ;U
	add	eax,edi		; V {v}+{dv}|{u}+{du}
	sbb	ebp,ebp		;U FFFFFFFF if need add TextureWidth(v)
	rol	eax,17		; V set carry if need add 1(u)
	adc	esi,ecx         ;U
	and	ebp,textureW		; V 0 or TextureWidth
	and	eax,0FFFFFFFEh
	add	esi,ebp         ;U
	ror	eax,17
	mov	[edx+screenOff+1],bl
    				;
	mov	bl,[esi]        ;U
	add	eax,edi		; V {v}+{dv}|{u}+{du}
	sbb	ebp,ebp		;U FFFFFFFF if need add TextureWidth(v)
	rol	eax,17		; V set carry if need add 1(u)
	adc	esi,ecx         ;U
	and	ebp,textureW		; V 0 or TextureWidth
	and	eax,0FFFFFFFEh
	add	esi,ebp         ;U
	ror	eax,17
	mov	[edx+screenOff+2],bl
    				;
	mov	bl,[esi]        ;U
	add	eax,edi		; V {v}+{dv}|{u}+{du}
	sbb	ebp,ebp		;U FFFFFFFF if need add TextureWidth(v)
	rol	eax,17		; V set carry if need add 1(u)
	adc	esi,ecx         ;U
	and	ebp,textureW		; V 0 or TextureWidth
	and	eax,0FFFFFFFEh
	add	esi,ebp         ;U
	ror	eax,17
	mov	[edx+screenOff+3],bl
    				;
endM
;====================================================================
InnerSpriteLoop4 Macro textureW,screenOff
	local	@@NoDraw1,@@NoDraw2,@@NoDraw3,@@NoDraw4
	;5 ticks/pixel

	add	eax,edi		; V {v}+{dv}|{u}+{du}
	mov	bl,[esi]        ; V
	sbb	ebp,ebp         ;U
	add	ah,80h		; V set carry if need add 1(u)
	adc	esi,ecx         ;U
	and	ebp,textureW		; V 0 or TextureWidth
	add	esi,ebp         ;U
	and	ah,7Fh		; V restore eax and set 15 bit to zero
	and	bl,bl
	jz	@@NoDraw1
	mov	[edx+screenOff+0],bl
@@NoDraw1:
    				;
	add	eax,edi		;U {v}+{dv}|{u}+{du}
	mov	bl,[esi]        ; V
	sbb	ebp,ebp		;U FFFFFFFF if need add TextureWidth(v)
	add	ah,80h          ; V
	adc	esi,ecx         ;U
	and	ebp,textureW		;U 0 or TextureWidth
	add	esi,ebp         ;U
	and	ah,7Fh		; V restore eax and set 15 bit to zero
	and	bl,bl
	jz	@@NoDraw2
	mov	[edx+screenOff+1],bl
@@NoDraw2:
    				;
	add	eax,edi		; V {v}+{dv}|{u}+{du}
	mov	bl,[esi]        ;U
	sbb	ebp,ebp		;U FFFFFFFF if need add TextureWidth(v)
	add	ah,80h		; V set carry if need add 1(u)
	adc	esi,ecx         ;U
	and	ebp,textureW		; V 0 or TextureWidth
	add	esi,ebp         ;U
	and	ah,7Fh		; V restore eax and set 15 bit to zero
	and	bl,bl
	jz	@@NoDraw3
	mov	[edx+screenOff+2],bl
@@NoDraw3:
    				;
	add	eax,edi		;U {v}+{dv}|{u}+{du}
	mov	bl,[esi]        ; V
	sbb	ebp,ebp		;U FFFFFFFF if need add TextureWidth(v)
	add	ah,80h		; V set carry if need add 1(u)
   	adc	esi,ecx         ;U
   	and	ebp,textureW		;U 0 or TextureWidth
	and	ah,7Fh          ; V
   	add	esi,ebp         ; V
	and	bl,bl
	jz	@@NoDraw4
	mov	[edx+screenOff+3],bl
@@NoDraw4:
    				;

endM
;==============================================================
InnerAlphaTextureLoop32 Macro textureW
        ;5 ticks/pixel
	;
	;draw 4 pixels of texture
	;eax - {v}0{u}
	;ebx - store pixels
        ;edx - AlphaMixTable Pointer [du] + [dv]*TextureWidth
        ;ecx - Screen Pointer
	;esi - Texture Pointer
	;edi - {dv}0{du}
	;ebp - carry

        REPT    32

        mov     bh,[esi]        ;U  get alpha from texture
	add	eax,edi		; V {v}+{dv}|{u}+{du}
	sbb	ebp,ebp		;U FFFFFFFF if need add TextureWidth(v)
        mov     bl,[ecx]        ; V get color from screen
        inc     ecx             ;U  inc screen pointer
	add	ah,80h		; V set carry if need add 1(u)
        adc     esi,[ebp*4 + textureW] ;U add deltas
        mov     bl,[edx + ebx]  ; V get color from AlphaMixTable
        and     ah,7Fh          ;U restore eax and set 15 bit to zero
        mov     [ecx - 1],bl    ; V put color to screen

        ENDM

endM
;==============================================================
InnerDiserLoop32 Macro textureW
        ;7.5 ticks/pixel
        ;64x64
	;draw 4 pixels of texture
	;eax - {v}0{u}
        ;edi - {dv}0{du}
        ;ebx - store pixels
        ;edx - tmp
        ;ecx - Screen Pointer
	;esi - Texture Pointer
        ;ebp - carry

        ;mov     ebx,eax          ;U  vvvv|vv--|----|----|-uuu|uuu-|----|----
        ;and     ebx,0FC007E00h   ;U
        ;shr     bl,9             ;U  vvvv|vv--|----|----|----|----|--uu|uuuu
        ;rol     ebx,(6+2)        ;U
        ;mov     ebx,[_DiserT+ebx];U+1

        REPT    32

        ;mov     edx,eax          ;U  vvvv|vv--|----|----|-uuu|uuu-|----|----
        lea     edx,[eax+edi]
        mov     bh,[esi+ebx]     ; V get alpha from texture
        shr     dx,9             ;U  vvvv|vv--|----|----|----|----|--uu|uuuu
        add     eax,edi          ; V {v}+{dv}|{u}+{du}
        sbb     ebp,ebp          ;U  FFFFFFFF if need add TextureWidth(v)
        and     edx,0FC00003Fh   ; V
        rol     edx,(6+2)        ;U
        add     ah,80h           ; V set carry if need add 1(u)
        adc     esi,[ebp*4 + textureW] ;U add deltas
        mov     [ecx],bh         ; V put color to screen
        mov     ebx,[_gr_pDiserTable+edx];U
        and     ah,7Fh           ; V restore eax and set 15 bit to zero
        inc     ecx              ;U

        ENDM

endM
;==============================================================
InnerBumpLoop4 Macro screenOff,intDelta
	;8 ticks/pixel
	;
	;eax - {Vb}0{Ub}
	;edi - {dVb}0{dUb}
	;esi - Screen
	;ecx - <16Bit0>[Vb][Ub]
	;edx - {Z}[dZ][Z]
	;ebp - {dZ}<16Bit0>
	;ebx - Tmp
	;intDelta - [dUb]+[dVb]*256

	mov	bh,dl
	add	edx,ebp
	adc	dl,dh
	mov	bl,B_P [ecx + BumpPtr]
	add	eax,edi
	mov	bh,B_P [ebx + MulTable]
	adc	ch,0
	mov	bl,B_P [esi + screenOff + 0]
	add	ah,80h
	mov	bl,B_P [ebx + MixColorTable]
	adc	ecx,intDelta
	mov	B_P [esi + screenOff + 0],bl
	and	ah,7fh	;U
	;
	mov	bh,dl
	mov	bl,B_P [ecx + BumpPtr]
	add	edx,ebp
	adc	dl,dh
	add	eax,edi
	adc	ch,0
	mov	bh,B_P [ebx + MulTable]
	mov	bl,B_P [esi + screenOff + 1]
	add	ah,80h
	adc	ecx,intDelta
	mov	bl,B_P [ebx + MixColorTable]
	and	ah,7fh
	mov	B_P [esi + screenOff + 1],bl	;V
	;
	mov	bh,dl
	add	edx,ebp
	adc	dl,dh
	mov	bl,B_P [ecx + BumpPtr]
	add	eax,edi
	mov	bh,B_P [ebx + MulTable]
	adc	ch,0
	mov	bl,B_P [esi + screenOff + 2]
	add	ah,80h
	mov	bl,B_P [ebx + MixColorTable]
	adc	ecx,intDelta
	mov	B_P [esi + screenOff + 2],bl
	and	ah,7fh	;U
	;
	mov	bh,dl
	mov	bl,B_P [ecx + BumpPtr]
	add	edx,ebp
	adc	dl,dh
	add	eax,edi
	adc	ch,0
	mov	bh,B_P [ebx + MulTable]
	mov	bl,B_P [esi + screenOff + 3]
	add	ah,80h
	adc	ecx,intDelta
	mov	bl,B_P [ebx + MixColorTable]
	and	ah,7fh
	mov	B_P [esi + screenOff + 3],bl
endM

;==============================================================
InnerBumpLoop4New Macro screenOff,fracDelta,Delta
	;8 ticks/pixel
	;
	;eax - {Vb}0{Ub}
        ;fracDelta - {dVb}0{dUb}
	;esi - Screen
	;ecx - <16Bit0>[Vb][Ub]
	;edx - {Z}[dZ][Z]
	;ebp - {dZ}<16Bit0>
	;ebx - Tmp
        ;Delta - ( [dUb]+([dVb]+1)*256, [dUb]+[dVb]*256 ) <-pointer

	mov	bh,dl
	add	edx,ebp
	adc	dl,dh
        mov     bl,B_P [ecx]
        add     eax,fracDelta
        mov     bh,B_P [ebx + _gr_BumpMulTable]
        sbb     edi,edi
	mov	bl,B_P [esi + screenOff + 0]
	add	ah,80h
        mov     bl,B_P [ebx + _gr_BumpMixColorTable]
        adc     ecx,[Delta+4*edi]
	mov	B_P [esi + screenOff + 0],bl
	and	ah,7fh	;U
	;
	mov	bh,dl
        mov     bl,B_P [ecx]
	add	edx,ebp
	adc	dl,dh
        add     eax,fracDelta
        sbb     edi,edi
        mov     bh,B_P [ebx + _gr_BumpMulTable]
	mov	bl,B_P [esi + screenOff + 1]
	add	ah,80h
        adc     ecx,[Delta+4*edi]
        mov     bl,B_P [ebx + _gr_BumpMixColorTable]
	and	ah,7fh
	mov	B_P [esi + screenOff + 1],bl	;V
	;
	mov	bh,dl
	add	edx,ebp
	adc	dl,dh
        mov     bl,B_P [ecx]
        add     eax,fracDelta
        mov     bh,B_P [ebx + _gr_BumpMulTable]
        sbb     edi,edi
        mov     bl,B_P [esi + screenOff + 2]
	add	ah,80h
        mov     bl,B_P [ebx + _gr_BumpMixColorTable]
        adc     ecx,[Delta+4*edi]
	mov	B_P [esi + screenOff + 2],bl
	and	ah,7fh	;U
	;
	mov	bh,dl
        mov     bl,B_P [ecx]
	add	edx,ebp
	adc	dl,dh
        add     eax,fracDelta
        sbb     edi,edi
        mov     bh,B_P [ebx + _gr_BumpMulTable]
	mov	bl,B_P [esi + screenOff + 3]
	add	ah,80h
        adc     ecx,[Delta+4*edi]
        mov     bl,B_P [ebx + _gr_BumpMixColorTable]
	and	ah,7fh
	mov	B_P [esi + screenOff + 3],bl
endM