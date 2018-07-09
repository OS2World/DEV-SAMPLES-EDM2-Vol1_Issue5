/******************************************************************************
*******************************************************************************
**
** EXR0R3 - A ring 0/ring 3 IFS skeleton
** Copyright (C) 1993 by Andre Asselin
**
** R3COMM.C - Ring 3 side of communications
**
** History:
** 6/29/93 - created
**
*******************************************************************************
******************************************************************************/

#include "r3inc.h"

#pragma argsused
int main(int argc, char **argv, char **envp) {
   INITSTRUC initdata;
   unsigned long leninitdata = sizeof(initdata);
   OPDATA *OpData;
   void *Buf;
   unsigned int rc;
   char name[10];

   // Print a banner to let the user know we're running
   printf("EXR0R3 - A ring 0/ring 3 IFS skeleton\n"
          "Copyright (C) 1993 by Andre Asselin\n\n");


   // Allocate the two buffers
   rc = DosAllocMem(&OpData, sizeof(initdata.OpData),
                    PAG_READ | PAG_WRITE | PAG_COMMIT);
   if (rc != NO_ERROR) {
      printf("alloc failed - OpData: %d\n", rc);
      exit(1);
   }
   rc = DosAllocMem(&Buf, 65536, PAG_READ | PAG_WRITE | PAG_COMMIT);
   if (rc != NO_ERROR) {
      printf("alloc failed - Buf: %d\n", rc);
      exit(1);
   }


   // Set up the initialization data to be passed to the IFS
   initdata.OpData = OpData;
   initdata.Buf = Buf;

   // We want the FS router to route by IFS name, so copy the name into a
   // temporary buffer because the router could destroy the name.
   strncpy(name, FS_NAME, sizeof(name));

   // Call the IFS to initialize
   rc = DosFSCtl(NULL, 0, NULL, &initdata, sizeof(initdata), &leninitdata,
                 FSCTL_FUNC_INIT, name, -1, FSCTL_FSDNAME);
   if (rc != NO_ERROR) {
      printf("FSCtl failed - init: %x %d\n", rc, rc & 0x0fff);
      exit(1);
   }


   // We returned from the IFS-- that means we have an operation waiting.
   // Figure out what it is and route it appropriately.
   while (1) {
      switch (OpData->funccode) {
      case CPFC_ATTACH:
         // Handle FS_ATTACH
         printf("FS_ATTACH\n");
         OpData->rc = FSD_ATTACH(OpData->attach.flag, OpData->attach.Dev,
                                 &OpData->attach.vpfsd, &OpData->attach.cdfsd,
                                 Buf, &OpData->attach.Len);
         break;

      // For now, we don't support anything else
      default:
         printf("Unknown IFS function code: %d\n", OpData->funccode);
         OpData->rc = ERROR_NOT_SUPPORTED;
         break;
      }

      // We want the FS router to route by IFS name, so copy the name into a
      // temporary buffer because the router could destroy the name.
      strncpy(name, FS_NAME, sizeof(name));

      // Say we're done with this one, and wait for next one.
      rc = DosFSCtl(NULL, 0, NULL, NULL, 0, NULL, FSCTL_FUNC_NEXT, name,
                    -1, FSCTL_FSDNAME);
      if (rc != NO_ERROR) {
         printf("FSCtl failed - next: %d\n", rc);
         exit(1);
      }
   }
}
