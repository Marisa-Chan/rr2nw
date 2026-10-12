func void CreateProjectA12()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Briefs/ma12.brf");
 nNode:=p_AddSuccessKill(nNode,"K.Unit.ma12.bs0");
 nNode:=p_AddSuccessKill(nNode,"K.Unit.ma12.bs1");
 nNode:=p_AddSuccessKill(nNode,"K.Unit.ma12.bs2");
 nNode:=p_AddSuccessKill(nNode,"K.Unit.ma12.bs3");
 nNode:=p_AddSuccessKill(nNode,"K.Unit.ma12.bs4");
 nNode:=p_AddRunScript(nNode,"Briefs/ma12.sc");
 nNode:=p_AddCommander(nNode,"Magician");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectA12",nNode);
}

func void CreateProjectA13()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Briefs/ma13.brf");
 nNode:=p_AddSuccessKill(nNode,"k.unit.ma13.00");
 nNode:=p_AddSuccessKill(nNode,"k.unit.ma13.01");
 nNode:=p_AddSuccessKill(nNode,"k.unit.ma13.02");
 nNode:=p_AddSuccessKill(nNode,"k.unit.ma13.03");
 nNode:=p_AddRunScript(nNode,"Briefs/ma13.sc");
 nNode:=p_AddCommander(nNode,"Magician");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectA13",nNode);
}

func void CreateProjectA14()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Briefs/ma14.brf");
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
 nNode:=p_AddRunScript(nNode,"Briefs/ma14.sc");
 nNode:=p_AddCommander(nNode,"Kingdom");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectA14",nNode);
}

func void CreateProjectA15()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Briefs/ma15.brf");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma15.mr00");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma15.mr01");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma15.mr02");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma15.mr03");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma15.00");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma15.01");
 nNode:=p_AddRunScript(nNode,"Briefs/ma15.sc");
 nNode:=p_AddCommander(nNode,"Kingdom");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectA15",nNode);
}

func void CreateProjectA16()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Briefs/ma16.brf");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma16.mr00");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma16.mr01");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma16.mr02");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma16.mr03");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma16.bs00");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma16.bs01");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma16.bs02");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma16.ap00");
 nNode:=p_AddRunScript(nNode,"Briefs/ma16.sc");
 nNode:=p_AddCommander(nNode,"Kingdom");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectA16",nNode);
}

func void CreateProjectA17()
 var int nNode;
{nNode:=p_AddPlayBriefing(s_PNodeNULL(),"Briefs/ma17.brf");
/*
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma16.mr00");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma16.mr01");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma16.mr02");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma16.mr03");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma16.bs00");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma16.bs01");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma16.bs02");
 nNode:=p_AddSuccessKill(nNode,"m.unit.ma16.ap00");
*/
 nNode:=p_AddRunScript(nNode,"Briefs/ma17.sc");
 nNode:=p_AddCommander(nNode,"Kingdom");
 nNode:=p_AddMissionInfo(nNode,0);
 s_NewProject("ProjectA17",nNode);
}

func void CreateTestProject()
{
 CreateProjectA12();
 CreateProjectA13();
 CreateProjectA14();
 CreateProjectA16();
 CreateProjectA15();
 CreateProjectA17();
}
