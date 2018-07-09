/******************************************************************************
*******************************************************************************
**
** EXR0R3 - A ring 0/ring 3 IFS skeleton
** Copyright (C) 1993 by Andre Asselin
**
** R0COMM.C - Ring 0 side of communications
**
** History:
** 5/25/93 - created
**
*******************************************************************************
******************************************************************************/

#include "r0inc.h"


// Local prototypes
static void DetachCP(void);


/******************************************************************************
**
** FS_INIT - Initialize the IFS
**
** Parameters
** ---------
** char far *szParm                    pointer to command line parameters
** unsigned long pDevHlp               pointer to DevHlp entry point
** unsigned long far *pMiniFSD         pointer to data passed between the
**                                     mini-FSD and the IFS
**
******************************************************************************/


#pragma argsused
short int far pascal _export _loadds FS_INIT(char far *szParm,
                                             void (*pDevHlp)(void),
                                             unsigned long far *pMiniFSD)
{
   SEL GDTSels[2];
   int rc;
   static char signon[]="EXR0R3.IFS - A ring 0/ring 3 IFS skeleton\r\n"
                        "Copyright (C) 1993 by Andre Asselin\r\n";

   // Put up banner
   DosPutMessage(1, sizeof(signon)-1, signon);

   // Run C runtime startup code
   Startup();

   // If there were any command line parameters, save them
   if (szParm != NULL) {
      strncpy(CmdLineSave, szParm, sizeof(CmdLineSave));
   }

   // Save the DevHelp entry point
   DevHelp = pDevHlp;

   // Allocate virtual memory for the two lock handles
   rc = VMAlloc(VMAPA_NOPHYSADDR, 2 * sizeof(HLOCK),
                VMAF_SWAPPABLE | VMAF_GLOBALSPACE, &Lock1);
   if (rc != NO_ERROR)
      return rc;
   Lock2 = Lock1 + sizeof(HLOCK);

   // Allocate 2 GDT selectors to communicate with the control program
   rc = AllocGDTSelector(2, GDTSels);
   if (rc != NO_ERROR)
      return rc;

   // Set the pointers to the control program communication buffer
   CPData.OpData = (OPDATA *)MK_FP(GDTSels[0], 0);
   CPData.Buf = (void *)MK_FP(GDTSels[1], 0);

   // Say the control program has never been attached
   CPAttached = -1;

   return NO_ERROR;
}


/******************************************************************************
**
** FS_SHUTDOWN - Prepare for a system shutdown
**
** Parameters
** ---------
** unsigned short usType               flavor of call
**   values:
**     SD_BEGIN                  begining the shutdown process
**     SD_COMPLETE               finishing the shutdown process
** unsigned long ulReserved            reserved
**
******************************************************************************/

#pragma argsused
short int far pascal _export _loadds FS_SHUTDOWN(unsigned short usType,
                                                 unsigned long ulReserved)
{
   return NO_ERROR;
}


/******************************************************************************
**
** FS_FSCTL - Extended IFS control
**
** Parameters
** ---------
** union argdat far *pArgdat
** unsigned short iArgType             argument type
**   values:
**     FSCTL_ARG_FILEINSTANCE    filehandle directed
**     FSCTL_ARG_CURDIR          pathname directed
**     FSCTL_ARG_NULL            FSD directed
** unsigned short func                 function code
**   values:
**     FSCTL_FUNC_NONE           NULL function
**     FSCTL_FUNC_NEW_INFO       return error code information
**     FSCTL_FUNC_EASIZE         return max EA size and max EA list size
** char far *pParm                     UNVERIFIED pointer to parameter area
** unsigned short lenParm              size of area pointed to by pParm
** unsigned short far *plenParmOut     length of parameters passed in pParm
** char far *pData                     UNVERIFIED pointer to information area
** unsigned short lenData              size of area pointed to by pData
** unsigned short far *plenDataOut     length of parameters passed in pData
**
******************************************************************************/

#pragma argsused
short int far pascal _export _loadds FS_FSCTL(union argdat far *pArgdat,
                                              unsigned short iArgType,
                                              unsigned short func,
                                              char far *pParm,
                                              unsigned short lenParm,
                                              unsigned short far *plenParmOut,
                                              char far *pData,
                                              unsigned short lenData,
                                              unsigned short far *plenDataOut)
{
   int rc;
   SEL *LInfoSeg;

   switch (func) {
   case FSCTL_FUNC_NONE:
        return NO_ERROR;

   case FSCTL_FUNC_NEW_INFO:
   case FSCTL_FUNC_EASIZE:
        return ERROR_NOT_SUPPORTED;

   case FSCTL_FUNC_INIT:
      // pParm points to an InitStruc

      // If control program has never been attached, ignore these checks
      if (CPAttached != -1) {
         // Make sure the control program isn't already attached
         if (CPAttached)
            return (ERROR_INVALID_PARAMETER);

         // Make sure that it is OK to attach at this time
         rc = FSH_SEMWAIT(&CPData.BufLock, 0);
         if (rc != NO_ERROR)
            return (ERROR_INVALID_PARAMETER);
      }

      // Verify size of and addressability of the pParm area
      if (lenParm != sizeof(INITSTRUC))
         return ERROR_INVALID_PARAMETER;
      rc = FSH_PROBEBUF(PB_OPREAD, pParm, lenParm);
      if (rc != NO_ERROR)
         return rc;

      // Lock down the operation buffer
      rc = VMLock(((INITSTRUC *)pParm)->OpData, sizeof(OPDATA), VMLPL_NOPAGELIST,
                  Lock1, VMLAF_WRITE | VMLAF_LONG, NULL);
      if (rc != NO_ERROR)
         return rc;

      // Map the GDT selector we got at init time to the memory we just locked
      rc = LinToGDTSelector(FP_SEG(CPData.OpData), 
                            ((INITSTRUC *)pParm)->OpData, sizeof(OPDATA));
      if (rc != NO_ERROR)
         return rc;

      // Lock down the data buffer
      rc = VMLock(((INITSTRUC *)pParm)->Buf, 65536, VMLPL_NOPAGELIST,
                  Lock2, VMLAF_WRITE | VMLAF_LONG, NULL);
      if (rc != NO_ERROR)
         return rc;

      // Map the GDT selector we got at init time to the memory we just locked
      rc = LinToGDTSelector(FP_SEG(CPData.Buf), ((INITSTRUC *)pParm)->Buf,
                            65536);
      if (rc != NO_ERROR)
         return rc;

      // The operation data area is now OK to use
      rc = FSH_SEMCLEAR(&CPData.BufLock);
      if (rc != NO_ERROR)
         return rc;

      // Get PID of the control program
      rc = GetDOSVar(DHGETDOSV_LOCINFOSEG, (unsigned long *)&LInfoSeg);
      if (rc != NO_ERROR)
         return rc;

      CPPID = ((LINFOSEG *)MK_FP(LInfoSeg, 0))->pidCurrent;

      // The control program is now attached
      CPAttached = 1;

      /* FALLTHRU */

   case FSCTL_FUNC_NEXT:
      if (!CPAttached)
         return ERROR_BAD_COMMAND;

      // Set the semaphore that indicates the control program is ready for
      // another operation
      rc = FSH_SEMSET(&CPData.CmdReady);
      if (rc == ERROR_INTERRUPT) {
         DetachCP();
         return rc;
      }

      // Clear the semaphore that indicates the control program just returned
      rc = FSH_SEMCLEAR(&CPData.CmdComplete);
      if (rc == ERROR_INTERRUPT) {
         DetachCP();
         return rc;
      }

      // Wait for the semaphore that indicates another operation is ready
      rc = FSH_SEMWAIT(&CPData.CmdReady, -1);
      if (rc == ERROR_INTERRUPT)
         DetachCP();

      return rc;

   default:
      return ERROR_NOT_SUPPORTED;
   }
}



/******************************************************************************
**
** FS_EXIT - Notify FSD that a process is ending
**
** Parameters
** ---------
** unsigned short uid                   user ID of process
** unsigned short pid                   process ID of process
** unsigned short pdb                   DOS mode process ID of process
**
******************************************************************************/

#pragma argsused
void far pascal _export _loadds FS_EXIT(unsigned short uid, unsigned short pid,
                                        unsigned short pdb)
{
   // If the control program is exiting, detach ourselves from it
   if (CPAttached == 1 && pid == CPPID)
      DetachCP();
}


/******************************************************************************
**
** DetachCP - Detach the control program from the IFS
**
******************************************************************************/

static void DetachCP(void) {
   CPAttached = 0;
   VMUnlock(Lock1);
   VMUnlock(Lock2);
   FSH_SEMCLEAR(&CPData.CmdComplete);
}
