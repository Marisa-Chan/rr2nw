include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"

// -------------------------- Start of Mission AER00 --------------------------

func void CreateGroupsAER00() // own
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
 var int ctID, mctID;
{
 ctID := s_SearchSeanceClassTable("Route");
 mctID:= s_SearchSeanceClassTable("People");

 /* Confederates */

 s_LoadRoute( ctID, "Route\lev\aer00_02.rt", "aer00.route.02" );
 CreateManName( mctID, "Peop.Attr.shiz_f0a","aer00.route.02"   , 0, "shiz.a00e0" );
 s_SetCommander("shiz.a00e0", "Actek");

 /*comName:="Colony";
 groupName:="C.Group.l15.00";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l15.00",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l00.route.g6","Route/LEV/l00_g6.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g6","Route/LEV/l00_g6.rt",1,0);*/

 /* Enemy */

 s_LoadRoute( ctID, "Route\lev\aer00_00.rt"   ,"aer00.route.00"   );
 CreateManName( mctID, "Peop.Attr.cln_f01","aer00.route.00"   , 0, "plane.m13_0" );
 s_SetCommander("plane.m13_0", "Colony");

 s_LoadRoute( ctID, "Route\lev\aer00_01.rt"   ,"aer00.route.01"   );
 CreateManName( mctID, "Peop.Attr.cln_f01","aer00.route.01"   , 0, "plane.m13_1" );
 s_SetCommander("plane.m13_1", "Colony");

 comName:="Colony";
 groupName:="C.Group.aer00.00";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.aer00.00",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l00.route.g4","Route/LEV/l00_g4.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g4","Route/LEV/l00_g4.rt",1,0);

 groupName:="C.Group.aer00.01";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.aer00.01",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l00.route.g5","Route/LEV/l00_g5.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g5","Route/LEV/l00_g5.rt",1,0);
}

func void SetTaxiAER00a()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2393,2580,63.75],"Taxi.Attr.cln_f04");
}

/*func void SetTaxiAER00b()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2371,2576,67.15],"Taxi.Attr.cln_f04");
}*/

func void SetTaxiAER00c()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2876,2382,59.8],"Taxi.Attr.cln_f05");
}

func void SetTaxiAER00d()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2896,2382,59.8],"Taxi.Attr.cln_f01");
}

func void SetTaxiAER00e()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2876,2362,59.8],"Taxi.Attr.cln_f01");
}

func void main()
{
 SetTaxiAER00a();
 //SetTaxiAER00b();
 SetTaxiAER00c();
 SetTaxiAER00d();
 SetTaxiAER00e();
 CreateGroupsAER00();
}

// --------------------------- End of Mission AER00 ---------------------------
