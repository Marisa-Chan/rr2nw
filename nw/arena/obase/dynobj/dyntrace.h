/*
 * File   : C:\NW\ARENA\OBASE\DynObj\DynPart.h
 * Author : Suavik
 * Ver   1.0 
 */
#ifndef __DYNTRACE_H__INCLUDED
#define __DYNTRACE_H__INCLUDED
#include "kernel/h/s_debug.h"

#define TEXTURE_SEGMENT_LENGTH 63
#define TEXTURE_FIRST_COORD	   1
#define TEXTURE_ACTIVE_LENGTH  252
#define TEXTURE_STEP		   21
#define TEXTURE_CHECK_LENGTH   (TEXTURE_ACTIVE_LENGTH - TEXTURE_SEGMENT_LENGTH)

#define MAX_TRACE_LENGTH		20
#define TRACE_SEGMENT_LENGTH	30.0
#define TRACE_WIDTH				1.0


class s_ViewDynamicTrace : public  CViewSphericDynamic
{
public:
	CFVector3 m_pos;
	CFVector3 m_dir;
	CFVector3 m_nextCoord;
	
	double		   m_radius;
	unsigned long  m_color;
	GR_HTEXTURE    m_cacheImage;
	GR_HTEXTURE    m_cacheImageFront;
	int			   m_texturePos;
	double		   m_width0;
	double		   m_width1;	
	double		   m_minDist;
	double		   m_flatDist;
	double		   m_frontRatio;

	int		m_frontTextureX;
	int		m_frontTextureY;

	
	s_ViewDynamicTrace( )
		: m_pos(0,0,0),
		m_dir(0,0,0),
		m_radius(0),
		m_color(0)
	{
		m_bump.vel = CFVector3(0,0,0);
	}
	
	void setTexture(GR_HTEXTURE cacheImage, GR_HTEXTURE cacheImageFront )
	{ m_cacheImage = cacheImage; m_cacheImageFront = cacheImageFront;}
	
	virtual void Draw();
	void DrawHardware();
	
    void         prepareToRender(                               
			CFVector3	& nextCoord,
			double		nextWidth,
			int			texturePos
		);
};




#endif // ifndef __DYNPART_H__INCLUDED
/* End of file C:\NW\ARENA\OBASE\DynObj\DynPart.h */
