include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"
// -------------------------- Start of Mission AER04 --------------------------

func void CreateGroupsAER04() // own
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
 var int ctID, mctID;
{
 ctID := s_SearchSeanceClassTable("Route");
 mctID:= s_SearchSeanceClassTable("People");

 /* Confederates */

 s_LoadRoute( ctID, "Route\lev\aer04_05.rt", "aer04.route.05"   );
 CreateManName( mctID, "Peop.Attr.shiz_f0a","aer04.route.05"   , 0, "shiz.a02e0" );
 s_SetCommander("shiz.a02e0", "Actek");

 /*comName:="Colony";
 groupName:="C.Group.l15.00";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l15.00",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l00.route.g6","Route/LEV/l00_g6.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g6","Route/LEV/l00_g6.rt",1,0);*/

 /* Enemy */

 s_LoadRoute( ctID, "Route\lev\aer04_00.rt"   ,"aer04.route.00"   );
 CreateManName( mctID, "Peop.Attr.cln_f01","aer04.route.00"   , 0, "plane.a04_0" );
 s_SetCommander("plane.a04_0", "Colony");

 /*s_LoadRoute( ctID, "Route\lev\aer04_01.rt"   ,"aer04.route.01"   );
 CreateManName( mctID, "Peop.Attr.cln_f01","aer04.route.01"   , 0, "plane.a04_1" );
 s_SetCommander("plane.a04_1", "Colony");*/

 comName:="Colony";
 groupName:="C.Group.AER04.00";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.AER04.00",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"aer04.route.02","Route/LEV/aer04_02.rt",0,1);
 AddRouteToMember(comName,groupName,"aer04.route.02","Route/LEV/aer04_02.rt",1,0);

 groupName:="C.Group.AER04.01";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.AER04.01",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"aer04.route.03","Route/LEV/aer04_03.rt",0,1);
 AddRouteToMember(comName,groupName,"aer04.route.03","Route/LEV/aer04_03.rt",1,0);

 groupName:="C.Group.AER04.02";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.AER04.02",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"aer04.route.04","Route/LEV/aer04_04.rt",0,1);
 AddRouteToMember(comName,groupName,"aer04.route.04","Route/LEV/aer04_04.rt",1,0);
}

func void SetTaxiAER04()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2393,2580,63.75],"Taxi.Attr.cln_f04");

 CreateTaxiName(ctID,[2363, 3646, 51],"Taxi.Attr.cln_f01", "Taxia0400");
 CreateTaxiName(ctID,[2380, 3646, 55],"Taxi.Attr.cln_f01", "Taxia0401");
 CreateTaxiName(ctID,[2363, 3626, 51],"Taxi.Attr.cln_f01", "Taxia0402");
 CreateTaxiName(ctID,[2380, 3626, 55],"Taxi.Attr.cln_f01", "Taxia0403");

 s_SetDamage("Taxia0400", 0.8);
 s_SetDamage("Taxia0401", 0.8);
 s_SetDamage("Taxia0402", 0.8);
 s_SetDamage("Taxia0403", 0.8);
}

func void main()
{
 SetTaxiAER04();
 CreateGroupsAER04();
}

// --------------------------- End of Mission AER04 ---------------------------
