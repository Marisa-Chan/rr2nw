#define LAST_H__VIEW
#include "game.h"
#include "storage/h/savefile.h"
#include "kernel/h/context.h"
#include "kernel\h\echo.h"
#include "console.h"


const char CheckString[] = "Next Worlds";

PIN_SaveFile::PIN_SaveFile()
{ 
	m_data = 0; 
	m_hFileMapping = INVALID_HANDLE_VALUE; 
	m_hFile = INVALID_HANDLE_VALUE; 
	strcpy(m_Check, CheckString);
}

char * PIN_SaveFile::GetCurrentData()
{ 
	ASSERT(m_counter < m_length); 
	return & m_data[m_counter]; 
}

int  PIN_SaveFile::Shift(int size)
{ 
	m_counter += size; 
	ASSERT(m_counter < m_length); 
	return m_counter; 
}

int  PIN_SaveFile::GetNextDataItemType()
{
	PIN_SaveItemPrefix * thePrefix = (PIN_SaveItemPrefix *) &m_data[m_counter + sizeof(int)];

	if (strcmp(thePrefix->m_Check,PIN_check) != 0)
	{
		echo("Wrong check");
		return PIN_SaveItemPrefix::IP_ERROR;
	}

	return thePrefix->m_Type;
}

int  PIN_SaveFile::GetData(char * __data, int size) 
{ 
	ASSERT(m_counter + size + sizeof(int) < m_length); 


	int controlSize = * (int *) &m_data[m_counter];

	ASSERT(controlSize == size);
	if (controlSize != size)
		return false;

	m_counter += sizeof(int);
	
	memcpy(__data, &m_data[m_counter], size);
	m_counter += size;

	return m_counter;
}



bool PIN_SaveFile::OpenWrite (char * filename)
{
	m_hFile = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL, NULL);

	if (m_hFile == INVALID_HANDLE_VALUE)
	{		
		return false;
	}


	if (!WriteHeader())
		return false;

	return true;
}


void PIN_SaveFile::Close()
{
	
	if (m_data)
	{
		if (!UnmapViewOfFile(m_data))
		 echo("Error UnMapping");
	}
	
	m_data = 0;
	
	if (m_hFileMapping != INVALID_HANDLE_VALUE)
	{
		if (!CloseHandle(m_hFileMapping))
			echo("Error closing Mapping");
	}
	
	CloseHandle(m_hFile);
	
	m_hFileMapping  = INVALID_HANDLE_VALUE;
	m_hFile		= INVALID_HANDLE_VALUE;
}


bool PIN_SaveFile::WriteData(char * __data, int size)
{
	DWORD written;


	bool result = WriteFile(m_hFile, (char *) & size, sizeof(int) ,&written, NULL);
	
	if (!result || written != sizeof(int))
	{
		ASSERT(0);
		return false;
	}


	result = WriteFile(m_hFile, __data, size,&written, NULL);

	if (!result || written != size)
		return false;

	return true;
}

bool PIN_SaveFile::WriteHeader()
{
	/*DWORD written;

	bool result = WriteFile(m_hFile, this, sizeof(PIN_SaveFileHeader),&written, NULL);

	if (!result || written != sizeof(PIN_SaveFileHeader))
		return false;*/

	return WriteData(m_Check, sizeof(PIN_SaveFileHeader) );
}


bool PIN_SaveFile::OpenRead(char * filename)
{

	m_counter = 0;
	m_length  = 0;

	m_hFile = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL, NULL);

	if (m_hFile == INVALID_HANDLE_VALUE)
	{		
		return false;
	}

	m_hFileMapping = CreateFileMapping(m_hFile, NULL, PAGE_WRITECOPY, 0, 0, NULL);

	if (!m_hFileMapping)
	{
		CloseHandle(m_hFile);
		return false;
	}

	DWORD fileSizeHigh;

	m_length = GetFileSize(m_hFile, & fileSizeHigh);


	m_data = (char * ) MapViewOfFile(m_hFileMapping, FILE_MAP_COPY, 0, 0, 0);

	if (m_data == NULL)
	{
		CloseHandle(m_hFileMapping);
		CloseHandle(m_hFile);

		return false;
	}

	//memcpy(this, m_data, sizeof(PIN_SaveFileHeader)); // to fill out check structures.

	if (!GetData(m_Check, sizeof(PIN_SaveFileHeader) ) ||
		  strcmp(m_Check, CheckString) != 0)
	{
		CloseHandle(m_hFileMapping);
		CloseHandle(m_hFile);
		return false;
	}
	
	return true;
}




bool SaveGame(char *fileName, SimulationContext * context)
{
	PIN_SaveFile theFile;

	if (!theFile.OpenWrite(fileName))
		return false;


	VERIFY(context->dump(theFile));


	theFile.Close();

	return true;

	//return LoadGame(fileName, context);
}


bool LoadGame(char *fileName, SimulationContext * context)
{
	g_GameConsole.DeleteUrgentMessge();
	PIN_SaveFile theFile;

	if (!theFile.OpenRead(fileName))
		return false;


	VERIFY(context->load(theFile));

	theFile.Close();

	return true;
}
