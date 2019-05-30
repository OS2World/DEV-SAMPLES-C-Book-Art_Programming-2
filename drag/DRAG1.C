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
#define DRAG_RMF                 " < "DRAG_METHOD", "DRAG_FORMAT" > "

#define DRAG_TEMPNAME            "DRAGDROP.TMP"
#define MYM_DEWDROP              ( WM_USER )

typedef struct _CLIENTINFO {
   PDRAGINFO pdiDrag ;
   BOOL bDragging ;
   BOOL bEmphasis ;
   CHAR achLine [256] ;
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
                                    "Drag - n - Drop Sample"
                                    " Application",
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
{
   CHAR       achPath [CCHMAXPATH] ;
   CHAR       achFile [CCHMAXPATH] ;

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

//---------------------------------------------------------------
// This function paints the client window according its current
// status.
//
// Input:  hpsClient - handle to the presentation space.
//         hwndClient - handle to the client window.
//         pciInfo - pointer to the CLIENTINFO structure
//          describing the client window.
//---------------------------------------------------------------

{
   RECTL        rclWindow ;
   USHORT       usIndex ;
   POINTL       ptlPaint ;

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
                 0,
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
                    0,
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
//
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
         RECTL          rclWindow ;
         FILE           *pfFile ;
         DRAGITEM       diItem ;
         DRAGIMAGE      diImage ;
         HWND           hwndTarget ;

         WinQueryWindowRect ( hwndClient, &rclWindow ) ;

         //-----------------------------------------------------
         // If we're in the right half, return
         //-----------------------------------------------------

         if ( SHORT1FROMMP ( mpParm1 ) > rclWindow.xRight / 2 ) {
            return MRFROMSHORT ( FALSE ) ;
         } /* endif */

         //-----------------------------------------------------
         // Write the text to be dragged to a file,
         // since the type is
         // DRT_OS2FILE.
         //-----------------------------------------------------

         pfFile = fopen ( DRAG_TEMPNAME, "w" ) ;
         if ( pfFile == NULL ) {
            return MRFROMSHORT ( FALSE ) ;
         } /* endif */

         fprintf ( pfFile, "Dropped text" ) ;
         fclose ( pfFile ) ;

         //-----------------------------------------------------
         // Allocate the DRAGITEM/DRAGINFO structures and
         // initialize them
         //-----------------------------------------------------

         pciInfo -> pdiDrag = DrgAllocDraginfo ( 1 ) ;
         pciInfo -> pdiDrag -> hwndSource = hwndClient ;

         diItem.hwndItem = hwndClient ;
         diItem.ulItemID = 1 ;
         diItem.hstrType = DrgAddStrHandle ( DRT_TEXT ) ;
         diItem.hstrRMF = DrgAddStrHandle ( DRAG_RMF ) ;
         diItem.hstrContainerName = DrgAddStrHandle ( "." ) ;
         diItem.hstrSourceName = DrgAddStrHandle (
                                    DRAG_TEMPNAME ) ;

         diItem.hstrTargetName = diItem.hstrSourceName ;
         diItem.cxOffset = 0 ;
         diItem.cyOffset = 0 ;
         diItem.fsControl = 0 ;
         diItem.fsSupportedOps = DO_MOVEABLE ;

         DrgSetDragitem ( pciInfo -> pdiDrag,
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
                                pciInfo -> pdiDrag,
                                &diImage,
                                1L,
                                VK_ENDDRAG,
                                NULL ) ;

         if ( hwndTarget == NULLHANDLE ) {

            DrgDeleteDraginfoStrHandles ( pciInfo -> pdiDrag ) ;
            DrgFreeDraginfo ( pciInfo -> pdiDrag ) ;
            pciInfo -> pdiDrag = NULL ;
            pciInfo -> bDragging = FALSE ;
            remove ( DRAG_TEMPNAME ) ;

            WinInvalidateRect ( hwndClient, NULL, FALSE ) ;
            WinUpdateWindow ( hwndClient ) ;
         } /* endif */
      }
      break ;

   case DM_ENDCONVERSATION:
      {
         PDRAGITEM pdiItem ;
         CHAR achFullFile [CCHMAXPATH] ;

         //-----------------------------------------------------
         // Query the item dragged for cleanup purposes
         //-----------------------------------------------------

         pdiItem = DrgQueryDragitemPtr ( pciInfo -> pdiDrag,
                                         0 ) ;

         //-----------------------------------------------------
         // Delete the file used to transfer the data
         //-----------------------------------------------------

         fileFromDragItem ( pdiItem, achFullFile ) ;
         remove ( achFullFile ) ;

         //-----------------------------------------------------
         // Cleanup
         //-----------------------------------------------------

         DrgDeleteDraginfoStrHandles ( pciInfo -> pdiDrag ) ;
         DrgFreeDraginfo ( pciInfo -> pdiDrag ) ;
         pciInfo -> pdiDrag = NULL ;
         pciInfo -> bDragging = FALSE ;
      }
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
         // map them to our window and check if it is
         // in the right half of the window.  If not, return.
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
      break ;
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
         PDRAGINFO     pdiDrag ;
         PDRAGITEM     pdiItem ;
         CHAR          achFullFile [CCHMAXPATH] ;
         FILE          *pfFile ;
         HPS           hpsDrop ;

         //-----------------------------------------------------
         // Get the pointer to the DRAGINFO structure
         //-----------------------------------------------------

         pdiDrag = ( PDRAGINFO ) PVOIDFROMMP ( mpParm1 ) ;
         DrgAccessDraginfo ( pdiDrag ) ;

         //-----------------------------------------------------
         // Since we can render the object ourselves,
         // get the filename and read in the line
         //-----------------------------------------------------

         pdiItem = DrgQueryDragitemPtr ( pdiDrag, 0 ) ;
         fileFromDragItem ( pdiItem, achFullFile ) ;

         pfFile = fopen ( achFullFile, "r" ) ;
         if ( pfFile != NULL ) {
            fgets ( pciInfo -> achLine,
                    sizeof ( pciInfo -> achLine ) ,
                    pfFile ) ;
            fclose ( pfFile ) ;
         } /* endif */

         //-----------------------------------------------------
         // Turn the emphasis off and repaint ourselves to
         // show the dropped text
         //-----------------------------------------------------

         pciInfo -> bEmphasis = FALSE ;

         hpsDrop = DrgGetPS ( hwndClient ) ;
         paintClient ( hpsDrop, hwndClient, pciInfo ) ;
         DrgReleasePS ( hpsDrop ) ;

         //-----------------------------------------------------
         // Tell the source that we're done with the object
         //-----------------------------------------------------

         WinSendMsg ( pdiDrag -> hwndSource,
                      DM_ENDCONVERSATION,
                      MPFROMSHORT ( pdiItem -> ulItemID ) ,
                      0 ) ;

         //-----------------------------------------------------
         // Cleanup
         //-----------------------------------------------------

         if ( !pciInfo -> bDragging ) {
            DrgDeleteDraginfoStrHandles ( pdiDrag ) ;
            DrgFreeDraginfo ( pdiDrag ) ;
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
//--------------------------------------------------------------
// This function handles the messages sent to the client window.
//
// Input, output, returns:  as a window procedure
//--------------------------------------------------------------

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

      pciInfo -> bDragging = FALSE ;
      pciInfo -> bEmphasis = FALSE ;
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
   case DM_ENDCONVERSATION:
      return doSource ( hwndClient, ulMsg, mpParm1, mpParm2 ) ;

   case DM_DRAGOVER:
   case DM_DRAGLEAVE:
   case DM_DROP:
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