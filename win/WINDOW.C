#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLS_CLIENT               "MyClass"

MRESULT EXPENTRY ClientWndProc ( HWND hwndWnd,
                                 ULONG ulMsg,
                                 MPARAM mpParm1,
                                 MPARAM mpParm2 ) ;
VOID WriteWindowInfo ( HPS hpsPaint,
                       HWND hwndPaint,
                       PRECTL prclRect,
                       ULONG ulCharHeight ) ;

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
                      0 ) ;

   ulFlags = FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER |
             FCF_MINMAX | FCF_SHELLPOSITION | FCF_TASKLIST ;

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

   case WM_PAINT:
      {
         HPS               hpsPaint ;
         RECTL             rclRect ;
         RECTL             rclWindow ;
         ULONG             ulCharHeight ;
         HWND              hwndEnum ;
         HWND              hwndFrame ;
         HENUM             heEnum ;
         FONTMETRICS       fmMetrics ;

         hpsPaint = WinBeginPaint ( hwndWnd,
                                    NULLHANDLE,
                                    &rclRect ) ;

         GpiQueryFontMetrics ( hpsPaint,
                               sizeof ( fmMetrics ) ,
                               &fmMetrics ) ;

         ulCharHeight = fmMetrics.lMaxBaselineExt ;

         WinQueryWindowRect ( hwndWnd, &rclWindow ) ;
         rclWindow.yBottom = rclWindow.yTop - ulCharHeight ;

         hwndFrame = WinQueryWindow ( hwndWnd, QW_PARENT ) ;
         WriteWindowInfo ( hpsPaint,
                           hwndFrame,
                           &rclWindow,
                           ulCharHeight ) ;

         heEnum = WinBeginEnumWindows ( hwndFrame ) ;

         hwndEnum = WinGetNextWindow ( heEnum ) ;
         while ( hwndEnum != NULLHANDLE ) {
             WriteWindowInfo ( hpsPaint,
                               hwndEnum,
                               &rclWindow,
                               ulCharHeight ) ;
             hwndEnum = WinGetNextWindow ( heEnum ) ;
         }

         WinEndEnumWindows ( heEnum ) ;
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

VOID WriteWindowInfo ( HPS hpsPaint,
                       HWND hwndPaint,
                       PRECTL prclRect,
                       ULONG ulCharHeight )
{
   CHAR        achString [ 200 ] ;
   CHAR        achClass [ 65 ] ;
   CHAR        achClassText [ 25 ] ;
   PCHAR       pchStart ;
   USHORT      usIndex ;
   PCHAR       apchClasses [] =
   {
      " ",
      "WC_FRAME",
      "WC_COMBOBOX",
      "WC_BUTTON",
      "WC_MENU",
      "WC_STATIC",
      "WC_ENTRYFIELD",
      "WC_LISTBOX",
      "WC_SCROLLBAR",
      "WC_TITLEBAR"
   } ;

   WinQueryClassName ( hwndPaint,
                       sizeof ( achClass ) ,
                       achClass ) ;
   pchStart = achClass ;

   if ( achClass [ 0 ] == '#' ) {
      usIndex = atoi ( &achClass [ 1 ] ) ;
      strcpy ( achClassText, apchClasses [ usIndex ] ) ;
   } else {
      strcpy ( achClassText, pchStart ) ;
   } /* endif */

   sprintf ( achString,
             "Window 0x%08lX belongs to class \"%s\"",
             hwndPaint, achClassText ) ;

   prclRect -> yTop = prclRect -> yBottom ;
   prclRect -> yBottom = prclRect -> yTop - ulCharHeight ;

   WinDrawText ( hpsPaint,
                 -1,
                 achString,
                 prclRect,
                 0,
                 0,
                 DT_CENTER | DT_TEXTATTRS ) ;

   sprintf ( achString,
             "  and has style = 0x%08lX",
             WinQueryWindowULong ( hwndPaint, QWL_STYLE )) ;

   prclRect -> yTop = prclRect -> yBottom ;
   prclRect -> yBottom = prclRect -> yTop - ulCharHeight ;

   WinDrawText ( hpsPaint,
                 -1,
                 achString,
                 prclRect,
                 0,
                 0,
                 DT_CENTER | DT_TEXTATTRS ) ;

   return ;
}
