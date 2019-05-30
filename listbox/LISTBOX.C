#define INCL_WIN
#define INCL_GPI

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "listbox.h"

#define USRM_LOADDLG                ( WM_USER + 1 )

typedef struct _BITMAPDATA {
   CHAR   achName [20] ;
   USHORT usNumber ;
} BITMAPDATA, * PBITMAPDATA ;

#define MAX_BITMAPS              9

BITMAPDATA abdBitmaps [MAX_BITMAPS] = {
   "SBMP_CHILDSYSMENU",      SBMP_CHILDSYSMENU,
   "SBMP_MAXBUTTON",         SBMP_MAXBUTTON,
   "SBMP_MENUATTACHED",      SBMP_MENUATTACHED,
   "SBMP_MINBUTTON",         SBMP_MINBUTTON,
   "SBMP_PROGRAM",           SBMP_PROGRAM,
   "SBMP_RESTOREBUTTON",     SBMP_RESTOREBUTTON,
   "SBMP_SIZEBOX",           SBMP_SIZEBOX,
   "SBMP_SYSMENU",           SBMP_SYSMENU,
   "SBMP_TREEMINUS",         SBMP_TREEMINUS
} ;

#define CLS_CLIENT               "MyClass"

MRESULT EXPENTRY ClientWndProc ( HWND hwndWnd,
                                 ULONG ulMsg,
                                 MPARAM mpParm1,
                                 MPARAM mpParm2 ) ;


BOOL Draw1Bitmap ( HPS hpsDraw,
                   HBITMAP hbmBitmap,
                   PRECTL prclDest ) ;


MRESULT EXPENTRY DlgProc ( HWND hwndWnd,
                           ULONG ulMsg,
                           MPARAM mpParm1,
                           MPARAM mpParm2 ) ;

INT main ( VOID )
{
   HMQ        hmqQueue ;
   HAB        habAnchor ;
   ULONG      ulFlags ;
   HWND       hwndFrame ;
   HWND       hwndClient ;
   BOOL       bLoop ;
   QMSG       qmMsg ;

   habAnchor = WinInitialize ( 0 ) ;
   hmqQueue = WinCreateMsgQueue ( habAnchor, 0 ) ;

   WinRegisterClass ( habAnchor,
                      CLS_CLIENT,
                      ClientWndProc,
                      CS_SIZEREDRAW | CS_SYNCPAINT,
                      0 ) ;

   ulFlags = FCF_TITLEBAR | FCF_SYSMENU |
             FCF_SIZEBORDER | FCF_MINMAX ;

   hwndFrame = WinCreateStdWindow ( HWND_DESKTOP,
                                    0L,
                                    &ulFlags,
                                    CLS_CLIENT,
                                    "Listbox Example",
                                    0L,
                                    NULLHANDLE,
                                    0,
                                    &hwndClient ) ;

   if ( hwndFrame != NULLHANDLE ) {
      WinSetWindowPos ( hwndFrame,
                        NULLHANDLE,
                        50,
                        50,
                        300,
                        300,
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

BOOL Draw1Bitmap ( HPS hpsDraw, HBITMAP hbmBitmap, PRECTL prclDest )
{
   BITMAPINFOHEADER2       bmihHeader ;
   POINTL                  ptlPoint ;
   BOOL                    bRc ;

   bmihHeader.cbFix = 16 ;
   GpiQueryBitmapInfoHeader ( hbmBitmap, &bmihHeader ) ;

   ptlPoint.x = ( prclDest -> xRight -
                  prclDest -> xLeft -
                  bmihHeader.cx ) / 2 +
                  prclDest -> xLeft ;

   ptlPoint.y = ( prclDest -> yTop -
                  prclDest -> yBottom -
                  bmihHeader.cy ) / 2 +
                  prclDest -> yBottom ;

   bRc = WinDrawBitmap ( hpsDraw,
                         hbmBitmap,
                         NULL,
                         &ptlPoint,
                         0,
                         0,
                         DBM_NORMAL | DBM_IMAGEATTRS ) ;
   return bRc ;
}

MRESULT EXPENTRY DlgProc ( HWND hwndWnd,
                           ULONG ulMsg,
                           MPARAM mpParm1,
                           MPARAM mpParm2 )

{
   switch ( ulMsg ) {

   case WM_INITDLG:
      {
         USHORT usIndex ;

         for ( usIndex = 0 ; usIndex < MAX_BITMAPS ; usIndex ++ ) {
            WinSendDlgItemMsg ( hwndWnd,
                                IDL_LISTBOX,
                                LM_INSERTITEM,
                                MPFROMSHORT ( usIndex ) ,
                                MPFROMP ( "" )) ;
         } /* endfor */

         WinSendDlgItemMsg ( hwndWnd,
                             IDL_LISTBOX,
                             LM_SELECTITEM,
                             MPFROMSHORT ( 0 ) ,
                             MPFROMSHORT ( TRUE )) ;
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

   case WM_MEASUREITEM:
      {
         HPS                    hpsChar ;
         FONTMETRICS            fmMetrics ;
         LONG                   lMaxCy ;
         USHORT                 usIndex ;
         HBITMAP                hbmBitmap ;
         BITMAPINFOHEADER2      bmihHeader ;

         hpsChar = WinGetPS ( hwndWnd ) ;
         GpiQueryFontMetrics ( hpsChar,
                               ( LONG ) sizeof ( fmMetrics ) ,
                               &fmMetrics ) ;
         WinReleasePS ( hpsChar ) ;

         lMaxCy = fmMetrics.lMaxBaselineExt ;

         for ( usIndex = 0 ; usIndex < MAX_BITMAPS ; usIndex ++ ) {
            hbmBitmap = WinGetSysBitmap ( HWND_DESKTOP,
                               abdBitmaps [usIndex].usNumber ) ;

            bmihHeader.cbFix = 16 ;
            GpiQueryBitmapInfoHeader ( hbmBitmap, &bmihHeader ) ;
            lMaxCy = max ( lMaxCy, bmihHeader.cy ) ;

            GpiDeleteBitmap ( hbmBitmap ) ;
         } /* endfor */

         return MRFROMLONG ( lMaxCy + 10 ) ;
      }

   case WM_DRAWITEM:
      {
         POWNERITEM    poiItem ;
         HBITMAP       hbmBitmap ;
         RECTL         rclText ;

         poiItem = ( POWNERITEM ) PVOIDFROMMP ( mpParm2 ) ;

         rclText = poiItem -> rclItem ;
         rclText.xLeft = ( rclText.xRight - rclText.xLeft ) / 7 ;

         WinDrawText ( poiItem -> hps,
                       - 1,
                       abdBitmaps [poiItem -> idItem] .achName,
                       &rclText,
                       poiItem -> fsState?CLR_YELLOW:CLR_BLUE,
                       poiItem -> fsState?CLR_BLUE:CLR_WHITE,
                       DT_LEFT | DT_VCENTER | DT_ERASERECT ) ;

         rclText = poiItem -> rclItem ;
         rclText.xRight = ( rclText.xRight - rclText.xLeft ) / 7 ;

         WinFillRect ( poiItem -> hps, &rclText, CLR_WHITE ) ;

         hbmBitmap = WinGetSysBitmap ( HWND_DESKTOP,
                      abdBitmaps [poiItem -> idItem] .usNumber ) ;

         Draw1Bitmap ( poiItem -> hps, hbmBitmap, &rclText ) ;

         GpiDeleteBitmap ( hbmBitmap ) ;

         poiItem -> fsState = FALSE ;
         poiItem -> fsStateOld = FALSE ;

         return MRFROMSHORT ( TRUE ) ;
      }

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

   case WM_CREATE:
      WinPostMsg ( hwndWnd, USRM_LOADDLG, 0L, 0L ) ;
      break ;

   case WM_PAINT:
      {
         HPS hpsPaint ;

         hpsPaint = WinBeginPaint ( hwndWnd, NULLHANDLE, NULL ) ;
         GpiErase ( hpsPaint ) ;
         WinEndPaint ( hpsPaint ) ;
      }
      break ;

   case WM_ERASEBACKGROUND:
      return MRFROMSHORT ( TRUE ) ;

   case USRM_LOADDLG:
      WinDlgBox ( hwndWnd,
                  hwndWnd,
                  DlgProc,
                  NULLHANDLE,
                  IDD_LISTBOX,
                  NULL ) ;
      WinPostMsg ( hwndWnd, WM_CLOSE, 0L, 0L ) ;
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