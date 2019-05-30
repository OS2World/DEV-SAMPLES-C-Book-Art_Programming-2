SLIDERTH.EXE:                   SLIDERTH.OBJ \
                                SLIDERTH.RES
        LINK386 @<<
SLIDERTH
SLIDERTH
SLIDERTH
OS2386
SLIDERTH
<<
        RC SLIDERTH.RES SLIDERTH.EXE

SLIDERTH.RES:                   SLIDERTH.RC \
                                SLIDERTH.H
        RC -r SLIDERTH.RC SLIDERTH.RES

SLIDERTH.OBJ:                   SLIDERTH.C \
                                SLIDERTH.H
        ICC -C+ -Gm+ -Kb+ -Ss+ SLIDERTH.C
