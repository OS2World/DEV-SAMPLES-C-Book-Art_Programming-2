#define INCL_DOSERRORS
#define INCL_DOSFILEMGR

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

INT main (USHORT usNumArgs,
          PCHAR apchArgs [] )
{

   CHAR           achPath [CCHMAXPATHCOMP] ;
   PCHAR          pchPath ;
   ULONG          ulCount ;
   HDIR           hdFile ;
   APIRET         arReturn ;
   FILEFINDBUF4   ffbFile ;
   CHAR           achFile [CCHMAXPATHCOMP] ;
   PBYTE          pbBuffer ;
   PDENA2         pdAttribute ;

   if ( usNumArgs != 2 ) {

      puts ( "Syntax:  CHKEA [filename]" ) ;
      puts ( "" ) ;
      puts ( "where \'filename\' is the name of a " ) ;
      puts ( "file/directory and can contain wildcards." ) ;
      return 1 ;

   } /* endif */

   DosQueryPathInfo ( apchArgs[1],
                      FIL_QUERYFULLNAME,
                      achPath,
                      CCHMAXPATHCOMP ) ;

   pchPath = strrchr ( achPath, '\\' ) ;

   if ( pchPath != NULL ) {

      pchPath++ ;
      *pchPath = 0 ;

   } /* endif */

   ulCount = 1 ;
   hdFile = HDIR_SYSTEM ;

   arReturn = DosFindFirst ( apchArgs[1],
                             &hdFile,
                             FILE_DIRECTORY,
                             &ffbFile,
                             sizeof ( FILEFINDBUF4 ),
                             &ulCount,
                             FIL_QUERYEASIZE ) ;

   while ( arReturn == 0 ) {

      sprintf ( achFile, "%s%s", achPath, ffbFile.achName ) ;

      printf ( "\nFile name: %s\n", achFile ) ;
      printf ( "\nTotal bytes allocated for EAs: %ld bytes.\n",
               ffbFile.cbList ) ;

      pbBuffer = malloc ( ffbFile.cbList ) ;

      ulCount = -1 ;

      arReturn = DosEnumAttribute ( ENUMEA_REFTYPE_PATH,
                                    achFile,
                                    1,
                                    pbBuffer,
                                    ffbFile.cbList,
                                    &ulCount,
                                    ENUMEA_LEVEL_NO_VALUE ) ;

      printf ( "\nThis object contains %ld EAs.\n", ulCount ) ;

      pdAttribute = (PDENA2) pbBuffer ;

      while ( ulCount != 0) {

         printf ( "Found EA with name \"%s\"\n",
                  pdAttribute->szName ) ;

         ulCount-- ;
         pdAttribute = (PDENA2) (((PBYTE) pdAttribute ) +
                        pdAttribute->oNextEntryOffset ) ;

      } /* endwhile */

      free ( pbBuffer ) ;

      ulCount = 1 ;
      arReturn = DosFindNext ( hdFile,
                               &ffbFile,
                               sizeof ( ffbFile ),
                               &ulCount ) ;

   } /* endwhile */

   if (( arReturn != 0 ) &&
        ( arReturn != ERROR_NO_MORE_FILES )) {

      printf ( "\nError %ld encountered\n", arReturn ) ;

   } /* endif */

   return arReturn ;
}
