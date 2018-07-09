.AUTODEPEND

#               *Translator Definitions*
CC = bcc +R0.CFG
TASM = TASM
TLIB = tlib
TLINK = tlink
LIBPATH = C:\BORLANDC\LIB;e:\os2
CINCL = C:\BORLANDC\INCLUDE;e:\TOOLKT21\C\OS2H
AINCL = e:\TOOLKT21\ASM\OS2INC


#               *Implicit Rules*
.c.obj:
  $(CC) -c {$< }

#               *List Macros*


EXE_dependencies =  \
 r0stubs.obj \
 r0comm.obj \
 r0global.obj \
 r0devhlp.obj \
 c0.obj \
 {$(LIBPATH)}fshelper.lib

#               *Explicit Rules*
exr0r3.ifs: r0.cfg r0.def $(EXE_dependencies)
  $(TLINK) /n/e/s/c/Tod/L$(LIBPATH) @&&|
c0.obj+
r0stubs.obj+
r0comm.obj+
r0global.obj+
r0devhlp.obj
exr0r3.ifs,r0
fshelper.lib+
cl.lib+
doscalls.lib
r0.def
|


#               *Individual File Dependencies*
r0stubs.obj: r0.cfg r0stubs.c

r0comm.obj: r0.cfg r0comm.c

r0global.obj: r0.cfg r0global.c

r0devhlp.obj: r0.cfg r0devhlp.asm
        $(TASM) /MX /i$(AINCL) R0DEVHLP.ASM,R0DEVHLP.OBJ

c0.obj: r0.cfg c0.asm
        $(TASM) /d__LARGE__ /MX /i$(AINCL) c0.ASM,c0.OBJ

#               *Compiler Configuration File*
r0.cfg: r0.mak
  copy &&|
#-S
-N-
-a-
-R
-ml!
-3
-f-
-C
-G
-O
-Og
-Oe
-Om
-Ov
-Ol
-Op
-O-i
-Z
-pr
-k-
-d
-vi-
-H=R0.CSM
-weas
-wpre
-I$(CINCL)
-L$(LIBPATH)
-P.C
| r0.cfg
