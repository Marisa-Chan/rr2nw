/*
     File:  Suavik\d:\game\kernel\src\object.cpp
     Autor: M.Krylov. Modify Suavik
     Ver    1.5

     Описание абстрактного класса KR_Object, и типа KR_Event.

     префикс o_
     Поскольку стек один, то при попытке писать в данные параллельно
     (например из скрипта) с тэгами  все может рухнуть.
 */
#include "Kernel\h\Object.h"
#include "Kernel\h\Context.h"
#include "kernel\h\Echo.h"

#define LAST_H__VIEW
#include "game.h"
#include "storage\h\savefile.h"



//const char PIN_check[] = "PIN";


bool	KR_Event::dump(PIN_SaveFile & sf)
{
	PIN_SaveItemPrefix prefix;

	prefix.m_Type = PIN_SaveItemPrefix::IP_EVENT;
	strcpy(prefix.m_Check,PIN_check);

	bool result = sf.WriteData( (char *) & prefix, sizeof(PIN_SaveItemPrefix)  );

	if (!result)
		return false;

	result = sf.WriteData( (char *) &label, sizeof(KR_Event)  );

	if (!result)
		return false;

	return true;
}

bool	KR_Event::load(PIN_SaveFile & sf)
{
	PIN_SaveItemPrefix prefix;	

	sf.GetData((char *) & prefix, sizeof(PIN_SaveItemPrefix));

	if (strcmp(prefix.m_Check,PIN_check) != 0 ||
		prefix.m_Type != PIN_SaveItemPrefix::IP_EVENT)
		return false;

	sf.GetData( (char *) &label, sizeof(KR_Event)  );

	return true;
}



//static char PIN_SaveItemCheck[4] = "PIN";


// ============================================================================
KR_Object::KR_Object()
 {
  /*
     Если объект не добавлен в контекст, то выдавать диаг.
   */

    context		= NULL;
	m_tableName = NULL;
 }

// ============================================================================
void KR_Object::issueEvent(const KR_Event &event)
 {
   if(  context != NULL  )
        context->addEvent(event);
 }


bool KR_Object::dump(PIN_SaveFile & /*sf*/)
{
	echo("Default Dump Called for %d\n",id);
	return true;
}

bool KR_Object::load(PIN_SaveFile & /*sf*/)
{
	echo("Default Load Called for %d\n",id);
	return true;
}

// ============================================================================
void *KR_Object::queryInterface( int interNum )
 {
    switch( interNum )
    {
    case IUnknownIID: return this;
    }
    return 0;
 }

/* End of file OBJECT.H */