/******************************************************************************
*******************************************************************************
**
** EXR0R3 - A ring 0/ring 3 IFS skeleton
** Copyright (C) 1993 by Andre Asselin
**
** R0DEVHLP.H - C interface to DevHelps
**
** History:
** 6/23/93 - created
**
*******************************************************************************
******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

typedef unsigned short SEL;
typedef char HLOCK[12];
typedef unsigned long LINADDR;
typedef unsigned long PHYSADDR;
typedef struct PageList_s {
   PHYSADDR pl_PhysAddr;
   unsigned long pl_cb;
} PAGELIST;

unsigned short pascal AllocGDTSelector(unsigned short NumSels,
                                       SEL far *pSelArr);

/* Dos variable indexes used in DevHlp_GetDOSVar */
#define DHGETDOSV_SYSINFOSEG         1
#define DHGETDOSV_LOCINFOSEG         2
#define DHGETDOSV_VECTORSDF          4
#define DHGETDOSV_VECTORREBOOT       5
#define DHGETDOSV_YIELDFLAG          7
#define DHGETDOSV_TCYIELDFLAG        8
#define DHGETDOSV_DOSCPTAG          11

/* Global Info Seg */

typedef struct _GINFOSEG {      
    ULONG   time;
    ULONG   msecs;
    UCHAR   hour;
    UCHAR   minutes;
    UCHAR   seconds;
    UCHAR   hundredths;
    USHORT  timezone;
    USHORT  cusecTimerInterval;
    UCHAR   day;
    UCHAR   month;
    USHORT  year;
    UCHAR   weekday;
    UCHAR   uchMajorVersion;
    UCHAR   uchMinorVersion;
    UCHAR   chRevisionLetter;
    UCHAR   sgCurrent;
    UCHAR   sgMax;
    UCHAR   cHugeShift;
    UCHAR   fProtectModeOnly;
    USHORT  pidForeground;
    UCHAR   fDynamicSched;
    UCHAR   csecMaxWait;
    USHORT  cmsecMinSlice;
    USHORT  cmsecMaxSlice;
    USHORT  bootdrive;
    UCHAR   amecRAS[32];
    UCHAR   csgWindowableVioMax;
    UCHAR   csgPMMax;
} GINFOSEG;
typedef GINFOSEG far *PGINFOSEG;

/* local info seg */

typedef struct _LINFOSEG {      
    PID     pidCurrent;
    PID     pidParent;
    USHORT  prtyCurrent;
    TID     tidCurrent;
    USHORT  sgCurrent;
    UCHAR   rfProcStatus;
    UCHAR   dummy1;
    USHORT  fForeground;
    UCHAR   typeProcess;
    UCHAR   dummy2;
    SEL     selEnvironment;
    USHORT  offCmdLine;
    USHORT  cbDataSegment;
    USHORT  cbStack;
    USHORT  cbHeap;
    USHORT  hmod;
    SEL     selDS;
} LINFOSEG;
typedef LINFOSEG far *PLINFOSEG;

unsigned short pascal GetDOSVar(USHORT Index, unsigned long far *Result);

unsigned short pascal LinToGDTSelector(SEL Selector,
                                       LINADDR LinearAddress,
                                       unsigned long Size);

#define VMAF_16M              0x0001
#define VMAF_FIXED            0x0002
#define VMAF_SWAPPABLE        0x0004
#define VMAF_CONTIGUOUS       0x0008
#define VMAF_MAPPHYS          0x0010
#define VMAF_GLOBALSPACE      0x0020
#define VMAF_SCREENGROUP      0x0040
#define VMAF_NOCOMMIT         0x0100
#define VMAPA_NOPHYSADDR      -1L

unsigned short pascal VMAlloc(PHYSADDR PhysAddr, ULONG Size, ULONG Flags,
                              LINADDR far *LinAddr);
 
#define VMLAF_NOBLOCK         0x0001
#define VMLAF_CONTIGUOUS      0x0002
#define VMLAF_16M             0x0004
#define VMLAF_WRITE           0x0008
#define VMLAF_LONG            0x0010
#define VMLAF_VERIFY          0x0020
#define VMLPL_NOPAGELIST      -1L
 
unsigned short pascal VMLock(LINADDR LinearAddress,
                             unsigned long Length,
                             LINADDR pPageList,
                             LINADDR pLockHandle,
                             unsigned long ActionFlags,
                             unsigned long far *NumElements);

unsigned short pascal VMUnlock(LINADDR LockHandle);

#if defined(__cplusplus)
}
#endif
