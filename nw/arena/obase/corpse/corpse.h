/*
 * File  : C:\nw\ARENA\OBASE\Corpse\Corpse.h
 * Autor :
 * Ver   1.0 
 */
#ifndef __CORPSE_H__INCLUDED
#define __CORPSE_H__INCLUDED

#define LAST_H__VIEW
#include "game.h"
#include "scene.h"
#include "..\DynObj\DynObj.h"
#include "storage/h/subject.h"
#include "storage/h/attr.h"
#include "kernel/h/active.h"
#include "storage/h/strgdefs.h"
#include "i/dynobj.i"


class AttributeCorpse;

typedef struct {
	int	m_attributeIndex;
	KR_ObjectID m_smoke;
	KR_ObjectID m_fire;
	int m_mustDieNow;
} CorpseData;

class Corpse :		public ct_Subject,
					public CorpseData
				
{
 public:

    strg_SUBJECT_DYNVIEW_DECLARE

	const AttributeCorpse   *m_attr;
	
             Corpse();
    virtual ~Corpse();
    virtual int  receiveEvent( KR_Event &event );    
    virtual void addNotify   ();
    virtual void removeNotify();

	void	DestroyMe();

	void onRender();
	virtual void       onHide(double time);

	virtual void      *queryInterface( int interNum );
    
	virtual bool	dump(PIN_SaveFile & sf);
	virtual bool	load(PIN_SaveFile & );
	virtual void	loadNotify();
	virtual bool	shouldDump () { return true; } 		
};

#endif // ifndef __CORPSE_H__INCLUDED
/* End of file C:\nw\ARENA\OBASE\Corpse\Corpse.h */