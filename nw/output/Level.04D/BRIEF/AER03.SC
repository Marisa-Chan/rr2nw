include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"

// -------------------------- Start of Mission AER03 --------------------------

func void CreateGroupsAER03()
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
 var int ctID, mctID;
{
 ctID := s_SearchSeanceClassTable("Route");
 mctID:= s_SearchSeanceClassTable("People");

 /* Confederates */

 comName:="Colony";
 groupName:="C.Group.aer03.00";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.aer03.00",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l06.route.p0","Route/LEV/l06_p0.rt",0,1);
 AddRouteToMember(comName,groupName,"l06.route.p0","Route/LEV/l06_p0.rt",1,0);

 /* Enemy */

 s_LoadRoute( ctID, "Route\lev\aer02_01.rt"   ,"aer02.route.01"   );
 CreateManName( mctID, "Peop.Attr.shiz_f0a","aer02.route.01"   , 0, "shiz.a03e0" );
 s_SetCommander("shiz.a03e0", "Actek");

 s_LoadRoute( ctID, "Route\lev\aer02_03.rt"   ,"aer02.route.03"   );
 CreateManName( mctID, "Peop.Attr.shiz_f0a","aer02.route.03"   , 0, "shiz.a03e1" );
 s_SetCommander("shiz.a03e1", "Actek");

 comName:="Actek";
 groupName:="A.Group.AER03.00";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.AER03.00",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l05.route.g0","Route/LEV/l05_g0.rt",0,1);
 AddRouteToMember(comName,groupName,"l05.route.g0","Route/LEV/l05_g0.rt",1,0);

 groupName:="A.Group.AER03.01";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.AER03.01",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l05.route.g1","Route/LEV/l05_g1.rt",0,1);
 AddRouteToMember(comName,groupName,"l05.route.g1","Route/LEV/l05_g1.rt",1,0);

 groupName:="A.Group.AER03.02";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.AER03.02",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l05.route.g7","Route/LEV/l05_g7.rt",0,1);
 AddRouteToMember(comName,groupName,"l05.route.g7","Route/LEV/l05_g7.rt",1,0);
}

func void SetTaxiAER03a()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2760,2632,74.144],"Taxi.Attr.cln_f01");
}

func void SetTaxiAER03b()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2672,3176,77],"Taxi.Attr.cln_f05");
}

func void SetTaxiAER03c()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2512,3236,76.05],"Taxi.Attr.cln_f01");
}

func void main()
{
 SetTaxiAER03a();
 SetTaxiAER03b();
 SetTaxiAER03c();
 CreateGroupsAER03();
}

// --------------------------- End of Mission AER03 ---------------------------
