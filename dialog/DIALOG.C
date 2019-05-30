#define INCL_WIN

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dialog.h"

#define CLS_CLIENT               "MyClass"

MRESULT EXPENTRY ClientWndProc ( HWND hwndWnd,
                               ULONG ulMsg,
                               MPARAM mpParm1,
                               MPARAM mpParm2 ) ;

MRESULT EXPENTRY DlgProc ( HWND hwndWnd,
                           ULONG ulMsg,
                           MPARAM mpParm1,
                           MPARAM mpParm2 ) ;

INT main ( VOID )
{
   HAB          habAnchor ;
   HMQ          hmqQueue ;
   ULONG        ulFlags ;
   HWND         hwndFrame ;
   HWND         hwndClient ;
   QMSG         qmMsg ;
   BOOL         bLoop ;

   habAnchor = WinInitialize ( 0 ) ;
   hmqQueue = WinCreateMsgQueue ( habAnchor, 0 ) ;

   WinRegisterClass ( habAnchor,
                      CLS_CLIENT,
                      ClientWndProc,
                      CS_SIZEREDRAW,
                      0 ) ;

   ulFlags = FCF_STANDARD & ~FCF_SHELLPOSITION & ~FCF_ACCELTABLE &
              ~FCF_TASKLIST ;

   hwndFrame = WinCreateStdWindow ( HWND_DESKTOP,
                                    0,
                                    &ulFlags,
                                    CLS_CLIENT,
                                    "Dialog Box Example",
                                    0,
                                    NULLHANDLE,
                                    IDR_CLIENT,
                                    &hwndClient ) ;

   if ( hwndFrame != 0 ) {
      WinSetWindowPos ( hwndFrame,
                        NULLHANDLE,
                        50,
                        50,
                        250,
                        150,
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

MRESULT EXPENTRY DlgProc ( HWND hwndWnd,
                           ULONG ulMsg,
                           MPARAM mpParm1,
                           MPARAM mpParm2 )
{
   switch ( ulMsg ) {

   case WM_INITDLG:
      {
         BOOL bModal ;
         CHAR achMessage [64] ;

         bModal = *(( PBOOL ) PVOIDFROMMP ( mpParm2 )) ;

         WinSetWindowPos ( hwndWnd,
                           NULLHANDLE,
                           10,
                           ( bModal?100:150 ) ,
                           0,
                           0,
                           SWP_MOVE ) ;

         sprintf ( achMessage,
                 "I'm a %s dialog box",
                 ( bModal? ( "modal" ) : ( "modeless" )) ) ;

         WinSetDlgItemText ( hwndWnd,
                             IDT_DIALOGNAME,
                             achMessage ) ;

         if ( bModal ) {
            strcpy ( achMessage,
                     "Try and click on the main window" ) ;
         } else {
            strcpy ( achMessage, "Click on the main window" ) ;
         } /* endif */

         WinSetDlgItemText ( hwndWnd, IDT_CLICK, achMessage ) ;
      }
      break ;
   case WM_COMMAND:

      switch ( SHORT1FROMMP ( mpParm1 )) {
         case DID_OK:
         case DID_CANCEL:
            WinDismissDlg ( hwndWnd, FALSE ) ;
            break ;

         default:
            return WinDefDlgProc ( hwndWnd,
                                   ulMsg,
                                   mpParm1,
                                   mpParm2 ) ;
      } /* endswitch */
      break ;

   default:
      return WinDefDlgProc ( hwndWnd, ulMsg, mpParm1, mpParm2 ) ;
   } /* endswitch */

   return MRFROMSHORT ( FALSE ) ;
}

MRESULT EXPENTRY ClientWndProc ( HWND hwndWnd,
                               ULONG ulMsg,
                               MPARAM mpParm1,
                               MPARAM mpParm2 )
{
   switch ( ulMsg ) {

   case WM_ERASEBACKGROUND:
      return MRFROMSHORT ( TRUE ) ;

   case WM_COMMAND:

      switch ( SHORT1FROMMP ( mpParm1 )) {
      case IDM_MODELESS:
         {
            BOOL bModal ;

            bModal = FALSE ;

            WinLoadDlg ( HWND_DESKTOP,
                         hwndWnd,
                         DlgProc,
                         NULLHANDLE,
                         IDD_EXAMPLE,
                         ( PVOID ) &bModal ) ;
         }
         break ;

      case IDM_MODAL:
         {
            BOOL bModal ;

            bModal = TRUE ;

            WinDlgBox ( HWND_DESKTOP,
                        hwndWnd,
                        DlgProc,
                        NULLHANDLE,
                        IDD_EXAMPLE,
                        ( PVOID ) &bModal ) ;
         }
         break ;

      case IDM_EXIT:
         WinPostMsg ( hwndWnd, WM_CLOSE, 0, 0 ) ;
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

