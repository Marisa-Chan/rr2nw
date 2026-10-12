
#ifndef	__LANG_CPP__
	#include<stdio.h>
	#include<dos.h>
	#include<ctype.h>
	#include<string.h>
	#include<math.h>
	#include<stdlib.h>
	#define	_NOAUTOINCLUDE
	#include"stdtypes.h"
	#include"debugext.h"
	#include"mathlib.h"
	#include"lang.h"
#endif

// ------------------------------------------------------------------------
CLangLabelScope::CLabel *CLangLabelScope::Label(TCchar *name)
{
	ASSERT(IsOpen());
	for( word hLabel = m_hLastLabel ; hLabel != (word) -1 ; ) {
		CLabel	*pLabel = (CLabel*)m_pHeap->H2P(hLabel);
		if( strcmp(pLabel->name+1,name)==0 ) return pLabel;
		hLabel = pLabel->hNext;
	}
	CLabel *pLabel = (CLabel*)m_pHeap->AllocS(sizeof(CLabel));
	if(!pLabel) return NULL;
	pLabel->name = (char*)m_pHeap->AllocS(strlen(name)+2);
	if(!pLabel->name) return NULL;
	pLabel->name[0] = 0;
	strcpy(pLabel->name+1,name);
	pLabel->hNext = m_hLastLabel;
	pLabel->offset = -1;
	m_hLastLabel = m_pHeap->P2H(pLabel);
	return pLabel;
}
// ------------------------------------------------------------------------
void CLangLabelScope::Define(CLabel *pLabel,CLangMemory &mem)
{
	ASSERT(IsOpen());
	ASSERT(!pLabel->Defined());
	for( word hRef = pLabel->offset ; hRef != (word) -1 ; ) {
		CLabelRef *pRef = (CLabelRef*)m_pHeap->H2P(hRef);
		*(iword*) mem.H2P(pRef->offset-sizeof(iword)) =
													mem.HN()-pRef->offset;
		hRef = pRef->hNext;
	}
	pLabel->name[0] = 1;
	pLabel->offset = mem.HN();
}
// ------------------------------------------------------------------------
bool CLangLabelScope::AddRef(CLabel *pLabel,CLangMemory &mem)
{
	ASSERT(IsOpen());
	ASSERT(!pLabel->Defined());
	CLabelRef  *pRef = (CLabelRef*)m_pHeap->AllocS(sizeof(CLabelRef));
	if(!pRef)	return FALSE;
	pRef->offset = mem.HN();
	pRef->hNext = pLabel->offset;
	pLabel->offset = m_pHeap->P2H(pRef);
	return TRUE;
}
// ------------------------------------------------------------------------
CLangLabelScope::CLabel *CLangLabelScope::Unresolved() const
{
	ASSERT(IsOpen());
	for( word hLabel = m_hLastLabel ; hLabel != (word) -1 ; ) {
		CLabel	*pLabel = (CLabel*)m_pHeap->H2P(hLabel);
		if( !pLabel->Defined() )  return pLabel;
		hLabel = pLabel->hNext;
	}
	return NULL;
}
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
CLangLoopScope::CLoop *CLangLoopScope::Find(TCchar *name) const
{
	ASSERT(IsOpen());
	if( !name )	return m_hInnerLoop == (word)-1 ? NULL :
										(CLoop*)m_pHeap->H2P(m_hInnerLoop);
	for( word hLoop = m_hInnerLoop ; hLoop != (word) -1 ; ) {
		CLoop	*pLoop = (CLoop*)m_pHeap->H2P(hLoop);
		if( strcmp(pLoop->name,name)==0 ) return pLoop;
		hLoop = pLoop->hNext;
	}
	return NULL;
}
// ------------------------------------------------------------------------
bool CLangLoopScope::BeginLoop(TCchar *name,word offset)
{
	ASSERT(IsOpen());
	CLoop *pLoop = (CLoop*)m_pHeap->AllocH(sizeof(CLoop));
	if(!pLoop) return FALSE;
	pLoop->name = (char*)m_pHeap->AllocH(strlen(name)+1);
	if(!pLoop->name) return FALSE;
	strcpy(pLoop->name,name);
	pLoop->hNext = m_hInnerLoop;
	pLoop->offset = offset;
	pLoop->hBreaks = -1;
	m_hInnerLoop = m_pHeap->P2H(pLoop);
	return TRUE;
}
// ------------------------------------------------------------------------
bool CLangLoopScope::BreakLoop(CLangLoopScope::CLoop *pLoop,word offset)
{
	ASSERT(IsOpen());
	CLoopBreak  *pBreak = (CLoopBreak*)m_pHeap->AllocH(sizeof(CLoopBreak));
	if(!pBreak)	return FALSE;
	pBreak->offset = offset;
	pBreak->hNext = pLoop->hBreaks;
	pLoop->hBreaks = m_pHeap->P2H(pBreak);
	return TRUE;
}
// ------------------------------------------------------------------------
void CLangLoopScope::EndLoop(CLangMemory &mem)
{
	ASSERT(IsOpen());
	ASSERT(m_hInnerLoop != (word)-1 );
	CLoop  *pLoop = (CLoop*) m_pHeap->H2P(m_hInnerLoop);
	for( word hBreak = pLoop->hBreaks ; hBreak != (word) -1 ; ) {
		CLoopBreak *pBreak = (CLoopBreak*)m_pHeap->H2P(hBreak);
		*(iword*) mem.H2P(pBreak->offset-sizeof(iword)) =
												mem.HN()-pBreak->offset;
		hBreak = pBreak->hNext;
	}
	m_hInnerLoop = pLoop->hNext;
	m_pHeap->ReleaseH(pLoop);
}
// ------------------------------------------------------------------------
char CLangStatementCompiler::m_szMsgBuf[300];
// ------------------------------------------------------------------------
#ifdef	_DISPPCODE
	#define	Compile(C) (m_PrintF("\n%04Xh: %s",m_mem.HN(),#C),Compile(C))
#endif
bool CLangStatementCompiler::CompileBuf(TCvoid *pBuf,word len)
{
	#ifdef	_DISPPCODE
		m_PrintF(" Buf(%d)",len);
	#endif
	byte	*p = m_mem.HP();
	if( !m_mem.AllocH(len) ) return
								m_ls.Error("Not enough p-memory"), FALSE;
	memcpy(p,pBuf,len);
    return TRUE;
}
// ------------------------------------------------------------------------
bool CLangStatementCompiler::ProcessStatement()
{
	while( m_ls.Get() == CLangLex::T_LABEL ) {
		CLangLabelScope::CLabel	*pLabel = m_labelScope.Label(
												m_ls.This().Buffer());
		if( !pLabel ) return m_ls.Error("Not enough memory"), FALSE;
		if( m_ls.Get() != CLangLex::T_COLON )
			return m_ls.Error("Colon expected"), FALSE;
		if( pLabel->Defined() )
			return m_ls.Error("Label already defined"), FALSE;
		m_labelScope.Define(pLabel,m_mem);
	}
	if( m_ls.This() == CLangLex::T_ID ) {
		m_ls.Unget();
		if(!ProcessExprUna())	return FALSE;
		if( m_ls.Get() == CLangLex::T_ASSIGN ) {
			if( !(m_t.type & m_t.T_LEFT) )
				return m_ls.Error("Left value expected"), FALSE;
			if( !(m_t.type & m_t.T_SINGULAR) )
				return m_ls.Error("Singular type expected"), FALSE;
			CType	t = m_t;
			if( !ProcessExpr() ) return FALSE;
			if( m_ls.Get() != CLangLex::T_SEMICOLON )
				return m_ls.Error("Semicolon expected"), FALSE;
			switch( t.Type() ) {
				case t.T_VECTOR:
					if( m_t.Type() != m_t.T_VECTOR ) return
						m_ls.Error("Types incompatible in assignment"), FALSE;
					if( !Deref() )	return FALSE;
					ASSERT( MAKEBOOL(t.type&t.T_EXT) +
							MAKEBOOL(t.type&t.T_STACK) +
							MAKEBOOL(t.type&t.T_HEAP) == 1 );
					if( !Compile( t.type & t.T_HEAP ? P_ASSIGN_HV :
								t.type & t.T_STACK ? P_ASSIGN_SV :
												P_ASSIGN_EV ) ) return FALSE;
					break;
				case t.T_FLOAT:
					if( !CastScalar(m_t.T_FLOAT) )	return FALSE;
					ASSERT( MAKEBOOL(t.type&t.T_EXT) +
							MAKEBOOL(t.type&t.T_STACK) +
							MAKEBOOL(t.type&t.T_HEAP) == 1 );
					if( !Compile( t.type & t.T_HEAP ? P_ASSIGN_HF :
								t.type & t.T_STACK ? P_ASSIGN_SF :
												P_ASSIGN_EF ) ) return FALSE;
					break;
				case t.T_INT:
					if( !CastScalar(m_t.T_INT) )	return FALSE;
					ASSERT( MAKEBOOL(t.type&t.T_EXT) +
							MAKEBOOL(t.type&t.T_STACK) +
							MAKEBOOL(t.type&t.T_HEAP) == 1 );
					if( !Compile( t.type & t.T_HEAP ? P_ASSIGN_HI :
								t.type & t.T_STACK ? P_ASSIGN_SI :
												P_ASSIGN_EI ) ) return FALSE;
					break;
				default: ASSERT(0);
			}
		} else {	// if not assign
			if( m_ls.This() != CLangLex::T_SEMICOLON ) return
				m_ls.Error("Semicolon or sign of assignment expected"), FALSE;
			if( m_t.Type() != m_t.T_VOID ) {
				if( m_t.type & m_t.T_LEFT ) {
					if( m_t.type & m_t.T_EXT )	{
						if( !Compile(P_DROP_EP) ) return FALSE;
					} else {
						if( !Compile(P_DROP_P) ) return FALSE;
					}
				} else {
					switch(m_t.Type()) {
						case m_t.T_INT:	if(!Compile(P_DROP_I)) return FALSE;
						case m_t.T_FLOAT: if(!Compile(P_DROP_F)) return FALSE;
						case m_t.T_VECTOR: if(!Compile(P_DROP_V)) return FALSE;
					}
				}
			} // if not void
		} // if not assign
	} /* ASSIGN or PROC call */ else
	if( m_ls.This() == CLangLex::T_LBRACE ) {
		while( m_ls.Get() != CLangLex::T_RBRACE ) {
			m_ls.Unget();
			if( !ProcessStatement() )	return FALSE;
		}
	} /* Compound operator */ else
	if( m_ls.This() == CLangLex::T_IF ) {
		if( !ProcessExpr() )	return FALSE;
		if( !(m_t.Type() & m_t.T_SCALAR) )
			return	m_ls.Error("Scalar type expected"), FALSE;
		if( !Deref() )	return FALSE;
		switch(m_t.Type()) {
			case m_t.T_INT:	if(!Compile(P_JZ_I)) return FALSE; break;
			case m_t.T_FLOAT: if(!Compile(P_JZ_F)) return FALSE; break;
			default: ASSERT(0);
		}
		if(!CompileWord(0))	return FALSE;
		unsigned nIfJump = m_mem.HN();
		if( m_ls.Get() != CLangLex::T_THEN )
			return	m_ls.Error("THEN keyword expected"), FALSE;
		if( !ProcessStatement() ) return FALSE;
		if( m_ls.Get() == CLangLex::T_ELSE ) {
			if( !Compile(P_JMP) || !CompileWord(0) )	return FALSE;
			*(iword*)m_mem.H2P(nIfJump-sizeof(iword)) = m_mem.HN()-nIfJump;
			nIfJump = m_mem.HN();
			if( !ProcessStatement() )	return FALSE;
		} else m_ls.Unget();
		*(iword*)m_mem.H2P(nIfJump-sizeof(iword)) = m_mem.HN()-nIfJump;
	} /* IF */ else
	if( m_ls.This() == CLangLex::T_LOOP ) {
		unsigned nBegin = m_mem.HN();
		if( !m_loopScope.BeginLoop( m_ls.Get() == CLangLex::T_ID ?
				m_ls.This().Buffer() :
						(m_ls.Unget(),(TCchar*)NULL), nBegin ) )
			return m_ls.Error("Not enough memory"), FALSE;
		if( m_ls.Get() != CLangLex::T_LBRACE )
			return	m_ls.Error("Compound operator expected"), FALSE;
		m_ls.Unget();
		if( !ProcessStatement() )	return FALSE;
		if( !Compile(P_JMP) ||
				!CompileWord(nBegin-m_mem.HN()-sizeof(word)) ) return FALSE;
		m_loopScope.EndLoop(m_mem);
	} /* LOOP */ else
	if( m_ls.This() == CLangLex::T_FOR ) {
		if( m_ls.Get() != CLangLex::T_ID )
			return m_ls.Error("Identifier expected"), FALSE;
		CLangContext::CEntry *pEntry = m_funcContext.Find(
												m_ls.This().Buffer());
		if(!pEntry) pEntry = m_context.Find(m_ls.This().Buffer());
		if(!pEntry) return m_ls.Error("Undefined identifier"), FALSE;
		if( (pEntry->Type() & m_t.T_TYPE) != m_t.T_INT )
			return m_ls.Error("Integer variable expected"), FALSE;
		bool bExt = pEntry->Type() & m_t.T_EXT;
		PCOMMAND ePAssign, ePDeref, ePPtr;
		if( bExt ) {
			ePAssign = P_ASSIGN_EI; ePDeref = P_DEREF_EI;
			if( pEntry->Type() & m_t.T_HEAP) ePPtr = P_EPTR_H;
			else ASSERT(0);
		} else {
			if( pEntry->Type() & m_t.T_HEAP)
				ePAssign = P_ASSIGN_HI, ePDeref = P_DEREF_HI,
												ePPtr = P_PTR; else
			if( pEntry->Type() & m_t.T_STACK)
				ePAssign = P_ASSIGN_SI, ePDeref = P_DEREF_SI,
												ePPtr = P_PTR;
			else ASSERT(0);
		}
		if( m_ls.Get() != CLangLex::T_ASSIGN )
			return m_ls.Error("Sign of assignment expected"), FALSE;
		if( !Compile(ePPtr) || !CompileWord(pEntry->Offset()) )  return FALSE;
		if( !ProcessExpr() ) return FALSE;
		bool bAscend;
		if( m_ls.Get() == CLangLex::T_TO ) bAscend = TRUE; else
		if( m_ls.This() == CLangLex::T_DOWNTO ) bAscend = FALSE; else
			return m_ls.Error("TO or DOWNTO expected"), FALSE;
		if( !CastScalar(m_t.T_INT) )	return FALSE;
		if( !Compile(ePAssign) )	return FALSE;
		unsigned nBegin = m_mem.HN();
		if( !Compile(ePPtr) || !CompileWord(pEntry->Offset()) )  return FALSE;
		if( !Compile(ePDeref) )	return FALSE;
		if( !ProcessExpr() ) return FALSE;
		if( m_ls.Get() != CLangLex::T_LOOP )
			return m_ls.Error("LOOP expected"), FALSE;
		if( !CastScalar(m_t.T_INT) )	return FALSE;
		if( !Deref() )	return FALSE;
		if( !Compile(bAscend?P_LE_II:P_GE_II) )	return FALSE;
		if( !Compile(P_JZ_I) || !CompileWord(0) ) return FALSE;
		unsigned nLeave = m_mem.HN();
		if( !m_loopScope.BeginLoop( m_ls.Get() == CLangLex::T_ID ?
				m_ls.This().Buffer() :
						(m_ls.Unget(),(TCchar*)NULL), nBegin ) )
			return m_ls.Error("Not enough memory"), FALSE;
		if( m_ls.Get() != CLangLex::T_LBRACE )
			return	m_ls.Error("Compound operator expected"), FALSE;
		m_ls.Unget();
		if( !ProcessStatement() )	return FALSE;
		if( !Compile(ePPtr) || !CompileWord(pEntry->Offset()) ||
			!Compile(ePPtr) || !CompileWord(pEntry->Offset()) ||
			!Compile(ePDeref) ||
			!Compile(P_INT) || !CompileInt(bAscend?1:-1) ||
			!Compile(P_ADD_II) || !Compile(ePAssign))	return FALSE;
		if( !Compile(P_JMP) ||
				!CompileWord(nBegin-m_mem.HN()-sizeof(word)) ) return FALSE;
		m_loopScope.EndLoop(m_mem);
		*(iword*)m_mem.H2P(nLeave-sizeof(iword)) = m_mem.HN()-nLeave;
	} /* FOR */ else
	if( m_ls.This() == CLangLex::T_BREAK ) {
		CLangLoopScope::CLoop *pLoop = m_loopScope.Find(
			m_ls.Get() == CLangLex::T_ID ? m_ls.This().Buffer() :
										(m_ls.Unget(),(TCchar*)NULL) );
		if( m_ls.Get() != CLangLex::T_SEMICOLON )
			return	m_ls.Error("Semicolon expected"), FALSE;
		if( !pLoop ) return m_ls.Error("BREAK for no LOOP"), FALSE;
		if( !Compile(P_JMP) || !CompileWord(0) )
			return m_ls.Error("Not enough p-memory"), FALSE;
		if( !m_loopScope.BreakLoop(pLoop,m_mem.HN()) )
			return m_ls.Error("Not enough memory"), FALSE;
	} /* BREAK */ else
	if( m_ls.This() == CLangLex::T_GOTO ) {
		if( m_ls.Get() != CLangLex::T_LABEL )
			return	m_ls.Error("Label expected"), FALSE;
		CLangLabelScope::CLabel *pLabel = m_labelScope.Label(
													m_ls.This().Buffer());
		if( !pLabel )	return m_ls.Error("Not enough memory"), FALSE;
		if( !Compile(P_JMP) )	return m_ls.Error("Not enough p-memory"), FALSE;
		if( pLabel->Defined() ) {
			if( !CompileWord(pLabel->Offset()-m_mem.HN()-sizeof(word)) )
				return m_ls.Error("Not enough p-memory"), FALSE;
		} else {
			if( !CompileWord(0) )
				return m_ls.Error("Not enough p-memory"), FALSE;
			if( !m_labelScope.AddRef(pLabel,m_mem) )
				return m_ls.Error("Not enough p-memory"), FALSE;
		}
		if( m_ls.Get() != CLangLex::T_SEMICOLON )
			return	m_ls.Error("Semicolon expected"), FALSE;
	} else
	if( m_ls.This() == CLangLex::T_QST || m_ls.This() == CLangLex::T_PRINT ) {
		if( m_ls.Get() == CLangLex::T_SEMICOLON || (m_ls.Unget(),FALSE) ) {
			if( ! Compile(P_PRINT_NL) )	return FALSE;
		} else
		for(;;) {
			if( m_ls.Get() == CLangLex::T_STRINGV ) {
				if( ! Compile(P_PRINT_SC) ) return FALSE;
				word len = (word) strlen(m_ls.This().Buffer())+1;
				if( ! CompileWord(len) ||
								! CompileBuf(m_ls.This().Buffer(),len) )
														return FALSE;
			} else {
				m_ls.Unget();
				if( !ProcessExpr() )	return FALSE;
				if( !(m_t.Type() & m_t.T_SINGULAR) )
					return m_ls.Error("Singular expression expected"), FALSE;
				if( !Deref() )	return FALSE;
				switch( m_t.Type() ) {
					case m_t.T_INT:
						if( ! Compile(P_PRINT_I) )	return FALSE; break;
					case m_t.T_FLOAT:
						if( ! Compile(P_PRINT_F) )	return FALSE; break;
					case m_t.T_VECTOR:
						if( ! Compile(P_PRINT_V) )	return FALSE; break;
					default: ASSERT(0);
				}
			}
			if( m_ls.Get() != CLangLex::T_COMMA &&
										m_ls.This() != CLangLex::T_SEMICOLON )
					return m_ls.Error("Comma or semicolon expected"), FALSE;
			if( m_ls.This() == CLangLex::T_SEMICOLON )	{
					if( ! Compile(P_PRINT_NL) ) return FALSE; break; }
			if( m_ls.Get() == CLangLex::T_SEMICOLON ) break; else m_ls.Unget();
		}
	} /* PRINT */ else
	if( m_ls.This() == CLangLex::T_RETURN ) {
		if( m_ls.Get() == CLangLex::T_SEMICOLON ) {
			if( m_retType.Type() != m_retType.T_VOID )
				return m_ls.Error("Function must return a value"), FALSE;
			if(	 ! Compile(P_LEAVE) ||
				 ! CompileWord(m_nArgsSize) ||
					! CompileWord(0)  )	return FALSE;
		}
		else {
			if( m_retType.Type() == m_retType.T_VOID )
				return m_ls.Error("Semicolon expected"), FALSE;
			m_ls.Unget();
			if( ! ProcessExpr() )	return FALSE;
			if( m_ls.Get() != CLangLex::T_SEMICOLON )
				return m_ls.Error("Semicolon expected"), FALSE;
			if( (m_t.Type() & m_t.T_SINGULAR) && !Deref() ) return FALSE;
			switch( m_retType.Type() ) {
				case m_retType.T_VECTOR:
					if( m_t.Type() != m_t.T_VECTOR )
						return m_ls.Error("Incompatible return type"), FALSE;
					break;
				case m_retType.T_FLOAT:
					if( !CastScalar(m_t.T_FLOAT) )	return FALSE; break;
				case m_retType.T_INT:
					if( !CastScalar(m_t.T_INT) )	return FALSE; break;
			}
			if( ! Compile(P_LEAVE) ||
				 ! CompileWord(m_nArgsSize) ||
					! CompileWord(m_t.SizeArg())  )	return FALSE;
		}
	} /* RETURN */ else
	if( m_ls.This() == CLangLex::T_SEMICOLON ) {	// Empty statement
	} else
		return m_ls.Error("Syntax error"),FALSE;
	return TRUE;
}
// ------------------------------------------------------------------------
bool CLangStatementCompiler::ProcessScopedStatement()
{
	ASSERT(!m_labelScope.IsOpen());
	ASSERT(!m_loopScope.IsOpen());
	TCvoid		*pStart = m_mem.HP();
	m_labelScope.Open();
	m_loopScope.Open();
	bool bRes = ProcessStatement();
	ASSERT(m_loopScope.IsEmpty() || !bRes);
	if( bRes ) {
		CLangLabelScope::CLabel	*pUnresolved = m_labelScope.Unresolved();
		if( pUnresolved ) {
			sprintf(m_szMsgBuf,"Undefined label: @%s",pUnresolved->Name());
			m_ls.Error(m_szMsgBuf);
			bRes = FALSE;
		}
	}
	m_labelScope.Close();
	m_loopScope.Close();
	if( !bRes )	m_mem.ReleaseH(pStart);
	return bRes;
}
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
bool CLangStatementCompiler::Exec(TCchar *lpText,unsigned flags)
{
	CMemoryCharStream cs(m_PrintF);
	if( !cs.Open(lpText,CTextPos(flags&F_MULTILINE?1:0,1)) ||
									!m_ls.Open(&cs) )
		return m_PrintF("Input stream error\n"), FALSE;
	for(;;) {
		TCvoid *pStart = m_mem.HP();
		if(!ProcessScopedStatement() || !Compile(P_HALT))
			return m_ls.Close(), m_mem.ReleaseH(pStart), FALSE;
		#ifdef	_DISPPCODE
			m_PrintF("\n");
		#endif
		ASSERT(m_mem.StackUsed()==0);
		if(!Go(m_mem.P2H(pStart))) return ClearStack(),
						m_ls.Close(), m_mem.ReleaseH(pStart), FALSE;
		ASSERT(m_mem.StackUsed()==0);
		m_mem.ReleaseH(pStart);
		if(m_ls.Get()==CLangLex::T_EOF) break;
		m_ls.Unget();
	}
	if( !m_ls.Close() || !cs.Close() )
		return m_PrintF("Input stream error\n"), FALSE;
	return	TRUE;
}
// ------------------------------------------------------------------------
