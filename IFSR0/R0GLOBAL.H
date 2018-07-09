/******************************************************************************
*******************************************************************************
**
** EXR0R3 - A ring 0/ring 3 IFS skeleton
** Copyright (C) 1993 by Andre Asselin
**
** R0GLOBAL.H - Ring 0 side global variables
**
** History:
** 6/22/93 - created
**
*******************************************************************************
******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

// Save area for IFS command line
extern char CmdLineSave[256];

// Pointer to DevHlp entry point
extern void (*DevHelp)();

// Data block to hold info on control program
extern CPDATA CPData;

// 0=control program is unattached, 1=CP is attached, -1=CP never attached
extern int CPAttached;

// Startup the C library
extern void far pascal Startup(void);

// Locks for the control program's buffers
extern LINADDR Lock1, Lock2;

// Process ID of the control program
extern PID CPPID;

#if defined(__cplusplus)
}
#endif
