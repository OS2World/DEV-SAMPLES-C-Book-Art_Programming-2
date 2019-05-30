SLIDEROB.EXE:                   SLIDEROB.OBJ \
                                SLIDEROB.RES
        LINK386 @<<
SLIDEROB
SLIDEROB
SLIDEROB
OS2386
SLIDEROB
<<
        RC SLIDEROB.RES SLIDEROB.EXE

SLIDEROB.RES:                   SLIDEROB.RC \
                                SLIDEROB.H
        RC -r SLIDEROB.RC SLIDEROB.RES

SLIDEROB.OBJ:                   SLIDEROB.C \
                                SLIDEROB.H
        ICC -C+ -Gm+ -Kb+ -Ss+ SLIDEROB.C
