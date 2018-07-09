/******************************************************************************
*******************************************************************************
**
** EXR0R3 - A ring 0/ring 3 IFS skeleton
** Copyright (C) 1993 by Andre Asselin
**
** R0GLOBAL.C - Ring 0 side global variables
**
** History:
** 6/22/93 - created
**
*******************************************************************************
******************************************************************************/

#include "r0inc.h"

// Save area for IFS command line
char CmdLineSave[256];

// Pointer to DevHlp entry point
void (*DevHelp)();

// Data block to hold info on control program
CPDATA CPData;

// 0=control program is unattached, 1=CP is attached
int CPAttached;

// Locks for the control program's buffers
LINADDR Lock1, Lock2;

// Process ID of the control program
PID CPPID;
