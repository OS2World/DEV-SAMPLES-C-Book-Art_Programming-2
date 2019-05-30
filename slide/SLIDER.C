#define INCL_DOSFILEMGR
#define INCL_DOSMEMMGR
#define INCL_WINDIALOGS
#define INCL_WINMENUS
#define INCL_WINSTDSLIDER
#define INCL_WINSYS

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "slider.h"

#define COPY_FILE                "SLIDER.C"
#define BACKUP_FILE              "SLIDER.BAK"

BOOL CopyFile ( HWND hwndSlider ) ;
MRESULT EXPENTRY DlgProc ( HWND hwndDlg,
                           ULONG ulMsg,
                           MPARAM mpParm1,
                           MPARAM mpParm2 ) ;

INT main ( VOID )
{
   HAB      habAnchor ;
   HMQ      hmqQueue ;

   habAnchor = WinInitialize ( 0 ) ;
   hmqQueue = WinCreateMsgQueue ( habAnchor, 0 ) ;

   WinDlgBox ( HWND_DESKTOP,
               HWND_DESKTOP,
               DlgProc,
               NULLHANDLE,
               IDD_FCOPYDLG,
               NULL ) ;

   WinDestroyMsgQueue ( hmqQueue ) ;
   WinTerminate ( habAnchor ) ;
   return 0 ;
}

MRESULT EXPENTRY DlgProc ( HWND hwndDlg,
   ULONG ulMsg,
   MPARAM mpParm1,
   MPARAM mpParm2 )
{
   switch ( ulMsg ) {
   case WM_INITDLG:
      {
         CHAR        achFont [16] ;
         USHORT      usIndex ;
         CHAR        achMessage [64] ;

         //--------------------------------------------------------
         // Set the size of the tick marks
         //--------------------------------------------------------

         WinSendDlgItemMsg ( hwndDlg,
                             IDS_SLIDER,
                             SLM_SETTICKSIZE,
                             MPFROM2SHORT ( SMA_SETALLTICKS, 7 ) ,
                             0 ) ;

         strcpy ( achFont, "8.Tms Rmn" ) ;
         WinSetPresParam ( WinWindowFromID ( hwndDlg, IDS_SLIDER ),
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
         CopyFile ( WinWindowFromID ( hwndDlg, IDS_SLIDER )) ;

         WinAlarm ( HWND_DESKTOP, WA_NOTE ) ;
         WinMessageBox ( HWND_DESKTOP,
                         hwndDlg,
                         "Backup is Complete",
                         "Status",
                         0,
                         MB_OK | MB_INFORMATION ) ;
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

         poiItem = (POWNERITEM) PVOIDFROMMP ( mpParm2 ) ;

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

   default:
      return WinDefDlgProc ( hwndDlg,
                             ulMsg,
                             mpParm1,
                             mpParm2 ) ;
   } /* endswitch */

   return MRFROMSHORT ( FALSE ) ;
}

BOOL CopyFile ( HWND hwndSlider )
{
   APIRET         arRc ;
   FILESTATUS3    fsStatus ;
   PBYTE          pbBuffer ;
   HFILE          hfRead ;
   ULONG          ulAction ;
   HFILE          hfWrite ;
   ULONG          ulSzBlock ;
   USHORT         usIndex ;
   ULONG          ulBytesRead ;
   ULONG          ulBytesWritten ;

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