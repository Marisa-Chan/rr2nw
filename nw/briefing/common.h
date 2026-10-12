#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


//#define MEM_CHECK  // надо проверять динамическую память

//#define NDEBUG     // нужны assert() - снять
#include <assert.h>


#define MAXPATH       128
#define MAX_RES       4096
#define MAX_PLANES    4
#define MAX_PAGES     2000
#define MAX_POINTS    4
#define PI            3.1415926535846
#define FILE_SIGN     "VIA FRPACK"
#define FILE_SIGN_LEN 10

#define MAX(x, y)  (x) < (y) ? (y) : (x)
#define MIN(x, y)  (x) > (y) ? (y) : (x)
#define SIGN(x)    ((x) > 0) ? 1 : ((x) == 0) ? 0 : -1
//#define SQR(x)     ((x)*(x))

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef int BOOL;

typedef enum{
   SORT_DECREASE,
   SORT_INCREASE,
}TSortDir;

inline void Swap(int &a, int &b){
  int tmp = a;

  a = b;
  b = tmp;
}


#endif //__COMMON_H__
