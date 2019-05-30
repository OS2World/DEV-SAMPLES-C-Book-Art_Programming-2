LIST1.EXE:                    LIST1.OBJ \
                                LIST1.RES
        LINK386 @<<
LIST1
LIST1
LIST1
OS2386
LIST1
<<
        RC LIST1.RES LIST1.EXE

LIST1.RES:                    LIST1.RC \
                                LIST1.H
        RC -r LIST1.RC LIST1.RES

LIST1.OBJ:                    LIST1.C \
                                LIST1.H
        ICC -C+ -Kb+ -Ss+ LIST1.C
