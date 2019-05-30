FONTDLG.EXE:                    FONTDLG.OBJ \
                                FONTDLG.RES
        LINK386 /CO @<<
FONTDLG
FONTDLG
FONTDLG
OS2386
FONTDLG
<<
        RC FONTDLG.RES FONTDLG.EXE

FONTDLG.RES:                    FONTDLG.RC \
                                FONTDLG.H
        RC -r FONTDLG.RC FONTDLG.RES

FONTDLG.OBJ:                    FONTDLG.C \
                                FONTDLG.H
        ICC -C+ -Kb+ -Ss+ -Ti FONTDLG.C
