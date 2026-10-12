
#ifndef	__LANG_CPP__
	#include<stdio.h>
	#include<dos.h>
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

// ------------------------------------------------------------------------
CLangContext::CEntry *CLangContext::AddEntry(TCchar *name,int type)
{
	CEntry	*pEntry;
	if( type&CEntry::T_FUNC )
		pEntry = (CEntry*)(CFuncEntry*) m_pHeap->AllocH(sizeof(CFuncEntry));
	else
		pEntry = (CEntry*) m_pHeap->AllocH(sizeof(CEntry));
	if(!pEntry)	return NULL;
	char	*pName = (char*) m_pHeap->AllocH(strlen(name)+1);
	if(!pName)	return NULL;
	strcpy(pName,name);
	pEntry->m_hName = m_pHeap->P2H(pName);
	pEntry->m_type = type;
	pEntry->m_hNext = m_hLastEntry;
	m_hLastEntry = m_pHeap->P2H(pEntry);
	return pEntry;
}
// ------------------------------------------------------------------------
bool CLangContext::Add(TCchar *name,int type,word descr,word offset)
{
	CEntry	*pEntry = AddEntry(name,type);
	pEntry->m_p.a = offset;
	pEntry->m_p.d = descr;
	return TRUE;
}
// ------------------------------------------------------------------------
bool CLangContext::AddArg(TCchar *name,int type,word descr,word offset)
{
	CEntry	*pEntry = (CEntry*) m_pHeap->AllocS(sizeof(CEntry));
	if(!pEntry)	return NULL;
	char	*pName = (char*) m_pHeap->AllocS(strlen(name)+1);
	if(!pName)	return NULL;
	strcpy(pName,name);
	pEntry->m_hName = m_pHeap->P2H(pName);
	pEntry->m_type = type;
	pEntry->m_hNext = m_hLastEntry;
	m_hLastEntry = m_pHeap->P2H(pEntry);
	pEntry->m_p.a = offset;
	pEntry->m_p.d = descr;
	return TRUE;
}
// ------------------------------------------------------------------------
void CLangContext::SetLastArgTypes(word h)
{
	ASSERT(m_hLastEntry!=(word)-1);
	CFuncEntry *pEntry = (CFuncEntry*)(CEntry*)m_pHeap->H2P(m_hLastEntry);
	pEntry->m_hArgTypes = h;
}
// ------------------------------------------------------------------------
void CLangContext::DeleteLast()
{
	ASSERT(m_hLastEntry!=(word)-1);
	CEntry *pEntry = (CEntry*)m_pHeap->H2P(m_hLastEntry);
	m_hLastEntry = pEntry->m_hNext;
	m_pHeap->ReleaseH(pEntry);
}
// ------------------------------------------------------------------------
#ifdef	_DISPPCODE
	#define	Compile(C) (m_PrintF("\n%04Xh: %s",m_mem.HN(),#C),Compile(C))
#endif
bool CLangCompiler::ProcessType(unsigned ptflags)
{
	CLangLex::TYPE	lxt = m_ls.Get().Type();
	if( lxt == CLangLex::T_INT )	m_t.type = m_t.T_INT; else
	if( lxt == CLangLex::T_FLOAT )	m_t.type = m_t.T_FLOAT; else
	if( lxt == CLangLex::T_VECTOR )	m_t.type = m_t.T_VECTOR; else
	if( lxt == CLangLex::T_VOID )	m_t.type = m_t.T_VOID; else
					return	m_ls.Error("Type name expected"), FALSE;
	if(m_t.type == m_t.T_VOID && !(ptflags & PTF_ALLOWVOID))
			return	m_ls.Error("VOID type is not allowed here"), FALSE;
	CLangExprCompiler::SArrayDesc	*pArrayDesc = NULL,	*pDesc = NULL;
	for(;;) {
		if( m_ls.Get() != CLangLex::T_LBRAK ) { m_ls.Unget(); break; }
		if( !(ptflags & PTF_ARRAYS) )
			return	m_ls.Error("Arrays are not allowed here"), FALSE;
		if( m_t.type == m_t.T_VOID )
			return m_ls.Error("Arrays of VOID type are not allowed"), FALSE;
		long lDim = 0;
		m_ls.Get();
		if( !(ptflags & PTF_DUMMYDIMS) || pArrayDesc ||
								m_ls.This() != CLangLex::T_RBRAK ) {
			if( m_ls.This() != CLangLex::T_INTV )
				return	m_ls.Error("Integer constant expected"), FALSE;
			lDim = (long)m_ls.This();
			if( lDim <= 0 ) return
				m_ls.Error("Bad array dimension"), FALSE;
			m_ls.Get();
		}
		if( m_ls.This() != CLangLex::T_RBRAK ) return
			m_ls.Error("Right square bracket expected"), FALSE;
		if( lDim > 64000L )
				return m_ls.Error("Array too big"), FALSE;
		pDesc = (SArrayDesc*) m_heap.AllocH(sizeof(SArrayDesc));
		if( ! pDesc ) return m_ls.Error("Not enough memory"), FALSE;
		pDesc->dim = (int)lDim;
		if( ! pArrayDesc )	pArrayDesc = pDesc;
	}
	if( pArrayDesc ) {
		pDesc->type = m_t.type;
		switch(m_t.type) {
			case m_t.T_INT:	pDesc->size=sizeof(long); break;
			case m_t.T_FLOAT:	pDesc->size=sizeof(double); break;
			case m_t.T_VECTOR: pDesc->size=sizeof(CFVector3); break;
			default:	ASSERT(0);
		}
		long lArrayDim = pDesc->size;
		for(;;) {
			if( (lArrayDim*=pDesc->dim) > 64000L )
						return m_ls.Error("Array too big"), FALSE;
			if( pDesc-- <= pArrayDesc ) break;
			pDesc->type = m_t.T_ARRAY;
			pDesc->size = (unsigned)(lArrayDim);
		}
		m_t.type = m_t.T_ARRAY;
		m_t.hArrayDesc = m_heap.P2H(pArrayDesc);
	}
	return TRUE;
}
// ------------------------------------------------------------------------
bool CLangCompiler::ProcessFunctionDecl(unsigned functype,
								bool (*pFunc)(CLangProcessor &lp))
{
	ASSERT(m_funcContext.IsEmpty());
	if( ! ProcessType(PTF_ALLOWVOID) )
		return FALSE;
	if( m_ls.Get() != CLangLex::T_ID )
		return	m_ls.Error("Identifier expected"), FALSE;

	bool bFound = m_context.Find(m_ls.This().Buffer()) != NULL;
	if(!bFound)	{
		if( !m_context.Add(m_ls.This().Buffer(),
							m_t.type|functype,
							m_t.hArrayDesc,m_mem.HN()) )
					return m_ls.Error("Not enough memory"), FALSE;
	}
	if( m_ls.Get() != CLangLex::T_LPAR ) return
			m_ls.Error("Left parenthesis expected"),
					(bFound?(void)0:m_context.DeleteLast()), FALSE;
	if( bFound )  return m_ls.Error("Identifier already defined"), FALSE;
	int  nStackOffset = 2*sizeof(word);
	TCvoid *pArgsBegin = m_heap.SP();
	CLangContext::SArgType *pArgType = NULL;
	TCvoid *pFuncContextStart = m_heap.HP();
	word hArgTypes = -1;
	m_retType = m_t;
	if( m_ls.Get() != CLangLex::T_RPAR && (m_ls.Unget(),TRUE) )
	for(;;) {
		if( !ProcessType(PTF_ARRAYS|PTF_DUMMYDIMS) )
				return m_retType = CType(),
					m_heap.ReleaseS(pArgsBegin), m_funcContext.Clear(),
											m_context.DeleteLast(), FALSE;
		if( m_t.Type() == m_t.T_ARRAY ) {
			byte *pSrc = m_heap.H2P(m_t.hArrayDesc);
			unsigned nTypeSize = (unsigned)(m_heap.HP()-pSrc);
			byte *pDest = m_heap.SP()-nTypeSize;
			memmove(pDest,pSrc,nTypeSize);
			m_heap.ReleaseH(pSrc);
			VERIFY(m_heap.AllocS(nTypeSize));
			m_t.hArrayDesc = m_heap.P2H(pDest);
		}
		for(;;) {
			if( m_ls.Get() != CLangLex::T_ID ) return
					m_ls.Error("Identifier expected"), m_retType = CType(),
						m_heap.ReleaseS(pArgsBegin), m_funcContext.Clear(),
										m_context.DeleteLast(), FALSE;
			bool bFound = m_funcContext.Find(m_ls.This().Buffer()) != NULL;
			if( !bFound ) {
				if( m_t.type&m_t.T_ARRAY)	m_t.type |= m_t.T_EXT;
				m_t.type |= m_t.T_STACK;
				if( !m_funcContext.Add(m_ls.This().Buffer(),
								m_t.type|m_t.T_STACK,
										m_t.hArrayDesc,nStackOffset) )
					return m_ls.Error("Not enough memory"), m_retType = CType(),
						m_heap.ReleaseS(pArgsBegin), m_funcContext.Clear(),
											m_context.DeleteLast(), FALSE;
				nStackOffset += m_t.SizeArg();
				CLangContext::SArgType *pType =
							(CLangContext::SArgType*) m_heap.AllocS(
											sizeof(CLangContext::SArgType));
				if(!pType) return m_ls.Error("Not enough memory"),
						m_retType = CType(), m_heap.ReleaseS(pArgsBegin),
						m_funcContext.Clear(), m_context.DeleteLast(), FALSE;
				pType->type = m_t.type;
				pType->hDesc = m_t.hArrayDesc;
				pType->hNext = -1;
				if( pArgType )
					pArgType->hNext = m_heap.P2H(pType);
				else
					hArgTypes = m_heap.P2H(pType);
				pArgType = pType;
			}
			if( m_ls.Get() != CLangLex::T_COMMA &&
					m_ls.This() != CLangLex::T_SEMICOLON &&
						m_ls.This() != CLangLex::T_RPAR )	return
							m_ls.Error("Comma or semicolon or right parenthesis expected"),
							m_retType = CType(), m_heap.ReleaseS(pArgsBegin),
							m_funcContext.Clear(), m_context.DeleteLast(), FALSE;
			if( bFound ) return m_ls.Error("Identifier already defined"),
							m_retType = CType(), m_heap.ReleaseS(pArgsBegin),
							m_funcContext.Clear(), m_context.DeleteLast(), FALSE;
			if( m_ls.This() != CLangLex::T_COMMA )	break;
		}
		if( m_ls.This() == CLangLex::T_RPAR ) break;
	}
	m_nArgsSize = nStackOffset-4;
	m_context.SetLastArgTypes(hArgTypes);
	if(functype&CLangContext::CEntry::T_LANGFUNC) {
		nStackOffset = 0;
		while( m_ls.Get() == CLangLex::T_VAR ) {
			if( !ProcessType(PTF_ARRAYS) )	return m_retType = CType(),
						m_heap.ReleaseS(pArgsBegin), m_funcContext.Clear(),
											m_context.DeleteLast(), FALSE;
			for(;;) {
				if( m_ls.Get() != CLangLex::T_ID ) return
						m_ls.Error("Identifier expected"), m_retType = CType(),
							m_heap.ReleaseS(pArgsBegin), m_funcContext.Clear(),
											m_context.DeleteLast(), FALSE;
				bool bFound = m_funcContext.Find(m_ls.This().Buffer()) != NULL;
				if(!bFound)	{
					nStackOffset -= m_t.Size(m_heap);
					if( !m_funcContext.Add(m_ls.This().Buffer(),
							m_t.type | m_t.T_STACK, m_t.hArrayDesc,
															nStackOffset) )
							return m_ls.Error("Not enough memory"),
								m_retType = CType(), m_heap.ReleaseS(pArgsBegin),
										m_funcContext.Clear(),
											m_context.DeleteLast(), FALSE;
				}
				if( m_ls.Get() != CLangLex::T_COMMA &&
										m_ls.This() != CLangLex::T_SEMICOLON )
					return m_ls.Error("Comma or semicolon expected"),
						m_retType = CType(), m_heap.ReleaseS(pArgsBegin),
						m_funcContext.Clear(), m_context.DeleteLast(), FALSE;
				if(bFound)
					return	m_ls.Error("Identifier already defined"),
						m_retType = CType(), m_heap.ReleaseS(pArgsBegin),
						m_funcContext.Clear(), m_context.DeleteLast(), FALSE;
				if( m_ls.This() == CLangLex::T_SEMICOLON ) break;
			}
		}
		if( m_ls.This() != CLangLex::T_LBRACE ) return
			m_ls.Error("Compound operator expected"), m_retType = CType(),
						m_heap.ReleaseS(pArgsBegin), m_funcContext.Clear(),
											m_context.DeleteLast(), FALSE;
		m_ls.Unget();
		if( ! Compile(P_ENTER) ||
			! CompileWord(-nStackOffset) ||
			 ! ProcessScopedStatement() )
			  return m_retType = CType(),
						m_heap.ReleaseS(pArgsBegin), m_funcContext.Clear(),
											m_context.DeleteLast(), FALSE;
		if( m_retType.Type() != m_retType.T_VOID ) {
			if( ! Compile(P_NORET) )
				  return m_retType = CType(),
						m_heap.ReleaseS(pArgsBegin), m_funcContext.Clear(),
											m_context.DeleteLast(), FALSE;
		} else
		if(	 ! Compile(P_LEAVE) ||
			 ! CompileWord(m_nArgsSize) ||
				! CompileWord(0)  )
			  return m_retType = CType(),
						m_heap.ReleaseS(pArgsBegin), m_funcContext.Clear(),
											m_context.DeleteLast(), FALSE;
	} else
	if( functype&CLangContext::CEntry::T_EXTFUNC ) {
		bool (**ppFunc)(CLangProcessor &lp) =
			(TExtFuncPtr*) m_mem.AllocH(sizeof(pFunc));
		if( !ppFunc )	return m_ls.Error("Not enough p-memory"),
				m_retType = CType(),
						m_heap.ReleaseS(pArgsBegin), m_funcContext.Clear(),
											m_context.DeleteLast(), FALSE;
		*ppFunc = pFunc;
	}
	m_retType = CType();
	m_heap.ReleaseH(pFuncContextStart);
	m_funcContext.Clear();
	return TRUE;
}
// ------------------------------------------------------------------------
bool CLangCompiler::ProcessOuterStatement()
{
	ASSERT(m_retType.Type() == m_retType.T_ERROR);
	if( m_ls.Get() == CLangLex::T_VAR ) {
		if( !ProcessType(PTF_ARRAYS) )	return FALSE;
		for(;;) {
			if( m_ls.Get() != CLangLex::T_ID )
					return	m_ls.Error("Identifier expected"), FALSE;
			bool bFound = m_context.Find(m_ls.This().Buffer()) != NULL;
			byte	*pData = NULL;
			if(!bFound)	{
				pData = (byte*) m_mem.AllocH(m_t.Size(m_heap));
				if(!pData)	return m_ls.Error("Not enough p-memory"), FALSE;
				if( !m_context.Add(m_ls.This().Buffer(),
							m_t.type | m_t.T_HEAP, m_t.hArrayDesc,
													m_mem.P2H(pData))  )
						return m_ls.Error("Not enough memory"), FALSE;
			}
			if( m_ls.Get() != CLangLex::T_COMMA &&
										m_ls.This() != CLangLex::T_SEMICOLON )
				return	(bFound ? (void)0:m_context.DeleteLast()),
							(pData ? m_mem.ReleaseH(pData) : (void)0),
							m_ls.Error("Comma or semicolon expected"), FALSE;
			if(bFound)
				return	m_ls.Error("Identifier already defined"), FALSE;
			if( m_ls.This() == CLangLex::T_SEMICOLON ) break;
		}
	} /* VAR */ else
	if( m_ls.This() == CLangLex::T_FUNC ) {
		if( !ProcessFunctionDecl(CLangContext::CEntry::T_LANGFUNC) )
			return FALSE;
	} /* FUNC */ else
	if( m_ls.This() == CLangLex::T_RUN ) {
		FILE *f = NULL;
		if( m_ls.Get() == CLangLex::T_STRINGV ) {
			f = fopen(m_ls.This().Buffer(),"rt");
		} else
			return	m_ls.Error("String expected"), FALSE;
		if( m_ls.Get() != CLangLex::T_SEMICOLON )
			return	(f?fclose(f):0), m_ls.Error("Semicolon expected"), FALSE;
		if(!f)	return m_ls.Error("File not found"), FALSE;
		CCharStream *pcs = m_ls.Suspend();
		CFileCharStream cs(m_PrintF);
		cs.Open(f,CTextPos(1,1));
		m_ls.Open(&cs);
		for(;;) {
			ASSERT(m_mem.StackUsed()==0);
			if(!ProcessOuterStatement())	return
						m_ls.Close(), cs.Close(), m_ls.Open(pcs), FALSE;
			ASSERT(m_mem.StackUsed()==0);
			if(m_ls.Get()==CLangLex::T_EOF) break;
			m_ls.Unget();
		}
		m_ls.Close();
		cs.Close();
		m_ls.Open(pcs);
	} /* RUN */ else
	{
		m_ls.Unget();
		TCvoid *pStart = m_mem.HP();
		if(!ProcessScopedStatement()) return FALSE;
		if(!Compile(P_HALT)) return m_mem.ReleaseH(pStart), FALSE;
		#ifdef	_DISPPCODE
			m_PrintF("\n");
		#endif
		ASSERT(m_mem.StackUsed()==0);
		if(!Go(m_mem.P2H(pStart))) return ClearStack(),
										m_mem.ReleaseH(pStart), FALSE;
		ASSERT(m_mem.StackUsed()==0);
		m_mem.ReleaseH(pStart);
	}
	return TRUE;
}
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
bool CLangCompiler::Process(TCchar *lpText,unsigned flags)
{
	CMemoryCharStream cs(m_PrintF);
	if( !cs.Open(lpText,CTextPos(flags&F_MULTILINE?1:0,1)) ||
									!m_ls.Open(&cs) )
		return m_PrintF("Input stream error\n"), FALSE;
	for(;;) {
		ASSERT(m_mem.StackUsed()==0);
		if(!ProcessOuterStatement())	return m_ls.Close(), FALSE;
		ASSERT(m_mem.StackUsed()==0);
		if(m_ls.Get()==CLangLex::T_EOF) break;
		m_ls.Unget();
	}
	if( !m_ls.Close() || !cs.Close() )
		return m_PrintF("Input stream error\n"), FALSE;
	return	TRUE;
}
// ------------------------------------------------------------------------
bool CLangCompiler::Process(FILE *f,unsigned flags)
{
	CFileCharStream cs(m_PrintF);
	if( !cs.Open(f,CTextPos(flags&F_MULTILINE?1:0,1)) ||
									!m_ls.Open(&cs) )
		return m_PrintF("Input stream error\n"), FALSE;
	for(;;) {
		ASSERT(m_mem.StackUsed()==0);
		if(!ProcessOuterStatement())	return m_ls.Close(), FALSE;
		ASSERT(m_mem.StackUsed()==0);
		if(m_ls.Get()==CLangLex::T_EOF) break;
		m_ls.Unget();
	}
	if( !m_ls.Close() || !cs.Close() )
		return m_PrintF("Input stream error\n"), FALSE;
	return	TRUE;
}
// ------------------------------------------------------------------------
bool CLangCompiler::DeclareExtVar(TCchar *decl,...)
{
	CMemoryCharStream cs(m_PrintF);
	if( !cs.Open(decl,CTextPos(1,1)) ||
									!m_ls.Open(&cs) )
		return m_PrintF("Input stream error\n"), FALSE;
	va_list	pVArgs;
	va_start(pVArgs,decl);
	for(;;) {
		if( !ProcessType(PTF_ARRAYS) )	return m_ls.Close(), FALSE;
		for(;;) {
			if( m_ls.Get() != CLangLex::T_ID )
				return	m_ls.Error("Identifier expected"),
												m_ls.Close(), FALSE;
			bool bFound = m_context.Find(m_ls.This().Buffer()) != NULL;
			void **pData = NULL;
			if(!bFound)	{
				pData = (void**) m_mem.AllocH(sizeof(void*));
				*pData = va_arg(pVArgs,void*);
				if(!pData)	return m_ls.Error("Not enough p-memory"),
													m_ls.Close(), FALSE;
				if( !m_context.Add(m_ls.This().Buffer(),
							m_t.type|m_t.T_HEAP|m_t.T_EXT, m_t.hArrayDesc,
													m_mem.P2H(pData))  )
					return m_mem.ReleaseH(pData), m_ls.Error("Not enough memory"),
							m_ls.Close(), FALSE;
			}
			if( m_ls.Get() != CLangLex::T_COMMA &&
					m_ls.This() != CLangLex::T_SEMICOLON &&
										m_ls.This() != CLangLex::T_EOF )
				return	(bFound ? (void)0:m_context.DeleteLast()),
							(pData ? m_mem.ReleaseH(pData) : (void)0),
							m_ls.Error("Comma or semicolon expected"),
														m_ls.Close(), FALSE;
			if(bFound)
				return	m_ls.Error("Identifier already defined"),
												m_ls.Close(), FALSE;
			if( m_ls.This() == CLangLex::T_SEMICOLON ||
										m_ls.This() == CLangLex::T_EOF ) break;
		}
		if( m_ls.This() == CLangLex::T_EOF )  break;
	}
	va_end(pVArgs);
	if( !m_ls.Close() )	return m_PrintF("Input stream error\n"), FALSE;
	return	TRUE;
}
// ------------------------------------------------------------------------
bool CLangCompiler::DeclareExtFunc(TCchar *decl,
								bool (*pFunc)(CLangProcessor &lp))
{
	CMemoryCharStream cs(m_PrintF);
	if( !cs.Open(decl,CTextPos(1,1)) ||
									!m_ls.Open(&cs) )
		return m_PrintF("Input stream error\n"), FALSE;
	if( !ProcessFunctionDecl(CLangContext::CEntry::T_EXTFUNC,pFunc) )
		return m_ls.Close(), FALSE;
	if( !m_ls.Close() )	return m_PrintF("Input stream error\n"), FALSE;
	return TRUE;
}
// ------------------------------------------------------------------------
