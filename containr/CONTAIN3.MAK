CONTAIN3.EXE:                    CONTAIN3.OBJ \
                                CONTAIN3.RES
        LINK386 $(LINKOPTS) @<<
CONTAIN3
CONTAIN3
CONTAIN3
OS2386
CONTAIN3
<<
        RC CONTAIN3.RES CONTAIN3.EXE

CONTAIN3.RES:                    CONTAIN3.RC \
                                CONTAIN3.H
        RC -r CONTAIN3.RC CONTAIN3.RES

CONTAIN3.OBJ:                    CONTAIN3.C \
                                CONTAIN3.H
        ICC -C+ -Kb+ -Ss+ CONTAIN3.C
