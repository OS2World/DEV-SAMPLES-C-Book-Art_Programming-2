SPIN.EXE:                       SPIN.OBJ \
                                SPIN.RES
        LINK386 @<<
SPIN
SPIN
SPIN
OS2386
SPIN
<<
        RC SPIN.RES SPIN.EXE

SPIN.RES:                       SPIN.RC \
                                SPIN.H
        RC -r SPIN.RC SPIN.RES

SPIN.OBJ:                       SPIN.C \
                                SPIN.H
        ICC -C+ -Kb+ -Ss+ SPIN.C
