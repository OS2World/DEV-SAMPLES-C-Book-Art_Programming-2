VALUE.EXE:                      VALUE.OBJ \
                                VALUE.RES
        LINK386 @<<
VALUE
VALUE
VALUE
OS2386
VALUE
<<
        RC VALUE.RES VALUE.EXE

VALUE.RES:                      VALUE.RC \
                                VALUE.H
        RC -r VALUE.RC VALUE.RES

VALUE.OBJ:                      VALUE.C \
                                VALUE.H
        ICC -C+ -Kb+ -Ss+ VALUE.C
