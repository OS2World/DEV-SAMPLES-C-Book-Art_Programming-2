LIBPATH.EXE:                    LIBPATH.OBJ
        LINK386 @<<
LIBPATH
LIBPATH
LIBPATH
OS2386
LIBPATH
<<

LIBPATH.OBJ:                    LIBPATH.C
        ICC -C+ -Kb+ -Ss+ LIBPATH.C
