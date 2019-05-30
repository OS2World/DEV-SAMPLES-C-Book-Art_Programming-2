#define INCL_DOSMEMMGR

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include "dynduo.h"

INT main ( VOID )
{
   PCHAR    pchShare ;
   APIRET   arReturn ;

   arReturn = DosAllocSharedMem((PVOID) &pchShare,
                                SHAREMEM_NAME,
                                1024,
                                PAG_READ |
                                PAG_WRITE |
                                PAG_COMMIT ) ;

   if ( arReturn == 0 ) {
      strcpy ( pchShare, "Holy Toledo, Batman" ) ;
      getchar ( ) ;
      DosFreeMem ( pchShare ) ;
   } /* endif */

   return 0 ;
}
