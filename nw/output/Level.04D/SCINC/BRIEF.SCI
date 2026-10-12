// -------------------------- LEV ----------------------------------------

func void CreateProjectL00()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/l00.txt");
 /*nNode:=p_AddSuccessKill(nNode,"C.Unit.l00.00");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l00.01");*/
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l00.02");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l00.03");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l00.04");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l00.05");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l00.06");
 nNode:=p_AddFiledKill(nNode,"A.Unit.l00.00");
 nNode:=p_AddRunScript(nNode,"Brief/l00.sc");
 nNode:=p_AddCommander(nNode,"Actek");
 nNode:=p_SetMissionSummary(nNode, "", "Спасите Командора у РЦ.$Отправтесь в Вархельм.$Уничтожте его гарнизон !",
			    "Route/lev/l00_g5.rt", ConvertColor(255,0,0), 4,2);
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectL00",nNode);
}

func void CreateProjectL00a()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/l00a.txt");
 /*nNode:=p_AddSuccessKill(nNode,"C.Unit.l00.00");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l00.01");*/
 /*nNode:=p_AddSuccessKill(nNode,"C.Unit.l00.02");*/
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l00.03");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l00.04");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l00.05");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l00.06");
 nNode:=p_AddFiledKill(nNode,"A.Unit.l00.00");
 nNode:=p_AddRunScript(nNode,"Brief/l00a.sc");
 nNode:=p_AddCommander(nNode,"Actek");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectL00a",nNode);
}

func void CreateProjectL01()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/l01.txt");
 /*nNode:=p_AddSuccessKill(nNode,"A.Unit.l01.00");*/
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l01.01");
 /*nNode:=p_AddSuccessKill(nNode,"A.Unit.l01.02");*/
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l01.03");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l01.04");
 /*nNode:=p_AddSuccessKill(nNode,"A.Unit.l01.05");*/
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l01.06");
 /*nNode:=p_AddFiledKill(nNode,"C.Unit.l01.00");*/
 nNode:=p_AddRunScript(nNode,"Brief/l01.sc");
 nNode:=p_AddCommander(nNode,"Colony");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectL01",nNode);
}

func void CreateProjectL01a()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/l01.txt");
 /*nNode:=p_AddSuccessKill(nNode,"A.Unit.l01.00");*/
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l01.01");
 /*nNode:=p_AddSuccessKill(nNode,"A.Unit.l01.02");*/
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l01.03");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l01.04");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l01.05");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l01.06");
 /*nNode:=p_AddFiledKill(nNode,"C.Unit.l01.00");*/
 nNode:=p_AddRunScript(nNode,"Brief/l01a.sc");
 nNode:=p_AddCommander(nNode,"Colony");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectL01a",nNode);
}

func void CreateProjectL02()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/l02.txt");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l02.00");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l02.02");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l02.03");
 /*nNode:=p_AddSuccessKill(nNode,"A.Unit.l02.04");*/
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l02.05");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l02.06");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l02.07");
 /*nNode:=p_AddSuccessKill(nNode,"A.Unit.l02.08");*/
 /*nNode:=p_AddSuccessKill(nNode,"A.Unit.l02.09");*/
 /*nNode:=p_AddSuccessKill(nNode,"A.Unit.l02.10");*/
 nNode:=p_AddRunScript(nNode,"Brief/l02.sc");
 nNode:=p_AddCommander(nNode,"Colony");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectL02",nNode);
}

func void CreateProjectL03()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/l03.txt");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l03.02");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l03.03");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l03.04");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l03.05");
 /*nNode:=p_AddSuccessKill(nNode,"C.Unit.l03.06");*/
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l03.07");
 /*nNode:=p_AddSuccessKill(nNode,"C.Unit.l03.08");*/
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l03.09");
 /*nNode:=p_AddSuccessKill(nNode,"C.Unit.l03.10");*/
 nNode:=p_AddRunScript(nNode,"Brief/l03.sc");
 nNode:=p_AddCommander(nNode,"Actek");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectL03",nNode);
}

func void CreateProjectL04()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/l04.txt");
 /*nNode:=p_AddSuccessKill(nNode,"C.Unit.l04.00");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l04.01");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l04.02");*/
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l04.03");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l04.04");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l04.05");
 nNode:=p_AddRunScript(nNode,"Brief/l04.sc");
 nNode:=p_AddCommander(nNode,"Actek");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectL04",nNode);
}

func void CreateProjectL05()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/l05.txt");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l05.00");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l05.01");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l05.02");
 /*nNode:=p_AddSuccessKill(nNode,"C.Unit.l05.03");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l05.04");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l05.05");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l05.06");*/
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l05.07");
 /*nNode:=p_AddSuccessKill(nNode,"C.Unit.l05.08");
 nNode:=p_AddFiledKill(nNode,"A.Unit.l05.00");*/
 nNode:=p_AddRunScript(nNode,"Brief/l05.sc");
 nNode:=p_AddCommander(nNode,"Actek");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectL05",nNode);
}

func void CreateProjectL05a()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/l05.txt");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l05.00");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l05.01");
 /*nNode:=p_AddSuccessKill(nNode,"C.Unit.l05.02");*/
 /*nNode:=p_AddSuccessKill(nNode,"C.Unit.l05.03");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l05.04");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l05.05");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l05.06");*/
 nNode:=p_AddSuccessKill(nNode,"C.Unit.l05.07");
 /*nNode:=p_AddSuccessKill(nNode,"C.Unit.l05.08");
 nNode:=p_AddFiledKill(nNode,"A.Unit.l05.00");*/
 nNode:=p_AddRunScript(nNode,"Brief/l05a.sc");
 nNode:=p_AddCommander(nNode,"Actek");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectL05a",nNode);
}

func void CreateProjectL06()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/l06.txt");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l06.00");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l06.01");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l06.02");
 /*nNode:=p_AddSuccessKill(nNode,"A.Unit.l06.03");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l06.04");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l06.05");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l06.06");*/
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l06.07");
 /*nNode:=p_AddSuccessKill(nNode,"A.Unit.l06.08");
 nNode:=p_AddFiledKill(nNode,"C.Unit.l06.00");*/
 nNode:=p_AddRunScript(nNode,"Brief/l06.sc");
 nNode:=p_AddCommander(nNode,"Colony");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectL06",nNode);
}

func void CreateProjectL06a()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/l06a.txt");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l06.00");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l06.01");
 /*nNode:=p_AddSuccessKill(nNode,"A.Unit.l06.02");*/
 /*nNode:=p_AddSuccessKill(nNode,"A.Unit.l06.03");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l06.04");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l06.05");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l06.06");*/
 nNode:=p_AddSuccessKill(nNode,"A.Unit.l06.07");
 /*nNode:=p_AddSuccessKill(nNode,"A.Unit.l06.08");
 nNode:=p_AddFiledKill(nNode,"C.Unit.l06.00");*/
 nNode:=p_AddRunScript(nNode,"Brief/l06a.sc");
 nNode:=p_AddCommander(nNode,"Colony");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectL06a",nNode);
}

//---------------------------------------------------------------------------

func void CreateProjectAER00()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/aer00.txt");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.aer00.00");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.aer00.01");
 nNode:=p_AddSuccessKill(nNode,"plane.m13_0");
 nNode:=p_AddSuccessKill(nNode,"plane.m13_1");
 nNode:=p_AddRunScript(nNode,"Brief/aer00.sc");
 nNode:=p_AddCommander(nNode,"Actek");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectAER00",nNode);
}

func void CreateProjectAER01()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/aer01.txt");
 nNode:=p_AddSuccessKill(nNode,"plane.m13_0");
 nNode:=p_AddRunScript(nNode,"Brief/aer01.sc");
 nNode:=p_AddCommander(nNode,"Actek");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectAER01",nNode);
}

func void CreateProjectAER02()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/aer02.txt");
 nNode:=p_AddSuccessKill(nNode,"plane.m13_0");
 nNode:=p_AddSuccessKill(nNode,"plane.m13_1");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.AER02.02");
 nNode:=p_AddRunScript(nNode,"Brief/aer02.sc");
 nNode:=p_AddCommander(nNode,"Actek");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectAER02",nNode);
}

func void CreateProjectAER03()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/aer03.txt");
 nNode:=p_AddSuccessKill(nNode,"shiz.a03e0");
 nNode:=p_AddSuccessKill(nNode,"shiz.a03e1");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.AER03.00");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.AER03.01");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.AER03.02");
 nNode:=p_AddRunScript(nNode,"Brief/aer03.sc");
 nNode:=p_AddCommander(nNode,"Colony");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectAER03",nNode);
}

func void CreateProjectAER04()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/aer04.txt");
 nNode:=p_AddSuccessKill(nNode,"Taxia0400");
 nNode:=p_AddSuccessKill(nNode,"Taxia0401");
 nNode:=p_AddSuccessKill(nNode,"Taxia0402");
 nNode:=p_AddSuccessKill(nNode,"Taxia0403");
 nNode:=p_AddSuccessKill(nNode,"plane.a04_0");
 nNode:=p_AddSuccessKill(nNode,"plane.a04_1");
 nNode:=p_AddRunScript(nNode,"Brief/aer04.sc");
 nNode:=p_AddCommander(nNode,"Actek");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectAER04",nNode);
}

func void CreateProjectAER04a()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/aer04.txt");
 nNode:=p_AddSuccessKill(nNode,"Taxia0400");
 nNode:=p_AddSuccessKill(nNode,"Taxia0401");
 nNode:=p_AddSuccessKill(nNode,"Taxia0402");
 nNode:=p_AddSuccessKill(nNode,"Taxia0403");
 nNode:=p_AddSuccessKill(nNode,"plane.a04_0");
 nNode:=p_AddSuccessKill(nNode,"plane.a04_1");
 nNode:=p_AddRunScript(nNode,"Brief/aer04a.sc");
 nNode:=p_AddCommander(nNode,"Actek");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectAER04a",nNode);
}

func void CreateProjectAER04b()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/aer04.txt");
 nNode:=p_AddSuccessKill(nNode,"Taxia0400");
 nNode:=p_AddSuccessKill(nNode,"Taxia0401");
 nNode:=p_AddSuccessKill(nNode,"Taxia0402");
 nNode:=p_AddSuccessKill(nNode,"Taxia0403");
 nNode:=p_AddSuccessKill(nNode,"plane.a04_0");
 /*nNode:=p_AddSuccessKill(nNode,"plane.a04_1");*/
 nNode:=p_AddRunScript(nNode,"Brief/aer04b.sc");
 nNode:=p_AddCommander(nNode,"Actek");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectAER04b",nNode);
}

func void CreateProjectAER05()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/aer05.txt");
 nNode:=p_AddSuccessKill(nNode,"Taxia0500");
 nNode:=p_AddSuccessKill(nNode,"Taxia0501");
 nNode:=p_AddSuccessKill(nNode,"Taxia0502");
 nNode:=p_AddSuccessKill(nNode,"Taxia0503");
 nNode:=p_AddSuccessKill(nNode,"shiz.a05_0");
 nNode:=p_AddSuccessKill(nNode,"shiz.a05_1");
 nNode:=p_AddRunScript(nNode,"Brief/aer05.sc");
 nNode:=p_AddCommander(nNode,"Colony");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectAER05",nNode);
}

func void CreateProjectAER06()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/aer06.txt");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.aer06.00");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.aer06.01");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.aer06.02");
 nNode:=p_AddSuccessKill(nNode,"plane.a06_0");
 nNode:=p_AddSuccessKill(nNode,"plane.a06_1");
 nNode:=p_AddRunScript(nNode,"Brief/aer06.sc");
 nNode:=p_AddCommander(nNode,"Actek");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectAER06",nNode);
}

func void CreateProjectAER07()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/aer07.txt");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.aer07.00");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.aer07.01");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.aer07.02");
 nNode:=p_AddSuccessKill(nNode,"shiz.a07_0");
 nNode:=p_AddSuccessKill(nNode,"plane.a07_0");
 nNode:=p_AddSuccessKill(nNode,"plane.a07_1");
 nNode:=p_AddRunScript(nNode,"Brief/aer07.sc");
 nNode:=p_AddCommander(nNode,"Colony");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectAER07",nNode);
}

func void CreateProjectAER08()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/aer08.txt");
 nNode:=p_AddSuccessKill(nNode,"plane.a08_0");
 nNode:=p_AddSuccessKill(nNode,"plane.a08_1");
 nNode:=p_AddSuccessKill(nNode,"plane.a08_2");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.aer08.00");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.aer08.01");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.aer08.02");
 nNode:=p_AddRunScript(nNode,"Brief/aer08.sc");
 nNode:=p_AddCommander(nNode,"Actek");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectAER08",nNode);
}

func void CreateProjectAER09()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/aer09.txt");
 nNode:=p_AddSuccessKill(nNode,"helic.a09_0");
 nNode:=p_AddSuccessKill(nNode,"helic.a09_1");
 nNode:=p_AddSuccessKill(nNode,"helic.a09_2");
 nNode:=p_AddSuccessKill(nNode,"shiz.a09_0");
 nNode:=p_AddSuccessKill(nNode,"shiz.a09_1");
 nNode:=p_AddRunScript(nNode,"Brief/aer09.sc");
 nNode:=p_AddCommander(nNode,"Colony");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectAER09",nNode);
}

func void CreateProjectAER10()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/aer10.txt");
 nNode:=p_AddSuccessKill(nNode,"plane.a10_0");
 nNode:=p_AddSuccessKill(nNode,"plane.a10_1");
 nNode:=p_AddSuccessKill(nNode,"plane.a10_2");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.aer10.00");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.aer10.01");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.aer10.02");
 nNode:=p_AddRunScript(nNode,"Brief/aer10.sc");
 nNode:=p_AddCommander(nNode,"Actek");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectAER10",nNode);
}

func void CreateProjectAER11()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/aer11.txt");
 nNode:=p_AddSuccessKill(nNode,"shiz.a11_1");
 nNode:=p_AddSuccessKill(nNode,"helic.a11_0");
 nNode:=p_AddSuccessKill(nNode,"helic.a11_1");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.aer11.00");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.aer11.01");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.aer11.02");
 nNode:=p_AddRunScript(nNode,"Brief/aer11.sc");
 nNode:=p_AddCommander(nNode,"Colony");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectAER11",nNode);
}

func void CreateProjectAER12()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/aer12.txt");
 nNode:=p_AddSuccessKill(nNode,"plane.a12_0");
 nNode:=p_AddSuccessKill(nNode,"plane.a12_1");
 nNode:=p_AddSuccessKill(nNode,"plane.a12_2");
 nNode:=p_AddRunScript(nNode,"Brief/aer12.sc");
 nNode:=p_AddCommander(nNode,"Actek");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectAER12",nNode);
}

func void CreateProjectAER13()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/aer13.txt");
 nNode:=p_AddSuccessKill(nNode,"helic.a13_0");
 nNode:=p_AddSuccessKill(nNode,"helic.a13_1");
 nNode:=p_AddSuccessKill(nNode,"shiz.a13_0");
 nNode:=p_AddRunScript(nNode,"Brief/aer13.sc");
 nNode:=p_AddCommander(nNode,"Colony");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectAER13",nNode);
}

func void CreateProjectAER14()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/aer14.txt");
 nNode:=p_AddSuccessKill(nNode,"plane.a14_0");
 nNode:=p_AddSuccessKill(nNode,"plane.a14_1");
 nNode:=p_AddSuccessKill(nNode,"plane.a14_2");
 //nNode:=p_AddSuccessReached(nNode,"A.Unit.aer14.00",2112.00,-3705.00,77.00);
 nNode:=p_AddFiledKill(nNode,"A.Unit.aer14.00");
 nNode:=p_AddRunScript(nNode,"Brief/aer14.sc");
 nNode:=p_AddCommander(nNode,"Actek");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectAER14",nNode);
}

func void CreateProjectAER15()
 var int nNode;
{
 nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/aer15.txt");
 nNode:=p_AddSuccessKill(nNode,"helic.a15_0");
 nNode:=p_AddSuccessKill(nNode,"helic.a15_1");
 nNode:=p_AddSuccessKill(nNode,"shiz.a15_0");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.aer15.00");
 nNode:=p_AddFiledKill(nNode,"C.Unit.aer15.00");
 nNode:=p_AddRunScript(nNode,"Brief/aer15.sc");
 nNode:=p_AddCommander(nNode,"Colony");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectAER15",nNode);
}



// ------------------------- Mission projects ----------------------------
func void CreateProjectA00()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/ma00.txt");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.ma0.00");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.ma0.01");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.ma0.02");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.ma0.03");
 nNode:=p_AddRunScript(nNode,"Brief/ma00.sc");
 nNode:=p_AddCommander(nNode,"Actek");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectA00",nNode);
}

func void CreateProjectA01()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/ma01.txt");
 nNode:=p_AddFiledSuccess(nNode);
 nNode:=p_AddFiledKill(nNode,"A.Unit.ma01.01");
 nNode:=p_AddSuccessReached(nNode,"A.Unit.ma01.01",1356.00,-3094.00,10.00);
 nNode:=p_AddRunScript(nNode,"Brief/ma01.sc");
 nNode:=p_AddCommander(nNode,"Actek");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectA01",nNode);
}

func void CreateProjectA02()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/ma02.txt");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.ma02.00");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.ma02.01");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.ma02.02");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.ma02.03");
 nNode:=p_AddFiledReached(nNode,"C.Unit.ma02.00",4312.00,-2976.00,10.00);
 nNode:=p_AddFiledReached(nNode,"C.Unit.ma02.01",4336.00,-3006.00,10.00);
 nNode:=p_AddFiledReached(nNode,"C.Unit.ma02.02",4350.00,-3034.00,10.00);
 nNode:=p_AddFiledReached(nNode,"C.Unit.ma02.03",4370.00,-3060.00,10.00);
 nNode:=p_AddRunScript(nNode,"Brief/ma02.sc");
 nNode:=p_AddCommander(nNode,"Actek");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectA02",nNode);
}

func void CreateProjectA03()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/ma03.txt");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.ma03.00");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.ma03.01");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.ma03.02");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.ma03.03");
 nNode:=p_AddRunScript(nNode,"Brief/ma03.sc");
 nNode:=p_AddCommander(nNode,"Colony");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectA03",nNode);
}

func void CreateProjectA04()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/ma04.txt");
 nNode:=p_AddFiledSuccess(nNode);
 nNode:=p_AddFiledKill(nNode,"C.Unit.ma04.02");
 nNode:=p_AddSuccessReached(nNode,"C.Unit.ma04.02",3096.00,-1972.00,10.00);
 nNode:=p_AddRunScript(nNode,"Brief/ma04.sc");
 nNode:=p_AddCommander(nNode,"Colony");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectA04",nNode);
}

func void CreateProjectA05()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/ma05.txt");
 nNode:=p_AddFiledSuccess(nNode);
 nNode:=p_AddFiledKill(nNode,"A.Unit.ma05.02");
 nNode:=p_AddSuccessReached(nNode,"A.Unit.ma05.02",2434.00,-2416.00,10.00);
 nNode:=p_AddRunScript(nNode,"Brief/ma05.sc");
 nNode:=p_AddCommander(nNode,"Actek");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectA05",nNode);
}

func void CreateProjectA06()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/ma06.txt");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.ma06.00");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.ma06.01");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.ma06.02");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.ma06.03");
 nNode:=p_AddFiledReached(nNode,"A.Unit.ma06.00",2074.00,-3090.00,20.00);
 nNode:=p_AddFiledReached(nNode,"A.Unit.ma06.01",2074.00,-3126.00,20.00);
 nNode:=p_AddFiledReached(nNode,"A.Unit.ma06.02",2122.00,-3100.00,20.00);
 nNode:=p_AddFiledReached(nNode,"A.Unit.ma06.03",2114.00,-3136.00,20.00);
 nNode:=p_AddRunScript(nNode,"Brief/ma06.sc");
 nNode:=p_AddCommander(nNode,"Colony");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectA06",nNode);
}

func void CreateProjectA07()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/ma07.txt");
/*
 nNode:=p_AddSuccessKill(nNode,"A.Unit.ma07.00");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.ma07.01");
 nNode:=p_AddSuccessReached(nNode,"  ",2930.113,-2445.221,50.00);
 nNode:=p_AddFiledKill(nNode,"  ");
*/
 nNode:=p_AddRunScript(nNode,"Brief/ma07.sc");
 nNode:=p_AddCommander(nNode,"Colony");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectA07",nNode);
}

func void CreateProjectA08()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/ma08.txt");
/*
 nNode:=p_AddSuccessKill(nNode,"C.Unit.ma08.00");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.ma08.01");
 nNode:=p_AddSuccessReached(nNode,"  ",2394.186,-2411.241,50.00);
 nNode:=p_AddFiledKill(nNode,"  ");
*/
 nNode:=p_AddRunScript(nNode,"Brief/ma08.sc");
 nNode:=p_AddCommander(nNode,"Actek");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectA08",nNode);
}

func void CreateProjectA09()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/ma09.txt");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.ma09.00");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.ma09.01");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.ma09.02");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.ma09.03");
 nNode:=p_AddRunScript(nNode,"Brief/ma09.sc");
 nNode:=p_AddCommander(nNode,"Actek");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectA09",nNode);
}

func void CreateProjectA10()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/ma10.txt");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.ma10.00");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.ma10.01");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.ma10.02");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.ma10.03");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.ma10.04");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.ma10.05");
 nNode:=p_AddSuccessKill(nNode,"C.Unit.ma10.06");
 nNode:=p_AddRunScript(nNode,"Brief/ma10.sc");
 nNode:=p_AddCommander(nNode,"Actek");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectA10",nNode);
}

func void CreateProjectA11()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Brief/ma11.txt");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.ma11.00");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.ma11.01");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.ma11.02");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.ma11.03");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.ma11.04");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.ma11.05");
 nNode:=p_AddSuccessKill(nNode,"A.Unit.ma11.06");
 nNode:=p_AddRunScript(nNode,"Brief/ma11.sc");
 nNode:=p_AddCommander(nNode,"Colony");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectA11",nNode);
}
// ---------------------- End of mission projects ------------------------



func void CreateGProject()
 var int node,node0;
 {
     node0 := s_NewPNode( COM_PLAY_BRIEFING, s_PNodeNULL(),s_PNodeNULL());
     s_OpenProjectData  ( node0 );
     s_ProjectWriteStr  ( node0, "BRIEF/brg00.txt" );
     s_CloseProjectData ( node0 );

     node := s_NewPNode( COM_FILED_KILL, s_PNodeNULL(),node0);
     s_OpenProjectData  ( node );
     s_ProjectWriteStr  ( node, "A.Unit.m0g0" );
     s_CloseProjectData ( node );


     node := s_NewPNode( COM_SUCCESS_REACHED, s_PNodeNULL(),node);
     s_OpenProjectData  ( node );
     s_ProjectWriteStr  ( node, "A.Unit.m0g0" );
     s_ProjectWriteFloat( node, 1930.000000 );
     s_ProjectWriteFloat( node, -3832.000000 );
     s_ProjectWriteFloat( node, 60.000000 );
     s_CloseProjectData ( node );

     node := s_NewPNode( COM_RUN_SCRIPT, s_PNodeNULL(),node);
     s_OpenProjectData  ( node );
     s_ProjectWriteStr  ( node, "BRIEF/briefg0.sc" );
     s_CloseProjectData ( node );

     node := s_NewPNode( COM_0COMMANDER, s_PNodeNULL(),node);
     s_OpenProjectData  ( node );
     s_ProjectWriteStr  ( node, "Actek" );
     s_CloseProjectData ( node );

     node := s_NewPNode( COM_MISSIONINFO, s_PNodeNULL(),node);
     s_OpenProjectData  ( node );
     s_ProjectWriteInt  ( node, 1 );
     s_CloseProjectData ( node );

	

     s_NewProject( "ProjectG0", node );
 }

func void CreateG1Project()
 var int node,node0;
 {
     node0 := s_NewPNode( COM_PLAY_BRIEFING, s_PNodeNULL(),s_PNodeNULL());
     s_OpenProjectData  ( node0 );
     s_ProjectWriteStr  ( node0, "BRIEF/brg01.txt" );
     s_CloseProjectData ( node0 );

     node0 := p_AddSuccessKill(node0,"A.Unit.m1g0");
     node0 := p_AddSuccessKill(node0,"A.Unit.m1g1");
     node0 := p_AddSuccessKill(node0,"A.Unit.m1g2");


     node := s_NewPNode( COM_RUN_SCRIPT, s_PNodeNULL(),node0);
     s_OpenProjectData  ( node );
     s_ProjectWriteStr  ( node, "BRIEF/briefg1.sc" );
     s_CloseProjectData ( node );

     node := s_NewPNode( COM_0COMMANDER, s_PNodeNULL(),node);
     s_OpenProjectData  ( node );
     s_ProjectWriteStr  ( node, "Colony" );
     s_CloseProjectData ( node );

     s_NewProject( "ProjectG1", node );

 }

func void CreateG2Project()
 var int nNode;
 {
	nNode := p_AddPlayBriefing (s_PNodeNULL(), "Brief/brg02.txt");
	nNode := p_AddSuccessKill (nNode, "A.Unit.m2g0");
	nNode := p_AddSuccessKill (nNode, "A.Unit.m2g1");
	nNode := p_AddSuccessKill (nNode, "A.Unit.m2g2");
	nNode := p_AddRunScript (nNode, "Brief/briefg2.sc");
 	nNode := p_AddCommander (nNode, "Colony");
 	nNode := p_AddMissionInfo (nNode, 2);
 	s_NewProject ("ProjectG2", nNode);
}

func void CreateG3Project()
 var int nNode;
 {
	nNode := p_AddPlayBriefing (s_PNodeNULL(), "Brief/brg03.txt");
	nNode := p_AddSuccessKill (nNode, "A.Unit.m3g1");
	nNode := p_AddRunScript (nNode, "Brief/briefg3.sc");
 	nNode := p_AddCommander (nNode, "Colony");
 	nNode := p_AddMissionInfo (nNode, 2);
 	s_NewProject ("ProjectG3", nNode);
}

func void CreateG4Project()
 var int nNode;
 {
	nNode := p_AddPlayBriefing (s_PNodeNULL(), "Brief/brg04.txt");
	nNode := p_AddSuccessKill (nNode, "A.Unit.m4g0");
	nNode := p_AddSuccessKill (nNode, "A.Unit.m4g1");
	nNode := p_AddSuccessKill (nNode, "A.Unit.m4g2");
	nNode := p_AddSuccessKill (nNode, "A.Unit.m4g3");
	nNode := p_AddRunScript (nNode, "Brief/briefg4.sc");
 	nNode := p_AddCommander (nNode, "Colony");
 	nNode := p_AddMissionInfo (nNode, 2);
 	s_NewProject ("ProjectG4", nNode);
}

func void CreateG5Project()
 var int nNode;
 {
	nNode := p_AddPlayBriefing (s_PNodeNULL(), "Brief/brg05.txt");
	nNode := p_AddSuccessKill (nNode, "A.Unit.m5g0");
	nNode := p_AddSuccessKill (nNode, "A.Unit.m5g1");
	nNode := p_AddSuccessKill (nNode, "A.Unit.m5g2");
	nNode := p_AddSuccessKill (nNode, "A.Unit.m5g3");
	nNode := p_AddSuccessKill (nNode, "A.Unit.m5g4");
	nNode := p_AddSuccessKill (nNode, "A.Unit.m5g5");
	nNode := p_AddRunScript (nNode, "Brief/briefg5.sc");
 	nNode := p_AddCommander (nNode, "Colony");
 	nNode := p_AddMissionInfo (nNode, 2);
 	s_NewProject ("ProjectG5", nNode);
}

func void CreateG6Project()
 var int nNode;
 {
	nNode := p_AddPlayBriefing (s_PNodeNULL(), "Brief/brg06.txt");
	nNode := p_AddSuccessReached (nNode, "Vehicle.default", 3120, -1970, 60);
	nNode := p_AddRunScript (nNode, "Brief/briefg6.sc");
 	nNode := p_AddCommander (nNode, "Colony");
 	nNode := p_AddMissionInfo (nNode, 15);
 	s_NewProject ("ProjectG6", nNode);
}

func void CreateG7Project()
 var int nNode;
 {
	nNode := p_AddPlayBriefing (s_PNodeNULL(), "Brief/brg07.txt");
	nNode := p_AddSuccessKill (nNode, "A.Unit.m7g0");
	nNode := p_AddFiledReached (nNode, "A.Unit.m7g0", 2782, -3390, 60);
	nNode := p_AddRunScript (nNode, "Brief/briefg7.sc");
 	nNode := p_AddCommander (nNode, "Colony");
 	nNode := p_AddMissionInfo (nNode, 2);
 	s_NewProject ("ProjectG7", nNode);
}

func void CreateG8Project()
 var int nNode;
 {
	nNode := p_AddPlayBriefing (s_PNodeNULL(), "Brief/brg08.txt");
	nNode := p_AddSuccessKill (nNode, "A.Unit.m8g0");
	nNode := p_AddSuccessKill (nNode, "A.Unit.m8g1");
	nNode := p_AddRunScript (nNode, "Brief/briefg8.sc");
 	nNode := p_AddCommander (nNode, "Colony");
 	nNode := p_AddMissionInfo (nNode, 3);
 	s_NewProject ("ProjectG8", nNode);
}

func void CreateG9Project()
 var int nNode;
 {
	nNode := p_AddPlayBriefing (s_PNodeNULL(), "Brief/brg09.txt");
        nNode := p_AddSuccessReached (nNode, "TaxiHell", 2720, 2720, 60.00 );
	nNode := p_AddRunScript (nNode, "Brief/briefg9.sc");
 	nNode := p_AddCommander (nNode, "Colony");
 	nNode := p_AddMissionInfo (nNode, 15);
 	s_NewProject ("ProjectG9", nNode);
}

func void CreateG10Project()
 var int nNode;
 {
	nNode := p_AddPlayBriefing (s_PNodeNULL(), "Brief/brg10.txt");
        nNode := p_AddSuccessKill (nNode, "shiz.m10e0");
        nNode := p_AddSuccessKill (nNode, "shiz.m10e1");
	nNode := p_AddRunScript (nNode, "Brief/briefg10.sc");
 	nNode := p_AddCommander (nNode, "Colony");
 	nNode := p_AddMissionInfo (nNode, 1);
 	s_NewProject ("ProjectG10", nNode);
}


func void SuavikProject()
 var int node,node0;
{
    node0 := s_NewPNode( COM_PLAY_BRIEFING, s_PNodeNULL(),s_PNodeNULL());
     s_OpenProjectData  ( node0 );
     s_ProjectWriteStr  ( node0, "BRIEF/br00.txt" );
     s_CloseProjectData ( node0 );

     node := s_NewPNode( COM_SUCCESS_KILL,s_PNodeNULL(),node0);
     s_OpenProjectData  ( node );
     s_ProjectWriteStr  ( node, "C.Unit.m0.01" );
     s_CloseProjectData ( node );

     node := s_NewPNode( COM_SUCCESS_KILL,s_PNodeNULL(),node);
     s_OpenProjectData  ( node );
     s_ProjectWriteStr  ( node, "C.Unit.m0.02" );
     s_CloseProjectData ( node );

     node := s_NewPNode( COM_SUCCESS_KILL,s_PNodeNULL(),node);
     s_OpenProjectData  ( node );
     s_ProjectWriteStr  ( node, "C.Unit.m0.03" );
     s_CloseProjectData ( node );


     node := s_NewPNode( COM_RUN_SCRIPT, s_PNodeNULL(),node);
     s_OpenProjectData  ( node );
     s_ProjectWriteStr  ( node, "BRIEF/brief0.sc" );
     s_CloseProjectData ( node );

     node := s_NewPNode( COM_0COMMANDER, s_PNodeNULL(),node);
     s_OpenProjectData  ( node );
     s_ProjectWriteStr  ( node, "Actek" );
     s_CloseProjectData ( node );

     node := s_NewPNode( COM_MISSIONINFO, s_PNodeNULL(),node);
     s_OpenProjectData  ( node );
     s_ProjectWriteInt  ( node, 0 );
     s_CloseProjectData ( node );

     node := s_NewPNode( COM_SKIP_WAY, s_PNodeNULL(),node);
     s_OpenProjectData  ( node );
     s_ProjectWriteFloat( node, 2129.038 );
     s_ProjectWriteFloat( node, 72.535 );
     s_ProjectWriteFloat( node, -3137.272 );
     s_ProjectWriteFloat( node, 30 );
     s_CloseProjectData ( node );

     s_NewProject( "Project1", node );
}


func void CreateTestProject()
 var int node,node0;
 {

     
     CreateProjectA00();
     CreateProjectA01();    
     CreateProjectA02();
     CreateProjectA03();
     CreateProjectA04();
     CreateProjectA05();
     CreateProjectA06();
//   CreateProjectA07();
//   CreateProjectA08();
     CreateProjectA09();
     CreateProjectA10();
     CreateProjectA11();

     //return;
     CreateGProject();
     CreateG1Project();
     CreateG2Project();
     CreateG3Project();
     CreateG4Project();
     CreateG5Project();
     CreateG6Project();

     CreateG7Project();

     CreateG8Project();
     CreateG9Project();
     CreateG10Project();
     

     CreateProjectL00();
     CreateProjectL01a();
     CreateProjectL02();
     CreateProjectL03();
     CreateProjectL04();
     CreateProjectL05();
     CreateProjectL06();

//-----------------------------------------------------------------------------

     CreateProjectAER00();
     CreateProjectAER01();
     CreateProjectAER02();
     CreateProjectAER03();
     CreateProjectAER04();
     CreateProjectAER05();
     CreateProjectAER06();
     CreateProjectAER07();
     CreateProjectAER08();
     CreateProjectAER09();
     CreateProjectAER10();
     CreateProjectAER11();
     CreateProjectAER12();
     CreateProjectAER13();
     CreateProjectAER14();
     CreateProjectAER15();

     SuavikProject();
 }
