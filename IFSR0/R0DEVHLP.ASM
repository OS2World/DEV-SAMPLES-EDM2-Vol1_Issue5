;******************************************************************************
;******************************************************************************
;*
;* EXR0R3 - A ring 0/ring 3 IFS skeleton
;* Copyright (C) 1993 by Andre Asselin
;*
;* R0DEVHLP.ASM - C interface to DevHelps
;*
;* History:
;* 6/23/93 - created
;*
;******************************************************************************
;******************************************************************************

        .386P
   .MODEL FARSTACK USE16 OS2 LARGE, PASCAL
   LOCALS

;; Includes from the OS/2 Toolkit
INCL_DOSERRORS  EQU     1
   INCLUDE      BSEERR.INC
   INCLUDE      DEVHLP.INC


   .DATA
   EXTRN _DevHelp:PTR PROC


   .CODE

PUBLIC ALLOCGDTSELECTOR
ALLOCGDTSELECTOR PROC
ARG @@NumSels:WORD, @@pSelArr:PTR WORD
USES DI
   les   di, @@pSelArr
   mov   cx, @@NumSels
   mov   dl, DevHlp_AllocGDTSelector
   call  [_DevHelp]
   jc    @@out
   mov   ax, NO_ERROR

@@out:
   ret
ENDP


PUBLIC GETDOSVAR
GETDOSVAR PROC
ARG @@Index:WORD, @@Result:PTR DWORD
   mov   al, BYTE PTR @@Index
   mov   dl, DevHlp_GetDOSVar
   call  [_DevHelp]
   jc    @@out
 
   mov   cx, bx
   les   bx, @@Result
   mov   es:[bx], cx
   mov   es:[bx+2], ax
   mov   ax, NO_ERROR

@@out:
   ret
ENDP


PUBLIC LINTOGDTSELECTOR
LINTOGDTSELECTOR PROC
ARG @@GDTSel:WORD, @@LinAddr:LARGE NEAR PTR, @@RgnSize:DWORD
   mov   ax, @@GDTSel
   mov   ebx, @@LinAddr
   mov   ecx, @@RgnSize
   mov   dl, DevHlp_LinToGDTSelector
   call  [_DevHelp]
   jc    @@out
   mov   ax, NO_ERROR

@@out:
   ret
ENDP


PUBLIC VMALLOC
VMALLOC PROC
ARG @@PhysAddr:DWORD, @@Size:DWORD, @@Flags:DWORD, @@LinAddr:PTR DWORD
USES EDI
   mov   ecx, @@Size
   mov   edi, @@PhysAddr
   mov   eax, @@Flags
   mov   dl, DevHlp_VMAlloc
   call  [_DevHelp]
   jc    @@out

   les   bx, @@LinAddr
   mov   es:[bx], eax
   mov   ax, NO_ERROR

@@out:
   ret
ENDP


PUBLIC VMLOCK
VMLOCK PROC
ARG @@LinAddr:LARGE NEAR PTR, @@RgnSize:DWORD, @@pPageList:LARGE NEAR PTR, \
    @@pLockHandle:LARGE NEAR PTR, @@ActionFlags:DWORD, @@NumElements:PTR DWORD
USES ESI, EDI
   mov   ebx, @@LinAddr
   mov   ecx, @@RgnSize
   mov   edi, @@pPageList
   mov   esi, @@pLockHandle
   mov   eax, @@ActionFlags
   mov   dl, DevHlp_VMLock
   call  [_DevHelp]
   jc    @@out

   cmp   @@pPageList, -1
   je    @@nosave
   les   bx, @@NumElements
   mov   es:[bx], eax

@@nosave:
   mov   ax, NO_ERROR

@@out:
   ret
ENDP


PUBLIC VMUNLOCK
VMUNLOCK PROC
ARG @@pLockHandle:LARGE NEAR PTR
   mov   esi, @@pLockHandle
   mov   dl, DevHlp_VMUnlock
   call  [_DevHelp]
   jc    @@out
   mov   ax, NO_ERROR

@@out:
   ret
ENDP

   END
