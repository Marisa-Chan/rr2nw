/**************************************************************************
 *
 *                            Corpse
 *
 **************************************************************************/

func void CreateCorpseAttrDefault( int ctID )
 var int oID, cachePos;
{

   New(ctID, "Corpse.Attr.Default", oID, cachePos);
   SetAttribute_s( oID, cachePos, "m_skinName","sk.corpse.default");
   SetAttribute_f( oID, cachePos, "m_fireOffsetY",2.0);
}

func void CreateCorpseAttrFinal( int ctID )
 var int oID, cachePos;
{

   New(ctID, "Corpse.Attr.Final", oID, cachePos);
   SetAttribute_s( oID, cachePos, "m_skinName","sk.corpse.final");
   SetAttribute_i( oID, cachePos, "m_isBurning",0);
   SetAttribute_i( oID, cachePos, "m_isSmoking",0);
}

func void CreateCorpseAttrFirst(int nCorpseAttrCTID)
 var int nObjectID,nCachePos;
{New(nCorpseAttrCTID,"Corpse.Attr.First",nObjectID,nCachePos);
 SetAttribute_s(nObjectID,nCachePos,"m_skinName","sk.corpse.default");
 SetAttribute_i(nObjectID,nCachePos,"m_isBurning",0);
 SetAttribute_i(nObjectID,nCachePos,"m_isSmoking",0);
}

func void CreateCorpseAttrSecond(int nCorpseAttrCTID)
 var int nObjectID,nCachePos;
{New(nCorpseAttrCTID,"Corpse.Attr.Second",nObjectID,nCachePos);
 SetAttribute_s(nObjectID,nCachePos,"m_skinName","sk.corpse.other");
 SetAttribute_i(nObjectID,nCachePos,"m_isBurning",0);
 SetAttribute_i(nObjectID,nCachePos,"m_isSmoking",0);
}

func void main_CreateCorpseAttr()
 var int   ctID;
{
   ctID := s_AddClassTable( "CorpseAttr", 4);
   CreateCorpseAttrDefault(ctID);
   CreateCorpseAttrFinal(ctID);
   CreateCorpseAttrFirst(ctID);
   CreateCorpseAttrSecond(ctID);
}

