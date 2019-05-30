#define INCL_DOSMEMMGR
#define INCL_DOSMISC

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>

INT main ( VOID )
{
   PBYTE apbBuf [1500] ;
   USHORT usIndex ;

   for ( usIndex = 0 ; usIndex < 1500 ; usIndex ++ ) {
      apbBuf [usIndex] = malloc ( usIndex ) ;

      if ( usIndex > 0 ) {
         printf ( "\napbBuf [%d] = %p delta = %ld",
                  usIndex,
                  apbBuf [usIndex] ,
                  (PBYTE) apbBuf [usIndex] -
                     (PBYTE) apbBuf [usIndex - 1] ) ;
      } /* endif */

      if ((( usIndex % 25 ) == 0 ) && ( usIndex != 0 )) {
         printf ( "\nPress ENTER to continue..." ) ;
         fflush ( stdout ) ;
         getchar () ;
      } /* endif */
   } /* endfor */

   for ( usIndex = 0 ; usIndex < 1500 ; usIndex ++ ) {
      free ( apbBuf [usIndex] ) ;
   } /* endfor */

   return 0 ;
}
