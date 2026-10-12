//***************************************************************
//***************************************************************
// GENNNNNNNNNNNNNNNNNNN!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!SSSSSSS
//***************************************************************
//***************************************************************



func void Create2G01Project()
 var int nNode;
 {
	nNode := p_AddPlayBriefing (s_PNodeNULL(), "Brief/br2g01.txt");
        nNode := p_AddSuccessKill (nNode, "K.Unit.2m01g0");
	nNode := p_AddRunScript (nNode, "Brief/br2g01.sc");
 	nNode := p_AddCommander (nNode, "Magician");
	nNode:=p_SetMissionSummary(nNode,"Chase", "One of our mercenaries betrays us.$You must overtake and kill him.$It is supposed that he go to Emerald Dragon.", 
			   "Route/m2g01m.rt", ConvertColor(0,255,0), 4,2);
 	nNode := p_AddMissionInfo (nNode, 0);
 	s_NewProject ("Project2G01", nNode);

}

func void Create2G02Project()
 var int nNode;
 {
	nNode := p_AddPlayBriefing (s_PNodeNULL(), "Brief/br2g02.txt");
        nNode := p_AddSuccessKill (nNode, "K.Unit.2m02g0");
        nNode := p_AddSuccessKill (nNode, "K.Unit.2m02g1");
        nNode := p_AddSuccessKill (nNode, "K.Unit.2m02g2");
        nNode := p_AddSuccessKill (nNode, "K.Unit.2m02g3");
        nNode := p_AddSuccessKill (nNode, "K.Unit.2m02g4");
        nNode := p_AddSuccessKill (nNode, "K.Unit.2m02g5");
	nNode := p_AddRunScript (nNode, "Brief/br2g02.sc");
 	nNode := p_AddCommander (nNode, "Magician");
	nNode:=p_SetMissionSummary(nNode,"", "You must defend bridge from the enemies armade$that approaching from the west.$You have to kill all enemies.", 
			   "Route/m2g02m.rt", ConvertColor(0,255,0), 4,2);
 	nNode := p_AddMissionInfo (nNode, 0);
 	s_NewProject ("Project2G02", nNode);
}

func void Create2G03Project()
 var int nNode;
 {
	nNode := p_AddPlayBriefing (s_PNodeNULL(), "Brief/br2g03.txt");
        nNode := p_AddSuccessKill (nNode, "ship.m2g03e0");
        nNode := p_AddSuccessKill (nNode, "ship.m2g03e1");
        nNode := p_AddSuccessKill (nNode, "ship.m2g03e2");
        nNode := p_AddSuccessKill (nNode, "ship.m2g03e3");
        nNode := p_AddSuccessKill (nNode, "ship.m2g03e4");
	nNode := p_AddRunScript (nNode, "Brief/br2g03.sc");
 	nNode := p_AddCommander (nNode, "Magician");
	nNode:=p_SetMissionSummary(nNode,"Naval battle", "You must annihilate kingdom trade flotilla$that are concentred near Flying Dutchman.", 
			   "Route/m2g03m.rt", ConvertColor(0,255,0), 4,2);
 	nNode := p_AddMissionInfo (nNode, 0);
 	s_NewProject ("Project2G03", nNode);
}

func void Create2G04Project()
 var int nNode;
 {
	nNode := p_AddPlayBriefing (s_PNodeNULL(), "Brief/br2g04.txt");
        nNode := p_AddSuccessKill (nNode, "hel.m2g03e0");
        nNode := p_AddSuccessKill (nNode, "hel.m2g03e1");
        nNode := p_AddSuccessKill (nNode, "hel.m2g03e2");
        nNode := p_AddSuccessKill (nNode, "hel.m2g03e3");
        nNode := p_AddSuccessKill (nNode, "hel.m2g03e4");
        nNode := p_AddSuccessKill (nNode, "hel.m2g03e5");
	nNode := p_AddSuccessLive (nNode, "ship.m2g042");
	nNode := p_AddSuccessLive (nNode, "ship.m2g043");
	nNode := p_AddSuccessLive (nNode, "ship.m2g044");
	nNode := p_AddRunScript (nNode, "Brief/br2g04.sc");
 	nNode := p_AddCommander (nNode, "Kingdom");
	nNode:=p_SetMissionSummary(nNode,"Naval battle", "You must defend king trade flotilla$that are concentred near Flying Dutchman.$You must destroy all enemies vehiclies$that try to attack them.", 
			   "Route/m2g04m.rt", ConvertColor(0,255,0), 4,2);
 	nNode := p_AddMissionInfo (nNode, 0);
 	s_NewProject ("Project2G04", nNode);
}

func void Create2G05Project()
 var int nNode;
 {
	nNode := p_AddPlayBriefing (s_PNodeNULL(), "Brief/br2g05.txt");
	nNode := p_AddSuccessReached (nNode, "ship.m2g050", 3047, 1932, 50);
	nNode := p_AddSuccessReached (nNode, "ship.m2g051", 3047, 1975, 50);
	nNode := p_AddSuccessReached (nNode, "ship.m2g052", 3047, 1955, 50);
	nNode := p_AddRunScript (nNode, "Brief/br2g05.sc");
 	nNode := p_AddCommander (nNode, "Kingdom");
	nNode:=p_SetMissionSummary(nNode,"Caravan defender", "You must defend our ships along it way from$Flying Dutchman to Benoon against$the mean magician creaturies.", 
			   "Route/m2g05m.rt", ConvertColor(0,255,0), 4,2);
 	nNode := p_AddMissionInfo (nNode, 0);
 	s_NewProject ("Project2G05", nNode);
}

func void Create2G06Project()
 var int nNode;
 {
	nNode := p_AddPlayBriefing (s_PNodeNULL(), "Brief/br2g06.txt");
	nNode := p_AddSuccessKill (nNode, "ship.m2g06e0");
	nNode := p_AddSuccessKill (nNode, "ship.m2g06e1");
	nNode := p_AddSuccessKill (nNode, "ship.m2g06e2");
	nNode := p_AddRunScript (nNode, "Brief/br2g06.sc");
 	nNode := p_AddCommander (nNode, "Magician");
	nNode:=p_SetMissionSummary(nNode,"Caravan interceptor", "You must intercept kings ship caravan$that follow from Flying Dutchman to the east$and sink all trade ships.", 
			   "Route/m2g06m.rt", ConvertColor(0,255,0), 4,2);
 	nNode := p_AddMissionInfo (nNode, 0);
 	s_NewProject ("Project2G06", nNode);
}






//***************************************************************
//***************************************************************
//***************************************************************
//***************************************************************
func void CreateProjectA12()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/ma12.txt");
/*
 nNode:=p_AddSuccessKill(nNode,"K.Unit.ma12.bs0");
 nNode:=p_AddSuccessKill(nNode,"K.Unit.ma12.bs1");
 nNode:=p_AddSuccessKill(nNode,"K.Unit.ma12.bs2");
 nNode:=p_AddSuccessKill(nNode,"K.Unit.ma12.bs3");
 nNode:=p_AddSuccessKill(nNode,"K.Unit.ma12.bs4");
*/
 nNode:=p_AddRunScript(nNode,"Brief/ma12.sc");
 nNode:=p_AddCommander(nNode,"Magician");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectA12",nNode);
}

func void CreateProjectA13()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/ma13.txt");
 nNode:=p_AddSuccessKill(nNode,"k.unit.ma13.00");
 nNode:=p_AddSuccessKill(nNode,"k.unit.ma13.01");
 nNode:=p_AddSuccessKill(nNode,"k.unit.ma13.02");
 nNode:=p_AddSuccessKill(nNode,"k.unit.ma13.03");
 nNode:=p_AddRunScript(nNode,"Brief/ma13.sc");
 nNode:=p_AddCommander(nNode,"Magician");
 nNode:=p_SetMissionSummary(
  nNode,"Young Beasts Killing (A13)",
  "You should kill four young battle beasts",
  "Route/A13/ms.rt",ConvertColor(255,0,0),2,2
 );
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectA13",nNode);
}

func void CreateProjectA14()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/ma14.txt");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma14.mr00");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma14.mr01");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma14.mr02");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma14.mr13");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma14.mr14");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma14.mr15");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma14.bs00");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma14.00");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma14.01");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma14.ap00");
 nNode:=p_AddRunScript(nNode,"Brief/ma14.sc");
 nNode:=p_AddCommander(nNode,"Kingdom");
 nNode:=p_SetMissionSummary(
  nNode,"Town Guard Destruction (A14)",
  "Destroy all defenders in City of Tears$and steal the stone",
  "Route/A14/ms.rt",ConvertColor(255,0,0),2,2
 );
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectA14",nNode);
}

func void CreateProjectA15()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/ma15.txt");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma15.mr00");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma15.mr01");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma15.mr02");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma15.mr03");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma15.00");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma15.01");
 nNode:=p_AddRunScript(nNode,"Brief/ma15.sc");
 nNode:=p_AddCommander(nNode,"Kingdom");
 nNode:=p_SetMissionSummary(
  nNode,"Guard Destruction (A15)",
  "Destroy all the guardians on the opposite side$of the bridge near Warrior FortressSpies reported$that the bridge is defended by$4 mortars and 2 battle beasts",
  "Route/A15/ms.rt",ConvertColor(255,0,0),2,2
 );
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectA15",nNode);
}

func void CreateProjectA16()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/ma16.txt");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma16.mr00");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma16.mr01");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma16.mr02");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma16.mr03");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma16.bs00");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma16.bs01");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma16.bs02");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma16.ap00");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma16.ap01");
 nNode:=p_AddRunScript(nNode,"Brief/ma16.sc");
 nNode:=p_AddCommander(nNode,"Kingdom");
 nNode:=p_SetMissionSummary(
  nNode,"Taking over a town(A16)",
  "Take over Veto Fort$It is defended by 4 mortars, 4 kernelsenders,$2 helicopters, and 2 knigths",
  "Route/A16/ms.rt",ConvertColor(255,0,0),2,2
 );
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectA16",nNode);
}

func void CreateProjectA17()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/ma17.txt");
 nNode:=p_AddSuccessKill(nNode,"k.unit.ma17.00");
 nNode:=p_AddSuccessKill(nNode,"k.unit.ma17.01");
 nNode:=p_AddSuccessKill(nNode,"k.unit.ma17.02");
 nNode:=p_AddSuccessKill(nNode,"k.unit.ma17.ap00");
 nNode:=p_AddSuccessKill(nNode,"k.unit.ma17.ap01");
 nNode:=p_AddSuccessKill(nNode,"k.unit.ma17.ap02");
 nNode:=p_AddRunScript(nNode,"Brief/ma17.sc");
 nNode:=p_AddCommander(nNode,"Magician");
 nNode:=p_AddMissionInfo(nNode,0);
 nNode:=p_SetMissionSummary(
  nNode,"Town Protection (A17)",
  "Three royal battle beasts$and three dragons attack$Veto Fort, marked red$Kill 'em all!",
  "Route/A17/ms.rt",ConvertColor(255,0,0),2,2
 );
 s_NewProject("ProjectA17",nNode);
}

func void CreateProjectA18()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/ma18.txt");
 nNode:=p_AddSuccessReached(nNode,"k.unit.ma18.02",2355.5,-2607.5,20.0);
 nNode:=p_AddFiledKill(nNode,"k.unit.ma18.02");
 nNode:=p_AddRunScript(nNode,"Brief/ma18.sc");
 nNode:=p_AddCommander(nNode,"Kingdom");
 nNode:=p_AddMissionInfo(nNode,0);
 nNode:=p_SetMissionSummary(
  nNode,"Escort Protection(A18)",
  "Meet the escort - cargo carrier$with guards proceeding$from Bencon to Mill of Happiness$Their route shown White$Mission is successfull,$if the carrier reaches the destination",
  "Route/A18/ms.rt",ConvertColor(255,255,255),2,2
 );
 s_NewProject("ProjectA18",nNode);
}

func void CreateProjectA19()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/ma19.txt");
 nNode:=p_AddFiledReached(nNode,"k.unit.ma19.00",1612,-3536,20);
 nNode:=p_AddFiledReached(nNode,"k.unit.ma19.01",1646,-3511,20);
 nNode:=p_AddFiledReached(nNode,"k.unit.ma19.02",1653,-3479,20);
 nNode:=p_AddFiledReached(nNode,"k.unit.ma19.03",1618,-3478,20);
 nNode:=p_AddFiledReached(nNode,"k.unit.ma19.04",1629,-3423,20);
 nNode:=p_AddSuccessKill(nNode,"k.unit.ma19.00");
 nNode:=p_AddSuccessKill(nNode,"k.unit.ma19.01");
 nNode:=p_AddSuccessKill(nNode,"k.unit.ma19.02");
 nNode:=p_AddSuccessKill(nNode,"k.unit.ma19.03");
 nNode:=p_AddSuccessKill(nNode,"k.unit.ma19.04");
 nNode:=p_AddRunScript(nNode,"Brief/ma19.sc");
 nNode:=p_AddCommander(nNode,"Magician");
 nNode:=p_SetMissionSummary(
  nNode,"Escort Assault(A19)",
  "Destroy four Royal battle beasts$and cargo carriage$moving from Gold Horn$to Dragon Heart$(their possible route shown red)$Mission is a failure if$they reach the destination$Beware partisans and$tower artillery in Emerald Dragon",
   "Route/A19/ms.rt",ConvertColor(255,0,0),2,2
 );
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectA19",nNode);
}

func void CreateProjectA20()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/ma20.txt");
 nNode:=p_AddSuccessKill(nNode,"k.unit.ma20.00");
 nNode:=p_AddSuccessKill(nNode,"k.unit.ma20.ap00");
 nNode:=p_AddRunScript(nNode,"Brief/ma20.sc");
 nNode:=p_AddCommander(nNode,"Magician");
 nNode:=p_SetMissionSummary(
  nNode,"Aura of Death Protection",
  "Defend our recruit center$Kill the royal beast$and the dragon",
   "Route/A20/ms.rt",ConvertColor(255,0,0),2,2
 );
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectA20",nNode);
}

func void CreateProjectA21()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/ma21.txt");

 nNode:=p_AddSuccessKill(nNode,"m.unit.ma21.00");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma21.01");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma21.ap00");
 nNode:=p_AddRunScript(nNode,"Brief/ma21.sc");
 nNode:=p_AddCommander(nNode,"Kingdom");
 nNode:=p_AddMissionInfo(nNode,0);
 nNode:=p_SetMissionSummary(
  nNode,"Recruit Center Protection",
  "Repel attack of enemy forces$Two magician beasts and helicopter$must be perished$Our tower artillery is your support$during this", 
  "Route/A21/ms.rt",ConvertColor(255,0,0),2,2
 );
 s_NewProject("ProjectA21",nNode);
}


func void CreateProjectPIN()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/pin.txt");
 nNode:=p_AddSuccessKill(nNode,"hwz.mag");
 nNode:=p_AddRunScript(nNode,"Brief/Pin.sc");
 nNode:=p_AddCommander(nNode,"Kingdom");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectA21",nNode);

 nNode:=p_SetMissionSummary(
  nNode,"PIN's shitty mission",
  "Just do nothing",
  "Route/A21/ms.rt",ConvertColor(255,0,0),2,2
 );

}


func void CreateTestProject()
{

 Create2G01Project();
 Create2G03Project();
 Create2G04Project();
 Create2G05Project();
 Create2G06Project();

// CreateProjectA12();
 CreateProjectA19();
 CreateProjectA18();
 CreateProjectA17();
 CreateProjectA13();
 CreateProjectA14();
 CreateProjectA16();
 CreateProjectA15();
 CreateProjectA20();
 CreateProjectA21();


 CreateProjectPIN();
 
}
