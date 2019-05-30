#define INCL_DOS
#define INCL_WINDIALOGS
#define INCL_WINMENUS
#define INCL_WINSTDSLIDER
#define INCL_WINSYS
#define INCL_WINWINDOWMGR

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sliderob.h"

#define CLS_OBJECT               "ObjectClass"

#define UM_STARTCOPY             ( WM_USER )
#define UM_COPYDONE              ( WM_USER + 1 )

#define COPY_FILE                "SLIDEROB.C"
#define BACKUP_FILE              "SLIDEROB.BAK"

#define SZ_STACK                 16384

typedef struct _SLIDERINFO {
   ULONG       ulStructSize ;
   HWND        hwndSlider ;
   HWND        hwndObject ;
   HWND        hwndDialog ;
} SLIDERINFO, * PSLIDERINFO ;

typedef VOID ( * _Optlink PFNCOPY ) ( PVOID ) ;

BOOL CopyFile ( HWND hwndSlider ) ;
MRESULT EXPENTRY ObjectWndProc ( HWND hwndWnd,
                                 ULONG ulMsg,
                                 MPARAM mpParm1,
                                 MPARAM mpParm2 ) ;

VOID CopyThread ( PSLIDERINFO psiSlider ) ;

MRESULT EXPENTRY DlgProc ( HWND hwndDlg,
                           ULONG ulMsg,
                           MPARAM mpParm1,
                           MPARAM mpParm2 ) ;

INT main ( VOID )
{
   HAB               habAnchor ;
   HMQ               hmqQueue ;
   PSLIDERINFO       pSliderInfo ;

   habAnchor = WinInitialize ( 0 ) ;
   hmqQueue = WinCreateMsgQueue ( habAnchor, 0 ) ;

   pSliderInfo = (PSLIDERINFO) malloc ( sizeof ( SLIDERINFO )) ;
   if ( pSliderInfo == NULL ) {
      WinMessageBox ( HWND_DESKTOP,
                      HWND_DESKTOP,
                      "Out of memory !",
                      "Error",
                      0,
                      MB_ICONEXCLAMATION | MB_OK ) ;
      WinDestroyMsgQueue ( hmqQueue ) ;
      WinTerminate ( habAnchor ) ;
      return 0 ;
   } /* endif */

   pSliderInfo -> ulStructSize = sizeof ( SLIDERINFO ) ;

   if ( _beginthread(( PFNCOPY ) CopyThread,
                       0,
                       SZ_STACK,
                       ( PVOID ) pSliderInfo ) == - 1 ) {

      WinMessageBox ( HWND_DESKTOP,
                      HWND_DESKTOP,
                      "Cannot create thread !",
                      "Error",
                      0,
                      MB_ICONEXCLAMATION | MB_OK ) ;

      free ( pSliderInfo ) ;
      WinDestroyMsgQueue ( hmqQueue ) ;
      WinTerminate ( habAnchor ) ;
      return 0 ;
   } /* endif */

   WinDlgBox ( HWND_DESKTOP,
               HWND_DESKTOP,
               DlgProc,
               NULLHANDLE,
               IDD_FCOPYDLG,
               pSliderInfo ) ;

   WinDestroyMsgQueue ( hmqQueue ) ;
   WinDestroyWindow ( pSliderInfo -> hwndObject ) ;
   free ( pSliderInfo ) ;
   WinTerminate ( habAnchor ) ;
   return 0 ;
}

MRESULT EXPENTRY DlgProc ( HWND hwndDlg,
                           ULONG ulMsg,
                           MPARAM mpParm1,
                           MPARAM mpParm2 )
{
   PSLIDERINFO psiSlider ;

   psiSlider = WinQueryWindowPtr ( hwndDlg, 0 ) ;

   switch ( ulMsg ) {

   case WM_INITDLG:
      {
         CHAR        achFont [16] ;
         USHORT      usIndex ;
         CHAR        achMessage [64] ;

         psiSlider = (PSLIDERINFO) PVOIDFROMMP ( mpParm2 ) ;
         WinSetWindowPtr ( hwndDlg, 0, psiSlider ) ;

         psiSlider -> hwndDialog = hwndDlg ;
         psiSlider -> hwndSlider = WinWindowFromID ( hwndDlg,
                                                     IDS_SLIDER ) ;

         //--------------------------------------------------------
         // Set the size of the tick marks
         //--------------------------------------------------------

         WinSendDlgItemMsg ( hwndDlg,
                             IDS_SLIDER,
                             SLM_SETTICKSIZE,
                             MPFROM2SHORT ( SMA_SETALLTICKS, 7 ) ,
                             0 ) ;

         strcpy ( achFont, "8.Tms Rmn" ) ;
         WinSetPresParam ( WinWindowFromID ( hwndDlg, IDS_SLIDER ) ,
                           PP_FONTNAMESIZE,
                           strlen ( achFont ) + 1,
                           achFont ) ;

         for ( usIndex = 0 ; usIndex < 11 ; usIndex ++ ) {
            sprintf ( achMessage, "%d%%", usIndex * 10 ) ;

            WinSendDlgItemMsg ( hwndDlg,
                                IDS_SLIDER,
                                SLM_SETSCALETEXT,
                                MPFROMSHORT ( usIndex ) ,
                                MPFROMP ( achMessage )) ;
         } /* endfor */
      }
      break ;


   case WM_COMMAND:

      switch ( SHORT1FROMMP ( mpParm1 )) {

         case IDP_START:
            WinPostMsg ( psiSlider -> hwndObject,
                         UM_STARTCOPY,
                         0,
                         0 ) ;
            WinEnableWindow ( WinWindowFromID ( hwndDlg,
                                                IDP_START ) ,
                              FALSE ) ;
            break ;

         case IDP_CANCEL:
            WinDismissDlg ( hwndDlg, FALSE ) ;
            break ;

         default:
            return WinDefDlgProc ( hwndDlg,
                                   ulMsg,
                                   mpParm1,
                                   mpParm2 ) ;
      } /* endswitch */
      break ;

   case WM_DRAWITEM:
      {
         POWNERITEM poiItem ;

         //--------------------------------------------------------
         // get the OWNERITEM structure from mpParm2
         //--------------------------------------------------------

         poiItem = ( POWNERITEM ) PVOIDFROMMP ( mpParm2 ) ;

         switch ( poiItem -> idItem ) {
         case SDA_RIBBONSTRIP:
            WinFillRect ( poiItem -> hps,
                          &poiItem -> rclItem,
                          CLR_BLUE ) ;
            return MRFROMSHORT ( TRUE ) ;

         default:
            return WinDefDlgProc ( hwndDlg,
                                   ulMsg,
                                   mpParm1,
                                   mpParm2 ) ;
         } /* endswitch */
      }

   case UM_COPYDONE:
      {
         USHORT      usState ;
         CHAR        achMessage [60] ;

         WinEnableWindow ( WinWindowFromID ( hwndDlg,
                                             IDP_START ) ,
                           TRUE ) ;

         usState = SHORT1FROMMP ( mpParm1 ) ;
         if ( usState == TRUE ) {
            strcpy ( achMessage, "Copy completed Successfully" ) ;
            WinAlarm ( HWND_DESKTOP, WA_NOTE ) ;
         } else {
            strcpy ( achMessage, "Copy failed" ) ;
            WinAlarm ( HWND_DESKTOP, WA_ERROR ) ;
         } /* endif */

         WinMessageBox ( HWND_DESKTOP,
                         hwndDlg,
                         achMessage,
                         "Copy Status",
                         0,
                         MB_OK | MB_INFORMATION ) ;
      }
      break ;

   default:
      return WinDefDlgProc ( hwndDlg,
                             ulMsg,
                             mpParm1,
                             mpParm2 ) ;
   } /* endswitch */

   return MRFROMSHORT ( FALSE ) ;
}

VOID CopyThread ( PSLIDERINFO psiSlider )
{
   HAB        habAnchor ;
   HMQ        hmqQueue ;
   BOOL       bLoop ;
   QMSG       qmMsg ;

   habAnchor = WinInitialize ( 0 ) ;
   hmqQueue = WinCreateMsgQueue ( habAnchor, 0 ) ;

   WinRegisterClass ( habAnchor,
                      CLS_OBJECT,
                      ObjectWndProc,
                      0,
                      sizeof ( PVOID )) ;

   psiSlider -> hwndObject = WinCreateWindow ( HWND_OBJECT,
                                               CLS_OBJECT,
                                               NULL,
                                               0,
                                               0,
                                               0,
                                               0,
                                               0,
                                               NULLHANDLE,
                                               HWND_TOP,
                                               ID_OBJECT,
                                               psiSlider,
                                               NULL ) ;

   if ( psiSlider -> hwndObject != NULLHANDLE ) {
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

      WinDestroyWindow ( psiSlider -> hwndObject ) ;
   } /* endif */

   WinDestroyMsgQueue ( hmqQueue ) ;
   WinTerminate ( habAnchor ) ;
   return ;
}

MRESULT EXPENTRY ObjectWndProc ( HWND hwndWnd,
                                 ULONG  ulMsg,
                                 MPARAM mpParm1,
                                 MPARAM mpParm2 )
{
   switch ( ulMsg ) {

      case WM_CREATE:
         WinSetWindowPtr ( hwndWnd, 0, PVOIDFROMMP ( mpParm1 )) ;
         break ;

      case UM_STARTCOPY:
         {
            PSLIDERINFO pSliderInfo ;

            pSliderInfo = WinQueryWindowPtr ( hwndWnd, 0 ) ;

            if ( CopyFile ( pSliderInfo -> hwndSlider )) {
               WinPostMsg ( pSliderInfo -> hwndDialog,
                            UM_COPYDONE,
                            MPFROMSHORT ( TRUE ) ,
                            0 ) ;
            } else {
               WinPostMsg ( pSliderInfo -> hwndDialog,
                            UM_COPYDONE,
                            MPFROMSHORT ( FALSE ) ,
                            0 ) ;
            } /* endif */
         }
         break ;

      default:
         return WinDefWindowProc ( hwndWnd,
                                   ulMsg,
                                   mpParm1,
                                   mpParm2 ) ;
   } /* endswitch */

   return MRFROMSHORT ( FALSE ) ;
}

BOOL CopyFile ( HWND hwndSlider )
{
   APIRET            arRc ;
   FILESTATUS3       fsStatus ;
   PBYTE             pbBuffer ;
   HFILE             hfRead ;
   ULONG             ulAction ;
   HFILE             hfWrite ;
   ULONG             ulSzBlock ;
   USHORT            usIndex ;
   ULONG             ulBytesRead ;
   ULONG             ulBytesWritten ;

   arRc = DosQueryPathInfo ( COPY_FILE,
                             FIL_STANDARD,
                             (PVOID) &fsStatus,
                             sizeof ( fsStatus )) ;

   if ( !arRc ) {

      ulSzBlock = fsStatus.cbFile / 10 + 1 ;

      pbBuffer = (PBYTE) malloc ( ulSzBlock ) ;

      //-----------------------------------------------------------
      // Open up the file for reading
      //-----------------------------------------------------------

      arRc = DosOpen ( COPY_FILE,
                       &hfRead,
                       &ulAction,
                       0,
                       FILE_NORMAL,
                       FILE_OPEN,
                       OPEN_ACCESS_READONLY | OPEN_SHARE_DENYWRITE,
                       0 ) ;

      //-----------------------------------------------------------
      // Open up the backup file for write
      //-----------------------------------------------------------

      arRc = DosOpen ( BACKUP_FILE,
                       &hfWrite,
                       &ulAction,
                       0,
                       FILE_NORMAL,
                       FILE_CREATE,
                       OPEN_ACCESS_WRITEONLY |
                       OPEN_SHARE_DENYREADWRITE,
                       0 ) ;

      for ( usIndex = 1 ; usIndex < 11 ; usIndex ++ ) {

         //--------------------------------------------------------
         // Read a block
         //--------------------------------------------------------

         DosRead ( hfRead,
                   pbBuffer,
                   ulSzBlock,
                   &ulBytesRead ) ;

         //--------------------------------------------------------
         // Write a block
         //--------------------------------------------------------

         DosWrite ( hfWrite,
                    pbBuffer,
                    ulBytesRead,
                    &ulBytesWritten ) ;

         //--------------------------------------------------------
         // Tell the slider to move
         //--------------------------------------------------------

         WinSendMsg ( hwndSlider,
                      SLM_SETSLIDERINFO,
                      MPFROM2SHORT ( SMA_SLIDERARMPOSITION,
                         SMA_INCREMENTVALUE ) ,
                      MPFROMSHORT ( usIndex )) ;
      } /* endfor */

      //-----------------------------------------------------------
      // Clean up
      //-----------------------------------------------------------

      DosClose ( hfRead ) ;
      DosClose ( hfWrite ) ;
      free ( pbBuffer ) ;
      return TRUE ;
   } else {
      return FALSE ;
   } /* endif */
}
