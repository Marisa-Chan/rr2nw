#include "windows.h"

int CheckExistingRegistry()
{  char *strKey = "SOFTWARE\\LOGOS\\RR2";
   HKEY hSettings;

    /*
     * Initialize the global variables and allow the sample code to override
     * some of these default settings.
     */

	// Time to have fun with the registry!

	// First, open the registry entry
   if (RegOpenKeyEx(HKEY_CURRENT_USER, strKey, 0, KEY_ALL_ACCESS, &hSettings) != ERROR_SUCCESS)
      return 0;

   RegCloseKey(hSettings);
   return 1;

}

void SetChooseRegistry(int driver, int fullScreen, int mode)
{  char* strKey = "SOFTWARE\\LOGOS\\RR2\\Choose";
   HKEY hSettings;
   DWORD dwDisposition;
    /*
     * Initialize the global variables and allow the sample code to override
     * some of these default settings.
     */

	// Time to have fun with the registry!

	// First, open the registry entry
   RegCreateKeyEx(HKEY_CURRENT_USER, strKey, 0, NULL, REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS, NULL, &hSettings, &dwDisposition);

   // See if the key is existing or not...
   //if (dwDisposition == REG_CREATED_NEW_KEY) {


      RegSetValueEx(hSettings, (LPCSTR) "3DCard", 0, REG_DWORD,
                      (const unsigned char *) &driver, sizeof(int));
      RegSetValueEx(hSettings, (LPCSTR) "FullScreen", 0, REG_DWORD,
                      (const unsigned char *) &fullScreen, sizeof(int));
      RegSetValueEx(hSettings, (LPCSTR) "Mode", 0, REG_DWORD,
                      (const unsigned char *) &mode, sizeof(int));

   RegCloseKey(hSettings);
}

int GetChooseRegistry(int *driver, int *fullScreen, int *mode)
{  char *strKey = "SOFTWARE\\LOGOS\\RR2\\Choose";
   HKEY hSettings;
   unsigned long dwKeySize;

    /*
     * Initialize the global variables and allow the sample code to override
     * some of these default settings.
     */

	// Time to have fun with the registry!

	// First, open the registry entry
   if (RegOpenKeyEx(HKEY_CURRENT_USER, strKey, 0, KEY_ALL_ACCESS, &hSettings) != ERROR_SUCCESS)
      return 0;


   // The key exists, load the values stored in it.

   dwKeySize = sizeof(int);
   RegQueryValueEx(hSettings, (LPCSTR) "3DCard", 0, 0,
            (unsigned char *) driver, &dwKeySize);

   RegQueryValueEx(hSettings, (LPCSTR) "FullScreen", 0, 0,
            (unsigned char *) fullScreen, &dwKeySize);

   RegQueryValueEx(hSettings, (LPCSTR) "Mode", 0, 0,
            (unsigned char *) mode, &dwKeySize);

   RegCloseKey(hSettings);

   return 1;
}

void SetLastWorkRegistry(int driver, int fullScreen, int mode)
{  char* strKey = "SOFTWARE\\LOGOS\\RR2\\LastWork";
   HKEY hSettings;
   DWORD dwDisposition;
    /*
     * Initialize the global variables and allow the sample code to override
     * some of these default settings.
     */

	// Time to have fun with the registry!

	// First, open the registry entry
   RegCreateKeyEx(HKEY_CURRENT_USER, strKey, 0, NULL, REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS, NULL, &hSettings, &dwDisposition);

   // See if the key is existing or not...
   //if (dwDisposition == REG_CREATED_NEW_KEY) {


      RegSetValueEx(hSettings, (LPCSTR) "3DCard", 0, REG_DWORD,
                      (const unsigned char *) &driver, sizeof(int));
      RegSetValueEx(hSettings, (LPCSTR) "FullScreen", 0, REG_DWORD,
                      (const unsigned char *) &fullScreen, sizeof(int));
      RegSetValueEx(hSettings, (LPCSTR) "Mode", 0, REG_DWORD,
                      (const unsigned char *) &mode, sizeof(int));

   RegCloseKey(hSettings);
}

int GetLastWorkRegistry(int *driver, int *fullScreen, int *mode)
{  char *strKey = "SOFTWARE\\LOGOS\\RR2\\LastWork";
   HKEY hSettings;
   unsigned long dwKeySize;

    /*
     * Initialize the global variables and allow the sample code to override
     * some of these default settings.
     */

	// Time to have fun with the registry!

	// First, open the registry entry
   if (RegOpenKeyEx(HKEY_CURRENT_USER, strKey, 0, KEY_ALL_ACCESS, &hSettings) != ERROR_SUCCESS)
      return 0;


   // The key exists, load the values stored in it.

   dwKeySize = sizeof(int);
   RegQueryValueEx(hSettings, (LPCSTR) "3DCard", 0, 0,
            (unsigned char *) driver, &dwKeySize);

   RegQueryValueEx(hSettings, (LPCSTR) "FullScreen", 0, 0,
            (unsigned char *) fullScreen, &dwKeySize);

   RegQueryValueEx(hSettings, (LPCSTR) "Mode", 0, 0,
            (unsigned char *) mode, &dwKeySize);

   RegCloseKey(hSettings);

   return 1;
}
