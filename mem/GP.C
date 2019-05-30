#define INCL_DOSMEMMGR
#define INCL_DOSEXCEPTIONS

#include <os2.h>
#include <stdio.h>

#define NUM_PAGES                8
#define SZ_PAGE                  4096

ULONG MyExceptionHandler ( PEXCEPTIONREPORTRECORD pTrap ) ;

PBYTE pbBase ;
BOOL bGuardUp ;

INT main ( USHORT usNumArgs, PCHAR apchArgs [] )
{
   LONG     lIndex ;
   EXCEPTIONREGISTRATIONRECORD      errRegister ;
   APIRET      arReturn ;

   pbBase = NULL ;

   if ( usNumArgs > 1 ) {
      bGuardUp = TRUE ;
      printf ( "Guarding up\n" ) ;
   } else {
      bGuardUp = FALSE ;
      printf ( "Guarding down\n" ) ;
   } /* endif */

   errRegister.ExceptionHandler = (_ERR *) &MyExceptionHandler ;
   arReturn = DosSetExceptionHandler ( &errRegister ) ;
   printf ( "DosSetExceptionHandler returns %ld\n", arReturn ) ;

   /* allocate some memory */
   arReturn = DosAllocMem ( (PPVOID) &pbBase,
                             NUM_PAGES * SZ_PAGE,
                             PAG_READ | PAG_WRITE ) ;

   printf ( "DosAllocMem returns %ld ( pbBase = %p ) \n",
      arReturn,
      pbBase ) ;

   if ( !bGuardUp ) {
      //-----------------------------------------------------------
      // Commit last page and set to guard page
      //-----------------------------------------------------------
      arReturn = DosSetMem ( pbBase + (( NUM_PAGES - 1 ) *
                             SZ_PAGE ) ,
                             SZ_PAGE,
                             PAG_COMMIT | PAG_READ |
                             PAG_WRITE | PAG_GUARD ) ;
      printf ( "Return Code from DosSetMem, "
               "%ld - pbBase = %p\n",
               arReturn,
               pbBase ) ;

      //-----------------------------------------------------------
      // Write to pages, from top to bottom
      //-----------------------------------------------------------
      for ( lIndex = ( NUM_PAGES * SZ_PAGE ) - 1L ;
         lIndex >= 0L ;
         lIndex -= 0x0010L ) {
          printf ( "\rWriting to offset 0x%08lX", lIndex ) ;
          pbBase [lIndex] = 1 ;
          printf ( "\rWritten to offset 0x%08lX", lIndex ) ;
      } /* endfor */
   } else {
      //-----------------------------------------------------------
      // Commit first page and set to guard page
      //-----------------------------------------------------------
      arReturn = DosSetMem ( pbBase,
                             SZ_PAGE,
                             PAG_COMMIT | PAG_READ |
                             PAG_WRITE | PAG_GUARD ) ;
      printf ( "Return Code from DosSetMem, "
               "%ld - pbBase = %p\n",
               arReturn,
               pbBase ) ;

      //-----------------------------------------------------------
      // Write to pages, from bottom to top
      //-----------------------------------------------------------
      for ( lIndex = 0L ;
         lIndex < ( NUM_PAGES * SZ_PAGE ) ;
         lIndex += 0x0010L ) {
          printf ( "\rWriting to offset 0x%08lX", lIndex ) ;
          pbBase [lIndex] = 1 ;
          printf ( "\rWritten to offset 0x%08lX", lIndex ) ;
      } /* endfor */
   } /* endif */

   printf ( "\n" ) ;

   //--------------------------------------------------------------
   // Free memory area
   //--------------------------------------------------------------
   printf ( "Freeing pbBase = %p\n", pbBase ) ;
   arReturn = DosFreeMem ( pbBase ) ;

   printf ( "Done\n" ) ;
   return 0 ;
}

ULONG MyExceptionHandler ( PEXCEPTIONREPORTRECORD perrTrap )
{
   ULONG       ulReturn ;
   APIRET      arReturn ;
   PBYTE       pbTrap ;

   ulReturn = XCPT_CONTINUE_SEARCH ;

   if ( perrTrap -> ExceptionNum == XCPT_GUARD_PAGE_VIOLATION ) {
      DosBeep ( 300, 100 ) ;
      printf ( "\n *** Guard exception *** \n" ) ;

      pbTrap = ( PBYTE ) perrTrap -> ExceptionInfo [1] ;

      //---------------------------------------------------------
      // Check that the fault is within our memory zone, so that
      // we won't interfer with system handling of stack growth
      //---------------------------------------------------------
      if (( pbTrap >= pbBase ) &&
          ( pbTrap < pbBase + NUM_PAGES * SZ_PAGE )) {

         if ( !bGuardUp ) {
            //-----------------------------------------------------
            // Unguard guard page
            //-----------------------------------------------------
            arReturn = DosSetMem ( pbTrap,
                                   SZ_PAGE,
                                   PAG_READ | PAG_WRITE ) ;

            printf ( "DosSetMem returns %ld "
                     "( pbTrap = 0x%08lX ) \n",
                     arReturn,
                     pbTrap ) ;

            //-----------------------------------------------------
            // Commit and guard next page below
            //-----------------------------------------------------
            printf ( "Going down!\n" ) ;
            pbTrap -= SZ_PAGE ;

            if (( pbTrap >= pbBase ) &&
                ( pbTrap < pbBase + NUM_PAGES * SZ_PAGE )) {
               arReturn = DosSetMem ( pbTrap,
                                      SZ_PAGE,
                                      PAG_COMMIT | PAG_READ |
                                      PAG_WRITE | PAG_GUARD ) ;

               printf ( "DosSetMem returns %ld "
                        "( pbTrap = 0x%08lX ) \n",
                        arReturn,
                        pbTrap ) ;
            } /* endif */

            //-----------------------------------------------------
            // We can continue execution
            //-----------------------------------------------------
            ulReturn = XCPT_CONTINUE_EXECUTION ;
         } else {
            //-----------------------------------------------------
            // Unguard guard page
            //-----------------------------------------------------
            arReturn = DosSetMem ( pbTrap,
                                   SZ_PAGE,
                                   PAG_READ | PAG_WRITE ) ;

            printf ( "DosSetMem returns %ld "
                     "( pbTrap = 0x%08lX ) \n",
                     arReturn,
                     pbTrap ) ;

            printf ( "Going up!\n" ) ;
            pbTrap += SZ_PAGE ;

            //-----------------------------------------------------
            // Commit and guard next page above
            //-----------------------------------------------------
            if (( pbTrap >= pbBase ) &&
                ( pbTrap < pbBase + NUM_PAGES * SZ_PAGE )) {
               arReturn = DosSetMem ( pbTrap,
                                      SZ_PAGE,
                                      PAG_COMMIT | PAG_READ |
                                      PAG_WRITE | PAG_GUARD ) ;

               printf ( "DosSetMem returns %ld "
                        "( pbTrap = 0x%08lX ) \n",
                        arReturn,
                        pbTrap ) ;
            } /* endif */

            //-----------------------------------------------------
            // We can continue execution
            //-----------------------------------------------------
            ulReturn = XCPT_CONTINUE_EXECUTION ;
         } /* endif */
      } /* endif */
   } /* endif */

   return ulReturn ;
}
