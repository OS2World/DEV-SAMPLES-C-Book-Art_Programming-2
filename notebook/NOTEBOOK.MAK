NOTEBOOK.EXE:                   NOTEBOOK.OBJ \
                                NOTEBOOK.RES
        LINK386 @<<
NOTEBOOK
NOTEBOOK
NOTEBOOK
OS2386
NOTEBOOK
<<
        RC NOTEBOOK.RES NOTEBOOK.EXE

NOTEBOOK.RES:                   NOTEBOOK.RC \
                                NOTEBOOK.H
        RC -r NOTEBOOK.RC NOTEBOOK.RES

NOTEBOOK.OBJ:                   NOTEBOOK.C \
                                NOTEBOOK.H
        ICC -C+ -Kb+ -Ss+ NOTEBOOK.C
