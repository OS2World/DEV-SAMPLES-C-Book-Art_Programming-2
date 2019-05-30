#define  INCL_DOSNMPIPES

#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>

#define MAX_INPUT_ARGS           3
#define SERVER_MODE              1
#define CLIENT_MODE              2
#define DISCON_MODE              3
#define BAD_INPUT_ARGS           99
#define MAX_PIPE_NAME_LEN        80
#define MAX_SERV_NAME_LEN        8
#define DEFAULT_PIPE_NAME        "\\PIPE\\MYPIPE"
#define DEFAULT_MAKE_MODE        NP_ACCESS_DUPLEX
#define DEFAULT_PIPE_MODE        NP_WMESG | NP_RMESG | 0x01
#define DEFAULT_OPEN_FLAG        OPEN_ACTION_OPEN_IF_EXISTS
#define DEFAULT_OPEN_MODE        OPEN_FLAGS_WRITE_THROUGH | \
                                 OPEN_FLAGS_FAIL_ON_ERROR | \
                                 OPEN_FLAGS_RANDOM |        \
                                 OPEN_SHARE_DENYNONE |      \
                                 OPEN_ACCESS_READWRITE
#define DEFAULT_OUTB_SIZE        0x1000
#define DEFAULT_INPB_SIZE        0x1000
#define DEFAULT_TIME_OUTV        20000L
#define TOKEN_F2_SWITCH          0x0000003CL
#define TOKEN_F3_DISCON          0x0000003DL
#define RETURN_CHAR              0x0D
#define LINE_FEED_CHAR           0x0A
#define FUNC_KEYS_CHAR           0x00
#define EXTD_KEYS_CHAR           0xE0
#define HAND_SHAKE_LEN           0x08
#define HAND_SHAKE_INP           "pIpEtEsT"
#define HAND_SHAKE_OUT           "PiPeTeSt"
#define HAND_SHAKE_ERROR         101
#define PROGRAM_ERROR            999

CHAR    achPipeName [MAX_PIPE_NAME_LEN] ;
HPIPE   hpPipe ;
CHAR    chToken ;

USHORT BadArgs ( USHORT usNumArgs, PCHAR apchArgs [] ) ;
APIRET ConnToClient ( VOID ) ;
APIRET ConnToServer ( VOID ) ;
APIRET SendToClient ( ULONG ulHiLoCh ) ;
APIRET RecvFromServer ( PULONG pulHiLoCh ) ;

INT main ( USHORT usNumArgs, PCHAR apchArgs [] )
{
   USHORT   usHiCh ;
   USHORT   usLoCh ;
   ULONG    ulHiLoCh ;
   APIRET   arReturn ;

   achPipeName [0] = 0 ;
   chToken = 0 ;

   usHiCh = 0 ;
   usLoCh = 0 ;
   ulHiLoCh = 0 ;

   if ( !BadArgs ( usNumArgs, apchArgs )) {
      if ( chToken == SERVER_MODE ) {
          printf ( "\n[0;32;40m Starting the program in "
                   "Server Mode...\n" ) ;
          printf ( "\n Press F2 to switch to client mode, "
                   "F3 to quit \n" ) ;
          strcpy ( achPipeName, DEFAULT_PIPE_NAME ) ;
          arReturn = ConnToClient ( ) ;
      } else

      if ( chToken == CLIENT_MODE ) {
          printf ( "\n[0;31;40m Starting the program in "
                   "Client Mode...\n" ) ;
          printf ( "\n Press F2 to switch to server mode, "
                   "F3 to quit \n" ) ;
          if ( usNumArgs == MAX_INPUT_ARGS ) {
              sprintf ( achPipeName, "\\\\%s", apchArgs [2] ) ;
          }
          strcat ( achPipeName, DEFAULT_PIPE_NAME ) ;
          arReturn = ConnToServer ( ) ;

      } else {
          arReturn = PROGRAM_ERROR ;
      }

      while ( !arReturn && ( chToken != DISCON_MODE )) {

         switch ( chToken ) {
         case SERVER_MODE:
            usHiCh = getch ( ) ;
            if (( usHiCh == FUNC_KEYS_CHAR ) ||
                ( usHiCh == EXTD_KEYS_CHAR )) {
               usLoCh = getch ( ) ;
            } else {
               usLoCh = usHiCh ;
            } /* endif */

            ulHiLoCh = (( ULONG ) usHiCh << 16 ) |
                        ( ULONG ) usLoCh ;

            arReturn  = SendToClient ( ulHiLoCh ) ;

            if ( ulHiLoCh == TOKEN_F2_SWITCH ) {
               chToken = CLIENT_MODE ;
               printf ( "\n  [0 ; 31 ; 40m Switching to "
                        "Client Mode...\n" ) ;
               break ;
            } else
            if ( ulHiLoCh == TOKEN_F3_DISCON ) {
               chToken = DISCON_MODE ;
               break ;
            } /* endif */

            putch ( usHiCh ) ;

            if ( usHiCh == RETURN_CHAR ) {
               putch ( LINE_FEED_CHAR ) ;
            } /* endif */
            break ;

         case CLIENT_MODE:
            arReturn = RecvFromServer ( &ulHiLoCh ) ;

            if ( ulHiLoCh == TOKEN_F2_SWITCH ) {
               chToken = SERVER_MODE ;
               printf ( "\n  [0 ; 32 ; 40m Switching to "
                        "Server Mode...\n" ) ;
               break ;
            } else
            if ( ulHiLoCh == TOKEN_F3_DISCON ) {
               chToken = DISCON_MODE ;
               break ;
            } /* endif */

            usHiCh = ( USHORT ) ( ulHiLoCh & 0x0000FFFFL ) ;

            putch ( usHiCh ) ;

            if ( usHiCh == RETURN_CHAR ) {
               putch ( LINE_FEED_CHAR ) ;
            } /* endif */
            break ;

         default:
            arReturn = PROGRAM_ERROR ;
            break ;
         } /* endswitch */
      } /* endwhile */
   } /* endif */

   if ( arReturn == 0 ) {
      arReturn = DosClose ( hpPipe ) ;
   } /* endif */

   printf ( "\n  [0 ; 37 ; 40m" ) ;
   return arReturn ;
}

USHORT BadArgs ( USHORT usNumArgs, PCHAR apchArgs [] )
{
   if ( usNumArgs == SERVER_MODE ) {
      chToken = SERVER_MODE ;
   } else
   if (( usNumArgs == CLIENT_MODE ) &&
      ( apchArgs [1] [0] != '?' )) {
      chToken = CLIENT_MODE ;
   } else
   if (( usNumArgs == MAX_INPUT_ARGS ) &&
       ( strlen ( apchArgs [2] ) <= MAX_SERV_NAME_LEN )) {
      chToken = CLIENT_MODE ;
   } else {
       printf ( "\n  Incorrect command line argument was "
                "entered" ) ;
       printf ( "\n  Please enter:\n" ) ;
       printf ( "\n  [0;31;40m  pipe CLIENT SERVERNAME "
                " ( use for CLIENT operation ) " ) ;
       printf ( "\n  [0;32;40m  pipe                "
                " ( use for SERVER operation ) " ) ;
       printf ( "\n  [0;37;40m \n" ) ;

       return BAD_INPUT_ARGS ;
   } /* endif */

   return 0 ;
}

APIRET ConnToClient ( VOID )
{
   CHAR         achInitBuf [HAND_SHAKE_LEN + 1] ;
   ULONG        ulOpenMode ;
   ULONG        ulPipeMode ;
   ULONG        ulOutBufSize ;
   ULONG        ulInpBufSize ;
   ULONG        ulTimeOut ;
   USHORT       arReturn ;
   ULONG        ulBytesDone ;

   memset ( achInitBuf, 0, sizeof ( achInitBuf )) ;

   ulOpenMode =   DEFAULT_MAKE_MODE ;
   ulPipeMode =   DEFAULT_PIPE_MODE ;
   ulOutBufSize = DEFAULT_OUTB_SIZE ;
   ulInpBufSize = DEFAULT_INPB_SIZE ;
   ulTimeOut =    DEFAULT_TIME_OUTV ;

   arReturn = DosCreateNPipe ( achPipeName,
                               &hpPipe,
                               ulOpenMode,
                               ulPipeMode,
                               ulOutBufSize,
                               ulInpBufSize,
                               ulTimeOut ) ;

   if ( !arReturn ) {
      arReturn = DosConnectNPipe ( hpPipe ) ;
      if ( !arReturn ) {
         arReturn = DosRead ( hpPipe,
                              achInitBuf,
                              ( ULONG ) HAND_SHAKE_LEN,
                              &ulBytesDone ) ;

         if ( !strcmp ( achInitBuf, HAND_SHAKE_INP )
                        && !arReturn ) {
             arReturn = DosWrite ( hpPipe,
                                   HAND_SHAKE_OUT,
                                   strlen ( HAND_SHAKE_OUT ) ,
                                   &ulBytesDone ) ;
         } else {
             arReturn = HAND_SHAKE_ERROR ;
         } /* endif */
      } /* endif */
   } /* endif */

   printf ( "\n  The Pipe Creation / Connection API "
            "returned rc = %02X\n\n",
            arReturn ) ;
   return arReturn ;
}

APIRET ConnToServer ( VOID )
{
   CHAR achInitBuf [HAND_SHAKE_LEN + 1] ;
   ULONG ulOpenFlag ;
   ULONG ulOpenMode ;
   ULONG ulActionTaken ;
   INT arReturn ;
   ULONG ulBytesDone ;

   memset ( achInitBuf, 0, sizeof ( achInitBuf )) ;

   ulOpenFlag = DEFAULT_OPEN_FLAG ;
   ulOpenMode = DEFAULT_OPEN_MODE ;

   arReturn = DosOpen ( achPipeName,
                        &hpPipe,
                        &ulActionTaken,
                        0,
                        0,
                        ulOpenFlag,
                        ulOpenMode,
                        0 ) ;

   if ( !arReturn ) {
      arReturn = DosWrite ( hpPipe,
                            HAND_SHAKE_INP,
                            strlen ( HAND_SHAKE_INP ) ,
                            &ulBytesDone ) ;

      if ( !arReturn ) {
         arReturn = DosRead ( hpPipe,
                              achInitBuf,
                              ( ULONG ) HAND_SHAKE_LEN,
                              &ulBytesDone ) ;

         if ( strcmp ( achInitBuf, HAND_SHAKE_OUT )) {
            arReturn = HAND_SHAKE_ERROR ;
         } /* endif */
      } /* endif */
   } /* endif */

   if ( arReturn ) {
      printf ( "\n  The Pipe Open / Connection API "
               "returned rc = %02x\n",
               arReturn ) ;
      printf ( "\n  Make sure the Server is running.\n\n" ) ;
   } /* endif */

   return arReturn ;
}

APIRET SendToClient ( ULONG ulHiLoCh )
{
   ULONG ulBytesDone ;

   return DosWrite ( hpPipe,
                     &ulHiLoCh,
                     sizeof ( ulHiLoCh ) ,
                     &ulBytesDone ) ;
}

APIRET RecvFromServer ( PULONG pulHiLoCh )
{
   ULONG ulBytesDone ;

   return DosRead ( hpPipe,
                    pulHiLoCh,
                    sizeof ( pulHiLoCh ) ,
                    &ulBytesDone ) ;
}
