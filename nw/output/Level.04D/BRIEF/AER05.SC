include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"
// -------------------------- Start of Mission AER05 --------------------------

func void CreateGroupsAER05() // own
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
 var int ctID, mctID;
{
 ctID := s_SearchSeanceClassTable("Route");
 mctID:= s_SearchSeanceClassTable("People");

 /* Confederates */

 s_LoadRoute( ctID, "Route\lev\aer05_00.rt", "aer05.route.00"   );
 CreateManName( mctID, "Peop.Attr.shiz_f0a","aer05.route.00", 0, "shiz.a05_2" );
 s_SetCommander("shiz.a05_2", "Colony");

 /* Enemy */

 s_LoadRoute( ctID, "Route\lev\aer04_00.rt"   ,"aer04.route.00"   );
 CreateManName( mctID, "Peop.Attr.shiz_f0a","aer04.route.00"   , 0, "shiz.a05_0" );
 s_SetCommander("shiz.a05_0", "Actek");

 s_LoadRoute( ctID, "Route\lev\aer04_01.rt"   ,"aer04.route.01"   );
 CreateManName( mctID, "Peop.Attr.shiz_f0a","aer04.route.01"   , 0, "shiz.a05_1" );
 s_SetCommander("shiz.a05_1", "Actek");

 comName:="Actek";
 groupName:="A.Group.AER05.00";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.AER05.00",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"aer04.route.02","Route/LEV/aer04_02.rt",0,1);
 AddRouteToMember(comName,groupName,"aer04.route.02","Route/LEV/aer04_02.rt",1,0);

 groupName:="A.Group.AER05.01";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.AER05.01",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"aer04.route.03","Route/LEV/aer04_03.rt",0,1);
 AddRouteToMember(comName,groupName,"aer04.route.03","Route/LEV/aer04_03.rt",1,0);

 groupName:="A.Group.AER05.02";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.AER05.02",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"aer04.route.04","Route/LEV/aer04_04.rt",0,1);
 AddRouteToMember(comName,groupName,"aer04.route.04","Route/LEV/aer04_04.rt",1,0);
}

func void SetTaxiAER05()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2760,2632,74.144],"Taxi.Attr.cln_f01");

 CreateTaxiName(ctID,[2363, 3646, 51],"Taxi.Attr.cln_f04", "Taxia0500");
 CreateTaxiName(ctID,[2380, 3646, 55],"Taxi.Attr.cln_f04", "Taxia0501");
 CreateTaxiName(ctID,[2363, 3626, 51],"Taxi.Attr.cln_f04", "Taxia0502");
 CreateTaxiName(ctID,[2380, 3626, 55],"Taxi.Attr.cln_f04", "Taxia0503");

 s_SetDamage("Taxia0500", 0.8);
 s_SetDamage("Taxia0501", 0.8);
 s_SetDamage("Taxia0502", 0.8);
 s_SetDamage("Taxia0503", 0.8);
}

func void main()
{
 SetTaxiAER05();
 CreateGroupsAER05();
}

// --------------------------- End of Mission AER05 ---------------------------
