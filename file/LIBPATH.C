#define INCL_DOSFILEMGR
#define INCL_DOSMISC

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define CONFIG_FILE              "?:\\CONFIG.SYS"
#define LIBPATH                  "LIBPATH"
#define CR                       13
#define LF                       10

INT main ( VOID )
{
   APIRET         arReturn ;
   ULONG          ulDrive;
   PCHAR          pchFile ;
   HFILE          hfFile ;
   ULONG          ulAction ;
   ULONG          ulBytesRead ;
   PCHAR          pchBuffer ;
   PCHAR          pchLibpath ;
   USHORT         usIndex ;
   FILESTATUS3    fsStatus ;

   pchBuffer = NULL ;

   arReturn = DosQuerySysInfo ( QSV_BOOT_DRIVE,
                                QSV_BOOT_DRIVE,
                                &ulDrive,
                                sizeof ( ulDrive )) ;

   pchFile = CONFIG_FILE ;
   pchFile [0] = ulDrive + 'A' - 1 ;

   arReturn = DosQueryPathInfo ( pchFile,
                                 FIL_STANDARD,
                                 &fsStatus,
                                 sizeof ( fsStatus )) ;

   pchBuffer = malloc ( fsStatus.cbFile ) ;

   arReturn = DosOpen ( pchFile,
                        &hfFile,
                        &ulAction,
                        0,
                        FILE_NORMAL,
                        FILE_OPEN,
                        OPEN_FLAGS_FAIL_ON_ERROR |
                        OPEN_FLAGS_SEQUENTIAL |
                        OPEN_SHARE_DENYNONE |
                        OPEN_ACCESS_READONLY,
                        NULL ) ;

   arReturn = DosRead ( hfFile,
                        pchBuffer,
                        fsStatus.cbFile,
                        &ulBytesRead ) ;

   arReturn = DosClose ( hfFile ) ;

   pchBuffer [fsStatus.cbFile] = 0 ;

   pchLibpath = strstr ( pchBuffer, LIBPATH ) ;

   if (pchLibpath == NULL){
   /*will only execute this section of code if LIBPATH is NOT all caps*/
      for ( usIndex = 0; usIndex < strlen (pchBuffer); usIndex++){
      if (toupper(pchBuffer[usIndex]) == 'L')
         if (toupper(pchBuffer[usIndex+1]) == 'I' &&
             toupper(pchBuffer[usIndex+2]) == 'B' &&
             toupper(pchBuffer[usIndex+3]) == 'P' &&
             toupper(pchBuffer[usIndex+4]) == 'A' &&
             toupper(pchBuffer[usIndex+5]) == 'T' &&
             toupper(pchBuffer[usIndex+6]) == 'H'){
           pchLibpath = pchBuffer + usIndex;
           break;
         }
      }
   }

   for ( usIndex = 0 ; usIndex < CCHMAXPATHCOMP ; usIndex++ ){
      if ( pchLibpath [usIndex] == CR ){
         if ( pchLibpath [usIndex + 1] == LF )
         break ;
      } /* endif */
   } /* endfor */

   pchLibpath [usIndex] = 0 ;
   printf ( "\n%s\n", pchLibpath ) ;
   free ( pchBuffer ) ;
   return arReturn ;
}
