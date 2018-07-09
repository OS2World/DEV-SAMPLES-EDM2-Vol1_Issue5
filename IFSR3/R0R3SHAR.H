/******************************************************************************
*******************************************************************************
**
** EXR0R3 - A ring 0/ring 3 IFS skeleton
** Copyright (C) 1993 by Andre Asselin
**
** R0R3SHAR.H - Shared structures between the ring 0 and ring 3 piece
**
** History:
** 6/22/93 - created
**
*******************************************************************************
******************************************************************************/

enum { FSCTL_FUNC_INIT = 0x9000, FSCTL_FUNC_NEXT };

#if defined (__OS2__)
typedef void *LINADDR;
#endif

typedef struct InitStruc {
   LINADDR OpData;
   LINADDR Buf;
} INITSTRUC;

// Make enumerations take a byte or word if possible
#pragma option -b-

typedef enum CPFUNCCODES {
   CPFC_ATTACH = 0x8000, 
   CPFC_CHDIR
} CPFUNCCODES;

typedef struct OpData {
   union {
      CPFUNCCODES funccode;
      unsigned short rc;
   };
   union {
      struct {
         unsigned short flag;
         struct vpfsd vpfsd;
         struct cdfsd cdfsd;
         unsigned short Len;
         char Dev[CCHMAXPATH];
      } attach;
      struct {
         long b;
      } op2;
   };
} OPDATA;
