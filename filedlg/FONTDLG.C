#define INCL_WIN
#define INCL_STDDLG
#define INCL_GPI

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fontdlg.h"

typedef struct {
   FONTDLG fdFontDlg ;
   USHORT  bInit ;
} MYFONTINFO, *PMYFONTINFO ;

MRESULT EXPENTRY ClientWndProc ( HWND hwndWnd,
                                 ULONG ulMsg,
                                 MPARAM mpParm1,
                                 MPARAM mpParm2 ) ;

VOID SetFont ( HWND hwndWnd, PMYFONTINFO pmfiFont ) ;

#define CLS_CLIENT               "MyClass"

INT main ( VOID )
{
   HAB   habAnchor ;
   HMQ   hmqQueue ;
   ULONG ulFlags ;
   HWND  hwndFrame ;
   HWND  hwndClient ;
   BOOL  bLoop ;
   QMSG  qmMsg ;

   habAnchor = WinInitialize ( 0 ) ;
   hmqQueue = WinCreateMsgQueue ( habAnchor, 0 ) ;

   WinRegisterClass ( habAnchor,
                      CLS_CLIENT,
                      ClientWndProc,
                      CS_SIZEREDRAW,
                      sizeof ( PVOID )) ;

   ulFlags = FCF_STANDARD & ~FCF_ACCELTABLE & ~FCF_ICON ;

   hwndFrame = WinCreateStdWindow ( HWND_DESKTOP,
                                    0,
                                    &ulFlags,
                                    CLS_CLIENT,
                                    "Font Dialog Example",
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
         bLoop = WinGetMsg ( habAnchor, &qmMsg, NULLHANDLE, 0, 0 ) ;
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
   PMYFONTINFO pmfiFont ;

   pmfiFont = WinQueryWindowPtr ( hwndWnd, 0 ) ;

   switch ( ulMsg ) {

   case WM_CREATE:

      pmfiFont = malloc ( sizeof ( MYFONTINFO )) ;
      if ( pmfiFont == NULL ) {
         WinAlarm ( HWND_DESKTOP, WA_ERROR ) ;
         WinMessageBox ( HWND_DESKTOP,
                         hwndWnd,
                         "No memory could be allocated !",
                         "Error",
                         0,
                         MB_INFORMATION | MB_OK ) ;
         return MRFROMSHORT ( TRUE ) ;
      } /* endif */

      WinSetWindowPtr ( hwndWnd, 0, pmfiFont ) ;

      memset ( pmfiFont, 0, sizeof ( MYFONTINFO )) ;
      pmfiFont -> bInit = FALSE ;
      break ;
   case WM_DESTROY:
      if ( pmfiFont != NULL ) {
         free ( pmfiFont ) ;
      } /* endif */
      break ;

   case WM_PAINT:
      {
         HPS   hpsPaint ;
         ULONG ulReturn ;
         RECTL rclPaint ;
         CHAR  achFontName [200] , achMsg [256] ;

         hpsPaint = WinBeginPaint ( hwndWnd, NULLHANDLE, & rclPaint ) ;
         ulReturn = WinQueryPresParam ( hwndWnd,
                                        PP_FONTNAMESIZE,
                                        0,
                                        NULL,
                                        256,
                                        achFontName,
                                        0 ) ;
         if ( ulReturn ) {
            sprintf ( achMsg,
                      "The font selected is \"%s\"",
                      achFontName ) ;
         } else {
            strcpy ( achMsg, "No font selected" ) ;
         } /* endif */

         WinFillRect ( hpsPaint, &rclPaint, SYSCLR_WINDOW ) ;
         WinQueryWindowRect ( hwndWnd, &rclPaint ) ;

         WinDrawText ( hpsPaint,
                       - 1,
                       achMsg,
                       & rclPaint,
                       0,
                       0,
                       DT_VCENTER | DT_CENTER | DT_TEXTATTRS ) ;

         WinEndPaint ( hpsPaint ) ;
      }
      break ;

   case WM_COMMAND:

      switch ( SHORT1FROMMP ( mpParm1 )) {

      case IDM_FONT:
         SetFont ( hwndWnd , pmfiFont ) ;
         WinInvalidateRect ( hwndWnd, NULL, TRUE ) ;
         WinUpdateWindow ( hwndWnd ) ;
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

VOID SetFont ( HWND hwndWnd, PMYFONTINFO pmfiFont )
{
   FATTRS faAttrs ;
   FIXED  fxSzFont ;
   CHAR   achFamily [256] ;
   CHAR   achFont [256] ;


   faAttrs = pmfiFont -> fdFontDlg.fAttrs ;
   fxSzFont = pmfiFont -> fdFontDlg.fxPointSize ;

   memset ( &pmfiFont -> fdFontDlg, 0, sizeof ( FONTDLG )) ;
   memset ( achFont, 0, 256 ) ;

   pmfiFont -> fdFontDlg.cbSize = sizeof ( FONTDLG ) ;
   pmfiFont -> fdFontDlg.hpsScreen = WinGetPS ( hwndWnd ) ;
   pmfiFont -> fdFontDlg.pszFamilyname = achFamily ;
   pmfiFont -> fdFontDlg.usFamilyBufLen = sizeof ( achFamily ) ;
   pmfiFont -> fdFontDlg.fl = FNTS_CENTER | FNTS_INITFROMFATTRS ;
   pmfiFont -> fdFontDlg.clrFore = CLR_BLACK ;
   pmfiFont -> bInit = TRUE ;

   if ( pmfiFont -> bInit ) {
      pmfiFont -> fdFontDlg.fAttrs = faAttrs ;
      pmfiFont -> fdFontDlg.fxPointSize = fxSzFont ;
   } /* endif */

   if ( WinFontDlg ( HWND_DESKTOP,
                     hwndWnd,
                     &pmfiFont -> fdFontDlg ) != DID_OK ) {
      WinAlarm ( HWND_DESKTOP, WA_ERROR ) ;
      return ;
   } /* endif */

   WinReleasePS ( pmfiFont -> fdFontDlg.hpsScreen ) ;

   sprintf ( achFont,
             "%d.%s",
             FIXEDINT ( pmfiFont -> fdFontDlg.fxPointSize ) ,
             pmfiFont -> fdFontDlg.fAttrs.szFacename ) ;

   if ( pmfiFont -> fdFontDlg.fAttrs.fsSelection &
         FATTR_SEL_ITALIC ) {
      strcat ( achFont, ".Italic" ) ;
   } /* endif */

   if ( pmfiFont -> fdFontDlg.fAttrs.fsSelection &
         FATTR_SEL_UNDERSCORE ) {
      strcat ( achFont, ".Underscore" ) ;
   } /* endif */

   if ( pmfiFont -> fdFontDlg.fAttrs.fsSelection &
         FATTR_SEL_STRIKEOUT ) {
      strcat ( achFont, ".Strikeout" ) ;
   } /* endif */

   if ( pmfiFont -> fdFontDlg.fAttrs.fsSelection &
         FATTR_SEL_BOLD ) {
      strcat ( achFont, ".Bold" ) ;
   } /* endif */

   if ( pmfiFont -> fdFontDlg.fAttrs.fsSelection &
         FATTR_SEL_OUTLINE ) {
      strcat ( achFont, ".Outline" ) ;
   } /* endif */

   WinSetPresParam ( hwndWnd,
                     PP_FONTNAMESIZE,
                     strlen ( achFont ) + 1,
                     achFont ) ;

   return ;
}
