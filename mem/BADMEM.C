#define INCL_DOSMEMMGR

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>

INT main ( VOID )
{
   PBYTE          pbBuffer ;
   APIRET         arReturn ;
   USHORT         usIndex ;

   arReturn = DosAllocMem (( PPVOID ) &pbBuffer,
                             3000,
                             PAG_READ | PAG_WRITE | PAG_COMMIT ) ;

   if ( arReturn == 0 ) {

      for ( usIndex = 0 ; usIndex < 4097 ; usIndex++ ) {

          printf ( "\nNow Writing to %p ( index = %d ) ",
                 &pbBuffer [usIndex] ,
                 usIndex ) ;

          pbBuffer [usIndex] = 1 ;

      } /* endfor */

   } /* endif */

   return 0 ;
}
