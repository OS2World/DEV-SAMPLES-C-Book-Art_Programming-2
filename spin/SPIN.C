#define INCL_WIN
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include "spin.h"

#define CLS_CLIENT               "MyClass"

PCHAR achMonthArray [] = {
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December"
} ;

PCHAR achDayArray [] = {
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "10",
    "11",
    "12",
    "13",
    "14",
    "15",
    "16",
    "17",
    "18",
    "19",
    "20",
    "21",
    "22",
    "23",
    "24",
    "25",
    "26",
    "27",
    "28",
    "29",
    "30",
    "31"
} ;

PCHAR achYearArray [] = {
    "1990",
    "1991",
    "1992",
    "1993",
    "1994",
    "1995",
    "1996",
    "1997",
    "1998",
    "1999",
    "2000"
} ;

MRESULT EXPENTRY ClientWndProc ( HWND hwndWnd,
                                 ULONG ulMsg,
                                 MPARAM mpParm1,
                                 MPARAM mpParm2 ) ;

INT main ( VOID )
{
   HAB         habAnchor ;
   HMQ         hmqQueue ;
   ULONG       ulFlags ;
   LONG        lHeight ;
   LONG        lWidth ;
   HWND        hwndFrame ;
   HWND        hwndClient ;
   BOOL        bLoop ;
   QMSG        qmMsg ;

   habAnchor = WinInitialize ( 0 ) ;
   hmqQueue = WinCreateMsgQueue ( habAnchor, 0 ) ;

   WinRegisterClass ( habAnchor,
                      CLS_CLIENT,
                      ClientWndProc,
                      CS_SIZEREDRAW | CS_SYNCPAINT,
                      0 ) ;

   ulFlags = FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER |
             FCF_MINMAX | FCF_ACCELTABLE ;

   hwndFrame = WinCreateStdWindow ( HWND_DESKTOP,
                                    0,
                                    &ulFlags,
                                    CLS_CLIENT,
                                    "Spin Button Example",
                                    0,
                                    NULLHANDLE,
                                    ID_WINDOW,
                                    &hwndClient ) ;

   if ( hwndFrame != NULLHANDLE ) {

      lHeight = WinQuerySysValue ( HWND_DESKTOP, SV_CYSCREEN ) ;
      lWidth  = WinQuerySysValue ( HWND_DESKTOP, SV_CXSCREEN ) ;

      WinSetWindowPos ( hwndFrame,
                        NULLHANDLE,
                        lWidth  / 4,
                        lHeight / 4,
                        lWidth / 2,
                        lHeight / 2,
                        SWP_SIZE | SWP_MOVE |
                        SWP_ACTIVATE | SWP_SHOW ) ;

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

   case WM_CREATE:
      {
         ULONG       ulMonthStyle ;
         ULONG       ulDayStyle ;
         ULONG       ulYearStyle ;
         LONG        lWidth ;
         LONG        lHeight ;
         LONG        xPosition ;
         LONG        yPosition ;
         LONG        yHeight ;

         ulMonthStyle = SPBS_SERVANT | SPBS_READONLY |
                        SPBS_JUSTLEFT | SPBS_FASTSPIN |
                        WS_VISIBLE ;

         ulDayStyle =   SPBS_SERVANT | SPBS_READONLY |
                        SPBS_JUSTLEFT | SPBS_FASTSPIN |
                        WS_VISIBLE ;

         ulYearStyle =  SPBS_MASTER | SPBS_READONLY |
                        SPBS_JUSTLEFT | SPBS_FASTSPIN |
                        WS_VISIBLE ;

         lHeight = WinQuerySysValue ( HWND_DESKTOP,
                                      SV_CYSCREEN ) / 2 ;
         lWidth  = WinQuerySysValue ( HWND_DESKTOP,
                                      SV_CXSCREEN ) / 2 ;

         xPosition = lWidth / 5 ;
         yPosition = lHeight / 3 ;
         yHeight = 50 ;

         WinCreateWindow ( hwndWnd,
                           WC_SPINBUTTON,
                           NULL,
                           ulMonthStyle,
                           xPosition,
                           yPosition,
                           90,
                           yHeight,
                           hwndWnd,
                           HWND_TOP,
                           ID_SPINBUTTONMONTH,
                           NULL,
                           NULL ) ;

         WinCreateWindow ( hwndWnd,
                           WC_SPINBUTTON,
                           NULL,
                           ulDayStyle,
                           xPosition + 90,
                           yPosition,
                           40,
                           yHeight,
                           hwndWnd,
                           HWND_TOP,
                           ID_SPINBUTTONDAY,
                           NULL,
                           NULL ) ;

         WinCreateWindow ( hwndWnd,
                           WC_SPINBUTTON,
                           NULL,
                           ulYearStyle,
                           xPosition + 90 + 40,
                           yPosition,
                           110,
                           yHeight,
                           hwndWnd,
                           HWND_TOP,
                           ID_SPINBUTTONYEAR,
                           NULL,
                           NULL ) ;

         WinSendDlgItemMsg ( hwndWnd,
                             ID_SPINBUTTONMONTH,
                             SPBM_SETARRAY,
                             MPFROMP ( achMonthArray ) ,
                             MPFROMSHORT ( 12 )) ;

         WinSendDlgItemMsg ( hwndWnd,
                             ID_SPINBUTTONMONTH,
                             SPBM_SETMASTER,
                             MPFROMHWND ( WinWindowFromID (
                                          hwndWnd,
                                          ID_SPINBUTTONYEAR )),
                             0 ) ;

         WinSendDlgItemMsg ( hwndWnd,
                             ID_SPINBUTTONDAY,
                             SPBM_SETARRAY,
                             MPFROMP ( achDayArray ) ,
                             MPFROMSHORT ( 31 ) ) ;

         WinSendDlgItemMsg ( hwndWnd,
                             ID_SPINBUTTONDAY,
                             SPBM_SETMASTER,
                             MPFROMHWND ( WinWindowFromID (
                                          hwndWnd,
                                          ID_SPINBUTTONYEAR )),
                             0 ) ;

         WinSendDlgItemMsg ( hwndWnd,
                             ID_SPINBUTTONYEAR,
                             SPBM_SETARRAY,
                             MPFROMP ( achYearArray ) ,
                             MPFROMSHORT ( 11 ) ) ;

         WinSetFocus ( HWND_DESKTOP,
                       WinWindowFromID ( hwndWnd,
                                         ID_SPINBUTTONMONTH )) ;
      }
      break ;

   case WM_CONTROL:
      {
         USHORT      usID ;
         USHORT      usNotifyCode ;
         RECTL       rclWindow ;

         usID = SHORT1FROMMP ( mpParm1 ) ;
         usNotifyCode = SHORT2FROMMP ( mpParm1 ) ;

         if ( usID == ID_SPINBUTTONDAY ||
            usID == ID_SPINBUTTONMONTH ||
            usID == ID_SPINBUTTONYEAR ) {

            if ( usNotifyCode == SPBN_ENDSPIN ) {

               WinQueryWindowRect ( hwndWnd, &rclWindow ) ;
               rclWindow.yBottom = ( rclWindow.yTop -
                                     rclWindow.yBottom ) / 3 * 2 ;

               WinInvalidateRect ( hwndWnd,
                                   &rclWindow,
                                   FALSE ) ;
            } else {
               return WinDefWindowProc ( hwndWnd,
                                         ulMsg,
                                         mpParm1,
                                         mpParm2 ) ;
            } /* endif */
         } else {
            return WinDefWindowProc ( hwndWnd,
                                      ulMsg,
                                      mpParm1,
                                      mpParm2 ) ;
         } /* endif */
      }
      break ;
   case WM_COMMAND:
      {
         HWND        hwndActive ;
         USHORT      usFocusID ;

         if ( SHORT1FROMMP ( mpParm2 ) == CMDSRC_ACCELERATOR ) {

            hwndActive = WinQueryFocus ( HWND_DESKTOP ) ;
            usFocusID = WinQueryWindowUShort ( hwndActive,
                                               QWS_ID ) ;

            if ( SHORT1FROMMP ( mpParm1 ) == IDK_TAB ) {
               usFocusID ++ ;

               if ( usFocusID > LAST_CONTROL ) {
                   usFocusID = FIRST_CONTROL ;
               } /* endif */

               hwndActive = WinWindowFromID ( hwndWnd,
                                              usFocusID ) ;
               WinSetFocus ( HWND_DESKTOP, hwndActive ) ;

            } else

            if ( SHORT1FROMMP ( mpParm1 ) == IDK_BACKTAB ) {
               usFocusID -- ;

               if ( usFocusID < FIRST_CONTROL ) {
                   usFocusID = LAST_CONTROL ;
               } /* endif */

               hwndActive = WinWindowFromID ( hwndWnd,
                                              usFocusID ) ;
               WinSetFocus ( HWND_DESKTOP, hwndActive ) ;

            } /* endif */
         } /* endif */
      }
      break ;
   case WM_PAINT:
      {
         HPS         hpsPaint ;
         RECTL       rclBox ;
         CHAR        achMonth [15] ;
         CHAR        achDay [3] ;
         CHAR        achYear [5] ;
         CHAR        achMsg [128] ;

         hpsPaint = WinBeginPaint ( hwndWnd,
                                    NULLHANDLE,
                                    NULL ) ;

         WinQueryWindowRect ( hwndWnd, &rclBox ) ;

         rclBox.yBottom = ( rclBox.yTop - rclBox.yBottom ) / 3 *
                            2 ;

         WinFillRect ( hpsPaint, &rclBox, CLR_WHITE ) ;

         WinSendDlgItemMsg ( hwndWnd,
                             ID_SPINBUTTONMONTH,
                             SPBM_QUERYVALUE,
                             MPFROMP ( achMonth ) ,
                             MPFROM2SHORT ( sizeof ( achMonth ) ,
                                SPBQ_DONOTUPDATE )) ;

         WinSendDlgItemMsg ( hwndWnd,
                             ID_SPINBUTTONDAY,
                             SPBM_QUERYVALUE,
                             MPFROMP ( achDay ) ,
                             MPFROM2SHORT ( sizeof ( achDay ) ,
                                SPBQ_DONOTUPDATE )) ;

         WinSendDlgItemMsg ( hwndWnd,
                             ID_SPINBUTTONYEAR,
                             SPBM_QUERYVALUE,
                             MPFROMP ( achYear ) ,
                             MPFROM2SHORT ( sizeof ( achYear ) ,
                                SPBQ_DONOTUPDATE )) ;

         sprintf ( achMsg,
                   "SpinButton's set to: %s %s, %s",
                   achMonth,
                   achDay,
                   achYear ) ;

         WinDrawText ( hpsPaint,
                       -1,
                       achMsg,
                       &rclBox,
                       0,
                       0,
                       DT_CENTER | DT_VCENTER | DT_TEXTATTRS ) ;

         WinEndPaint ( hpsPaint ) ;
      }
      break ;
   case WM_ERASEBACKGROUND:
       return MRFROMSHORT ( TRUE ) ;

   default:
       return WinDefWindowProc ( hwndWnd,
                                 ulMsg,
                                 mpParm1,
                                 mpParm2 ) ;
   } /* endswitch */

   return MRFROMSHORT ( FALSE ) ;
}

