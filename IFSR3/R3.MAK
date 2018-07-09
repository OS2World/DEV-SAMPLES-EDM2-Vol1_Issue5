#=============================================================
#
#       R3.MAK - Makefile for project D:\PROG\R3\R3.PRJ
#               Created on 06/30/93 at 01:00
#
#=============================================================

.AUTODEPEND

#=============================================================
#               Translator Definitions
#=============================================================
CC = bcc +R3.CFG
TASM = tasm
TLINK = tlink
LIBPATH = C:\BCOS2\LIB
INCLUDEPATH = C:\BCOS2\INCLUDE


#=============================================================
#               Implicit Rules
#=============================================================
.c.obj:
  $(CC) -c {$< }

.asm.obj:
  $(TASM) -Mx $*.asm,$*.obj

#=============================================================
#               List Macros
#=============================================================


EXE_DEPENDENCIES =  \
 r3comm.obj \
 r3stubs.obj

#=============================================================
#               Explicit Rules
#=============================================================

all: r3.exe fsatt.exe

r3.exe: r3.cfg $(EXE_DEPENDENCIES)
  $(TLINK) /x /v /B:0x10000 /c /Toe /ap /L$(LIBPATH) @&&|
C:\BCOS2\LIB\C02.OBJ+
r3comm.obj+
r3stubs.obj
r3

C:\BCOS2\LIB\C2.LIB+
C:\BCOS2\LIB\OS2.LIB

|

fsatt.exe: r3.cfg fsatt.obj
  $(TLINK) /x /v /B:0x10000 /c /Toe /ap /L$(LIBPATH) @&&|
C:\BCOS2\LIB\C02.OBJ+
fsatt.obj
fsatt

C:\BCOS2\LIB\C2.LIB+
C:\BCOS2\LIB\OS2.LIB

|


#=============================================================
#               Individual File Dependencies
#=============================================================
r3comm.obj: r3.cfg r3comm.c

R3STUBS.obj: r3.cfg R3STUBS.C

fsatt.obj: r3.cfg fsatt.c

#=============================================================
#               Compiler Configuration File
#=============================================================
r3.cfg: r3.mak
  copy &&|
-Oi
-Oz
-Ob
-Oe
-Oc
-L$(LIBPATH)
-I$(INCLUDEPATH)
-P
-H=D:\PROG\R3\R3.CSM
-vi-
-d
-O
-Ot
-v
-N
-C
-a
| r3.cfg

