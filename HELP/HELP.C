#define INCL_WIN
#include <os2.h>
#include <stdio.h>
#include "rc.h"

#define CLS_CLIENT               "ClientClass"

BOOL EXPENTRY helpHook(HAB habAnchor,
                       SHORT sMode,
                       SHORT sTopic,
                       SHORT sSubTopic,
                       PRECTL prclPosition)
//-------------------------------------------------------------------------
// This is the help hook installed by main().
//
// Input, Output, Returns:  as with help hooks.
//-------------------------------------------------------------------------
{
   HWND hwndActive;

   //----------------------------------------------------------------------
   // The following checks for the Help button on a message box.  If it
   // isn't, pass the event to the next hook in the call chain.
   //----------------------------------------------------------------------
   if ((sMode==HLPM_WINDOW) && (sSubTopic==1)) {
      //-------------------------------------------------------------------
      // It was ours, so figure out the help instance (any window handle
      // in this process will do on the call to WinQueryHelpInstance(), so
      // call WinQueryActiveWindow() to get the message box window handle)
      // and display the panel.
      //-------------------------------------------------------------------
      hwndActive=WinQueryActiveWindow(HWND_DESKTOP);

      WinSendMsg(WinQueryHelpInstance(hwndActive),
                 HM_DISPLAY_HELP,
                 MPFROMLONG(MAKEULONG(sTopic,0)),
                 MPFROMLONG(HM_RESOURCEID));

      return TRUE;
   } else {
      return FALSE;
   } /* endif */
}

MRESULT EXPENTRY wndProc(HWND hwndWnd,
                         ULONG ulMsg,
                         MPARAM mpParm1,
                         MPARAM mpParm2)
//-------------------------------------------------------------------------
// This is the window procedure for the client window.
//
// Input, Output, Returns:  as with window procedures
//-------------------------------------------------------------------------
{
   switch (ulMsg) {
   case WM_COMMAND:
      switch (SHORT1FROMMP(mpParm1)) {
      case MI_BOX:
         WinMessageBox(HWND_DESKTOP,
                       hwndWnd,
                       "Message box help test",
                       "Test",
                       MSG_TEST,
                       MB_OK|MB_HELP|MB_MOVEABLE);
         break;
      case MI_EXIT:
         WinPostMsg(hwndWnd,WM_CLOSE,0L,0L);
         break;
      default:
         return WinDefWindowProc(hwndWnd,ulMsg,mpParm1,mpParm2);
      } /* endswitch */
      break;
   case WM_PAINT:
      {
         HPS hpsPaint;
         RECTL rclPaint;

         hpsPaint=WinBeginPaint(hwndWnd,NULLHANDLE,&rclPaint);
         WinFillRect(hpsPaint,&rclPaint,SYSCLR_WINDOW);
         WinEndPaint(hpsPaint);
      }
      break;
   case HM_QUERY_KEYS_HELP:
      return MRFROMLONG(KEYSHELP_CLIENT);
   default:
      return WinDefWindowProc(hwndWnd,ulMsg,mpParm1,mpParm2);
   } /* endswitch */

   return MRFROMSHORT(FALSE);
}

INT main(VOID)
{
   HAB habAnchor;
   HMQ hmqQueue;
   ULONG ulCreate;
   HWND hwndFrame;
   HWND hwndClient;
   HELPINIT hiHelp;
   HWND hwndHelp;
   BOOL bLoop;
   QMSG qmMsg;

   habAnchor=WinInitialize(0);
   hmqQueue=WinCreateMsgQueue(habAnchor,0);

   WinRegisterClass(habAnchor,CLS_CLIENT,wndProc,0,0L);

   ulCreate=FCF_SYSMENU | FCF_TITLEBAR | FCF_MINMAX | FCF_SIZEBORDER |
               FCF_MENU | FCF_ACCELTABLE | FCF_TASKLIST | FCF_SHELLPOSITION;

   hwndFrame=WinCreateStdWindow(HWND_DESKTOP,
                                WS_VISIBLE,
                                &ulCreate,
                                CLS_CLIENT,
                                "Help Sample",
                                0L,
                                NULLHANDLE,
                                RES_CLIENT,
                                &hwndClient);
   if (hwndFrame!=NULLHANDLE) {
      //-------------------------------------------------------------------
      // Initialize the HELPINIT structure
      //-------------------------------------------------------------------
      hiHelp.cb=sizeof(hiHelp);
      hiHelp.ulReturnCode=0;
      hiHelp.pszTutorialName=NULL;
      hiHelp.phtHelpTable=(PHELPTABLE)MAKEULONG(RES_CLIENT,0xFFFF);
      hiHelp.hmodHelpTableModule=NULLHANDLE;
      hiHelp.hmodAccelActionBarModule=NULLHANDLE;
      hiHelp.idAccelTable=0;
      hiHelp.idActionBar=0;
      hiHelp.pszHelpWindowTitle="Help Test";
      hiHelp.fShowPanelId=CMIC_HIDE_PANEL_ID;
      hiHelp.pszHelpLibraryName="HELP.HLP";

      //-------------------------------------------------------------------
      // Create the help instance and check for an error
      //-------------------------------------------------------------------
      hwndHelp=WinCreateHelpInstance(habAnchor,&hiHelp);
      if ((hwndHelp!=NULLHANDLE) && (hiHelp.ulReturnCode!=0)) {
         WinDestroyHelpInstance(hwndHelp);
         hwndHelp=NULLHANDLE;
      } else {
         WinAssociateHelpInstance(hwndHelp,hwndFrame);

         //----------------------------------------------------------------
         // Install the hook
         //----------------------------------------------------------------
         WinSetHook(habAnchor,hmqQueue,HK_HELP,(PFN)helpHook,NULLHANDLE);
      } /* endif */

      bLoop=WinGetMsg(habAnchor,&qmMsg,(HWND)NULL,0,0);
      while (bLoop) {
         WinDispatchMsg(habAnchor,&qmMsg);
         bLoop=WinGetMsg(habAnchor,&qmMsg,(HWND)NULL,0,0);
      } /* endwhile */

      if (hwndHelp!=NULLHANDLE) {
         //----------------------------------------------------------------
         // Release the hook and destroy the help instance
         //----------------------------------------------------------------
         WinReleaseHook(habAnchor,hmqQueue,HK_HELP,(PFN)helpHook,NULLHANDLE);
         WinDestroyHelpInstance(hwndHelp);
      } /* endif */

      WinDestroyWindow(hwndFrame);
   } /* endif */

   WinDestroyMsgQueue(hmqQueue);
   WinTerminate(habAnchor);
   return 0;
}
