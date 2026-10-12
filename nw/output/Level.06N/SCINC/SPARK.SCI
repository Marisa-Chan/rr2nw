func void SetPhase( int oID, int cachePos,
                    int phNum, int x, int y, int w, int h, float time,
                    int brightness, int color, float radius )
 var int event;
 {
     event := s_OpenEventData( EDO_WRITE );
                  s_WriteInt( event, phNum );
                  s_Descend( event, RECT2D_I, 0 );
                      s_WriteInt( event, x-w );
                      s_WriteInt( event, y-h );
                      s_WriteInt( event, x+w );
                      s_WriteInt( event, y+h );
                  s_Ascend( event );
                  s_WriteFloat( event, time );
                  s_WriteInt  ( event, brightness );
                  s_WriteInt  ( event, color );
                  s_WriteFloat( event, radius );
              s_CloseEventData( event );
    s_SendEventNow( event,
                    sp_EV_SET_PHASE,
                    oID, cachePos );
 }

func void SetSpark0()
 var int oID,cachePos, event;
 {
    s_SearchObjectID( oID, cachePos, "Spark.Flash" );
    event := s_OpenEventData( EDO_WRITE );
               s_WriteInt( event, 6 );
             s_CloseEventData( event );

    s_SendEventNow( event,
                    sp_EV_SET_PHASE_COUNT,
                    oID, cachePos );

    SetPhase( oID, cachePos, 0, 25, 45, 20,40 , 0.04, 100, LIGHT_COLOR_YELLOW, 7  );
    SetPhase( oID, cachePos, 1, 78, 46, 20,40 , 0.03, 200, LIGHT_COLOR_YELLOW, 14 );
    SetPhase( oID, cachePos, 2,126, 47, 20,40 , 0.03, 100, LIGHT_COLOR_YELLOW, 10 );
    SetPhase( oID, cachePos, 3,180, 44, 20,40 , 0.1,   20, LIGHT_COLOR_YELLOW, 7 );
    SetPhase( oID, cachePos, 4,230, 44, 20,40 , 0.1,    0, LIGHT_COLOR_YELLOW, 4 );
    SetPhase( oID, cachePos, 5,230, 44, 20,40 , 0.0,    0, LIGHT_COLOR_YELLOW, 0 );
 }
