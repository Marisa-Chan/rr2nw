#if !defined(_DRAWD3D_H_)
#define _DRAWD3D_H_


//#include <ddraw.h>
//#include <d3d.h>


//---Defines for Hardware
//Maximal of Texture Quantity on Texture DataBase
#define MAX_TEXTURE_COUNT 1024




#define MY_ARGB(a, r, g, b)  (((a)<<24) | ((r)<<16) | ((g)<<8) | (b))
#define MY_RGB(r, g, b)  (0x000000 | ((r)<<16) | ((g)<<8) | (b))


#define SET_RENDER_STATE(type, par) if (_d3dDevice->SetRenderState(type, par) != DD_OK) return FALSE
#define SET_LIGHT_STATE(type, par) if (_d3dDevice->SetLightState(type, par) != DD_OK) return FALSE


//Functions
char * GRGetLastErrorStr();
void D3DSetError(char * title, int error);


typedef struct D3D_ZList_{
    SGRPolygon polygon;
    int       vertC;
    UGRVertex v[4];
    int       type;
    void      *texture;
    float     z;
    struct D3D_ZList_ *link;
}D3D_ZList;

D3D_ZList * D3D_InsertToZList(float z);
void D3D_ClearZList();
void D3D_DrawZList();

//Functions

extern "C" void D3D_GRSetZPrecision(int nShiftL65536);

extern "C" int D3D_GRDrawPolygonPCCW();
/* Function draw a polygon on BackBuffer.
 * Return TRUE on success.
 * Notes:
 * 1) Before using this function you must call next function: GRAppInit,
 *    GRD3DInit, GRInitTables, _GR_SetClipRect, GRSetHaze, GRInitTextureDB,
 *    GRSetPalette.
 * 2) You must call GRClearScreen before call a first GRDrawPolygonPCCW
 *    and call GRDumpScreen after last GRDrawPolygonPCCW.
 */


//int GRAppInit(HINSTANCE hInstance);

//void GRD3DTerminate();
/* Procedure for release DD, D3D and etc.
 */

extern "C" int D3D_GRInitTables(unsigned long mmx);
/* Procedure set internal tables for Renderer.
 * Prameter mmx currently not use.
 * Note: This procedure must call before using GRDrawPolygonPCCW
 */

extern "C" void D3D_GR_SetClipRect();
/*
 */
//extern "C" int D3D_GRSetPalette(unsigned char * pal6);
//unsigned long GRFillColor(int r, int g, int b);
//extern "C" int D3D_GRSetHaze(int start, int len, );

//int GRClearScreen(BOOL fClr = FALSE, long fColor = 0);
//int GRDumpScreen(int nWidth, int nHeight);
extern "C" void GRInitTextureDB();
/* Procedure initialized Texture DataBase.
 * Note: This procedure must call before using GRDrawPolygonPCCW
 */

extern "C" void * D3D_GRLoadTextureToDB(void * handle, unsigned char *pal, int palCnt, unsigned char *text);
/* Function Load Texture to TextureDB (TDB).
 * Parameters:
 *   handle - If this parameter NULL then new texture will created in TDB,
 *            else texture what have this handle will be overwriten.
 *   palCnt - Count of palette entries on pal argument.
 *   pal    - Palette.
 *   text   - Pointer to texture. Texture must have a following format:
 *            [Texture Flags DWORD][Texture Height DWORD][Texture Width DWORD]
 *            <text must be set on this place>[Texture Data...].
 * Return non NULL handle for texture on success.
 * Notes:
 * 1) Returned handle must be use for draw texture!!!!!!!!!!!!.
 * 2) Before use this function, GRInitTextureDB must called.
 *
 */


#endif