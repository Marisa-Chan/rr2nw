// ------------------------ Units creation functions -------------------------
func void CreateMortarFirst(str szCom,str szGroup,str szUnit,str szRoad,str szFile)
{CreateGroup(szGroup,szCom);
 CreateUnit(szUnit,szGroup,"Tank.MortarFirst.Attr");
 AddRouteToMember(szCom,szGroup,szRoad,szFile,0,1);
 AddRouteToMember(szCom,szGroup,szRoad,szFile,1,0);
}

func void CeateMortarSecond(str szCom,str szGroup,str szUnit,str szRoad,str szFile)
{CreateGroup(szGroup,szCom);
 CreateUnit(szUnit,szGroup,"Tank.MortarSecond.Attr");
 AddRouteToMember(szCom,szGroup,szRoad,szFile,0,1);
 AddRouteToMember(szCom,szGroup,szRoad,szFile,1,0);
}

func void CreateBallSenderFirst(str szCom,str szGroup,str szUnit,str szRoad,str szFile)
{CreateGroup(szGroup,szCom);
 CreateUnit(szUnit,szGroup,"Tank.BallSenderFirst.Attr");
 AddRouteToMember(szCom,szGroup,szRoad,szFile,0,1);
 AddRouteToMember(szCom,szGroup,szRoad,szFile,1,0);
}

func void CreateBallSenderSecond(str szCom,str szGroup,str szUnit,str szRoad,str szFile)
{CreateGroup(szGroup,szCom);
 CreateUnit(szUnit,szGroup,"Tank.BallSenderSecond.Attr");
 AddRouteToMember(szCom,szGroup,szRoad,szFile,0,1);
 AddRouteToMember(szCom,szGroup,szRoad,szFile,1,0);
}

func void CreateKingTank(str szGroup,str szUnit,str szGRoad,str szGFile,
                         str szPRoad,str szPFile)
 var str szCom;
{szCom:="Kingdom";
 CreateGroup(szGroup,szCom);
 CreateUnit(szUnit,szGroup,"Tank.King.Attr");
 AddRouteToMember(szCom,szGroup,szGRoad,szGFile,0,1);
 AddRouteToMember(szCom,szGroup,szPRoad,szPFile,1,0);
}

/*
func void CreateKingYoungBattleBeast(str szGroup,str szUnit,str szRoad,str szFile,int nDelay)
 var int nRouteCTID,nPeopleCTID;
{nRouteCTID:=s_SearchSeanceClassTable("Route");
 nPeopleCTID:=s_SearchSeanceClassTable("People");
 s_LoadRoute(nRouteCTID,szFile,szRoad);
 CreateManName(nPeopleCTID,"Peop.Attr.anl_00",szRoad,nDelay,szUnit);
 s_SetCommander(szUnit,"Kingdom");
}
*/

func void CreateKingYoungTank(str szGroup,str szUnit,str szGRoad,str szGFile,
                              str szPRoad,str szPFile)
 var str szCom;
{szCom:="Kingdom";
 CreateGroup(szGroup,szCom);
 CreateUnit(szUnit,szGroup,"Tank.KingYoung.Attr");
 AddRouteToMember(szCom,szGroup,szGRoad,szGFile,0,1);
 AddRouteToMember(szCom,szGroup,szPRoad,szPFile,1,0);
}

func void CreateMagicTank(str szGroup,str szUnit,str szGRoad,str szGFile,
                          str szPRoad,str szPFile)
 var str szCom;
{szCom:="Magician";
 CreateGroup(szGroup,szCom);
 CreateUnit(szUnit,szGroup,"Tank.Magic.Attr");
 AddRouteToMember(szCom,szGroup,szGRoad,szGFile,0,1);
 AddRouteToMember(szCom,szGroup,szPRoad,szPFile,1,0);
}

/*
func void CreateMagicYoungBattleBeast(str szGroup,str szUnit,str szRoad,str szFile,int nDelay)
 var int nRouteCTID,nPeopleCTID;
{nRouteCTID:=s_SearchSeanceClassTable("Route");
 nPeopleCTID:=s_SearchSeanceClassTable("People");
 s_LoadRoute(nRouteCTID,szFile,szRoad);
 CreateManName(nPeopleCTID,"Peop.Attr.anl_01",szRoad,nDelay,szUnit);
 s_SetCommander(szUnit,"Magician");
}
*/

func void CreateMagicYoungTank(str szGroup,str szUnit,str szGRoad,str szGFile,
                              str szPRoad,str szPFile)
 var str szCom;
{szCom:="Magician";
 CreateGroup(szGroup,szCom);
 CreateUnit(szUnit,szGroup,"Tank.MagicYoung.Attr");
 AddRouteToMember(szCom,szGroup,szGRoad,szGFile,0,1);
 AddRouteToMember(szCom,szGroup,szPRoad,szPFile,1,0);
}

func void CreateKingAirplane(str szGroup,str szUnit,str szRoad,str szFile,int nDelay)
 var int nRouteCTID,nPeopleCTID;
{nRouteCTID:=s_SearchSeanceClassTable("Route");
 nPeopleCTID:=s_SearchSeanceClassTable("People");
 s_LoadRoute(nRouteCTID,szFile,szRoad);
 CreateManName(nPeopleCTID,"Peop.Attr.anl_02",szRoad,nDelay,szUnit);
 s_SetCommander(szUnit,"Kingdom");
}

func void CreateMagicAirplane(str szGroup,str szUnit,str szRoad,str szFile,int nDelay)
 var int nRouteCTID,nPeopleCTID;
{nRouteCTID:=s_SearchSeanceClassTable("Route");
 nPeopleCTID:=s_SearchSeanceClassTable("People");
 s_LoadRoute(nRouteCTID,szFile,szRoad);
 CreateManName(nPeopleCTID,"Peop.Attr.vhl_hel",szRoad,nDelay,szUnit);
 s_SetCommander(szUnit,"Magician");
}

func void CreateKingKnight(str szGroup,str szUnit,str szRoad,str szFile,int nDelay)
 var int nRouteCTID,nPeopleCTID;
{nRouteCTID:=s_SearchSeanceClassTable("Route");
 nPeopleCTID:=s_SearchSeanceClassTable("People");
 s_LoadRoute(nRouteCTID,szFile,szRoad);
 CreateManName(nPeopleCTID,"Peop.Attr.man_knht",szRoad,nDelay,szUnit);
 s_SetCommander(szUnit,"Kingdom");
}

func void CreateMagicKnight(str szGroup,str szUnit,str szRoad,str szFile,int nDelay)
 var int nRouteCTID,nPeopleCTID;
{nRouteCTID:=s_SearchSeanceClassTable("Route");
 nPeopleCTID:=s_SearchSeanceClassTable("People");
 s_LoadRoute(nRouteCTID,szFile,szRoad);
 CreateManName(nPeopleCTID,"Peop.Attr.man_mknt",szRoad,nDelay,szUnit);
 s_SetCommander(szUnit,"Magician");
}
// --------------------- End of units creation functions ---------------------
