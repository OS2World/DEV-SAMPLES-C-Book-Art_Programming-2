FILEDLG.EXE:                    FILEDLG.OBJ \
                                FILEDLG.RES
        LINK386 @<<
FILEDLG
FILEDLG
FILEDLG
OS2386
FILEDLG
<<
        RC FILEDLG.RES FILEDLG.EXE

FILEDLG.RES:                    FILEDLG.RC \
                                FILEDLG.H
        RC -r FILEDLG.RC FILEDLG.RES

FILEDLG.OBJ:                    FILEDLG.C \
                                FILEDLG.H
        ICC -C+ -Kb+ -Ss+ FILEDLG.C
