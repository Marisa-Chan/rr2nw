            // ================================================================
            // FUNCTIONAL AREA:   Jeneral Purpose Tools
            // NAME:              echo.h
            // AUTHORS:           MKrylov
            // DESIGN REFERENCE:  ???
            // MODIFICATION:      21 Aug 97 - creation
            // ================================================================

#ifndef _ECHO_H_
#define _ECHO_H_

#include <stdio.h>

class CWnd;

void echo(char *format, ...);
void warning(char *format, ...);
void createConsole( CWnd *parent = NULL );
void removeConsole();
CWnd *getConsole();

#endif// _ECHO_H_


