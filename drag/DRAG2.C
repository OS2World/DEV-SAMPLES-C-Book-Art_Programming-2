#define INCL_DOSFILEMGR
#define INCL_WININPUT
#define INCL_WINPOINTERS
#define INCL_WINSTDDRAG
#define INCL_WINSYS
#define INCL_WINWINDOWMGR

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLS_CLIENT               "SampleClass"

#define DRAG_METHOD              "DRM_OS2FILE"
#define DRAG_FORMAT              "DRF_UNKNOWN"
#define DRAG_RMF                 " <"DRAG_METHOD", "DRAG_FORMAT"> "

#define DRAG_TEMPNAME            "DRAGDROP.TMP"

#define MYM_DEWDROP              ( WM_USER )

typedef struct _CLIENTINFO {
   PDRAGINFO pdiSource ;          // Used by source
   BOOL      bDragging ;          // Used by source
   BOOL      bEmphasis ;          // Used by source
   PDRAGINFO pdiTarget ;          // Used by source
   CHAR      achLine [256] ;      // Used by target
} CLIENTINFO, *PCLIENTINFO ;


VOID fileFromDragItem ( PDRAGITEM pdiItem, PCHAR pchFile ) ;
VOID paintClient ( HPS hpsClient,
                   HWND hwndClient,
                   PCLIENTINFO pciInfo ) ;

MRESULT doSource ( HWND hwndClient,
                   ULONG ulMsg,
                   MPARAM mpParm1,
                   MPARAM mpParm2 ) ;

MRESULT doTarget ( HWND hwndClient,
                   ULONG ulMsg,
                   MPARAM mpParm1,
                   MPARAM mpParm2 ) ;

MRESULT EXPENTRY clientWndProc ( HWND hwndClient,
                                 ULONG ulMsg,
                                 MPARAM mpParm1,
                                 MPARAM mpParm2 ) ;

INT main ( VOID )
{
   HAB            habAnchor ;
   HMQ            hmqQueue ;
   ULONG          ulFlags ;
   HWND           hwndFrame ;
   BOOL           bLoop ;
   QMSG           qmMsg ;

   habAnchor = WinInitialize ( 0 ) ;
   hmqQueue = WinCreateMsgQueue ( habAnchor, 0 ) ;

   WinRegisterClass ( habAnchor,
                      CLS_CLIENT,
                      clientWndProc,
                      CS_SIZEREDRAW,
                      sizeof ( PVOID )) ;

   ulFlags = FCF_SIZEBORDER | FCF_TITLEBAR |
             FCF_TASKLIST | FCF_SHELLPOSITION |
             FCF_SYSMENU ;

   hwndFrame = WinCreateStdWindow ( HWND_DESKTOP,
                                    WS_VISIBLE,
                                    &ulFlags,
                                    CLS_CLIENT,
                                    "Drag - n - Drop"
                                    " Sample Application 2",
                                    0,
                                    NULLHANDLE,
                                    0,
                                    NULL ) ;

   bLoop = WinGetMsg ( habAnchor, &qmMsg, NULLHANDLE, 0, 0 ) ;
   while ( bLoop ) {
      WinDispatchMsg ( habAnchor, &qmMsg ) ;
      bLoop = WinGetMsg ( habAnchor, &qmMsg, NULLHANDLE, 0, 0 ) ;
   } /* endwhile */

   WinDestroyWindow ( hwndFrame ) ;
   WinDestroyMsgQueue ( hmqQueue ) ;
   WinTerminate ( habAnchor ) ;
   return ( 0 ) ;
}

VOID fileFromDragItem ( PDRAGITEM pdiItem, PCHAR pchFile )

//--------------------------------------------------------------
// This function composes a filename from the DRAGITEM structure
//  and returns it in pchFile.  It is assumed that pchFile
//  points to a buffer of size CCHMAXPATH.
//
//
// Input:  pdiItem - points to the DRAGITEM structure containing
//                   the necessary information.
// Output:  pchFile - points to the variable containing the
//                    filename.
//--------------------------------------------------------------
{
   CHAR     achPath [CCHMAXPATH] ;
   CHAR     achFile [CCHMAXPATH] ;

   DrgQueryStrName ( pdiItem -> hstrContainerName,
                     sizeof ( achPath ) ,
                     achPath ) ;

   DosQueryPathInfo ( achPath,
                      FIL_QUERYFULLNAME,
                      achPath,
                      sizeof ( achPath )) ;

   if ( achPath [strlen ( achPath ) - 1] != '\\' ) {
      strcat ( achPath, "\\" ) ;
   } /* endif */

   DrgQueryStrName ( pdiItem -> hstrSourceName,
                     sizeof ( achFile ) ,
                     achFile ) ;

   sprintf ( pchFile, "%s%s", achPath, achFile ) ;
   return ;
}

VOID paintClient ( HPS hpsClient,
                   HWND hwndClient,
                   PCLIENTINFO pciInfo )

//--------------------------------------------------------------
// This function paints the client window according
//    its current status.
//
// Input:  hpsClient - handle to the presentation space.
//         hwndClient - handle to the client window.
//         pciInfo - pointer to the CLIENTINFO structure
//                   describing the client window.
//--------------------------------------------------------------

{
   RECTL         rclWindow ;
   USHORT        usIndex ;
   POINTL        ptlPaint ;

   WinQueryWindowRect ( hwndClient, &rclWindow ) ;
   WinFillRect ( hpsClient, &rclWindow, SYSCLR_WINDOW ) ;

   //-----------------------------------------------------------
   // Draw the dividing line
   //-----------------------------------------------------------

   ptlPaint.x = rclWindow.xRight / 2 ;
   ptlPaint.y = rclWindow.yBottom ;
   GpiMove ( hpsClient, &ptlPaint ) ;

   ptlPaint.y = rclWindow.yTop ;
   GpiLine ( hpsClient, &ptlPaint ) ;

   //-----------------------------------------------------------
   // Set the color to indicate emphasized or
   // non-emphasized state
   //-----------------------------------------------------------

   if ( pciInfo -> bEmphasis ) {
      GpiSetColor ( hpsClient, CLR_BLACK ) ;
   } else {
      GpiSetColor ( hpsClient, SYSCLR_WINDOW ) ;
   } /* endif */

   //-----------------------------------------------------------
   // Draw/erase the emphasis
   //-----------------------------------------------------------

   for ( usIndex = 1 ; usIndex < 5 ; usIndex ++ ) {
      ptlPaint.x = rclWindow.xRight / 2 + usIndex ;
      ptlPaint.y = rclWindow.yBottom + usIndex ;
      GpiMove ( hpsClient, &ptlPaint ) ;

      ptlPaint.x = rclWindow.xRight - usIndex ;
      ptlPaint.y = rclWindow.yTop - usIndex ;
      GpiBox ( hpsClient, DRO_OUTLINE, &ptlPaint, 0, 0 ) ;
   } /* endfor */

   //-----------------------------------------------------------
   // Draw the instructing text
   //-----------------------------------------------------------

   WinQueryWindowRect ( hwndClient, &rclWindow ) ;
   rclWindow.xRight /= 2 ;

   WinDrawText ( hpsClient,
                 - 1,
                 "Drag me",
                 &rclWindow,
                 CLR_BLACK,
                 0L,
                 DT_CENTER | DT_VCENTER ) ;

   if ( pciInfo -> achLine [0] != 0 ) {

      //--------------------------------------------------------
      // Draw the text received if we've been dropped on
      //--------------------------------------------------------

      WinQueryWindowRect ( hwndClient, &rclWindow ) ;
      rclWindow.xLeft = rclWindow.xRight / 2 ;

      WinDrawText ( hpsClient,
                    - 1,
                    pciInfo -> achLine,
                    &rclWindow,
                    CLR_BLACK,
                    0L,
                    DT_CENTER | DT_VCENTER ) ;
   } /* endif */
   return ;
}

MRESULT doSource ( HWND hwndClient,
                   ULONG ulMsg,
                   MPARAM mpParm1,
                   MPARAM mpParm2 )

//--------------------------------------------------------------
// This function handles the direct-manipulation messages
// sent to the source part of the window.
//
// Input, output, returns:  as a window procedure
//--------------------------------------------------------------

{
   PCLIENTINFO pciInfo ;

   pciInfo = WinQueryWindowPtr ( hwndClient, 0 ) ;

   switch ( ulMsg ) {

   case WM_BEGINDRAG:
      {
         RECTL rclWindow ;
         DRAGITEM diItem ;
         DRAGIMAGE diImage ;
         HWND hwndTarget ;

         WinQueryWindowRect ( hwndClient, &rclWindow ) ;

         //-----------------------------------------------------
         // If we're in the right half, return
         //-----------------------------------------------------

         if ( SHORT1FROMMP ( mpParm1 ) >
              rclWindow.xRight / 2 ) {

            return MRFROMSHORT ( FALSE ) ;
         } /* endif */

         //-----------------------------------------------------
         // Allocate the DRAGITEM/DRAGINFO structures and
         // initialize them
         //-----------------------------------------------------

         pciInfo -> pdiSource = DrgAllocDraginfo ( 1 ) ;
         pciInfo -> pdiSource -> hwndSource = hwndClient ;

         diItem.hwndItem = hwndClient ;
         diItem.ulItemID = 1 ;
         diItem.hstrType = DrgAddStrHandle ( DRT_TEXT ) ;
         diItem.hstrRMF = DrgAddStrHandle ( DRAG_RMF ) ;
         diItem.hstrContainerName = DrgAddStrHandle ( "." ) ;
         diItem.hstrSourceName = NULLHANDLE ;
         diItem.hstrTargetName = NULLHANDLE ;
         diItem.cxOffset = 0 ;
         diItem.cyOffset = 0 ;
         diItem.fsControl = 0 ;
         diItem.fsSupportedOps = DO_MOVEABLE ;

         DrgSetDragitem ( pciInfo -> pdiSource,
                          &diItem,
                          sizeof ( diItem ) ,
                          0 ) ;

         //-----------------------------------------------------
         // Initialize the DRAGIMAGE structure
         //-----------------------------------------------------

         diImage.cb = sizeof ( diImage ) ;
         diImage.cptl = 0 ;
         diImage.hImage = WinQuerySysPointer ( HWND_DESKTOP,
                                               SPTR_FILE,
                                               FALSE ) ;
         diImage.sizlStretch.cx = 0 ;
         diImage.sizlStretch.cy = 0 ;
         diImage.fl = DRG_ICON | DRG_TRANSPARENT ;
         diImage.cxOffset = 0 ;
         diImage.cyOffset = 0 ;

         //-----------------------------------------------------
         // Set the bDragging flag and call DrgDrag().
         //-----------------------------------------------------
         pciInfo -> bDragging = TRUE ;

         hwndTarget = DrgDrag ( hwndClient,
                            pciInfo -> pdiSource,
                            &diImage,
                            1L,
                            VK_ENDDRAG,
                            NULL ) ;
         if ( hwndTarget == NULLHANDLE ) {
            DrgDeleteDraginfoStrHandles ( pciInfo -> pdiSource ) ;
            DrgFreeDraginfo ( pciInfo -> pdiSource ) ;
            pciInfo -> pdiSource = NULL ;
            pciInfo -> bDragging = FALSE ;
            remove ( DRAG_TEMPNAME ) ;

            WinInvalidateRect ( hwndClient, NULL, FALSE ) ;
            WinUpdateWindow ( hwndClient ) ;
         } /* endif */
      }
      break ;

   case DM_RENDERPREPARE:
      return MRFROMSHORT ( TRUE ) ;

   case DM_RENDER:
      {
         PDRAGTRANSFER pdtXfer ;
         CHAR          achFile [CCHMAXPATH] ;
         FILE          *pfFile ;

         pdtXfer = ( PDRAGTRANSFER ) PVOIDFROMMP ( mpParm1 ) ;

         DrgQueryStrName ( pdtXfer -> hstrRenderToName,
                         sizeof ( achFile ) ,
                         achFile ) ;
         DrgFreeDragtransfer(( PDRAGTRANSFER )
                               PVOIDFROMMP ( mpParm1 )) ;

         //-----------------------------------------------------
         // Write the text to be dragged to a file, since the
         // type is DRT_OS2FILE.
         //-----------------------------------------------------

         pfFile = fopen ( achFile, "w" ) ;
         if ( pfFile != NULL ) {
            fprintf ( pfFile, "Dropped text" ) ;
            fclose ( pfFile ) ;

            return MRFROMSHORT ( TRUE ) ;
         } else {
            return MRFROMSHORT ( FALSE ) ;
         } /* endif */
      }

   case DM_ENDCONVERSATION:

      //--------------------------------------------------------
      // Cleanup
      //--------------------------------------------------------

      DrgDeleteDraginfoStrHandles ( pciInfo -> pdiSource ) ;
      DrgFreeDraginfo ( pciInfo -> pdiSource ) ;
      pciInfo -> pdiSource = NULL ;
      pciInfo -> bDragging = FALSE ;
      break ;

   default:
      break ;
   } /* endswitch */

   return MRFROMSHORT ( FALSE ) ;
}

MRESULT doTarget ( HWND hwndClient,
                   ULONG ulMsg,
                   MPARAM mpParm1,
                   MPARAM mpParm2 )

//--------------------------------------------------------------
// This function handles the direct-manipulation messages
// sent to the target part of the window.
//
// Input, output, returns:  as a window procedure
//--------------------------------------------------------------

{
   PCLIENTINFO pciInfo ;

   pciInfo = WinQueryWindowPtr ( hwndClient, 0 ) ;

   switch ( ulMsg ) {
   case DM_DRAGOVER:
      {
         PDRAGINFO pdiDrag ;
         POINTL ptlDrop ;
         RECTL rclWindow ;
         HPS hpsWindow ;
         PDRAGITEM pdiItem ;

         //-----------------------------------------------------
         // Get the pointer to the DRAGINFO structure
         //-----------------------------------------------------

         pdiDrag = ( PDRAGINFO ) PVOIDFROMMP ( mpParm1 ) ;
         DrgAccessDraginfo ( pdiDrag ) ;

         //-----------------------------------------------------
         // Since the drop position is in screen coordinates,
         // map them to our window and check if it is in the
         // right half of the window.  If not, return.
         //-----------------------------------------------------

         ptlDrop.x = SHORT1FROMMP ( mpParm2 ) ;
         ptlDrop.y = SHORT2FROMMP ( mpParm2 ) ;
         WinMapWindowPoints ( HWND_DESKTOP,
                              hwndClient,
                              &ptlDrop,
                              1 ) ;

         WinQueryWindowRect ( hwndClient, &rclWindow ) ;

         if ( ptlDrop.x < rclWindow.xRight / 2 ) {
            if ( pciInfo -> bEmphasis ) {
               pciInfo -> bEmphasis = FALSE ;

               hpsWindow = DrgGetPS ( hwndClient ) ;
               paintClient ( hpsWindow, hwndClient, pciInfo ) ;
               DrgReleasePS ( hpsWindow ) ;
            } /* endif */

            return MRFROMSHORT ( DOR_NODROP ) ;
         } /* endif */

         //-----------------------------------------------------
         // Check to see if we've already turned emphasis on.
         //-----------------------------------------------------

         if ( !pciInfo -> bEmphasis ) {

            pciInfo -> bEmphasis = TRUE ;

            hpsWindow = DrgGetPS ( hwndClient ) ;
            paintClient ( hpsWindow, hwndClient, pciInfo ) ;
            DrgReleasePS ( hpsWindow ) ;

         } /* endif */

         //-----------------------------------------------------
         // We should only be dragging one item.
         //-----------------------------------------------------

         if ( DrgQueryDragitemCount ( pdiDrag ) != 1 ) {
            return MRFROMSHORT ( DOR_NODROP ) ;
         } /* endif */

         //-----------------------------------------------------
         // Check the true type and native RMF
         //-----------------------------------------------------

         pdiItem = DrgQueryDragitemPtr ( pdiDrag, 0 ) ;

         if ( !DrgVerifyTrueType ( pdiItem, DRT_TEXT )) {
            return MRFROMSHORT ( DOR_NODROP ) ;
         } else
         if ( !DrgVerifyNativeRMF ( pdiItem, DRAG_RMF )) {
            return MRFROMSHORT ( DOR_NODROP ) ;
         } /* endif */

         return MRFROM2SHORT ( DOR_DROP, DO_MOVE ) ;
      }

   case DM_DRAGLEAVE:
      if ( pciInfo -> bEmphasis ) {
         HPS hpsWindow ;

         //-----------------------------------------------------
         // Turn off the emphasis
         //-----------------------------------------------------

         pciInfo -> bEmphasis = FALSE ;

         hpsWindow = DrgGetPS ( hwndClient ) ;
         paintClient ( hpsWindow, hwndClient, pciInfo ) ;
         DrgReleasePS ( hpsWindow ) ;
      } /* endif */
      break ;

   case DM_DROP:
      WinPostMsg ( hwndClient, MYM_DEWDROP, mpParm1, mpParm2 ) ;
      break ;

   case DM_RENDERCOMPLETE:
      break ;

   case DM_DROPHELP:
      WinMessageBox ( HWND_DESKTOP,
                    hwndClient,
                    "This is the drag - n - drop help."
                    "  Great, isn't it?",
                    "Help",
                    0,
                    MB_INFORMATION | MB_OK ) ;
      break ;

   case MYM_DEWDROP:
      {
         HPS               hpsDrop ;
         PDRAGITEM         pdiItem ;
         CHAR              achRMF [256] ;
         PDRAGTRANSFER     pdtXfer ;
         CHAR              achFile [CCHMAXPATH] ;
         FILE              *pfFile ;

         //-----------------------------------------------------
         // Get the pointer to the DRAGINFO structure
         //-----------------------------------------------------

         pciInfo -> pdiTarget = ( PDRAGINFO )
                                  PVOIDFROMMP ( mpParm1 ) ;

         DrgAccessDraginfo ( pciInfo -> pdiTarget ) ;

         //-----------------------------------------------------
         // Turn the emphasis off
         //-----------------------------------------------------

         pciInfo -> bEmphasis = FALSE ;

         hpsDrop = DrgGetPS ( hwndClient ) ;
         paintClient ( hpsDrop, hwndClient, pciInfo ) ;
         DrgReleasePS ( hpsDrop ) ;

         //-----------------------------------------------------
         // If the source did not render the data previously,
         // we need to allocate a DRAGTRANSFER structure and
         // send the source a DM_RENDER message.
         //-----------------------------------------------------

         pdiItem = DrgQueryDragitemPtr ( pciInfo -> pdiTarget,
                                         0 ) ;

         if ( pdiItem -> hstrSourceName == NULLHANDLE ) {
            DrgQueryNativeRMF ( pdiItem,
                                sizeof ( achRMF ) ,
                                achRMF ) ;

            pdtXfer = DrgAllocDragtransfer ( 1 ) ;
            pdtXfer -> cb = sizeof ( DRAGTRANSFER ) ;
            pdtXfer -> hwndClient = hwndClient ;
            pdtXfer -> pditem = pdiItem ;
            pdtXfer -> hstrSelectedRMF = DrgAddStrHandle (
                                             achRMF ) ;
            pdtXfer -> hstrRenderToName = DrgAddStrHandle (
                                             DRAG_TEMPNAME ) ;
            pdtXfer -> ulTargetInfo = 0 ;
            pdtXfer -> usOperation =
               pciInfo -> pdiTarget -> usOperation ;

            pdtXfer -> fsReply = 0 ;

            //--------------------------------------------------
            // Does the source need to prepare the item?
            // If so, send a DM_RENDERPREPARE message.
            //--------------------------------------------------

            if (( pdiItem -> fsControl & DC_PREPARE ) != 0 ) {

               if ( !SHORT1FROMMR (
                        DrgSendTransferMsg (
                           pciInfo -> pdiTarget -> hwndSource,
                           DM_RENDERPREPARE,
                           MPFROMP ( pdtXfer ) ,
                           0L )) ) {

                  DrgDeleteStrHandle (
                     pdtXfer -> hstrSelectedRMF ) ;

                  DrgDeleteStrHandle (
                     pdtXfer -> hstrRenderToName ) ;

                  DrgFreeDragtransfer ( pdtXfer ) ;
                  return MRFROMSHORT ( FALSE ) ;

               } /* endif */
            } /* endif */

            //--------------------------------------------------
            // Render the object
            //--------------------------------------------------

            if ( !SHORT1FROMMR (
                  DrgSendTransferMsg (
                        pciInfo -> pdiTarget -> hwndSource,
                        DM_RENDER,
                        MPFROMP ( pdtXfer ) ,
                        0L )) ) {

               DrgDeleteStrHandle ( pdtXfer ->hstrSelectedRMF ) ;
               DrgDeleteStrHandle ( pdtXfer ->hstrRenderToName ) ;
               DrgFreeDragtransfer ( pdtXfer ) ;
               return MRFROMSHORT ( FALSE ) ;

            } /* endif */

            strcpy ( achFile, DRAG_TEMPNAME ) ;

         } else {

            //--------------------------------------------------
            // The source already rendered the object, so we
            // can simply read from the file.
            //--------------------------------------------------

            pdtXfer = NULL ;
            fileFromDragItem ( pdiItem, achFile ) ;
         } /* endif */

         pfFile = fopen ( achFile, "r" ) ;
         if ( pfFile != NULL ) {
            fgets ( pciInfo -> achLine,
                    sizeof ( pciInfo -> achLine ) ,
                    pfFile ) ;
            fclose ( pfFile ) ;

            if ( pdtXfer != NULL ) {
               remove ( achFile ) ;
            } /* endif */

            //--------------------------------------------------
            // Repaint ourselves again to show the dropped text
            //--------------------------------------------------

            hpsDrop = DrgGetPS ( hwndClient ) ;
            paintClient ( hpsDrop, hwndClient, pciInfo ) ;
            DrgReleasePS ( hpsDrop ) ;
         } /* endif */

         //-----------------------------------------------------
         // Tell the source that we're done rendering
         //-----------------------------------------------------

         WinSendMsg ( pciInfo -> pdiTarget -> hwndSource,
                    DM_ENDCONVERSATION,
                    MPFROMSHORT ( pdiItem -> ulItemID ) ,
                    0L ) ;

         //-----------------------------------------------------
         // Cleanup
         //-----------------------------------------------------

         if ( pdtXfer != NULL ) {
            DrgDeleteStrHandle ( pdtXfer -> hstrSelectedRMF ) ;
            DrgDeleteStrHandle ( pdtXfer -> hstrRenderToName ) ;
            DrgFreeDragtransfer ( pdtXfer ) ;
         } /* endif */

         if ( !pciInfo -> bDragging ) {
            DrgDeleteDraginfoStrHandles ( pciInfo -> pdiTarget ) ;
            DrgFreeDraginfo ( pciInfo -> pdiTarget ) ;
         } /* endif */
      }
      break ;
   default:
      break ;
   } /* endswitch */

   return MRFROMSHORT ( FALSE ) ;
}

MRESULT EXPENTRY clientWndProc ( HWND hwndClient,
                                 ULONG ulMsg,
                                 MPARAM mpParm1,
                                 MPARAM mpParm2 )

{
   PCLIENTINFO pciInfo ;

   pciInfo = WinQueryWindowPtr ( hwndClient, 0 ) ;

   switch ( ulMsg ) {

   case WM_CREATE:

      //--------------------------------------------------------
      // Allocate memory for the instance data structure
      // and initialize it
      //--------------------------------------------------------

      pciInfo = malloc ( sizeof ( CLIENTINFO )) ;
      if ( pciInfo == NULL ) {
         WinMessageBox ( HWND_DESKTOP,
                       hwndClient,
                       "There is not enough memory.",
                       "Error",
                       0,
                       MB_ICONHAND | MB_OK ) ;
         return MRFROMSHORT ( TRUE ) ;
      } /* endif */

      WinSetWindowPtr ( hwndClient, 0, pciInfo ) ;

      pciInfo -> pdiSource = NULL ;
      pciInfo -> bDragging = FALSE ;
      pciInfo -> bEmphasis = FALSE ;
      pciInfo -> pdiTarget = NULL ;
      pciInfo -> achLine [0] = 0 ;
      break ;

   case WM_DESTROY:

      //--------------------------------------------------------
      // Free the memory used by the instance data
      //--------------------------------------------------------
      free ( pciInfo ) ;
      WinSetWindowPtr ( hwndClient, 0, NULL ) ;
      break ;

   case WM_PAINT:
      {
         HPS hpsPaint ;

         hpsPaint = WinBeginPaint ( hwndClient,
                                    NULLHANDLE,
                                    NULL ) ;

         paintClient ( hpsPaint, hwndClient, pciInfo ) ;
         WinEndPaint ( hpsPaint ) ;
      }
      break ;

   case WM_BEGINDRAG:
   case DM_RENDERPREPARE:
   case DM_RENDER:
   case DM_ENDCONVERSATION:
      return doSource ( hwndClient, ulMsg, mpParm1, mpParm2 ) ;

   case DM_DRAGOVER:
   case DM_DRAGLEAVE:
   case DM_DROP:
   case DM_RENDERCOMPLETE:
   case DM_DROPHELP:
   case MYM_DEWDROP:
      return doTarget ( hwndClient, ulMsg, mpParm1, mpParm2 ) ;

   default:
      return WinDefWindowProc ( hwndClient,
                                ulMsg,
                                mpParm1,
                                mpParm2 ) ;
   } /* endswitch */

   return MRFROMSHORT ( FALSE ) ;
}

