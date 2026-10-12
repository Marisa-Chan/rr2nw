
#ifndef	__LANG_H__
#define	__LANG_H__

#ifndef	_NOAUTOINCLUDE
	#define	_NOAUTOINCLUDE
	#if !defined(__STDIO_H) && !defined(_STDIO_H_INCLUDED)
		#include<stdio.h>
	#endif
	#if !defined(__MATH_H) && !defined(_MATH_H_INCLUDED)
		#include<math.h>
	#endif
	#if !defined(__CTYPE_H) && !defined(_CTYPE_H_INCLUDED)
		#include<ctype.h>
	#endif
	#ifndef	__STDTYPES_H__
		#include"stdtypes.h"
	#endif
	#ifndef	__DEBUGEXT_H__
		#include"debugext.h"
	#endif
	#ifndef	__MATHLIB_H__
		#include"mathlib.h"
	#endif
	#undef	_NOAUTOINCLUDE
#endif

// ------------------------------------------------------------------------
struct	STextPos {
protected:
	int  m_row, m_col;
};

class	CTextPos : public STextPos {
public:
	CTextPos() { m_row = 1; m_col = 1; }
	CTextPos(int r,int c) { m_row = r; m_col = c; }
	void operator+=(int c);
	int Row() const { return m_row; }
	int Col() const { return m_col; }
};

class	CCharStream	{
protected:
	int		m_c, m_cmark;
	bool	m_bUnget;
	bool	m_bErr;
	CTextPos m_pos;
	int		(*m_PrintF)(TCchar *format,...);
	virtual	int GetNext() = 0;
public:
	CCharStream(int (*PrintF)(TCchar *format,...));
	virtual ~CCharStream();
	void  SetPrint(int (*PrintF)(TCchar *format,...)) { m_PrintF = PrintF; }
	bool  Open(const CTextPos &pos0);
	bool  Close() { return TRUE; }
	int Get();
	int This() const { return m_c; }
	bool Unget();
	void Error(TCchar *msg, const CTextPos * m_pos = NULL );
	virtual operator int() const = 0;
	const CTextPos &Pos() const { return m_pos; }
};

class	CMemoryCharStream : public CCharStream	{
private:
	TCchar	*m_pPtr;
	virtual int GetNext() { return (int)(unsigned)(byte)*m_pPtr++; }
public:
	CMemoryCharStream(int (*PrintF)(TCchar *format,...))
						 : CCharStream(PrintF) { m_pPtr = NULL; }
	virtual ~CMemoryCharStream() { Close(); }
	bool  Open(TCchar *lpText,const CTextPos &pos0) { ASSERT(!m_pPtr);
						m_pPtr = lpText; return CCharStream::Open(pos0); }
	bool  Close() { m_pPtr = NULL; return CCharStream::Close(); }
	virtual operator int() const { return m_pPtr!=NULL; }
};

class	CFileCharStream : public CCharStream	{
private:
	FILE	*m_f;
	virtual int GetNext() { return fgetc(m_f); }
public:
	CFileCharStream(int (*PrintF)(TCchar *format,...))
								: CCharStream(PrintF) { m_f = NULL; }
	virtual ~CFileCharStream() { Close(); }
	bool  Open(FILE *f,const CTextPos &pos0);
	bool  Close();
	virtual operator int() const { return m_f!=NULL; }
};

class	CLangLex {
public:
	enum TYPE {
		T_NONE,
		T_SEMICOLON, T_COMMA, T_COLON,
		T_LBRACE, T_RBRACE, T_LBRAK, T_RBRAK, T_LPAR, T_RPAR,
		T_EQ, T_NEQ, T_GT, T_GE, T_LT, T_LE,
		T_PLUS, T_MINUS, T_STAR, T_SLASH, T_PERC, T_PERIOD, T_ASSIGN,
		T_QST, T_DBLSTAR, T_TILDE,
		T_MOD, T_NOT, T_AND, T_OR,
		T_ID, T_LABEL, T_STRINGV, T_FLOATV, T_INTV,
		T_VAR, T_FLOAT, T_INT, T_VECTOR, T_VOID, T_FUNC, T_RETURN,
		T_IF, T_THEN, T_ELSE, T_LOOP, T_BREAK, T_GOTO, T_PRINT, T_RUN,
		T_FOR, T_TO, T_DOWNTO, T_PI, T_ABS, T_SQRT,
		T_SIN, T_COS, T_TAN, T_ASIN, T_ACOS, T_ATAN, T_ATAN2, T_LOG, T_EXP,
		T_EOF, T_BAD,
		T_SUP
	};
private:
	TYPE m_type;
	union {
		long  i;
		double f;
		char  s[256];
	} m_v;
	CLangLex() {}
	CLangLex &operator=(const CLangLex &lx);
	CLangLex(const CLangLex &lx) { *this=lx; }
public:
	bool operator==(TYPE t) const { return m_type==t; }
	bool operator!=(TYPE t) const { return m_type!=t; }
	bool operator==(TCchar *lpsz) const;
	bool operator!=(TCchar *lpsz) const { return !(*this==lpsz); }
	int  IsListedID(TCchar **lpList,int n) const;
	TYPE Type() const { return m_type; }
	TCchar *Buffer() const;
	operator long() const { ASSERT(m_type==T_INTV); return m_v.i; }
	operator double() const { ASSERT(m_type==T_FLOATV); return m_v.f; }
	friend class CLangLexStream;
};

class	CLangLexStream  {
private:
	CCharStream	*m_pcs;
	CLangLex	m_lx;
	bool	m_bUnget;
	CTextPos  m_pos;

	static	bool IsAlpha(int c) {
					return isalpha(c) || c == '_'; }
	static	bool IsAlNum(int c) {
					return isalnum(c) || c == '_'; }

public:
	CLangLexStream();
	~CLangLexStream();
	bool  Open(CCharStream *pcs);
	bool  Close();
	CCharStream *Suspend();
	const CLangLex &Get();
	const CLangLex &This() { return m_lx; }
	bool  Unget();
	void  Error(TCchar *msg) { m_pcs->Error(msg,&m_pos); }
	const CTextPos &Pos() const { return m_pos; }
	operator int() const { return (int)m_pcs; }
};
// -----------------------------------------------------------------------
class	CLangMemory {
private:
	byte *m_pStart, *m_pEnd, *m_pTop, *m_pBottom;
public:
	CLangMemory() { m_pStart = m_pEnd = m_pTop = m_pBottom = NULL; }
	CLangMemory(unsigned nSize) { ASSERT(nSize<(unsigned)-1);
		VERIFY( (m_pBottom = m_pStart = new byte[nSize])!=NULL );
		m_pTop = m_pEnd = m_pStart + nSize; }
	~CLangMemory() { delete [] m_pStart; }
	void *AllocH(unsigned nSize);
	void *AllocS(unsigned nSize);
	void *AllocSI(int nSize);
	byte *HP() const { return m_pBottom; }
	byte *SP() const { return m_pTop; }
	unsigned HeapUsed() const { return (unsigned)(m_pBottom-m_pStart); }
	unsigned StackUsed() const { return (unsigned)(m_pEnd-m_pTop); }
	unsigned HN() const { return (unsigned)(m_pBottom-m_pStart); }
	unsigned SN() const { return (unsigned)(m_pTop-m_pStart); }
	byte *HeapBase() const { return m_pStart; }
	byte *StackBase() const { return m_pEnd; }
    byte *H2P(unsigned h) const { return m_pStart+h; }
	unsigned P2H(TCvoid *p) const { return (unsigned)((TCbyte*)p-m_pStart); }
	void ReleaseH(TCvoid *pMark) {
		ASSERT((unsigned long)pMark>=(unsigned long)m_pStart&&
				(unsigned long)pMark<=(unsigned long)m_pBottom);
			m_pBottom = (byte*)pMark; }
	void ReleaseS(TCvoid *pMark) {
		ASSERT((unsigned long)pMark>=(unsigned long)m_pTop&&
				(unsigned long)pMark<=(unsigned long)m_pEnd);
			 m_pTop = (byte*)pMark; }
	void ReleaseS(unsigned n) {
			ASSERT((unsigned long)m_pTop+n<=(unsigned long)m_pEnd);
				m_pTop += n; }
	void Clear() { m_pBottom = m_pStart; m_pTop = m_pEnd; }
	unsigned Remaining() const { return (unsigned)(m_pTop-m_pBottom); }
	bool IsValid(TCvoid *p,unsigned size) const {
		return	(unsigned long)p>=(unsigned long)m_pStart &&
				(unsigned long)p+size<=(unsigned long)m_pBottom ||
				(unsigned long)p>=(unsigned long)m_pTop &&
				(unsigned long)p+size<=(unsigned long)m_pEnd; }
};

class	CLangProcessor	{
protected:
	typedef bool (*TExtFuncPtr)(CLangProcessor&);
	enum PCOMMAND {
		P_HALT,
		P_INT, P_FLOAT, P_VECTOR, P_PTR, P_EPTR_H, P_EPTR_S,
		P_ASSIGN_HI, P_ASSIGN_SI, P_ASSIGN_EI,
		P_ASSIGN_HF, P_ASSIGN_SF, P_ASSIGN_EF,
		P_ASSIGN_HV, P_ASSIGN_SV, P_ASSIGN_EV,
		P_DEREF_HI, P_DEREF_SI, P_DEREF_EI,
		P_DEREF_HF, P_DEREF_SF, P_DEREF_EF,
		P_DEREF_HV, P_DEREF_SV, P_DEREF_EV,
		P_JMP, P_JZ_I, P_JZ_F, P_JNZ_I, P_JNZ_F,
		P_CALL, P_CALLEXT,
		P_ENTER, P_LEAVE, P_ALLOC_S,
		P_CAST_IF, P_CAST_FI, P_CAST_HPE, P_CAST_SPE,
		P_CAST_DEGRAD, P_CAST_RADDEG,
		P_DROP_P, P_DROP_EP, P_DROP_I, P_DROP_F, P_DROP_V,
		P_DUP_P, P_DUP_EP,
		P_ADD_PI, P_ADD_EPI,
		P_ADD_II, P_ADD_IF, P_ADD_FI, P_ADD_FF, P_ADD_VV,
		P_SUB_II, P_SUB_IF, P_SUB_FI, P_SUB_FF, P_SUB_VV,
		P_EQ_II, P_EQ_IF, P_EQ_FI, P_EQ_FF, P_EQ_VV,
		P_NE_II, P_NE_IF, P_NE_FI, P_NE_FF, P_NE_VV,
		P_GT_II, P_GT_IF, P_GT_FI, P_GT_FF, P_GT_VV,
		P_LT_II, P_LT_IF, P_LT_FI, P_LT_FF, P_LT_VV,
		P_GE_II, P_GE_IF, P_GE_FI, P_GE_FF, P_GE_VV,
		P_LE_II, P_LE_IF, P_LE_FI, P_LE_FF, P_LE_VV,
		P_NOT_I, P_NOT_F,
		P_NEG_I, P_NEG_F, P_NEG_V,
		P_DOT_X, P_DOT_Y, P_DOT_Z,
		P_MUL_II, P_MUL_IF, P_MUL_FI, P_MUL_FF, P_MUL_VV,
			P_MUL_VI, P_MUL_VF, P_MUL_IV, P_MUL_FV, P_MUL_VVC,
		P_DIV_II, P_DIV_IF, P_DIV_FI, P_DIV_FF, P_DIV_VI, P_DIV_VF,
		P_MOD_II,
		P_POW_II, P_POW_IF, P_POW_FI, P_POW_FF,
		P_PRINT_SC, P_PRINT_NL, P_PRINT_I, P_PRINT_F, P_PRINT_V,
		P_ATAN2, P_SIN, P_COS, P_TAN, P_ASIN, P_ACOS, P_ATAN,
		P_LOG, P_EXP, P_SQRT, P_ABS_I, P_ABS_F, P_ABS_V,
		P_NORET,
		P_SUP	// Max value, never used
	};
	// Order of II,IF,FI,FF in above is essential!

	int (*m_PrintF)(TCchar *format,...);
	CLangMemory	m_mem;
	byte	*m_pHeapBase, *m_pStackFrame;
	union {
		byte *b;
		char *c;
		long *l;
		double *f;
		word *w;
		iword *iw;
		dword *dw;
		CFVector3	*v;
	} m_ip;
	void Error(TCchar *msg);
	static double Pow(double b,long e);
	bool Go(unsigned nAddr);
public:
	CLangProcessor(int (*PrintF)(TCchar *format,...),unsigned nMem)
			: m_PrintF(PrintF), m_mem(nMem) {
				m_pHeapBase = m_mem.HeapBase(); m_pStackFrame = m_mem.SP(); }
	bool Push(TCvoid *pBuf,unsigned nSize);
	bool Pop(void *pBuf,unsigned nSize);
	bool Call(unsigned offset);
	void *Var(unsigned offset);
	void ClearStack() { m_mem.ReleaseS(m_mem.StackUsed());
											m_pStackFrame = m_mem.SP(); }
	unsigned GetPMemoryFree() const { return m_mem.Remaining(); }
};

class	CLangValue	{
public:
	enum TYPE { T_ERROR, T_VOID, T_INT, T_FLOAT, T_VECTOR };
private:
	TYPE	m_type;
	union { long l; double f; SFVector3 v; } m_v;

	CLangValue() { m_type = T_ERROR; }
	CLangValue(TYPE t) { m_type = t; }
	CLangValue(long l) { m_type = T_INT; m_v.l = l; }
	CLangValue(double f) { m_type = T_FLOAT; m_v.f = f; }
	CLangValue(const SFVector3 &v) { m_type = T_VECTOR; m_v.v = v; }
	friend class CLangExprCompiler;
public:
	TYPE Type() const { return m_type; }
	operator long() const {
		ASSERT(m_type == T_INT || m_type == T_FLOAT);
		return m_type == T_INT ? m_v.l : m_v.f; }
	operator double() const {
		ASSERT(m_type == T_INT || m_type == T_FLOAT);
		return m_type == T_FLOAT ? m_v.f : m_v.l; }
	operator CFVector3() const {
							ASSERT(m_type == T_VECTOR); return m_v.v; }
};

class	CLangContext {
public:
	struct  SArgType {
		int  type;
		word hDesc;
		word hNext;
	};
	class	CEntry	{
	protected:
		word m_hName;
		int	m_type;
		struct { word a,d; } m_p;
		word	m_hNext;
		friend class CLangContext;
	public:
		enum { T_FUNC = 0x300, T_LANGFUNC = 0x100, T_FORWARD = 0x200,
										T_EXTFUNC = 0x200, T_REF = 0x400 };
		int	Type() const { return m_type; }
		word Offset() const { return m_p.a; }
		word DescOffset() const;
		word ArgTypes() const;
		void Refer() { m_type |= T_REF; }
		bool IsReferred() const { return m_type & T_REF; }
	};
protected:
	class	CFuncEntry : public CEntry {
	public:
		word	m_hArgTypes;
	};
	CLangMemory	*m_pHeap;
	word m_hLastEntry;
	CEntry *AddEntry(TCchar *name,int type);
public:
	CLangContext(CLangMemory *pHeap) { m_pHeap = pHeap; m_hLastEntry = -1; }
	CEntry *Find(TCchar *name) const;
	bool Add(TCchar *name,int type,word descr,word offset);
	bool AddArg(TCchar *name,int type,word descr,word offset);
	void SetLastArgTypes(word h);
	void DeleteLast();
	void Clear() { m_hLastEntry = -1; }
	bool IsEmpty() const { return m_hLastEntry == (word)-1; }
};
inline word CLangContext::CEntry::ArgTypes() const { ASSERT(m_type&T_FUNC);
					return ((CLangContext::CFuncEntry*)this)->m_hArgTypes; }

class	CLangExprCompiler : public CLangProcessor {
public:
	struct SArrayDesc {
		iword type;
		word dim, size;
	};
	class	CType {
	public:
		enum {	T_ERROR = 0, T_VOID = 1, T_ARRAY = 2,
				T_VECTOR = 4,
				T_INT = 0xC, T_FLOAT = 0xD,
				T_SCALAR = 8, T_SINGULAR = 4,
				T_TYPE = 0xF,
				T_LEFT = 0x70,
				T_HEAP = 0x10, T_STACK = 0x20, T_EXT = 0x40,
				T_TYPEINFO = 0xFF };
		int		type;
		word	hArrayDesc;
		CType() { type = T_ERROR; }
		CType(int t) { type = t; }
		int Type() const { return type & T_TYPE; }
		unsigned SizeArg();
		unsigned Size(CLangMemory &mem);
	};
protected:
	struct SArgCode {
		byte  *p; word n;
	};

	CType	m_t;
	unsigned m_flags;
	CLangLexStream	m_ls;
	CLangMemory	m_heap;
	CLangContext m_context, m_funcContext;

	bool 	Compile(PCOMMAND com);
	bool	CompileWord(word w);
	bool	CompileInt(long l);
	bool	CompileFloat(double f);
	bool	Deref();
	bool	CastScalar(int type);
	bool	ApplyPeriod(PCOMMAND dot);
	bool	ApplyBinaryScalar(const CType &t,PCOMMAND oper);
	bool	ApplyComparison(const CType &t,PCOMMAND oper);

	bool	ProcessExprUna();
	bool	ProcessExprProd();
	bool	ProcessExprSum();
	bool	ProcessExprComp();
	bool	ProcessExprAnd();
	bool	ProcessExprOr();
	bool	ProcessExpr() { return ProcessExprOr(); }

	CLangExprCompiler(int (*PrintF)(TCchar *format,...),unsigned nMem,
														unsigned nHeap )
				: m_heap(nHeap), m_context(&m_heap), m_funcContext(&m_heap),
										CLangProcessor(PrintF,nMem) {}
public:
	enum { F_MULTILINE = 1 };
	CLangExprCompiler(int (*PrintF)(TCchar *format,...),unsigned nMem)
						: m_context(&m_heap), m_funcContext(&m_heap),
											CLangProcessor(PrintF,nMem) {}
	~CLangExprCompiler() {}
	CLangValue	Eval(TCchar *lpText,unsigned flags=0);
	unsigned VarOffset(TCchar *name) const;
	unsigned FuncOffset(TCchar *name) const;
	bool Call(unsigned offset) { return CLangProcessor::Call(offset); }
	void *Var(unsigned offset) { return CLangProcessor::Var(offset); }
	bool Call(TCchar *name) { unsigned offset = FuncOffset(name);
					return offset == (unsigned)-1 ? FALSE : Call(offset); }
	void *Var(TCchar *name) { unsigned offset = FuncOffset(name);
					return offset == (unsigned)-1 ? NULL : Var(offset); }
	unsigned GetMemoryFree() const { return m_heap.Remaining(); }
};

class	CLangLabelScope {
public:
	class CLabel {
	private:
		char *name;
		word offset;
		word hNext;
		friend class CLangLabelScope;
	public:
		bool Defined() const { return name[0]!=0; }
		word Offset() const { ASSERT(Defined()); return offset; }
		TCchar *Name() const { return name+1; }
	};
private:
	CLangMemory	*m_pHeap;
	void *m_pMark;
	class CLabelRef {
	public:
		word offset;
		word hNext;
	};
	word	m_hLastLabel;
public:
	CLangLabelScope(CLangMemory *pHeap) {
									m_pHeap = pHeap; m_pMark = NULL; }
	void Open() { ASSERT(!m_pMark); m_hLastLabel = -1;
											m_pMark = m_pHeap->SP(); }
	void Close() { ASSERT(m_pMark); m_pHeap->ReleaseS(m_pMark);
													m_pMark = NULL; }
	CLabel *Label(TCchar *name);
	void Define(CLabel *pLabel,CLangMemory &mem);
	bool AddRef(CLabel *pLabel,CLangMemory &mem);
	CLabel *Unresolved() const;
	bool IsOpen() const { return m_pMark != NULL; }
};

class	CLangLoopScope {
public:
	class CLoop {
	private:
		char *name;
		word offset;
		word hBreaks;
		word hNext;
		friend class CLangLoopScope;
	};
private:
	CLangMemory	*m_pHeap;
	void *m_pMark;
	class CLoopBreak {
	public:
		word offset;
		word hNext;
	};
	word	m_hInnerLoop;
public:
	CLangLoopScope(CLangMemory *pHeap) {
			m_pHeap = pHeap, m_pMark = NULL; }
	void Open() { ASSERT(!m_pMark); m_hInnerLoop = -1;
												m_pMark = m_pHeap->HP(); }
	void Close() { ASSERT(m_pMark); m_pHeap->ReleaseH(m_pMark);
											m_pMark = NULL; }
	CLoop *Find(TCchar *name) const;
	bool BeginLoop(TCchar *name,word offset);
	bool BreakLoop(CLoop *pLoop,word offset);
	void EndLoop(CLangMemory &mem);
	bool IsOpen() const { return m_pMark != NULL; }
	bool IsEmpty() const { ASSERT(IsOpen());
									return m_pHeap->HP() == m_pMark; }
};

class	CLangStatementCompiler : public CLangExprCompiler	{
protected:
	CLangLabelScope	m_labelScope;
	CLangLoopScope	m_loopScope;
	CType	m_retType;
	unsigned	m_nArgsSize;
	static char m_szMsgBuf[300];
	bool 	CompileBuf(TCvoid *pBuf,word len);
	bool	ProcessStatement();
	bool	ProcessScopedStatement();
public:
	CLangStatementCompiler(int (*PrintF)(TCchar *format,...),unsigned nMem,
													unsigned nHeap)
			: CLangExprCompiler(PrintF,nMem,nHeap),
					m_labelScope(&m_heap), m_loopScope(&m_heap) {}
	~CLangStatementCompiler() {}
	bool  	Exec(TCchar *lpText,unsigned flags=0);
};

inline word CLangContext::CEntry::DescOffset() const {
		ASSERT(m_type&CLangExprCompiler::CType::T_ARRAY); return m_p.d; }


class	CLangCompiler : public CLangStatementCompiler {
protected:
	enum { PTF_ARRAYS = 1, PTF_DUMMYDIMS = 2, PTF_ALLOWVOID = 4 };
	bool	ProcessType(unsigned ptflags);
	bool	ProcessFunctionDecl(unsigned functype,
								bool (*pFunc)(CLangProcessor &lp) = NULL);
	bool	ProcessOuterStatement();
public:
	CLangCompiler(int (*PrintF)(TCchar *format,...),unsigned nMem,
														unsigned nHeap)
				: CLangStatementCompiler(PrintF,nMem,nHeap) {}
	~CLangCompiler() {}
	bool  	Process(TCchar *lpText,unsigned flags=F_MULTILINE);
	bool	Process(FILE *f,unsigned flags=F_MULTILINE);
	bool	DeclareExtVar(TCchar *decl,...);
	bool	DeclareExtFunc(TCchar *decl,
								bool (*pFunc)(CLangProcessor &lp));
};

#endif	// __LANG_H__
