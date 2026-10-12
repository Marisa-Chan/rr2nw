
    Clip	struc
	left	dd ?
	top	dd ?
	right	dd ?
	bottom	dd ?
    Clip	ends

    Screen	struc
    	xl	dd ?
	xr	dd ?
	izl	dd ?
	izr	dd ?
	uzl	dd ?
	uzr	dd ?
	vzl	dd ?
	vzr	dd ?
	ubzl	dd ?
	ubzr	dd ?
	vbzl	dd ?
	vbzr	dd ?
    Screen	ends

    Vertex	struc
	x	dd ?
	y	dd ?
        ub      dd ?
	iz	dd ?
        col     dd ?
        vb      dd ?
        u       dd ?
        v       dd ?
    Vertex	ends

    Polygon	struc
	typall	dd ?
	addtyp	dd ?
	vertc	dd ?
        color   dd ?
        opacity dd ?
	textp	dd ?	;or color
        bumpp   dd ?
	a_f	dd ?
	b_f	dd ?
	c_f	dd ?
	d_f	dd ?
        lights  dd ?
    Polygon	ends

    AlphaSprite struc
        xb      dd ?
        yb      dd ?
        xe      dd ?
        ye      dd ?
        us      dd ?
        vs      dd ?
        ue      dd ?
        ve      dd ?
        colorS  dd ?
        alpha   dd ?
        invz    dd ?
        hText   dd ?
    AlphaSprite ends

    PCCW	equ 1
    PCW		equ 0

    TRUE	equ 1
    FALSE	equ 0

    LEFT	equ 1
    RIGHT	equ 0

    D_P		equ dword ptr
    B_P		equ byte ptr
    W_P		equ word ptr

    _gr_pDivCache equ ((offset pYDivCache) + 2047*4)
    _gr_p1DivCache equ ((offset p1DivCache))


    @MAX_UNROLLPOLY     equ (2048/4)

    Y_DIV_CACHE_PRES    equ 20
    Y_DIV_CACHE		equ (1 SHL Y_DIV_CACHE_PRES)
    ;-------------------------------------------------------------

    TYPE_COUNT          equ 8
    ADD_TYPE_COUNT	equ 3

    ADD_TYPE_SIZE	equ ((1 SHL ADD_TYPE_COUNT)*4)
    TABLE_SIZE		equ (TYPE_COUNT * ADD_TYPE_SIZE)

    FLAT_TYPE           equ 0*ADD_TYPE_SIZE
    TRANSPARENT_TYPE    equ 1*ADD_TYPE_SIZE
    GOURAUD_TYPE	equ 2*ADD_TYPE_SIZE	;iz,u
    TEXTURE_P_TYPE	equ 3*ADD_TYPE_SIZE     ;iz,u,v
    TEXTURE_L_TYPE	equ 4*ADD_TYPE_SIZE     ;u,v
    TEXTURE_P_TRSP_TYPE equ 5*ADD_TYPE_SIZE     ;iz,u,v
    TEXTURE_ALPHA_TYPE  equ 6*ADD_TYPE_SIZE
    TEXTURE_L_TRSP_TYPE equ 7*ADD_TYPE_SIZE

    NO_ADDTYPE		equ 0*4
    HAZE_ADDTYPE	equ 1*4
    BUMP_ADDTYPE	equ 2*4
    LIGHT_ADDTYPE	equ 4*4
