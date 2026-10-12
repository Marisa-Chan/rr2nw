func void local_createMissionProject()
{
    s_CreateProjectTable( 200,  // project cnt
                          1024, // tree node cnt
                          1024*10 // project data size
                        );
}

func void local_createCommanders()
var int classTableID;
{
    classTableID := s_AddClassTable("Commander", 2);
    s_NewObject( classTableID, "Kingdom" );
    s_NewObject( classTableID, "Magician" );

    s_AddClassTable( "TankGroup",30 );
    s_AddClassTable("Tank", 64);
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
    s_AddClassTable("StaticObj"  ,200);
    s_AddClassTable("Corpse"     ,100);
    s_AddClassTable("DynSmoker"  ,50+12 );
    s_AddClassTable("Orphan"     ,  5 );
}

func void local_main()
{
    s_SetLevel(6); // set of animation for 4-th level
    suavik_Main();
}
