#include <stdarg.h>
#include <conio.h>
#include "kernel\h\echo.h"

class CWnd{};


void createConsole( CWnd * )
 {
 }

void removeConsole()
 {
 }

CWnd *getConsole()
 {
    return NULL;
 }

// ============================================================================
void echo(
              char *format,
              ...
         )
{
  va_list l;

  va_start(l, format);
  vprintf (format, l); printf("\n");
  va_end  (l);

}
// ============================================================================
void warning(
              char *format,
              ...
            )
{
  va_list l;
  char errString[512];

  va_start(l, format);
  vsprintf(errString, format, l);
  va_end(l);

  printf("Warning: %s\n",errString);

}
