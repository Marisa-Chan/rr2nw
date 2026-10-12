#ifndef _REGISTRY_H_
#define _REGISTRY_H_
int  CheckExistingRegistry();
void SetChooseRegistry    (int driver,  int fullScreen,  int mode);
int  GetChooseRegistry    (int *driver, int *fullScreen, int *mode);
void SetLastWorkRegistry  (int driver,  int fullScreen,  int mode);
int  GetLastWorkRegistry  (int *driver, int *fullScreen, int *mode);
#endif