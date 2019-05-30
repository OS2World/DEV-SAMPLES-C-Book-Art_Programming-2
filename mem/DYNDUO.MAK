ALL:                            BATMAN.EXE \
                                ROBIN.EXE

BATMAN.EXE:                     BATMAN.OBJ
        LINK386 @<<
BATMAN
BATMAN
BATMAN
OS2386
BATMAN
<<

BATMAN.OBJ:                     BATMAN.C \
                                DYNDUO.H
        ICC -C+ -Kb+ -Ss+ BATMAN.C

ROBIN.EXE:                      ROBIN.OBJ
        LINK386 @<<
ROBIN
ROBIN
ROBIN
OS2386
ROBIN
<<

ROBIN.OBJ:                      ROBIN.C \
                                DYNDUO.H
        ICC -C+ -Kb+ -Ss+ ROBIN.C
