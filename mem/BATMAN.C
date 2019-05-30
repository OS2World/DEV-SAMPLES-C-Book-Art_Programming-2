#define INCL_DOSMEMMGR

#include <os2.h>
#include <stdio.h>
#include "dynduo.h"

INT main ( VOID )
{
   PBYTE    pchShare ;
   APIRET   arReturn ;

   arReturn = DosGetNamedSharedMem((PPVOID) &pchShare,
                                    SHAREMEM_NAME,
                                    PAG_READ | PAG_WRITE ) ;

   if ( arReturn == 0 ) {
      printf ( "\nString read is: \"%s\"\n", pchShare ) ;
   } /* endif */

   DosFreeMem ( pchShare ) ;
   return 0 ;
}
