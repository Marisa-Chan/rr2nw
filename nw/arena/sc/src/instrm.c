#include "instrm.h"


 /*========================================================================*/
void is_NextCh( TScanner *scanner )
 {
    scanner->och = scanner->ch;
    scanner->ch  = scanner->text[ scanner->chptr ];

    if( scanner->ch == SY_EOF )
         return;

    if( scanner->ch == SY_EOL )
    {
         scanner->chptr += EOL_LEN;
         scanner->pos = 0;
         scanner->line++;
    }
    else
    {
         scanner->chptr++;

         if( scanner->ch == SY_TAB )
              scanner->pos += (TAB_SIZE+1) - scanner->pos % TAB_SIZE;
         else scanner->pos++;
    }
 }

 /*========================================================================*/
void is_ReadChar( TScanner *scanner )
 {
    if( scanner->ch != SY_EOF )
    {
         scanner->ch = (char)fgetc( scanner->inputFile );
         if( feof( scanner->inputFile ) )
              scanner->ch = SY_EOF;
    }
 }

 /*========================================================================*/
void is_NextChFile( TScanner *scanner )
 {
    scanner->och = scanner->ch;
    is_ReadChar( scanner );

    if( scanner->ch == SY_EOF )
         return;

    if( scanner->ch == SY_EOL )
    {
         is_ReadChar( scanner );
         scanner->ch  = SY_EOL;
         scanner->pos = 0;
         scanner->line++;
    }
    else
    {
         if( scanner->ch == SY_TAB )
              scanner->pos += (TAB_SIZE+1) - scanner->pos % TAB_SIZE;
         else scanner->pos++;
    }
 }

