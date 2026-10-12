func void main_Control()
 var int oID, cachePos;
{
    s_SearchObjectID( oID, cachePos, "LevelAttr" );
    SetAttribute_f( oID, cachePos, "m_addDesire", 0.3 );
    SetAttribute_f( oID, cachePos, "m_666Time", 20 );
}


func void suavik_Main()
{
       s_SetViewPoint(0, [2874,160,-3179]);
       //s_SetViewPoint(1, [2370.133,65.450, -2610.0] );
       s_SetViewPoint(1, [3112,52, -2800] );
       //s_SetViewPoint(2, [2755.274,79.137, -2660.0] );
       s_SetViewPoint(2, [2640,52, -2815] );
       s_SetViewPoint(3, [2745.965,79.500, -2660.0] );
  
//  CreateRecruitCenter(ctID,"A.Recr0",[2370.133,65.450, -2608.999],"Actek");
//  CreateRecruitCenter(ctID,"C.Recr0",[2755.274,79.137, -2658.355],"Colony");
//  CreateRecruitCenter(ctID,"C.Recr0",[2745.965,79.500, -2658.999],"Colony");

       s_SetViewPoint(4, [1326.5,87.13,-2576.5]);
       s_SetViewPoint(5, [3369 ,    73,-3256  ]);
       s_SetViewPoint(6, [4173.31,49.82,-3464.48]);
       s_SetViewPoint(7, [3010,174.19,-2769.85]);
       s_SetViewPoint(8, [4233.3,51.88,-3009.2]);

       SetRecruitCenter();
       main_Control();
}
