/******************************************************************************
*******************************************************************************
**
** EXR0R3 - A ring 0/ring 3 IFS skeleton
** Copyright (C) 1993 by Andre Asselin
**
** OS216.H - 16-bit OS/2 definitions
**
** History:
** 6/29/93 - created
**
*******************************************************************************
******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

#define stdin  0
#define stdout 1
#define stderr 2

USHORT far pascal DosPutMessage(USHORT hfile,
                                USHORT cbMsg,
                                PCHAR pBuf);

#if defined(__cplusplus)
}
#endif
