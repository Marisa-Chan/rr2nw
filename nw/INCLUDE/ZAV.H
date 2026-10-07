#ifndef	__ZAV_H__
#define	__ZAV_H__

int   ZAV_InitGraph(HINSTANCE hInst);
int   ZAV_InitLevel(const char *dir);
void  ZAV_DeInitLevel();
void  ZAV_BeginLoop();
void ZAV_BeginRenderFrame(CViewDynamicList &list);
void ZAV_RenderFrame(TCSFMatrix3x4 *pDir,CViewDynamicList &list);
void ZAV_EndRenderFrame();
void	ZAV_NextFrame();
void	ZAV_SaveScreenshot();
CConfigFile	&ZAV_Config();
CConfigFile	&GLOBAL_Config();
void ZAV_ProcessKey(byte key);
CVesselPack &ZAV_Vessels();
CViewScene *ZAV_Scene();
void ZAV_Deinit();
void ZAV_PrintFrameInfo();
void ZAV_PrintOverallInfo();


extern char g_HDDir[260];
extern char g_CDDir[260];

#endif	// __ZAV_H__
