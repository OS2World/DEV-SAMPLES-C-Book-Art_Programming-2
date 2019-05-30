#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define DEFAULT_THREAD_FLAGS     0
#define DEFAULT_THREAD_STACK     0x4000
#define MY_BEGIN_SEMAPHORE       "\\SEM32\\BEGIN"
#define MAX_SEM_WAIT             - 1L
#define DEFAULT_NUM_THREADS      10
#define MAX_NUM_THREADS          255
#define SEMAPHORE_STATE          1
#define WAIT_FLAG_STATE          0

typedef struct _OPTIONS {
   BYTE     bWait ;
   HEV      hevKillThread ;
   BYTE     usMode ;
} OPTIONS, *POPTIONS ;

VOID APIENTRY MyThreadOne ( POPTIONS poOptions ) ;

INT main ( USHORT usNumArgs, PCHAR apchArgs [] )
{
   USHORT        usNumThreads ;
   OPTIONS       oOptions ;
   USHORT        usIndex ;
   TID           tidThread ;
   INT           iCharRead ;
   USHORT        usReturn ;

   if ( usNumArgs > 1 ) {

      //----------------------------------------------------------
      // Insure that usNumThreads is in the range 1<<x<<MAX
      //----------------------------------------------------------

      usNumThreads = max ( min ( atoi ( apchArgs [1] ) ,
                           MAX_NUM_THREADS ) ,
                           1 ) ;
   } else {
      usNumThreads = DEFAULT_NUM_THREADS ;
   } /* endif */

   if ( usNumArgs > 2 ) {
      oOptions.usMode = SEMAPHORE_STATE ;
      usReturn = DosCreateEventSem ( MY_BEGIN_SEMAPHORE,
                                     &oOptions.hevKillThread,
                                     NULLHANDLE,
                                     FALSE ) ;
   } else {
      oOptions.usMode = WAIT_FLAG_STATE ;
      oOptions.bWait = TRUE ;
   } /* endif */

   for ( usIndex = 0 ; usIndex < usNumThreads ; usIndex ++ ) {
      printf ( " Starting Thread #%2d\n", usIndex + 1 ) ;
      usReturn = DosCreateThread ( &tidThread,
                                   ( PFNTHREAD ) MyThreadOne,
                                   ( ULONG ) &oOptions,
                                   DEFAULT_THREAD_FLAGS,
                                   DEFAULT_THREAD_STACK ) ;
   } /* endfor */

   printf ( "\n Start typing and experience " ) ;
   printf ( "the speed of flags for yourself..." ) ;
   printf ( "\n >> lower case 'x' exits << \n\n" ) ;

   fflush ( stdout ) ;

   iCharRead = getche ( ) ;

   while ( iCharRead != 'x' ) {
      iCharRead = getche ( ) ;
   } /* endwhile */

   usReturn = DosPostEventSem ( oOptions.hevKillThread ) ;
   DosSleep ( 1000L ) ;
   usReturn = DosCloseEventSem ( oOptions.hevKillThread ) ;

   return usReturn ;
}

VOID APIENTRY MyThreadOne ( POPTIONS poOptions )
{
   if ( poOptions -> usMode == SEMAPHORE_STATE ) {
       DosWaitEventSem ( poOptions -> hevKillThread,
                        MAX_SEM_WAIT ) ;
   } else {
      while ( poOptions -> bWait ) {

         //-------------------------------------------------------
         // This area is left blank to demonstrate the superiority
         // of semaphores!
         //-------------------------------------------------------

      } /* endwhile */
   } /* endif */
   return ;
}
