SLIDER.EXE:                     SLIDER.OBJ \
                                SLIDER.RES
        LINK386 @<<
SLIDER
SLIDER
SLIDER
OS2386
SLIDER
<<
        RC SLIDER.RES SLIDER.EXE

SLIDER.RES:                     SLIDER.RC \
                                SLIDER.H
        RC -r SLIDER.RC SLIDER.RES

SLIDER.OBJ:                     SLIDER.C \
                                SLIDER.H
        ICC -C+ -Kb+ -Ss+ SLIDER.C
