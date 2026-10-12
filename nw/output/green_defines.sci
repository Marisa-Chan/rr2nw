const int s_Menu_EXIT			extern;
const int s_Menu_SURRENDER		extern;
const int s_Menu_MISSIONOK		extern;
const int s_lev_LOAD			extern;
const int s_lev_SAVE			extern;

const int s_lev_LOAD_SLOT0		extern;
const int s_lev_LOAD_SLOT1		extern;
const int s_lev_LOAD_SLOT2		extern;
const int s_lev_LOAD_SLOT3		extern;
const int s_lev_LOAD_SLOT4		extern;
const int s_lev_LOAD_SLOT5		extern;
const int s_lev_LOAD_SLOT6		extern;
const int s_lev_LOAD_SLOT7		extern;

const int s_lev_SAVE_SLOT0		extern;
const int s_lev_SAVE_SLOT1		extern;
const int s_lev_SAVE_SLOT2		extern;
const int s_lev_SAVE_SLOT3		extern;
const int s_lev_SAVE_SLOT4		extern;
const int s_lev_SAVE_SLOT5		extern;
const int s_lev_SAVE_SLOT6		extern;
const int s_lev_SAVE_SLOT7		extern;

const int s_lev_RESTART			extern;


func void s_SetCTRL( str actName, str keyName ) extern;
func void s_LinkCTRL() extern;
func void s_SetHardware(int k, int m, int j) extern;
func void s_EnableHardware(int k, int m, int j) extern;
func void s_LoadImage(str fName) extern;
func void s_LoadFont(str fName,str oName,) extern;
func int  s_CreateTransparetColor(int r, int g, int b) extern; 
func void s_AddMenuText(str path, str name, int id, int cp, int el, int pVertSpace, 
                        int sideW, int pColor, int pOpasity, int abColor, int abOpasity, 
						int afColor, int afOpasity, 
						str pFontName, str aFontName, str l0Text, str l1Text) extern;

func void s_AddMenuInput(str path, str name, int id, int cp, int el, int pVertSpace, 
                        int sideW, int pColor, int pOpasity, int abColor, int abOpasity, 
						int afColor, int afOpasity, 
						str pFontName, str aFontName, str defText) extern;

func void s_AddMenuScroll(str path, str name, int id, int cp, int el, int pVertSpace, 
                        int sideW, int pColor, int pOpasity, int abColor, int abOpasity, 
						int afColor, int afOpasity, str pFontName, str aFontName, str attrName, str l0Text, str l1Text,
						float min, float max, float step, float def) extern;
func void s_AddMenuList(str path, str name, int id, int cp, int el, int pVertSpace, 
                        int sideW, int pColor, int pOpasity, int abColor, int abOpasity, 
						int afColor, int afOpasity, 
						str pFontName, str aFontName, str attrName, str l0Text, str l1Text, int immediate) extern;
func void s_AddMenuListItem(str path, str s) extern;

func void s_AddMenuSetup(str path, str name, int id, int cp, int el, int pVertSpace, 
                        int sideW, int pColor, int pOpasity, int abColor, int abOpasity, 
						int afColor, int afOpasity, str pFontName, str aFontName, str l0Text, str l1Text) extern;
func void s_AddMenuSetupLine(str name, str ctrlName) extern;

const int s_Menu_Point0 extern;
const int s_Menu_Point1 extern;
const int s_Menu_Point2 extern;
const int s_Menu_Point3 extern;
const int s_Menu_Point4 extern;
const int s_Menu_Point5 extern;
const int s_Menu_Point6 extern;
const int s_Menu_Point7 extern;
const int s_Menu_Point8 extern;
const int s_MenuSetLocale extern;
const int s_ChangeMouseParams extern;
const int s_ChangeJoystickParams extern;
const int s_CalibrateJoystick extern;