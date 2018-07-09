BASE=CNR3

ICCOPTS=-C+ -Gd- -Ge+ -Gm+ -Kb+ -Ms -O- -Re -S2ae -Sp -Ss+ -W3
LINKOPTS=/MAP /A:16

!IFDEF DEBUG
ICCOPTS=$(ICCOPTS) -Ti
LINKOPTS=$(LINKOPTS) /CO
!ENDIF

$(BASE).EXE:                    $(BASE).OBJ \
                                $(BASE).RES
        LINK386 $(LINKOPTS) @<<
$(BASE)
$(BASE)
$(BASE)
OS2386
$(BASE)
<<
        RC $(BASE).RES $(BASE).EXE

$(BASE).RES:                    $(BASE).RC \
                                RC.H
        RC -r $(BASE).RC $(BASE).RES

$(BASE).OBJ:                    $(BASE).C \
                                RC.H
        ICC $(ICCOPTS) $(BASE).C
