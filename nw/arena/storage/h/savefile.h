#ifndef __SAVEFILE_H
#define __SAVEFILE_H

#include "kernel/h/object.h"
#include "storage/h/classtab.h"


#define PIN_check "PIN"


typedef struct {	
	enum {IP_CONTEXT, IP_EVENT, IP_OBJECT, IP_BRANCH, IP_FINITALACOMEDIA , IP_ERROR};
	char	m_Check[4];	
	int		m_Type;
} PIN_SaveItemPrefix;


typedef struct {

	char	m_ClassTableName [MAX_CLASS_NAME_LEN+1];
	char	m_ObjectName	 [MAX_CLASS_NAME_LEN+1];
	int		m_DataSize;

} PIN_ObjectPrefix;



typedef struct {

	char m_Check[15];
	int	 m_Level;

} PIN_SaveFileHeader;



class PIN_SaveFile : public PIN_SaveFileHeader {

char	* m_data;
int		m_counter;
int		m_length;


HANDLE m_hFile;
HANDLE m_hFileMapping;


public:

	PIN_SaveFile(); 
	~PIN_SaveFile() { Close(); }

	bool OpenRead  (char * filename);
	bool OpenWrite (char * filename);
	void Close();

	char * GetCurrentData();
	int  Shift(int size);

	int  GetNextDataItemType();
	
	int  GetData(char * __data, int size) ;
	bool WriteHeader();
	bool WriteData(char * data, int size);
};



bool SaveGame(char *fileName, SimulationContext * context);
bool LoadGame(char *fileName, SimulationContext * context);

#endif