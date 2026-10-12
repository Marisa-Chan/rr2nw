func void SetupControls()
{
	/* 
	   Это часть управления для "чайников". Часть его используется и 
           в нормальном управлении.
           Изменения:
           1) Стоп клавиша W вместо S
           2) Сброс артифакта F2 вместо D


	*/
	s_SetCTRL("LookUp", "Up");	
	s_SetCTRL("LookDown", "Down");
	s_SetCTRL("TurnLeft", "Left");
	s_SetCTRL("TurnRight", "Right");
	s_SetCTRL("Forward", "A");
	s_SetCTRL("Backward", "Z");
	s_SetCTRL("Forceage", "Q");
	s_SetCTRL("Strafe", "LAlt");
	s_SetCTRL("FirePrimary", "LCtrl" );
	s_SetCTRL("FireSecondary", "Enter" );
        s_SetCTRL("Jump", "Space");
        s_SetCTRL("StopVehicle", "W");
        s_SetCTRL("ChangeVehicle", "F1" );
        s_SetCTRL("DropArtefact", "F2");
        s_SetCTRL("DMap", "M");


	/* 
	Это продвинутая клавиатурная и мышино-джойстиковая раскладка 
        управления. Она дополняет раскладку для чайников и является 
        значительно более удобной и полной.
        */
	s_SetCTRL("LookUp", "Num8");
	s_SetCTRL("LookDown", "Num5");
	s_SetCTRL("TurnLeft", "Num4");
	s_SetCTRL("TurnRight", "Num6");
	s_SetCTRL("RollLeft", "Num7");
	s_SetCTRL("RollRight", "Num9");

	s_SetCTRL("Forward", "E");
	s_SetCTRL("Backward", "D");
	s_SetCTRL("StrafeLeft", "S");
	s_SetCTRL("StrafeRight", "F");
	s_SetCTRL("StrafeUp", "T");
	s_SetCTRL("StrafeDown", "G");
        
	s_SetCTRL("Forceage", "LShift");
	s_SetCTRL("FirePrimary", "NumIns" );
	s_SetCTRL("FireSecondary", "NumEnter" );
	
        s_SetCTRL("FirePrimary", "MouseL" );
	s_SetCTRL("FireSecondary", "MouseR" );
        s_SetCTRL("FirePrimary", "Joy1" );
	s_SetCTRL("FireSecondary", "Joy2" );
	
        
        // Это для разработчиков брифингов, в релизе не нужно
//	s_SetCTRL("BrfAddPoint", "RCtrl" );
	s_SetCTRL("BrfAddPoint",   "Del" );
	s_SetCTRL("BrfSaveFlight", "End" );

	// Управление картой, не трогай лучше лишний раз
	s_SetCTRL("DMapScrollUp", "Up");
	s_SetCTRL("DMapScrollDown", "Down");
	s_SetCTRL("DMapScrollLeft", "Left");
	s_SetCTRL("DMapScrollRight", "Right");
        s_SetCTRL("DMapToggleFollowMode", "Del");
        s_SetCTRL("DMapNextMission", "]");
        s_SetCTRL("DMapPreviousMission", "[");
        s_SetCTRL("DMapTextBoxUp", "PgUp");
        s_SetCTRL("DMapTextBoxDown", "PgDn");


        // Это системные бинды, не трож-убьет
	s_SetCTRL("MenuUp", "Up");
	s_SetCTRL("MenuUp", "Num8");
	s_SetCTRL("MenuDown", "Down");
	s_SetCTRL("MenuDown", "Num2");
	s_SetCTRL("MenuRight", "Right");
	s_SetCTRL("MenuRight", "Num6");
	s_SetCTRL("MenuLeft", "Left");
	s_SetCTRL("MenuLeft", "Num4");
	s_SetCTRL("MenuEnter", "Enter");
	s_SetCTRL("MenuEnter", "NumEnter");
	s_SetCTRL("MenuDel", "Del");
	s_SetCTRL("MenuDel", "NumDel");
        s_SetCTRL("Console", "`");
 	s_SetCTRL("MenuToggle", "Esc");


	s_LinkCTRL(); // Без этого работать не будет :))))
}
