func void CreateLocalPointMenu( int pColor, int afColor, int abColor)
 var int vehicleID, vehicleCP;
{

 s_SearchObjectID(vehicleID, vehicleCP, "Vehicle.Default");

 s_AddMenuText("/Game/Cheats", "Points", -1, -1, 0, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "POINTS", "ТОЧКИ");
  s_AddMenuText("/Game/Cheats/Points", "Point0", vehicleID, vehicleCP, s_Menu_Point0, 5, 60, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Portal", "Портал");
  s_AddMenuText("/Game/Cheats/Points", "Point1", vehicleID, vehicleCP, s_Menu_Point1, 5, 60, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Recruit Center, Natives", "Вербовочный Пункт Аборигенов");
  s_AddMenuText("/Game/Cheats/Points", "Point2", vehicleID, vehicleCP, s_Menu_Point2, 5, 60, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Recruit Center, Conquerors 1", "Вербовочный Пункт Завоевателей 1");
  s_AddMenuText("/Game/Cheats/Points", "Point3", vehicleID, vehicleCP, s_Menu_Point3, 5, 60, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Recruit Center, Conquerors 2", "Вербовочный Пункт Завоевателей 2");
//  s_AddMenuText("/Game/Cheats/Points", "Point4", vehicleID, vehicleCP, s_Menu_Point4, 5, 60, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Latest Mission Point", "Задание");


// s_AddMenuText("/", "Points", -1, -1, 0, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "POINTS", "ТОЧКИ");
//	s_AddMenuText("/Points", "Point0", vehicleID, vehicleCP, s_Menu_Point0, 5, 40, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Portal", "Портал");
//	s_AddMenuText("/Points", "Point1", vehicleID, vehicleCP, s_Menu_Point1, 5, 40, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Recruit Center, Natives", "Вербовочный Пункт Аборигенов");
//	s_AddMenuText("/Points", "Point2", vehicleID, vehicleCP, s_Menu_Point2, 5, 40, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Recruit Center, Conquerors 1", "Вербовочный Пункт Завоевателей 1");
//	s_AddMenuText("/Points", "Point3", vehicleID, vehicleCP, s_Menu_Point3, 5, 40, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Recruit Center, Conquerors 2", "Вербовочный Пункт Завоевателей 2");
//	s_AddMenuText("/Points", "Point4", vehicleID, vehicleCP, s_Menu_Point4, 5, 40, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Latest Mission Point", "Задание");
}