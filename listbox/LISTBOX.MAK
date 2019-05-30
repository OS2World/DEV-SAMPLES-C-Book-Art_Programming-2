LISTBOX.EXE:                    LISTBOX.OBJ \
                                LISTBOX.RES
        LINK386 @<<
LISTBOX
LISTBOX
LISTBOX
OS2386
LISTBOX
<<
        RC LISTBOX.RES LISTBOX.EXE

LISTBOX.RES:                    LISTBOX.RC \
                                LISTBOX.H
        RC -r LISTBOX.RC LISTBOX.RES

LISTBOX.OBJ:                    LISTBOX.C \
                                LISTBOX.H
        ICC -C+ -Kb+ -Ss+ LISTBOX.C
