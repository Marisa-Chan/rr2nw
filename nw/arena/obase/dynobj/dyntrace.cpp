#define LAST_H__VIEW
#include "game.h"
#include "dyntrace.h"
#include "..\..\..\graph\drawd3d.h"



extern unsigned short psi_sqrt(unsigned long v);
//extern D3D_ZList * D3D_InsertToZList(float z);
extern SDeviceList _dL;
extern int __HazeStartInt;

void s_ViewDynamicTrace::Draw()
{
    //double		width = m_radius, height  = m_radius;


    if (_dL.currDevice->swHw != GR_SOFTWARE)
	DrawHardware();


    CFVector3	v0 = CViewObject::m_viewPointDirSMx*m_pos;
	CFVector3	v1 = CViewObject::m_viewPointDirSMx*m_nextCoord;
    
	
	//if( v0.z < CViewObject::m_fFrontClip ) return;
	//if( v1.z < CViewObject::m_fFrontClip ) return;

	if( v0.z < m_minDist ) return;
	if( v1.z < m_minDist ) return;
	if( v0.z > CViewObject::m_fBackClip  ) return;
	if( v1.z > CViewObject::m_fBackClip  ) return;
	


    double d_v0 = 1./v0.z;
	double d_v1 = 1./v1.z;
    	    
	int		screen_x0 = Round(v0.x*d_v0),
		    screen_y0 = Round(v0.y*d_v0);

	int		screen_x1 = Round(v1.x*d_v1),
		    screen_y1 = Round(v1.y*d_v1);

	int dx = screen_x1 - screen_x0;
	int dy = screen_y1 - screen_y0;

    if ( !dx && !dy)
    {
        return;
    }

	/*if (dx == 0)
		dx = 1;
	if (dy == 0)
		dy = 1;*/

	int		screen_z0  = d_v0 * 65536;
	int		screen_z1  = d_v1 * 65536;

	double  screen_w0  = m_width0 * d_v0 * CViewObject::m_viewPointScale.x;
	double  screen_w1  = m_width1 * d_v1 * CViewObject::m_viewPointScale.x;

	ASSERT(screen_w0 > 0);
	ASSERT(screen_w1 > 0);
	

	int    length = psi_sqrt(dx * dx + dy * dy);

	if( v0.z < m_flatDist && v1.z < m_flatDist ) 
	if (length < screen_w1 || length < screen_w0 )
	// Front Image
	{
	  double frontW = screen_w0 * m_frontRatio;

	  int frontX = screen_x0;
          int frontY = screen_y0;

	    _gr_polygon.dwFullType = GR_POLY_TEXTURE_ALPHA;
	    _gr_polygon.dwAddType  = 0;
	    _gr_polygon.nVertices  = 4;
	   _gr_polygon.nLights    = 0;
	   _gr_polygon.hTexture   = m_cacheImageFront;
	_gr_polygon.dwColor.color = GRTransparentColor(0,0,0);
	_gr_polygon.dwOpacity  = 255;



	_gr_vertices[0].texture.x  = frontX - frontW;
	_gr_vertices[0].texture.y  = frontY - frontW;
	_gr_vertices[0].texture.iz = screen_z0;
        _gr_vertices[0].texture.u  = m_frontTextureX << 16; 
	_gr_vertices[0].texture.v  = m_frontTextureY << 16;

	
	_gr_vertices[1].texture.x  = frontX + frontW;
	_gr_vertices[1].texture.y  = frontY - frontW;
	_gr_vertices[1].texture.iz = screen_z0;
        _gr_vertices[1].texture.u  = (m_frontTextureX + 127) << 16; 	
	_gr_vertices[1].texture.v  = m_frontTextureY << 16;

	_gr_vertices[2].texture.x  = frontX + frontW;
	_gr_vertices[2].texture.y  = frontY + frontW;
	_gr_vertices[2].texture.iz = screen_z0;
	_gr_vertices[2].texture.u  = (m_frontTextureX + 127) << 16; 	
	_gr_vertices[2].texture.v  = (m_frontTextureY + 127) << 16;

	
	_gr_vertices[3].texture.x  = frontX - frontW;
	_gr_vertices[3].texture.y  = frontY + frontW;
	_gr_vertices[3].texture.iz = screen_z0;
    _gr_vertices[3].texture.u      = m_frontTextureX << 16;  	
	_gr_vertices[3].texture.v  = (m_frontTextureX + 127) << 16;


        GRDrawPolygonPCCW();

  	frontW = screen_w1  * m_frontRatio;

	  frontX = screen_x1;
          frontY = screen_y1;

	 _gr_polygon.dwFullType = GR_POLY_TEXTURE_ALPHA;
//		_gr_polygon.dwFullType = GR_POLY_TEXTURE_PERSP;
	//	_gr_polygon.dwFullType = GR_POLY_FLAT;
	    _gr_polygon.dwAddType  = 0;
	    _gr_polygon.nVertices  = 4;
	   _gr_polygon.nLights    = 0;
	   _gr_polygon.hTexture   = m_cacheImageFront;
	_gr_polygon.dwColor.color = GRTransparentColor(0,0,0);
	_gr_polygon.dwOpacity  = 255;



	_gr_vertices[0].texture.x  = frontX - frontW;
	_gr_vertices[0].texture.y  = frontY - frontW;
	_gr_vertices[0].texture.iz = screen_z1;
        _gr_vertices[0].texture.u  = (m_frontTextureX) << 16; 
	_gr_vertices[0].texture.v  = m_frontTextureY << 16;

	
	_gr_vertices[1].texture.x  = frontX + frontW;
	_gr_vertices[1].texture.y  = frontY - frontW;
	_gr_vertices[1].texture.iz = screen_z1;
        _gr_vertices[1].texture.u  = (m_frontTextureX + 127) << 16; 	
	_gr_vertices[1].texture.v  = m_frontTextureY << 16;

	_gr_vertices[2].texture.x  = frontX + frontW;
	_gr_vertices[2].texture.y  = frontY + frontW;
	_gr_vertices[2].texture.iz = screen_z1;
	_gr_vertices[2].texture.u  = 127 << 16; 	
	_gr_vertices[2].texture.v  = 127 << 16;

	
	_gr_vertices[3].texture.x  = frontX - frontW;
	_gr_vertices[3].texture.y  = frontY + frontW;
	_gr_vertices[3].texture.iz = screen_z1;
        _gr_vertices[3].texture.u      = m_frontTextureX << 16;  	
	_gr_vertices[3].texture.v  = (m_frontTextureX + 127) << 16;


        GRDrawPolygonPCCW();


	return;
	}

	CFVector2 m_traceCacheCoord0, m_traceCacheCoord1;

	double ratio = (double) screen_w0 / length;
	double dx01 = - dy * ratio;
	double dy01 = dx    * ratio;

	m_traceCacheCoord0.x = screen_x0 - dx01;
	m_traceCacheCoord0.y = screen_y0 - dy01;

	m_traceCacheCoord1.x = screen_x0 + dx01;
	m_traceCacheCoord1.y = screen_y0 + dy01;
	
	ASSERT(m_texturePos <= TEXTURE_CHECK_LENGTH);
	ASSERT(m_texturePos >= TEXTURE_FIRST_COORD);
    
	GRSetZPrecision(0);

	
       _gr_polygon.dwFullType = GR_POLY_TEXTURE_ALPHA;
//	_gr_polygon.dwFullType = GR_POLY_TEXTURE_PERSP;
//	_gr_polygon.dwFullType = GR_POLY_FLAT;
    _gr_polygon.dwAddType  = 0;
    _gr_polygon.nVertices  = 4;
	_gr_polygon.nLights    = 0;
	_gr_polygon.hTexture   = m_cacheImage;
	_gr_polygon.dwColor.color = GRTransparentColor(0,0,0);
	_gr_polygon.dwOpacity  = 255;



	_gr_vertices[0].texture.x  = m_traceCacheCoord0.x;
	_gr_vertices[0].texture.y  = m_traceCacheCoord0.y;
	_gr_vertices[0].texture.iz = screen_z0;
    _gr_vertices[0].texture.u  = 1 << 16; 
	//_gr_vertices[0].texture.v  = (m_texturePos ) << 16;
	_gr_vertices[0].texture.v  = 1 << 16;

	
	_gr_vertices[1].texture.x  = m_traceCacheCoord1.x;
	_gr_vertices[1].texture.y  = m_traceCacheCoord1.y;
	_gr_vertices[1].texture.iz = screen_z0;
    _gr_vertices[1].texture.u  = TEXTURE_SEGMENT_LENGTH << 16; 	
	_gr_vertices[1].texture.v  = 1 << 16;



	ratio = (double) screen_w1 / length;
	dx01 = - dy * ratio;
	dy01 = dx    * ratio;

	m_traceCacheCoord0.x = screen_x1 - dx01;
	m_traceCacheCoord0.y = screen_y1 - dy01;

	m_traceCacheCoord1.x = screen_x1 + dx01;
	m_traceCacheCoord1.y = screen_y1 + dy01;

	
	_gr_vertices[2].texture.x  = m_traceCacheCoord1.x;
	_gr_vertices[2].texture.y  = m_traceCacheCoord1.y;
	_gr_vertices[2].texture.iz = screen_z1;
	_gr_vertices[2].texture.u  = TEXTURE_SEGMENT_LENGTH << 16; 	
	_gr_vertices[2].texture.v  = 254 << 16;

	
	_gr_vertices[3].texture.x  = m_traceCacheCoord0.x;
	_gr_vertices[3].texture.y  = m_traceCacheCoord0.y;
	_gr_vertices[3].texture.iz = screen_z1;
    _gr_vertices[3].texture.u  = 1 << 16;  	
	_gr_vertices[3].texture.v  = 254 << 16;


    GRDrawPolygonPCCW();
}


void s_ViewDynamicTrace::DrawHardware()
{


    CFVector3	v0 = CViewObject::m_viewPointDirSMx*m_pos;
	CFVector3	v1 = CViewObject::m_viewPointDirSMx*m_nextCoord;
    
	if( v0.z < m_minDist ) return;
	if( v1.z < m_minDist ) return;
	if( v0.z > CViewObject::m_fBackClip  ) return;
	if( v1.z > CViewObject::m_fBackClip  ) return;
	


    double d_v0 = 1./v0.z;
	double d_v1 = 1./v1.z;



    	    
	int		screen_x0 = Round(v0.x*d_v0),
		    screen_y0 = Round(v0.y*d_v0);

	int		screen_x1 = Round(v1.x*d_v1),
		    screen_y1 = Round(v1.y*d_v1);

	int dx = screen_x1 - screen_x0;
	int dy = screen_y1 - screen_y0;

    if ( !dx && !dy)
    {
        return;
    }

	/*if (dx == 0)
		dx = 1;
	if (dy == 0)
		dy = 1;*/

	int screen_z0  = d_v0 * 65536;
	int screen_z1  = d_v1 * 65536;

	int addType = screen_z1 < __HazeStartInt?GR_POLY_ADD_HAZE:0;

	double  screen_w0  = m_width0 * d_v0 * CViewObject::m_viewPointScale.x;
	double  screen_w1  = m_width1 * d_v1 * CViewObject::m_viewPointScale.x;

	ASSERT(screen_w0 > 0);
	ASSERT(screen_w1 > 0);


     UGRVertex *v;
     D3D_ZList *zl;
	

	int    length = psi_sqrt(dx * dx + dy * dy);

	if( v0.z < m_flatDist && v1.z < m_flatDist ) 
	if (length < screen_w1 || length < screen_w0 )
	// Front Image
	{
	  double frontW = screen_w0 * m_frontRatio;

	  int frontX = screen_x0;
          int frontY = screen_y0;

     	  float z = 1. - (float) screen_z0;

     	  zl = D3D_InsertToZList(z);
     	  v = zl->v;
     	  zl->type = 1;
     	  zl->vertC = 4;


	  zl->polygon.dwFullType = GR_POLY_TEXTURE_ALPHA;
	  zl->polygon.dwAddType  = 0;
	  zl->polygon.nVertices  = 4;
	  zl->polygon.hTexture   = m_cacheImageFront;
	  zl->polygon.dwColor.color = GRTransparentColor(0,0,0);
	  zl->polygon.dwOpacity  = 255;
     	  zl->polygon.dwAddType = addType;



	v[0].any.x  = frontX - frontW;
	v[0].any.y  = frontY - frontW;
	v[0].any.iz = screen_z0;
        v[0].texture.u  = m_frontTextureX << 16; 
        v[0].texture.v  = m_frontTextureY << 16;

	
	v[1].any.x  = frontX + frontW;
	v[1].any.y  = frontY - frontW;
	v[1].any.iz = screen_z0;
        v[1].texture.u  = (m_frontTextureX + 127) << 16; 	
	v[1].texture.v  = m_frontTextureY << 16;

	v[2].any.x  = frontX + frontW;
	v[2].any.y  = frontY + frontW;
	v[2].any.iz = screen_z0;
	v[2].texture.u  = (m_frontTextureX + 127) << 16; 	
	v[2].texture.v  = (m_frontTextureY + 127) << 16;

	
	v[3].any.x  = frontX - frontW;
	v[3].any.y  = frontY + frontW;
	v[3].any.iz = screen_z0;
    	v[3].texture.u      = m_frontTextureX << 16;  	
	v[3].texture.v  = (m_frontTextureX + 127) << 16;


        //GRDrawPolygonPCCW();

     	  z = 1. - (float) screen_z1;

     	  zl = D3D_InsertToZList(z);
     	  v = zl->v;
     	  zl->type = 1;
     	  zl->vertC = 4;

	  zl->polygon.dwFullType = GR_POLY_TEXTURE_ALPHA;
	  zl->polygon.dwAddType  = 0;
	  zl->polygon.nVertices  = 4;
	  zl->polygon.hTexture   = m_cacheImageFront;
	  zl->polygon.dwColor.color = GRTransparentColor(0,0,0);
	  zl->polygon.dwOpacity  = 255;
     	  zl->polygon.dwAddType = addType;



  	frontW = screen_w1  * m_frontRatio;

	  frontX = screen_x1;
          frontY = screen_y1;


	v[0].any.x  = frontX - frontW;
	v[0].any.y  = frontY - frontW;
	v[0].any.iz = screen_z1;
        v[0].texture.u  = (m_frontTextureX) << 16; 
	v[0].texture.v  = m_frontTextureY << 16;

	
	v[1].any.x  = frontX + frontW;
	v[1].any.y  = frontY - frontW;
	v[1].any.iz = screen_z1;
        v[1].texture.u  = (m_frontTextureX + 127) << 16; 	
	v[1].texture.v  = m_frontTextureY << 16;

	v[2].any.x  = frontX + frontW;
	v[2].any.y  = frontY + frontW;
	v[2].any.iz = screen_z1;
	v[2].texture.u  = 127 << 16; 	
	v[2].texture.v  = 127 << 16;

	
	v[3].any.x  = frontX - frontW;
	v[3].any.y  = frontY + frontW;
	v[3].any.iz = screen_z1;
        v[3].texture.u      = m_frontTextureX << 16;  	
	v[3].texture.v  = (m_frontTextureX + 127) << 16;


        //GRDrawPolygonPCCW();


	return;
	}


     	  float z = 1. - (float) screen_z0;

     	  zl = D3D_InsertToZList(z);
     	  v = zl->v;
     	  zl->type = 1;
    	  zl->vertC = 4;

	  zl->polygon.dwFullType = GR_POLY_TEXTURE_ALPHA;
	  zl->polygon.dwAddType  = 0;
	  zl->polygon.nVertices  = 4;
	  zl->polygon.hTexture   = m_cacheImage;
	  zl->polygon.dwColor.color = GRTransparentColor(0,0,0);
	  zl->polygon.dwOpacity  = 255;
     	  zl->polygon.dwAddType = addType;




	CFVector2 m_traceCacheCoord0, m_traceCacheCoord1;

	double ratio = (double) screen_w0 / length;
	double dx01 = - dy * ratio;
	double dy01 = dx    * ratio;

	m_traceCacheCoord0.x = screen_x0 - dx01;
	m_traceCacheCoord0.y = screen_y0 - dy01;

	m_traceCacheCoord1.x = screen_x0 + dx01;
	m_traceCacheCoord1.y = screen_y0 + dy01;
	
	ASSERT(m_texturePos <= TEXTURE_CHECK_LENGTH);
	ASSERT(m_texturePos >= TEXTURE_FIRST_COORD);
    
	GRSetZPrecision(0);

	
	v[0].any.x  = m_traceCacheCoord0.x;
	v[0].any.y  = m_traceCacheCoord0.y;
	v[0].any.iz = screen_z0;
    	v[0].texture.u  = 1 << 16; 
	v[0].texture.v  = 1 << 16;

	
	v[1].any.x  = m_traceCacheCoord1.x;
	v[1].any.y  = m_traceCacheCoord1.y;
	v[1].any.iz = screen_z0;
    	v[1].texture.u  = TEXTURE_SEGMENT_LENGTH << 16; 	
	v[1].texture.v  = 1 << 16;



	ratio = (double) screen_w1 / length;
	dx01 = - dy * ratio;
	dy01 = dx    * ratio;

	m_traceCacheCoord0.x = screen_x1 - dx01;
	m_traceCacheCoord0.y = screen_y1 - dy01;

	m_traceCacheCoord1.x = screen_x1 + dx01;
	m_traceCacheCoord1.y = screen_y1 + dy01;

	
	v[2].any.x  = m_traceCacheCoord1.x;
	v[2].any.y  = m_traceCacheCoord1.y;
	v[2].any.iz = screen_z1;
	v[2].texture.u  = TEXTURE_SEGMENT_LENGTH << 16; 	
	v[2].texture.v  = 254 << 16;

	
	v[3].any.x  = m_traceCacheCoord0.x;
	v[3].any.y  = m_traceCacheCoord0.y;
	v[3].any.iz = screen_z1;
    	v[3].texture.u  = 1 << 16;  	
	v[3].texture.v  = 254 << 16;


    //GRDrawPolygonPCCW();



}


void s_ViewDynamicTrace::prepareToRender(									
									CFVector3	& nextCoord,
									double		nextWidth,
									int			texturePos
                                )
{
	m_texturePos	= texturePos;
	m_nextCoord		= nextCoord;
    m_bump.fTime	= 0;
	m_width1		= nextWidth;

//    m_dynBase = m_dynBase1 = m_bump.start = m_pos;
//    m_bump.fRadius = m_radius;
	
	m_dynBase = m_dynBase1 = m_bump.start = (m_pos + m_nextCoord)*0.5;
    m_bump.fRadius = Abs(m_pos-m_dynBase);
}

/* End of file C:\NW\ARENA\OBASE\DynObj\DynPart.cpp */
