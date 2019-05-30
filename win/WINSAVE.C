#define INCL_WIN
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLS_CLIENT               "MyClass"

#define SAVE_NAME                "WINSAVE"
#define SAVE_KEY                 "WINDOWPOS"

MRESULT EXPENTRY ClientWndProc ( HWND hwndWnd,
                                 ULONG ulMsg,
                                 MPARAM mpParm1,
                                 MPARAM mpParm2 ) ;

INT main ( VOID )
{
   HAB         habAnchor ;
   HMQ         hmqQueue ;
   ULONG       ulFlags ;
   HWND        hwndFrame ;
   HWND        hwndClient ;
   BOOL        bReturn ;
   BOOL        bLoop ;
   QMSG        qmMsg ;

   habAnchor = WinInitialize ( 0 ) ;
   hmqQueue = WinCreateMsgQueue ( habAnchor, 0 ) ;

   WinRegisterClass ( habAnchor,
                      CLS_CLIENT,
                      ClientWndProc,
                      CS_SIZEREDRAW,
                      0 ) ;

   ulFlags = FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER |
             FCF_MINMAX | FCF_TASKLIST ;

   hwndFrame = WinCreateStdWindow ( HWND_DESKTOP,
                                    WS_VISIBLE,
                                    &ulFlags,
                                    CLS_CLIENT,
                                    "Titlebar",
                                    0L,
                                    NULLHANDLE,
                                    0,
                                    &hwndClient ) ;

   if ( hwndFrame != NULLHANDLE ) {
      bReturn = WinRestoreWindowPos ( SAVE_NAME,
                                      SAVE_KEY,
                                      hwndFrame ) ;

      if ( bReturn ) {
         WinSetWindowPos ( hwndFrame,
                           HWND_TOP,
                           0,
                           0,
                           0,
                           0,
                           SWP_ACTIVATE | SWP_SHOW ) ;
      } else {
         WinSetWindowPos ( hwndFrame,
                           NULLHANDLE,
                           10,
                           10,
                           400,
                           300,
                           SWP_ACTIVATE |
                           SWP_MOVE |
                           SWP_SIZE |
                           SWP_SHOW ) ;
      } /* endif */

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
   return 0 ;
}

MRESULT EXPENTRY ClientWndProc ( HWND hwndWnd,
                                 ULONG ulMsg,
                                 MPARAM mpParm1,
                                 MPARAM mpParm2 )
{
   switch ( ulMsg ) {

   case WM_ERASEBACKGROUND:
      return MRFROMSHORT ( TRUE ) ;

   case WM_SAVEAPPLICATION:
      WinStoreWindowPos ( SAVE_NAME,
                          SAVE_KEY,
                          WinQueryWindow ( hwndWnd, QW_PARENT )) ;
      break ;

   default:
      return WinDefWindowProc ( hwndWnd,
                                ulMsg,
                                mpParm1,
                                mpParm2 ) ;
   } /* endswitch */

   return MRFROMSHORT ( FALSE ) ;
}
