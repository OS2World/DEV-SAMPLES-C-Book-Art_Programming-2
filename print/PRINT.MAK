PRINT.EXE:                   PRINT.OBJ \
                                PRINT.RES
        LINK386 @<<
PRINT
PRINT
PRINT
OS2386
PRINT
<<
        RC PRINT.RES PRINT.EXE

PRINT.RES:                   PRINT.RC \
                                PRINT.H
        RC -r PRINT.RC PRINT.RES

PRINT.OBJ:                   PRINT.C \
                                PRINT.H
        ICC -C+ -Gm+ -Kb+ -Ss+ PRINT.C
