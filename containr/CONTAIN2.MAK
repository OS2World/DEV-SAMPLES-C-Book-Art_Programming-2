CONTAIN2.EXE:                    CONTAIN2.OBJ \
                                CONTAIN2.RES
        LINK386 @<<
CONTAIN2
CONTAIN2
CONTAIN2
OS2386
CONTAIN2
<<
        RC CONTAIN2.RES CONTAIN2.EXE

CONTAIN2.RES:                    CONTAIN2.RC \
                                CONTAIN2.H
        RC -r CONTAIN2.RC CONTAIN2.RES

CONTAIN2.OBJ:                    CONTAIN2.C \
                                CONTAIN2.H
        ICC -C+ -Kb+ -Ss+ CONTAIN2.C
