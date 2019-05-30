CONTAIN1.EXE:                    CONTAIN1.OBJ \
                                CONTAIN1.RES
        LINK386 @<<
CONTAIN1
CONTAIN1
CONTAIN1
OS2386
CONTAIN1
<<
        RC CONTAIN1.RES CONTAIN1.EXE

CONTAIN1.RES:                    CONTAIN1.RC \
                                CONTAIN1.H
        RC -r CONTAIN1.RC CONTAIN1.RES

CONTAIN1.OBJ:                    CONTAIN1.C \
                                CONTAIN1.H
        ICC -C+ -Kb+ -Ss+ CONTAIN1.C
