#define LAST_H__VIEW
#include "game.h"
#include "scene.h"
#include "zav.h"

#include "stdtypes.h"
#include "debugext.h"
#include "filesys.h"
#include "mathlib.h"
#include "graph.h"
#include "patterns.h"
#include "view.h"

#include "sound.h"
#include "wav.h"




IRSX2              * lpRSX2    = NULL;
IUnknown           * lpRSX2Unk = NULL;


int    snd_engine   = 0;
int    snd_true3d          = 0;
int    snd_useDS           = 0;
double snd_distMax         = 100;
double snd_distMax2 	   = 0;
double snd_engineIntensity = 0;



HRESULT InitializeRSX()
{
       
    int enable = GLOBAL_Config().GetInt("Sound","Enabled",1);
    if (!enable)
	return 0;


    snd_engine   = GLOBAL_Config().GetInt("Sound","Engine",1);
    snd_true3d   = GLOBAL_Config().GetInt("Sound","True3d",1);
    snd_useDS    = GLOBAL_Config().GetInt("Sound","UseDS",1);
    snd_distMax  = GLOBAL_Config().GetDouble("Sound","DistMax",100.);
    snd_engineIntensity  = GLOBAL_Config().GetDouble("Sound","EngineIntensity",0.3);
    snd_distMax2 = snd_distMax * snd_distMax;
	
      	
    HRESULT hr = CoCreateInstance(  CLSID_RSX20, NULL, CLSCTX_INPROC_SERVER,
                                    IID_IUnknown, (void **) &lpRSX2Unk);


    if ( FAILED(hr) || !lpRSX2Unk )
     return hr;

     
    hr = lpRSX2Unk->QueryInterface(IID_IRSX2, (void**)& lpRSX2);

    if ( SUCCEEDED(hr) && lpRSX2 )
    {
            RSXENVIRONMENT rsxEnv;
            // Set right-handed coordinate system.
            rsxEnv.cbSize = sizeof(RSXENVIRONMENT);
            rsxEnv.dwFlags = RSXENVIRONMENT_COORDINATESYSTEM;
            rsxEnv.bUseRightHand = TRUE;
            lpRSX2->SetEnvironment(&rsxEnv);
    }
	else
    {
	if (lpRSX2)
	{
		lpRSX2->Release();
		lpRSX2 = NULL;
	}
    }


    return hr;
}


void DeInitializeRSX()
{
     if (lpRSX2)
     {
	     lpRSX2->Release();		
	     lpRSX2Unk->Release();		
     }
}