#ifndef __SOUND_H__
#define __SOUND_H__

#include <windows.h>
#include <windowsx.h>
#include <olectl.h>

#include <rsx.h>


extern IUnknown* lpRSX2Unk;
extern IRSX2* lpRSX2;

HRESULT InitializeRSX();
void DeInitializeRSX();

extern int snd_engine ;
extern int snd_true3d ;
extern int snd_useDS  ;
extern double snd_distMax ;
extern double snd_distMax2;
extern double snd_engineIntensity;


#endif