const int anim_UPDATE      = 0;
const int anim_MOVE         = 1;
const int anim_ROTATEOX     = 2;
const int anim_ROTATEOY     = 3;
const int anim_ROTATEOZ     = 4;
const int anim_ROTATEOXC    = 5;
const int anim_ROTATEOYC    = 6;
const int anim_ROTATEOZC    = 7;
const int anim_ROTATE       = 8;
const int anim_ROTATEC      = 9;
const int anim_LOADIDENTITY = 10;

const int EXPLOSION_MSG_BOOM = 10000;
const int com_EV_GROUP_AREA = 2001;

const int EDO_WRITE = 1;
const int EDO_READ  = 2;

const int KR_SET_ATTR = 1;
const int VECTOR_LAND = 5;
const int VECTOR3D_F  = 6;
const int VECTOR3D_I  = 7;
const int VECTOR2D_F  = 8;
const int VECTOR2D_I  = 9;
const int SETOFINT    =10;
const int RECT2D_I    =11;
const int RECT2D_F    =12;

const int NO_LAND     = 0;
const int ON_LAND     = 1;
const int ON_WATER    = 2;
const int ON_OBJECTS  = 3;

const float INFINITY_TIME = -1;

const int s_GROUP_ADD_MEMBER      extern;
const int s_GROUP_ADD_MEMBER_N    extern;
const int s_COMMANDER_ADD_MEMBER_N extern;

const int s_ATTR_MSG_SET_INT      extern;
const int s_ATTR_MSG_SET_DOUBLE   extern;
const int s_ATTR_MSG_SET_STR      extern;

const int sk_EV_LOAD              extern;
const int sk_EV_PROG              extern;
const int sp_EV_SET_PHASE_COUNT   extern;
const int sp_EV_SET_PHASE         extern;
const int fou_EVCMD_START         extern;
const int bi_EV_BEGIN_MOVE        extern;
const int pe_EVCMD_START          extern;
const int pe_EV_SETANIM           extern;
const int START_FARTING           extern;
const int lmp_EV_START            extern;
const int lmp_EV_SETENDPOS	  extern;
const int t_EV_SET_ATTR_POS       extern;
const int rc_SET_EJECT            extern;
const int com_EV_SET_ROUTE        extern;
const int ARTEFACT_MOVETO         extern;

const int LIGHT_COLOR_BLACK  = 0;
const int LIGHT_COLOR_RED    = 1;
const int LIGHT_COLOR_GREEN  = 2;
const int LIGHT_COLOR_BLUE   = 4;
const int LIGHT_COLOR_YELLOW = 3;
const int LIGHT_COLOR_VIOLET = 5;
const int LIGHT_COLOR_CYAN   = 6;
const int LIGHT_COLOR_WHITE  = 7;

func void s_SetHostileCommander(int oID0, int cachePos,int oID1, int cachePos1) extern;
func void s_SetFriendlyCommander(int oID0, int cachePos,int oID1, int cachePos1) extern;
func void s_DeleteHowitzer( str Holder) extern;


func void s_New( int ctID, str name, var int oID, var  int cachePos ) extern;
func float s_GetTime() extern;
func int  s_OpenEventData(int style) extern;
func void s_CloseEventData(int event) extern;
func void s_WriteInt     ( int event, int    i ) extern;
func void s_WriteFloat   ( int event, float  f ) extern;
func void s_WriteStr     ( int event, str    s ) extern;
func void s_WriteVector  ( int event, vector v ) extern;
func void s_WriteObjectID( int event, int oID, int cachePos ) extern;
func void s_Descend      ( int event,int type, int ver ) extern;
func void s_Ascend       ( int event ) extern;
func void s_IssueEvent( int event,
                        int label,
                        int time,
                        int oID, int cachePos ) extern;
func void s_SendEventNow(
                        int event,
                        int label,
                        int oID, int cachePos
                        ) extern;
func void s_SearchObjectID( var int id, var int cachePos, str s ) extern;
func int  s_AddClassTable ( str className, int maxTableSize ) extern;
func void s_NewObject( int classID, str name ) extern;
func void s_NewObjectN( str className, str name ) extern;
func void s_LoadRoute( int routeTable, str fileName, str routeName ) extern;

 //====================== Project table ==================================
func void s_CreateProjectTable(
                                int misProjQnty,
                                int treeNodeQnty,
                                int projectDataSize
                              ) extern;
func int  s_NewPNode( int command, int left, int right ) extern;
func void s_OpenProjectData  ( int node ) extern;
func void s_CloseProjectData ( int node ) extern;
func void s_ProjectWriteInt  ( int node, int   val ) extern;
func void s_ProjectWriteFloat( int node, float val ) extern;
func void s_ProjectWriteStr  ( int node, str   val ) extern;
func void s_ProjectNodeSetLink( int node, int left, int right ) extern;
func int  s_PNodeNULL() extern;
func void s_NewProjectEx( str pName, int node , int permanent) extern;
func void s_NewProject( str pName, int node)
{
 s_NewProjectEx( pName, node , 0);
}


func void s_UpdateAttributes() extern;
func void s_SetLevel(int level) extern;
func void s_AttachObject( int ctID, str name, int attrID, int attrCP, 
                          str nameRef, vector locPos, int eventLabel) extern;
func void s_SetViewPoint(int index, vector pos) extern;
func int  s_SearchSeanceClassTable(str tableName) extern;
func int  s_SetDamage(str objName, float damage/* 0..1 */) extern;
func int  s_SetCommander(str objName, str commander) extern;

func void s_AttachSmoker( int ctID, str name, int attrID, int attrCP, 
                          str nameRef, vector locPos) 
{
	s_AttachObject( ctID, name, attrID, attrCP, 
                        nameRef, locPos, fou_EVCMD_START);
}
func void CreateAnimSets(var int oID, var int cachePos, 
                         str skinName, int init, int count )
 var int event;
 {
    s_SearchObjectID( oID, cachePos, skinName );

    event := s_OpenEventData( EDO_WRITE );
               s_WriteStr( event, "createAniSets" );
               s_WriteInt( event, init  );
               s_WriteInt( event, count );
             s_CloseEventData( event );

    s_SendEventNow( event,
                    sk_EV_PROG,
                    oID, cachePos );
 }

func void CreateAnimSetsAuto(var int oID, var int cachePos, 
                         str skinName, int init, int count, int progLen )
 var int event;
 {
    s_SearchObjectID( oID, cachePos, skinName );

    event := s_OpenEventData( EDO_WRITE );
               s_WriteStr( event, "createAniSetsAuto" );
               s_WriteInt( event, init  );
               s_WriteInt( event, count );
               s_WriteInt( event, progLen );
             s_CloseEventData( event );

    s_SendEventNow( event,
                    sk_EV_PROG,
                    oID, cachePos );
 }

func void WriteVector(int event, vector v)
{
   s_Descend( event, VECTOR3D_F, 0 );
     s_WriteFloat( event, v.x );
     s_WriteFloat( event, v.y );
     s_WriteFloat( event, v.z );
   s_Ascend ( event );
}

func void skin_SetAnimProg_UPDATE(int oID, int cachePos,int blkNum)
 var int event;
{
    event := s_OpenEventData( EDO_WRITE );
               s_WriteInt( event, blkNum );
               s_WriteInt( event, anim_UPDATE );
             s_CloseEventData( event );

    s_SendEventNow( event,
                    pe_EV_SETANIM,
                    oID, cachePos );

}

func void skin_SetAnimProg_LOADIDENTITY(int oID, int cachePos,int blkNum)
 var int event;
{
    event := s_OpenEventData( EDO_WRITE );
               s_WriteInt( event, blkNum );
               s_WriteInt( event, anim_LOADIDENTITY );
             s_CloseEventData( event );

    s_SendEventNow( event,
                    pe_EV_SETANIM,
                    oID, cachePos );

}

func void skin_SetAnimProg_MOVE(int oID, int cachePos,int blkNum, 
                                vector axis, vector dir, float A, float w, float F)
 var int event;
{
    event := s_OpenEventData( EDO_WRITE );
               s_WriteInt( event, blkNum );
               s_WriteInt( event, anim_MOVE );

               WriteVector  ( event, axis );
               WriteVector  ( event, dir );
               s_WriteFloat ( event, A );
               s_WriteFloat ( event, w );
               s_WriteFloat ( event, F );
             s_CloseEventData( event );

    s_SendEventNow( event,
                    pe_EV_SETANIM,
                    oID, cachePos );

}

func void skin_SetAnimProgReadDataRot(int oID, int cachePos,int blkNum, int com,
                                vector axis, float w, float F)
 var int event;
{
    event := s_OpenEventData( EDO_WRITE );
               s_WriteInt( event, blkNum );
               s_WriteInt( event, com );

               WriteVector  ( event, axis );
               s_WriteFloat ( event, w );
               s_WriteFloat ( event, F );
             s_CloseEventData( event );

    s_SendEventNow( event,
                    pe_EV_SETANIM,
                    oID, cachePos );
}

func void skin_SetAnimProgReadDataRotC(int oID, int cachePos,int blkNum, int com,
                                vector axis, float A, float w, float F)
 var int event;
{
    event := s_OpenEventData( EDO_WRITE );
               s_WriteInt( event, blkNum );
               s_WriteInt( event, com );

               WriteVector  ( event, axis );
               s_WriteFloat ( event, A );
               s_WriteFloat ( event, w );
               s_WriteFloat ( event, F );
             s_CloseEventData( event );

    s_SendEventNow( event,
                    pe_EV_SETANIM,
                    oID, cachePos );
}

func void skin_SetAnimProg_ROTATEOX(int oID, int cachePos,int blkNum, 
                                vector axis, float w, float F)
{
    skin_SetAnimProgReadDataRot(oID,cachePos,blkNum, anim_ROTATEOX, axis, w,F);
}

func void skin_SetAnimProg_ROTATEOY(int oID, int cachePos,int blkNum, 
                                vector axis, float w, float F)
{
    skin_SetAnimProgReadDataRot(oID,cachePos,blkNum, anim_ROTATEOY, axis, w,F);
}

func void skin_SetAnimProg_ROTATEOZ(int oID, int cachePos,int blkNum, 
                                vector axis, float w, float F)
{
    skin_SetAnimProgReadDataRot(oID,cachePos,blkNum, anim_ROTATEOZ, axis, w,F);
}

func void skin_SetAnimProg_ROTATEOXC(int oID, int cachePos,int blkNum, 
                                vector axis, float A,float w, float F)
{
    skin_SetAnimProgReadDataRotC(oID,cachePos,blkNum, anim_ROTATEOXC, axis,A,w,F);
}

func void skin_SetAnimProg_ROTATEOYC(int oID, int cachePos,int blkNum, 
                                vector axis, float A,float w, float F)
{
    skin_SetAnimProgReadDataRotC(oID,cachePos,blkNum, anim_ROTATEOYC, axis,A,w,F);
}

func void skin_SetAnimProg_ROTATEOZC(int oID, int cachePos,int blkNum, 
                                vector axis, float A,float w, float F)
{
    skin_SetAnimProgReadDataRotC(oID,cachePos,blkNum, anim_ROTATEOZC, axis,A,w,F);
}

func void artefact_MoveTo(int oID, int cachePos, vector v  )
 var int event;
{
    event := s_OpenEventData( EDO_WRITE );
               s_WriteFloat ( event, v.x );
               s_WriteFloat ( event, v.y );
               s_WriteFloat ( event, v.z );
             s_CloseEventData( event );

    s_SendEventNow( event,
                    ARTEFACT_MOVETO,
                    oID, cachePos );
}
