include "../defines.sci"
include "../green_defines.sci"
include "../green_hardware.sci"
include "scinc/localmenu.sci"
include "../green_menu.sci"
include "../defs.h"
include "../sys.sci"
include "../pfunc.sci"
include "scinc/brief.sci"
include "../spark.sci"

func void LoadWAV( int ctID, str oName , str fname,
                   float  minFront,
                   float  minBack,
                   float  maxFront,
                   float  maxBack,
                   float  intensity
               )
 var int event,
     oID, cachePos;

 {
     s_NewObject( ctID, oName );
     s_SearchObjectID( oID, cachePos, oName );
     event := s_OpenEventData( EDO_WRITE );
               s_WriteStr( event, fname );
               s_WriteFloat( event, minFront );
               s_WriteFloat( event, minBack );
               s_WriteFloat( event, maxFront );
               s_WriteFloat( event, maxBack );
               s_WriteFloat( event, intensity );
             s_CloseEventData( event );

    s_SendEventNow( event,
                    sk_EV_LOAD,
                    oID, cachePos );
 }


func void SetGroupArea( str oName, float x,float z, int w, int d)
 var int event,
     oID, cachePos;
{
    s_SearchObjectID( oID, cachePos, oName );
     event := s_OpenEventData( EDO_WRITE );
               s_WriteFloat( event, x );
               s_WriteFloat( event, z );
               s_WriteInt  ( event, w );
               s_WriteInt  ( event, d );
             s_CloseEventData( event );

    s_SendEventNow( event,
                    com_EV_GROUP_AREA,
                    oID, cachePos );
}

include "..\incubator.sci"
include "..\bullet.sci"
include "scinc\bullet_loc.sci"
include "..\fountain.sci"
include "..\farter.sci"
include "scinc\farterattr.sci"
include "scinc\set_farter.sci"
include "..\smoke.sci"
include "scinc\set_smoker.sci"


include "../explosion.sci"
include "scinc/tank.sci"
include "scinc/skin.sci"
include "../bird.sci"
include "scinc/set_bird.sci"
include "scinc/vehicle.sci"
include "scinc/taxi.sci"
include "scinc/set_taxi.sci"
include "scinc/people.sci"
include "scinc/set_people.sci"
include "scinc/load_route.sci"
include "scinc/loadwav.sci"
include "scinc/recrcen.sci"
include "scinc/suavik_main.sci"
include "../lamp.sci"
include "scinc/set_lamps.sci"
include "scinc/corpse.sci"
include "scinc/set_fountain.sci"
include "../artefact.sci"
include "scinc/localmain.sci"
include "scinc/set_Fires.sci"
include "scinc/Howitzer.sci"
include "scinc/set_howitzers.sci"
include "scinc/cr_fountainAttr.sci"

 /***********************************************************************/
func void main()
 var int event, oID, cachePos;
 var int classTableID;
 var int classTableIDSparkAttr;
 var int ctSmokeAttr;
 var int classTableIDOrphanAttr;
 var int i;
 {   
    local_createTables();    

    classTableIDSparkAttr  := s_AddClassTable("SparkAttr",3);

    main_LoadRoute               ();
    main_LoadSkin                ();

// Creating Attributes
    main_CreateVehicleAttr       ();
    main_CreateSmokeAttr         ();
    main_CreateExplosionAttr     ();
    main_CreateCannonsAndTankAttr();
    main_CreateBirdAttr          ();
    main_CreateTaxiAttr          ();
    main_CreateSmokerAttr        ();
    main_CreatePeopleAttr        ();
    main_CreateFarterAttrs       ();
    main_CreateLampAttr		 ();
    main_CreateCorpseAttr        ();
    main_CreateHowitzerAttrs     ();
    main_CreateFountainAttr      ();
    main_CreateBullets           ();

    s_AddClassTable("Portal",2);

    s_NewObject( classTableIDSparkAttr,  "Spark.Flash" );
    SetSpark0();

    classTableIDOrphanAttr := s_AddClassTable("OrphanAttr",3);
    New(classTableIDOrphanAttr, "Orphan.Attr.Default", oID, cachePos);

    local_createCommanders();
    local_createMissionProject();
    CreateTestProject();
    SetArtefactAttr();

    s_UpdateAttributes(); //----------------


    main_CreateBirds();
    main_CreateVehicle();
    main_CreateTaxi();
    main_CreateSmoker();
    main_CreatePeoples();
    main_CreateFarters();
    main_CreateLamps();
    main_CreateFountain();
    main_CreateFires();
    main_CreateHowitzers();
    main_SetArtefact();


    SetAnimatePeople();
    SetupControls();
    CreateMenu();
    

    local_main();
 }

/* End of file LEVEL0.SC */