
func void AddBird( int ctID, int attrID, int attrCP, 
                   vector min, vector max )
 var int event, oID, cachePos;
{
    s_New( ctID, "Bird.Simple", oID, cachePos );

    event := s_OpenEventData( EDO_WRITE );
             s_WriteObjectID      ( event, attrID, attrCP );
             s_Descend( event, 1, 0 );
                s_WriteFloat    ( event, min.x  );
                s_WriteFloat    ( event, min.y  );
                s_WriteFloat    ( event, min.z  );
             s_Ascend ( event );

             s_Descend( event, 2, 0 );
                s_WriteFloat    ( event, max.x  );
                s_WriteFloat    ( event, max.y  );
                s_WriteFloat    ( event, max.z  );
             s_Ascend ( event );

             s_CloseEventData( event );

    s_SendEventNow( event,
                    bi_EV_BEGIN_MOVE,
                    oID, cachePos );
}

func void main_CreateBirdAttr()
 var int ctID, oID, cachePos, oB, cpB, event;
{
    ctID := s_AddClassTable("BirdAttr",1);
    New( ctID, "Bird.Attr.0", oID, cachePos );

    SetAttribute_f (oID, cachePos, "m_speed", 2.0);
    SetAttribute_s (oID, cachePos, "m_skinName","sk.Bird.0");
    SetAttribute_f (oID, cachePos, "m_calcPosIncrement", 0.2);
}

func void CreateSomeBirs(
                           int ctID, int attrID, int attrCP, 
						   float x0, float z0, float x1, float z1, float y0, float dy,
						   int cnt
                        )
 var int     i;
 var vector  v0,v1;
{
    cnt := cnt*0.6;
    if   cnt=0   then cnt := 1;

    v0.x := x0*10;
	v0.y := y0-10;
	v0.z := -z0*10;

	v1.x := x1*10;
	v1.y := v0.y+dy;
	v1.z := -z1*10;

    for i:=0 to cnt-1 loop
	    AddBird( ctID, attrID, attrCP, v0, v1);
}
