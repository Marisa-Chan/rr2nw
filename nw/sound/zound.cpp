//#define INITGUID  //MUST precede everything



#include "stdtypes.h"
#include "debugext.h"
#include "filesys.h"
#include "mathlib.h"
#include "graph.h"
#include "patterns.h"
#include "view.h"

#include "sound.h"
#include "wav.h"


IRSXDirectListener * lpDL;       // Direct Listener
IRSX2              * lpRSX2;
IUnknown           * lpRSX2Unk;
IRSXCachedEmitter  * lpCE;        // Cached Emitter

RSXBUFFERHDR RSXBuffer;


HRESULT InitializeRSX()
{

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


    return hr;
}


void DeInitializeRSX()
{
     ReleaseListener();
     lpRSX2->Release();		
     lpRSX2Unk->Release();		
}



HRESULT CreateListener()
{

    HRESULT hr = CoCreateInstance(
		CLSID_RSXDIRECTLISTENER,		// GUID for direct listener object
		NULL,							// the rest of the params are the same
        CLSCTX_INPROC_SERVER,           // as before
		IID_IRSXDirectListener,
        (void ** ) &lpDL);


    if ( FAILED(hr) || !lpDL)
    {
        return hr;
    }

    // Now that the object is created we have to initialize it first
    //

    RSXDIRECTLISTENERDESC rsxDL;            // listener description
    ZeroMemory(&rsxDL, sizeof(RSXDIRECTLISTENERDESC));

    // fill up the directlistener settings for the initialize call
    rsxDL.cbSize = sizeof(RSXDIRECTLISTENERDESC);
    rsxDL.hMainWnd = 0; //_gr_hWnd;
    rsxDL.dwUser = 0;
    rsxDL.lpwf = NULL;

    hr = lpDL->Initialize(&rsxDL, lpRSX2Unk);

    if ( FAILED(hr))
    {
        return hr;
    }

// Now that our direct listener is running we can
// start adding sound sources - emitters
// In this sample we will only have 2
// but you can in reality add as many
// as your computer can handle (CPU and/or memory)
// And the great thing is the two sounds will automatically
// be mixed together
//
// Create a Cachedemitter - a cached emitter is an emitter
// based on a wave file found either on a local or networked
// hard drive, as a resource in an .exe or .dll or
// it can be based on an Internet URL.

    hr = CoCreateInstance(
            CLSID_RSXCACHEDEMITTER,     // GUID for cachedemitter object
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_IRSXCachedEmitter,
            (void ** )&lpCE);


    // see if it passed
    //
    if ( FAILED(hr) || !lpCE)
    {
        return hr;
    }

    RSXCACHEDEMITTERDESC rsxCE;             // emitter description

    ZeroMemory(&rsxCE, sizeof(RSXCACHEDEMITTERDESC));
    rsxCE.cbSize = sizeof(RSXCACHEDEMITTERDESC);
    rsxCE.dwFlags = RSXEMITTERDESC_NODOPPLER | RSXEMITTERDESC_NOREVERB | RSXEMITTERDESC_PREPROCESS | RSXEMITTERDESC_INMEMORY;
    rsxCE.dwUser = 50;

    strcpy(rsxCE.szFilename,"bird.wav");

    // initialize the CachedEmitter and 'attach' it to
    // the same RSX20 object as the DirectListener
    // otherwise we won't hear anything

    hr = lpCE->Initialize(&rsxCE, lpRSX2Unk);

    if ( FAILED(hr))
    {
        return hr;
    }


    // Now we should set the emitter model
    RSXEMITTERMODEL rsxEModel;
    rsxEModel.cbSize = sizeof(RSXEMITTERMODEL);

    // This defines the inner ellipsoid of the
    // emitter.  Sound is ambient in this region
    rsxEModel.fMinFront = 1.0f;
    rsxEModel.fMinBack = 1.0f;

    // This defines the outer ellipsoid of the emitter
    // Sound is localized between the edge of the inner
    // ellipsoid and the outer ellipse. If the listener
    // position is outside of this ellipsoid the emitter
    // can not be heard.
    rsxEModel.fMaxFront = 20.0f;
    rsxEModel.fMaxBack = 20.0f;

    // The intensity (volume) of the emitter
    // Generally between 0.00f and 1.00f
    rsxEModel.fIntensity = 0.1f;

    // Actually set the model
    lpCE->SetModel(&rsxEModel);


    return hr;
}


void PositionListener()
{
    CFVector3 pos = CViewObject::m_viewPointInvMx.Offset();
    RSXVECTOR3D v3d, dir, up;

    v3d.x = pos.x;
    v3d.y = pos.y;
    v3d.z = pos.z;

    lpCE->SetPosition(&v3d);
    lpDL->SetPosition(&v3d);

    CFVector3 front = CViewObject::m_viewPointInvMx.Column(2);
    CFVector3 upper = CViewObject::m_viewPointInvMx.Column(1);

    dir.x = float(front.x);	
    dir.y = float(front.y);	
    dir.z = float(front.z);	

    up.x  = float(upper.x);
    up.y  = float(upper.y);
    up.z  = float(upper.z);


    lpDL->SetOrientation(&dir, &up);
}

void Sing()
{
    //lpCE->ControlMedia(RSX_PLAY, 0, 0);
    //lpSE->SubmitBuffer(& RSXBuffer);
}

void ShutUp()
{
    lpCE->ControlMedia(RSX_STOP , 0, 0);
    //lpSE->Flush();
}

void ReleaseListener()
{
        ShutUp();

        if(lpCE){
            lpCE->Release();
            lpCE = NULL;
        } // if

        /*if(lpSE){
            lpSE->Release();
            lpSE = NULL;
        } // if*/


        if(lpDL){
            lpDL->Release();
            lpDL = NULL;
        } // if
}


/*void StopFarting()
{
    lpFarter->ControlMedia(RSX_STOP , 0, 0);
} */
