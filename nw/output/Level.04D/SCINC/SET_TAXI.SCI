func void main_CreateTaxi()
 var int ctID;
{
/*
 Объект	Имя		Тип		Принад.		Пастбище

cln_f00	махолёт		вертолёт        колониз.	3761,2868,91.25
							3764,2807,91.25
							3705,324,163.75
							3291,3249,70

cln_f01	аэроплан	самолёт		колониз.	3701,3253,163.75
							3909,3740,73.75
							3754,3754,73.75

cln_f05	танк		ездилка		колониз.	4327,3391,42.5 /90/
							4206,3011,42.5
							2762,2648,71.25
							3389,3308,70

cln_f03	шагоход		ходилка		ацтеки		2393,2572,63.75
							1314,2591,67.5 /180/
							1412,3098,42.5
							2037,3227,67.5

cln_f04	вертолёт	вертолёт	ацтеки		2076,3166,67.5
							1671,2034,105
							1694,2034,105
							709,2589,105
							768,2589,105

cln_f08	паромобиль	ездилка		ацтеки		1235,2453,67.5
							1371,3126,42.5
							1993,3310,67.5


cln_f01 - wing.pnl
cln_f00 - /wing.pnl/
cln_f03 - /civil/
cln_f04 - helicop.pnl
cln_f05 - otank.pnl
cln_f08 - civil.pnl
*/

   ctID := s_AddClassTable("Taxi",40);

   CreateTaxi(ctID,[3761,2868,91.25],"Taxi.Attr.cln_f00");
//CreateTaxi(ctID,[2060-50,70,2390],"Taxi.Attr.cln_f00");
   CreateTaxi(ctID,[3764,2807,91.25],"Taxi.Attr.cln_f00");
   CreateTaxi(ctID,[3705,324,163.75],"Taxi.Attr.cln_f00");
   CreateTaxi(ctID,[3291,3249,70],"Taxi.Attr.cln_f00");

   CreateTaxi(ctID,[3701,3253,163.75],"Taxi.Attr.cln_f01");
   CreateTaxi(ctID,[3909,3740,73.75],"Taxi.Attr.cln_f01");
   CreateTaxi(ctID,[3754,3754,73.75],"Taxi.Attr.cln_f01");

   CreateTaxi(ctID,[4327,3391,42.5], "Taxi.Attr.cln_f05");
   CreateTaxi(ctID,[4206,3011,42.5], "Taxi.Attr.cln_f05");
   CreateTaxi(ctID,[3389,3308,70],   "Taxi.Attr.cln_f05");



//   CreateTaxi(ctID,[2393,2572,63.75],"Taxi.Attr.cln_f03");
   CreateTaxi(ctID,[1314,2591,67.5],"Taxi.Attr.cln_f03");
   CreateTaxi(ctID,[1412,3098,42.5],"Taxi.Attr.cln_f03");
   CreateTaxi(ctID,[2037,3227,67.5],"Taxi.Attr.cln_f03");

  // CreateTaxi(ctID,[2076,3166,67.5],"Taxi.Attr.cln_f04");
   CreateTaxi(ctID,[1671,2034,105] ,"Taxi.Attr.cln_f04");
   CreateTaxi(ctID,[1694,2034,105] ,"Taxi.Attr.cln_f04");
   CreateTaxi(ctID,[709,2589,105]  ,"Taxi.Attr.cln_f04");
   CreateTaxi(ctID,[768,2589,105]  ,"Taxi.Attr.cln_f04");

   CreateTaxi(ctID,[1235,2453,67.5],"Taxi.Attr.cln_f08");
   CreateTaxi(ctID,[1371,3126,42.5],"Taxi.Attr.cln_f08");
   CreateTaxi(ctID,[1993,3310,67.5],"Taxi.Attr.cln_f08");


}


