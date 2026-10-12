#include "strequ.h"

 /*========================================================================*/
 /*
  * Возвращает 1 при равенстве строк
  */
int str_StrEQU( const char *str0, const char *str1 )
 {
    register const char *s0, *s1;

    for( s0 = str0, s1 = str1 ;
         *s0 != 0  &&  *s1 != 0  &&  *s0 == *s1 ;
         ++s0,++s1);

    return *s0 == *s1;
 }
