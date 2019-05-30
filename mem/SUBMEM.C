#define INCL_DOSMEMMGR

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>

INT main ( VOID )
{
    PBYTE          pbHeap ;
    PBYTE          pbPtr1 ;
    PBYTE          pbPtr2 ;
    PBYTE          pbPtr3 ;
    PBYTE          pbPtr4 ;
    APIRET         arReturn ;

    arReturn = DosAllocMem(( PPVOID ) &pbHeap,
                           4096,
                           PAG_READ | PAG_WRITE ) ;

    printf ( "\nDosAllocMem ( ) returns %d", arReturn ) ;

    arReturn = DosSubSetMem(( PVOID ) pbHeap,
                              DOSSUB_SPARSE_OBJ | DOSSUB_INIT,
                              4096 ) ;

    printf ( "\nDosSubSetMem ( ) returns %d", arReturn ) ;

    arReturn = DosSubAllocMem ( pbHeap,
                                ( PPVOID ) &pbPtr1,
                                20 ) ;

    printf ( "\nDosSubAlloc ( ) returns %ld "
             "pbPtr1 size requested = 20",
             arReturn ) ;

    arReturn = DosSubAllocMem ( pbHeap,
                                ( PVOID ) &pbPtr2,
                                15 ) ;

    printf ( "\nDosSubAlloc ( ) returns %ld "
             "pbPtr2 size requested = 15",
             arReturn ) ;

    arReturn = DosSubAllocMem ( pbHeap,
                                ( PVOID ) &pbPtr3,
                                45 ) ;

    printf ( "\nDosSubAlloc ( ) returns %ld "
             "pbPtr3 size requested = 45",
             arReturn ) ;

    arReturn = DosSubAllocMem ( pbHeap,
                                ( PVOID ) &pbPtr4,
                                8 ) ;

    printf ( "\nDosSubAlloc ( ) returns %ld "
             "pbPtr4 size requested = 8",
             arReturn ) ;

    printf ( "\n\nHeader size = %d", pbPtr1 - pbHeap ) ;
    printf ( "\nSize of pbPtr1 ptr = %d", pbPtr2 - pbPtr1 ) ;
    printf ( "\nSize of pbPtr2 ptr = %d", pbPtr3 - pbPtr2 ) ;
    printf ( "\nSize of pbPtr3 ptr = %d", pbPtr4 - pbPtr3 ) ;
    printf ( "\nSize of pbPtr4 undeterminable" ) ;

    DosSubFreeMem ( pbHeap, pbPtr1, 20 ) ;
    DosSubFreeMem ( pbHeap, pbPtr2, 15 ) ;
    DosSubFreeMem ( pbHeap, pbPtr3, 45 ) ;
    DosSubFreeMem ( pbHeap, pbPtr4, 8 ) ;

    arReturn = DosFreeMem ( pbHeap ) ;

    printf ( "\nDosFreeMem ( ) returns %d", arReturn ) ;

    return 0 ;
}
