func int ConvertColor(int r,int g,int b)
{
    if   r > 255  
    then r := 255;
    else if  r < 0 
         then r := 0;

    if   g > 255
    then g := 255;
    else if   g < 0 
         then g := 0;

    if   b > 255 
    then b := 255;
    else if   b < 0 
         then b := 0;
    return r*65536+g*256+b;
}


func void New( int ctID, str name, var int oID, var int cachePos )
{
   s_NewObject(ctID, name);
   s_SearchObjectID( oID, cachePos, name );
}

 //=======================================================================
func void AddMemberToGroup( int grID, int grCachePos, str objectName)
 var int event;
 {
    event := s_OpenEventData( EDO_WRITE );
               s_WriteStr( event, objectName );
             s_CloseEventData( event );

    s_SendEventNow( event, s_GROUP_ADD_MEMBER_N, grID, grCachePos );
 }

 //=======================================================================
func void AddMemberToCommander( int comID, int comCachePos, str groupName )
 var int event;
 {
    event := s_OpenEventData( EDO_WRITE );
               s_WriteStr( event, groupName );
             s_CloseEventData( event );

    s_SendEventNow( event, s_COMMANDER_ADD_MEMBER_N, comID, comCachePos );
 }

 //=======================================================================
func void SetAttribute_f(
                         int oID, int cachePos,  // Attribute object id.
                         str   attrName,         // variable name
                         float val               // variable value
                        )
 var int event;
 {
    event := s_OpenEventData(EDO_WRITE);
             s_WriteStr  ( event, attrName );
             s_WriteFloat( event, val      );
             s_CloseEventData( event );
    s_SendEventNow( event,
                  s_ATTR_MSG_SET_DOUBLE,
                  oID, cachePos );
 }

 //=======================================================================
func void SetAttribute_i( int oID, int cachePos, str attrName, int val )
 var int event;
 {
    event := s_OpenEventData(EDO_WRITE);
             s_WriteStr  ( event, attrName );
             s_WriteInt  ( event, val      );
             s_CloseEventData( event );
    s_SendEventNow( event,
                  s_ATTR_MSG_SET_INT,
                  oID, cachePos );
 }

 //=======================================================================
func void SetAttribute_s( int oID, int cachePos, str attrName, str val )
 var int event;
 {
    event := s_OpenEventData(EDO_WRITE);
             s_WriteStr  ( event, attrName );
             s_WriteStr  ( event, val      );
             s_CloseEventData( event );
    s_SendEventNow( event,
                  s_ATTR_MSG_SET_STR,
                  oID, cachePos );
 }

 //=======================================================================
func void ChangeObjectAttr( int attrID, int attrCachePos, str objectName )
 var int event, oID, cachePos;
 {
    event := s_OpenEventData(EDO_WRITE);
             s_WriteObjectID ( event, attrID, attrCachePos );
             s_CloseEventData( event );

    s_SearchObjectID( oID, cachePos, objectName );
    s_SendEventNow( event,
                  KR_SET_ATTR,
                  oID, cachePos );
 }

 //=======================================================================
func void ChangeObjectAttrN( str attrName, str objectName )
 var int event, attrID, attrCachePos,
         oID, cachePos;
 {
    s_SearchObjectID( attrID, attrCachePos, attrName );

    event := s_OpenEventData(EDO_WRITE);
             s_WriteObjectID ( event, attrID, attrCachePos );
             s_CloseEventData( event );

    s_SearchObjectID( oID, cachePos, objectName );
    s_SendEventNow( event,
                  KR_SET_ATTR,
                  oID, cachePos );
 }


 //=======================================================================
func void LoadSkin( int classTableID, str fileName, str skinName )
 var int event, oID, cachePos;
 {
    s_NewObject( classTableID, skinName );
    s_SearchObjectID( oID, cachePos, skinName );

    event := s_OpenEventData( EDO_WRITE );
               s_WriteStr( event, fileName );
             s_CloseEventData( event );

    s_SendEventNow( event,
                    sk_EV_LOAD,
                    oID, cachePos );

 }

 //=======================================================================
func void SetAnimateBlock( int oID, int cachePos, int as, int pos, str name )
 var int event;
 {
    event := s_OpenEventData( EDO_WRITE );

    if   as=0  
    then     s_WriteStr( event, "set0" );
    else     s_WriteStr( event, "set" );
             s_WriteInt( event, pos  );
             s_WriteStr( event, name  );
             s_CloseEventData( event );

    s_SendEventNow( event,
                    sk_EV_PROG,
                    oID, cachePos );
 }
