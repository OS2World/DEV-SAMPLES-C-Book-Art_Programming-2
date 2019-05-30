#define INCL_WIN
#define INCL_GPI

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list1.h"

MRESULT EXPENTRY ClientWndProc ( HWND hwndClient,
                                 ULONG ulMsg,
                                 MPARAM mpParm1,
                                 MPARAM mpParm2 ) ;

MRESULT EXPENTRY DlgProc ( HWND hwndDlg,
                           ULONG ulMsg,
                           MPARAM mpParm1,
                           MPARAM mpParm2 ) ;




#define CLS_CLIENT               "MyClass"
int main ( void ) ;

typedef struct {
    BOOL   fSelectedItems ;
    USHORT ausListBoxSel [10] ;
} LISTBOXINFO, *PLISTBOXINFO ;

int main ( )
{
    HMQ hmqQueue ;
    HAB habAnchor ;
    ULONG ulFlags ;
    HWND hwndFrame ;
    HWND hwndClient ;
    QMSG qmMsg ;

    /* initialization stuff */
    habAnchor = WinInitialize ( 0 ) ;
    hmqQueue = WinCreateMsgQueue ( habAnchor, 0 ) ;

    WinRegisterClass ( habAnchor,
                       CLS_CLIENT,
                       ClientWndProc,
                       CS_SYNCPAINT,
                       sizeof ( PVOID )) ;

    ulFlags = FCF_TITLEBAR | FCF_SYSMENU |
              FCF_SIZEBORDER | FCF_MINMAX | FCF_SHELLPOSITION ;


    hwndFrame = WinCreateStdWindow ( HWND_DESKTOP,
                                   WS_VISIBLE,
                                   &ulFlags,
                                   CLS_CLIENT,
                                   "Plain Listbox Example",
                                   0,
                                   ( HMODULE ) 0,
                                   IDM_MENU,
                                   &hwndClient ) ;


    if ( hwndFrame != NULLHANDLE ) {
        while ( WinGetMsg ( habAnchor, &qmMsg, NULLHANDLE, 0, 0 ))
            WinDispatchMsg ( habAnchor, &qmMsg ) ;
        WinDestroyWindow ( hwndFrame ) ;
    } /* endif */

    /* clean - up */
    WinDestroyMsgQueue ( hmqQueue ) ;
    WinTerminate ( habAnchor ) ;
    return 0 ;
}

MRESULT EXPENTRY ClientWndProc ( HWND hwndClient, ULONG ulMsg,
                                 MPARAM mpParm1, MPARAM mpParm2 )

{
    PLISTBOXINFO pliInfo ;


    pliInfo = WinQueryWindowPtr ( hwndClient, 0 ) ;

    switch ( ulMsg ) {
        case WM_ERASEBACKGROUND:
            return ( MRFROMLONG ( TRUE ) ) ;
        case WM_CREATE:
            pliInfo =
            ( PLISTBOXINFO ) malloc ( sizeof ( LISTBOXINFO )) ;
            if ( pliInfo == ( PLISTBOXINFO ) 0 ) {
                WinAlarm ( HWND_DESKTOP,
                           WA_ERROR ) ;
                return (( MRESULT ) TRUE ) ;
            } /* endif */
            WinSetWindowPtr ( hwndClient,
                0,
                pliInfo ) ;
            pliInfo -> fSelectedItems = FALSE ;
            WinPostMsg ( hwndClient,
                         UM_LOADDLG,
                         ( MPARAM ) 0,
                         ( MPARAM ) 0 ) ;
            break ;
        case WM_DESTROY:
            if ( pliInfo )
                free ( pliInfo ) ;
            break ;
        case WM_PAINT:
            {
                HPS hpsPresentationSpace ;
                RECTL rectInvalidRect, rclPaintRegion ;
                USHORT i ;

                hpsPresentationSpace = WinBeginPaint (
                                          hwndClient,
                                          NULLHANDLE,
                                          &rectInvalidRect ) ;
                rclPaintRegion.xLeft = rectInvalidRect.xLeft ;
                rclPaintRegion.xRight = rectInvalidRect.xRight ;
                rclPaintRegion.yBottom = rectInvalidRect.yBottom ;
                rclPaintRegion.yTop = rectInvalidRect.yTop ;

                WinFillRect ( hpsPresentationSpace,
                              &rectInvalidRect,
                              CLR_WHITE ) ;
                if ( pliInfo -> fSelectedItems == TRUE ) {

                    rclPaintRegion.yTop -= 15 ;
                    WinDrawText ( hpsPresentationSpace,
                                  - 1,
                                  "You have selected:",
                                  &rclPaintRegion,
                                  0,
                                  0,
                                  DT_LEFT | DT_TEXTATTRS ) ;

                    for ( i = 0 ; i < 10 ; i ++ )
                        if ( pliInfo -> ausListBoxSel [i] ==
                              TRUE ) {

                            rclPaintRegion.yTop -= 15 ;
                            WinDrawText ( hpsPresentationSpace,
                                          - 1,
                                          pszListBoxEntry [i] ,
                                          &rclPaintRegion,
                                          0,
                                          0,
                                          DT_LEFT |
                                          DT_TEXTATTRS ) ;

                            pliInfo -> ausListBoxSel [i] = FALSE ;
                        } /* end if selected */
                    pliInfo -> fSelectedItems = FALSE ;
                } /* end if */
                WinEndPaint ( hpsPresentationSpace ) ;
                break ;

            }
        case UM_LOADDLG:
            WinDlgBox ( hwndClient,
                        hwndClient,
                        DlgProc,
                        ( HMODULE ) 0,
                        IDD_LISTBOX,
                        NULL ) ;
            return ( MRFROMLONG ( TRUE ) ) ;
        case WM_SIZE:
            WinPostMsg ( hwndClient,
                         UM_LISTBOXSEL ,
                         ( MPARAM ) 0,
                         ( MPARAM ) 0 ) ;
            break ;
        case UM_LISTBOXSEL:
            {

                SHORT sSelect = 0 ;
                USHORT usIndex = LIT_FIRST ;
                RECTL rclInvalidRegion ;

                while ( sSelect != LIT_NONE ) {
                    sSelect = ( SHORT ) WinSendDlgItemMsg (
                        WinWindowFromID (
                        hwndClient, IDD_LISTBOX ) ,
                        IDL_LISTBOX,
                        LM_QUERYSELECTION,
                        MPFROMSHORT ( usIndex ) ,
                        ( MPARAM ) 0 ) ;
                    pliInfo -> ausListBoxSel [sSelect] = TRUE ;
                    usIndex = sSelect ;
                 }
                pliInfo -> fSelectedItems = TRUE ;

                WinQueryWindowRect ( hwndClient,
                    &rclInvalidRegion ) ;

                rclInvalidRegion.xLeft =
                   ( rclInvalidRegion.xRight -
                   rclInvalidRegion.xLeft ) / 3 * 2 ;

                WinInvalidateRect ( hwndClient,
                    &rclInvalidRegion, FALSE ) ;
                break ;
            }
        default:
            return WinDefWindowProc ( hwndClient,
                ulMsg, mpParm1, mpParm2 ) ;
         } /* endswitch */

    return ( MRFROMLONG ( FALSE )) ;
    }

MRESULT EXPENTRY DlgProc ( HWND hwndDlg, ULONG ulMsg,
     MPARAM mpParm1, MPARAM mpParm2 )

{
    USHORT usNumItems = 10 ;
    USHORT i ;

    switch ( ulMsg ) {

        case WM_INITDLG:
            {
                HWND hwndListBox ;

                hwndListBox = WinWindowFromID ( hwndDlg,
                     IDL_LISTBOX ) ;
                for ( i = 0 ; i < usNumItems ; i ++ )
                    WinInsertLboxItem ( hwndListBox,
                                        LIT_END,
                                        pszListBoxEntry [i] ) ;

                WinSendDlgItemMsg ( hwndDlg,
                                    IDL_LISTBOX,
                                    LM_SELECTITEM,
                                    MPFROMSHORT ( 0 ) ,
                                    MPFROMSHORT ( TRUE )) ;
            }
            break ;

        case WM_COMMAND:
            {
                SHORT sCommand ;

                HWND hwndClient ;
                RECTL rclRectangle ;

                sCommand = SHORT1FROMMP ( mpParm1 ) ;
                switch ( sCommand ) {
                    case DID_OK:
                        hwndClient = WinQueryWindow ( hwndDlg,
                                                      QW_PARENT ) ;
                        WinPostMsg ( hwndClient,
                                     UM_LISTBOXSEL,
                                     ( MPARAM ) 0,
                                     ( MPARAM ) 0 ) ;
                        return (MRFROMLONG ( TRUE )) ;

                    case DID_CANCEL:
                        hwndClient = WinQueryWindow ( hwndDlg,
                                                      QW_PARENT ) ;
                        WinQueryWindowRect ( hwndClient,
                                             &rclRectangle ) ;
                        WinInvalidateRect ( hwndClient,
                                            &rclRectangle,
                                            FALSE ) ;
                        WinDismissDlg ( hwndDlg, sCommand ) ;
                        break ;

                 } /* end switch sCommand */
               break ;
            }
        default:
            return ( WinDefDlgProc ( hwndDlg, ulMsg,
                                     mpParm1, mpParm2 )) ;
   } /* endswitch */

   return ( MRFROMLONG ( FALSE ) ) ;
}

