#define INCL_GPICONTROL
#define INCL_GPILCIDS
#define INCL_WIN

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "notebook.h"

#define WM_LOADDLG               (WM_USER + 1)
#define CLS_CLIENT               "MyClass"

BOOL InitializeNotebook ( HWND hwndNotebook,
                          HWND hwndPage1,
                          HWND hwndPage2,
                          LONG lCxChar,
                          LONG lCyChar ) ;

MRESULT EXPENTRY DlgProc ( HWND hwndDlg,
                           ULONG ulMsg,
                           MPARAM mpParm1,
                           MPARAM mpParm2 ) ;

MRESULT EXPENTRY ClientWndProc ( HWND hwndWnd,
                                 ULONG ulMsg,
                                 MPARAM mpParm1,
                                 MPARAM mpParm2 ) ;

INT main ( VOID )
{
   HAB      habAnchor ;
   HMQ      hmqQueue ;
   ULONG    ulFlags ;
   HWND     hwndFrame ;
   HWND     hwndClient ;
   BOOL     bLoop ;
   QMSG     qmMsg ;

   habAnchor = WinInitialize ( 0 ) ;
   hmqQueue = WinCreateMsgQueue ( habAnchor, 0 ) ;

   WinRegisterClass ( habAnchor,
                      CLS_CLIENT,
                      ClientWndProc,
                      CS_SIZEREDRAW,
                      0 ) ;

   ulFlags = FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER |
             FCF_SHELLPOSITION ;

   hwndFrame = WinCreateStdWindow ( HWND_DESKTOP,
                                    0,
                                    &ulFlags,
                                    CLS_CLIENT,
                                    "Notebook Example",
                                    0,
                                    NULLHANDLE,
                                    0,
                                    &hwndClient ) ;

   if ( hwndFrame != 0 ) {
      WinSetWindowPos ( hwndFrame,
                        NULLHANDLE,
                        50,
                        50,
                        550,
                        400,
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
         HPS            hpsChar ;
         FONTMETRICS    fmMetrics ;
         LONG           lCxChar ;
         LONG           lCyChar ;
         HWND           hwndNotebook ;
         HWND           hwndPage1 ;
         HWND           hwndPage2 ;

         hpsChar = WinGetPS ( hwndWnd ) ;
         GpiQueryFontMetrics ( hpsChar,
                               sizeof ( fmMetrics ) ,
                               &fmMetrics ) ;
         WinReleasePS ( hpsChar ) ;

         lCxChar = fmMetrics.lAveCharWidth ;
         lCyChar = fmMetrics.lMaxBaselineExt ;

         hwndNotebook = WinCreateWindow ( hwndWnd,
                                          WC_NOTEBOOK,
                                          "",
                                          BKS_SPIRALBIND |
                                          BKS_SQUARETABS |
                                          BKS_STATUSTEXTCENTER,
                                          0,
                                          0,
                                          0,
                                          0,
                                          hwndWnd,
                                          HWND_TOP,
                                          ID_NOTEBOOK,
                                          NULL,
                                          NULL ) ;

         hwndPage1 = WinLoadDlg ( hwndWnd,
                                  hwndWnd,
                                  DlgProc,
                                  NULLHANDLE,
                                  IDD_PERSONAL,
                                  NULL ) ;

         hwndPage2 = WinLoadDlg ( hwndWnd,
                                  hwndWnd,
                                  DlgProc,
                                  NULLHANDLE,
                                  IDD_OS2,
                                  NULL ) ;

         InitializeNotebook ( hwndNotebook,
                              hwndPage1,
                              hwndPage2,
                              lCxChar,
                              lCyChar ) ;

         WinSetFocus ( HWND_DESKTOP,
                       WinWindowFromID ( hwndPage1,
                                         IDE_NAME )) ;
      }
      break ;

   case WM_SIZE:
      WinSetWindowPos ( WinWindowFromID ( hwndWnd, ID_NOTEBOOK ) ,
                        NULLHANDLE,
                        0,
                        0,
                        SHORT1FROMMP ( mpParm2 ) ,
                        SHORT2FROMMP ( mpParm2 ) ,
                        SWP_SIZE | SWP_SHOW ) ;
      break ;

   case WM_CONTROL:
      switch ( SHORT1FROMMP ( mpParm1 )) {
      case ID_NOTEBOOK:
         switch ( SHORT2FROMMP ( mpParm1 )) {
         case BKN_PAGESELECTED:
            {
               PPAGESELECTNOTIFY       ppsnSelect ;
               HWND                    hwndPage ;
               USHORT                  usDlgId ;

               ppsnSelect =
                  (PPAGESELECTNOTIFY) PVOIDFROMMP ( mpParm2 ) ;

               hwndPage =
                  (HWND) PVOIDFROMMR (
                     WinSendMsg ( ppsnSelect -> hwndBook,
                                  BKM_QUERYPAGEWINDOWHWND,
                                  MPFROMLONG (
                                  ppsnSelect -> ulPageIdNew ) ,
                                  0 )) ;

               usDlgId = WinQueryWindowUShort ( hwndPage,
                                                QWS_ID ) ;

               if ( usDlgId == IDD_PERSONAL ) {
                  WinSetFocus ( HWND_DESKTOP,
                                WinWindowFromID ( hwndPage,
                                                  IDE_NAME )) ;
               } else {
                  WinSetFocus ( HWND_DESKTOP,
                                WinWindowFromID ( hwndPage,
                                                  IDE_TEAMOS2 )) ;
               } /* endif */
            }
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
      break ;

   case WM_ERASEBACKGROUND:
      return MRFROMSHORT ( TRUE ) ;

   case WM_PAINT:
      {
         HPS hpsPaint ;

         hpsPaint = WinBeginPaint ( hwndWnd,
                                    NULLHANDLE,
                                    NULLHANDLE ) ;
         GpiErase ( hpsPaint ) ;
         WinEndPaint ( hpsPaint ) ;
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

MRESULT EXPENTRY DlgProc ( HWND hwndDlg,
                           ULONG ulMsg,
                           MPARAM mpParm1,
                           MPARAM mpParm2 )
{
   switch ( ulMsg ) {

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

BOOL InitializeNotebook ( HWND hwndNotebook,
                          HWND hwndPage1,
                          HWND hwndPage2,
                          LONG lCxChar,
                          LONG lCyChar )
{
   ULONG       ulIdPage1 ;
   ULONG       ulIdPage2 ;
   ULONG       ulWidth ;
   CHAR        achPage1Text [64] ;
   CHAR        achPage2Text [64] ;

   ulIdPage1 =
      LONGFROMMR ( WinSendMsg ( hwndNotebook,
                                BKM_INSERTPAGE,
                                0,
                                MPFROM2SHORT ( BKA_MAJOR |
                                               BKA_STATUSTEXTON,
                                               BKA_FIRST ))) ;

   ulIdPage2 =
      LONGFROMMR ( WinSendMsg ( hwndNotebook,
                                BKM_INSERTPAGE,
                                0,
                                MPFROM2SHORT ( BKA_MAJOR |
                                               BKA_STATUSTEXTON,
                                               BKA_LAST ))) ;

   WinSendMsg ( hwndNotebook,
                BKM_SETSTATUSLINETEXT,
                MPFROMLONG ( ulIdPage1 ) ,
                MPFROMP ( "Personal Information for This User" )) ;

   WinSendMsg ( hwndNotebook,
                BKM_SETSTATUSLINETEXT,
                MPFROMLONG ( ulIdPage2 ) ,
                MPFROMP ( "TEAM OS / 2 Information"
                  " for this Location" )) ;

   strcpy ( achPage1Text, " ~Personal" ) ;
   strcpy ( achPage2Text, " ~TEAMOS2" ) ;

   ulWidth = ( max ( strlen ( achPage1Text ) ,
      strlen ( achPage2Text )) + 6 ) * lCxChar ;

   WinSendMsg ( hwndNotebook,
                BKM_SETDIMENSIONS,
                MPFROM2SHORT ( ulWidth, lCyChar * 2 ) ,
                MPFROMSHORT ( BKA_MAJORTAB )) ;

   WinSendMsg ( hwndNotebook,
                BKM_SETTABTEXT,
                MPFROMLONG ( ulIdPage1 ) ,
                MPFROMP ( achPage1Text )) ;

   WinSendMsg ( hwndNotebook,
                BKM_SETTABTEXT,
                MPFROMLONG ( ulIdPage2 ) ,
                MPFROMP ( achPage2Text )) ;

   WinSendMsg ( hwndNotebook,
                BKM_SETPAGEWINDOWHWND,
                MPFROMLONG ( ulIdPage1 ) ,
                MPFROMHWND ( hwndPage1 )) ;

   WinSendMsg ( hwndNotebook,
                BKM_SETPAGEWINDOWHWND,
                MPFROMLONG ( ulIdPage2 ) ,
                MPFROMHWND ( hwndPage2 )) ;

   WinSendMsg ( hwndNotebook,
                BKM_SETNOTEBOOKCOLORS,
                MPFROMLONG ( CLR_BLUE ) ,
                MPFROMSHORT ( BKA_FOREGROUNDMAJORCOLORINDEX )) ;

   return TRUE ;
}
