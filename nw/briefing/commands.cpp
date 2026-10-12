#define LAST_H__SCENE
#include "game.h"

#include "console.h"
#include "hardware.h"
#include "..\arena\h\phisics.h"
#include "..\arena\h\cnststr.h"
#include "..\arena\h\olevel.h"
#include "..\arena\h\vehicle.h"
#include "..\ARENA\STORAGE\H\classtab.h"
#include "..\ARENA\STORAGE\H\subject.h"
#include "..\ARENA\STORAGE\H\attr.h"
#include "..\ARENA\KERNEL\H\s_evdata.h"
#include "..\ARENA\KERNEL\H\session.h"

int g_godMode = 0;

void GameConsole::ReadVessels(const char *)
{
		VehicleTable  * theTable = (VehicleTable *) g_arena.searchClassTable( "Vehicle" );
		theTable->ReadConfig ();

}


void GameConsole::logEcho(char * string)
{
	echo(string);
/*	createMessage(g_arena.getObjectID(), 
		     "Font.p_menu.fnt",			  
		      string,
		      10, Session::m_moment);
*/

	
	if (m_logMode)
	{
		FILE * fileHandle = fopen(m_logFileName,"at");
		if (fileHandle)
		{
			fprintf(fileHandle,"%s\n",string);
			fclose(fileHandle);
		}
	}
}  


void GameConsole::PrintAttributeValues()
{
	static char buffer[128];
	static char buffer2[128];

	for (int i = 0; i < m_cacheAttribute->m_attrQnty; i++)
	{
		
		m_cacheAttribute->m_list[i].getString(buffer2);
		
		sprintf(buffer,"%s [%s] = %s",
			m_cacheAttribute->m_list[i].m_name,
			ed_Tag2Msg( m_cacheAttribute->m_list[i].m_type), buffer2);
		
		logEcho(buffer);
	}
}


//---------------------------------------------------
void GameConsole::ParseCommand(const char *str){
	s_ASSERT(str != NULL, "");


	if (! * str)
		return;
	
	char buffer[256];
	
	str_ConstStr theString(str);
	
    int i = theString.trim(0), j;

	//****************************************************************************
	if( (j=theString.get("god",i))!=i)
	{
	 if (g_godMode)
 		g_godMode = 0;
	 else
		g_godMode = 1;

	 sprintf(buffer,"God Mode = %d\n",g_godMode);
	 logEcho(buffer);			
	
	 return;
	}

	//****************************************************************************
	if( (j=theString.get("attach",i))!=i)
	{	
		if (hookThreadHandle)
		{
			logEcho("Already mapped\n");
			return;
		}

		hookThreadHandle = ::CreateThread (NULL, 0, HookThread, (LPVOID *) this, 0, &hookThreadID); 
		logEcho("Thread Created\n");
		return;
	}
	
	//****************************************************************************
	if( (j=theString.get("dump",i))!=i)
	{	
		if (currentLevel != LEVEL_ATTRIBUTE)
		{
			logEcho("Must be at ATTRIBUTE level\n");
			return;
		}

		for (int i = 0; i < m_cacheAttribute->m_attrQnty; i++)
		{
			char buffer2[128];
			
			m_cacheAttribute->m_list[i].getString(buffer2);
			
					
			switch(m_cacheAttribute->m_list[i].m_type){
			case EDI_BYTE:
			case EDI_CHAR:
			case EDI_INT:
			case EDI_UINT:
			case EDI_LONG:
			case EDI_ULONG:
			case EDI_SHORT:
			case EDI_USHORT:			
				sprintf(buffer,"SetAttribute_i (oID, cachePos, \"%s\", %s);",
					m_cacheAttribute->m_list[i].m_name,
					buffer2);
				break;
			case EDI_DOUBLE:
			case EDI_FLOAT:
				sprintf(buffer,"SetAttribute_f (oID, cachePos, \"%s\", %s);",
					m_cacheAttribute->m_list[i].m_name,
					buffer2);
				break;
			case EDI_STR:
				sprintf(buffer,"SetAttribute_s (oID, cachePos, \"%s\", \"%s\");",
					m_cacheAttribute->m_list[i].m_name,
					buffer2);
				break;
				
			}
							
			logEcho(buffer);			
		}

		logEcho( "");
		return;
	}	
	
	//****************************************************************************		
	if( (j=theString.get("tables",i))!=i)
	{
		logEcho( "Tables:");
		ct_ClassTable *list = ct_Storage::m_classTableList;
		
		for(; list!=NULL ; list = list->m_nextClassTable )
		{
			logEcho(list->m_name);			
		}
		logEcho( "");
		return;
	}

	//****************************************************************************		
	if( (j=theString.get("attrtables",i))!=i)
	{
		logEcho( "Tables:");
		ct_ClassTable *list = ct_Storage::m_classTableList;
		
		for(; list!=NULL ; list = list->m_nextClassTable )
		{
			if (list->objectsType() == OBJECT_ATTRIBUTE)
				logEcho(list->m_name);				
		}
		logEcho( "");
		return;
	}

	//****************************************************************************			
	if ((j=theString.get("help",i))!=i)
	{
		logEcho("Commands:log [filename]\ntables, seancetables, attrtables, level\ntable <tablename>\nlist, dump\n\nattr <attribute name>\n");
		return;
	}


	//****************************************************************************				
	if ((j=theString.get("needkill",i))!=i)
	{
		IPlayer * player = & g_vehicle->player();
		PlayerMission mis = player->getMiss(0);

		int index;
		i = theString.trim(j);
		
		str_ConstStr::STR_TYPE type = theString.query( i );
		if (type != str_ConstStr::STR_INT)
		{
			logEcho("And where is the number?");
			return;
		}
		
		theString.to( index , i);
		int maxnum = mis.success_needKill.getCount();

		if (index >= maxnum)
		{
			logEcho("Number is too big");
			return;
		}


		KR_ObjectID unitID = mis.success_needKill[index];
		IDynamicObject * object = (IDynamicObject *) context->queryInterface( unitID, IDynamicObjectIID);

		if (!object)
		{
			logEcho("Object is not yet created");
			return;
		}

		CFVector3  pos = object->getPos();

		pos.y += 3;

		g_vehicle->Restart(); 
		g_vehicle->SetPos(pos);

		return;
	}


//****************************************************************************				
	if ((j=theString.get("needlive",i))!=i)
	{
		IPlayer * player = & g_vehicle->player();
		PlayerMission mis = player->getMiss(0);

		int index;
		i = theString.trim(j);
		
		str_ConstStr::STR_TYPE type = theString.query( i );
		if (type != str_ConstStr::STR_INT)
		{
			logEcho("And where is the number?");
			return;
		}
		
		theString.to( index , i);
		int maxnum = mis.success_needLive.getCount();

		if (index >= maxnum)
		{
			logEcho("Number is too big");
			return;
		}


		KR_ObjectID unitID = mis.success_needLive[index];
		IDynamicObject * object = (IDynamicObject *) context->queryInterface( unitID, IDynamicObjectIID);

		if (!object)
		{
			logEcho("Object is not yet created");
			return;
		}

		CFVector3  pos = object->getPos();

		pos.y += 3;

		g_vehicle->Restart(); 
		g_vehicle->SetPos(pos);

		return;
	}
		
//****************************************************************************				
	if ((j=theString.get("needreach",i))!=i)
	{
		IPlayer * player = & g_vehicle->player();
		PlayerMission mis = player->getMiss(0);

		int index;
		i = theString.trim(j);
		
		str_ConstStr::STR_TYPE type = theString.query( i );
		if (type != str_ConstStr::STR_INT)
		{
			logEcho("And where is the number?");
			return;
		}
		
		theString.to( index , i);
		int maxnum = mis.success_needReached.getCount();

		if (index >= maxnum)
		{
			logEcho("Number is too big");
			return;
		}


		KR_ObjectID unitID = mis.success_needReached[index];
		IDynamicObject * object = (IDynamicObject *) context->queryInterface( unitID, IDynamicObjectIID);

		if (!object)
		{
			logEcho("Object is not yet created");
			return;
		}

		CFVector3  pos = object->getPos();

		pos.y += 3;

		g_vehicle->Restart(); 
		g_vehicle->SetPos(pos);

		return;
	}

	//****************************************************************************				
	if ((j=theString.get("seancetables",i))!=i)
	{
					
		for( int i = 0; i< g_arena.m_seanceClassTableQnty; ++i )
			logEcho(g_arena.m_seanceClassTablePool[i]->m_name);
		logEcho( "");
		return;
	}
				
	//****************************************************************************
	if ((j=theString.get("state",i))!=i)

	{		
		sprintf(buffer,"Log Mode %s",m_logMode?"On" : "Off");
		logEcho(buffer);
		const char * name ;

		switch (currentLevel) {
			case LEVEL_ATTRIBUTE:
				name = g_arena.context->searchObject(m_cacheAttribute->getObjectID());
				sprintf(buffer,"Attribute = %s",name);
				logEcho(buffer);
			case LEVEL_TABLE:
				sprintf(buffer,"Table = %s",m_cacheClassTable->m_name);
				logEcho(buffer);
		}
		
		logEcho("");
		return;
	}

	//****************************************************************************
	if ((j=theString.get("level",i))!=i)
	{
		currentLevel = LEVEL_ATTRIBUTE;

		m_cacheAttribute = &g_levelAttr;

		PrintAttributeValues();
		logEcho("");
		return;
	}


	//****************************************************************************
	if ((j=theString.get("list",i))!=i)
	{
		switch (currentLevel) {
		case LEVEL_TABLE:
			{					
				if(  m_cacheClassTable->m_existList == NULL  )
				{
					logEcho("Table Empty\n");
				}
				else
				{
					for( const ct_Object *ob = m_cacheClassTable->m_existList; 
					ob!=NULL;ob = ob->m_next )
					{													
						strcpy(buffer,g_arena.context->searchObject(ob->getObjectID()));
						logEcho(buffer);
					} // for
					
					logEcho( "");
				}// Table Is Not Empty
				
			}
			break;
			
		case LEVEL_ATTRIBUTE:
			{
				for (int i = 0; i < m_cacheAttribute->m_attrQnty; i++)
				{
					char buffer2[128];
					
					m_cacheAttribute->m_list[i].getString(buffer2);
					
					sprintf(buffer,"%s [%s] = %s",
						m_cacheAttribute->m_list[i].m_name,
						ed_Tag2Msg( m_cacheAttribute->m_list[i].m_type), buffer2);
					
					logEcho(buffer);
				}
			}
			return;
		}	

		return;
	}


	//****************************************************************************				
	if ((j=theString.get("attr",i))!=i)
	{

		if (currentLevel != LEVEL_TABLE)
		{
			logEcho("Must be at TABLE level\n");
			return;
		}

		i = theString.trim(j);


		if (theString.query(i) == str_ConstStr::STR_OTHER)
		{
			int k = 0;
			str_ConstStr::STR_TYPE result;
							
			for (	result = theString.query(k+i); 
					result != str_ConstStr::STR_SEPARATOR && result != str_ConstStr::STR_OUTOFRANGE;
					result  = theString.query(k+i))
						k++;						
			
			strncpy(buffer,theString.getChars() + i, k );
			buffer[k] = 0;

			if(  m_cacheClassTable->m_existList == NULL  )
			{
				logEcho("Table Empty\n");
				return;
			}
			
			m_cacheAttribute = 0;

			for( const ct_Object *ob = m_cacheClassTable->m_existList; ob!=NULL;ob = ob->m_next )
			{
				const char * name = g_arena.context->searchObject(ob->getObjectID());
				if (strcmp(buffer,name) == 0)
				{
					if (ob->style() == OBJECT_ATTRIBUTE)
						m_cacheAttribute = (ct_Attribute	*) ob;
					else
					{
						m_cacheAttribute = NULL;
						logEcho("Not an Attribute\n");
						return;
					}
					break;
				}				
			}

			if (!m_cacheAttribute)
			{
				logEcho("Object Not Found\n");
				return;
			}

			PrintAttributeValues();


			currentLevel = LEVEL_ATTRIBUTE;
			logEcho( "");
		}					
		else
		logEcho("USAGE attribute <name>\n");

		return;
	}
	
			
	//****************************************************************************
	if ((j=theString.get("table",i))!=i)
	{
		i = theString.trim(j);	
		
		if (theString.query(i) == str_ConstStr::STR_OTHER)
		{
			int k = 0;
			str_ConstStr::STR_TYPE result;
							
			for (	result = theString.query(k+i); 
					result != str_ConstStr::STR_SEPARATOR && result != str_ConstStr::STR_OUTOFRANGE;
					result  = theString.query(k+i))
						k++;						
			
			strncpy(buffer,theString.getChars() + i, k );
			buffer[k] = 0;

			m_cacheClassTable = g_arena.searchClassTable( buffer );

			if (! m_cacheClassTable)
			{
				logEcho("Table Not Found\n");
			}	
				else
			{
				currentLevel = LEVEL_TABLE;				
				sprintf(buffer,"Table = %s",m_cacheClassTable->m_name);
				logEcho(buffer);
				logEcho( "");
			}
		}					
		else
		logEcho("USAGE table <name>\n");

		return;
						
	}

//****************************************************************************						
	if ((j=theString.get("update",i))!=i)
	{
		if (currentLevel != LEVEL_ATTRIBUTE)
		{
			logEcho("Must be at ATTRIBUTE level\n");
			return;
		}

		logEcho("Update\n");
		m_cacheAttribute->update(Session::m_moment);
		return;
	}
					
	//****************************************************************************						
	if ((j=theString.get("log",i))!=i)
	{
		if (m_logMode)
		{
			m_logMode = FALSE;			
		}
		else
		{
			int i = theString.trim(j);	
			if (theString.query(i) == str_ConstStr::STR_OTHER)
			{
				int k = 0;
				str_ConstStr::STR_TYPE result;
									
				for (	result = theString.query(k+i); 
						result != str_ConstStr::STR_SEPARATOR && result != str_ConstStr::STR_OUTOFRANGE;
						result  = theString.query(k+i))
							k++;
									
				strncpy(m_logFileName,theString.getChars() + i, k );
				m_logFileName[k] = 0;
									
				sprintf(buffer,"File Name : %s\n",m_logFileName);
				logEcho(buffer);
				m_logMode = TRUE;				
			}
			
			else
			
			logEcho("USAGE log <filename>\n");
		}
		return;
	}
						
	//****************************************************************************							

	if (currentLevel == LEVEL_ATTRIBUTE)	// try to find and change attr
	{
		i = theString.trim(j);		
		if (theString.query(i) != str_ConstStr::STR_OTHER)
		{
			logEcho("Unknown Command\n");
			return;
		}

		int k = 0;
		str_ConstStr::STR_TYPE result;
									
		for (	result = theString.query(k+i); 
				result != str_ConstStr::STR_SEPARATOR && result != str_ConstStr::STR_OUTOFRANGE;
				result  = theString.query(k+i))
					k++;
									
		strncpy(buffer,theString.getChars() + i, k );
		buffer[k] = 0;

		
		ct_AttrItem  *theItem = m_cacheAttribute->searchItem( buffer);
		
		if (!theItem)
		{
			logEcho(buffer);
			logEcho("Attribute Not Found\n");			
			return;
		}

		i += k;
		i = theString.trim(i);

		if ( (j = theString.get("=",i)) == i)
		{
			logEcho(buffer);
			logEcho("'=' expected\n");
			return;
		}

		
		i = theString.trim(j+1);		

		str_ConstStr::STR_TYPE type = theString.query( i );

		switch(theItem->m_type) {
		case EDI_BYTE:
			if (type == str_ConstStr::STR_INT)
				theString.to( * ((unsigned char *) theItem->m_variable), i);
			break;
		case EDI_CHAR:
			if (type == str_ConstStr::STR_OTHER)
				theString.to( *((char *) theItem->m_variable), i);
			break;
		case EDI_INT:
			if (type == str_ConstStr::STR_INT)
				theString.to( *((int *) theItem->m_variable), i);
			break;
		case EDI_UINT:
			if (type == str_ConstStr::STR_INT)
				theString.to( *((unsigned int *) theItem->m_variable), i);
			break;
		case EDI_LONG:
			if (type == str_ConstStr::STR_INT)
				theString.to( *((long *) theItem->m_variable), i);
			break;
		case EDI_ULONG:
			if (type == str_ConstStr::STR_INT)
				theString.to( *((unsigned long *) theItem->m_variable), i);
			break;
		case EDI_SHORT:
			if (type == str_ConstStr::STR_INT)
				theString.to( *((short *) theItem->m_variable), i);
			break;
		case EDI_USHORT:
			if (type == str_ConstStr::STR_INT)
				theString.to( *((unsigned short *) theItem->m_variable), i);
			break;
		case EDI_DOUBLE:
			if (type == str_ConstStr::STR_FLOAT || type == str_ConstStr::STR_INT)
				theString.to( *((double *) theItem->m_variable), i);
			break;
		case EDI_FLOAT:
			if (type == str_ConstStr::STR_FLOAT || type == str_ConstStr::STR_INT)
				theString.to( *((float *) theItem->m_variable), i);
			break;
		case EDI_STR:
			//sprintf(buffer,"%s", (char *) m_variable);
			{
				k = 0;
				str_ConstStr::STR_TYPE result;
				
				for (	result = theString.query(k+i); 
					result != str_ConstStr::STR_SEPARATOR && result != str_ConstStr::STR_OUTOFRANGE;
					result  = theString.query(k+i))
						k++;
				
				char * b = new char [k + 1];

				strncpy(b,theString.getChars() + i, k);
				b[k] = 0;
		
				theItem->m_variable	= (void * ) b;				
			}
			
		} // switch

		char buffer2[128];
		theItem->getString(buffer2);
		sprintf(buffer,"%s [%s] = %s",theItem->m_name, ed_Tag2Msg( theItem->m_type), buffer2);
		logEcho(buffer);
		logEcho("\n");
		return;
	}

	logEcho("Unknown Command\n");
						
	
}
//=========================================================
