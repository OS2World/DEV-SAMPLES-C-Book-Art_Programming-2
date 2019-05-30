DIALOG.EXE:                     DIALOG.OBJ \
                                DIALOG.RES
        LINK386 @<<
DIALOG
DIALOG
DIALOG
OS2386
DIALOG
<<
        RC DIALOG.RES DIALOG.EXE

DIALOG.RES:                     DIALOG.RC \
                                DIALOG.H
        RC -r DIALOG.RC DIALOG.RES

DIALOG.OBJ:                     DIALOG.C \
                                DIALOG.H
        ICC -C+ -Kb+ -Ss+ DIALOG.C
