#define INCL_WINSTDFILE
#define INCL_WINSYS
#define INCL_WINWINDOWMGR

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filedlg.h"

MRESULT EXPENTRY ClientWndProc ( HWND hwndWnd,
                                 ULONG ulMsg,
                                 MPARAM mpParm1,
                                 MPARAM mpParm2 ) ;

BOOL FindFile ( HWND hwndWnd, CHAR * pchFile ) ;

#define CLS_CLIENT               "MyClass"

INT main ( VOID )
{
   HAB         habAnchor ;
   HMQ         hmqQueue ;
   ULONG       ulFlags ;
   HWND        hwndFrame ;
   HWND        hwndClient ;
   BOOL        bLoop ;
   QMSG        qmMsg ;

   habAnchor = WinInitialize ( 0 ) ;
   hmqQueue = WinCreateMsgQueue ( habAnchor, 0 ) ;

   WinRegisterClass ( habAnchor,
                      CLS_CLIENT,
                      ClientWndProc,
                      CS_SIZEREDRAW,
                      sizeof ( PVOID )) ;

   ulFlags = FCF_TITLEBAR | FCF_SYSMENU | FCF_MENU |
             FCF_MINMAX | FCF_SIZEBORDER | FCF_SHELLPOSITION |
             FCF_TASKLIST ;

   hwndFrame = WinCreateStdWindow ( HWND_DESKTOP,
                                  0,
                                  &ulFlags,
                                  CLS_CLIENT,
                                  "File Dialog Example",
                                  0,
                                  NULLHANDLE,
                                  RES_CLIENT,
                                  &hwndClient ) ;

   if ( hwndFrame != NULLHANDLE ) {

      WinSetWindowPos ( hwndFrame,
                        NULLHANDLE,
                        50,
                        50,
                        500,
                        250,
                        SWP_SIZE |
                        SWP_MOVE |
                        SWP_ACTIVATE |
                        SWP_SHOW ) ;

      bLoop = WinGetMsg ( habAnchor, &qmMsg, NULLHANDLE, 0, 0 ) ;

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
   return 0 ;
}

MRESULT EXPENTRY ClientWndProc ( HWND hwndWnd,
                                 ULONG ulMsg,
                                 MPARAM mpParm1,
                                 MPARAM mpParm2 )
{
   PCHAR pchFile ;

   pchFile = WinQueryWindowPtr ( hwndWnd, 0 ) ;

   switch ( ulMsg ) {

   case WM_CREATE:
      pchFile = ( PCHAR ) malloc ( CCHMAXPATH ) ;
      if ( pchFile == NULL ) {
         WinAlarm ( HWND_DESKTOP, WA_ERROR ) ;
         WinMessageBox ( HWND_DESKTOP,
                         hwndWnd,
                         "No memory could be allocated !",
                         "Error",
                         0,
                         MB_INFORMATION | MB_OK ) ;
         return MRFROMSHORT ( TRUE ) ;
      } /* endif */

      WinSetWindowPtr ( hwndWnd, 0, pchFile ) ;
      pchFile [0] = 0 ;
      break ;

   case WM_DESTROY:
      if ( pchFile != NULL ) {
         free ( pchFile ) ;
      } /* endif */
      break ;

   case WM_PAINT:
      {
         HPS   hpsPaint ;
         RECTL rclInvalid ;
         CHAR  achText [CCHMAXPATH] ;

         hpsPaint = WinBeginPaint ( hwndWnd,
                                    NULLHANDLE,
                                    &rclInvalid ) ;

         WinFillRect ( hpsPaint, &rclInvalid, SYSCLR_WINDOW ) ;

         if ( pchFile [0] != 0 ) {

            WinQueryWindowRect ( hwndWnd, &rclInvalid ) ;
            sprintf ( achText,
                      "You have selected file %s",
                      pchFile ) ;
            WinDrawText ( hpsPaint,
                          - 1,
                          achText,
                          &rclInvalid,
                          0,
                          0,
                          DT_CENTER | DT_VCENTER | DT_TEXTATTRS ) ;
         } /* endif */

         WinEndPaint ( hpsPaint ) ;
      }
      break ;

   case WM_COMMAND:

      switch ( SHORT1FROMMP ( mpParm1 )) {

      case IDM_OPEN:
         if ( pchFile ) {
            FindFile ( hwndWnd, pchFile ) ;
         } /* endif */

         WinInvalidateRect ( hwndWnd, NULL, TRUE ) ;
         WinUpdateWindow ( hwndWnd ) ;
         break ;

      case IDM_EXIT:
         WinPostMsg ( hwndWnd, WM_QUIT, 0, 0 ) ;
         break ;

      default:
         return WinDefWindowProc ( hwndWnd,
                                   ulMsg,
                                   mpParm1,
                                   mpParm2 ) ;
      } /* endswitch */
      break ;

   default:
      return WinDefWindowProc ( hwndWnd,
                                ulMsg,
                                mpParm1,
                                mpParm2 ) ;
   } /* endswitch */

   return MRFROMSHORT ( FALSE ) ;
}

BOOL FindFile ( HWND hwndWnd, CHAR * pchFile )
{
   FILEDLG      fdFileDlg ;

   memset ( &fdFileDlg, 0, sizeof ( FILEDLG )) ;

   fdFileDlg.cbSize = sizeof ( FILEDLG ) ;
   fdFileDlg.fl = FDS_CENTER | FDS_PRELOAD_VOLINFO |
                  FDS_OPEN_DIALOG ;

   if ( WinFileDlg ( HWND_DESKTOP,
                     hwndWnd,
                     &fdFileDlg ) != DID_OK ) {
      WinAlarm ( HWND_DESKTOP, WA_ERROR ) ;
      return FALSE ;
   } /* endif */

   strcpy ( pchFile, fdFileDlg.szFullFile ) ;
   return TRUE ;
}
