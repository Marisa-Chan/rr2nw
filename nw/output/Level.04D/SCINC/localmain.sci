
func void local_createMissionProject()
{
    SetGroupArea( "Actek", 2060,-2390-300, 100, 100);

    s_CreateProjectTable( 200,  // project cnt
                          1024, // tree node cnt
                          1024*10 // project data size
                        );
}

func void local_createCommanders()
var int classTableID;
var int oID0, cachePos0, oID1, cachePos1;
{
    classTableID := s_AddClassTable("Commander", 2);
    

    s_New( classTableID, "Colony" , oID0, cachePos0 );
    s_New( classTableID, "Actek", oID1, cachePos1 );

    s_SetHostileCommander(oID0, cachePos0, oID1, cachePos1);

    s_AddClassTable( "TankGroup",10 );
    s_AddClassTable("Tank", 40);

}


func void local_createTables()
var int classTableID;
{
    classTableID := s_AddClassTable("WAVObj",30);
    LoadAllWaves(classTableID);

    s_AddClassTable("SoundObj"   ,250);
    s_AddClassTable("DebugCross" ,200);
    s_AddClassTable("Spark"      ,40);
    s_AddClassTable("Cannon"     ,140);
    s_AddClassTable("Explosion"  ,40);
    s_AddClassTable("Smoke"      ,300);
    s_AddClassTable("StaticObj"  ,50);
    s_AddClassTable("Corpse"     ,100);
    s_AddClassTable("DynSmoker"  ,50+12 );
    s_AddClassTable("Orphan"     ,  5 );
}


func void local_main()
var int ctSmokerID,ctSmokerCP;
{

    //s_SearchObjectID( ctSmokerID, ctSmokerCP, "Smoker"); 
    ctSmokerID := s_SearchSeanceClassTable("Smoker");
    CreateFountain("Fount.3.Arab","Fount.Attr.Arab",[2514.84, 63.1582, -2306.53]);    

    s_SetLevel(4); // set of animation for 4-th level
    suavik_Main();
}