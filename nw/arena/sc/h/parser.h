#ifndef __PARSER_H__
#define __PARSER_H__

#ifndef __TR_DATA_H__
#include "tr_data.h"
#endif

TTree *p_Programm( TTreeHeap  *heap,
                   TScanner   *scanner,
                   TNameArray *nameDescr,
                   TTree     **treeHead,
                   TNameDefs  *ndefs );


#endif

/*  End of PARSER.H */
