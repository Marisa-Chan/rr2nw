
#ifndef	__LANG_CPP__
	#include<stdio.h>
	#include<ctype.h>
	#include<string.h>
	#include<math.h>
	#include<stdlib.h>
	#include<stdarg.h>
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
CLangContext::CEntry *CLangContext::Find(TCchar *name) const
{
	for( word hEntry = m_hLastEntry ; hEntry != (word)-1 ; ) {
		CEntry	*pEntry = (CEntry*)m_pHeap->H2P(hEntry);
		TCchar	*pName = (TCchar*)m_pHeap->H2P(pEntry->m_hName);
		if( strcmp(pName,name)==0 )	return pEntry;
		hEntry = pEntry->m_hNext;
	}
	return NULL;
}
// ------------------------------------------------------------------------
unsigned CLangExprCompiler::CType::SizeArg()
{
	switch(Type()) {
		case T_ARRAY:	return sizeof(void*);
		case T_INT:	return sizeof(long);
		case T_FLOAT:	return sizeof(double);
		case T_VECTOR:	return sizeof(SFVector3);
		default: ASSERT(0); return 0;
	}
}
// ------------------------------------------------------------------------
unsigned CLangExprCompiler::CType::Size(CLangMemory &mem)
{
	switch(Type()) {
		case T_ARRAY:	{
			CLangExprCompiler::SArrayDesc	*pDesc =
				(CLangExprCompiler::SArrayDesc*) mem.H2P(hArrayDesc);
			ASSERT(pDesc->dim>0);
			return	pDesc->dim*pDesc->size;
		}
		default: return SizeArg();
	}
}
// ------------------------------------------------------------------------
bool CLangExprCompiler::Compile(PCOMMAND com)
{
	byte	*p = m_mem.HP();
	if( !m_mem.AllocH(sizeof(byte)) )	return
								m_ls.Error("Not enough p-memory"), FALSE;
	*p = com;
	return TRUE;
}
#ifdef	_DISPPCODE
	#define	Compile(C) (m_PrintF("\n%04Xh: %s",m_mem.HN(),#C),Compile(C))
#endif
// ------------------------------------------------------------------------
bool CLangExprCompiler::CompileWord(word w)
{
	#ifdef	_DISPPCODE
		m_PrintF(" Word=%04Xh",w);
	#endif
	word	*p = (word*)m_mem.HP();
	if( !m_mem.AllocH(sizeof(word)) )	return
								m_ls.Error("Not enough p-memory"), FALSE;
	*p = w;
	return TRUE;
}
// ------------------------------------------------------------------------
bool CLangExprCompiler::CompileInt(long l)
{
	#ifdef	_DISPPCODE
		m_PrintF(" Int=%ld",l);
	#endif
	long	*p = (long*)m_mem.HP();
	if( !m_mem.AllocH(sizeof(long)) )	return
								m_ls.Error("Not enough p-memory"), FALSE;
	*p = l;
	return TRUE;
}
// ------------------------------------------------------------------------
bool CLangExprCompiler::CompileFloat(double f)
{
	#ifdef	_DISPPCODE
		m_PrintF(" FLoat=%lg",f);
	#endif
	double	*p = (double*)m_mem.HP();
	if( !m_mem.AllocH(sizeof(double)) )	return
								m_ls.Error("Not enough p-memory"), FALSE;
	*p = f;
	return TRUE;
}
// ------------------------------------------------------------------------
bool CLangExprCompiler::Deref()
{
	if( !(m_t.type & m_t.T_LEFT) )	return TRUE;
	ASSERT(m_t.type & m_t.T_SINGULAR);
	switch(m_t.Type()) {
		case m_t.T_INT:
			if( m_t.type & m_t.T_HEAP ) {
				if( !Compile(P_DEREF_HI)) return FALSE;
			} else
			if( m_t.type & m_t.T_STACK ) {
				if( !Compile(P_DEREF_SI)) return FALSE;
			} else
			if( m_t.type & m_t.T_EXT )	{
				if( !Compile(P_DEREF_EI)) return FALSE;
			} else {
				ASSERT(0); return FALSE;
			}
			break;
		case m_t.T_FLOAT:
			if( m_t.type & m_t.T_HEAP ) {
				if( !Compile(P_DEREF_HF)) return FALSE;
			} else
			if( m_t.type & m_t.T_STACK ) {
				if( !Compile(P_DEREF_SF)) return FALSE;
			} else
			if( m_t.type & m_t.T_EXT )	{
				if( !Compile(P_DEREF_EF)) return FALSE;
			} else {
				ASSERT(0); return FALSE;
			}
			break;
		case m_t.T_VECTOR:
			if( m_t.type & m_t.T_HEAP ) {
				if( !Compile(P_DEREF_HV)) return FALSE;
			} else
			if( m_t.type & m_t.T_STACK ) {
				if( !Compile(P_DEREF_SV)) return FALSE;
			} else
			if( m_t.type & m_t.T_EXT )	{
				if( !Compile(P_DEREF_EV)) return FALSE;
			} else {
				ASSERT(0); return FALSE;
			}
			break;
		default: ASSERT(0); return FALSE;
	}
	m_t.type &= ~m_t.T_LEFT;
	return TRUE;
}
// ------------------------------------------------------------------------
bool CLangExprCompiler::CastScalar(int type)
{
	ASSERT(type == m_t.T_INT || type == m_t.T_FLOAT);
	if( !(m_t.type & m_t.T_SCALAR) )
		return m_ls.Error("Scalar expression expected"), FALSE;
	if( !Deref() )	return FALSE;
	switch( type ) {
		case m_t.T_INT:
			switch( m_t.Type() ) {
				case m_t.T_INT: break;
				case m_t.T_FLOAT:
					if(!Compile(P_CAST_FI)) return FALSE;
					m_t.type = m_t.T_INT; break;
				default: ASSERT(0);
			} break;
		case m_t.T_FLOAT:
			switch( m_t.Type() ) {
				case m_t.T_FLOAT: break;
				case m_t.T_INT:
					if(!Compile(P_CAST_IF)) return FALSE;
					m_t.type = m_t.T_FLOAT; break;
				default: ASSERT(0);
			} break;
		default: ASSERT(0);
	}
	return TRUE;
}
// ------------------------------------------------------------------------
bool CLangExprCompiler::ApplyPeriod(PCOMMAND dot)
{
	ASSERT(m_t.Type() == m_t.T_VECTOR);
	if( m_t.type & m_t.T_LEFT ) {
		long	offset = 0;
		switch(dot) {
			case P_DOT_X:	offset = 0; break;
			case P_DOT_Y:	offset = sizeof(double); break;
			case P_DOT_Z:	offset = sizeof(double)*2; break;
			default: ASSERT(0);
		}
		if( !Compile(P_INT) || !CompileInt((long)offset) )	return FALSE;
		m_t.type &= ~m_t.T_TYPE;
		m_t.type |= m_t.T_FLOAT;
		if( m_t.type & m_t.T_EXT )	return Compile(P_ADD_EPI);
		return	Compile(P_ADD_PI);
	}
	else {
		m_t.type = m_t.T_FLOAT;
		return Compile(dot);
	}
}
// ------------------------------------------------------------------------
bool CLangExprCompiler::ApplyBinaryScalar(const CType &t,PCOMMAND oper)
{
	ASSERT(!(m_t.type&m_t.T_LEFT) && !(t.type&t.T_LEFT));
	ASSERT((m_t.type&m_t.T_SCALAR) && (t.type&t.T_SCALAR));
	if(t.Type() == t.T_FLOAT) {
		if(m_t.Type() == m_t.T_FLOAT)	return Compile((PCOMMAND)(oper+3));
		else return m_t.type = m_t.T_FLOAT, Compile((PCOMMAND)(oper+2));
	}
	else {
		if(m_t.Type() == m_t.T_FLOAT)	return Compile((PCOMMAND)(oper+1));
		else return Compile(oper);
	}
}
// ------------------------------------------------------------------------
bool CLangExprCompiler::ApplyComparison(const CType &t,PCOMMAND oper)
{
	ASSERT(!(m_t.type&m_t.T_LEFT) && !(t.type&t.T_LEFT));
	ASSERT((m_t.type&m_t.T_SCALAR) && (t.type&t.T_SCALAR));
	if(t.Type() == t.T_FLOAT) {
		if(m_t.Type() == m_t.T_INT)	return Compile((PCOMMAND)(oper+2));
		else return m_t.type = m_t.T_INT, Compile((PCOMMAND)(oper+3));
	}
	else {
		if(m_t.Type() == m_t.T_INT)	 return Compile(oper);
		else return m_t.type = m_t.T_INT, Compile((PCOMMAND)(oper+1));
	}
}
// ------------------------------------------------------------------------
bool CLangExprCompiler::ProcessExprUna()
{
	bool	bNegate = FALSE, bNegates = FALSE;
	int		nPrefixTildes = 0;
	for(;;) {
		if( m_ls.Get() == CLangLex::T_PLUS ) { bNegates = TRUE; } else
		if( m_ls.This() == CLangLex::T_MINUS ) {
			bNegate = !bNegate; bNegates = TRUE;
		} else
		if( m_ls.This() == CLangLex::T_TILDE ) {
			if( ++nPrefixTildes > 100 )
				return m_ls.Error("How many tildes can you afford?"), FALSE;
		}
		else break;
	}
	if( m_ls.This() == CLangLex::T_INTV ) {
		if(! Compile(P_INT) ||
				!CompileInt((long)m_ls.This()) ) return FALSE;
		m_t.type = m_t.T_INT;
	} else
	if( m_ls.This() == CLangLex::T_FLOATV ) {
		if(! Compile(P_FLOAT) ||
				!CompileFloat((double)m_ls.This()) ) return FALSE;
		m_t.type = m_t.T_FLOAT;
	} else
	if( m_ls.This() == CLangLex::T_PI ) {
		if(! Compile(P_FLOAT) ||
				!CompileFloat(M_PI) )	return FALSE;
		m_t.type = m_t.T_FLOAT;
	} else
	if( m_ls.This() == CLangLex::T_LBRAK ) {	// Vector
		byte  *pStart = m_mem.HP();
		if( !ProcessExpr() )	return FALSE;
		if( !CastScalar(m_t.T_FLOAT) )	return FALSE;
		unsigned  nCountX = (unsigned)(m_mem.HP() - pStart);
		m_mem.ReleaseH(pStart);
		byte *pEndX = m_mem.SP();
		byte *pStartX = (byte*)m_mem.AllocS(nCountX);
		if( !pStartX )	return m_ls.Error("Not enough p-memory"), FALSE;
		memmove(pStartX,pStart,nCountX);
		if( m_ls.Get() != CLangLex::T_COMMA )	return
					m_ls.Error("Comma expected"), FALSE;
		if( !ProcessExpr() )	return FALSE;
		if( !CastScalar(m_t.T_FLOAT) )	return FALSE;
		unsigned  nCountY = (unsigned)(m_mem.HP() - pStart);
		m_mem.ReleaseH(pStart);
		byte *pStartY = (byte*)m_mem.AllocS(nCountY);
		if( !pStartY )	return m_ls.Error("Not enough p-memory"), FALSE;
		memmove(pStartY,pStart,nCountY);
		if( m_ls.Get() != CLangLex::T_COMMA )	return
					m_ls.Error("Comma expected"), FALSE;
		if( !ProcessExpr() )	return FALSE;
		if( m_ls.Get() != CLangLex::T_RBRAK )	return
					m_ls.Error("Right square bracket expected"), FALSE;
		if( !CastScalar(m_t.T_FLOAT) )	return FALSE;
		memmove(m_mem.HP(),pStartY,nCountY);
		m_mem.ReleaseS(pStartX);
		ASSERT( m_mem.AllocH(nCountY) );
		memmove(m_mem.HP(),pStartX,nCountX);
		m_mem.ReleaseS(pEndX);
		ASSERT( m_mem.AllocH(nCountX) );
		m_t.type = m_t.T_VECTOR;
	} else
	if( m_ls.This() == CLangLex::T_ID ) {
		CLangContext::CEntry *pEntry = m_funcContext.Find(m_ls.This().Buffer());
		if(!pEntry)	pEntry = m_context.Find(m_ls.This().Buffer());
		if(!pEntry)
			return	m_ls.Error("Undefined identifier"), FALSE;
		m_t.type = pEntry->Type() & m_t.T_TYPEINFO;
		if( !(pEntry->Type() & pEntry->T_FUNC) ) {
			if( m_t.type & m_t.T_ARRAY ) m_t.hArrayDesc = pEntry->DescOffset();
			if( m_t.type & m_t.T_EXT ) {
				if( m_t.type & m_t.T_HEAP ) {
					if(!Compile(P_EPTR_H))	return FALSE;
					m_t.type &= ~m_t.T_HEAP;
				} else {
					ASSERT(m_t.type & m_t.T_STACK);
					if(!Compile(P_EPTR_S))	return FALSE;
					m_t.type &= ~m_t.T_STACK;
				}
				if(!CompileWord(pEntry->Offset()))	return FALSE;
			} else {
				if(!Compile(P_PTR))	return FALSE;
				if(!CompileWord(pEntry->Offset()))	return FALSE;
			}
		}
		else {
			if( m_ls.Get() != CLangLex::T_LPAR )
				return	m_ls.Error("Left parenthesis expected"), FALSE;
			byte *pStart = m_mem.HP();
			SArgCode *pCodePtrs = (SArgCode*)m_heap.HP(), *pCodePtr = NULL;
			word hTypePtr = pEntry->ArgTypes();
			CLangContext::SArgType *pTypePtr = (CLangContext::SArgType *)
					(hTypePtr==(word)-1 ? (void*)NULL :
										(void*)m_heap.H2P(hTypePtr) );
			if( !pTypePtr ) {
				if( m_ls.Get() != CLangLex::T_RPAR )
					return m_ls.Error("Right parentesis expected"), FALSE;
			} else
			while( pTypePtr ) {
				if( !ProcessExpr() ) return m_heap.ReleaseH(pCodePtrs), FALSE;
				if( pTypePtr->hNext != (word)-1 ) {
					if( m_ls.Get() != CLangLex::T_COMMA )
						return m_ls.Error("Comma expected"),
											m_heap.ReleaseH(pCodePtrs), FALSE;
				} else {
					if( m_ls.Get() != CLangLex::T_RPAR )
						return m_ls.Error("Right parentesis expected"),
											m_heap.ReleaseH(pCodePtrs), FALSE;
				}
				if( (m_t.Type() & m_t.T_SINGULAR) && !Deref() )
							return m_heap.ReleaseH(pCodePtrs), FALSE;
				if( m_t.Type() != (pTypePtr->type & m_t.T_TYPE) ) {
					if( (pTypePtr->type&m_t.T_SCALAR) &&
											(m_t.type&m_t.T_SCALAR) ) {
						if( !CastScalar(pTypePtr->type&m_t.T_TYPE) )
									return m_heap.ReleaseH(pCodePtrs), FALSE;
					} else
					return m_ls.Error("Incompatible argument type"),
										m_heap.ReleaseH(pCodePtrs), FALSE;
				}
				if( (pTypePtr->type & m_t.T_TYPE) == m_t.T_ARRAY ) {
					for( SArrayDesc *pDesc1 = (SArrayDesc*)
									m_heap.H2P(pTypePtr->hDesc),
								*pDesc2 = (SArrayDesc*)
									m_heap.H2P(m_t.hArrayDesc);;
													pDesc1++, pDesc2++ ) {
						if( pDesc1->type != pDesc2->type ||
									pDesc1->dim != 0 &&
											pDesc1->dim != pDesc2->dim )
							return m_ls.Error("Incompatible argument type"),
										m_heap.ReleaseH(pCodePtrs), FALSE;
						if( pDesc1->type != m_t.T_ARRAY ) break;
					}
					if(!(m_t.type&m_t.T_EXT)) {
						if( m_t.type&m_t.T_HEAP ) {
							if( !Compile(P_CAST_HPE) ) return
										m_heap.ReleaseH(pCodePtrs), FALSE;
							m_t.type &= ~m_t.T_HEAP;
						} else
						if( m_t.type&m_t.T_STACK ) {
							if( !Compile(P_CAST_SPE) ) return
										m_heap.ReleaseH(pCodePtrs), FALSE;
							m_t.type &= ~m_t.T_STACK;
						} else ASSERT(0);
					}
				}
				if( pTypePtr->hNext != (word)-1 ) {
					pCodePtr = (SArgCode*) m_heap.AllocH(sizeof(SArgCode));
					if( !pCodePtr) return m_ls.Error("Not enough memory"),
											m_heap.ReleaseH(pCodePtrs), FALSE;
					unsigned nCode = (unsigned)(m_mem.HP()-pStart);
					pCodePtr->p = m_mem.SP() - nCode;
					pCodePtr->n = (word)nCode;
					memmove(pCodePtr->p,pStart,nCode);
					m_mem.ReleaseH(pStart);
					VERIFY(m_mem.AllocS(nCode));
				} else {
					if( pCodePtr )
					while( pCodePtr >= pCodePtrs ) {
						memmove(m_mem.HP(),pCodePtr->p,pCodePtr->n);
						m_mem.ReleaseS(pCodePtr->n);
						VERIFY(m_mem.AllocH(pCodePtr->n));
						pCodePtr--;
					}
					m_heap.ReleaseH(pCodePtrs);
				}
				pTypePtr = pTypePtr->hNext == (word)-1 ?
					  (CLangContext::SArgType *) NULL :
					 (CLangContext::SArgType *) m_heap.H2P(pTypePtr->hNext);
			}
			if( pEntry->Type()&CLangContext::CEntry::T_LANGFUNC ) {
				if( !Compile(P_CALL) )	return FALSE;
			} else
			if( pEntry->Type()&CLangContext::CEntry::T_EXTFUNC ) {
				if( !Compile(P_CALLEXT) )	return FALSE;
			} else ASSERT(0);
			if( !CompileWord(pEntry->Offset())) return FALSE;
			m_t.type = pEntry->Type() & m_t.T_TYPE;
		}
	} else
	if( m_ls.This() == CLangLex::T_LPAR ) {
		if(! ProcessExpr())	return FALSE;
		if( m_ls.Get() != CLangLex::T_RPAR )
			return	m_ls.Error("Right parenthesis expected"), FALSE;
	} else
	if( m_ls.This() == CLangLex::T_NOT ) {
		m_ls.Unget();
		if( !ProcessExprComp() )	return FALSE;
	} else
	if( m_ls.This() == CLangLex::T_ATAN2 ) {
		if( m_ls.Get() != CLangLex::T_LPAR )
			return	m_ls.Error("Left parenthesis expected"), FALSE;
		if( !ProcessExpr() )	return FALSE;
		if( m_ls.Get() != CLangLex::T_COMMA )
			return	m_ls.Error("Comma expected"), FALSE;
		if( !CastScalar(m_t.T_FLOAT) )	return FALSE;
		if( !ProcessExpr() )	return FALSE;
		if( m_ls.Get() != CLangLex::T_RPAR )
			return	m_ls.Error("Right parenthesis expected"), FALSE;
		if( !CastScalar(m_t.T_FLOAT) )	return FALSE;
		if( !Compile(P_ATAN2) )	return FALSE;
	} else
	if( m_ls.This() == CLangLex::T_FLOAT || m_ls.This() == CLangLex::T_INT ||
		 m_ls.This() == CLangLex::T_ABS || m_ls.This() == CLangLex::T_SQRT ||
			m_ls.This() == CLangLex::T_SIN || m_ls.This() == CLangLex::T_COS ||
			m_ls.This() == CLangLex::T_TAN || m_ls.This() == CLangLex::T_ASIN ||
			m_ls.This() == CLangLex::T_ACOS || m_ls.This() == CLangLex::T_ATAN ||
			m_ls.This() == CLangLex::T_LOG || m_ls.This() == CLangLex::T_EXP ) {
		CLangLex::TYPE	lxt = m_ls.This().Type();
		if( m_ls.Get() != CLangLex::T_LPAR )
			return	m_ls.Error("Left parenthesis expected"), FALSE;
		if( !ProcessExpr() )	return FALSE;
		if( m_ls.Get() != CLangLex::T_RPAR )
			return	m_ls.Error("Right parenthesis expected"), FALSE;
		if( lxt == CLangLex::T_INT ) {
			if( !CastScalar(m_t.T_INT) ) return FALSE;
		} else
		if( lxt == CLangLex::T_ABS ) {
			if( !(m_t.type & m_t.T_SINGULAR) )
				return	m_ls.Error("Singular expression expected"), FALSE;
			if( !Deref() )	return FALSE;
			switch( m_t.type ) {
				case m_t.T_INT:	if( !Compile(P_ABS_I) )	return FALSE; break;
				case m_t.T_FLOAT: if( !Compile(P_ABS_F) ) return FALSE; break;
				case m_t.T_VECTOR:
					if( !Compile(P_ABS_V) ) return FALSE;
					m_t.type = m_t.T_FLOAT; break;
				default: ASSERT(0);
			}
		} else {
			if( !CastScalar(m_t.T_FLOAT) )	return FALSE;
			switch(lxt) {
				case CLangLex::T_FLOAT: break;
				case CLangLex::T_SQRT: if( !Compile(P_SQRT) ) return FALSE; break;
				case CLangLex::T_SIN: if( !Compile(P_SIN) ) return FALSE; break;
				case CLangLex::T_COS: if( !Compile(P_COS) ) return FALSE; break;
				case CLangLex::T_TAN: if( !Compile(P_TAN) ) return FALSE; break;
				case CLangLex::T_ASIN: if( !Compile(P_ASIN) ) return FALSE; break;
				case CLangLex::T_ACOS: if( !Compile(P_ACOS) ) return FALSE; break;
				case CLangLex::T_ATAN: if( !Compile(P_ATAN) ) return FALSE; break;
				case CLangLex::T_LOG: if( !Compile(P_LOG) ) return FALSE; break;
				case CLangLex::T_EXP: if( !Compile(P_EXP) ) return FALSE; break;
				default: ASSERT(0);	// How come we did get here???
			}
		} // if not ABS
	} else
		return	m_ls.Error("Unexpected symbol"), FALSE;

	for(;;) {
		if( m_ls.Get() == CLangLex::T_PERIOD ) {
			ASSERT(m_t.Type()!=m_t.T_ERROR);
			if( m_t.Type()!=m_t.T_VECTOR )
				return m_ls.Error("Dot operator applies to vectors only"),
																FALSE;
			if( m_ls.Get() != CLangLex::T_ID )
				return m_ls.Error("Component name expected"), FALSE;
			if( m_ls.This() == "x" ) {
				if(!ApplyPeriod(P_DOT_X))	return FALSE;
			} else
			if( m_ls.This() == "y" ) {
				if(!ApplyPeriod(P_DOT_Y))	return FALSE;
			} else
			if( m_ls.This() == "z" ) {
				if(!ApplyPeriod(P_DOT_Z))	return FALSE;
			} else
				return m_ls.Error("Component name expected"), FALSE;
		} else
		if( m_ls.This() == CLangLex::T_DBLSTAR ) {
			if( !(m_t.Type() & m_t.T_SCALAR) )
				return m_ls.Error("Exp operator applies to scalar types only"),
																FALSE;
			if(!Deref()) return FALSE;
			CType	t = m_t;
			if(! ProcessExprUna())	return FALSE;
			if( !(m_t.Type() & m_t.T_SCALAR) )
				return m_ls.Error("Exp operator applies to scalar types only"),
																FALSE;
			if(!Deref()) return FALSE;
			if(! ApplyBinaryScalar(t,P_POW_II) )	return FALSE;
			m_t.type = m_t.T_FLOAT;
			break;	// There can be no more
		} else
		if( m_ls.This() == CLangLex::T_LBRAK ) {
			if( m_t.Type() != m_t.T_ARRAY )	return
				m_ls.Error("Subscript on non-array expression"), FALSE;
			CType t = m_t;
			if( !ProcessExpr() )	return FALSE;
			if( m_ls.Get() != CLangLex::T_RBRAK )	return
					m_ls.Error("Right square bracket expected"), FALSE;
			if( !CastScalar(m_t.T_INT) )	return FALSE;
			SArrayDesc	*pDesc = (SArrayDesc*)m_heap.H2P(t.hArrayDesc);
			if( !Compile(P_INT) ||
						!CompileInt(pDesc->size) ||
							!Compile(P_MUL_II)) return FALSE;
			if( !Compile(m_t.type&m_t.T_EXT?P_ADD_EPI:P_ADD_PI) ) return FALSE;
			m_t = t;
			if( pDesc->type != t.T_ARRAY ) {
				m_t.type &= ~m_t.T_TYPE;
				m_t.type |= pDesc->type;
			} else
				m_t.hArrayDesc += sizeof(SArrayDesc);
		} else // if LBRAK
		if( m_ls.This() == CLangLex::T_TILDE )  {
			if( !CastScalar(m_t.T_FLOAT) )	return FALSE;
			if( !Compile(P_CAST_DEGRAD) )	return FALSE;
		} else
		{
			m_ls.Unget(); break;
		}
	}

	while( nPrefixTildes-- ) {
		if( !CastScalar(m_t.T_FLOAT) )	return FALSE;
		if( !Compile(P_CAST_RADDEG) )	return FALSE;
	}
	if( ! bNegates )	return TRUE;
	if( ! (m_t.Type() & m_t.T_SINGULAR ) )	return
		m_ls.Error("Negation on non-singular expression"), FALSE;
	if( ! Deref() )	return FALSE;
	if( bNegate )
	switch( m_t.Type() ) {
		case m_t.T_INT:	return Compile(P_NEG_I);
		case m_t.T_FLOAT:	return Compile(P_NEG_F);
		case m_t.T_VECTOR:	return Compile(P_NEG_V);
		default: ASSERT(0); return FALSE;
	}
	return TRUE;
}
// ------------------------------------------------------------------------
bool CLangExprCompiler::ProcessExprProd()
{
	if(! ProcessExprUna())	return FALSE;
	CLangLex::TYPE oper;
	while( (oper=m_ls.Get().Type()) == CLangLex::T_STAR ||
				oper == CLangLex::T_SLASH ||
					oper == CLangLex::T_PERC || oper == CLangLex::T_MOD ) {
		if( !(m_t.Type() & m_t.T_SINGULAR) )
			return m_ls.Error("Types incompatible with operation"), FALSE;
		if( !Deref() )	return FALSE;
		if( oper == CLangLex::T_MOD ) {
			if( !CastScalar(m_t.T_INT) ) return FALSE;
		}
		CType	t = m_t;
		if(! ProcessExprUna())	return FALSE;
		if( !(m_t.Type() & m_t.T_SINGULAR) )
			return m_ls.Error("Types incompatible with operation"), FALSE;
		if( !Deref() )	return FALSE;
		if( t.Type() == t.T_VECTOR ) {
			if( m_t.Type() == m_t.T_VECTOR ) {
				if( oper == CLangLex::T_STAR ) {
					if(!Compile(P_MUL_VV))	return FALSE;
					m_t.type = m_t.T_FLOAT;
				} else
				if( oper == CLangLex::T_PERC ) {
					if(!Compile(P_MUL_VVC))	return FALSE;
				} else
					return m_ls.Error("Types incompatible with operation"),
															FALSE;
			} else
			if( oper == CLangLex::T_STAR ) {
				if(m_t.type == m_t.T_FLOAT) {
					if(!Compile(P_MUL_VF))	return FALSE;
					m_t.type = m_t.T_VECTOR;
				} else {
					ASSERT(m_t.type == m_t.T_INT);
					if(!Compile(P_MUL_VI))	return FALSE;
					m_t.type = m_t.T_VECTOR;
				}
			} else
			if( oper == CLangLex::T_SLASH ) {
				if(m_t.type == m_t.T_FLOAT) {
					if(!Compile(P_DIV_VF))	return FALSE;
					m_t.type = m_t.T_VECTOR;
				} else {
					ASSERT(m_t.type == m_t.T_INT);
					if(!Compile(P_DIV_VI))	return FALSE;
					m_t.type = m_t.T_VECTOR;
				}
			} else
				return m_ls.Error("Types incompatible with operation"),
															FALSE;
		} else	// t.type != t.T_VECTOR
		if( m_t.Type() == m_t.T_VECTOR ) {
			if( oper == CLangLex::T_STAR ) {
				if(t.type == t.T_FLOAT) {
					if(!Compile(P_MUL_FV))	return FALSE;
				} else {
					ASSERT(t.type == t.T_INT);
					if(!Compile(P_MUL_IV))	return FALSE;
				}
			} else
				return m_ls.Error("Types incompatible with operation"),
															FALSE;
		} else
		if( oper == CLangLex::T_MOD ) {
			if( !CastScalar(m_t.T_INT) )	return FALSE;
			if(!Compile(P_MOD_II))	return FALSE;
		} else {
			if( oper == CLangLex::T_STAR ) {
				if(!ApplyBinaryScalar(t,P_MUL_II))	return FALSE;
			} else
			if( oper == CLangLex::T_SLASH ) {
				if(!ApplyBinaryScalar(t,P_DIV_II))	return FALSE;
			} else
				return m_ls.Error("Types incompatible with operation"), FALSE;
		}
	}
	m_ls.Unget();
	return TRUE;
}
// ------------------------------------------------------------------------
bool CLangExprCompiler::ProcessExprSum()
{
	if( !ProcessExprProd() ) return FALSE;
	CLangLex::TYPE oper;
	while( (oper=m_ls.Get().Type()) == CLangLex::T_PLUS ||
											oper == CLangLex::T_MINUS ) {
		if( !(m_t.Type() & m_t.T_SINGULAR) )
			return m_ls.Error("Types incompatible with operation"), FALSE;
		if( !Deref() )	return FALSE;
		CType	t = m_t;
		if( !ProcessExprProd() ) return FALSE;
		if( !(m_t.Type() & m_t.T_SINGULAR) )
			return m_ls.Error("Types incompatible with operation"), FALSE;
		if( !Deref() )	return FALSE;
		if( t.Type() == t.T_VECTOR ) {
			if( m_t.Type() != m_t.T_VECTOR )
				return m_ls.Error("Incompatible types"), FALSE;
			if( !Compile(oper==CLangLex::T_PLUS?P_ADD_VV:P_SUB_VV) )
																return FALSE;
		} else
		if( m_t.Type() == m_t.T_VECTOR ) {
			return m_ls.Error("Incompatible types"), FALSE;
		} else {
			if( !ApplyBinaryScalar(t,
					oper==CLangLex::T_PLUS?P_ADD_II:P_SUB_II) ) return FALSE;
		}
	}
	m_ls.Unget();
	return TRUE;
}
// ------------------------------------------------------------------------
bool CLangExprCompiler::ProcessExprComp()
{
	bool bNot = FALSE, bNots = FALSE;
	while( m_ls.Get() == CLangLex::T_NOT )	bNot = !bNot, bNots = TRUE;
	m_ls.Unget();
	if( !ProcessExprSum() )	return FALSE;
	CLangLex::TYPE oper;
	while( (oper=m_ls.Get().Type()) == CLangLex::T_EQ ||
					oper == CLangLex::T_NEQ ||
					oper == CLangLex::T_GT || oper == CLangLex::T_LT ||
					oper == CLangLex::T_GE || oper == CLangLex::T_LE ) {
		if( !(m_t.type & m_t.T_SCALAR) && (m_t.Type() != m_t.T_VECTOR ||
								oper != CLangLex::T_EQ && oper != CLangLex::T_NEQ) )
			return m_ls.Error("Type incompatible with operation"), FALSE;
		if( !Deref() )	return FALSE;
		CType	t = m_t;
		if( !ProcessExprSum() )	return FALSE;
		if( !(m_t.type & m_t.T_SINGULAR) )
			return m_ls.Error("Type incompatible with operation"), FALSE;
		if( !Deref() )	return FALSE;
		if( t.Type() == m_t.T_VECTOR ) {
			if( m_t.Type() != m_t.T_VECTOR )
				return m_ls.Error("Incompatible types"), FALSE;
			switch(oper) {
				case CLangLex::T_EQ: if(!Compile(P_EQ_VV)) return FALSE; break;
				case CLangLex::T_NEQ: if(!Compile(P_NE_VV)) return FALSE; break;
				default: ASSERT(0);
			}
			m_t.type = m_t.T_INT;
		} else
		if( !(t.type & t.T_SCALAR) || !(m_t.type & m_t.T_SCALAR) )
			return m_ls.Error("Incompatible types"), FALSE;
		else
		switch(oper) {
			case CLangLex::T_EQ: if(!ApplyComparison(t,P_EQ_II)) return FALSE; break;
			case CLangLex::T_NEQ: if(!ApplyComparison(t,P_NE_II)) return FALSE; break;
			case CLangLex::T_GT: if(!ApplyComparison(t,P_GT_II)) return FALSE; break;
			case CLangLex::T_LT: if(!ApplyComparison(t,P_LT_II)) return FALSE; break;
			case CLangLex::T_GE: if(!ApplyComparison(t,P_GE_II)) return FALSE; break;
			case CLangLex::T_LE: if(!ApplyComparison(t,P_LE_II)) return FALSE; break;
			default: ASSERT(0);
		}
	}
	m_ls.Unget();
	if( bNots )	{
		if( !(m_t.Type() & m_t.T_SCALAR) )
			return m_ls.Error("Types incompatible with operation"), FALSE;
		if( !Deref() )	return FALSE;
		if( m_t.type == m_t.T_FLOAT ) {
			if( ! Compile(P_NOT_F) )	return FALSE;
			m_t.type = m_t.T_INT;
		} else
			if( ! Compile(P_NOT_I) )	return FALSE;
		if( ! bNot )
			return	Compile(P_NOT_I);
	}
	return TRUE;
}
// ------------------------------------------------------------------------
bool CLangExprCompiler::ProcessExprAnd()
{
	if( !ProcessExprComp() )	return FALSE;
	while( m_ls.Get() == CLangLex::T_AND ) {
		if( !(m_t.Type() & m_t.T_SCALAR) )
			return m_ls.Error("Types incompatible with operation"), FALSE;
		if( ! Deref() )	return FALSE;
		if( ! Compile(m_t.type == m_t.T_FLOAT ? P_JZ_F : P_JZ_I) ) return FALSE;
		if( ! CompileWord(0) ) return FALSE;
		unsigned  nAnd1Jump = m_mem.HN();
		if( ! ProcessExprComp() )	return FALSE;
		if( !(m_t.Type() & m_t.T_SCALAR) )
			return m_ls.Error("Types incompatible with operation"), FALSE;
		if( ! Deref() )	return FALSE;
		if( ! Compile(m_t.type == m_t.T_FLOAT ? P_JZ_F : P_JZ_I) ) return FALSE;
		if( ! CompileWord(0) ) return FALSE;
		unsigned  nAnd2Jump = m_mem.HN();
		if( ! Compile(P_INT) || ! CompileInt(1) ) 	return FALSE;
		if( ! Compile(P_JMP) ) return FALSE;
		if( ! CompileWord(0) ) return FALSE;
		*(iword*)m_mem.H2P(nAnd1Jump-sizeof(iword)) = m_mem.HN()-nAnd1Jump;
		*(iword*)m_mem.H2P(nAnd2Jump-sizeof(iword)) = m_mem.HN()-nAnd2Jump;
		nAnd1Jump = m_mem.HN();
		if( ! Compile(P_INT) || ! CompileInt(0) ) 	return FALSE;
		*(iword*)m_mem.H2P(nAnd1Jump-sizeof(iword)) = m_mem.HN()-nAnd1Jump;
		m_t.type = m_t.T_INT;
	}
	m_ls.Unget();
	return TRUE;
}
// ------------------------------------------------------------------------
bool CLangExprCompiler::ProcessExprOr()
{
	if( !ProcessExprAnd() )	return FALSE;
	while( m_ls.Get() == CLangLex::T_OR ) {
		if( !(m_t.Type() & m_t.T_SCALAR) )
			return m_ls.Error("Types incompatible with operation"), FALSE;
		if( ! Deref() )	return FALSE;
		if( ! Compile(m_t.type == m_t.T_FLOAT ? P_JNZ_F : P_JNZ_I) ) return FALSE;
		if( ! CompileWord(0) ) return FALSE;
		unsigned  nOr1Jump = m_mem.HN();
		if( ! ProcessExprAnd() )	return FALSE;
		if( !(m_t.Type() & m_t.T_SCALAR) )
			return m_ls.Error("Types incompatible with operation"), FALSE;
		if( ! Deref() )	return FALSE;
		if( ! Compile(m_t.type == m_t.T_FLOAT ? P_JNZ_F : P_JNZ_I) ) return FALSE;
		if( ! CompileWord(0) ) return FALSE;
		unsigned  nOr2Jump = m_mem.HN();
		if( ! Compile(P_INT) || ! CompileInt(0) ) 	return FALSE;
		if( ! Compile(P_JMP) ) return FALSE;
		if( ! CompileWord(0) ) return FALSE;
		*(iword*)m_mem.H2P(nOr1Jump-sizeof(iword)) = m_mem.HN()-nOr1Jump;
		*(iword*)m_mem.H2P(nOr2Jump-sizeof(iword)) = m_mem.HN()-nOr2Jump;
		nOr1Jump = m_mem.HN();
		if( ! Compile(P_INT) || ! CompileInt(1) ) 	return FALSE;
		*(iword*)m_mem.H2P(nOr1Jump-sizeof(iword)) = m_mem.HN()-nOr1Jump;
		m_t.type = m_t.T_INT;
	}
	m_ls.Unget();
	return TRUE;
}
// ------------------------------------------------------------------------
CLangValue CLangExprCompiler::Eval(TCchar *lpText,unsigned flags)
{
	ASSERT(m_mem.StackUsed()==0);
	CMemoryCharStream cs(m_PrintF);
	if( !cs.Open(lpText,CTextPos(flags&F_MULTILINE?1:0,1)) ||
									!m_ls.Open(&cs) )
		return m_PrintF("Input stream error\n"), CLangValue();
	TCvoid		*pStart = m_mem.HP();
	if( ! ProcessExpr() || ! Deref() || !Compile(P_HALT) )
		return m_ls.Close(), m_mem.ReleaseH(pStart), ClearStack(),
															CLangValue();
	#ifdef	_DISPPCODE
		m_PrintF("\n");
	#endif
	ASSERT(m_mem.StackUsed()==0);
	if( m_ls.Get() != CLangLex::T_EOF )
		return	m_ls.Error("Unexpected symbol"), m_ls.Close(),
									m_mem.ReleaseH(pStart), CLangValue();
	if( !m_ls.Close() || !cs.Close() )
		return m_PrintF("Input stream error\n"),
										m_mem.ReleaseH(pStart), CLangValue();
	if(! Go(m_mem.P2H(pStart)) )
				return ClearStack(), m_mem.ReleaseH(pStart), CLangValue();
	CLangValue v;
	switch(m_t.type) {
		case m_t.T_INT:
			ASSERT(m_mem.StackUsed()==sizeof(long));
			v = CLangValue(*(long*)m_mem.SP());
			m_mem.ReleaseS(sizeof(long));
			break;
		case m_t.T_FLOAT:
			ASSERT(m_mem.StackUsed()==sizeof(double));
			v = CLangValue(*(double*)m_mem.SP());
			m_mem.ReleaseS(sizeof(double));
			break;
		case m_t.T_VECTOR:
			ASSERT(m_mem.StackUsed()==sizeof(SFVector3));
			v = CLangValue(*(SFVector3*)m_mem.SP());
			m_mem.ReleaseS(sizeof(SFVector3));
			break;
		case m_t.T_VOID:
			v = CLangValue(v.T_VOID);
			break;
		default:
			return m_PrintF("Incompatible eval type\n"),
										m_mem.ReleaseH(pStart), CLangValue();
	}
	ASSERT(m_mem.StackUsed()==0);
	m_mem.ReleaseH(pStart);
	return	v;
}
// ------------------------------------------------------------------------
unsigned CLangExprCompiler::VarOffset(TCchar *name) const
{
	ASSERT(m_funcContext.IsEmpty());
	CLangContext::CEntry *pEntry = m_context.Find(name);
	if( !pEntry )	return
		m_PrintF("Error in CLangFuncContext::VarOffset: Var '%s' not found",
								name), (unsigned)-1;
	if( pEntry->Type() & CLangContext::CEntry::T_FUNC )
		m_PrintF("Error in CLangFuncContext::VarOffset: '%s' is a function",
								name), (unsigned)-1;
	if( pEntry->Type() & CType::T_EXT )
		m_PrintF("Error in CLangFuncContext::VarOffset: '%s' is external",
								name), (unsigned)-1;
	return pEntry->Offset();
}
// ------------------------------------------------------------------------
unsigned CLangExprCompiler::FuncOffset(TCchar *name) const
{
	ASSERT(m_funcContext.IsEmpty());
	CLangContext::CEntry *pEntry = m_context.Find(name);
	if( !pEntry )	return
		m_PrintF("Error in CLangFuncContext::FuncOffset: Function '%s' not found",
								name), (unsigned)-1;
	if( !(pEntry->Type() & CLangContext::CEntry::T_FUNC) )
		m_PrintF("Error in CLangFuncContext::FuncOffset: '%s' is not a function",
								name), (unsigned)-1;
	if( pEntry->Type() & CLangContext::CEntry::T_EXTFUNC )
		m_PrintF("Error in CLangFuncContext::FuncOffset: '%s' is external",
								name), (unsigned)-1;
	return pEntry->Offset();
}
// ------------------------------------------------------------------------
