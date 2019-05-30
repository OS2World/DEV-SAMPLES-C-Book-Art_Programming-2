#define INCL_WIN
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "value.h"

#define CLS_CLIENT               "MyClass"
#define USRM_LOADDLG             ( WM_USER )

static LONG alColors [] = {
   CLR_BLUE,
   CLR_PINK,
   CLR_GREEN,
   CLR_CYAN,
   CLR_YELLOW,
   CLR_NEUTRAL,
   CLR_DARKGRAY,
   CLR_DARKBLUE,
   CLR_DARKRED,
   CLR_DARKPINK,
   CLR_DARKGREEN,
   CLR_DARKCYAN
} ;

typedef struct {
   SHORT       sColor ;
   HWND        hwndDlg ;
} WNDDATA, * PWNDDATA ;

MRESULT EXPENTRY DlgProc ( HWND hwndDlg,
                           ULONG  ulMsg,
                           MPARAM mpParm1,
                           MPARAM mpParm2 ) ;

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
   BOOL        bLoop ;
   QMSG        qmMsg ;

   habAnchor = WinInitialize ( 0 ) ;
   hmqQueue = WinCreateMsgQueue ( habAnchor, 0 ) ;

   WinRegisterClass ( habAnchor,
                      CLS_CLIENT,
                      ClientWndProc,
                      CS_SIZEREDRAW | CS_CLIPCHILDREN,
                      sizeof ( PVOID )) ;

   ulFlags = FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER | FCF_MINMAX ;

   hwndFrame = WinCreateStdWindow ( HWND_DESKTOP,
                                    0,
                                    &ulFlags,
                                    CLS_CLIENT,
                                    "Value Set Example",
                                    0,
                                    NULLHANDLE,
                                    0,
                                    NULL ) ;

   if ( hwndFrame != ( HWND ) NULL ) {
      WinSetWindowPos ( hwndFrame,
                        NULLHANDLE,
                        50,
                        50,
                        400,
                        250,
                        SWP_SIZE |
                        SWP_MOVE |
                        SWP_ACTIVATE |
                        SWP_SHOW ) ;

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
                                 ULONG  ulMsg,
                                 MPARAM mpParm1,
                                 MPARAM mpParm2 )
{
   PWNDDATA       pwdData ;

   pwdData = WinQueryWindowPtr ( hwndWnd, 0 ) ;

   switch ( ulMsg ) {

   case WM_CREATE:
      pwdData = (PWNDDATA) malloc ( sizeof ( WNDDATA )) ;
      if ( pwdData == NULL ) {
         WinAlarm ( HWND_DESKTOP, WA_ERROR ) ;
         return MRFROMSHORT ( TRUE ) ;
      } /* endif */

      WinSetWindowPtr ( hwndWnd, 0, pwdData ) ;

      pwdData -> sColor = - 1 ;

      WinPostMsg ( hwndWnd, USRM_LOADDLG, 0, 0 ) ;
      break ;

   case WM_DESTROY:
      WinDestroyWindow ( pwdData -> hwndDlg ) ;
      free ( pwdData ) ;
      break ;

   case WM_ERASEBACKGROUND:
      return MRFROMSHORT ( TRUE ) ;

   case WM_PAINT:
      {
         RECTL        rclWnd ;
         HPS          hpsPaint ;
         RECTL        rclPaint ;
         CHAR         achMsg [32] ;
         RECTL        rclBox ;

         WinQueryWindowRect ( hwndWnd, &rclWnd ) ;

         hpsPaint = WinBeginPaint ( hwndWnd,
                                    NULLHANDLE,
                                    &rclPaint ) ;
         GpiErase ( hpsPaint ) ;

         if ( pwdData -> sColor != - 1 ) {

            rclBox.xLeft = rclWnd.xRight / 8 * 6 ;
            rclBox.xRight = rclWnd.xRight / 16 * 15 ;
            rclBox.yTop = rclWnd.yTop / 8 * 7 ;
            rclBox.yBottom = rclWnd.yTop / 8 * 4 ;

            WinFillRect ( hpsPaint,
                          &rclBox,
                          alColors [pwdData -> sColor] ) ;

            sprintf ( achMsg,
                      "Color: %d",
                      pwdData -> sColor + 1 ) ;

            rclBox.xLeft = rclWnd.xRight / 8 * 6 ;
            rclBox.xRight = rclWnd.xRight / 16 * 15 ;
            rclBox.yBottom = rclWnd.yTop / 8 * 4 ;
            rclBox.yTop = rclWnd.yTop / 8 * 5 ;

            WinDrawText ( hpsPaint,
                          strlen ( achMsg ) ,
                          achMsg,
                          &rclBox,
                          0,
                          0,
                          DT_CENTER | DT_VCENTER | DT_TEXTATTRS |
                          DT_ERASERECT ) ;
         } /* endif */

         WinEndPaint ( hpsPaint ) ;
      }
      break ;

   case USRM_LOADDLG:
      pwdData -> hwndDlg = WinLoadDlg ( hwndWnd,
                                        hwndWnd,
                                        DlgProc,
                                        NULLHANDLE,
                                        IDD_VALUE,
                                        NULL ) ;
      break ;

   default:
      return WinDefWindowProc ( hwndWnd,
                                ulMsg,
                                mpParm1,
                                mpParm2 ) ;
   } /* endswitch */

   return MRFROMSHORT ( FALSE ) ;
}

MRESULT EXPENTRY DlgProc ( HWND hwndDlg,
                           ULONG ulMsg,
                           MPARAM mpParm1,
                           MPARAM mpParm2 )
{
   PWNDDATA pwdData ;

   pwdData = WinQueryWindowPtr ( WinQueryWindow ( hwndDlg,
                                                  QW_PARENT ) ,
                                 0 ) ;

   switch ( ulMsg ) {
   case WM_INITDLG:
      {
         SHORT       sColor ;
         USHORT      usX ;
         USHORT      usY ;
         HWND        hwndParent ;

         sColor = 0 ;

         for ( usX = 1 ; usX <= 3 ; usX ++ ) {
            for ( usY = 1 ; usY <= 4 ; usY ++ ) {
               WinSendDlgItemMsg ( hwndDlg,
                                   IDV_VALUE,
                                   VM_SETITEM,
                                   MPFROM2SHORT ( usX, usY ) ,
                                   MPFROMLONG (
                                    alColors [sColor ++ ] )) ;
            } /* endfor */
         } /* endfor */

         WinSendDlgItemMsg ( hwndDlg,
                             IDV_VALUE,
                             VM_SELECTITEM,
                             MPFROM2SHORT ( 1, 1 ) ,
                             NULL ) ;

         pwdData -> sColor = 0 ;
         hwndParent = WinQueryWindow ( hwndDlg, QW_PARENT ) ;
         WinInvalidateRect ( hwndParent, NULL, FALSE ) ;
         WinUpdateWindow ( hwndParent ) ;
      }
      break ;

   case WM_CONTROL:
      switch ( SHORT1FROMMP ( mpParm1 )) {
      case IDV_VALUE:
         switch ( SHORT2FROMMP ( mpParm1 )) {
         case VN_SELECT:
            {
               MRESULT     mrReply ;
               HWND        hwndParent ;

               mrReply = WinSendDlgItemMsg ( hwndDlg,
                                             IDV_VALUE,
                                             VM_QUERYSELECTEDITEM,
                                             0,
                                             0 ) ;

               pwdData -> sColor = ( SHORT1FROMMR ( mrReply ) - 1 )
                  * 4 + SHORT2FROMMR ( mrReply ) - 1 ;

               hwndParent = WinQueryWindow ( hwndDlg, QW_PARENT ) ;
               WinInvalidateRect ( hwndParent, NULL, FALSE ) ;
               WinUpdateWindow ( hwndParent ) ;
            }
            break ;

         default:
            return WinDefDlgProc ( hwndDlg,
                                   ulMsg,
                                   mpParm1,
                                   mpParm2 ) ;
         } /* endswitch */
         break ;

      default:
         return WinDefDlgProc ( hwndDlg,
                                ulMsg,
                                mpParm1,
                                mpParm2 ) ;
      } /* endswitch */
      break ;

   case WM_SYSCOMMAND:
      switch ( SHORT1FROMMP ( mpParm1 )) {

      case SC_CLOSE:
         WinPostMsg ( WinQueryWindow ( hwndDlg, QW_PARENT ) ,
                      WM_CLOSE,
                      0,
                      0 ) ;
         break ;

      default:
         return WinDefDlgProc ( hwndDlg,
                                ulMsg,
                                mpParm1,
                                mpParm2 ) ;
      } /* endswitch */
      break ;

   case WM_COMMAND:
      switch ( SHORT1FROMMP ( mpParm1 )) {
      case DID_OK:
      case DID_CANCEL:
         break ;

      default:
         return WinDefDlgProc ( hwndDlg,
                                ulMsg,
                                mpParm1,
                                mpParm2 ) ;
      } /* endswitch */
      break ;

   default:
      return WinDefDlgProc ( hwndDlg,
                             ulMsg,
                             mpParm1,
                             mpParm2 ) ;
   } /* endswitch */

   return MRFROMSHORT ( FALSE ) ;
}
