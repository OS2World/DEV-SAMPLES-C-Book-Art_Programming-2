#define INCL_DEV
#define INCL_DOSERRORS
#define INCL_SPL
#define INCL_SPLDOSPRINT
#define INCL_WINERRORS
#define INCL_WININPUT
#define INCL_WINLISTBOXES
#define INCL_WINMENUS
#define INCL_WINSHELLDATA
#define INCL_WINSYS
#define INCL_WINWINDOWMGR

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <process.h>
#include "print.h"

#define CLS_CLIENT               "SampleClass"

#define CPL_ERROR                (USHORT) 0
#define CPL_NOPRINTERS           (USHORT) 1
#define CPL_SUCCESS              (USHORT) 2

typedef VOID ( * _Optlink PRNTHREAD ) ( PVOID ) ;


typedef struct {
   DEVOPENSTRUC    dosPrinter ;
   CHAR            achDevice [256] ;
} PRNLISTINFO, *PPRNLISTINFO ;


typedef struct {
   ULONG          ulSizeStruct ;
   HWND           hwndOwner ;
   DEVOPENSTRUC   dosPrinter ;
} PRNTHREADINFO, *PPRNTHREADINFO ;


typedef struct _CLIENTINFO {
   HWND           hwndListbox ;
} CLIENTINFO, *PCLIENTINFO ;


MRESULT EXPENTRY clientWndProc ( HWND hwndClient,
                                 ULONG ulMsg,
                                 MPARAM mpParm1,
                                 MPARAM mpParm2 );

VOID _Optlink printThread ( PPRNTHREADINFO pptiInfo );
BOOL drawPage ( HPS hpsDraw, USHORT usPage );
USHORT createPrnList ( HWND hwndListbox );
VOID destroyPrnList ( HWND hwndListbox );
VOID extractPrnInfo ( PPRQINFO3 ppiQueue,
                      DEVOPENSTRUC *pdosPrinter );

INT main ( VOID )
{
   HAB            habAnchor ;
   HMQ            hmqQueue ;
   ULONG          ulFlags ;
   HWND           hwndFrame ;
   BOOL           bLoop ;
   QMSG           qmMsg ;

   habAnchor = WinInitialize ( 0 ) ;
   hmqQueue = WinCreateMsgQueue ( habAnchor, 0 ) ;

   WinRegisterClass ( habAnchor,
                      CLS_CLIENT,
                      clientWndProc,
                      0,
                      sizeof (PVOID)) ;

   ulFlags = FCF_SIZEBORDER | FCF_TITLEBAR | FCF_TASKLIST |
             FCF_SHELLPOSITION | FCF_SYSMENU | FCF_MENU |
             FCF_ACCELTABLE ;

   hwndFrame = WinCreateStdWindow ( HWND_DESKTOP,
                                    WS_VISIBLE,
                                    &ulFlags,
                                    CLS_CLIENT,
                                    "Printing Sample Application",
                                    0,
                                    NULLHANDLE,
                                    RES_CLIENT,
                                    NULL ) ;

   if ( hwndFrame != NULLHANDLE ) {
      bLoop = WinGetMsg ( habAnchor,
                          &qmMsg,
                          NULLHANDLE,
                          0,
                          0 ) ;
      while ( bLoop ) {
         WinDispatchMsg ( habAnchor, &qmMsg ) ;
         bLoop = WinGetMsg ( habAnchor,
                             &qmMsg,
                             NULLHANDLE,
                             0,
                             0 ) ;
      } /* endwhile */

      WinDestroyWindow ( hwndFrame ) ;
   } /* endif */

   WinDestroyMsgQueue ( hmqQueue ) ;
   WinTerminate ( habAnchor ) ;
   return ( 0 ) ;
}

MRESULT EXPENTRY clientWndProc ( HWND hwndClient,
                                 ULONG ulMsg,
                                 MPARAM mpParm1,
                                 MPARAM mpParm2 )
{
   PCLIENTINFO    pciInfo ;

   pciInfo = WinQueryWindowPtr ( hwndClient, 0 ) ;

   switch ( ulMsg ) {
   case WM_CREATE:
      {
         //--------------------------------------------------------
         // Allocate and initialize the CLIENTINFO structure
         //--------------------------------------------------------
         pciInfo = malloc ( sizeof ( CLIENTINFO )) ;
         if ( pciInfo == NULL ) {

            WinMessageBox ( HWND_DESKTOP,
                            HWND_DESKTOP,
                            "An error occurred in initialization.",
                            "Error",
                            0,
                            MB_OK | MB_INFORMATION ) ;

            return MRFROMSHORT ( TRUE ) ;
         } /* endif */

         WinSetWindowPtr ( hwndClient, 0, pciInfo ) ;

         pciInfo -> hwndListbox = WinCreateWindow ( hwndClient,
                                                    WC_LISTBOX,
                                                    "",
                                                    LS_HORZSCROLL |
                                                    LS_NOADJUSTPOS,
                                                    0,
                                                    0,
                                                    0,
                                                    0,
                                                    hwndClient,
                                                    HWND_TOP,
                                                    WND_LISTBOX,
                                                    NULL,
                                                    NULL ) ;

         if ( pciInfo -> hwndListbox == NULLHANDLE ) {
            WinMessageBox ( HWND_DESKTOP,
                            HWND_DESKTOP,
                            "An error occurred in initialization.",
                            "Error",
                            0,
                            MB_OK | MB_INFORMATION ) ;
            free ( pciInfo ) ;
            return MRFROMSHORT ( TRUE ) ;
         } /* endif */

         WinSendMsg ( hwndClient,
                      WM_COMMAND,
                      MPFROMSHORT ( MI_REFRESH ) ,
                      0 ) ;
      }
      break ;
   case WM_DESTROY:
      //-----------------------------------------------------------
      // Return the resources to the system
      //-----------------------------------------------------------
      destroyPrnList ( pciInfo -> hwndListbox ) ;
      WinDestroyWindow ( pciInfo -> hwndListbox ) ;
      break ;
   case WM_SIZE:
      WinSetWindowPos ( pciInfo -> hwndListbox,
                        NULLHANDLE,
                        0,
                        0,
                        SHORT1FROMMP ( mpParm2 ) ,
                        SHORT2FROMMP ( mpParm2 ) ,
                        SWP_MOVE | SWP_SIZE | SWP_SHOW ) ;
      break ;
   case WM_INITMENU:
      switch ( SHORT1FROMMP ( mpParm1 )) {
      case M_SAMPLE:
         {
            SHORT             sSelect ;
            PPRNLISTINFO      ppliInfo ;

            sSelect = WinQueryLboxSelectedItem (
               pciInfo -> hwndListbox ) ;

            ppliInfo =
               (PPRNLISTINFO) PVOIDFROMMR (
                  WinSendMsg ( pciInfo -> hwndListbox,
                               LM_QUERYITEMHANDLE,
                               MPFROMSHORT ( sSelect ) ,
                               0 )) ;

            //-----------------------------------------------------
            // If no printer is selected, disable the print
            // menuitem
            //-----------------------------------------------------
            if ( sSelect != LIT_NONE ) {
               WinEnableMenuItem ( HWNDFROMMP ( mpParm2 ) ,
                                   MI_PRINT,
                                   TRUE ) ;
            } else {
               WinEnableMenuItem ( HWNDFROMMP ( mpParm2 ) ,
                                   MI_PRINT,
                                   FALSE ) ;
            } /* endif */

            //-----------------------------------------------------
            // If no printer is selected or there is no driver
            // data, disable the setup menuitem
            //-----------------------------------------------------
            if (( sSelect != LIT_NONE ) && ( ppliInfo != NULL ) &&
                ( ppliInfo -> dosPrinter.pdriv != NULL )) {
               WinEnableMenuItem ( HWNDFROMMP ( mpParm2 ) ,
                                   MI_SETUP,
                                   TRUE ) ;
            } else {
               WinEnableMenuItem ( HWNDFROMMP ( mpParm2 ) ,
                                   MI_SETUP,
                                   FALSE ) ;
            } /* endif */
         }
         break ;
      default:
         return WinDefWindowProc ( hwndClient,
                                   ulMsg,
                                   mpParm1,
                                   mpParm2 ) ;
      } /* endswitch */
      break ;
   case WM_COMMAND:
      switch ( SHORT1FROMMP ( mpParm1 )) {
      case MI_PRINT:
         {
            SHORT          sIndex ;
            PPRNLISTINFO   ppliInfo ;
            PPRNTHREADINFO pptiInfo ;

            //-----------------------------------------------------
            // Query the selected printer
            //-----------------------------------------------------
            sIndex = WinQueryLboxSelectedItem (
               pciInfo -> hwndListbox ) ;

            ppliInfo =
               (PPRNLISTINFO) PVOIDFROMMR (
                  WinSendMsg ( pciInfo -> hwndListbox,
                               LM_QUERYITEMHANDLE,
                               MPFROMSHORT ( sIndex ) ,
                               0 )) ;

            //-----------------------------------------------------
            // Allocate and initialize the PRNTHREADINFO structure
            // to pass to the thread
            //-----------------------------------------------------
            pptiInfo = malloc ( sizeof ( PRNTHREADINFO )) ;
            if ( pptiInfo == NULL ) {
               WinMessageBox ( HWND_DESKTOP,
                               HWND_DESKTOP,
                               "An error occurred while"
                               " trying to print",
                               "Error",
                               0,
                               MB_OK | MB_INFORMATION ) ;
               return MRFROMSHORT ( TRUE ) ;
            } /* endif */

            pptiInfo -> ulSizeStruct = sizeof ( PRNTHREADINFO ) ;
            pptiInfo -> hwndOwner = hwndClient ;
            pptiInfo -> dosPrinter = ppliInfo -> dosPrinter ;

            //-----------------------------------------------------
            // Create the thread to do the printing
            //-----------------------------------------------------
            if ( _beginthread(( PRNTHREAD ) printThread,
                               NULL,
                               0x8000,
                               ( PVOID ) pptiInfo ) == - 1 ) {
                               WinMessageBox ( HWND_DESKTOP,
                                  HWND_DESKTOP,
                                  "An error occurred while"
                                  " trying to print",
                                  "Error",
                                  0,
                                  MB_OK | MB_INFORMATION ) ;
            } /* endif */
         }
         break ;
      case MI_SETUP:
         {
            USHORT            usSelect ;
            PPRNLISTINFO      ppliInfo ;

            //-----------------------------------------------------
            // Query the selected printer
            //-----------------------------------------------------
            usSelect = WinQueryLboxSelectedItem (
               pciInfo -> hwndListbox ) ;
            ppliInfo =
               (PPRNLISTINFO) PVOIDFROMMR (
                  WinSendMsg ( pciInfo -> hwndListbox,
                               LM_QUERYITEMHANDLE,
                               MPFROMSHORT ( usSelect ) ,
                               0 )) ;

            DevPostDeviceModes (
               WinQueryAnchorBlock ( hwndClient ) ,
                  ppliInfo -> dosPrinter.pdriv,
                  ppliInfo -> dosPrinter.pszDriverName,
                  ppliInfo -> achDevice,
                  NULL,
                  DPDM_POSTJOBPROP ) ;
         }
         break ;
      case MI_REFRESH:
         {
            USHORT      usResult ;

            destroyPrnList ( pciInfo -> hwndListbox ) ;

            usResult = createPrnList (
               pciInfo -> hwndListbox ) ;

            switch ( usResult ) {

            case CPL_ERROR:

               WinMessageBox ( HWND_DESKTOP,
                               HWND_DESKTOP,
                               "An error occurred while "
                               "refreshing the list",
                               "Error",
                               0,
                               MB_OK | MB_INFORMATION ) ;
               break ;

            case CPL_NOPRINTERS:

               WinMessageBox ( HWND_DESKTOP,
                               HWND_DESKTOP,
                               "There are no printers defined.",
                               "Warning",
                               0,
                               MB_OK | MB_INFORMATION ) ;
               break ;

            default:

               WinSendMsg (
                  WinWindowFromID ( hwndClient, WND_LISTBOX ) ,
                                    LM_SELECTITEM,
                                    MPFROMSHORT ( 0 ) ,
                                    MPFROMSHORT ( TRUE )) ;
               break ;

            } /* endswitch */
         }
         break ;
      case MI_EXIT:

         WinPostMsg ( hwndClient, WM_CLOSE, 0, 0 ) ;
         break ;

      default:

         return WinDefWindowProc ( hwndClient,
                                   ulMsg,
                                   mpParm1,
                                   mpParm2 ) ;
      } /* endswitch */
      break ;

   case WM_PAINT:
      {
         HPS         hpsPaint ;
         RECTL       rclPaint ;

         hpsPaint = WinBeginPaint ( hwndClient,
                                    NULLHANDLE,
                                    &rclPaint ) ;
         WinFillRect ( hpsPaint,
                       &rclPaint,
                       SYSCLR_WINDOW ) ;
         WinEndPaint ( hpsPaint ) ;
      }
      break ;
   default:
      return WinDefWindowProc ( hwndClient,
                                ulMsg,
                                mpParm1,
                                mpParm2 ) ;
   } /* endswitch */

   return MRFROMSHORT ( FALSE ) ;
}

VOID _Optlink printThread ( PPRNTHREADINFO pptiInfo )
//-----------------------------------------------------------------
// This function is the secondary thread which prints the output.
//
// Input:  pptiInfo - points to the PRNTHREADINFO structure
//                    containing needed information
//-----------------------------------------------------------------
{
   HAB            habAnchor ;
   HMQ            hmqQueue ;
   HDC            hdcPrinter ;
   SIZEL          szlHps ;
   HPS            hpsPrinter ;

   habAnchor = WinInitialize ( 0 ) ;
   hmqQueue = WinCreateMsgQueue ( habAnchor, 0 ) ;
   WinCancelShutdown ( hmqQueue, TRUE ) ;

   //--------------------------------------------------------------
   // Open a device context (HDC)
   //--------------------------------------------------------------
   hdcPrinter = DevOpenDC ( habAnchor,
                            OD_QUEUED,
                            "*",
                            9L,
                            (PDEVOPENDATA) &pptiInfo -> dosPrinter,
                            NULLHANDLE ) ;

   if ( hdcPrinter == NULLHANDLE ) {
      //-----------------------------------------------------------
      // An error occurred
      //-----------------------------------------------------------
      WinAlarm ( HWND_DESKTOP, WA_ERROR ) ;
      WinMessageBox ( HWND_DESKTOP,
                      pptiInfo -> hwndOwner,
                      "Error creating the device context.",
                      "Error",
                      0,
                      MB_INFORMATION | MB_OK ) ;

      WinDestroyMsgQueue ( hmqQueue ) ;
      WinTerminate ( habAnchor ) ;
      free ( pptiInfo ) ;
      _endthread ( ) ;
   } /* endif */

   //--------------------------------------------------------------
   // Query the width and height of the printer page
   //--------------------------------------------------------------
   DevQueryCaps ( hdcPrinter, CAPS_WIDTH, 1L, &szlHps.cx ) ;
   DevQueryCaps ( hdcPrinter, CAPS_HEIGHT, 1L, &szlHps.cy ) ;

   //--------------------------------------------------------------
   // Create a presentation space (HPS) associated with the printer
   // HDC
   //--------------------------------------------------------------
   hpsPrinter = GpiCreatePS ( habAnchor,
                              hdcPrinter,
                              &szlHps,
                              PU_LOENGLISH |
                              GPIT_MICRO |
                              GPIA_ASSOC ) ;

   if ( hpsPrinter == NULLHANDLE ) {
      //-----------------------------------------------------------
      // An error occurred
      //-----------------------------------------------------------
      DevCloseDC ( hdcPrinter ) ;

      WinAlarm ( HWND_DESKTOP, WA_ERROR ) ;
      WinMessageBox ( HWND_DESKTOP,
                      pptiInfo -> hwndOwner,
                      "Error creating the presentation space.",
                      "Error",
                      0,
                      MB_INFORMATION | MB_OK ) ;

      WinDestroyMsgQueue ( hmqQueue ) ;
      WinTerminate ( habAnchor ) ;
      free ( pptiInfo ) ;
      _endthread ( ) ;
   } /* endif */

   //--------------------------------------------------------------
   // Tell the printer that we are starting a print job
   //--------------------------------------------------------------
   if ( DevEscape ( hdcPrinter,
                    DEVESC_STARTDOC,
                    strlen ( pptiInfo -> dosPrinter.pszComment ) ,
                    pptiInfo -> dosPrinter.pszComment,
                    NULL,
                    NULL ) != DEV_OK ) {
      //-----------------------------------------------------------
      // An error occurred
      //-----------------------------------------------------------
      GpiDestroyPS ( hpsPrinter ) ;
      DevCloseDC ( hdcPrinter ) ;

      WinAlarm ( HWND_DESKTOP, WA_ERROR ) ;
      WinMessageBox ( HWND_DESKTOP,
                      pptiInfo -> hwndOwner,
                      "Error starting the print job.",
                      "Error",
                      0,
                      MB_INFORMATION | MB_OK ) ;

      WinDestroyMsgQueue ( hmqQueue ) ;
      WinTerminate ( habAnchor ) ;
      free ( pptiInfo ) ;
      _endthread ( ) ;
   } /* endif */

   //--------------------------------------------------------------
   // Draw sample output
   //--------------------------------------------------------------

   if ( !drawPage ( hpsPrinter, 1 )) {

      //-----------------------------------------------------------
      // An error occurred so abort the print job
      //-----------------------------------------------------------

      DevEscape ( hdcPrinter,
                  DEVESC_ABORTDOC,
                  0,
                  NULL,
                  NULL,
                  NULL ) ;
      GpiDestroyPS ( hpsPrinter ) ;
      DevCloseDC ( hdcPrinter ) ;

      WinAlarm ( HWND_DESKTOP, WA_ERROR ) ;
      WinMessageBox ( HWND_DESKTOP,
                      pptiInfo -> hwndOwner,
                      "Error drawing the printer page.",
                      "Error",
                      0,
                      MB_INFORMATION | MB_OK ) ;

      WinDestroyMsgQueue ( hmqQueue ) ;
      WinTerminate ( habAnchor ) ;
      free ( pptiInfo ) ;
      _endthread ( ) ;
   } /* endif */

   //--------------------------------------------------------------
   // Tell the printer that we are finished with the print job
   //--------------------------------------------------------------
   if ( DevEscape ( hdcPrinter,
                    DEVESC_ENDDOC,
                    0,
                    NULL,
                    NULL,
                    NULL ) != DEV_OK ) {

      //-----------------------------------------------------------
      // An error occurred so abort the print job
      //-----------------------------------------------------------

      DevEscape ( hdcPrinter,
                  DEVESC_ABORTDOC,
                  0,
                  NULL,
                  NULL,
                  NULL ) ;

      GpiDestroyPS ( hpsPrinter ) ;
      DevCloseDC ( hdcPrinter ) ;

      WinAlarm ( HWND_DESKTOP, WA_ERROR ) ;
      WinMessageBox ( HWND_DESKTOP,
                      pptiInfo -> hwndOwner,
                      "Error ending the print job.",
                      "Error",
                      0,
                      MB_INFORMATION | MB_OK ) ;

      WinDestroyMsgQueue ( hmqQueue ) ;
      WinTerminate ( habAnchor ) ;
      free ( pptiInfo ) ;
      _endthread ( ) ;
   } /* endif */

   //--------------------------------------------------------------
   // Destroy the HPS and close the printer HDC
   //--------------------------------------------------------------

   GpiDestroyPS ( hpsPrinter ) ;
   DevCloseDC ( hdcPrinter ) ;
   WinAlarm ( HWND_DESKTOP, WA_NOTE ) ;
   WinDestroyMsgQueue ( hmqQueue ) ;
   WinTerminate ( habAnchor ) ;
   free ( pptiInfo ) ;
   _endthread ( ) ;
   return ;
}

BOOL drawPage ( HPS hpsDraw, USHORT usPage )

//-----------------------------------------------------------------
// This function draws a box for page 1.  Any other page number
// returns an error.
//
// Input:  usPage - specifies the page number to draw.
// Returns:  TRUE if successful, FALSE otherwise
//-----------------------------------------------------------------

{
   switch ( usPage ) {
   case 1:
      {
         POINTL      ptlMove ;

         ptlMove.x = 10 ;
         ptlMove.y = 10 ;

         GpiMove ( hpsDraw, &ptlMove ) ;

         ptlMove.x = 100 ;
         ptlMove.y = 100 ;

         GpiBox ( hpsDraw, DRO_OUTLINE, &ptlMove, 0, 0 ) ;
      }
      break ;
   default:
      return FALSE ;
   } /* endswitch */

   return TRUE ;
}

USHORT createPrnList ( HWND hwndListbox )

//-----------------------------------------------------------------
// This function enumerates the printers available and inserts them
// into the specified listbox.
//
// Input:  hwndListbox - handle to the listbox to contain the list
// Returns:  an CPL_* constant
//-----------------------------------------------------------------

{
   SPLERR            seError ;
   ULONG             ulSzBuf ;
   ULONG             ulNumQueues ;
   ULONG             ulNumReturned ;
   ULONG             ulSzNeeded ;
   ULONG             ulIndex ;
   PPRQINFO3         ppiQueue ;
   PCHAR             pchPos ;
   PPRNLISTINFO      ppliInfo ;
   SHORT             sInsert ;

   //--------------------------------------------------------------
   // Get the size of the buffer needed
   //--------------------------------------------------------------
   seError = SplEnumQueue ( NULL,
                            3,
                            NULL,
                            0,
                            &ulNumReturned,
                            &ulNumQueues,
                            &ulSzNeeded,
                            NULL ) ;
   if ( seError != ERROR_MORE_DATA ) {
      return CPL_ERROR ;
   } /* endif */

   ppiQueue = malloc ( ulSzNeeded ) ;
   if ( ppiQueue == NULL ) {
      return CPL_ERROR ;
   } /* endif */

   ulSzBuf = ulSzNeeded ;

   //--------------------------------------------------------------
   // Get the information
   //--------------------------------------------------------------

   SplEnumQueue ( NULL,
                  3,
                  ppiQueue,
                  ulSzBuf,
                  &ulNumReturned,
                  &ulNumQueues,
                  &ulSzNeeded,
                  NULL ) ;

   //--------------------------------------------------------------
   // ulNumReturned has the count of the number of PRQINFO3
   // structures.
   //--------------------------------------------------------------

   for ( ulIndex = 0 ; ulIndex < ulNumReturned ; ulIndex ++ ) {

      //-----------------------------------------------------------
      // Since the "comment" can have newlines in it, replace them
      // with spaces
      //-----------------------------------------------------------

      pchPos = strchr ( ppiQueue [ulIndex] .pszComment, '\n' ) ;
      while ( pchPos != NULL ) {
         *pchPos = ' ' ;
         pchPos = strchr ( ppiQueue [ulIndex] .pszComment, '\n' ) ;
      } /* endwhile */

      ppliInfo = malloc ( sizeof ( PRNLISTINFO )) ;
      if ( ppliInfo == NULL ) {
         continue ;
      } /* endif */

      //-----------------------------------------------------------
      // Extract the device name before initializing the
      // DEVOPENSTRUC structure
      //-----------------------------------------------------------

      pchPos = strchr ( ppiQueue [ulIndex] .pszDriverName, '.' ) ;
      if ( pchPos != NULL ) {
         *pchPos = 0 ;
         strcpy ( ppliInfo -> achDevice, pchPos + 1 ) ;
      } /* endif */

      extractPrnInfo ( &ppiQueue [ulIndex] ,
                       &ppliInfo -> dosPrinter ) ;

      sInsert = ( SHORT ) WinInsertLboxItem ( hwndListbox,
                              0,
                              ppiQueue [ulIndex] .pszComment ) ;

      WinSendMsg ( hwndListbox,
                   LM_SETITEMHANDLE,
                   MPFROMSHORT ( sInsert ) ,
                   MPFROMP ( ppliInfo )) ;
   } /* endfor */

   free ( ppiQueue ) ;
   return CPL_SUCCESS ;
}

VOID destroyPrnList ( HWND hwndListbox )

//-----------------------------------------------------------------
// This function destroys the printer list and returns the memory
// to the system.
//
// Input:  hwndListbox - handle of the listbox containing the
//                       printer list
//-----------------------------------------------------------------

{
   USHORT            usNumItems ;
   USHORT            usIndex ;
   PPRNLISTINFO      ppliInfo ;

   usNumItems = WinQueryLboxCount ( hwndListbox ) ;

   for ( usIndex = 0 ; usIndex < usNumItems ; usIndex ++ ) {
      ppliInfo =
         (PPRNLISTINFO) PVOIDFROMMR (
            WinSendMsg ( hwndListbox,
                         LM_QUERYITEMHANDLE,
                         MPFROMSHORT ( usIndex ) ,
                         0 )) ;

      if ( ppliInfo != NULL ) {
         free ( ppliInfo ) ;
      } /* endif */
   } /* endfor */

   WinSendMsg ( hwndListbox, LM_DELETEALL, 0, 0 ) ;
   return ;
}

VOID extractPrnInfo ( PPRQINFO3 ppiQueue,
   DEVOPENSTRUC *pdosPrinter )

//-----------------------------------------------------------------
// This function extracts the needed information from the specified
// PRQINFO3 structure and places it into the specifies DEVOPENSTRUC
// structure.
//
// Input:  ppiQueue - points to the PRQINFO3 structure
// Output:  pdosPrinter - points to the initialized DEVOPENSTRUC
//                        structure
//-----------------------------------------------------------------

{
   PCHAR       pchPos ;

   pdosPrinter -> pszLogAddress = ppiQueue -> pszName ;

   pdosPrinter -> pszDriverName = ppiQueue -> pszDriverName ;
   pchPos = strchr ( pdosPrinter -> pszDriverName, '.' ) ;
   if ( pchPos != NULL ) {
      *pchPos = 0 ;
   } /* endif */

   pdosPrinter -> pdriv = ppiQueue -> pDriverData ;
   pdosPrinter -> pszDataType = "PM_Q_STD" ;
   pdosPrinter -> pszComment = ppiQueue -> pszComment ;

   if ( strlen ( ppiQueue -> pszPrProc ) > 0 ) {
      pdosPrinter -> pszQueueProcName = ppiQueue -> pszPrProc ;
   } else {
      pdosPrinter -> pszQueueProcName = NULL ;
   } /* endif */

   if ( strlen ( ppiQueue -> pszParms ) > 0 ) {
      pdosPrinter -> pszQueueProcParams = ppiQueue -> pszParms ;
   } else {
      pdosPrinter -> pszQueueProcParams = NULL ;
   } /* endif */

   pdosPrinter -> pszSpoolerParams = NULL ;
   pdosPrinter -> pszNetworkParams = NULL ;

   return ;
}
