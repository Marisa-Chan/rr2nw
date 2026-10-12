#ifndef	__GL3DE_H__
#define	__GL3DE_H__

#if _USE_OPENGL
	inline void glLoadMatrix(TCCFMatrix3x4 &m) {
		double	mx[16];
		mx[0+4*0] = m.m[0][0];
		mx[0+4*1] = m.m[0][1];
		mx[0+4*2] = m.m[0][2];
		mx[0+4*3] = m.m[0][3];
		mx[1+4*0] = m.m[1][0];
		mx[1+4*1] = m.m[1][1];
		mx[1+4*2] = m.m[1][2];
		mx[1+4*3] = m.m[1][3];
		mx[2+4*0] = m.m[2][0];
		mx[2+4*1] = m.m[2][1];
		mx[2+4*2] = m.m[2][2];
		mx[2+4*3] = m.m[2][3];
		mx[3+4*0] = 0;
		mx[3+4*1] = 0;
		mx[3+4*2] = 0;
		mx[3+4*3] = 1;
		glLoadMatrixd(mx);
	}
	inline void glMultMatrix(TCCFMatrix3x4 &m) {
		double	mx[16];
		mx[0+4*0] = m.m[0][0];
		mx[0+4*1] = m.m[0][1];
		mx[0+4*2] = m.m[0][2];
		mx[0+4*3] = m.m[0][3];
		mx[1+4*0] = m.m[1][0];
		mx[1+4*1] = m.m[1][1];
		mx[1+4*2] = m.m[1][2];
		mx[1+4*3] = m.m[1][3];
		mx[2+4*0] = m.m[2][0];
		mx[2+4*1] = m.m[2][1];
		mx[2+4*2] = m.m[2][2];
		mx[2+4*3] = m.m[2][3];
		mx[3+4*0] = 0;
		mx[3+4*1] = 0;
		mx[3+4*2] = 0;
		mx[3+4*3] = 1;
		glMultMatrixd(mx);
	}
#endif

#endif	// __GL3DE_H__

