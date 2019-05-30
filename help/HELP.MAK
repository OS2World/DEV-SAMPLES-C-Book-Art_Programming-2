
HELP.EXE:                     HELP.OBJ HELP.RES HELP.HLP
        LINK386 @<<
HELP,
HELP,
HELP,
OS2386
HELP
<<
        RC HELP.RES HELP.EXE

HELP.OBJ:                     HELP.C HELP.H
        ICC -C+ -Kb+ -Ss+ HELP.C

HELP.RES:                     HELP.RC HELP.H
        RC -r HELP.RC HELP.RES

HELP.HLP:                     HELP.IPF
        IPFC HELP.IPF
