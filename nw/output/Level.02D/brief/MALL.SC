include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"

// -------------------------- Start of Mission A12 --------------------------
func void CreateGroupsA12()
 var str szGroupName, szComName;
 var int nRouteCTID,nPeopleCTID;
{
 // Confederates (Kingdom)

 szComName:="Kingdom";
/*
 szGroupName:="K.Group.ma12.00";
 CreateGroup(szGroupName,szComName);
 CreateUnit("K.Unit.ma12.00",szGroupName,"Tank.King.Attr");
 AddRouteToMember(szComName,szGroupName,"ma12.route.g0","Routes/A12/g0.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma12.route.p0","Routes/A12/p0.rt",1,0);

 szGroupName:="K.Group.ma12.01";
 CreateGroup(szGroupName,szComName);
 CreateUnit("K.Unit.ma12.01",szGroupName,"Tank.Magic.Attr");
 AddRouteToMember(szComName,szGroupName,"ma12.route.g1","Routes/A12/g1.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma12.route.p1","Routes/A12/p1.rt",1,0);
*/
 szGroupName:="K.Group.ma12.02";
 CreateGroup(szGroupName,szComName);
 CreateUnit("K.Unit.ma12.02",szGroupName,"Tank.MortarFirst.Attr");
 AddRouteToMember(szComName,szGroupName,"ma12.route.mr0","Routes/A12/mr0.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma12.route.mr0","Routes/A12/mr0.rt",1,0);

 szGroupName:="K.Group.ma12.03";
 CreateGroup(szGroupName,szComName);
 CreateUnit("K.Unit.ma12.03",szGroupName,"Tank.MortarSecond.Attr");
 AddRouteToMember(szComName,szGroupName,"ma12.route.mr1","Routes/A12/mr1.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma12.route.mr1","Routes/A12/mr1.rt",1,0);

 szGroupName:="K.Group.ma12.04";
 CreateGroup(szGroupName,szComName);
 CreateUnit("K.Unit.ma12.04",szGroupName,"Tank.BallSenderFirst.Attr");
 AddRouteToMember(szComName,szGroupName,"ma12.route.bs0","Routes/A12/bs0.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma12.route.bs0","Routes/A12/bs0.rt",1,0);

 szGroupName:="K.Group.ma12.05";
 CreateGroup(szGroupName,szComName);
 CreateUnit("K.Unit.ma12.05",szGroupName,"Tank.BallSenderSecond.Attr");
 AddRouteToMember(szComName,szGroupName,"ma12.route.bs1","Routes/A12/bs1.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma12.route.bs1","Routes/A12/bs1.rt",1,0);

 szGroupName:="K.Group.ma12.06";
 CreateGroup(szGroupName,szComName);
 CreateUnit("K.Unit.ma12.06",szGroupName,"Tank.MortarFirst.Attr");
 AddRouteToMember(szComName,szGroupName,"ma12.route.test","Routes/A12/test.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma12.route.test","Routes/A12/test.rt",1,0);

 szGroupName:="K.Group.ma12.07";
 CreateGroup(szGroupName,szComName);
 CreateUnit("K.Unit.ma12.07",szGroupName,"Tank.MortarFirst.Attr");
 AddRouteToMember(szComName,szGroupName,"ma12.route.test1","Routes/A12/test1.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma12.route.test1","Routes/A12/test1.rt",1,0);

 szGroupName:="K.Group.ma12.08";
 CreateGroup(szGroupName,szComName);
 CreateUnit("K.Unit.ma12.08",szGroupName,"Tank.BallSenderFirst.Attr");
 AddRouteToMember(szComName,szGroupName,"ma12.route.test2","Routes/A12/test2.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma12.route.test2","Routes/A12/test2.rt",1,0);

 szGroupName:="K.Group.ma12.09";
 CreateGroup(szGroupName,szComName);
 CreateUnit("K.Unit.ma12.09",szGroupName,"Tank.MortarFirst.Attr");
 AddRouteToMember(szComName,szGroupName,"ma12.route.test3","Routes/A12/test3.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma12.route.test3","Routes/A12/test3.rt",1,0);

 szGroupName:="K.Group.ma12.10";
 CreateGroup(szGroupName,szComName);
 CreateUnit("K.Unit.ma12.10",szGroupName,"Tank.BallSenderFirst.Attr");
 AddRouteToMember(szComName,szGroupName,"ma12.route.test4","Routes/A12/test4.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma12.route.test4","Routes/A12/test4.rt",1,0);

 szGroupName:="K.Group.ma12.11";
 CreateGroup(szGroupName,szComName);
 CreateUnit("K.Unit.ma12.11",szGroupName,"Tank.MortarFirst.Attr");
 AddRouteToMember(szComName,szGroupName,"ma12.route.test5","Routes/A12/test5.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma12.route.test5","Routes/A12/test5.rt",1,0);

 szGroupName:="K.Group.ma12.12";
 CreateGroup(szGroupName,szComName);
 CreateUnit("K.Unit.ma12.12",szGroupName,"Tank.BallSenderFirst.Attr");
 AddRouteToMember(szComName,szGroupName,"ma12.route.test6","Routes/A12/test6.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma12.route.test6","Routes/A12/test6.rt",1,0);

 nRouteCTID:=s_SearchSeanceClassTable("Route");
 nPeopleCTID:=s_SearchSeanceClassTable("People");
 s_LoadRoute(nRouteCTID,"Routes\A12\ap0.rt","ma12.route.ap0");
 CreateManName(nPeopleCTID,"Peop.Attr.anl_02","ma12.route.ap0",0,"K.Unit.ma12.02");

 s_LoadRoute(nRouteCTID,"Routes\A12\ap1.rt","ma12.route.ap1");
 CreateManName(nPeopleCTID,"Peop.Attr.vhl_hel","ma12.route.ap1",0,"K.Unit.ma12.03");

 s_LoadRoute(nRouteCTID,"Routes\A12\m0.rt","ma12.route.m0");
 CreateManName(nPeopleCTID,"Peop.Attr.man_knht","ma12.route.m0",0,"K.Unit.ma12.04");

 s_LoadRoute(nRouteCTID,"Routes\A12\m1.rt","ma12.route.m1");
 CreateManName(nPeopleCTID,"Peop.Attr.man_peas","ma12.route.m1",0,"K.Unit.ma12.05");

 s_LoadRoute(nRouteCTID,"Routes\A12\m2.rt","ma12.route.m2");
 CreateManName(nPeopleCTID,"Peop.Attr.man_mknt","ma12.route.m2",0,"K.Unit.ma12.06");

 s_LoadRoute(nRouteCTID,"Routes\A12\m3.rt","ma12.route.m3");
 CreateManName(nPeopleCTID,"Peop.Attr.vhl_wag","ma12.route.m3",0,"K.Unit.ma12.07");

 // Enemies (Magician)

}

func void SetTaxiA12()
 var int nTaxiCTID;
{nTaxiCTID:=s_SearchSeanceClassTable("Taxi");
 CreateTaxi(nTaxiCTID,[2655.83,2841.07,51.67],"Taxi.Attr.vhl_hel");
}

func void main()
{SetTaxiA12();
 CreateGroupsA12();
}
// --------------------------- End of Mission A12 ---------------------------
