BUTTON.EXE:                     BUTTON.OBJ \
                                BUTTON.RES
        LINK386 @<<
BUTTON
BUTTON
BUTTON
OS2386
BUTTON
<<
        RC BUTTON.RES BUTTON.EXE

BUTTON.RES:                     BUTTON.RC \
                                BUTTON.H
        RC -r BUTTON.RC BUTTON.RES

BUTTON.OBJ:                     BUTTON.C \
                                BUTTON.H
        ICC -C+ -Kb+ -Ss+ BUTTON.C
