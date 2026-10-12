include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"

// -------------------------- Start of Mission AER02 --------------------------

func void CreateGroupsAER02() // own
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
 var int ctID, mctID;
{
 ctID := s_SearchSeanceClassTable("Route");
 mctID:= s_SearchSeanceClassTable("People");

 /* Confederates */

 s_LoadRoute( ctID, "Route\lev\aer02_04.rt", "aer02.route.04"   );
 CreateManName( mctID, "Peop.Attr.shiz_f0a","aer02.route.04"   , 0, "shiz.a02e0" );
 s_SetCommander("shiz.a02e0", "Actek");

 /*comName:="Colony";
 groupName:="C.Group.l15.00";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l15.00",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l00.route.g6","Route/LEV/l00_g6.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g6","Route/LEV/l00_g6.rt",1,0);*/

 /* Enemy */

 s_LoadRoute( ctID, "Route\lev\aer02_01.rt"   ,"aer02.route.01"   );
 CreateManName( mctID, "Peop.Attr.cln_f01","aer02.route.01"   , 0, "plane.m13_0" );
 s_SetCommander("plane.m13_0", "Colony");

 s_LoadRoute( ctID, "Route\lev\aer02_03.rt"   ,"aer02.route.03"   );
 CreateManName( mctID, "Peop.Attr.cln_f01","aer02.route.03"   , 0, "plane.m13_1" );
 s_SetCommander("plane.m13_1", "Colony");

 comName:="Colony";
 groupName:="C.Group.AER02.00";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.AER02.00",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l05.route.g0","Route/LEV/l05_g0.rt",0,1);
 AddRouteToMember(comName,groupName,"l05.route.g0","Route/LEV/l05_g0.rt",1,0);

 groupName:="C.Group.AER02.01";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.AER02.01",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l05.route.g1","Route/LEV/l05_g1.rt",0,1);
 AddRouteToMember(comName,groupName,"l05.route.g1","Route/LEV/l05_g1.rt",1,0);

 groupName:="C.Group.AER02.02";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.AER02.02",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l05.route.g7","Route/LEV/l05_g7.rt",0,1);
 AddRouteToMember(comName,groupName,"l05.route.g7","Route/LEV/l05_g7.rt",1,0);
}

func void SetTaxiAER02a()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2393,2580,63.75],"Taxi.Attr.cln_f04");
}

func void SetTaxiAER02b()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2672,3176,77],"Taxi.Attr.cln_f03");
}

func void SetTaxiAER02c()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2512,3236,76.05],"Taxi.Attr.cln_f01");
}

func void main()
{
 SetTaxiAER02a();
 SetTaxiAER02b();
 SetTaxiAER02c();
 CreateGroupsAER02();
}

// --------------------------- End of Mission AER02 ---------------------------
