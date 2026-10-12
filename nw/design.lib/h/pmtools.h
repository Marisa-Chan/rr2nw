#ifndef	__PMTOOLS_H__
#define __PMTOOLS_H__

// --------------------------------------------------------------------------
class	DOSMem {
private:
	unsigned short  m_numPara, m_rmSeg, m_pmSel;
public:
	DOSMem( unsigned short numPara );
	~DOSMem();
	unsigned short  Size() const { return m_numPara; }
	unsigned short  RMSeg() const { return m_rmSeg; }
	unsigned short  PMSel() const { return m_pmSel; }
	operator void*() const { return (void*)((unsigned long)m_rmSeg<<4); }
};
// --------------------------------------------------------------------------
class DPMI {
private:
	static	DOSMem	stackDOS;
public:
	struct	VERSIONINFO {
		unsigned char	majVer, minVer;
		unsigned short	flags;
		unsigned char	processor;
	};
	enum	VERSIONFLAGS {
		VF_32BIT = 0x01,
		VF_REALMODE = 0x02,
		VF_VIRTUALMEM = 0x04
	};
	
	union RMREGS {		// Real mode call registers
		struct {
			unsigned short di,hdi,si,hsi,bp,hbp;
			unsigned long reserved0;
			unsigned char bl,bh; unsigned short hbx;
			unsigned char dl,dh; unsigned short hdx;
			unsigned char cl,ch; unsigned short hcx;
			unsigned char al,ah; unsigned short hax;
			unsigned short flags,es,ds,fs,gs,ip,cs,sp,ss;
		} h;
		struct {
			unsigned short di,hdi,si,hsi,bp,hbp;
			unsigned long reserved0;
			unsigned short bx,hbx,dx,hdx,cx,hcx,ax,hax;
			unsigned short flags,es,ds,fs,gs,ip,cs,sp,ss;
		} w;
		struct {
			unsigned long edi,esi,ebp,reserved0,ebx,edx,ecx,eax;
			unsigned short flags,es,ds,fs,gs,ip,cs,sp,ss;
		} x;
	};

	// The following 'int' functions return FALSE(0) on failure
	static int  GetVersion( VERSIONINFO &info );
	static int	AllocDOS( unsigned short numPara,
						unsigned short &rmSeg, unsigned short &pmSel );
	static int  FreeDOS( unsigned short pmSel );
	static int	RMIntr( unsigned char intNo, RMREGS &regs );
	static unsigned long MemMax();
};
// --------------------------------------------------------------------------
#endif
