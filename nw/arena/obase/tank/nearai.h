#ifndef __nearAI
#define __nearAI
/*
	Coded by Green.
	Describe land based dynamic objects moving AI using local view region.
	BEGIN: 08/05/98
 */

#define MAX_DIRECTIONS 16 // max direction tank can move (must be 17, add +1 in array alloc) 

class Tank;

void Move(Tank &t, double time);
void InitNearAI(Tank &t);
void ShutdownNearAI(Tank &t);
void SetupDirections(Tank &t);
int LoadLandSprite(const char *fName);
void DrawLandSprite();

#endif //__nearAI
