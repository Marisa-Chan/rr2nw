#define LAST_H__VIEW
#include "game.h"
#include "scene.h"
#include "zav.h"
#include "i/staticobj.i"
#include "kernel/h/session.h"
#include "kernel/h/echo.h"
#include "storage/h/subject.h"
#include "StaticObj.h"
#include "message/fountmsg.h"

#define RAD 0.017453292

//double a = ZAV_Config().GetDouble("Conv", "a");

//------------------------------------------------------------------------------
//  Level.1
//------------------------------------------------------------------------------   

void Htk_gunCallback( CViewObjectBaseSet *pBaseSet,
                      CViewObjectBase    *pBase,
                      CViewObjectRef     *pRef
                    )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
	int    time =  (int)(Session::m_viewTime / 30.0 );
        double ph   =  Session::m_viewTime - time * 30.0;
	double angle, gAngle, scale1 = 0.0, scale2 = 0.0; 

	if (ph < 3){
	 angle = 3.14 + (ph - 3)*3.14/3.;
	 gAngle = 0.;
	}else
	if (ph < 15){ // 1
	 angle = 3.14;
	 gAngle = 50 * 3.14 / 180.;
	 if (ph < 5){
	  gAngle = 50 * 3.14 / 180. - (5 - ph) * 50 * 3.14 /180. /2.;
	 }else
	  if (ph > 13)
	   gAngle = 50 * 3.14 / 180. + (13 - ph) * 50 * 3.14 /180. /2.;
	  else{
	   if (ph < 5.1)
	    scale1 = (5 - ph) / 0.2;
           else
	    if (ph < 6)
	     scale1 = (ph - 6) / 1.8;
            else
	     if (ph < 6.1)
	    scale2 = (6 - ph) / 0.2;
           else
	    if (ph < 7)
	     scale2 = (ph - 7) / 1.8;	
	  }
	}
	else
        if (ph < 18){
	 angle = - (ph - 18)*3.14/3.;
         gAngle = 0.0;
	}else
	if (ph < 30){ // 2
         angle = 0.;
	 gAngle = 70 * 3.14 / 180.;
	 if (ph < 20)
	    gAngle = 70 * 3.14 / 180. - (20 - ph) * 70 * 3.14 /180. /2.;
	 else
	 if (ph > 28)
	  gAngle = 70 * 3.14 / 180. + (28 - ph) * 70 * 3.14 /180. /2.;	
	 else{
	   if (ph < 20.1)
	    scale1 = (20 - ph) / 0.2;
           else
	    if (ph < 21)
	     scale1 = (ph - 21) / 1.8;
            else
	     if (ph < 21.1)
	    scale2 = (21 - ph) / 0.2;
           else
	    if (ph < 22)
	     scale2 = (ph - 22) / 1.8;	
	  }
	}
    
    pData->vbmi0->LoadIdentity()
      .RotateOy(angle, pData->axis0)
      .Update();

    pData->vbmi1->LoadIdentity()
      .RotateOx(-gAngle, pData->axis1)
      .RotateOy(angle, pData->axis0)
      .Update();

    pData->vbmi2->LoadIdentity()
      .Translate( scale1 * pData->axis2)
      .RotateOx(-gAngle, pData->axis1)
      .RotateOy(angle, pData->axis0)
      .Update();
   
    pData->vbmi3->LoadIdentity()
      .RotateOx(-gAngle, pData->axis1)
      .RotateOy(angle, pData->axis0)
      .Update();

    pData->vbmi4->LoadIdentity()
      .Translate( scale2 * pData->axis2)
      .RotateOx(-gAngle, pData->axis1)
      .RotateOy(angle, pData->axis0)
      .Update();	
}

void Door_Callback( CViewObjectBaseSet *pBaseSet,
                    CViewObjectBase    *pBase,
                    CViewObjectRef     *pRef
                  )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
        int    time =  (int)(Session::m_viewTime / 60.0 );
        double ph   =  Session::m_viewTime - time * 60.0;
        double scale;

        if ((ph < 10) || (ph > 40))
         scale = 0.0;
        else
        {
        if (ph < 10.5)
          scale = (ph - 10) / 0.5;
        else
          if (ph > 39.5)
           scale = (40 - ph) / 0.5; 
	  else 
           scale = 1.; 
	}
    
    pData->vbmi0->LoadIdentity()
      .Translate(scale * pData->axis0)
      .Update();
}

void Pol_02Callback( CViewObjectBaseSet *pBaseSet,
                     CViewObjectBase    *pBase,
                     CViewObjectRef     *pRef
                   )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
	int    time =  (int)(Session::m_viewTime / 30.0 );
        double ph   =  Session::m_viewTime - time * 30.0;
	double angle, gAngle, scale1 = 0.0, scale2 = 0.0; 

	if (ph < 3){
	 angle = 3.14 + (ph - 3)*3.14/3.;
	 gAngle = 0.;
	}else
	if (ph < 15){ // 1
	 angle = 3.14;
	 gAngle = 50 * 3.14 / 180.;
	 if (ph < 5){
	  gAngle = 50 * 3.14 / 180. - (5 - ph) * 50 * 3.14 /180. /2.;
	 }else
	  if (ph > 13)
	   gAngle = 50 * 3.14 / 180. + (13 - ph) * 50 * 3.14 /180. /2.;
	  else{
	   if (ph < 5.1)
	    scale1 = (5 - ph) / 0.4;
           else
	    if (ph < 6)
	     scale1 = (ph - 6) / 3.6;
            else
	     if (ph < 6.1)
	    scale2 = (6 - ph) / 0.4;
           else
	    if (ph < 7)
	     scale2 = (ph - 7) / 3.6;	
	  }
	}
	else
        if (ph < 18){
	 angle = - (ph - 18)*3.14/3.;
         gAngle = 0.0;
	}else
	if (ph < 30){ // 2
         angle = 0.;
	 gAngle = 70 * 3.14 / 180.;
	 if (ph < 20)
	    gAngle = 70 * 3.14 / 180. - (20 - ph) * 70 * 3.14 /180. /2.;
	 else
	 if (ph > 28)
	  gAngle = 70 * 3.14 / 180. + (28 - ph) * 70 * 3.14 /180. /2.;	
	 else{
	   if (ph < 20.1)
	    scale1 = (20 - ph) / 0.4;
           else
	    if (ph < 21)
	     scale1 = (ph - 21) / 3.6;
            else
	     if (ph < 21.1)
	    scale2 = (21 - ph) / 0.4;
           else
	    if (ph < 22)
	     scale2 = (ph - 22) / 3.6;	
	  }
	}
    
    pData->vbmi0->LoadIdentity()
      .RotateOy(angle, pData->axis0)
      .Update();

    pData->vbmi1->LoadIdentity()
      .RotateOx(-gAngle, pData->axis1)
      .RotateOy(angle, pData->axis0)
      .Update();

    pData->vbmi2->LoadIdentity()
      .Translate( scale1 * pData->axis2)
      .RotateOx(-gAngle, pData->axis1)
      .RotateOy(angle, pData->axis0)
      .Update();
   
    pData->vbmi3->LoadIdentity()
      .Translate( scale2 * pData->axis2)
      .RotateOx(-gAngle, pData->axis1)
      .RotateOy(angle, pData->axis0)
      .Update();	
}

void Rotate_Callback( CViewObjectBaseSet *pBaseSet,
                      CViewObjectBase    *pBase,
                      CViewObjectRef     *pRef
                    )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
     pData->vbmi0->LoadIdentity()
      .RotateOy(Session::m_viewTime * pData->speed + pData->phase, pData->axis0)
      .Update();
}

void Flags_Callback( CViewObjectBaseSet *pBaseSet,
                     CViewObjectBase    *pBase,
                     CViewObjectRef     *pRef
                   )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
     pData->vbmi1->LoadIdentity()
      .Translate(0.8 * (-5 + 2 * sin(Session::m_viewTime*pData->speed + pData->phase + 3.14 * 0.2)) * pData->axis1)	
      .Translate(0.9 * sin(Session::m_viewTime*pData->speed + pData->phase + 3.14 * 0.5) * pData->axis0);
     
     pData->vbmi2->LoadIdentity()
      .Translate((-5.5 + 2.5 * sin(Session::m_viewTime*pData->speed + pData->phase + 3.14 * 0.8)) * pData->axis1)   
      .Translate(sin(Session::m_viewTime*pData->speed + pData->phase + 3.14 * 0.65) * pData->axis0);
     
     pData->vbmi3->LoadIdentity()
      .Translate(1.1 * (-7.5 + 4.5 * sin(Session::m_viewTime*pData->speed + pData->phase + 3.14 * 0.2)) * pData->axis1)   	
      .Translate(1.1 * sin(Session::m_viewTime*pData->speed + pData->phase) * pData->axis0);
        
     pData->vbmi4->LoadIdentity()
      .Translate((-10 + 5.5 * sin(Session::m_viewTime*pData->speed + pData->phase + 3.14 * 0.4)) * pData->axis1)	
      .Translate(sin(Session::m_viewTime*pData->speed + pData->phase + 3.14 * 0.35) * pData->axis0);
         
     pData->vbmi0->LoadIdentity()
      .Update();
}

void Pol_16Callback( CViewObjectBaseSet *pBaseSet,
                     CViewObjectBase    *pBase,
                     CViewObjectRef     *pRef
                   )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
	int    time =  (int)((Session::m_viewTime + pData->phase) / 7.0 );
        double ph   =  Session::m_viewTime + pData->phase - time * 7.0;
	double angle; 

	if (ph < 1)
	 angle = 50 * 3.14 /180. + (ph - 1) * 50 * 3.14 /180.;
	else
	if (ph < 3) // 1
	 angle = 150 * 3.14 /180. + (ph - 3) * 100 * 3.14 /180. / 2.;
	else
        if (ph < 6)
	 angle = 150 * 3.14 /180. - (ph - 3) * 270 * 3.14 /180. / 3.;
	else
	if (ph < 7) // 2
         angle = -120 * 3.14 /180. + (ph - 6) * 120 * 3.14 /180.;
    
    pData->vbmi0->LoadIdentity()
      .RotateOy(angle, pData->axis0)
      .Update();

    pData->vbmi1->LoadIdentity()
      .RotateOx(Session::m_viewTime * pData->speed + pData->phase, pData->axis1)
      .RotateOy(angle, pData->axis0)
      .Update();

    pData->vbmi2->LoadIdentity()
      .RotateOx(-(Session::m_viewTime * pData->speed + pData->phase), pData->axis1)
      .RotateOy(angle, pData->axis0)
      .Update();
}   

void g_staticInit1()
{                                       
    CNameDecls &refNames = ZAV_Scene()->ObjRefNames();  // objects' names
    ct_ClassTableID  ctID = g_arena.searchSeanceClassTable("StaticObj");

    CNameDecl     &Flag_flyArr = refNames["flag_fly"];
    int       nFlag_flyCount = Flag_flyArr.Count();

    for(int i = 0 ; i < nFlag_flyCount ; i++ )
    {
         CViewObjectRef  *pFlag_fly = (CViewObjectRef*)Flag_flyArr[i];
         CViewObjectBase &pBase = pFlag_fly->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Flag_fly");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

	 data->speed = g_arena.context->rnd_f(6,8);
         data->phase = g_arena.context->rnd_f(5);

         data->axis0 = *(CFVector3*)pBase.Names().vertices["Axis0"][0]
		       -*(CFVector3*)pBase.Names().vertices["Axis0"][1]; 

         data->axis1 = (*(CFVector3*)pBase.Names().vertices["Axis1"][0]
		       -*(CFVector3*)pBase.Names().vertices["Axis1"][1]) * 0.2; 

	 data->vbmi0 = &pBase.KFSet().Mod0("Planes");
	 data->vbmi1 = &pBase.KFSet().Mod0("Point1");       
	 data->vbmi2 = &pBase.KFSet().Mod0("Point2");	
         data->vbmi3 = &pBase.KFSet().Mod0("Point3");       
	 data->vbmi4 = &pBase.KFSet().Mod0("Point4");	

         pFlag_fly->SetAnimationCallback(Flags_Callback);
         pFlag_fly->SetUserAttrib(data);
    }

    CNameDecl     &Flag_rbtArr = refNames["flag_rbt"];
    int       nFlag_rbtCount = Flag_rbtArr.Count();

    for( int i = 0 ; i < nFlag_rbtCount ; i++ )
    {
         CViewObjectRef  *pFlag_rbt = (CViewObjectRef*)Flag_rbtArr[i];
         CViewObjectBase &pBase = pFlag_rbt->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Flag_rbt");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

	 data->speed = g_arena.context->rnd_f(6,8);
         data->phase = g_arena.context->rnd_f(5);

         data->axis0 = *(CFVector3*)pBase.Names().vertices["Axis0"][0]
		       -*(CFVector3*)pBase.Names().vertices["Axis0"][1]; 

         data->axis1 = (*(CFVector3*)pBase.Names().vertices["Axis1"][0]
		       -*(CFVector3*)pBase.Names().vertices["Axis1"][1]) * 0.2; 

	 data->vbmi0 = &pBase.KFSet().Mod0("Planes");
	 data->vbmi1 = &pBase.KFSet().Mod0("Point1");       
	 data->vbmi2 = &pBase.KFSet().Mod0("Point2");	
         data->vbmi3 = &pBase.KFSet().Mod0("Point3");       
	 data->vbmi4 = &pBase.KFSet().Mod0("Point4");	

         pFlag_rbt->SetAnimationCallback(Flags_Callback);
         pFlag_rbt->SetUserAttrib(data);
    }	

    CNameDecl     &Flag_tnkArr = refNames["flag_tnk"];
    int       nFlag_tnkCount = Flag_tnkArr.Count();

    for( int i = 0 ; i < nFlag_tnkCount ; i++ )
    {
         CViewObjectRef  *pFlag_tnk = (CViewObjectRef*)Flag_tnkArr[i];
         CViewObjectBase &pBase = pFlag_tnk->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Flag_tnk");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

	 data->speed = g_arena.context->rnd_f(6,8);
         data->phase = g_arena.context->rnd_f(5);

         data->axis0 = *(CFVector3*)pBase.Names().vertices["Axis0"][0]
		       -*(CFVector3*)pBase.Names().vertices["Axis0"][1]; 

         data->axis1 = (*(CFVector3*)pBase.Names().vertices["Axis1"][0]
		       -*(CFVector3*)pBase.Names().vertices["Axis1"][1]) * 0.2; 

	 data->vbmi0 = &pBase.KFSet().Mod0("Planes");
	 data->vbmi1 = &pBase.KFSet().Mod0("Point1");       
	 data->vbmi2 = &pBase.KFSet().Mod0("Point2");	
         data->vbmi3 = &pBase.KFSet().Mod0("Point3");       
	 data->vbmi4 = &pBase.KFSet().Mod0("Point4");	

         pFlag_tnk->SetAnimationCallback(Flags_Callback);
         pFlag_tnk->SetUserAttrib(data);
    }
    
    CNameDecl  &Pol_02Arr = refNames["pol_02"];      // array of objects named "mill"
    int       nPol_02Count = Pol_02Arr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nPol_02Count ; i++ ) 
    {
         CViewObjectRef  *pPol_02 = (CViewObjectRef*)Pol_02Arr[i];
         CViewObjectBase &pBase = pPol_02->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Pol_02");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               ) * 0.5;

         data->axis1 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis1"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis1"][1]
               ) * 0.5;

	 data->axis2 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis2"][1]
               -*(CFVector3*)pBase.Names().vertices["Axis2"][0]
               );

         data->vbmi0 = &pBase.KFSet().Mod0("Body");
         data->vbmi1 = &pBase.KFSet().Mod0("Guns");
         data->vbmi2 = &pBase.KFSet().Mod0("TGuns");
         data->vbmi3 = &pBase.KFSet().Mod0("BGuns");

         pPol_02->SetAnimationCallback(Pol_02Callback);
         pPol_02->SetUserAttrib(data);
    }
    
    CNameDecl     &Pol_13Arr = refNames["pol_13"];
    int       nPol_13Count = Pol_13Arr.Count();

    for( int i = 0 ; i < nPol_13Count ; i++ )
    {
         CViewObjectRef  *pPol_13 = (CViewObjectRef*)Pol_13Arr[i];
         CViewObjectBase &pBase = pPol_13->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Pol_13");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

	 data->speed = g_arena.context->rnd_f(1,2);
         data->phase = g_arena.context->rnd_f(5);

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               ) * 0.5;

         data->vbmi0 = &pBase.KFSet().Mod0("Body");

         pPol_13->SetAnimationCallback(Rotate_Callback);
         pPol_13->SetUserAttrib(data);
    }

    CNameDecl     &Twn_pikeArr = refNames["twn_pike"];
    int       nTwn_pikeCount = Twn_pikeArr.Count();

    for( int i = 0 ; i < nTwn_pikeCount ; i++ )
    {
         CViewObjectRef  *pTwn_pike = (CViewObjectRef*)Twn_pikeArr[i];
         CViewObjectBase &pBase = pTwn_pike->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Twn_pike");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

	 data->speed = g_arena.context->rnd_f(0.5,1);
         data->phase = g_arena.context->rnd_f(5);

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               ) * 0.5;

         data->vbmi0 = &pBase.KFSet().Mod0("Body");

         pTwn_pike->SetAnimationCallback(Rotate_Callback);
         pTwn_pike->SetUserAttrib(data);
    }

    CNameDecl     &Slo_06aArr = refNames["slo_06a"];
    int       nSlo_06aCount = Slo_06aArr.Count();

    for( int i = 0 ; i < nSlo_06aCount ; i++ )
    {
         CViewObjectRef  *pSlo_06a = (CViewObjectRef*)Slo_06aArr[i];
         CViewObjectBase &pBase = pSlo_06a->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Slo_06a");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

	 data->speed = g_arena.context->rnd_f(0.5,2);
         data->phase = g_arena.context->rnd_f(5);

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               ) * 0.5;

         data->vbmi0 = &pBase.KFSet().Mod0("Body");

         pSlo_06a->SetAnimationCallback(Rotate_Callback);
         pSlo_06a->SetUserAttrib(data);
    }

    CNameDecl     &Pol_03Arr = refNames["pol_03"];
    int       nPol_03Count = Pol_03Arr.Count();

    for( int i = 0 ; i < nPol_03Count ; i++ )
    {
         CViewObjectRef  *pPol_03 = (CViewObjectRef*)Pol_03Arr[i];
         CViewObjectBase &pBase = pPol_03->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Pol_03");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               -*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               );

         data->vbmi0 = &pBase.KFSet().Mod0("Door");

         pPol_03->SetAnimationCallback(Door_Callback);
         pPol_03->SetUserAttrib(data);
    }
    
   /* CNameDecl     &Pol_04Arr = refNames["pol_04"];
    int       nPol_04Count = Pol_04Arr.Count();

    for( i = 0 ; i < nPol_04Count ; i++ )
    {
         CViewObjectRef  *pPol_04 = (CViewObjectRef*)Pol_04Arr[i];
         CViewObjectBase &pBase = pPol_04->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Pol_04");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               -*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               );

         data->vbmi0 = &pBase.KFSet().Mod0("Door");

         pPol_04->SetAnimationCallback(Door_Callback);
         pPol_04->SetUserAttrib(data);
    }
    */
    CNameDecl     &tel_00Arr = refNames["tel_00"];
    int       ntel_00Count = tel_00Arr.Count();

    for( int i = 0 ; i < ntel_00Count ; i++ )
    {
         CViewObjectRef  *ptel_00 = (CViewObjectRef*)tel_00Arr[i];
         CViewObjectBase &pBase = ptel_00->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.tel_00");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][1]
               -*(CFVector3*)pBase.Names().vertices["Axis0"][0]
               );

         data->vbmi0 = &pBase.KFSet().Mod0("Door");

         ptel_00->SetAnimationCallback(Door_Callback);
         ptel_00->SetUserAttrib(data);
    }

    CNameDecl     &Pol_16Arr = refNames["pol_16"];
    int       nPol_16Count = Pol_16Arr.Count();

    for( int i = 0 ; i < nPol_16Count ; i++ )
    {
         CViewObjectRef  *pPol_16 = (CViewObjectRef*)Pol_16Arr[i];
         CViewObjectBase &pBase = pPol_16->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Pol_16");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->phase = g_arena.context->rnd_f(5);
	 data->speed = g_arena.context->rnd_f(2,4);

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Body"][0]
               +*(CFVector3*)pBase.Names().vertices["Body"][1]
               ) * 0.5;

	 data->axis1 =
               (
                *(CFVector3*)pBase.Names().vertices["Arms"][0]
               +*(CFVector3*)pBase.Names().vertices["Arms"][1]
               ) * 0.5;

         data->vbmi0 = &pBase.KFSet().Mod0("Body");
	 data->vbmi1 = &pBase.KFSet().Mod0("LArm");
	 data->vbmi2 = &pBase.KFSet().Mod0("RArm");

         pPol_16->SetAnimationCallback(Pol_16Callback);
         pPol_16->SetUserAttrib(data);
    }

    CNameDecl  &Htk_gunArr = refNames["htk_gun"];      // array of objects named "mill"
    int       nHtk_gunCount = Htk_gunArr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nHtk_gunCount ; i++ ) 
    {
         CViewObjectRef  *pHtk_gun = (CViewObjectRef*)Htk_gunArr[i];
         CViewObjectBase &pBase = pHtk_gun->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Htk_gun");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               ) * 0.5;

         data->axis1 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis1"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis1"][1]
               ) * 0.5;

	 data->axis2 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis2"][1]
               -*(CFVector3*)pBase.Names().vertices["Axis2"][0]
               );

         data->vbmi0 = &pBase.KFSet().Mod0("Base");
         data->vbmi1 = &pBase.KFSet().Mod0("Gun1");
         data->vbmi2 = &pBase.KFSet().Mod0("Gun11");
         data->vbmi3 = &pBase.KFSet().Mod0("Gun2");
         data->vbmi4 = &pBase.KFSet().Mod0("Gun21");

         pHtk_gun->SetAnimationCallback(Htk_gunCallback);
         pHtk_gun->SetUserAttrib(data);
    }

}

//------------------------------------------------------------------------------
//  Level.2
//------------------------------------------------------------------------------   

void Bld_a03Callback( CViewObjectBaseSet *pBaseSet,
                      CViewObjectBase    *pBase,
                      CViewObjectRef     *pRef
                    )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
	int    time =  (int)(Session::m_viewTime / 30.0 );
        double ph   =  Session::m_viewTime - time * 30.0;

        double shot   =  Session::m_viewTime - (int)(Session::m_viewTime);     
        double gunAngle = 0.;
        double Scale = 0.0;

        if (ph < 3)
         gunAngle = 30 * 3.14 /180. + (ph - 3)*30 * 3.14 / 180. /3.;
        else
        if (ph < 20){ 
         gunAngle = 30 * 3.14 / 180.;

         if (shot < 0.15)
          Scale =  -shot / 0.9;
         else if (shot < 0.5)
          Scale =  (shot - 0.5) / 2.7;

        }else
          if (ph < 23)
             gunAngle = 30 * 3.14 / 180. + (20 - ph) * 30 * 3.14 /180. /3.;
    
    pData->vbmi0->LoadIdentity()
      .Translate(Scale * pData->axis1)  
      .RotateOx(gunAngle, pData->axis0)
      .Update();
}

void Bld_a02Callback( CViewObjectBaseSet *pBaseSet,
                      CViewObjectBase    *pBase,
                      CViewObjectRef     *pRef
                    )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
	int    time =  (int)(Session::m_viewTime / 30.0 );
        double ph   =  Session::m_viewTime - time * 30.0;

        int    shottime =  (int)(Session::m_viewTime / 0.25 );
        double shot   =  Session::m_viewTime - shottime * 0.25;
        
        double vorotAngle = 0.0;
        double gunsAngle = 0.;
        double porshScale = 0.0;

        if (ph < 3)
         vorotAngle = 10 * 3.14 /180. + (ph - 3)*10 * 3.14 / 180. /3.;
        else
        if (ph < 20){ 
         vorotAngle = 10 * 3.14 / 180.;

         gunsAngle = shot * 90 * 3.14 / 180 / 0.25;

         if (shot < 0.125)
          porshScale =  shot / 0.15;
         else
          porshScale =  (0.25 - shot) / 0.15;
        }else
          if (ph < 23)
             vorotAngle = 10 * 3.14 / 180. + (20 - ph) * 10 * 3.14 /180. /3.;
    
    pData->vbmi0->LoadIdentity()
      .RotateOx(-vorotAngle, pData->axis0)
      .Update();

    pData->vbmi1->LoadIdentity()
      .Translate(porshScale * pData->axis2)  
      .RotateOx(-vorotAngle, pData->axis0)
      .Update();

    pData->vbmi2->LoadIdentity()
      .RotateOz(gunsAngle, pData->axis1)
      .RotateOx(-vorotAngle, pData->axis0)
      .Update();
}

void Bld_v00Callback( CViewObjectBaseSet *pBaseSet,
                      CViewObjectBase    *pBase,
                      CViewObjectRef     *pRef
                    )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
        int    time =  (int)(Session::m_viewTime / 6.0 );
        double ph   =  Session::m_viewTime - time * 6.0;
        double angle;

        if ((ph < 1) || (ph > 4))
         angle = 0.0;
        else
        {
        if (ph < 1.5)
          angle = 3.14 * 90 / 180. - (1.5 - ph) * 3.14 * 90 / 180./ .5;
        else
          if (ph > 3.5)
           angle = 3.14 * 90 / 180. + (3.5 - ph) * 3.14 * 90 / 180. / .5; 
	  else 
           angle = 3.14 * 90 / 180.; 
	}
    
    pData->vbmi0->LoadIdentity()
      .RotateOy(angle, pData->axis0)
      .Update();

    pData->vbmi1->LoadIdentity()
      .RotateOy(-angle, pData->axis1)
      .Update();
}

void Bld_00Callback( CViewObjectBaseSet *pBaseSet,
                     CViewObjectBase    *pBase,
                     CViewObjectRef     *pRef
                   )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
     pData->vbmi0->LoadIdentity()
      .RotateOx(Session::m_viewTime * pData->speed + pData->phase, pData->axis0)
      .Update();
}

void Bld_01Callback( CViewObjectBaseSet *pBaseSet,
                     CViewObjectBase    *pBase,
                     CViewObjectRef     *pRef
                   )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();

	CViewBaseModifier0	&fan = pBase->KFSet().Mod0("Propeller");
	fan.LoadIdentity()             
      .RotateOx(pData->phase+Session::m_viewTime*pData->speed,pData->axis0)
      .Update();
} 

void Bld_15Callback( CViewObjectBaseSet *pBaseSet,
                     CViewObjectBase    *pBase,
                     CViewObjectRef     *pRef
                   )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();

	CViewBaseModifier0	&Col = pBase->KFSet().Mod0("Colocol");
	Col.LoadIdentity()             
      .RotateOx(0.1 * sin(pData->phase+Session::m_viewTime*pData->speed), pData->axis0)
      .Update();
}

// Bld_w03.vbc
 
void Bld_w03Callback( CViewObjectBaseSet *pBaseSet,
                      CViewObjectBase    *pBase,
                      CViewObjectRef     *pRef
                    )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();

        CViewBaseModifier0      &Man = pBase->KFSet().Mod0("Man");
        Man.LoadIdentity()             
      .RotateOx(0.05 * sin(pData->phase+Session::m_viewTime*pData->speed * 0.5 + 3.14 * 0.5), pData->axis0)
      .RotateOz(0.05 * sin(pData->phase+Session::m_viewTime*pData->speed ), pData->axis0)
      .Update();
} 

// Ctl_01.vbc
  
void Ctl_01Callback ( CViewObjectBaseSet *pBaseSet,
                      CViewObjectBase    *pBase,
                      CViewObjectRef     *pRef
                    )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
	CViewBaseModifier0	&Most = pBase->KFSet().Mod0("Most");
    
	int    time =  (int)(Session::m_viewTime / 50.0 );
        double ph   =  Session::m_viewTime - time * 50.0;
	double angle; 
	
	if (ph < 15) // 1
	 angle = 0.;
        else
        if (ph < 25) // 2
	 angle = - 3.14 * 80 / 180 + (25 - ph) * 3.14 * 80 / 180 / 10.;
        else
	if (ph < 40) // 3
	 angle = - 3.14 * 80 / 180.;
	else
	if (ph < 50){
	 angle = - 3.14 * 80 /180 - (40 - ph) * 3.14 * 80 / 180 / 10.;
	}

    Most.LoadIdentity()
      .RotateOx(angle, pData->axis0)
      .Update();
}

void Flg_magCallback( CViewObjectBaseSet *pBaseSet,
                      CViewObjectBase    *pBase,
                      CViewObjectRef     *pRef
                    )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();

        CViewBaseModifier0      &Flag = pBase->KFSet().Mod0("Flag");
    Flag.LoadIdentity()             
      .RotateOz(0.05 * sin(pData->phase+Session::m_viewTime*pData->speed), pData->axis0)
      .Update();
}

// flg_royl.vbc

void Flg_roylCallback( CViewObjectBaseSet *pBaseSet,
                       CViewObjectBase    *pBase,
                       CViewObjectRef     *pRef
                     )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
        CViewBaseModifier0	&Flag = pBase->KFSet().Mod0("Flag");
	CViewBaseModifier0	&Points1 = pBase->KFSet().Mod0("Points1");       
        CViewBaseModifier0      &Points2B = pBase->KFSet().Mod0("Points2B");       
        CViewBaseModifier0      &Points2T = pBase->KFSet().Mod0("Points2T");       
        CViewBaseModifier0      &Points3B = pBase->KFSet().Mod0("Points3B");       
        CViewBaseModifier0      &Points3T = pBase->KFSet().Mod0("Points3T");       
        CViewBaseModifier0      &Point4B = pBase->KFSet().Mod0("Point4B");       
        CViewBaseModifier0      &Point4T = pBase->KFSet().Mod0("Point4T");       

     Points1.LoadIdentity()
      .Translate(0.1 * sin(Session::m_viewTime*pData->speed) * pData->axis1)
      .Translate(0.1 * sin(Session::m_viewTime*pData->speed) * pData->axis0);

     Points2T.LoadIdentity()
      .Translate(-0.5 * sin(Session::m_viewTime*pData->speed + 3.14 * 0.05) * pData->axis1)
      .Translate(0.5 *sin(Session::m_viewTime*pData->speed + 3.14 * 0.05) * pData->axis0);

     Points2B.LoadIdentity()                                                           
      .Translate(-0.5 * sin(Session::m_viewTime*pData->speed + 3.14 * 0.35) * pData->axis1)
      .Translate(0.5 *sin(Session::m_viewTime*pData->speed + 3.14 * 0.35) * pData->axis0);

     Points3T.LoadIdentity()
      .Translate(0.8 * (.5 - sin(Session::m_viewTime*pData->speed + 3.14 * 0.3)) * pData->axis1)
      .Translate(0.8 *sin(Session::m_viewTime*pData->speed + 3.14 * 0.3) * pData->axis0);

     Points3B.LoadIdentity()
      .Translate(-0.8 *sin(Session::m_viewTime*pData->speed + 3.14 * 0.6) * pData->axis1)
      .Translate(0.8 *sin(Session::m_viewTime*pData->speed + 3.14 * 0.3) * pData->axis0);

     Point4T.LoadIdentity()
      .Translate((1 - sin(Session::m_viewTime*pData->speed + 3.14 * 0.65)) * pData->axis1)
      .Translate(sin(Session::m_viewTime*pData->speed + 3.14 * 0.65) * pData->axis0);

     Point4B.LoadIdentity()
      .Translate(-sin(Session::m_viewTime*pData->speed + 3.14 * 1.05) * pData->axis1)
      .Translate(sin(Session::m_viewTime*pData->speed + 3.14 * 1.05) * pData->axis0);
	         
     Flag.LoadIdentity()
      .Update();
}

void g_staticInit2()
{
    CNameDecls &refNames = ZAV_Scene()->ObjRefNames();  // objects' names
    CNameDecl  &Bld_01Arr = refNames["bld_01"];      // array of objects named "mill"
    int       nBld_01Count = Bld_01Arr.Count(); // number of objects named "mill"

    ct_ClassTableID  ctID = g_arena.searchSeanceClassTable("StaticObj");

    for( int i = 0 ; i < nBld_01Count ; i++ ) 
    {
         CViewObjectRef  *pBld_01 = (CViewObjectRef*)Bld_01Arr[i];
         CViewObjectBase &pBase = pBld_01->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.bld_01");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->phase = g_arena.context->rnd_f(3);
         data->speed = g_arena.context->rnd_f(2,4);

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               ) * 0.5;

         pBld_01->SetAnimationCallback(Bld_01Callback);
         pBld_01->SetUserAttrib(data);
    }

// Bld_15.vbc    

    CNameDecl     &Bld_15Arr = refNames["bld_15"];	// array of objects named "mill"
    int       nBld_15Count = Bld_15Arr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nBld_15Count ; i++ ) 
    {
         CViewObjectRef  *pBld_15 = (CViewObjectRef*)Bld_15Arr[i];
         CViewObjectBase &pBase = pBld_15->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.bld_15");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->phase = g_arena.context->rnd_f(3);
         data->speed = g_arena.context->rnd_f(1,4);

         data->axis0 = *(CFVector3*)pBase.Names().vertices["Axis0"][0];

         pBld_15->SetAnimationCallback(Bld_15Callback);
         pBld_15->SetUserAttrib(data);
    }

// Bld_W03.vbc    

    CNameDecl     &Bld_w03Arr = refNames["bld_w03"];      // array of objects named "mill"
    int       nBld_w03Count = Bld_w03Arr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nBld_w03Count ; i++ ) 
    {
         CViewObjectRef  *pBld_w03 = (CViewObjectRef*)Bld_w03Arr[i];
         CViewObjectBase &pBase = pBld_w03->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Bld_w03");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->phase = g_arena.context->rnd_f(3);
         data->speed = g_arena.context->rnd_f(2,4);

         data->axis0 = *(CFVector3*)pBase.Names().vertices["Axis0"][0];

         pBld_w03->SetAnimationCallback(Bld_w03Callback);
         pBld_w03->SetUserAttrib(data);
    }

// Bld_W05.vbc    

    CNameDecl     &Bld_w05Arr = refNames["bld_w05"];      // array of objects named "mill"
    int       nBld_w05Count = Bld_w05Arr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nBld_w05Count ; i++ ) 
    {
         CViewObjectRef  *pBld_w05 = (CViewObjectRef*)Bld_w05Arr[i];
         CViewObjectBase &pBase = pBld_w05->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Bld_w05");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->phase = g_arena.context->rnd_f(3);
         data->speed = g_arena.context->rnd_f(1,4);

         data->axis0 = *(CFVector3*)pBase.Names().vertices["Axis0"][0];

         pBld_w05->SetAnimationCallback(Bld_w03Callback);
         pBld_w05->SetUserAttrib(data);
    }

// Bld_W06.vbc    

    CNameDecl     &Bld_w06Arr = refNames["bld_w06"];      // array of objects named "mill"
    int       nBld_w06Count = Bld_w06Arr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nBld_w06Count ; i++ ) 
    {
         CViewObjectRef  *pBld_w06 = (CViewObjectRef*)Bld_w06Arr[i];
         CViewObjectBase &pBase = pBld_w06->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Bld_w06");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->phase = g_arena.context->rnd_f(3);
         data->speed = g_arena.context->rnd_f(1,4);

         data->axis0 = *(CFVector3*)pBase.Names().vertices["Axis0"][0];

         pBld_w06->SetAnimationCallback(Bld_w03Callback);
         pBld_w06->SetUserAttrib(data);
    }

// Ctl_01.vbc    

    CNameDecl     &Ctl_01Arr = refNames["ctl_01"];      // array of objects named "mill"
    int       nCtl_01Count = Ctl_01Arr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nCtl_01Count ; i++ ) 
    {
         CViewObjectRef  *pCtl_01 = (CViewObjectRef*)Ctl_01Arr[i];
         CViewObjectBase &pBase = pCtl_01->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Ctl_01");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->axis0 = *(CFVector3*)pBase.Names().vertices["Axis0"][0];

         pCtl_01->SetAnimationCallback(Ctl_01Callback);
         pCtl_01->SetUserAttrib(data);
    }

// flg_mag.vbc

    CNameDecl     &Flg_magArr = refNames["flg_mag"];      // array of objects named "mill"
    int       nFlg_magCount = Flg_magArr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nFlg_magCount ; i++ ) 
    {
         CViewObjectRef  *pFlg_mag = (CViewObjectRef*)Flg_magArr[i];
         CViewObjectBase &pBase = pFlg_mag->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Flg_mag");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->phase = g_arena.context->rnd_f(3);
         data->speed = g_arena.context->rnd_f(1,4);

         data->axis0 = *(CFVector3*)pBase.Names().vertices["Axis0"][0];

         pFlg_mag->SetAnimationCallback(Flg_magCallback);
         pFlg_mag->SetUserAttrib(data);
    }

// flg_royl.vbc    

    CNameDecl     &flg_roylArr = refNames["flg_royl"];      // array of objects named "mill"
    int       nflg_roylCount = flg_roylArr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nflg_roylCount ; i++ ) 
    {
         CViewObjectRef  *pflg_royl = (CViewObjectRef*)flg_roylArr[i];
         CViewObjectBase &pBase = pflg_royl->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.flg_royl");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->speed = g_arena.context->rnd_f(5,10);

         data->axis0 = (
			*(CFVector3*)pBase.Names().vertices["Axis0"][1]-
                        *(CFVector3*)pBase.Names().vertices["Axis0"][0]
		       ) * 3;
         data->axis1 = *(CFVector3*)pBase.Names().vertices["Axis1"][1]-
                       *(CFVector3*)pBase.Names().vertices["Axis1"][0];

         pflg_royl->SetAnimationCallback(Flg_roylCallback);
         pflg_royl->SetUserAttrib(data);
    }

    CNameDecl     &bld_00cArr = refNames["bld_00c"];
    int       nbld_00cCount = bld_00cArr.Count();

    for( int i = 0 ; i < nbld_00cCount ; i++ )
    {
         CViewObjectRef  *pbld_00c = (CViewObjectRef*)bld_00cArr[i];
         CViewObjectBase &pBase = pbld_00c->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.bld_00c");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

	 data->speed = g_arena.context->rnd_f(0.5,2);
         data->phase = g_arena.context->rnd_f(5);

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               ) * 0.5;

         data->vbmi0 = &pBase.KFSet().Mod0("Circ");

         pbld_00c->SetAnimationCallback(Bld_00Callback);
         pbld_00c->SetUserAttrib(data);
    }

    CNameDecl     &Bld_v00Arr = refNames["bld_v00"];
    int       nBld_v00Count = Bld_v00Arr.Count();

    for( int i = 0 ; i < nBld_v00Count ; i++ )
    {
         CViewObjectRef  *pBld_v00 = (CViewObjectRef*)Bld_v00Arr[i];
         CViewObjectBase &pBase = pBld_v00->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Bld_v00");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->axis0 =
                *(CFVector3*)pBase.Names().vertices["Axis0"][0];

	 data->axis1 =
                *(CFVector3*)pBase.Names().vertices["Axis1"][0];

         data->vbmi0 = &pBase.KFSet().Mod0("RDoor");
         data->vbmi1 = &pBase.KFSet().Mod0("LDoor");

         pBld_v00->SetAnimationCallback(Bld_v00Callback);
         pBld_v00->SetUserAttrib(data);
    }

    CNameDecl     &ctl_09Arr = refNames["ctl_09"];
    int       nctl_09Count = ctl_09Arr.Count();

    for( int i = 0 ; i < nctl_09Count ; i++ )
    {
         CViewObjectRef  *pctl_09 = (CViewObjectRef*)ctl_09Arr[i];
         CViewObjectBase &pBase = pctl_09->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.ctl_09");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               -*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               );

         data->vbmi0 = &pBase.KFSet().Mod0("Door");

         pctl_09->SetAnimationCallback(Door_Callback);
         pctl_09->SetUserAttrib(data);
    }

    CNameDecl  &Bld_a02Arr = refNames["bld_a02"];      // array of objects named "mill"
    int       nBld_a02Count = Bld_a02Arr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nBld_a02Count ; i++ ) 
    {
         CViewObjectRef  *pBld_a02 = (CViewObjectRef*)Bld_a02Arr[i];
         CViewObjectBase &pBase = pBld_a02->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Bld_a02");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               ) * 0.5;

         data->axis1 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis1"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis1"][1]
               ) * 0.5;

	 data->axis2 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis2"][0]
               -*(CFVector3*)pBase.Names().vertices["Axis2"][1]
               );

         data->vbmi0 = &pBase.KFSet().Mod0("Vorot");
         data->vbmi1 = &pBase.KFSet().Mod0("Porsh");
         data->vbmi2 = &pBase.KFSet().Mod0("Guns");

         pBld_a02->SetAnimationCallback(Bld_a02Callback);
         pBld_a02->SetUserAttrib(data);
    }

    CNameDecl  &Bld_a03Arr = refNames["bld_a03"];      // array of objects named "mill"
    int       nBld_a03Count = Bld_a03Arr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nBld_a03Count ; i++ ) 
    {
         CViewObjectRef  *pBld_a03 = (CViewObjectRef*)Bld_a03Arr[i];
         CViewObjectBase &pBase = pBld_a03->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Bld_a03");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               ) * 0.5;

         data->axis1 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis1"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis1"][1]
               ) * 0.5 - *(CFVector3*)pBase.Names().vertices["Axis1"][2];

         data->vbmi0 = &pBase.KFSet().Mod0("Gun");

         pBld_a03->SetAnimationCallback(Bld_a03Callback);
         pBld_a03->SetUserAttrib(data);
    }

    CNameDecl     &evl_angrArr = refNames["evl_angr"];
    int       nevl_angrCount = evl_angrArr.Count();

    for( int i = 0 ; i < nevl_angrCount ; i++ )
    {
         CViewObjectRef  *pevl_angr = (CViewObjectRef*)evl_angrArr[i];
         CViewObjectBase &pBase = pevl_angr->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.evl_angr");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               -*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               );

         data->vbmi0 = &pBase.KFSet().Mod0("Door");

         pevl_angr->SetAnimationCallback(Door_Callback);
         pevl_angr->SetUserAttrib(data);
    }

}

//------------------------------------------------------------------------------
//  Level.3
//------------------------------------------------------------------------------   

void War_s07Callback( CViewObjectBaseSet *pBaseSet,
                      CViewObjectBase    *pBase,
                      CViewObjectRef     *pRef
                    )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
     pData->vbmi0->LoadIdentity()
      .RotateOy(Session::m_viewTime * pData->speed + pData->phase, pData->axis0)
      .Update();

     pData->vbmi1->LoadIdentity()
      .RotateOy(-Session::m_viewTime * pData->speed + pData->phase, pData->axis0)
      .Update();
}

void Vhl_qganCallback( CViewObjectBaseSet *pBaseSet,
                       CViewObjectBase    *pBase,
                       CViewObjectRef     *pRef
                     )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
	int    time =  (int)(Session::m_viewTime / 30.0 );
        double ph   =  Session::m_viewTime - time * 30.0;
        double angle, gAngle; 

	if (ph < 3){
	 angle = 3.14 + (ph - 3)*3.14/3.;
	 gAngle = 0.;
	}else
	if (ph < 15){ // 1
	 angle = 3.14;
         gAngle = 30 * 3.14 / 180.;
	 if (ph < 5){
          gAngle = 30 * 3.14 / 180. - (5 - ph) * 30 * 3.14 /180. /2.;
	 }else
	  if (ph > 13)
           gAngle = 30 * 3.14 / 180. + (13 - ph) * 30 * 3.14 /180. /2.;
	}
	else
        if (ph < 18){
	 angle = - (ph - 18)*3.14/3.;
         gAngle = 0.0;
	}else
	if (ph < 30){ // 2
         angle = 0.;
         gAngle = 40 * 3.14 / 180.;
	 if (ph < 20)
            gAngle = 40 * 3.14 / 180. - (20 - ph) * 40 * 3.14 /180. /2.;
	 else
	 if (ph > 28)
          gAngle = 40 * 3.14 / 180. + (28 - ph) * 40 * 3.14 /180. /2.;
	}
    
    pData->vbmi0->LoadIdentity()
      .RotateOy(angle, pData->axis0)	
      .Update();

    pData->vbmi1->LoadIdentity()
      .RotateOx(gAngle, pData->axis1)
      .RotateOy(angle, pData->axis0)
      .Update();
}

void War_s01Callback( CViewObjectBaseSet *pBaseSet,
                      CViewObjectBase    *pBase,
                      CViewObjectRef     *pRef
                    )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
        int    time =  (int)(Session::m_viewTime / 2.5 );
        double scale = Session::m_viewTime / 2.5 - time;
    
    pData->vbmi0->LoadIdentity()
      .Translate(scale * pData->axis0)
      .Update();
}

void Swing_Callback( CViewObjectBaseSet *pBaseSet,
                     CViewObjectBase    *pBase,
                     CViewObjectRef     *pRef
                   )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();

      pData->vbmi0->LoadIdentity()             
      .RotateOx(0.05 * sin(pData->phase+Session::m_viewTime*pData->speed * 0.5 + 3.14 * 0.5), pData->axis0)
      .RotateOz(0.05 * sin(pData->phase+Session::m_viewTime*pData->speed), pData->axis0)
      .Update();
} 

void Arn_entrCallback( CViewObjectBaseSet *pBaseSet,
                       CViewObjectBase    *pBase,
                       CViewObjectRef     *pRef
                     )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
        int    time =  (int)(Session::m_viewTime / 30.0 );
        double ph   =  Session::m_viewTime - time * 30.0;
        double scale1, scale2;

        if ((ph < 10) || (ph > 20))
         scale1 = 0.0;
        else
        {
        if (ph < 13)
          scale1 = (ph - 10) / 3;
        else
          if (ph > 17)
           scale1 = (20 - ph) / 3; 
	  else 
           scale1 = 1; 
	}

        scale2 = 1 - scale1;
    
    pData->vbmi1->LoadIdentity()
      .Translate(scale1 * pData->axis1)
      .Update();
    pData->vbmi2->LoadIdentity()
      .Translate(scale2 * pData->axis2)
      .Update();
}

void Arn_angrCallback( CViewObjectBaseSet *pBaseSet,
                       CViewObjectBase    *pBase,
                       CViewObjectRef     *pRef
                     )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
        int    time =  (int)(Session::m_viewTime / 30.0 );
        double ph   =  Session::m_viewTime - time * 30.0;
        double scale;

        if ((ph < 10) || (ph > 20))
         scale = 0.0;
        else
        {
        if (ph < 13)
          scale = (ph - 10) / 3;
        else
          if (ph > 17)
           scale = (20 - ph) / 3; 
	  else 
           scale = 1; 
	}
    
    pData->vbmi0->LoadIdentity()
      .Translate(scale * pData->axis0)
      .Update();
    pData->vbmi1->LoadIdentity()
      .Translate(-scale * pData->axis0)
      .Update();
}

void War_s00Callback( CViewObjectBaseSet *pBaseSet,
                      CViewObjectBase    *pBase,
                      CViewObjectRef     *pRef
                    )                              
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();

     double moment = Session::m_viewTime * pData->speed + pData->phase;	

     pData->vbmi0->LoadIdentity()
      .RotateOx(-moment + 80 * 3.14 / 180., pData->axis0)
      .Update();

     pData->vbmi1->LoadIdentity()
      .Translate(0.5 * (1-sin(moment + 3.14 * 1.5)) * pData->axis1)
      .Update();

     pData->vbmi3->LoadIdentity() 
      .RotateOx(moment - 8.5 * 3.14 / 180. + (7 * 3.14 / 180.) * sin(moment + 3.14 * 0.5), pData->axis2)
      .RotateOx(-moment, pData->axis0)
      .Update();
	
     CFVector3 *pVect = (CFVector3*)pBase->Names().vertices["Shatun"][1];	
     CFVector3 Vect(0, pVect->y - pData->axis3.y, 0);	

     pData->vbmi2->LoadIdentity()
      .Translate(Vect)
      .Update();
}

void War_m14Callback( CViewObjectBaseSet *pBaseSet,
                      CViewObjectBase    *pBase,
                      CViewObjectRef     *pRef
                     )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
        int    time =  (int)((Session::m_viewTime + pData->phase) / 30.0 );
        double ph   =  (Session::m_viewTime + pData->phase) - time * 30.0;
        double angle, rAngle; 

        if (ph < 6){
         angle = -45 * 3.14 /180. + ph * 90 * 3.14 / 180./6.;
         rAngle = 0.;
	}else
	if (ph < 15){ // 1
         angle = 45 * 3.14 / 180;
         rAngle = 20 * 3.14 / 180.;
         if (ph < 9){
          rAngle = 20 * 3.14 / 180. - (9 - ph) * 20 * 3.14 /180. /3.;
	 }else
          if (ph > 12)
           rAngle = 20 * 3.14 / 180. + (12 - ph) * 20 * 3.14 /180. /3.;
	}
	else
        if (ph < 21){
         angle = 45 * 3.14 / 180. - (ph - 15)* 90 *3.14/180./6.;
         rAngle = 0.0;
	}else
	if (ph < 30){ // 2
         angle = -45. * 3.14 / 180.;
         rAngle = 20 * 3.14 / 180.;
         if (ph < 24)
            rAngle = 20 * 3.14 / 180. - (24 - ph) * 20 * 3.14 /180. /3.;
	 else
         if (ph > 27)
          rAngle = 20 * 3.14 / 180. + (27 - ph) * 20 * 3.14 /180. /3.;
	}
    
    pData->vbmi0->LoadIdentity()
      .RotateOx(-rAngle, pData->axis1)
      .RotateOy(angle, pData->axis0)
      .Update();

    pData->vbmi1->LoadIdentity()
      .RotateOy(angle, pData->axis0)
      .Update();
}

void War_grdCallback( CViewObjectBaseSet *pBaseSet,
                      CViewObjectBase    *pBase,
                      CViewObjectRef     *pRef
                    )                              
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();

     pData->vbmi0->LoadIdentity()
      .RotateOz(Session::m_viewTime * pData->speed + pData->phase, pData->axis0)
      .Update();
        
}

void g_staticInit3()
{                                       
    CNameDecls &refNames = ZAV_Scene()->ObjRefNames();  // objects' names
    ct_ClassTableID  ctID = g_arena.searchSeanceClassTable("StaticObj");
    
    CNameDecl     &War_grdArr = refNames["war_grd"];
    int       nWar_grdCount = War_grdArr.Count();

    for( int i = 0 ; i < nWar_grdCount ; i++ )
    {
         CViewObjectRef  *pWar_grd = (CViewObjectRef*)War_grdArr[i];
         CViewObjectBase &pBase = pWar_grd->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.War_grd");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->speed = g_arena.context->rnd_f(0.2,1);
         data->phase = g_arena.context->rnd_f(5);

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               ) * 0.5;

         data->vbmi0 = &pBase.KFSet().Mod0("Disk");

         pWar_grd->SetAnimationCallback(War_grdCallback);
         pWar_grd->SetUserAttrib(data);
    }
    
    CNameDecl     &War_m16Arr = refNames["war_m16"];      // array of objects named "mill"
    int       nWar_m16Count = War_m16Arr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nWar_m16Count ; i++ ) 
    {
         CViewObjectRef  *pWar_m16 = (CViewObjectRef*)War_m16Arr[i];
         CViewObjectBase &pBase = pWar_m16->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.War_m16");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->phase = g_arena.context->rnd_f(3);
         data->speed = g_arena.context->rnd_f(2,3);

         data->axis0 = *(CFVector3*)pBase.Names().vertices["Axis0"][0];

         data->vbmi0 = &pBase.KFSet().Mod0("Man");

         pWar_m16->SetAnimationCallback(Swing_Callback);
         pWar_m16->SetUserAttrib(data);
    }
        
    CNameDecl     &War_m08Arr = refNames["war_m08"];      // array of objects named "mill"
    int       nWar_m08Count = War_m08Arr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nWar_m08Count ; i++ )
    {
         CViewObjectRef  *pWar_m08 = (CViewObjectRef*)War_m08Arr[i];
         CViewObjectBase &pBase = pWar_m08->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.War_m08");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->phase = g_arena.context->rnd_f(3);
         data->speed = g_arena.context->rnd_f(2,3);

         data->axis0 = *(CFVector3*)pBase.Names().vertices["Axis0"][0];

         data->vbmi0 = &pBase.KFSet().Mod0("Hook");

         pWar_m08->SetAnimationCallback(Swing_Callback);
         pWar_m08->SetUserAttrib(data);
    }

    CNameDecl     &War_m06bArr = refNames["war_m06b"];      // array of objects named "mill"
    int       nWar_m06bCount = War_m06bArr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nWar_m06bCount ; i++ )
    {
         CViewObjectRef  *pWar_m06b = (CViewObjectRef*)War_m06bArr[i];
         CViewObjectBase &pBase = pWar_m06b->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.War_m06b");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->phase = g_arena.context->rnd_f(3);
         data->speed = g_arena.context->rnd_f(2,3);

         data->axis0 = *(CFVector3*)pBase.Names().vertices["Axis0"][0];

         data->vbmi0 = &pBase.KFSet().Mod0("Hook");

         pWar_m06b->SetAnimationCallback(Swing_Callback);
         pWar_m06b->SetUserAttrib(data);
    }
   
    CNameDecl     &War_m14Arr = refNames["war_m14"];      // array of objects named "mill"
    int       nWar_m14Count = War_m14Arr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nWar_m14Count ; i++ ) 
    {
         CViewObjectRef  *pWar_m14 = (CViewObjectRef*)War_m14Arr[i];
         CViewObjectBase &pBase = pWar_m14->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.War_m14");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->phase = g_arena.context->rnd_f(30);

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               ) * 0.5;

         data->axis1 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis1"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis1"][1]
               ) * 0.5;

         data->vbmi0 = &pBase.KFSet().Mod0("Radar");
         data->vbmi1 = &pBase.KFSet().Mod0("Base");

         pWar_m14->SetAnimationCallback(War_m14Callback);
         pWar_m14->SetUserAttrib(data);
    }
  
    CNameDecl  &War_s00Arr = refNames["war_s00"];      // array of objects named "mill"
    int       nWar_s00Count = War_s00Arr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nWar_s00Count ; i++ ) 
    {
         CViewObjectRef  *pWar_s00 = (CViewObjectRef*)War_s00Arr[i];
         CViewObjectBase &pBase = pWar_s00->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.War_s00");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->speed = 1.;
         data->phase = g_arena.context->rnd_f(30);

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Wheels"][0]
               +*(CFVector3*)pBase.Names().vertices["Wheels"][1]
               ) * 0.5;
	 
         data->axis1 =
               (
                *(CFVector3*)pBase.Names().vertices["Door"][0]
               -*(CFVector3*)pBase.Names().vertices["Door"][1]
               );

         data->axis2 = *(CFVector3*)pBase.Names().vertices["Shatun"][0];
	 
         data->axis3 = *(CFVector3*)pBase.Names().vertices["Shatun"][1];

         data->vbmi0 = &pBase.KFSet().Mod0("Wheels");
         data->vbmi1 = &pBase.KFSet().Mod0("Door");
         data->vbmi2 = &pBase.KFSet().Mod0("VShatuns");
         data->vbmi3 = &pBase.KFSet().Mod0("Shatuns");

         pWar_s00->SetAnimationCallback(War_s00Callback);
         pWar_s00->SetUserAttrib(data);
    }
   
    CNameDecl     &Arn_angrArr = refNames["arn_angr"];      // array of objects named "mill"
    int       nArn_angrCount = Arn_angrArr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nArn_angrCount ; i++ ) 
    {
         CViewObjectRef  *pArn_angr = (CViewObjectRef*)Arn_angrArr[i];
         CViewObjectBase &pBase = pArn_angr->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Arn_angr");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->axis0 =
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               -*(CFVector3*)pBase.Names().vertices["Axis0"][1];

         data->vbmi0 = &pBase.KFSet().Mod0("LDoor");
         data->vbmi1 = &pBase.KFSet().Mod0("RDoor");

         pArn_angr->SetAnimationCallback(Arn_angrCallback);
         pArn_angr->SetUserAttrib(data);
    }
 
    CNameDecl     &flg_humnArr = refNames["flg_humn"];
    int       nflg_humnCount = flg_humnArr.Count();

    for( int i = 0 ; i < nflg_humnCount ; i++ )
    {
         CViewObjectRef  *pflg_humn = (CViewObjectRef*)flg_humnArr[i];
         CViewObjectBase &pBase = pflg_humn->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.flg_humn");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

	 data->speed = g_arena.context->rnd_f(6,8);
         data->phase = g_arena.context->rnd_f(5);

         data->axis0 = *(CFVector3*)pBase.Names().vertices["Axis0"][0]
		       -*(CFVector3*)pBase.Names().vertices["Axis0"][1]; 

         data->axis1 = (*(CFVector3*)pBase.Names().vertices["Axis1"][0]
		       -*(CFVector3*)pBase.Names().vertices["Axis1"][1]) * 0.2; 

	 data->vbmi0 = &pBase.KFSet().Mod0("Planes");
	 data->vbmi1 = &pBase.KFSet().Mod0("Point1");       
	 data->vbmi2 = &pBase.KFSet().Mod0("Point2");	
         data->vbmi3 = &pBase.KFSet().Mod0("Point3");       
	 data->vbmi4 = &pBase.KFSet().Mod0("Point4");	

         pflg_humn->SetAnimationCallback(Flags_Callback);
         pflg_humn->SetUserAttrib(data);
    }

    CNameDecl     &flg_mrdArr = refNames["flg_mrd"];
    int       nflg_mrdCount = flg_mrdArr.Count();

    for( int i = 0 ; i < nflg_mrdCount ; i++ )
    {
         CViewObjectRef  *pflg_mrd = (CViewObjectRef*)flg_mrdArr[i];
         CViewObjectBase &pBase = pflg_mrd->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.flg_mrd");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

	 data->speed = g_arena.context->rnd_f(6,8);
         data->phase = g_arena.context->rnd_f(5);

         data->axis0 = *(CFVector3*)pBase.Names().vertices["Axis0"][0]
		       -*(CFVector3*)pBase.Names().vertices["Axis0"][1]; 

         data->axis1 = (*(CFVector3*)pBase.Names().vertices["Axis1"][0]
		       -*(CFVector3*)pBase.Names().vertices["Axis1"][1]) * 0.2; 

	 data->vbmi0 = &pBase.KFSet().Mod0("Planes");
	 data->vbmi1 = &pBase.KFSet().Mod0("Point1");       
	 data->vbmi2 = &pBase.KFSet().Mod0("Point2");	
         data->vbmi3 = &pBase.KFSet().Mod0("Point3");       
	 data->vbmi4 = &pBase.KFSet().Mod0("Point4");	

         pflg_mrd->SetAnimationCallback(Flags_Callback);
         pflg_mrd->SetUserAttrib(data);
    }
   
    CNameDecl     &Arn_entrArr = refNames["arn_entr"];      // array of objects named "mill"
    int       nArn_entrCount = Arn_entrArr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nArn_entrCount ; i++ ) 
    {
         CViewObjectRef  *pArn_entr = (CViewObjectRef*)Arn_entrArr[i];
         CViewObjectBase &pBase = pArn_entr->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Arn_entr");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->axis1 =
                *(CFVector3*)pBase.Names().vertices["Axis1"][0]
               -*(CFVector3*)pBase.Names().vertices["Axis1"][1];

         data->axis2 =
                *(CFVector3*)pBase.Names().vertices["Axis2"][1]
               -*(CFVector3*)pBase.Names().vertices["Axis2"][0];


         data->vbmi1 = &pBase.KFSet().Mod0("Door1");
         data->vbmi2 = &pBase.KFSet().Mod0("Door2");

         pArn_entr->SetAnimationCallback(Arn_entrCallback);
         pArn_entr->SetUserAttrib(data);
    }
    
    CNameDecl     &war_s01Arr = refNames["war_s01"];      // array of objects named "mill"
    int       nwar_s01Count = war_s01Arr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nwar_s01Count ; i++ ) 
    {
         CViewObjectRef  *pwar_s01 = (CViewObjectRef*)war_s01Arr[i];
         CViewObjectBase &pBase = pwar_s01->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.war_s01");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->axis0 =
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               -*(CFVector3*)pBase.Names().vertices["Axis0"][1];

         data->vbmi0 = &pBase.KFSet().Mod0("Conv");

         pwar_s01->SetAnimationCallback(War_s01Callback);
         pwar_s01->SetUserAttrib(data);
    }
     
    CNameDecl     &Vhl_qganArr = refNames["vhl_qgan"];      // array of objects named "mill"
    int       nVhl_qganCount = Vhl_qganArr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nVhl_qganCount ; i++ ) 
    {
         CViewObjectRef  *pVhl_qgan = (CViewObjectRef*)Vhl_qganArr[i];
         CViewObjectBase &pBase = pVhl_qgan->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Vhl_qgan");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->phase = g_arena.context->rnd_f(30);

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               ) * 0.5;

         data->axis1 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis1"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis1"][1]
               ) * 0.5;

         data->vbmi0 = &pBase.KFSet().Mod0("Base");
         data->vbmi1 = &pBase.KFSet().Mod0("Guns");

         pVhl_qgan->SetAnimationCallback(Vhl_qganCallback);
         pVhl_qgan->SetUserAttrib(data);
    }

    CNameDecl     &War_s07Arr = refNames["war_s07"];      // array of objects named "mill"
    int       nWar_s07Count = War_s07Arr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nWar_s07Count ; i++ ) 
    {
         CViewObjectRef  *pWar_s07 = (CViewObjectRef*)War_s07Arr[i];
         CViewObjectBase &pBase = pWar_s07->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.War_s07");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->speed = g_arena.context->rnd_f(2,4);
         data->phase = g_arena.context->rnd_f(5);

         data->axis0 =
                (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis0"][1]
                ) * 0.5;

         data->vbmi0 = &pBase.KFSet().Mod0("TWheel");
         data->vbmi1 = &pBase.KFSet().Mod0("BWheel");

         pWar_s07->SetAnimationCallback(War_s07Callback);
         pWar_s07->SetUserAttrib(data);
    }
}

//------------------------------------------------------------------------------
//  Level.4
//------------------------------------------------------------------------------   
void MillCallback( CViewObjectBaseSet *pBaseSet,
                   CViewObjectBase    *pBase,
                   CViewObjectRef     *pRef
                 )                              
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();

	int n = pBase->ReductionNum();
	(void)n;

	CViewBaseModifier0	&fan = pBase->KFSet().Mod0("Propeller");
	fan.LoadIdentity()             
      .RotateOz(-pData->phase+Session::m_viewTime*pData->speed,pData->axis0)
      .Update();
	CViewBaseModifier0	&wh = pBase->KFSet().Mod0("Wheel");
	wh.LoadIdentity()
      .RotateOx(pData->phase+Session::m_viewTime*pData->speed * 0.5,pData->axis1)
      .Update();
}

// cln_f07.vbc

void ZeppCallback( CViewObjectBaseSet *pBaseSet,
                    CViewObjectBase    *pBase,
                    CViewObjectRef     *pRef
                  )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();

	int n = pBase->ReductionNum();
	(void)n;

	CViewBaseModifier0	&LProp = pBase->KFSet().Mod0("LPropeller");
	LProp.LoadIdentity()
      .RotateOx(pData->phase+Session::m_viewTime*pData->speed,pData->axis0)
      .Update();
	CViewBaseModifier0	&RProp = pBase->KFSet().Mod0("RPropeller");
	RProp.LoadIdentity()
      .RotateOx(pData->phase-Session::m_viewTime*pData->speed,pData->axis1)
      .Update();
}

// cln_b05.vbc crane

void CraneCallback( CViewObjectBaseSet *pBaseSet,
                    CViewObjectBase    *pBase,
                    CViewObjectRef     *pRef
                  )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
        CViewBaseModifier0	&body = pBase->KFSet().Mod0("Body");
	CViewBaseModifier0	&hook = pBase->KFSet().Mod0("Hook");	
    
	int    time =  (int)(Session::m_viewTime / 100.0 );
        double ph   =  Session::m_viewTime - time*100.0;
	double angle, scale = 0.0; 
	
	if (ph < 30){ // 1
	 angle = 90.0 * 3.14 / 180;
	 if (ph < 3)
	  scale = 1.3 - (3 - ph) * (1.3 / 3);
	 else
	 if (ph > 27)
	  scale = (30 - ph) * (1.3 / 3);
	 else 
	  scale = 1.3;	
	}
	else
	if (ph < 34)
	 angle = 90.0 * 3.14 / 180 - (ph - 30)*80*3.14/4/180.;
        else
        if (ph < 60){ // 2
         angle = 10 * 3.14/180.;
	 if (ph < 37)
	  scale = 1.3 - (37 - ph) * (1.3 / 3);
	 else
	 if (ph > 57)
	  scale = (60 - ph) * (1.3 / 3);	
	 else 
	  scale = 1.3;	
	}
	else
	if (ph < 66)
	 angle = 10*3.14/180. + (ph - 60)*240*3.14/6/180.;
        else
	if (ph < 96){ // 3
	 angle = 250.0 * 3.14 / 180;
	 if (ph < 69)
	  scale = 1.3 - (69 - ph) * (1.3 / 3);
	 else
	 if (ph > 93)
	  scale = (96 - ph) * (1.3 / 3);		
	 else 
	  scale = 1.3;	
	}
	else
	if (ph < 100)
	 angle = 250.0 * 3.14 / 180 - (ph - 96)*160*3.14/4/180.;
    
    hook.LoadIdentity()
      .RotateOy(angle, pData->axis0)
      .Translate( scale * pData->axis1)
      .Update();

    body.LoadIdentity()
      .RotateOy(angle, pData->axis0)
      .Update();
}

// srg_cran.vbc  my crane

void MyCraneCallback( CViewObjectBaseSet *pBaseSet,
                      CViewObjectBase    *pBase,
                      CViewObjectRef     *pRef
                    )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
        CViewBaseModifier0	&Crane = pBase->KFSet().Mod0("Crane");
	CViewBaseModifier0	&Wheel = pBase->KFSet().Mod0("Wheel");
	CViewBaseModifier0	&Propeller = pBase->KFSet().Mod0("Propeller");	
    
	double moment = Session::m_viewTime + pData->phase;
	int    time =  (int)(moment / 60.0 );
        double ph   =  moment - time * 60.0;
	double angle, whAngle, scale = 0.0; 

	if (ph < 5){
	 angle = 3.14 + (ph - 5)*3.14/5.;
	 whAngle = 2 * 3.14;
	}else
	if (ph < 30){ // 1
	 angle = 3.14;
	 whAngle = 2 * 3.14;
	 if (ph < 8){
	  whAngle = 2 * 3.14 + (ph - 5)*2*3.14/5.;
	  scale = 1.3 - (8 - ph) * (1.3 / 3);
	 }else
	 if (ph > 27){
	  scale = (30 - ph) * (1.3 / 3);
	  whAngle = - (ph - 5)*2*3.14/5.;
	 }else 
	  scale = 1.3;	
	}
	else
        if (ph < 35){
	 angle = - (ph - 35)*3.14/5.;
         whAngle = 0.0;
	}else
	if (ph < 60){ // 2
         angle = 0.;
	 whAngle = 2 * 3.14;
	 if (ph < 38){
	    whAngle = 2 * 3.14 + (ph - 38)*2*3.14/5.;
	    scale = 1.3 - (38 - ph) * (1.3 / 3);
	 }else
	 if (ph > 57){
	  scale = (60 - ph) * (1.3 / 3);
	  whAngle = - (ph - 60)*2*3.14/5.;	
	 }else 
	  scale = 1.3;	
	}
    
    Propeller.LoadIdentity()
      .RotateOy(angle, pData->axis0)
      .Translate( scale * pData->axis1)
      .Update();

    Wheel.LoadIdentity()
      .RotateOx(whAngle, pData->axis2)
      .RotateOy(angle, pData->axis0)
      .Update();

    Crane.LoadIdentity()
      .RotateOy(angle, pData->axis0)
      .Update();
}

// cln_b0c.vbc engine

void EngineCallback( CViewObjectBaseSet *pBaseSet,
                     CViewObjectBase    *pBase,
                     CViewObjectRef     *pRef
                   )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
        CViewBaseModifier0	&most = pBase->KFSet().Mod0("Vorot");
	CViewBaseModifier0	&shatun1 = pBase->KFSet().Mod0("Shatun1");       
	CViewBaseModifier0	&shatun2 = pBase->KFSet().Mod0("Shatun2");	

    most.LoadIdentity()
      .RotateOz(3.14 + Session::m_viewTime*pData->speed, pData->axis0)
      .Update();

     shatun1.LoadIdentity()
      .RotateOz(18*3.14/180 * sin(Session::m_viewTime*pData->speed), pData->axis1)
      .Translate((0.9-sin(Session::m_viewTime*pData->speed + 3.14 * 0.5)) * pData->axis2)
      .Update();
        
    shatun2.LoadIdentity()
      .Translate((0.9-sin(Session::m_viewTime*pData->speed + 3.14 * 0.5)) * pData->axis2)
      .Update();
}

// Bridge

void BridgeCallback ( CViewObjectBaseSet *pBaseSet,
                      CViewObjectBase    *pBase,
                      CViewObjectRef     *pRef
                    )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
	CViewBaseModifier0	&Bridge = pBase->KFSet().Mod0("Bridge");
        CViewBaseModifier0	&Wheels1 = pBase->KFSet().Mod0("Wheels1");
	CViewBaseModifier0	&Wheels2 = pBase->KFSet().Mod0("Wheels2");	
    
	int    time =  (int)(Session::m_viewTime / 40.0 );
        double ph   =  Session::m_viewTime - time*40.0;
	double angle, scale; 
	
	if (ph < 10){ // 1
	 angle = 0.;
	 scale = 0.;
        }
        else
        if (ph < 20){ // 2
	 angle = -3.14 + (20 - ph)*3.14/10.;
	 scale = (10 - ph) * 0.4;
        }
        else
	if (ph < 30){ // 3
	 angle = -3.14;
	 scale = -4;	
	}
	else
	if (ph < 40){
	 angle = -3.14 - (30 - ph)*3.14/10.;
	 scale = (ph - 40) * 0.4; 
	}
    Bridge.LoadIdentity()
      .Translate(scale * pData->axis0)
      .Update();

    Wheels1.LoadIdentity()
      .RotateOx(angle, pData->axis1)
      .Translate(scale * pData->axis0)
      .Update();

    Wheels2.LoadIdentity()
      .RotateOx(-angle, pData->axis2)
      .Translate(scale * pData->axis0)
      .Update();
}

// Gate

void GateCallback ( CViewObjectBaseSet *pBaseSet,
                    CViewObjectBase    *pBase,
                    CViewObjectRef     *pRef
                  )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

        IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
	CViewBaseModifier0	&Wheels = pBase->KFSet().Mod0("Wheels");
        
	int    time =  (int)(Session::m_viewTime / 40.0 );
        double ph   =  Session::m_viewTime - time*40.0;
	double angle; 
	
	if (ph < 10){ // 1
	 angle = 0.;
        }
        else
        if (ph < 20){ // 2
	 angle = 3.14 - (30 - ph)*3.14/10;
        }
        else
	if (ph < 30){ // 3
	 angle = 3.14;
	}
	else
	if (ph < 40){
	 angle = 3.14 + (30 - ph)*3.14/10;
	}

    Wheels.LoadIdentity()
      .RotateOz(angle, pData->axis0)
      .Update();
}

// flag_ast.vbc

void FlagACallback( CViewObjectBaseSet *pBaseSet,
                    CViewObjectBase    *pBase,
                    CViewObjectRef     *pRef
                  )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
        CViewBaseModifier0	&Planes = pBase->KFSet().Mod0("Planes");
	CViewBaseModifier0	&Point1 = pBase->KFSet().Mod0("Point1");       
	CViewBaseModifier0	&Point2 = pBase->KFSet().Mod0("Point2");	
        CViewBaseModifier0	&Point3 = pBase->KFSet().Mod0("Point3");       
	CViewBaseModifier0	&Point4 = pBase->KFSet().Mod0("Point4");	

     Point1.LoadIdentity()
      .Translate(0.9 * sin(Session::m_viewTime*pData->speed + 3.14 * 0.5) * pData->axis1);
     
     Point2.LoadIdentity()
      .Translate(sin(Session::m_viewTime*pData->speed + 3.14 * 0.75) * pData->axis0);
     
     Point3.LoadIdentity()
      .Translate(1.1 * sin(Session::m_viewTime*pData->speed) * pData->axis0);
        
     Point4.LoadIdentity()
      .Translate(sin(Session::m_viewTime*pData->speed + 3.14 * 0.25) * pData->axis1);
         
     Planes.LoadIdentity()
      .Update();
}

// flag_con.vbc

void FlagCCallback( CViewObjectBaseSet *pBaseSet,
                    CViewObjectBase    *pBase,
                    CViewObjectRef     *pRef
                  )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
        CViewBaseModifier0	&Planes = pBase->KFSet().Mod0("Planes");
	CViewBaseModifier0	&Point1 = pBase->KFSet().Mod0("Point1");       
	CViewBaseModifier0	&Point2 = pBase->KFSet().Mod0("Point2");	
        CViewBaseModifier0	&Point3 = pBase->KFSet().Mod0("Point3");       
	CViewBaseModifier0	&Point4 = pBase->KFSet().Mod0("Point4");	

     Point1.LoadIdentity()
      .Translate(0.9 * sin(Session::m_viewTime*pData->speed + 3.14 * 0.5) * pData->axis1);
     
     Point2.LoadIdentity()
      .Translate(sin(Session::m_viewTime*pData->speed + 3.14 * 0.75) * pData->axis0);
     
     Point3.LoadIdentity()
      .Translate(1.1 * sin(Session::m_viewTime*pData->speed) * pData->axis0);
        
     Point4.LoadIdentity()
      .Translate(sin(Session::m_viewTime*pData->speed + 3.14 * 0.25) * pData->axis1);
         
     Planes.LoadIdentity()
      .Update();
}

// Mast_b0i.vbc

void MastCallback( CViewObjectBaseSet *pBaseSet,
                   CViewObjectBase    *pBase,
                   CViewObjectRef     *pRef
                 )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();

	CViewBaseModifier0	&Vorot = pBase->KFSet().Mod0("Vorot");

	Vorot.LoadIdentity()             
      .RotateOz(-pData->phase+Session::m_viewTime*pData->speed,pData->axis0)
      .Update();
}

// Colon.age

void g_staticInit4()
{
    CNameDecls &refNames = ZAV_Scene()->ObjRefNames();	// objects' names
    CNameDecl     &mills = refNames["srg_turb"];	// array of objects named "mill"
    int       nMillCount = mills.Count();	// number of objects named "mill"

    ct_ClassTableID  ctID = g_arena.searchSeanceClassTable("StaticObj");

    for( int i = 0 ; i < nMillCount ; i++ ) 
    {
         CViewObjectRef	 *pMill = (CViewObjectRef*)mills[i];
         CViewObjectBase &pBase = pMill->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.turb");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->phase = g_arena.context->rnd_f(3);
         data->speed = g_arena.context->rnd_f(1,4);

         data->axis0 = *(CFVector3*)pBase.Names().vertices["Axis0"][0];
         data->axis1 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis1"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis1"][1]
               ) * 0.5;

         pMill->SetAnimationCallback(MillCallback);
         pMill->SetUserAttrib(data);
    }

 
// Cln_f07.vbc  Zeppelin

    CNameDecl     &Zepps = refNames["cln_f07"];	// array of objects named "mill"
    int       nZeppsCount = Zepps.Count();	// number of objects named "mill"

    for( int i = 0 ; i < nZeppsCount ; i++ ) 
    {
         CViewObjectRef	 *pZepp = (CViewObjectRef*)Zepps[i];
         CViewObjectBase &pBase = pZepp->Model()->BaseSet(0).Base(0);
         KR_ObjectID oID = g_arena.newObject(ctID,"static.zeppelin");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));
         data->phase = 0;
         data->speed = 3;
         data->axis0 = *(CFVector3*)pBase.Names().vertices["Axis0"][0];
         data->axis1 = *(CFVector3*)pBase.Names().vertices["Axis1"][0];
         pZepp->SetAnimationCallback(ZeppCallback);
         pZepp->SetUserAttrib(data);
    }

// cln_b05.vbc crane

    CNameDecl     &cranes = refNames["cln_b05"];
    int       nCraneCount = cranes.Count();	

    for( int i = 0 ; i < nCraneCount ; i++ ) 
    {
         CViewObjectRef	 *pCrane = (CViewObjectRef*)cranes[i];
         CViewObjectBase &pBase = pCrane->Model()->BaseSet(0).Base(0);
         if(!i)pCrane->Model()->SetRadius(pCrane->Model()->Radius()*2.0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.crane");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));
         data->phase = 0;
         data->speed = 1;
         data->axis1 = *(CFVector3*)pBase.Names().vertices["Move"][1]-
		       *(CFVector3*)pBase.Names().vertices["Move"][0];
         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               ) * 0.5;

         pCrane->SetAnimationCallback(CraneCallback);
         pCrane->SetUserAttrib(data);
    }

// srg_cran.vbc my crane

    CNameDecl     &myCranes = refNames["srg_cran"];
    int       nMyCraneCount = myCranes.Count();	

    for( int i = 0 ; i < nMyCraneCount ; i++ ) 
    {
         CViewObjectRef	 *pMyCrane = (CViewObjectRef*)myCranes[i];
         CViewObjectBase &pBase = pMyCrane->Model()->BaseSet(0).Base(0);
         if(!i)pMyCrane->Model()->SetRadius(pMyCrane->Model()->Radius()*2.0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.mycrane");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));
         data->phase = g_arena.context->rnd_f(60);

         data->axis1 = *(CFVector3*)pBase.Names().vertices["Propeller"][0]-
		       *(CFVector3*)pBase.Names().vertices["Propeller"][1];
         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Crane"][0]
               +*(CFVector3*)pBase.Names().vertices["Crane"][1]
               ) * 0.5;
       
	  data->axis2 =
               (
                *(CFVector3*)pBase.Names().vertices["Wheel"][0]
               +*(CFVector3*)pBase.Names().vertices["Wheel"][1]
               ) * 0.5;

         pMyCrane->SetAnimationCallback(MyCraneCallback);
         pMyCrane->SetUserAttrib(data);
    }

// cln_b0c.vbc engine

    CNameDecl     &Engines = refNames["cln_b0c"];
    int       nEngineCount = Engines.Count();	

    for( int i = 0 ; i < nEngineCount ; i++ ) 
    {
         CViewObjectRef	 *pEngine = (CViewObjectRef*)Engines[i];
         CViewObjectBase &pBase = pEngine->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.engine");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));
         data->speed = 3;
         data->axis0 = (
			*(CFVector3*)pBase.Names().vertices["Axis0"][1]
			+ *(CFVector3*)pBase.Names().vertices["Axis0"][0] 
		       ) * 0.5;
         data->axis1 = (
                	*(CFVector3*)pBase.Names().vertices["Axis1"][0]
               		+*(CFVector3*)pBase.Names().vertices["Axis1"][1]
               	       ) * 0.5;
         data->axis2 = (
                	*(CFVector3*)pBase.Names().vertices["Axis0"][0]
               	        -*(CFVector3*)pBase.Names().vertices["Axis0"][2]
               	       ) * 0.5;

         pEngine->SetAnimationCallback(EngineCallback);
         pEngine->SetUserAttrib(data);
    }

// Bridges

    CNameDecl     &Bridges = refNames["wal_gbrg"];
    int       nBridgeCount = Bridges.Count();	

    for( int i = 0 ; i < nBridgeCount ; i++ ) 
    {
         CViewObjectRef	 *pBridge = (CViewObjectRef*)Bridges[i];
         CViewObjectBase &pBase = pBridge->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.bridge");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->axis0 = *(CFVector3*)pBase.Names().vertices["Axis0"][0]
		       -*(CFVector3*)pBase.Names().vertices["Axis0"][1]; 
         data->axis1 = (
                	*(CFVector3*)pBase.Names().vertices["Axis1"][0]
               		+*(CFVector3*)pBase.Names().vertices["Axis1"][1]
               	       ) * 0.5;
         data->axis2 = (
                	*(CFVector3*)pBase.Names().vertices["Axis2"][0]
               	        +*(CFVector3*)pBase.Names().vertices["Axis2"][1]
               	       ) * 0.5;

         pBridge->SetAnimationCallback(BridgeCallback);
         pBridge->SetUserAttrib(data);
    }

// Gates

    CNameDecl     &Gates = refNames["wal_ggts"];
    int       nGateCount = Gates.Count();	

    for( int i = 0 ; i < nGateCount ; i++ ) 
    {
         CViewObjectRef	 *pGate = (CViewObjectRef*)Gates[i];
         CViewObjectBase &pBase = pGate->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.gate");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));
         
         data->axis0 = (
                	*(CFVector3*)pBase.Names().vertices["Axis0"][0]
               	        +*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               	       ) * 0.5;

	 
         pGate->SetAnimationCallback(GateCallback);
         pGate->SetUserAttrib(data);
    }

// Flag_ast.vbc

    CNameDecl     &FlagAs = refNames["flag_ast"];
    int       nFlagACount = FlagAs.Count();	

    for( int i = 0 ; i < nFlagACount ; i++ ) 
    {
         CViewObjectRef	 *pFlagA = (CViewObjectRef*)FlagAs[i];
         CViewObjectBase &pBase = pFlagA->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.flaga");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

	 data->speed = g_arena.context->rnd_f(6,8);
         data->phase = g_arena.context->rnd_f(5);

         data->axis0 = *(CFVector3*)pBase.Names().vertices["Axis0"][0]
		       -*(CFVector3*)pBase.Names().vertices["Axis0"][1]; 

         data->axis1 = (*(CFVector3*)pBase.Names().vertices["Axis1"][0]
		       -*(CFVector3*)pBase.Names().vertices["Axis1"][1]) * 0.2; 

	 data->vbmi0 = &pBase.KFSet().Mod0("Planes");
	 data->vbmi1 = &pBase.KFSet().Mod0("Point1");       
	 data->vbmi2 = &pBase.KFSet().Mod0("Point2");	
         data->vbmi3 = &pBase.KFSet().Mod0("Point3");       
	 data->vbmi4 = &pBase.KFSet().Mod0("Point4");	

         pFlagA->SetAnimationCallback(Flags_Callback);
         pFlagA->SetUserAttrib(data);
    }

// Flag_con.vbc

    CNameDecl     &FlagCs = refNames["flag_con"];
    int       nFlagCCount = FlagCs.Count();	

    for( int i = 0 ; i < nFlagCCount ; i++ ) 
    {
         CViewObjectRef	 *pFlagC = (CViewObjectRef*)FlagCs[i];
         CViewObjectBase &pBase = pFlagC->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.flagc");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

	 data->speed = g_arena.context->rnd_f(6,8);
         data->phase = g_arena.context->rnd_f(5);

         data->axis0 = *(CFVector3*)pBase.Names().vertices["Axis0"][0]
		       -*(CFVector3*)pBase.Names().vertices["Axis0"][1]; 

         data->axis1 = (*(CFVector3*)pBase.Names().vertices["Axis1"][0]
		       -*(CFVector3*)pBase.Names().vertices["Axis1"][1]) * 0.2; 

	 data->vbmi0 = &pBase.KFSet().Mod0("Planes");
	 data->vbmi1 = &pBase.KFSet().Mod0("Point1");       
	 data->vbmi2 = &pBase.KFSet().Mod0("Point2");	
         data->vbmi3 = &pBase.KFSet().Mod0("Point3");       
	 data->vbmi4 = &pBase.KFSet().Mod0("Point4");	

         pFlagC->SetAnimationCallback(Flags_Callback);
         pFlagC->SetUserAttrib(data);
    }
    
// Mast_b0i.vbc

    CNameDecl     &masts = refNames["mast_b0i"];	// array of objects named "mill"
    int       nMastCount = masts.Count();	// number of objects named "mill"

    for( int i = 0 ; i < nMastCount ; i++ ) 
    {
         CViewObjectRef	 *pMast = (CViewObjectRef*)masts[i];
         CViewObjectBase &pBase = pMast->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.mast");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->phase = g_arena.context->rnd_f(3);
         data->speed = g_arena.context->rnd_f(2,4);

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               ) * 0.5;

         pMast->SetAnimationCallback(MastCallback);
         pMast->SetUserAttrib(data);
    }
}

//------------------------------------------------------------------------------
//  Level.5
//------------------------------------------------------------------------------   

void Rwy_crosCallback( CViewObjectBaseSet *pBaseSet,
                       CViewObjectBase    *pBase,
                       CViewObjectRef     *pRef
                     )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
        int    time =  (int)(Session::m_viewTime / 60.0 );
        double ph   =  Session::m_viewTime - time * 60.0;
        double angle;

        if ((ph < 10) || (ph > 40))
         angle = 0.0;
        else
        {
        if (ph < 14)
          angle = 3.14 * 80 / 180. - (14 - ph) * 3.14 * 80 / 180./ 4.;
        else
          if (ph > 36)
           angle = 3.14 * 80 / 180. + (36 - ph) * 3.14 * 80 / 180. / 4.; 
	  else 
           angle = 3.14 * 80 / 180.; 
	}
    
    pData->vbmi0->LoadIdentity()
      .RotateOz(angle, pData->axis0)
      .Update();

    pData->vbmi1->LoadIdentity()
      .RotateOz(-angle, pData->axis1)
      .Update();
}

void Gun_villCallback( CViewObjectBaseSet *pBaseSet,
                       CViewObjectBase    *pBase,
                       CViewObjectRef     *pRef
                     )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
	int    time =  (int)(Session::m_viewTime / 30.0 );
        double ph   =  Session::m_viewTime - time * 30.0;
        double angle, gAngle; 

        double shot =  Session::m_viewTime - (int)(Session::m_viewTime / 0.5) * 0.5;     
        double Scale = 0.0;

	if (ph < 3){
         angle = -20 * 3.14 /180. + ph * 35 * 3.14 / 180./3.;
	 gAngle = 0.;
	}else
	if (ph < 15){ // 1
         angle = 15 * 3.14 / 180;
         gAngle = 10 * 3.14 / 180.;
	 if (ph < 5){
          gAngle = 10 * 3.14 / 180. - (5 - ph) * 10 * 3.14 /180. /2.;
	 }else
	  if (ph > 13)
           gAngle = 10 * 3.14 / 180. + (13 - ph) * 10 * 3.14 /180. /2.;
          else{
           if (shot < 0.15)
            Scale =  -shot / 0.9;
           else if (shot < 0.5)
            Scale =  (shot - 0.5) / 2.7;
          }
	}
	else
        if (ph < 18){
         angle = 15 * 3.14 / 180. - (ph - 15)* 35 *3.14/180./3.;
         gAngle = 0.0;
	}else
	if (ph < 30){ // 2
         angle = -20. * 3.14 / 180.;
         gAngle = 10 * 3.14 / 180.;
	 if (ph < 20)
            gAngle = 10 * 3.14 / 180. - (20 - ph) * 10 * 3.14 /180. /2.;
	 else
	 if (ph > 28)
          gAngle = 10 * 3.14 / 180. + (28 - ph) * 10 * 3.14 /180. /2.;
         else{
           if (shot < 0.15)
            Scale =  -shot / 0.9;
           else if (shot < 0.5)
            Scale =  (shot - 0.5) / 2.7;
         }
	}
    
    pData->vbmi0->LoadIdentity()
      .Translate(Scale * pData->axis1)
      .RotateOx(-gAngle, pData->axis0)
      .RotateOy(angle, pData->axis0)
      .Update();
}

void Gun_civCallback( CViewObjectBaseSet *pBaseSet,
                      CViewObjectBase    *pBase,
                      CViewObjectRef     *pRef
                    )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
	int    time =  (int)(Session::m_viewTime / 30.0 );
        double ph   =  Session::m_viewTime - time * 30.0;
        double angle, gAngle; 

        double shot =  Session::m_viewTime - (int)(Session::m_viewTime);     
        double Scale = 0.0;

	if (ph < 3){
	 angle = 3.14 + (ph - 3)*3.14/3.;
	 gAngle = 0.;
	}else
	if (ph < 15){ // 1
	 angle = 3.14;
         gAngle = 30 * 3.14 / 180.;
	 if (ph < 5){
          gAngle = 30 * 3.14 / 180. - (5 - ph) * 30 * 3.14 /180. /2.;
	 }else
	  if (ph > 13)
           gAngle = 30 * 3.14 / 180. + (13 - ph) * 30 * 3.14 /180. /2.;
          else{
           if (shot < 0.15)
            Scale =  -shot / 0.9;
           else if (shot < 0.5)
            Scale =  (shot - 0.5) / 2.7;
          }
	}
	else
        if (ph < 18){
	 angle = - (ph - 18)*3.14/3.;
         gAngle = 0.0;
	}else
	if (ph < 30){ // 2
         angle = 0.;
         gAngle = 40 * 3.14 / 180.;
	 if (ph < 20)
            gAngle = 40 * 3.14 / 180. - (20 - ph) * 40 * 3.14 /180. /2.;
	 else
	 if (ph > 28)
          gAngle = 40 * 3.14 / 180. + (28 - ph) * 40 * 3.14 /180. /2.;
         else{
           if (shot < 0.15)
            Scale =  -shot / 0.9;
           else if (shot < 0.5)
            Scale =  (shot - 0.5) / 2.7;
         }
	}
    
    pData->vbmi0->LoadIdentity()
      .RotateOy(angle, pData->axis1)
      .Update();

    pData->vbmi1->LoadIdentity()
      .RotateOx(-gAngle, pData->axis0)
      .RotateOy(angle, pData->axis1)
      .Update();

    pData->vbmi2->LoadIdentity()
      .Translate(Scale * pData->axis2)
      .RotateOx(-gAngle, pData->axis0)
      .RotateOy(angle, pData->axis1)
      .Update();
}

void Wtr_b05Callback( CViewObjectBaseSet *pBaseSet,
                      CViewObjectBase    *pBase,
                      CViewObjectRef     *pRef
                    )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();

	pData->vbmi0->LoadIdentity()             
      .RotateOz(0.4 * sin(Session::m_viewTime*pData->speed * 2.), pData->axis0)
      .Update();
       	pData->vbmi1->LoadIdentity()             
      .RotateOz(Session::m_viewTime*pData->speed * 1.5,pData->axis1)
      .Update();
	pData->vbmi2->LoadIdentity()             
      .RotateOz(Session::m_viewTime*pData->speed,pData->axis2)
      .Update();
}

void Wtr_f02Callback( CViewObjectBaseSet *pBaseSet,
                      CViewObjectBase    *pBase,
                      CViewObjectRef     *pRef
                    )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
	int    time =  (int)(Session::m_viewTime / 44.0 );
        double ph   =  Session::m_viewTime - time*44.0;
	double angle, armAngle, hookAngle, cordLag;

	if (ph < 10){ // 1
	 angle = 90.0 * 3.14 / 180;
	 if (ph < 3){
	  armAngle = -3.14 * 40 / 180. + (3 - ph) * 3.14 * 40 / 180. / 3.;
	  cordLag = (3 - ph) * 5. * 3.14 / 180. / 3. * cos(ph * 5.);
	 }
	 else{
	  cordLag = 0.;
	  if (ph > 7)
           armAngle = -3.14 * 40 / 180. - (7 - ph) * 3.14 * 40 / 180. / 3.;
	  else{ 
           armAngle = -3.14 * 40 / 180.;
	   if (ph < 4)
	      hookAngle = -3.14 * 40 / 180. + (4 - ph) * 3.14 * 40 / 180.;
	   else
	   if (ph > 6)
              hookAngle = -3.14 * 40 / 180. - (6 - ph) * 3.14 * 40 / 180.;
	   else 
              hookAngle = -3.14 * 40 / 180.;
	  }
	 }
	}
	else
	if (ph < 14){
	 angle = 90.0 * 3.14 / 180 - (ph - 10)*80*3.14/4/180.;
         if (ph < 12)
	  cordLag = 5. * 3.14 / 180. - (12 - ph) * 5. * 3.14 / 180. / 2.;
	 else
	  cordLag = 5. * 3.14 / 180.;
	}
	else
        if (ph < 24){ // 2
         angle = 10 * 3.14 /180.;
	 if (ph < 17){
          armAngle = -3.14 * 40 / 180. + (17 - ph) * 3.14 * 40 / 180./ 3.;
	  cordLag = (17 - ph) * 5. * 3.14 / 180. / 3. * cos((ph - 14) * 5.);
	 }
	 else{
	  cordLag = 0.;
	  if (ph > 21)
           armAngle = -3.14 * 40 / 180. - (21 - ph) * 3.14 * 40 / 180. / 3.; 
	  else{ 
           armAngle = -3.14 * 40 / 180.; 
	   if (ph < 18)
	      hookAngle = -3.14 * 40 / 180. + (18 - ph) * 3.14 * 40 / 180.;
	   else
	   if (ph > 20)
              hookAngle = -3.14 * 40 / 180. - (20 - ph) * 3.14 * 40 / 180.;
	   else 
              hookAngle = -3.14 * 40 / 180.;
	  }
	 }
	}
	else
	if (ph < 30){
	 angle = 10*3.14/180. + (ph - 24)*240*3.14/6/180.;
         if (ph < 26)
	  cordLag = -5. * 3.14 / 180. + (26 - ph) * 5. * 3.14 / 180. / 2.;
	 else
	  cordLag = -5. * 3.14 / 180.;
	}
        else
	if (ph < 40){ // 3
	 angle = 250.0 * 3.14 / 180;
	 if (ph < 33){
          armAngle = -3.14 * 40 / 180. + (33 - ph) * 3.14 * 40 / 180. / 3.;
	  cordLag = -(33 - ph) * 5. * 3.14 / 180. / 3. * cos((ph - 30) * 5.); 
	 }
	 else{
	  cordLag = 0.; 
	  if (ph > 37)
           armAngle = -3.14 * 40 / 180. - (37 - ph) * 3.14 * 40 / 180./ 3.;         
	  else{ 
           armAngle = -3.14 * 40 / 180.; 
	   if (ph < 34)
	      hookAngle = -3.14 * 40 / 180. + (34 - ph) * 3.14 * 40 / 180.;
	   else
	   if (ph > 36)
              hookAngle = -3.14 * 40 / 180. - (36 - ph) * 3.14 * 40 / 180.;
	   else 
              hookAngle = -3.14 * 40 / 180.;
	  }
	 }
	}
	else
	if (ph < 44){
	 angle = 250.0 * 3.14 / 180 - (ph - 40)*160*3.14/4/180.;
         if (ph < 42)
	  cordLag = 5. * 3.14 / 180. - (42 - ph) * 5. * 3.14 / 180. / 2.;
	 else
	  cordLag = 5. * 3.14 / 180.;
        }
    
    pData->vbmi0->LoadIdentity()
      .RotateOx(4. * armAngle, pData->axis0)
      .RotateOy(angle, pData->axis1)
      .Update();

    pData->vbmi1->LoadIdentity()
      .RotateOy(angle, pData->axis1)
      .Update();

    pData->vbmi2->LoadIdentity()
      .RotateOx(armAngle, pData->axis3)
      .RotateOy(angle, pData->axis1)
      .Update();
                         
    pData->vbmi3->LoadIdentity()                // Cord
      .RotateOy(cordLag, pData->axis1) 
      .RotateOx(-armAngle, pData->axis2)
      .RotateOx(armAngle, pData->axis3) 		
      .RotateOy(angle, pData->axis1)
      .Update();	
   
    pData->vbmi4->LoadIdentity()
      .RotateOy(cordLag, pData->axis1)
      .RotateOx(-hookAngle, pData->axis4)	
      .RotateOx(-armAngle, pData->axis2)
      .RotateOx(armAngle, pData->axis3)
      .RotateOy(angle, pData->axis1)
      .Update();

    pData->vbmi5->LoadIdentity()
      .RotateOy(cordLag, pData->axis1)
      .RotateOx(hookAngle, pData->axis4)
      .RotateOx(-armAngle, pData->axis2)
      .RotateOx(armAngle, pData->axis3) 
      .RotateOy(angle, pData->axis1)
      .Update();
}

void Wtr_f03aCallback( CViewObjectBaseSet *pBaseSet,
                       CViewObjectBase    *pBase,
                       CViewObjectRef     *pRef
                     )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();

        double S = Session::m_viewTime * pData->speed + pData->phase;
        double S1 = 15 * 3.14 / 180. * (1 - sin(S + 3.14 * 0.5));
        double S2 = 0.35 * sin(S + 3.14);

	pData->vbmi0->LoadIdentity()             
      .RotateOx(0.35 * sin(S + 3.14), pData->axis0)
      .Update();

        pData->vbmi1->LoadIdentity()             
      .RotateOx(-S ,pData->axis3)
      .Update();

        pData->vbmi2->LoadIdentity()
      .RotateOx(S1 - S2, pData->axis2)
      .RotateOx(S2, pData->axis0)
      .Update();

        pData->vbmi3->LoadIdentity()
      .RotateOx(S1 + S ,pData->axis1)
      .RotateOx(-S ,pData->axis3)
      .Update();

}

void Wtr_f04Callback( CViewObjectBaseSet *pBaseSet,
                      CViewObjectBase    *pBase,
                      CViewObjectRef     *pRef
                    )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();

	pData->vbmi0->LoadIdentity()             
        .RotateOz(Session::m_viewTime*pData->speed,pData->axis0)
	.Update();

	pData->vbmi1->LoadIdentity()             
        .RotateOz(Session::m_viewTime*pData->speed,pData->axis1)
	.Update();
	
	pData->vbmi2->LoadIdentity()  
        .RotateOz(Session::m_viewTime*pData->speed,pData->axis2)           
        .Update();
	
	pData->vbmi3->LoadIdentity()             
        .RotateOz(30 * 3.14 / 180. * sin(Session::m_viewTime*pData->speed) - Session::m_viewTime*pData->speed,pData->axis3)
        .RotateOz(Session::m_viewTime*pData->speed,pData->axis2)
	.Update();
	
	pData->vbmi4->LoadIdentity()  
        .RotateOz(-30 * 3.14 / 180. * sin(Session::m_viewTime*pData->speed)-Session::m_viewTime*pData->speed,pData->axis4)
        .RotateOz(Session::m_viewTime*pData->speed,pData->axis2)
        .Update();
	
	pData->vbmi5->LoadIdentity()             
	.Translate( 0.34 * pData->axis5 * (-1 + sin(Session::m_viewTime*pData->speed + 0.5 * 3.14)))
	.Update();

	pData->vbmi6->LoadIdentity()             
	.Translate( 0.4 * pData->axis5 * (1 - sin(Session::m_viewTime*pData->speed + 0.5 * 3.14)))
	.Update();
	
	pData->vbmi7->LoadIdentity()             
	.Translate( 0.02 * pData->axis5 * sin(Session::m_viewTime * 35.))
	.Update();

}

void Flg_civCallback( CViewObjectBaseSet *pBaseSet,
                      CViewObjectBase    *pBase,
                      CViewObjectRef     *pRef
                    )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
     pData->vbmi1->LoadIdentity()
      .Translate(0.8 * (-5 + 2 * sin(Session::m_viewTime*pData->speed + 3.14 * 0.2)) * pData->axis1)	
      .Translate(0.9 * sin(Session::m_viewTime*pData->speed + 3.14 * 0.5) * pData->axis0);
     
     pData->vbmi2->LoadIdentity()
      .Translate((-6 + 2.5 * sin(Session::m_viewTime*pData->speed + 3.14 * 0.8)) * pData->axis1)   
      .Translate(sin(Session::m_viewTime*pData->speed + 3.14 * 0.75) * pData->axis0);
     
     pData->vbmi3->LoadIdentity()
      .Translate(1.1 * (-7.5 + 4. * sin(Session::m_viewTime*pData->speed+ 3.14 * 0.1)) * pData->axis1)   	
      .Translate(1.1 * sin(Session::m_viewTime*pData->speed) * pData->axis0);
        
     pData->vbmi4->LoadIdentity()
      .Translate((-11 + 7. * sin(Session::m_viewTime*pData->speed + 3.14 * 0.4)) * pData->axis1)	
      .Translate(sin(Session::m_viewTime*pData->speed + 3.14 * 0.25) * pData->axis0);
         
     pData->vbmi0->LoadIdentity()
      .Update();
}

void Flg_villCallback( CViewObjectBaseSet *pBaseSet,
                       CViewObjectBase    *pBase,
                       CViewObjectRef     *pRef
                     )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
     pData->vbmi1->LoadIdentity()
      .Translate(0.8 * (-5 + 2 * sin(Session::m_viewTime*pData->speed + 3.14 * 0.2)) * pData->axis1)	
      .Translate(0.9 * sin(Session::m_viewTime*pData->speed + 3.14 * 0.5) * pData->axis0);
     
     pData->vbmi2->LoadIdentity()
      .Translate((-6 + 2.5 * sin(Session::m_viewTime*pData->speed + 3.14 * 0.8)) * pData->axis1)   
      .Translate(sin(Session::m_viewTime*pData->speed + 3.14 * 0.75) * pData->axis0);
     
     pData->vbmi3->LoadIdentity()
      .Translate(1.1 * (-7.5 + 4. * sin(Session::m_viewTime*pData->speed+ 3.14 * 0.1)) * pData->axis1)   	
      .Translate(1.1 * sin(Session::m_viewTime*pData->speed) * pData->axis0);
        
     pData->vbmi4->LoadIdentity()
      .Translate((-11 + 7. * sin(Session::m_viewTime*pData->speed + 3.14 * 0.4)) * pData->axis1)	
      .Translate(sin(Session::m_viewTime*pData->speed + 3.14 * 0.25) * pData->axis0);
     
     pData->vbmi5->LoadIdentity()
      .Translate((-7 + 3.5 * sin(Session::m_viewTime*pData->speed + 3.14 * 0.35)) * pData->axis1)	
      .Translate(sin(Session::m_viewTime*pData->speed + 3.14 * 0.4) * pData->axis0);
         
     pData->vbmi0->LoadIdentity()
      .Update();
}

void Rwy_smphCallback( CViewObjectBaseSet *pBaseSet,
                       CViewObjectBase    *pBase,
                       CViewObjectRef     *pRef
                     )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
        int    time =  (int)(Session::m_viewTime / 60.0 );
        double ph   =  Session::m_viewTime - time * 60.0;
        double angle;

        if ((ph < 10) || (ph > 40))
         angle = 0.0;
        else
        {
        if (ph < 14)
          angle = 3.14 * 60 / 180. - (14 - ph) * 3.14 * 60 / 180./ 4.;
        else
          if (ph > 36)
           angle = 3.14 * 60 / 180. + (36 - ph) * 3.14 * 60 / 180. / 4.; 
	  else 
           angle = 3.14 * 60 / 180.; 
	}
    
    pData->vbmi0->LoadIdentity()
      .RotateOz(angle, pData->axis0)
      .Update();

    pData->vbmi1->LoadIdentity()
      .RotateOz(-angle, pData->axis1)
      .RotateOz(angle, pData->axis0)
      .Update();
}

void g_staticInit5()
{                                       
    CNameDecls &refNames = ZAV_Scene()->ObjRefNames();  // objects' names
    ct_ClassTableID  ctID = g_arena.searchSeanceClassTable("StaticObj");
    
    CNameDecl  &Wtr_b05Arr = refNames["wtr_b05"];      // array of objects named "mill"
    int       nWtr_b05Count = Wtr_b05Arr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nWtr_b05Count ; i++ ) 
    {
         CViewObjectRef  *pWtr_b05 = (CViewObjectRef*)Wtr_b05Arr[i];
         CViewObjectBase &pBase = pWtr_b05->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Wtr_b05");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

	 data->speed = 1.;

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               ) * 0.5;

         data->axis1 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis1"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis1"][1]
               ) * 0.5;

	 data->axis2 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis2"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis2"][1]
               ) * 0.5;

	 data->vbmi0 = &pBase.KFSet().Mod0("Pendulum");
	 data->vbmi1 = &pBase.KFSet().Mod0("Wheel1");
	 data->vbmi2 = &pBase.KFSet().Mod0("Wheel2");

         pWtr_b05->SetAnimationCallback(Wtr_b05Callback);
         pWtr_b05->SetUserAttrib(data);
    }

    CNameDecl  &Wtr_f02Arr = refNames["wtr_f02"];      // array of objects named "mill"
    int       nWtr_f02Count = Wtr_f02Arr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nWtr_f02Count ; i++ ) 
    {
         CViewObjectRef  *pWtr_f02 = (CViewObjectRef*)Wtr_f02Arr[i];
         CViewObjectBase &pBase = pWtr_f02->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Wtr_f02");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->speed = 1.;

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               ) * 0.5;
         data->axis1 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis1"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis1"][1]
               ) * 0.5;
	 
	 data->axis2 = *(CFVector3*)pBase.Names().vertices["Axis2"][0];
	 
	 
         data->axis3 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis3"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis3"][1]
               ) * 0.5;

	 data->axis4 = *(CFVector3*)pBase.Names().vertices["Axis2"][1];

         data->vbmi0 = &pBase.KFSet().Mod0("Wheels");
         data->vbmi1 = &pBase.KFSet().Mod0("Body");
         data->vbmi2 = &pBase.KFSet().Mod0("Arm");
         data->vbmi3 = &pBase.KFSet().Mod0("Cord");
         data->vbmi4 = &pBase.KFSet().Mod0("Hook1");
         data->vbmi5 = &pBase.KFSet().Mod0("Hook2");

	 pWtr_f02->SetAnimationCallback(Wtr_f02Callback);
         pWtr_f02->SetUserAttrib(data);
    }

    CNameDecl  &Wtr_f03aArr = refNames["wtr_f03a"];      // array of objects named "mill"
    int       nWtr_f03aCount = Wtr_f03aArr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nWtr_f03aCount ; i++ ) 
    {
         CViewObjectRef  *pWtr_f03a = (CViewObjectRef*)Wtr_f03aArr[i];
         CViewObjectBase &pBase = pWtr_f03a->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Wtr_f03a");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

	 data->speed = 1.;
         data->phase = g_arena.context->rnd_f(50);

         data->axis0 =
                *(CFVector3*)pBase.Names().vertices["Axis0"][0];

         data->axis1 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis1"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis1"][1]
               ) * 0.5;

	 data->axis2 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis2"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis2"][1]
               ) * 0.5;

         data->axis3 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis3"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis3"][1]
               ) * 0.5;

         data->vbmi0 = &pBase.KFSet().Mod0("Arm");
         data->vbmi1 = &pBase.KFSet().Mod0("Wheel");
         data->vbmi2 = &pBase.KFSet().Mod0("TShatun");
         data->vbmi3 = &pBase.KFSet().Mod0("BShatun");
         
         pWtr_f03a->SetAnimationCallback(Wtr_f03aCallback);
         pWtr_f03a->SetUserAttrib(data);
    }

    CNameDecl  &Wtr_f04Arr = refNames["wtr_f04"];      // array of objects named "mill"
    int       nWtr_f04Count = Wtr_f04Arr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nWtr_f04Count ; i++ ) 
    {
         CViewObjectRef  *pWtr_f04 = (CViewObjectRef*)Wtr_f04Arr[i];
         CViewObjectBase &pBase = pWtr_f04->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Wtr_f04");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

	 data->speed = g_arena.context->rnd_f(4,6);

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["TWheel"][0]
               +*(CFVector3*)pBase.Names().vertices["TWheel"][1]
               ) * 0.5;

         data->axis1 =
               (
                *(CFVector3*)pBase.Names().vertices["BWheel"][0]
               +*(CFVector3*)pBase.Names().vertices["BWheel"][1]
               ) * 0.5;

         data->axis2 =
               (
                *(CFVector3*)pBase.Names().vertices["Kardan"][0]
               +*(CFVector3*)pBase.Names().vertices["Kardan"][1]
               ) * 0.5;  
 
         data->axis3 =
                *(CFVector3*)pBase.Names().vertices["Porsh1B"][0];

         data->axis4 =
                *(CFVector3*)pBase.Names().vertices["Porsh2B"][0];

	 data->axis5 =
                *(CFVector3*)pBase.Names().vertices["Porsh"][0]
                -*(CFVector3*)pBase.Names().vertices["Porsh"][1];

	 data->vbmi0 = &pBase.KFSet().Mod0("TWheel");
         data->vbmi1 = &pBase.KFSet().Mod0("BWheel");
	 data->vbmi2 = &pBase.KFSet().Mod0("Kardan");
	 data->vbmi3 = &pBase.KFSet().Mod0("Porsh1B");
	 data->vbmi4 = &pBase.KFSet().Mod0("Porsh2B");
	 data->vbmi5 = &pBase.KFSet().Mod0("Porsh1T");
	 data->vbmi6 = &pBase.KFSet().Mod0("Porsh2T");
	 data->vbmi7 = &pBase.KFSet().Mod0("Tube");

         pWtr_f04->SetAnimationCallback(Wtr_f04Callback);
         pWtr_f04->SetUserAttrib(data);
    }

    CNameDecl     &Flg_civArr = refNames["flg_civ"];
    int       nFlg_civCount = Flg_civArr.Count();

    for( int i = 0 ; i < nFlg_civCount ; i++ )
    {
         CViewObjectRef	 *pFlg_civ = (CViewObjectRef*)Flg_civArr[i];
         CViewObjectBase &pBase = pFlg_civ->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.flg_civ");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

	 data->speed = g_arena.context->rnd_f(6,8);

         data->axis0 = *(CFVector3*)pBase.Names().vertices["Axis0"][0]
		       -*(CFVector3*)pBase.Names().vertices["Axis0"][1]; 

         data->axis1 = (*(CFVector3*)pBase.Names().vertices["Axis1"][0]
		       -*(CFVector3*)pBase.Names().vertices["Axis1"][1]) * 0.2; 

	 data->vbmi0 = &pBase.KFSet().Mod0("Planes");
	 data->vbmi1 = &pBase.KFSet().Mod0("Point1");       
	 data->vbmi2 = &pBase.KFSet().Mod0("Point2");	
         data->vbmi3 = &pBase.KFSet().Mod0("Point3");       
	 data->vbmi4 = &pBase.KFSet().Mod0("Point4");	

         pFlg_civ->SetAnimationCallback(Flg_civCallback);
         pFlg_civ->SetUserAttrib(data);
    }

    CNameDecl     &Flg_villArr = refNames["flg_vill"];
    int       nFlg_villCount = Flg_villArr.Count();

    for( int i = 0 ; i < nFlg_villCount ; i++ )
    {
         CViewObjectRef  *pFlg_vill = (CViewObjectRef*)Flg_villArr[i];
         CViewObjectBase &pBase = pFlg_vill->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Flg_vill");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

	 data->speed = g_arena.context->rnd_f(6,8);

         data->axis0 = (*(CFVector3*)pBase.Names().vertices["Axis0"][0]
		       -*(CFVector3*)pBase.Names().vertices["Axis0"][1]) * 2.; 

         data->axis1 = (*(CFVector3*)pBase.Names().vertices["Axis1"][0]
		       -*(CFVector3*)pBase.Names().vertices["Axis1"][1]) * 0.2; 

	 data->vbmi0 = &pBase.KFSet().Mod0("Planes");
	 data->vbmi1 = &pBase.KFSet().Mod0("Point1");       
	 data->vbmi2 = &pBase.KFSet().Mod0("Point2");	
         data->vbmi3 = &pBase.KFSet().Mod0("Point3");       
	 data->vbmi4 = &pBase.KFSet().Mod0("Point4");
         data->vbmi5 = &pBase.KFSet().Mod0("Point5"); 

         pFlg_vill->SetAnimationCallback(Flg_villCallback);
         pFlg_vill->SetUserAttrib(data);
    }

    CNameDecl  &Rwy_smphArr = refNames["rwy_smph"];      // array of objects named "mill"
    int       nRwy_smphCount = Rwy_smphArr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nRwy_smphCount ; i++ ) 
    {
         CViewObjectRef  *pRwy_smph = (CViewObjectRef*)Rwy_smphArr[i];
         CViewObjectBase &pBase = pRwy_smph->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Rwy_smph");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               ) * 0.5;

         data->axis1 =
                *(CFVector3*)pBase.Names().vertices["Axis1"][0];

         data->vbmi0 = &pBase.KFSet().Mod0("Sign");
         data->vbmi1 = &pBase.KFSet().Mod0("CWeight");

         pRwy_smph->SetAnimationCallback(Rwy_smphCallback);
         pRwy_smph->SetUserAttrib(data);
    }

    CNameDecl  &Gun_civArr = refNames["gun_civ"];      // array of objects named "mill"
    int       nGun_civCount = Gun_civArr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nGun_civCount ; i++ ) 
    {
         CViewObjectRef  *pGun_civ = (CViewObjectRef*)Gun_civArr[i];
         CViewObjectBase &pBase = pGun_civ->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Gun_civ");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               ) * 0.5;

         data->axis1 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis1"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis1"][1]
               ) * 0.5;

         data->axis2 =
                *(CFVector3*)pBase.Names().vertices["Axis2"][0]
               -*(CFVector3*)pBase.Names().vertices["Axis2"][1];

         data->vbmi0 = &pBase.KFSet().Mod0("Base");
         data->vbmi1 = &pBase.KFSet().Mod0("Body");
         data->vbmi2 = &pBase.KFSet().Mod0("Gun");

         pGun_civ->SetAnimationCallback(Gun_civCallback);
         pGun_civ->SetUserAttrib(data);
    }

    /*=================================
    CNameDecl  &gun_ksaArr = refNames["gun_ksa"];      // array of objects named "mill"
    int       ngun_ksaCount = gun_ksaArr.Count(); // number of objects named "mill"

    for( i = 0 ; i < ngun_ksaCount ; i++ ) 
    {
         CViewObjectRef  *pgun_ksa = (CViewObjectRef*)gun_ksaArr[i];
         CViewObjectBase &pBase = pgun_ksa->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.gun_ksa");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               ) * 0.5;

         data->axis1 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis1"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis1"][1]
               ) * 0.5 - *(CFVector3*)pBase.Names().vertices["Axis1"][2];

         data->vbmi0 = &pBase.KFSet().Mod0("Gun");

         pgun_ksa->SetAnimationCallback(Bld_a03Callback);
         pgun_ksa->SetUserAttrib(data);
    }
    ====================================*/

    CNameDecl  &Gun_villArr = refNames["gun_vill"];      // array of objects named "mill"
    int       nGun_villCount = Gun_villArr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nGun_villCount ; i++ ) 
    {
         CViewObjectRef  *pGun_vill = (CViewObjectRef*)Gun_villArr[i];
         CViewObjectBase &pBase = pGun_vill->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Gun_vill");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               ) * 0.5;

         data->axis1 =
                *(CFVector3*)pBase.Names().vertices["Axis1"][1]
               -*(CFVector3*)pBase.Names().vertices["Axis1"][0];

         data->vbmi0 = &pBase.KFSet().Mod0("Gun");

         pGun_vill->SetAnimationCallback(Gun_villCallback);
         pGun_vill->SetUserAttrib(data);
    }

    CNameDecl  &Gun_srgArr = refNames["gun_srg"];      // array of objects named "mill"
    int       nGun_srgCount = Gun_srgArr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nGun_srgCount ; i++ ) 
    {
         CViewObjectRef  *pGun_srg = (CViewObjectRef*)Gun_srgArr[i];
         CViewObjectBase &pBase = pGun_srg->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Gun_srg");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               ) * 0.5;

         data->axis1 =
                *(CFVector3*)pBase.Names().vertices["Axis1"][1]
               -*(CFVector3*)pBase.Names().vertices["Axis1"][0];

         data->vbmi0 = &pBase.KFSet().Mod0("Gun");

         pGun_srg->SetAnimationCallback(Gun_villCallback);
         pGun_srg->SetUserAttrib(data);
    }  

    CNameDecl  &Rwy_crosArr = refNames["rwy_cros"];      // array of objects named "mill"
    int       nRwy_crosCount = Rwy_crosArr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nRwy_crosCount ; i++ ) 
    {
         CViewObjectRef  *pRwy_cros = (CViewObjectRef*)Rwy_crosArr[i];
         CViewObjectBase &pBase = pRwy_cros->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Rwy_cros");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->axis0 =
                *(CFVector3*)pBase.Names().vertices["Axis1"][0];

         data->axis1 =
                *(CFVector3*)pBase.Names().vertices["Axis2"][0];

         data->vbmi0 = &pBase.KFSet().Mod0("Shlakbaum1");
         data->vbmi1 = &pBase.KFSet().Mod0("ShlakBaum2");

         pRwy_cros->SetAnimationCallback(Rwy_crosCallback);
         pRwy_cros->SetUserAttrib(data);
    }

}

//------------------------------------------------------------------------------
//  Level.6
//------------------------------------------------------------------------------   

void Blg_twrCallback( CViewObjectBaseSet *pBaseSet,
                      CViewObjectBase    *pBase,
                      CViewObjectRef     *pRef
                    )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
        double angle[4] = {0., 0., 0., 0.};
        
	for (int j = 0; j < 4; j++) {
          int    time =  (int)((Session::m_viewTime + j * 0.25 + pData->phase) / 6.0 );
          double ph   =  Session::m_viewTime + j * 0.25 + pData->phase - time * 6.0;
	  if (ph < 0.25)
            angle[j] = ph * 3.14 * 10 / 180. / 0.25;
          else
            if (ph < 0.5)
              angle[j] = 3.14 * 10 / 180. + (0.25 - ph) * 3.14 * 10 / 180. / 0.25;
	    else
	      if ((ph < 1.25) && (ph >= 1))
            	angle[j] = (ph - 1) * 3.14 * 10 / 180./ 0.25;
              else
            	if ((ph < 1.5) && (ph >= 1.25))
                  angle[j] = 3.14 * 10 / 180. + (1.25 - ph) * 3.14 * 10 / 180./ 0.25;
	}
    pData->vbmi1->LoadIdentity()     	
      .RotateOx(-angle[0], pData->axis1)
      .Update();

    pData->vbmi2->LoadIdentity()
      .RotateOz(angle[1], pData->axis2)
      .Update();

    pData->vbmi3->LoadIdentity()     	
      .RotateOx(angle[2], pData->axis3)
      .Update();

    pData->vbmi4->LoadIdentity()
      .RotateOz(-angle[3], pData->axis4)
      .Update(); 
 }

void Blg_lnchCallback( CViewObjectBaseSet *pBaseSet,
                       CViewObjectBase    *pBase,
                       CViewObjectRef     *pRef
                     )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
        int    time =  (int)(Session::m_viewTime / 10.0 );
        double ph   =  Session::m_viewTime - time * 10.0;
        double angle;
        
        if (ph < 0.5)
          angle = -3.14 * 30 / 180. + (0.5 - ph) * 3.14 * 105 / 180. /0.5;
        else
          if (ph < 3.5)
           angle = 3.14 * 75 / 180. - (3.5 - ph) * 3.14 * 105 / 180. / 3.; 
	  else 
           angle = 3.14 * 75 / 180.; 
        
    pData->vbmi0->LoadIdentity()
      .Rotate(pData->axis7 - pData->axis0, angle, pData->axis0)
      .Update();
    
    pData->vbmi1->LoadIdentity()     	
      .RotateOx(-angle, pData->axis1)
      .Update();

    pData->vbmi2->LoadIdentity()
      .Rotate(pData->axis1 - pData->axis2, angle, pData->axis2)
      .Update();
    
    pData->vbmi3->LoadIdentity()
      .RotateOz(angle, pData->axis3)
      .Update();

    pData->vbmi4->LoadIdentity()
      .Rotate(pData->axis3 - pData->axis4, angle, pData->axis4)
      .Update();
    
    pData->vbmi5->LoadIdentity()
      .RotateOx(angle, pData->axis5)
      .Update();

    pData->vbmi6->LoadIdentity()
      .Rotate(pData->axis5 - pData->axis6, angle, pData->axis6)
      .Update();
    
    pData->vbmi7->LoadIdentity()
      .RotateOz(-angle, pData->axis7)
      .Update();
}

void Gun_gunCallback( CViewObjectBaseSet *pBaseSet,
                      CViewObjectBase    *pBase,
                      CViewObjectRef     *pRef
                    )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;

	IStaticObj *pData = (IStaticObj*)pRef->GetUserAttrib();
	   
	int    time =  (int)(Session::m_viewTime / 30.0 );
        double ph   =  Session::m_viewTime - time * 30.0;
	double angle, gAngle, scale1 = 0.0, scale2 = 0.0; 

	if (ph < 3){
	 angle = 3.14 + (ph - 3)*3.14/3.;
	 gAngle = 0.;
	}else
	if (ph < 15){ // 1
	 angle = 3.14;
	 gAngle = 50 * 3.14 / 180.;
	 if (ph < 5){
	  gAngle = 50 * 3.14 / 180. - (5 - ph) * 50 * 3.14 /180. /2.;
	 }else
	  if (ph > 13)
	   gAngle = 50 * 3.14 / 180. + (13 - ph) * 50 * 3.14 /180. /2.;
	  else{
	   if (ph < 5.1)
	    scale1 = (5 - ph) / 0.2;
           else
	    if (ph < 6)
	     scale1 = (ph - 6) / 1.8;
            else
	     if (ph < 6.1)
	    scale2 = (6 - ph) / 0.2;
           else
	    if (ph < 7)
	     scale2 = (ph - 7) / 1.8;	
	  }
	}
	else
        if (ph < 18){
	 angle = - (ph - 18)*3.14/3.;
         gAngle = 0.0;
	}else
	if (ph < 30){ // 2
         angle = 0.;
	 gAngle = 70 * 3.14 / 180.;
	 if (ph < 20)
	    gAngle = 70 * 3.14 / 180. - (20 - ph) * 70 * 3.14 /180. /2.;
	 else
	 if (ph > 28)
	  gAngle = 70 * 3.14 / 180. + (28 - ph) * 70 * 3.14 /180. /2.;	
	 else{
	   if (ph < 20.1)
	    scale1 = (20 - ph) / 0.2;
           else
	    if (ph < 21)
	     scale1 = (ph - 21) / 1.8;
            else
	     if (ph < 21.1)
	    scale2 = (21 - ph) / 0.2;
           else
	    if (ph < 22)
	     scale2 = (ph - 22) / 1.8;	
	  }
	}
    
    pData->vbmi0->LoadIdentity()
      .RotateOy(angle, pData->axis0)
      .Update();

    pData->vbmi1->LoadIdentity()
      .RotateOx(-gAngle, pData->axis1)
      .RotateOy(angle, pData->axis0)
      .Update();

    pData->vbmi2->LoadIdentity()
      .Translate( scale1 * pData->axis2)
      .RotateOx(-gAngle, pData->axis1)
      .RotateOy(angle, pData->axis0)
      .Update();
   
    pData->vbmi3->LoadIdentity()
      .RotateOx(-gAngle, pData->axis1)
      .RotateOy(angle, pData->axis0)
      .Update();

    pData->vbmi4->LoadIdentity()
      .Translate( scale2 * pData->axis2)
      .RotateOx(-gAngle, pData->axis1)
      .RotateOy(angle, pData->axis0)
      .Update();	
}

void g_staticInit6()
{                                       
    CNameDecls &refNames = ZAV_Scene()->ObjRefNames();  // objects' names
    ct_ClassTableID  ctID = g_arena.searchSeanceClassTable("StaticObj");

    CNameDecl  &Blg_lnchArr = refNames["blg_lnch"];      // array of objects named "mill"
    int       nBlg_lnchCount = Blg_lnchArr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nBlg_lnchCount ; i++ ) 
    {
         CViewObjectRef  *pBlg_lnch = (CViewObjectRef*)Blg_lnchArr[i];
         CViewObjectBase &pBase = pBlg_lnch->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Blg_lnch");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->axis0 =
                *(CFVector3*)pBase.Names().vertices["Axis0"][5];

         data->axis1 =
                *(CFVector3*)pBase.Names().vertices["Axis0"][6];
         
	 data->axis2 =
                *(CFVector3*)pBase.Names().vertices["Axis0"][7];

	 data->axis3 =
                *(CFVector3*)pBase.Names().vertices["Axis0"][0];

         data->axis4 =
                *(CFVector3*)pBase.Names().vertices["Axis0"][1];

         data->axis5 =
                *(CFVector3*)pBase.Names().vertices["Axis0"][2]; 

         data->axis6 =
                *(CFVector3*)pBase.Names().vertices["Axis0"][3];

         data->axis7 =
                *(CFVector3*)pBase.Names().vertices["Axis0"][4];

         data->vbmi0 = &pBase.KFSet().Mod0("0");
         data->vbmi1 = &pBase.KFSet().Mod0("1");
	 data->vbmi2 = &pBase.KFSet().Mod0("2");
	 data->vbmi3 = &pBase.KFSet().Mod0("3");
         data->vbmi4 = &pBase.KFSet().Mod0("4");
         data->vbmi5 = &pBase.KFSet().Mod0("5");
	 data->vbmi6 = &pBase.KFSet().Mod0("6");
	 data->vbmi7 = &pBase.KFSet().Mod0("7");

         pBlg_lnch->SetAnimationCallback(Blg_lnchCallback);
         pBlg_lnch->SetUserAttrib(data);
    }

    CNameDecl  &Blg_twrArr = refNames["blg_twr"];      // array of objects named "mill"
    int       nBlg_twrCount = Blg_twrArr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nBlg_twrCount ; i++ ) 
    {
         CViewObjectRef  *pBlg_twr = (CViewObjectRef*)Blg_twrArr[i];
         CViewObjectBase &pBase = pBlg_twr->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Blg_twr");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->phase = g_arena.context->rnd_f(5);

         data->axis1 =
                *(CFVector3*)pBase.Names().vertices["Axis1"][0];
         
	 data->axis2 =
                *(CFVector3*)pBase.Names().vertices["Axis2"][0];

	 data->axis3 =
                *(CFVector3*)pBase.Names().vertices["Axis3"][0];

         data->axis4 =
                *(CFVector3*)pBase.Names().vertices["Axis4"][0];

         data->vbmi1 = &pBase.KFSet().Mod0("1");
	 data->vbmi2 = &pBase.KFSet().Mod0("2");
	 data->vbmi3 = &pBase.KFSet().Mod0("3");
         data->vbmi4 = &pBase.KFSet().Mod0("4");

         pBlg_twr->SetAnimationCallback(Blg_twrCallback);
         pBlg_twr->SetUserAttrib(data);
    }

    CNameDecl  &Gun_gunArr = refNames["gun_gun"];      // array of objects named "mill"
    int       nGun_gunCount = Gun_gunArr.Count(); // number of objects named "mill"

    for( int i = 0 ; i < nGun_gunCount ; i++ ) 
    {
         CViewObjectRef  *pGun_gun = (CViewObjectRef*)Gun_gunArr[i];
         CViewObjectBase &pBase = pGun_gun->Model()->BaseSet(0).Base(0);

         KR_ObjectID oID = g_arena.newObject(ctID,"static.Gun_gun");
         IStaticObj *data = (IStaticObj*)(g_arena.context->queryInterface(oID,IStaticObjIID));

         data->axis0 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis0"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis0"][1]
               ) * 0.5;

         data->axis1 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis1"][0]
               +*(CFVector3*)pBase.Names().vertices["Axis1"][1]
               ) * 0.5;

	 data->axis2 =
               (
                *(CFVector3*)pBase.Names().vertices["Axis2"][1]
               -*(CFVector3*)pBase.Names().vertices["Axis2"][0]
               );

         data->vbmi0 = &pBase.KFSet().Mod0("Base");
         data->vbmi1 = &pBase.KFSet().Mod0("Gun1");
         data->vbmi2 = &pBase.KFSet().Mod0("Gun11");
         data->vbmi3 = &pBase.KFSet().Mod0("Gun2");
         data->vbmi4 = &pBase.KFSet().Mod0("Gun21");

         pGun_gun->SetAnimationCallback(Gun_gunCallback);
         pGun_gun->SetUserAttrib(data);
    }
}

void g_StaticAnim(int level)
{
    switch(level)
    {
    case 1: g_staticInit1(); break;	
    case 2: g_staticInit2(); break;
    case 3: g_staticInit3(); break;
    case 4: g_staticInit4(); break;
    case 5: g_staticInit5(); break;
    case 6: g_staticInit6(); break;
    }
}

void g_AttachObject(
                      ct_ClassTableID ctID,
                      const char     *name,
                      KR_ObjectID     attrID,
                      const char     *nameRef,
                      CFVector3       v,
	              int 	      eventLabel		 
                   )
{
    CNameDecls &refNames = ZAV_Scene()->ObjRefNames();	// objects' names
    CNameDecl *nd = refNames.Lookup(nameRef);

    if(  nd==NULL  )
    {
         echo("Warning: Object [%s] not found",nameRef);
         return;
    }

    CNameDecl     &m     = *nd;	// array of objects named "mill"
    int            count = m.Count();	// number of objects named "mill"

    for( int i = 0 ; i < count ; i++ ) 
    {
         CViewObjectRef	 *pObj = (CViewObjectRef*)m[i];
         CFVector3 pos( pObj->GetDir()*v );
         KR_ObjectID newObj = g_arena.newObject(ctID,name);
                                     
         if(  newObj==KR_ObjectID::NUL()  )
         {
              echo("Warning! Smoker table overflow");
              break;
         }

         KR_Event event;
//         event.label     = fou_EVCMD_START;
	 event.label     = eventLabel;
         event.timeStamp = 0.1;
         event.source    = g_arena.getObjectID();
         event.destination = newObj;

         event.data.open(EDO_WRITE)
                        .putObjectID(attrID)
                        .putDouble(pos.x)
                        .putDouble(pos.y)
                        .putDouble(pos.z)
                   .close();

         g_arena.getContext()->sendEventNow(event);
    }     
}
