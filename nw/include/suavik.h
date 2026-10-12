class	CViewScene;
class	CViewDynamicList;

void SUA_InitEverything();
void SUA_DeinitEverything();
void PIN_InitEverything();
void PIN_DeInitEverything();
void SUA_BeginRender(CViewScene *,CViewDynamicList &);
void SUA_EndRender(CViewScene *);
void SUA_ProcessEvents();
void SUA_SkipTime( double t );

