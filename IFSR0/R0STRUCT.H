/******************************************************************************
*******************************************************************************
**
** EXR0R3 - A ring 0/ring 3 IFS skeleton
** Copyright (C) 1993 by Andre Asselin
**
** R0STRUCT.H - Ring 0 side structures
**
** History:
** 6/22/93 - created
**
*******************************************************************************
******************************************************************************/

typedef unsigned long SEM;

typedef struct cpdata {
   // Ring 0 is messing with the buffers
   SEM BufLock;

   // A command is in the buffers
   SEM CmdReady;

   // The results of a command are in the buffers
   SEM CmdComplete;

   // Pointer to the shared operation buffer
   OPDATA *OpData;

   // Pointer to the shared data buffer
   void *Buf;
} CPDATA;

#define MAXCPRDYWAIT (10*1000)
#define MAXCPRESWAIT (10*1000)
