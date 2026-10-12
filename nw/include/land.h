#ifndef __LAND_H__

#define __LAND_H__

byte *GetLandHeightMap(int nReduction);
void ShowLandFragment(TCCRect2 &rect,CLandObjectMap2 &map2);
void SetLandParams(double fCellSize,double fHRatio,int planeNum=0);
void SetLandViewPoint(void);
void LoadLand(void);
void UnloadLand(void);
bool MatchLandColorsFile(char *name);

#if 0	// Use KHAMIDULIN
	class CViewTerrain {
	public:
		CViewTerrain(double fCellSize,double fHScale,double fViewDiameter) {
			(void)fViewDiameter;
			LoadLand();
			SetLandParams(fCellSize,fHScale);
		}
		~CViewTerrain() {
			UnloadLand();
		}
		void SetViewPoint() {
			SetLandViewPoint();
		}
		void Draw(TCCRect2 &rect,CLandObjectMap2 &map) {
			ShowLandFragment(rect,map);
		}
		void EndDrawTerrain() {}
		TCbyte *GetHeightMap() { return GetLandHeightMap(0); }
		void SetReductionStart(double fDist) /* in cells */ { (void)fDist; }
	};
#else	// Dont use KHAMIDULIN
	#define	CViewTerrain	_CViewTerrain
#endif

#endif
