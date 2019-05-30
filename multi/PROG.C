#define INCL_DOSPROCESS

#include <os2.h>
#include <stdio.h>

#define BUFFER_SIZE              200

INT main ( VOID )
{
   APIRET         arReturn ;
   CHAR           achFail [BUFFER_SIZE] ;
   RESULTCODES    rcResult ;

   arReturn = DosExecPgm ( achFail,
                           BUFFER_SIZE,
                           EXEC_ASYNC,
                           "CMD.EXE\0 /C dir *.* \0",
                           (PSZ) NULL,
                           &rcResult,
                           "CMD.EXE" ) ;

   if ( arReturn ) {
      printf ( "\narReturn = %d", arReturn ) ;
   } /* endif */

   printf ( "\nrcResult = %ld", rcResult.codeResult ) ;
   return 0 ;
}
