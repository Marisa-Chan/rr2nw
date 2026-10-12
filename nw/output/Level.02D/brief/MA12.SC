include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"

// -------------------------- Start of Mission A12 --------------------------
func void CreateGroupsA12()
 var str szGroupName, szComName;
 var int nRouteCTID,nPeopleCTID;
{
 // Confederates (Magician)

 szComName:="Magician";
 szGroupName:="M.Group.ma12.00";
 CreateGroup(szGroupName,szComName);
 CreateUnit("M.Unit.ma12.00",szGroupName,"Tank.Magic.Attr");
 AddRouteToMember(szComName,szGroupName,"ma12.route.g0","Routes/A12/g0.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma12.route.p0","Routes/A12/p0.rt",1,0);

 szGroupName:="M.Group.ma12.01";
 CreateGroup(szGroupName,szComName);
 CreateUnit("M.Unit.ma12.01",szGroupName,"Tank.Magic.Attr");
 AddRouteToMember(szComName,szGroupName,"ma12.route.g1","Routes/A12/g1.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma12.route.p1","Routes/A12/p1.rt",1,0);

 szGroupName:="M.Group.ma12.02";
 CreateGroup(szGroupName,szComName);
 CreateUnit("M.Unit.ma12.02",szGroupName,"Tank.Magic.Attr");
 AddRouteToMember(szComName,szGroupName,"ma12.route.g2","Routes/A12/g2.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma12.route.p2","Routes/A12/p2.rt",1,0);

 // Enemies (Kingdom)

 szComName:="Kingdom";
 szGroupName:="K.Group.ma12.00";
 CreateGroup(szGroupName,szComName);
 CreateUnit("K.Unit.ma12.bs0",szGroupName,"Tank.BallSenderFirst.Attr");
 AddRouteToMember(szComName,szGroupName,"ma12.route.ebs0","Routes/A12/ebs0.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma12.route.ebs0","Routes/A12/ebs0.rt",1,0);

 szGroupName:="K.Group.ma12.01";
 CreateGroup(szGroupName,szComName);
 CreateUnit("K.Unit.ma12.bs1",szGroupName,"Tank.BallSenderFirst.Attr");
 AddRouteToMember(szComName,szGroupName,"ma12.route.ebs1","Routes/A12/ebs1.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma12.route.ebs1","Routes/A12/ebs1.rt",1,0);

 szGroupName:="K.Group.ma12.02";
 CreateGroup(szGroupName,szComName);
 CreateUnit("K.Unit.ma12.bs2",szGroupName,"Tank.BallSenderFirst.Attr");
 AddRouteToMember(szComName,szGroupName,"ma12.route.ebs2","Routes/A12/ebs2.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma12.route.ebs2","Routes/A12/ebs2.rt",1,0);

 szGroupName:="K.Group.ma12.03";
 CreateGroup(szGroupName,szComName);
 CreateUnit("K.Unit.ma12.bs3",szGroupName,"Tank.BallSenderFirst.Attr");
 AddRouteToMember(szComName,szGroupName,"ma12.route.ebs3","Routes/A12/ebs3.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma12.route.ebs3","Routes/A12/ebs3.rt",1,0);

 szGroupName:="K.Group.ma12.04";
 CreateGroup(szGroupName,szComName);
 CreateUnit("K.Unit.ma12.bs4",szGroupName,"Tank.BallSenderFirst.Attr");
 AddRouteToMember(szComName,szGroupName,"ma12.route.ebs4","Routes/A12/ebs4.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma12.route.ebs4","Routes/A12/ebs4.rt",1,0);

 nRouteCTID:=s_SearchSeanceClassTable("Route");
 nPeopleCTID:=s_SearchSeanceClassTable("People");
 s_LoadRoute(nRouteCTID,"Routes\A12\eman0.rt","ma12.route.eman0");
 CreateManName(nPeopleCTID,"Peop.Attr.man_knht","ma12.route.eman0",0,"K.Unit.ma12.man0");

 s_LoadRoute(nRouteCTID,"Routes\A12\eman1.rt","ma12.route.eman1");
 CreateManName(nPeopleCTID,"Peop.Attr.man_knht","ma12.route.eman1",0,"K.Unit.ma12.man1");

 s_LoadRoute(nRouteCTID,"Routes\A12\eman2.rt","ma12.route.eman2");
 CreateManName(nPeopleCTID,"Peop.Attr.man_knht","ma12.route.eman2",0,"K.Unit.ma12.man2");

 s_LoadRoute(nRouteCTID,"Routes\A12\eman3.rt","ma12.route.eman3");
 CreateManName(nPeopleCTID,"Peop.Attr.man_knht","ma12.route.eman3",0,"K.Unit.ma12.man3");

 s_LoadRoute(nRouteCTID,"Routes\A12\eman4.rt","ma12.route.eman4");
 CreateManName(nPeopleCTID,"Peop.Attr.man_knht","ma12.route.eman4",0,"K.Unit.ma12.man4");

 s_LoadRoute(nRouteCTID,"Routes\A12\eman5.rt","ma12.route.eman5");
 CreateManName(nPeopleCTID,"Peop.Attr.man_knht","ma12.route.eman5",0,"K.Unit.ma12.man5");
}

func void SetTaxiA12()
 var int nTaxiCTID;
{nTaxiCTID:=s_SearchSeanceClassTable("Taxi");
 CreateTaxi(nTaxiCTID,[3125.899,2776.687,52.613],"Taxi.Attr.anl_00");
 CreateTaxi(nTaxiCTID,[2437.039,2454.647,68.819],"Taxi.Attr.anl_02");
}

func void main()
{SetTaxiA12();
 CreateGroupsA12();
}
// --------------------------- End of Mission A12 ---------------------------
