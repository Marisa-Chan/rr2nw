func void LoadMenuFonts()
{
	s_LoadFont("..\a_menu.fnt","Font.a_menu.fnt");
	s_LoadFont("..\p_menu.fnt","Font.p_menu.fnt");
	s_LoadFont("..\fnt16x16.fnt","Font.fnt16x16.fnt");
	s_LoadFont("..\fig8x8.fnt","Font.fig8x8.fnt");
}
func void LoadMenuSound()
{
}
func void LoadMenuImages()
{
	//s_LoadImage("scroll.bmp");
	//s_LoadImage("pointer.bmp");
}

func void CreateMenu()
var int menuID, menuCP, vehicleID, vehicleCP, 
	hrdID, hrdCP, 
	levelID, levelCP,
	afColor, abColor, pColor;
{
	LoadMenuFonts();
	LoadMenuSound();
	LoadMenuImages();

	s_SearchObjectID(menuID, menuCP, "MainMenu");
	s_SearchObjectID(hrdID, hrdCP, "Hardware");
	s_SearchObjectID(vehicleID, vehicleCP, "Vehicle.Default");
	s_SearchObjectID(levelID, levelCP, "LEVEL");

	pColor  := s_CreateTransparetColor(160,   0, 160);
	afColor := s_CreateTransparetColor(250, 200, 250);
	abColor	:= s_CreateTransparetColor(160,   0, 160);
	
	s_AddMenuText("/", "Game", -1, -1, 0, 8, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "GAME", "»√–¿");
		//s_AddMenuText("/Game", "Network", -1, -1, 0, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "NETWORK", "—≈“‹");
               // 	s_AddMenuText("/Game/Network", "Begin", -1, -1, 0, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "BEGIN", "—≈“‹");
		//	s_AddMenuText("/Game/Network", "Join", -1, -1, 0, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "JOIN", "—≈“‹");

                //s_AddMenuText("/Game", "Begin", -1, -1, 0, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "BEGIN", "—≈“‹");
                //s_AddMenuText("/Game", "Restart", -1, -1, 0, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Restart", "—≈“‹");
         	s_AddMenuText("/Game", "RestartL", -1, -1, 0, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "RESTART", "œ≈–≈√–”«»“‹");

	        s_AddMenuList("/Game/RestartL", "Restart", levelID, levelCP, s_lev_RESTART, 5, 40, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "m_reloadLevel", "", "", 0);
		 s_AddMenuListItem("/Game/RestartL/Restart", "AZTEC vs CONQUERORS");
		 s_AddMenuListItem("/Game/RestartL/Restart", "MEDIEVAL DAY");
		 s_AddMenuListItem("/Game/RestartL/Restart", "MEDIEVAL NIGHT");
		 s_AddMenuListItem("/Game/RestartL/Restart", "HI-TECH DAY");
		 s_AddMenuListItem("/Game/RestartL/Restart", "MOON");
		 s_AddMenuListItem("/Game/RestartL/Restart", "HI-TECH NIGHT");
		 s_AddMenuListItem("/Game/RestartL/Restart", "ISLANDS");
		 s_AddMenuListItem("/Game/RestartL/Restart", "POST WAR");

	        s_AddMenuText("/Game", "SaveGame", levelID, levelCP, s_lev_SAVE, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "SAVE", "—Œ’–¿Õ»“‹");
 			s_AddMenuInput("/Game/SaveGame", "Slot0", levelID, levelCP, s_lev_SAVE_SLOT0, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Empty");
                	s_AddMenuInput("/Game/SaveGame", "Slot1", levelID, levelCP, s_lev_SAVE_SLOT1, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Empty");
	 		s_AddMenuInput("/Game/SaveGame", "Slot2", levelID, levelCP, s_lev_SAVE_SLOT2, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Empty");
        		s_AddMenuInput("/Game/SaveGame", "Slot3", levelID, levelCP, s_lev_SAVE_SLOT3, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Empty");
	 		s_AddMenuInput("/Game/SaveGame", "Slot4", levelID, levelCP, s_lev_SAVE_SLOT4, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Empty");
        	        s_AddMenuInput("/Game/SaveGame", "Slot5", levelID, levelCP, s_lev_SAVE_SLOT5, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Empty");
 			s_AddMenuInput("/Game/SaveGame", "Slot6", levelID, levelCP, s_lev_SAVE_SLOT6, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Empty");
	                s_AddMenuInput("/Game/SaveGame", "Slot7", levelID, levelCP, s_lev_SAVE_SLOT7, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Empty");

        	s_AddMenuText("/Game", "LoadGame", levelID, levelCP, s_lev_LOAD, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "LOAD", "«¿√–”«»“‹");
 			s_AddMenuText("/Game/LoadGame", "Slot0", levelID, levelCP, s_lev_LOAD_SLOT0, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Empty", "œÛÒÚÓ");
	                s_AddMenuText("/Game/LoadGame", "Slot1", levelID, levelCP, s_lev_LOAD_SLOT1, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Empty", "œÛÒÚÓ");
 			s_AddMenuText("/Game/LoadGame", "Slot2", levelID, levelCP, s_lev_LOAD_SLOT2, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Empty", "œÛÒÚÓ");
                	s_AddMenuText("/Game/LoadGame", "Slot3", levelID, levelCP, s_lev_LOAD_SLOT3, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Empty", "œÛÒÚÓ");
	 		s_AddMenuText("/Game/LoadGame", "Slot4", levelID, levelCP, s_lev_LOAD_SLOT4, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Empty", "œÛÒÚÓ");
        	        s_AddMenuText("/Game/LoadGame", "Slot5", levelID, levelCP, s_lev_LOAD_SLOT5, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Empty", "œÛÒÚÓ");
 			s_AddMenuText("/Game/LoadGame", "Slot6", levelID, levelCP, s_lev_LOAD_SLOT6, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Empty", "œÛÒÚÓ");
	                s_AddMenuText("/Game/LoadGame", "Slot7", levelID, levelCP, s_lev_LOAD_SLOT7, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "Empty", "œÛÒÚÓ");
		s_AddMenuText("/Game", "Cheats", -1, -1, 0, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "CHEATS", "Œœ÷»»");
		CreateLocalPointMenu( pColor, afColor, abColor);

			//s_AddMenuText("/Game/Cheats", "GodMode", -1, -1, 0, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "GOD MODE", "“Œ◊ »");
                        //s_AddMenuText("/Game/Cheats", "InfWeapon", -1, -1, 0, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "INF WEAPON", "“Œ◊ »");
			s_AddMenuText("/Game/Cheats", "CompleteMission", vehicleID, vehicleCP, s_Menu_MISSIONOK, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "COMPLETE MISSION", "œÛÒÚÓ");
		s_AddMenuText("/Game", "Surrender", vehicleID, vehicleCP, s_Menu_SURRENDER, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "SURRENDER", "œÛÒÚÓ");


	//s_AddMenuList("/", "Skill", -1, -1, 0, 5, 60, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "gameSkill", "", "", 1);
	//	s_AddMenuListItem("/Skill", "EASY");
//		s_AddMenuListItem("/Skill", "MEDDIUM");
//		s_AddMenuListItem("/Skill", "HARD");

	s_AddMenuText("/", "Options", -1, -1, 0, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "OPTIONS", "Œœ÷»»");
        	s_AddMenuText("/Options", "Controls", -1, -1, 0, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "CONTROLS", "Œœ÷»»");
			s_AddMenuText("/Options/Controls", "Keyboard", -1, -1, 0, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "KEYBOARD", "Œœ÷»»");
				s_AddMenuScroll("/Options/Controls/Keyboard", "Sens", -1, -1, 0, 5, 60, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "keySens", "Sens", "— –ŒÀÀ", 0, 1, 0.1, 1);
				s_AddMenuSetup("/Options/Controls/Keyboard", "Setup", -1, -1, 0, 5, 60, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "KeySetup", " Î‡‚Ë‡ÚÛ‡");
					s_AddMenuSetupLine("TurnLeft", "TurnLeft");
					s_AddMenuSetupLine("TurnRight", "TurnRight");
					s_AddMenuSetupLine("LookUp", "LookUp");
					s_AddMenuSetupLine("LookDown", "LookDown");
					s_AddMenuSetupLine("RollRight", "RollRight");
					s_AddMenuSetupLine("RollLeft", "RollLeft");
					s_AddMenuSetupLine("Forward", "Forward");
					s_AddMenuSetupLine("Backward", "Backward");
					s_AddMenuSetupLine("StrafeLeft", "StrafeLeft");
					s_AddMenuSetupLine("StrafeRight", "StrafeRight");
					s_AddMenuSetupLine("StrafeUp", "StrafeUp");
					s_AddMenuSetupLine("StrafeDown", "StrafeDown");
					s_AddMenuSetupLine("Strafe", "Strafe");
					s_AddMenuSetupLine("Forceage", "Forceage");
					s_AddMenuSetupLine("Stop", "StopVehicle");
					s_AddMenuSetupLine("Jump", "Jump");
					s_AddMenuSetupLine("CenterView", "CenterView");
					s_AddMenuSetupLine("Change Vehicle", "ChangeVehicle");
					s_AddMenuSetupLine("FirePrimary", "FirePrimary");
					s_AddMenuSetupLine("FireSecondary", "FireSecondary");
        	                	s_AddMenuSetupLine("DropArtefact", "DropArtefact");
					s_AddMenuSetupLine("Map", "DMap");
  			s_AddMenuText("/Options/Controls", "Mouse", -1, -1, 0, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "MOUSE", "Œœ÷»»");
				s_AddMenuList("/Options/Controls/Mouse", "mUse", hrdID, hrdCP, s_ChangeMouseParams, 5, 60, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "msUse", "USE", "À»—“", 1);
					s_AddMenuListItem("/Options/Controls/Mouse/mUse", "OFF");
					s_AddMenuListItem("/Options/Controls/Mouse/mUse", "ON");
				s_AddMenuScroll("/Options/Controls/Mouse", "mSensX", hrdID, hrdCP, s_ChangeMouseParams, 5, 60, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "msSensX", "SensX", "— –ŒÀÀ", 0, 1, 0.1, 0.5);
				s_AddMenuScroll("/Options/Controls/Mouse", "mSensY", hrdID, hrdCP, s_ChangeMouseParams, 5, 60, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "msSensY", "SensY", "— –ŒÀÀ", 0, 1, 0.1, 0.5);
				s_AddMenuList("/Options/Controls/Mouse", "InvX", hrdID, hrdCP, s_ChangeMouseParams, 5, 60, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "msInvX", "InvX", "À»—“", 1);
					s_AddMenuListItem("/Options/Controls/Mouse/InvX", "OFF");
					s_AddMenuListItem("/Options/Controls/Mouse/InvX", "ON");
				s_AddMenuList("/Options/Controls/Mouse", "InvY", hrdID, hrdCP, s_ChangeMouseParams, 5, 60, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "msInvY", "InvY", "À»—“", 1);
					s_AddMenuListItem("/Options/Controls/Mouse/InvY", "OFF");
					s_AddMenuListItem("/Options/Controls/Mouse/InvY", "ON");

			s_AddMenuText("/Options/Controls", "Joystick", -1, -1, 0, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "JOYSTICK", "Œœ÷»»");
				s_AddMenuText ("/Options/Controls/Joystick", "Calibrate", hrdID, hrdCP, s_CalibrateJoystick, 5, 60, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "CALIBRATE", "Õ¿—“–Œ… »");
				s_AddMenuList("/Options/Controls/Joystick", "jUse", hrdID, hrdCP, s_ChangeJoystickParams, 5, 60, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "jsUse", "USE", "À»—“", 1);
					s_AddMenuListItem("/Options/Controls/Joystick/jUse", "OFF");
					s_AddMenuListItem("/Options/Controls/Joystick/jUse", "ON");
				s_AddMenuScroll("/Options/Controls/Joystick", "jSensX", hrdID, hrdCP, s_ChangeJoystickParams, 5, 60, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "jsSensX", "SensX", "— –ŒÀÀ", 0, 1, 0.1, 0.5);
				s_AddMenuScroll("/Options/Controls/Joystick", "jSensY", hrdID, hrdCP, s_ChangeJoystickParams, 5, 60, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "jsSensY", "SensY", "— –ŒÀÀ", 0, 1, 0.1, 0.5);
				s_AddMenuList("/Options/Controls/Joystick", "InvX", hrdID, hrdCP, s_ChangeJoystickParams, 5, 60, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "msInvX", "InvX", "À»—“", 1);
					s_AddMenuListItem("/Options/Controls/Joystick/InvX", "OFF");
					s_AddMenuListItem("/Options/Controls/Joystick/InvX", "ON");
				s_AddMenuList("/Options/Controls/Joystick", "InvY", hrdID, hrdCP, s_ChangeJoystickParams, 5, 60, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "msInvY", "InvY", "À»—“", 1);
					s_AddMenuListItem("/Options/Controls/Joystick/InvY", "OFF");
					s_AddMenuListItem("/Options/Controls/Joystick/InvY", "ON");

		s_AddMenuText("/Options", "Sound", -1, -1, 0, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "SOUND", "Œœ÷»»");
		s_AddMenuText("/Options", "Details", -1, -1, 0, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "DETAILS", "Œœ÷»»");

        s_AddMenuText("/", "Exit", menuID, menuCP, s_Menu_EXIT, 5, 100, pColor, 128, afColor, 128, abColor, 128, "Font.p_menu.fnt", "Font.a_menu.fnt", "EXIT", "¬€’Œƒ");
}
