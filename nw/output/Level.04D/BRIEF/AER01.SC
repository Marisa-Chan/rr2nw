include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"

// -------------------------- Start of Mission AER01 --------------------------

func void CreateGroupsAER01() // own
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
 var int ctID, mctID;
{
 ctID := s_SearchSeanceClassTable("Route");
 mctID:= s_SearchSeanceClassTable("People");

 /* Confederates */

 /*comName:="Colony";
 groupName:="C.Group.l15.00";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l15.00",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l00.route.g6","Route/LEV/l00_g6.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g6","Route/LEV/l00_g6.rt",1,0);*/

 /* Enemy */

 s_LoadRoute( ctID, "Route\lev\aer01_00.rt"   ,"aer01.route.00"   );
 CreateManName( mctID, "Peop.Attr.cln_f01","aer01.route.00"   , 0, "plane.m13_0" );
 s_SetCommander("plane.m13_0", "Colony");

 comName:="Colony";
 groupName:="C.Group.aer01.01";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.aer01.01",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l00.route.g5","Route/LEV/l00_g5.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g5","Route/LEV/l00_g5.rt",1,0);
}

func void SetTaxiAER01a()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2393,2580,63.75],"Taxi.Attr.cln_f04");
}

/*func void SetTaxiAER01b()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2371,2576,67.15],"Taxi.Attr.cln_f04");
}*/

/*func void SetTaxiAER01c()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2876,2382,59.8],"Taxi.Attr.cln_f05");
}

func void SetTaxiAER01d()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2896,2382,59.8],"Taxi.Attr.cln_f01");
}*/

func void SetTaxiAER01e()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2876,2362,59.8],"Taxi.Attr.cln_f01");
}

func void main()
{
 SetTaxiAER01a();
 //SetTaxiAER01b();
 //SetTaxiAER01c();
 //SetTaxiAER01d();
 SetTaxiAER01e();
 CreateGroupsAER01();
}

// --------------------------- End of Mission AER01 ---------------------------
