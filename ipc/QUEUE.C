#define INCL_DOSQUEUES
#define INCL_DOSMEMMGR
#define INCL_DOSPROCESS
#define INCL_DOSERRORS
#define INCL_DOSSEMAPHORES

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define MAX_INPUT_ARGS           2
#define MAX_CLIENTS              5
#define SERVER_MODE              1
#define CLIENT_MODE              2
#define DISCON_MODE              3
#define BAD_INPUT_ARGS           99
#define MAX_COLOR_LEN            11
#define DEFAULT_QUE_NAME         "\\QUEUES\\MYQUEUE"
#define DEFAULT_SEM_NAME         "\\SEM32\\EVENTQUE"
#define DEFAULT_PAGE_SIZE        4096
#define DEFAULT_QUE_FLAG         QUE_FIFO | QUE_CONVERT_ADDRESS
#define DEFAULT_SEG_NAME         "\\SHAREMEM\\MYQUEUE.SHR"
#define DEFAULT_SEG_FLAG         PAG_WRITE | PAG_COMMIT
#define TOKEN_F2_SWITCH          0x0000003CL
#define TOKEN_F3_DISCON          0x0000003DL
#define RETURN_CHAR              0x0D
#define LINE_FEED_CHAR           0x0A
#define FUNC_KEYS_CHAR           0x00
#define EXTD_KEYS_CHAR           0xE0
#define CLEAR_HI_WORD            0x0000FFFFL
#define ULONG_NULL               0L
#define PROGRAM_ERROR            999
#define CLIENT0_COLOR            "[0;31;40m"
#define CLIENT1_COLOR            "[0;32;40m"
#define CLIENT2_COLOR            "[0;33;40m"
#define CLIENT3_COLOR            "[0;34;40m"
#define CLIENT4_COLOR            "[0;35;40m"

typedef struct  _MYQUEUESTRUC {
   BYTE szColor [MAX_COLOR_LEN] ;
   PID ulPid ;
} MYQUEUESTRUC, * PMYQUEUESTRUC ;

HQUEUE        hqQueue ;
USHORT        usClientIndex = MAX_CLIENTS ;
PVOID         pvData ;
HEV           hsmSem ;
PMYQUEUESTRUC pmqsClient ;
CHAR          chToken = 0 ;
CHAR          aachColors [MAX_CLIENTS] [MAX_COLOR_LEN] ;

USHORT BadArgs ( USHORT usNumArgs, PCHAR apchArgs [] ) ;
APIRET InitServerQueEnv ( VOID ) ;
APIRET InitClientQueEnv ( VOID ) ;
APIRET ReadFromQue ( PULONG pulHiLoCh ) ;
APIRET WriteToQue ( ULONG ulHiLoCh ) ;

INT main ( USHORT usNumArgs, PCHAR apchArgs [] )
{
   USHORT   usHiCh ;
   USHORT   usLoCh ;
   ULONG    ulHiLoCh ;
   APIRET   arReturn ;

   usHiCh = 0 ;
   usLoCh = 0 ;
   ulHiLoCh = 0 ;

   strcpy ( aachColors [0] , CLIENT0_COLOR ) ;
   strcpy ( aachColors [1] , CLIENT1_COLOR ) ;
   strcpy ( aachColors [2] , CLIENT2_COLOR ) ;
   strcpy ( aachColors [3] , CLIENT3_COLOR ) ;
   strcpy ( aachColors [4] , CLIENT4_COLOR ) ;

   if ( !BadArgs ( usNumArgs, apchArgs )) {

      if ( chToken == SERVER_MODE ) {
         printf ( "\n  [0 ; 36 ; 40m" ) ;
         printf ( " Server process is creating" ) ;
         printf ( "and initializing the Queue...\n" ) ;
         arReturn = InitServerQueEnv ( ) ;
      } else

      if ( chToken == CLIENT_MODE ) {
         printf ( "\n  [0 ; 37 ; 40m" ) ;
         printf ( " Client process is initializing" ) ;
         printf ( " and connecting to the Queue...\n" ) ;
         arReturn = InitClientQueEnv ( ) ;

      } else {
         arReturn = PROGRAM_ERROR ;
      } /* endif */

      while ( !arReturn && ( chToken != DISCON_MODE )) {

         switch ( chToken ) {

         case SERVER_MODE:
            if ( kbhit ( )) {
               usHiCh = getch ( ) ;
               if (( usHiCh == FUNC_KEYS_CHAR ) ||
                   ( usHiCh == EXTD_KEYS_CHAR )) {
                  usLoCh = getch ( ) ;

               } else {
                  usLoCh = usHiCh ;
               } /* endif */

               ulHiLoCh = ( usHiCh << 16 ) | usLoCh ;

               if ( ulHiLoCh == TOKEN_F3_DISCON ) {
                  chToken = DISCON_MODE ;
               } /* endif */

            } /* endif */

            arReturn = ReadFromQue ( &ulHiLoCh ) ;
            if ( !arReturn ) {
                usLoCh = ( USHORT ) ( ulHiLoCh & 0x0000FFFFL ) ;
                usHiCh = ( USHORT ) ( ulHiLoCh >> 16 ) ;

                if ( usHiCh == usLoCh ) {
                   putch ( usHiCh ) ;

                   if ( usHiCh == RETURN_CHAR ) {
                      putch ( LINE_FEED_CHAR ) ;
                   } /* endif */
                } /* endif */

            } else
            if ( arReturn == ERROR_QUE_EMPTY ) {
               arReturn = ( SHORT ) NULL ;
            } /* endif */

            break ;

         case CLIENT_MODE:
            usHiCh = getch ( ) ;

            if (( usHiCh == FUNC_KEYS_CHAR ) ||
                ( usHiCh == EXTD_KEYS_CHAR )) {
               usLoCh = getch ( ) ;

            } else {
               usLoCh = usHiCh ;
               putch ( usHiCh ) ;

               if ( usHiCh == RETURN_CHAR ) {
                  putch ( LINE_FEED_CHAR ) ;
               } /* endif */

            } /* endif */

            ulHiLoCh = (( ULONG ) usHiCh << 16 ) |
               ( ULONG ) usLoCh ;
            if ( ulHiLoCh == TOKEN_F3_DISCON ) {
               chToken = DISCON_MODE ;
               pmqsClient [usClientIndex] .szColor [0] = '\0' ;
               pmqsClient [usClientIndex] .ulPid = 0 ;

            } else {
               arReturn = WriteToQue ( ulHiLoCh ) ;
            } /* endif */
            break ;

         default:
            arReturn = PROGRAM_ERROR ;
            break ;
         } /* endswitch */
      } /* endif */
   } /* endif */

   if ( arReturn == 0 ) {
      arReturn = DosCloseQueue ( hqQueue ) ;
   } /* endif */

   printf ( "\n  [0 ; 37 ; 40m" ) ;
   return arReturn ;
}

USHORT BadArgs ( USHORT usNumArgs, PCHAR apchArgs [] )
{
   if ( usNumArgs == SERVER_MODE ) {
      chToken =  SERVER_MODE ;
   } else
   if (( usNumArgs == CLIENT_MODE ) &&
       ( apchArgs [1] [0] != '?' )) {
      chToken =  CLIENT_MODE ;
   } else {
      printf ( "\n Incorrect command line argument was entered" ) ;
      printf ( "\n  Please enter:\n" ) ;
      printf ( "\n   [0 ; 37 ; 40m  QUEUE CLIENT" ) ;
      printf ( "use for CLIENT operation ) " ) ;
      printf ( "\n   [0 ; 36 ; 40m  QUEUE" ) ;
      printf ( "        ( use for SERVER operation ) " ) ;
      printf ( "\n   [0 ; 37 ; 40m \n" ) ;

      return BAD_INPUT_ARGS ;
   } /* endif */

   return 0 ;
}

APIRET InitServerQueEnv ( VOID )
{
   APIRET arReturn ;
   SHORT sIndex ;

   arReturn = DosAllocSharedMem(( PVOID ) &pmqsClient,
                                  DEFAULT_SEG_NAME,
                                  DEFAULT_PAGE_SIZE,
                                  DEFAULT_SEG_FLAG ) ;

   if ( !arReturn ) {
      arReturn = DosCreateQueue ( &hqQueue,
                                  DEFAULT_QUE_FLAG,
                                  DEFAULT_QUE_NAME ) ;

      if ( !arReturn ) {
         printf ( "\n  Queue created successfully \n" ) ;
         for ( sIndex = 0 ; sIndex < MAX_CLIENTS ; sIndex ++ ) {
            pmqsClient [sIndex] .szColor [0] = ( BYTE ) NULL ;
            pmqsClient [sIndex] .ulPid = ( PID ) NULL ;
         } /* endfor */

         arReturn = DosCreateEventSem ( DEFAULT_SEM_NAME,
                                        &hsmSem,
                                        ULONG_NULL,
                                        TRUE ) ;
      } else {
          printf ( "\n  DosCreateQueue API returned "
                  "%02d\n",
                  arReturn ) ;
      } /* endif */
   } else {
       printf ( " \n  Could not allocate "
               "Shared Memory ( %02d ) \n",
               arReturn ) ;
   } /* endif */

   return arReturn ;
}

APIRET InitClientQueEnv ( VOID )
{
   APIRET arReturn ;
   SHORT sIndex ;
   PID pidOwner ;

   arReturn = DosGetNamedSharedMem(( PVOID ) &pmqsClient,
                                   DEFAULT_SEG_NAME,
                                   PAG_WRITE | PAG_READ ) ;

   if ( !arReturn ) {
      for ( sIndex = 0 ; sIndex <= MAX_CLIENTS ; sIndex ++ ) {
         if (( pmqsClient [sIndex] .szColor [0] == 0 ) &&
             ( sIndex < MAX_CLIENTS )) {
            strcpy ( pmqsClient [sIndex] .szColor,
                    aachColors [sIndex] ) ;
            usClientIndex = sIndex ;
            break ;
         } /* endif */
      } /* endfor */

      if ( sIndex > MAX_CLIENTS ) {
         arReturn = PROGRAM_ERROR ;
         printf ( "\n\n  Maximum number of clients is FIVE !\n" ) ;
      } /* endif */

      if ( !arReturn ) {
         arReturn = DosOpenQueue ( &pidOwner,
                                 &hqQueue,
                                 DEFAULT_QUE_NAME ) ;
         if ( !arReturn ) {
             printf ( " %s", aachColors [usClientIndex] ) ;
             printf ( "\n Client #%d has connected to the Queue\n",
                     usClientIndex ) ;
         } /* endif */
      } /* endif */
   } /* endif */

   return arReturn ;
}

APIRET ReadFromQue ( PULONG pulHiLoCh )
{
   APIRET      arReturn ;
   REQUESTDATA rdRequest ;
   ULONG       ulSzData ;
   BYTE        bPriority ;
   SHORT       sIndex ;

   arReturn = DosReadQueue ( hqQueue,
                             &rdRequest,
                             &ulSzData,
                             &pvData,
                             0,
                             DCWW_NOWAIT,
                             &bPriority,
                             hsmSem ) ;

   if ( !arReturn ) {
      pmqsClient [rdRequest.ulData] .ulPid = rdRequest.pid ;
      * pulHiLoCh = ulSzData ;

      for ( sIndex = 0 ; sIndex < MAX_COLOR_LEN - 1 ; sIndex ++ ) {
         putch ( pmqsClient [rdRequest.ulData].szColor [sIndex] ) ;
      } /* endfor */
   } /* endif */

   return arReturn ;
}

APIRET WriteToQue ( ULONG ulHiLoCh )
{
   return DosWriteQueue ( hqQueue,
                          ( ULONG ) usClientIndex,
                          ulHiLoCh,
                          pvData,
                          ULONG_NULL ) ;
}
