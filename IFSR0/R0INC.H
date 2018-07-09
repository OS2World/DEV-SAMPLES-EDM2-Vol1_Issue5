/******************************************************************************
*******************************************************************************
**
** EXR0R3 - A ring 0/ring 3 IFS skeleton
** Copyright (C) 1993 by Andre Asselin
**
** R0INC.H - Include file for ring 0 side
**
** History:
** 6/21/93 - created
**
*******************************************************************************
******************************************************************************/

#define INCL_DOSERRORS
#define INCL_NOCOMMON
#define INCL_NOPMAPI
#define _Seg16
#define _Far16
#define _System
#include <os2.h>
#include "os216.h"
#include <stddef.h>
#include <string.h>
#include <dos.h>
#include "fsd.h"
#include "r0devhlp.h"
#include "r0r3shar.h"
#include "r0struct.h"
#include "fsh.h"

#include "r0global.h"

