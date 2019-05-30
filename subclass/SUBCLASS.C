#define INCL_WIN
#define INCL_GPILCIDS
#include <os2.h>
#include <string.h>
#include <ctype.h>

#define CLS_CLIENT               "MyClass"
#define IDE_ENTRYFIELD           256
#define STR_TEXT                 "Zip code:"

MRESULT EXPENTRY newEntryWndProc ( HWND hwndWnd,
   ULONG ulMsg,
   MPARAM mpParm1,
   MPARAM mpParm2 ) ;
MRESULT EXPENTRY clientWndProc ( HWND hwndWnd,
   ULONG ulMsg,
   MPARAM mpParm1,
   MPARAM mpParm2 ) ;

INT main ( VOID )
{
   HAB            habAnchor ;
   HMQ            hmqQueue ;
   ULONG          ulFlags ;
   HWND           hwndFrame ;
   HWND           hwndClient ;
   BOOL           bLoop ;
   QMSG           qmMsg ;

   habAnchor = WinInitialize ( 0 ) ;
   hmqQueue = WinCreateMsgQueue ( habAnchor, 0 ) ;

   WinRegisterClass ( habAnchor,
      CLS_CLIENT,
      clientWndProc,
      CS_SIZEREDRAW,
      0 ) ;

   ulFlags = FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER |
             FCF_MINMAX | FCF_SHELLPOSITION | FCF_TASKLIST ;

   hwndFrame = WinCreateStdWindow ( HWND_DESKTOP,
                                    0,
                                    &ulFlags,
                                    CLS_CLIENT,
                                    "Subclass Example",
                                    0,
                                    NULLHANDLE,
                                    0,
                                    &hwndClient ) ;

   if ( hwndFrame != NULLHANDLE ) {
      WinSetWindowPos ( hwndFrame,
                        HWND_TOP,
                        50,
                        50,
                        250,
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

MRESULT EXPENTRY clientWndProc ( HWND hwndWnd,
                                 ULONG ulMsg,
                                 MPARAM mpParm1,
                                 MPARAM mpParm2 )
{
   switch ( ulMsg ) {

   case WM_CREATE:
      {
         HPS               hpsChar ;
         FONTMETRICS       fmMetrics ;
         ENTRYFDATA        edEntry ;
         HWND              hwndEntry ;
         PFNWP             pfnOldEntryProc ;

         hpsChar = WinGetPS ( hwndWnd ) ;
         GpiQueryFontMetrics ( hpsChar,
                               sizeof ( fmMetrics ) ,
                               &fmMetrics ) ;
         WinReleasePS ( hpsChar ) ;

         edEntry.cb = sizeof ( edEntry ) ;
         edEntry.cchEditLimit = 9 ;
         edEntry.ichMinSel = 0 ;
         edEntry.ichMaxSel = 0 ;

         hwndEntry = WinCreateWindow ( hwndWnd,
                                       WC_ENTRYFIELD,
                                       "",
                                       WS_VISIBLE |
                                       ES_MARGIN |
                                       ES_AUTOSIZE,
                                       10,
                                       50,
                                       75,
                                       fmMetrics.lMaxBaselineExt,
                                       hwndWnd,
                                       HWND_TOP,
                                       IDE_ENTRYFIELD,
                                       &edEntry,
                                       NULL ) ;

         pfnOldEntryProc = WinSubclassWindow ( hwndEntry,
                                               newEntryWndProc ) ;

         WinSetWindowPtr ( hwndEntry,
                           0,
                           (PVOID) pfnOldEntryProc ) ;

         WinSetFocus ( HWND_DESKTOP, hwndEntry ) ;
      }
      break ;

   case WM_DESTROY:
      WinDestroyWindow ( WinWindowFromID ( hwndWnd,
                         IDE_ENTRYFIELD )) ;
      break ;

   case WM_ERASEBACKGROUND:
      return MRFROMSHORT ( TRUE ) ;

   case WM_PAINT:
      {
         HPS         hpsPaint ;
         SWP         swpEntry ;
         POINTL      ptlText ;

         hpsPaint = WinBeginPaint ( hwndWnd,
                                    NULLHANDLE,
                                    NULL ) ;

         GpiErase ( hpsPaint ) ;
         WinQueryWindowPos ( WinWindowFromID ( hwndWnd,
                             IDE_ENTRYFIELD ) ,
                             &swpEntry ) ;

         ptlText.x = swpEntry.x ;
         ptlText.y = swpEntry.y + swpEntry.cy + 10 ;

         GpiCharStringAt ( hpsPaint,
                           &ptlText,
                           strlen ( STR_TEXT ) ,
                           STR_TEXT ) ;

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

MRESULT EXPENTRY newEntryWndProc ( HWND hwndWnd,
                                   ULONG ulMsg,
                                   MPARAM mpParm1,
                                   MPARAM mpParm2 )
{
   PFNWP pfnOldEntryProc ;

   pfnOldEntryProc = (PFNWP) WinQueryWindowPtr ( hwndWnd, 0 ) ;

   switch ( ulMsg ) {
   case WM_CHAR:
      if ( CHARMSG ( &ulMsg ) -> fs & KC_CHAR ) {
         if ( !isdigit ( CHARMSG ( &ulMsg ) -> chr ) &&
            ( CHARMSG ( &ulMsg ) -> chr != '\b' )) {

            WinMessageBox ( HWND_DESKTOP,
                            HWND_DESKTOP,
                            "Only numeric characters are allowed "
                            "in this field",
                            "Numeric Field",
                            0,
                            MB_OK | MB_ERROR ) ;

             return MRFROMSHORT ( TRUE ) ;
         } /* endif */
      } /* endif */
      break ;
   case EM_PASTE:
      {
         HAB habAnchor ;
         PCHAR pchText ;
         CHAR achText [1024] ;
         USHORT usIndex ;

         habAnchor = WinQueryAnchorBlock ( hwndWnd ) ;
         WinOpenClipbrd ( habAnchor ) ;
         pchText = (PCHAR) WinQueryClipbrdData ( habAnchor,
                                                 CF_TEXT ) ;

         if ( pchText ) {

            strcpy ( achText, pchText ) ;
            WinCloseClipbrd ( habAnchor ) ;

            usIndex = 0 ;
            while ( achText [usIndex] ) {
               if ( !isdigit ( achText [usIndex ++ ] )){

                  WinMessageBox ( HWND_DESKTOP,
                                  HWND_DESKTOP,
                                  "Only numeric characters are "
                                  "allowed in this field",
                                  "Numerical Field",
                                  0,
                                  MB_OK | MB_ERROR ) ;

                  return MRFROMSHORT ( TRUE ) ;
                } /* endif */
            } /* endwhile */
         } else {
            WinCloseClipbrd ( habAnchor ) ;
         } /* endif */
      }
      break ;
   default:
      break ;
   } /* endswitch */

   return ( *pfnOldEntryProc ) ( hwndWnd,
                                 ulMsg,
                                 mpParm1,
                                 mpParm2 ) ;
}
