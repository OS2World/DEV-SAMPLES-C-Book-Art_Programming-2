#define INCL_DOSSESMGR

#include <os2.h>
#include <stdio.h>

INT main ( VOID )
{
   APIRET         arReturn ;
   ULONG          ulSession ;
   PID            pidProcess ;
   STARTDATA      sdSession ;
   CHAR           achFail [200] ;

   sdSession.Length = sizeof ( STARTDATA ) ;
   sdSession.Related = SSF_RELATED_INDEPENDENT ;
   sdSession.FgBg = SSF_FGBG_FORE ;
   sdSession.TraceOpt = SSF_TRACEOPT_NONE ;
   sdSession.PgmTitle =  "Directory Test Program" ;
   sdSession.PgmName = 0 ;
   sdSession.PgmInputs = " /C dir *.*" ;
   sdSession.TermQ = 0 ;
   sdSession.Environment = 0 ;
   sdSession.InheritOpt = SSF_INHERTOPT_PARENT ;
   sdSession.SessionType = SSF_TYPE_WINDOWABLEVIO ;
   sdSession.IconFile = 0 ;
   sdSession.PgmHandle = 0 ;
   sdSession.PgmControl = SSF_CONTROL_NOAUTOCLOSE ;
   sdSession.InitXPos = 10 ;
   sdSession.InitYPos = 10 ;
   sdSession.InitXSize = 400 ;
   sdSession.InitYSize = 400 ;
   sdSession.Reserved = 0 ;
   sdSession.ObjectBuffer = achFail ;
   sdSession.ObjectBuffLen = 200 ;

   arReturn = DosStartSession ( &sdSession,
                                &ulSession,
                                &pidProcess ) ;

   printf ( "\narReturn = %d", arReturn ) ;

   return 0 ;
}
