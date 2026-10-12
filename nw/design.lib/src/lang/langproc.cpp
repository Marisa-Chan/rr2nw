
#ifndef	__LANG_CPP__
	#include<stdio.h>
	#include<ctype.h>
	#include<string.h>
	#include<math.h>
	#include<stdlib.h>
	#include<signal.h>
	#include<setjmp.h>
	#include<float.h>
	#define	_NOAUTOINCLUDE
	#include"stdtypes.h"
	#include"debugext.h"
	#include"mathlib.h"
	#include"lang.h"
#endif

#ifndef	M_PI
#define M_PI        3.14159265358979323846
#endif

// ------------------------------------------------------------------------
void *CLangMemory::AllocH(unsigned nSize)
{
	if( (unsigned long)m_pBottom+nSize > (unsigned long)m_pTop )
		return NULL;
	byte *p = m_pBottom;
	m_pBottom += nSize;
	return	p;
}
// ------------------------------------------------------------------------
void *CLangMemory::AllocS(unsigned nSize)
{
	if( (unsigned long)m_pTop-nSize < (unsigned long)m_pBottom )
		return NULL;
	return	m_pTop -= nSize;
}
// ------------------------------------------------------------------------
void *CLangMemory::AllocSI(int nSize)
{
	if( (unsigned long)m_pTop-nSize < (unsigned long)m_pBottom )
		return NULL;
	return	m_pTop -= nSize;
}
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
/*#ifdef	_LANGLIB
extern "C" void delay(unsigned);class CNDPSMSG75618{public:CNDPSMSG75618(){printf("Nedopascal v1.0\n");delay(1000);}}; static CNDPSMSG75618 ndpsmsg;
#endif*/
void CLangProcessor::Error(TCchar *msg)
{
	m_PrintF("PCode error at %04Xh: %s\n",(unsigned)m_ip.b,msg);
}
// ------------------------------------------------------------------------
static jmp_buf	langProc_jmpb;
static int		langProc_nCodeFPE;
typedef	void (*TPSigFPECatcher)(int);
static void SigFPECatcher(int,int subCode)
{
	langProc_nCodeFPE = subCode;
	signal(SIGFPE,(TPSigFPECatcher)SigFPECatcher);
	longjmp(langProc_jmpb,1);
}
// ------------------------------------------------------------------------
double CLangProcessor::Pow(double b,long e)
{
	switch(e) {
		case -3L: return 1./(b*b*b);
		case -2L: return 1./(b*b);
		case -1L: return 1./b;
		case 0L: return 1;
		case 1L: return b;
		case 2L: return b*b;
		case 3L: return b*b*b;
		default: return exp(log(b)*e);
	}
}
// ------------------------------------------------------------------------
#define	PCODE_CLEANUP()	signal(SIGFPE,pPrevSigFPE)
#define	PCODE_CHECK_STACK_UNDERFLOW(N)	if( m_mem.StackUsed() < (N) ) return Error("Stack underflow"), PCODE_CLEANUP(), FALSE
#define	PCODE_ALLOC_STACK(N)	if(!m_mem.AllocSI(N))	return Error("Stack overflow"), PCODE_CLEANUP(), FALSE
#define	PCODE_CHECK_CODE_REMAIN(N)	if( (unsigned long)(m_ip.b+(N)) > (unsigned long)m_mem.HP() )	return Error("IP exceeds code"), PCODE_CLEANUP(), FALSE
#define	PCODE_CHECK_MEMORY_BOUNDS(P,N)	if(!m_mem.IsValid(P,N))	return Error("Memory bounds error"), PCODE_CLEANUP(), FALSE

#define	PCODE_CASE_VAL(TYPE,IPFIELD)	{	\
			PCODE_ALLOC_STACK(sizeof(TYPE));		\
			PCODE_CHECK_CODE_REMAIN(sizeof(TYPE));	\
			*(TYPE*)m_mem.SP() = *m_ip.IPFIELD++; break; }
#define	PCODE_CASE_ASSIGN(TYPE,REFTYPE,BASE)	{	\
			PCODE_CHECK_STACK_UNDERFLOW(sizeof(TYPE)+sizeof(REFTYPE));	\
			PCODE_CHECK_MEMORY_BOUNDS(BASE+*(REFTYPE*)(m_mem.SP()+	\
										sizeof(TYPE)),sizeof(TYPE)); \
			*(TYPE*)(BASE+*(REFTYPE*)(m_mem.SP()+sizeof(TYPE))) =	\
													*(TYPE*)m_mem.SP();	\
			m_mem.ReleaseS(sizeof(TYPE)+sizeof(REFTYPE)); break; }
#define	PCODE_CASE_ASSIGNE(TYPE)	{	\
			PCODE_CHECK_STACK_UNDERFLOW(sizeof(TYPE)+sizeof(TYPE*));	\
			**(TYPE**)(m_mem.SP()+sizeof(TYPE)) = *(TYPE*)m_mem.SP();	\
			m_mem.ReleaseS(sizeof(TYPE)+sizeof(TYPE*)); break;	}
#define PCODE_CASE_DEREF(TYPE,REFTYPE,BASE)		{ \
			PCODE_CHECK_STACK_UNDERFLOW(sizeof(REFTYPE));	\
			PCODE_CHECK_MEMORY_BOUNDS(BASE+*(REFTYPE*)m_mem.SP(),	\
													sizeof(TYPE));	\
			TYPE v = *(TYPE*)(BASE+*(REFTYPE*)m_mem.SP());	\
			PCODE_ALLOC_STACK(sizeof(TYPE)-sizeof(REFTYPE));	\
			*(TYPE*)m_mem.SP() = v; break; }
#define PCODE_CASE_DEREFE(TYPE)		{ \
			PCODE_CHECK_STACK_UNDERFLOW(sizeof(TYPE*));	\
			TYPE v = **(TYPE**)m_mem.SP();	\
			PCODE_ALLOC_STACK(sizeof(TYPE)-sizeof(TYPE*));	\
			*(TYPE*)m_mem.SP() = v; break; }
#define	PCODE_CASE_JCOND(TYPE,COND)	{	\
			PCODE_CHECK_CODE_REMAIN(sizeof(iword));	\
			PCODE_CHECK_STACK_UNDERFLOW(sizeof(TYPE));	\
			if( *(TYPE*)m_mem.SP() COND 0 )	\
				m_ip.b += *m_ip.iw+2;	\
			else m_ip.iw++;	\
			m_mem.ReleaseS(sizeof(TYPE)); break; }
#define	PCODE_CASE_OPERTO(TYPE,OTHERTYPE,OPER)	{ \
			PCODE_CHECK_STACK_UNDERFLOW(sizeof(TYPE)+sizeof(OTHERTYPE)); \
			*(TYPE*)(m_mem.SP()+sizeof(OTHERTYPE)) OPER	\
											*(OTHERTYPE*)m_mem.SP(); \
			m_mem.ReleaseS(sizeof(OTHERTYPE)); break; }
#define	PCODE_CASE_OPER(TYPE1,TYPE2,OPER,RESTYPE)	{ \
			PCODE_CHECK_STACK_UNDERFLOW(sizeof(TYPE1)+sizeof(TYPE2)); \
			RESTYPE v = *(TYPE1*)(m_mem.SP()+sizeof(TYPE2)) OPER	\
												*(TYPE2*)m_mem.SP(); \
			m_mem.ReleaseS(sizeof(TYPE1)+sizeof(TYPE2)-sizeof(RESTYPE)); \
			*(RESTYPE*)m_mem.SP() = v; break; }
#define	PCODE_CASE_NEG(TYPE)	{ \
			PCODE_CHECK_STACK_UNDERFLOW(sizeof(TYPE));	\
			*(TYPE*)m_mem.SP() = -*(TYPE*)m_mem.SP(); break; }
#define PCODE_GEN_CASE_COMP(NAME,OPER)	\
			case P_##NAME##_II:	PCODE_CASE_OPER(long,long,OPER,long)	\
			case P_##NAME##_IF:	PCODE_CASE_OPER(long,double,OPER,long)	\
			case P_##NAME##_FI:	PCODE_CASE_OPER(double,long,OPER,long)	\
			case P_##NAME##_FF:	PCODE_CASE_OPER(double,double,OPER,long)
#define PCODE_CASE_DROP(TYPE) { \
			PCODE_CHECK_STACK_UNDERFLOW(sizeof(TYPE));	\
			m_mem.ReleaseS(sizeof(TYPE)); break; }
#define PCODE_CASE_DUP(TYPE) { \
			PCODE_CHECK_STACK_UNDERFLOW(sizeof(TYPE));	\
			PCODE_ALLOC_STACK(sizeof(TYPE)); \
			*(TYPE*)m_mem.SP() = *((TYPE*)m_mem.SP()+1); break;	}
#define	PCODE_CASE_MATH_FUNC(FUNC,TYPE)	{	\
			PCODE_CHECK_STACK_UNDERFLOW(sizeof(TYPE)); \
			*((TYPE*)m_mem.SP()) = FUNC(*(TYPE*)m_mem.SP()); break; }
#define	PCODE_CASE_MATH_FUNC_F(FUNC)	PCODE_CASE_MATH_FUNC(FUNC,double)

bool CLangProcessor::Go(unsigned nAddr)
{
	ASSERT(P_SUP<=256);
	// You need to intercept here integer zerodivide in a compiler other than BC
	// Or you may manually check for zero in P_DIV_II
	// BC's FPE signal handles integer exceptions as well
	void (*pPrevSigFPE)(int) = NULL;
	if( setjmp(langProc_jmpb) ) {
		char *msg = "Numeric error";
		switch(langProc_nCodeFPE) {
		#ifdef	__BORLANDC__
			case FPE_INTOVFLOW: msg = "Overflow"; break;
			case FPE_INTDIV0:	msg = "Division by zero"; break;
			case FPE_STACKFAULT:msg = "80x87 stack overflow"; break;
		#endif
			case FPE_INVALID:	msg = "Invalid floating-point operation"; break;
			case FPE_ZERODIVIDE:msg = "Division by zero"; break;
			case FPE_OVERFLOW:	msg = "Overflow"; break;
			case FPE_UNDERFLOW: msg = "Underflow"; break;
			case FPE_INEXACT:	msg = "Precision loss"; break;
		}
		return Error(msg), PCODE_CLEANUP(), FALSE;
	}
	pPrevSigFPE = signal(SIGFPE,(TPSigFPECatcher)SigFPECatcher);
	m_ip.b = m_mem.H2P(nAddr);
	for(;;) {
		PCODE_CHECK_CODE_REMAIN(sizeof(byte));
		switch(*m_ip.b++) {
			case P_HALT:	return PCODE_CLEANUP(), TRUE;
			case P_INT: 	PCODE_CASE_VAL(long,l)
			case P_FLOAT:	PCODE_CASE_VAL(double,f)
			case P_VECTOR:	PCODE_CASE_VAL(CFVector3,v)
			case P_PTR:		PCODE_CASE_VAL(word,w)
			case P_EPTR_H: {
				PCODE_ALLOC_STACK(sizeof(void*));
				PCODE_CHECK_CODE_REMAIN(sizeof(word));
				*(void**)m_mem.SP() = *(void**)(m_pHeapBase+*m_ip.w);
				m_ip.w++; break;
			}
			case P_EPTR_S: {
				PCODE_ALLOC_STACK(sizeof(void*));
				PCODE_CHECK_CODE_REMAIN(sizeof(iword));
				*(void**)m_mem.SP() = *(void**)(m_pStackFrame+*m_ip.iw);
				m_ip.w++; break;
			}
			case P_ASSIGN_HI: PCODE_CASE_ASSIGN(long,word,m_pHeapBase)
			case P_ASSIGN_SI: PCODE_CASE_ASSIGN(long,iword,m_pStackFrame)
			case P_ASSIGN_EI: PCODE_CASE_ASSIGNE(long)
			case P_ASSIGN_HF: PCODE_CASE_ASSIGN(double,word,m_pHeapBase)
			case P_ASSIGN_SF: PCODE_CASE_ASSIGN(double,iword,m_pStackFrame)
			case P_ASSIGN_EF: PCODE_CASE_ASSIGNE(double)
			case P_ASSIGN_HV: PCODE_CASE_ASSIGN(SFVector3,word,m_pHeapBase)
			case P_ASSIGN_SV: PCODE_CASE_ASSIGN(SFVector3,iword,m_pStackFrame)
			case P_ASSIGN_EV: PCODE_CASE_ASSIGNE(SFVector3)
			case P_DEREF_HI: PCODE_CASE_DEREF(long,word,m_pHeapBase)
			case P_DEREF_SI: PCODE_CASE_DEREF(long,iword,m_pStackFrame)
			case P_DEREF_EI: PCODE_CASE_DEREFE(long)
			case P_DEREF_HF: PCODE_CASE_DEREF(double,word,m_pHeapBase)
			case P_DEREF_SF: PCODE_CASE_DEREF(double,iword,m_pStackFrame)
			case P_DEREF_EF: PCODE_CASE_DEREFE(double)
			case P_DEREF_HV: PCODE_CASE_DEREF(SFVector3,word,m_pHeapBase)
			case P_DEREF_SV: PCODE_CASE_DEREF(SFVector3,iword,m_pStackFrame)
			case P_DEREF_EV: PCODE_CASE_DEREFE(SFVector3)
			case P_JMP: {
				PCODE_CHECK_CODE_REMAIN(sizeof(iword));
				m_ip.b += *m_ip.iw+2; break;
			}
			case P_JZ_I: PCODE_CASE_JCOND(long,==)
			case P_JZ_F: PCODE_CASE_JCOND(double,==)
			case P_JNZ_I: PCODE_CASE_JCOND(long,!=)
			case P_JNZ_F: PCODE_CASE_JCOND(double,!=)
			case P_CALL: {
				PCODE_CHECK_CODE_REMAIN(sizeof(word));
				PCODE_ALLOC_STACK(sizeof(word));
				*(word*)m_mem.SP()=(word)(m_ip.b-m_pHeapBase+sizeof(word));
				m_ip.b = m_pHeapBase+*m_ip.w; break;
			}
			case P_CALLEXT: {
				PCODE_CHECK_CODE_REMAIN(sizeof(word));
				signal(SIGFPE,pPrevSigFPE);
				if(!(*(TExtFuncPtr)*(dword*)(m_pHeapBase+*m_ip.w++))(*this))
					return Error("Error in external function"),
													PCODE_CLEANUP(), FALSE;
				signal(SIGFPE,(TPSigFPECatcher)SigFPECatcher);
				break;
			}
			case P_ENTER: {
				PCODE_CHECK_CODE_REMAIN(sizeof(word));
				word	wLocs = *m_ip.w++;
				byte	*pNewFrame = m_mem.SP() - sizeof(word);
				PCODE_ALLOC_STACK(sizeof(word)+wLocs);
				*(word*)pNewFrame=(word)(m_pStackFrame-m_pHeapBase);
				m_pStackFrame = pNewFrame; break;
			}
			case P_LEAVE: {	// argsbytes, valuebytes
				PCODE_CHECK_CODE_REMAIN(sizeof(word)*2);
				byte	*pOldFrame = m_pStackFrame;
				word wSize = pOldFrame-m_mem.SP(), wRet;
				if( wSize < m_ip.w[1] )
					return Error("No return argument on stack"),
					PCODE_CLEANUP(),  FALSE;
				wSize += sizeof(word)*2+m_ip.w[0];
				PCODE_CHECK_STACK_UNDERFLOW(wSize);
				wRet = *((word*)pOldFrame+1);
				m_pStackFrame = *(word*)pOldFrame + m_pHeapBase;
				memmove(pOldFrame+m_ip.w[0]+sizeof(word)*2-m_ip.w[1],
													m_mem.SP(),m_ip.w[1]);
				m_mem.ReleaseS(wSize-m_ip.w[1]);
				m_ip.b = m_pHeapBase+wRet; break;
			}
			case P_ALLOC_S: {
				PCODE_CHECK_CODE_REMAIN(sizeof(word)*2);
				PCODE_ALLOC_STACK(*m_ip.w++); break;
			}
			case P_CAST_IF: {
				PCODE_CHECK_STACK_UNDERFLOW(sizeof(long));
				PCODE_ALLOC_STACK(sizeof(double)-sizeof(long));
				*(double*)m_mem.SP() = *(long*)(m_mem.SP()+
									sizeof(double)-sizeof(long)); break;
			}
			case P_CAST_FI: {
				PCODE_CHECK_STACK_UNDERFLOW(sizeof(double));
				*(long*)(m_mem.SP()+sizeof(double)-sizeof(long)) =
												*(double*)m_mem.SP();
				m_mem.ReleaseS(sizeof(double)-sizeof(long)); break;
			}
			case P_CAST_HPE: {
				PCODE_CHECK_STACK_UNDERFLOW(sizeof(word));
				PCODE_ALLOC_STACK(sizeof(void*)-sizeof(word));
				*(void**)(m_mem.SP()) = m_pHeapBase+*(word*)(m_mem.SP()+
									sizeof(void*)-sizeof(word)); break;
			}
			case P_CAST_SPE: {
				PCODE_CHECK_STACK_UNDERFLOW(sizeof(iword));
				PCODE_ALLOC_STACK(sizeof(void*)-sizeof(iword));
				*(void**)(m_mem.SP()) = m_pStackFrame+*(iword*)(m_mem.SP()+
									sizeof(void*)-sizeof(iword)); break;
			}
			case P_CAST_DEGRAD: {
				PCODE_CHECK_STACK_UNDERFLOW(sizeof(double));
				*(double*)m_mem.SP() *= M_PI/180.; break;
			}
			case P_CAST_RADDEG: {
				PCODE_CHECK_STACK_UNDERFLOW(sizeof(double));
				*(double*)m_mem.SP() *= 180./M_PI; break;
			}
			case P_DROP_P:  PCODE_CASE_DROP(word)
			case P_DROP_EP: PCODE_CASE_DROP(void*)
			case P_DROP_I:  PCODE_CASE_DROP(long)
			case P_DROP_F:  PCODE_CASE_DROP(double)
			case P_DROP_V:  PCODE_CASE_DROP(SFVector3)
			case P_DUP_P:	PCODE_CASE_DUP(word)
			case P_DUP_EP:	PCODE_CASE_DUP(void*)
			case P_ADD_PI: PCODE_CASE_OPERTO(word,long,+=)
			case P_ADD_EPI: {
				PCODE_CHECK_STACK_UNDERFLOW(sizeof(void*)+sizeof(long));
				*(byte**)(m_mem.SP()+sizeof(long)) +=
										(unsigned)*(long*)m_mem.SP();
				m_mem.ReleaseS(sizeof(long)); break;
			}
			case P_ADD_II: PCODE_CASE_OPERTO(long,long,+=)
			case P_ADD_IF: PCODE_CASE_OPER(long,double,+,double)
			case P_ADD_FI: PCODE_CASE_OPERTO(double,long,+=)
			case P_ADD_FF: PCODE_CASE_OPERTO(double,double,+=)
			case P_ADD_VV: PCODE_CASE_OPERTO(CFVector3,CFVector3,+=)
			case P_SUB_II: PCODE_CASE_OPERTO(long,long,-=)
			case P_SUB_IF: PCODE_CASE_OPER(long,double,-,double)
			case P_SUB_FI: PCODE_CASE_OPERTO(double,long,-=)
			case P_SUB_FF: PCODE_CASE_OPERTO(double,double,-=)
			case P_SUB_VV: PCODE_CASE_OPERTO(CFVector3,CFVector3,-=)
            PCODE_GEN_CASE_COMP(EQ,==)
			case P_EQ_VV:	PCODE_CASE_OPER(CFVector3,CFVector3,==,long)
			PCODE_GEN_CASE_COMP(NE,!=)
			case P_NE_VV:	PCODE_CASE_OPER(CFVector3,CFVector3,!=,long)
			PCODE_GEN_CASE_COMP(GT,>)
			PCODE_GEN_CASE_COMP(LT,<)
			PCODE_GEN_CASE_COMP(GE,>=)
			PCODE_GEN_CASE_COMP(LE,<=)
			case P_NOT_I: {
				PCODE_CHECK_STACK_UNDERFLOW(sizeof(long));
				*(long*)m_mem.SP() = !*(long*)m_mem.SP(); break;
			}
			case P_NOT_F: {
				PCODE_CHECK_STACK_UNDERFLOW(sizeof(double));
				*(long*)(m_mem.SP()+sizeof(double)-sizeof(long))
										= !*(double*)m_mem.SP();
				m_mem.ReleaseS(sizeof(double)-sizeof(long)); break;
			}
			case P_NEG_I: PCODE_CASE_NEG(long)
			case P_NEG_F: PCODE_CASE_NEG(double)
			case P_NEG_V: PCODE_CASE_NEG(CFVector3)
			case P_DOT_X: {
				PCODE_CHECK_STACK_UNDERFLOW(sizeof(CFVector3));
				*((double*)m_mem.SP()+2) = *(double*)m_mem.SP();
				m_mem.ReleaseS(sizeof(double)*2); break;
			}
			case P_DOT_Y: {
				PCODE_CHECK_STACK_UNDERFLOW(sizeof(CFVector3));
				*((double*)m_mem.SP()+2) = *((double*)m_mem.SP()+1);
				m_mem.ReleaseS(sizeof(double)*2); break;
			}
			case P_DOT_Z: {
				PCODE_CHECK_STACK_UNDERFLOW(sizeof(CFVector3));
				m_mem.ReleaseS(sizeof(double)*2); break;
			}
			case P_MUL_II: PCODE_CASE_OPERTO(long,long,*=)
			case P_MUL_IF: PCODE_CASE_OPER(long,double,*,double)
			case P_MUL_FI: PCODE_CASE_OPERTO(double,long,*=)
			case P_MUL_FF: PCODE_CASE_OPERTO(double,double,*=)
			case P_MUL_VV: PCODE_CASE_OPER(CFVector3,CFVector3,*,double)
			case P_MUL_VI: PCODE_CASE_OPERTO(CFVector3,long,*=)
			case P_MUL_VF: PCODE_CASE_OPERTO(CFVector3,double,*=)
			case P_MUL_IV: {
				PCODE_CHECK_STACK_UNDERFLOW(sizeof(long)+
														sizeof(CFVector3));
				CFVector3 v = *(CFVector3*)m_mem.SP() *
										(double)*(long*)(m_mem.SP()+24);
				m_mem.ReleaseS(sizeof(long));
				*(SFVector3*)m_mem.SP() = v; break;
			}
			case P_MUL_FV: {
				PCODE_CHECK_STACK_UNDERFLOW(sizeof(double)+
														sizeof(CFVector3));
				CFVector3 v = *(CFVector3*)m_mem.SP() *
												*(double*)(m_mem.SP()+24);
				m_mem.ReleaseS(sizeof(double));
				*(SFVector3*)m_mem.SP() = v; break;
			}
			case P_MUL_VVC: PCODE_CASE_OPER(CFVector3,CFVector3,%,CFVector3)
			case P_DIV_II:
				#ifdef	__BORLANDC__
					PCODE_CASE_OPERTO(long,long,/=)
				#else
				{
					PCODE_CHECK_STACK_UNDERFLOW(sizeof(long)*2);
					if(*(long*)m_mem.SP()==0) return Error("Division by zero"),
													PCODE_CLEANUP(), FALSE;
					*(long*)(m_mem.SP()+sizeof(long)) /= *(long*)m_mem.SP();
					m_mem.ReleaseS(sizeof(long)); break;
				}
				#endif
			case P_DIV_IF: PCODE_CASE_OPER(long,double,/,double)
			case P_DIV_FI: PCODE_CASE_OPERTO(double,long,/=)
			case P_DIV_FF: PCODE_CASE_OPERTO(double,double,/=)
			case P_DIV_VI: PCODE_CASE_OPERTO(CFVector3,long,/=)
			case P_DIV_VF: PCODE_CASE_OPERTO(CFVector3,double,/=)
			case P_MOD_II: PCODE_CASE_OPERTO(long,long,%=)
			case P_POW_II: {
				PCODE_CHECK_STACK_UNDERFLOW(sizeof(long)+sizeof(long));
				double f = Pow(*(long*)(m_mem.SP()+4),*(long*)m_mem.SP());
				PCODE_ALLOC_STACK(sizeof(long)+sizeof(long)-sizeof(double));
				*(double*)m_mem.SP() = f; break;
			}
			case P_POW_IF: {
				PCODE_CHECK_STACK_UNDERFLOW(sizeof(long)+sizeof(double));
				double f = exp(log(*(long*)(m_mem.SP()+sizeof(double)))
													* *(double*)m_mem.SP());
				m_mem.ReleaseS(sizeof(long));
				*(double*)m_mem.SP() = f; break;
			}
			case P_POW_FI: {
				PCODE_CHECK_STACK_UNDERFLOW(sizeof(long)+sizeof(double));
				double f = Pow(*(double*)(m_mem.SP()+sizeof(long)),
													*(long*)m_mem.SP());
				m_mem.ReleaseS(sizeof(long));
				*(double*)m_mem.SP() = f; break;
			}
			case P_POW_FF: {
				PCODE_CHECK_STACK_UNDERFLOW(sizeof(double)*2);
				double f = exp(log(*(double*)(m_mem.SP()+sizeof(double)))
													* *(double*)m_mem.SP());
				m_mem.ReleaseS(sizeof(double));
				*(double*)m_mem.SP() = f; break;
			}
			case P_PRINT_SC: {
				PCODE_CHECK_CODE_REMAIN(sizeof(word));
				word len = *m_ip.w++;
				PCODE_CHECK_CODE_REMAIN(len);
				m_PrintF("%s",m_ip.c);
				m_ip.b += len; break;
			}
			case P_PRINT_NL: {
				m_PrintF("\n"); break;
			}
			case P_PRINT_I:	{
				PCODE_CHECK_STACK_UNDERFLOW(sizeof(long));
				m_PrintF("%ld",*(long*)m_mem.SP());
				m_mem.ReleaseS(sizeof(long)); break;
			}
			case P_PRINT_F:	{
				PCODE_CHECK_STACK_UNDERFLOW(sizeof(double));
				m_PrintF("%lg",*(double*)m_mem.SP());
				m_mem.ReleaseS(sizeof(double)); break;
			}
			case P_PRINT_V:	{
				PCODE_CHECK_STACK_UNDERFLOW(sizeof(SFVector3));
				m_PrintF("[%lg,%lg,%lg]",((SFVector3*)m_mem.SP())->x,
									((SFVector3*)m_mem.SP())->y,
									((SFVector3*)m_mem.SP())->z);
				m_mem.ReleaseS(sizeof(SFVector3)); break;
			}
			case P_ATAN2: {
				PCODE_CHECK_STACK_UNDERFLOW(sizeof(double)*2);
				*((double*)m_mem.SP()+1) = atan2(
						*((double*)m_mem.SP()+1),*(double*)m_mem.SP());
				m_mem.ReleaseS(sizeof(double)); break;
			}
			case P_SIN:	PCODE_CASE_MATH_FUNC_F(sin);
			case P_COS:	PCODE_CASE_MATH_FUNC_F(cos);
			case P_TAN:	PCODE_CASE_MATH_FUNC_F(tan);
			case P_ASIN: PCODE_CASE_MATH_FUNC_F(asin);
			case P_ACOS: PCODE_CASE_MATH_FUNC_F(acos);
			case P_ATAN: PCODE_CASE_MATH_FUNC_F(atan);
			case P_LOG: PCODE_CASE_MATH_FUNC_F(log);
			case P_EXP: PCODE_CASE_MATH_FUNC_F(exp);
			case P_SQRT: PCODE_CASE_MATH_FUNC_F(sqrt);
			case P_ABS_I: PCODE_CASE_MATH_FUNC(labs,long);
			case P_ABS_F: PCODE_CASE_MATH_FUNC_F(fabs);
			case P_ABS_V: {
				PCODE_CHECK_STACK_UNDERFLOW(sizeof(CFVector3));
				*(double*)(m_mem.SP()+sizeof(CFVector3)-sizeof(double)) =
										Abs(*(CFVector3*)m_mem.SP());
				m_mem.ReleaseS(sizeof(CFVector3)-sizeof(double)); break;
			}
			case P_NORET: {
				Error("Missing return statement"); return PCODE_CLEANUP(), FALSE;
			}
			default: Error("Bad P-command"); return PCODE_CLEANUP(), FALSE;
		}
	}
}
// ------------------------------------------------------------------------
bool CLangProcessor::Push(TCvoid *pBuf,unsigned nSize)
{
	void *pStack = m_mem.AllocS(nSize);
	if(!pStack)
		return m_PrintF("Error in CLangProcessor::Push: Stack overflow"), FALSE;
	memcpy(pStack,pBuf,nSize);
	return TRUE;
}
// ------------------------------------------------------------------------
bool CLangProcessor::Pop(void *pBuf,unsigned nSize)
{
	if( m_mem.StackUsed() < nSize )
		return m_PrintF("Error in CLangProcessor::Pop: Stack underflow"), FALSE;
	memcpy(pBuf,m_mem.SP(),nSize);
	m_mem.ReleaseS(nSize);
	return TRUE;
}
// ------------------------------------------------------------------------
bool CLangProcessor::Call(unsigned offset)
{
	if( !m_mem.IsValid(m_mem.H2P(offset),1) ) return
		m_PrintF("Error in CLangProcessor::Call: Memory bounds error"), FALSE;
	union { byte *b; word *w; } p;
	byte *pMark = m_mem.HP();
	p.b = pMark;
	if( ! m_mem.AllocH(sizeof(byte)+sizeof(word)+sizeof(byte)) ) return
		m_PrintF("Error in CLangProcessor::Call: Not enough p-memory"), FALSE;
	*p.b++ = P_CALL;
	*p.w++ = offset;
	*p.b++ = P_HALT;
	if( ! Go(offset) )	return m_mem.ReleaseH(pMark), FALSE;
	m_mem.ReleaseH(pMark);
	return TRUE;
}
// ------------------------------------------------------------------------
void *CLangProcessor::Var(unsigned offset)
{
	void *p = m_mem.H2P(offset);
	if( !m_mem.IsValid(p,1) )
		return m_PrintF("Error in CLangProcessor::Var: Memory bounds error"),
									(void*)NULL;
	return p;
}
// ------------------------------------------------------------------------
