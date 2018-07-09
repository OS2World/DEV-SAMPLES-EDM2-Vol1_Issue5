/******************************************************************************
*******************************************************************************
**
** EXR0R3 - A ring 0/ring 3 IFS skeleton
** Copyright (C) 1993 by Andre Asselin
**
** R3INC.H - Ring 3 side includes
**
** History:
** 6/29/93 - created
**
*******************************************************************************
******************************************************************************/

#define INCL_DOSERRORS
#define INCL_NOCOMMON
#define INCL_NOPMAPI
#define INCL_DOSMEMMGR
#define INCL_DOSFILEMGR
#include <os2.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define far
#include "fsd.h"
#include "r0r3shar.h"
#include "r3global.h"

