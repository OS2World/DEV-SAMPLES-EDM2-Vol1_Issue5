#define INCL_WININPUT
#define INCL_WINMENUS
#define INCL_WINPOINTERS
#define INCL_WINSTDCNR
#define INCL_WINSYS
#define INCL_WINWINDOWMGR
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rc.h"

#define CLS_CLIENT               "Cnr2Class"

#define NUM_YEARS                10

//-------------------------------------------------------------------------
// Useful macros:
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// SETMENUATTR sets the specified attribute of the menuitem having the
// specified id.
//-------------------------------------------------------------------------
#define SETMENUATTR(hwndMenu,usId,usAttr,bSet) \
WinSendMsg(hwndMenu, \
           MM_SETITEMATTR, \
           MPFROM2SHORT(usId,TRUE), \
           MPFROM2SHORT(usAttr,(bSet?usAttr:0)))

typedef struct _CLIENTDATA {                          // @1
   USHORT usSzStruct;         // Size of the structure
   HAB habAnchor;             // Anchor block of the window
   HWND hwndFrame;            // Frame of the client (== parent)
   HWND hwndCnr;              // Container window
   HPOINTER hptrYear;         // Icon for the year records
   HPOINTER hptrMonth;        // Icon for the month records
   HWND hwndWndMenu;          // Menu window
} CLIENTDATA, *PCLIENTDATA;

typedef struct _MYCNRREC {
   MINIRECORDCORE mrcCore;    // Base structure
   CHAR achText[64];          // Icon text
} MYCNRREC, *PMYCNRREC;

//-------------------------------------------------------------------------
// PFNSRCH is used by searchCnr().  The parameters are:  container window
// handle, record pointer, and user-data pointer, respectively.
//-------------------------------------------------------------------------
typedef BOOL (*PFNSRCH)(HWND,PVOID,PVOID);

const PCHAR apchMonths[]={
   "January",
   "February",
   "March",
   "April",
   "May",
   "June",
   "July",
   "August",
   "September",
   "October",
   "November",
   "December"
};

BOOL searchSelect(HWND hwndCnr,PMYCNRREC pmcrRecord,PBOOL pbSelect)
//-------------------------------------------------------------------------
// This function is used to select/deselect all records.  Note that it
// always returns FALSE, so that searchCnr() will traverse the entire
// record list.
//
// Input:  hwndCnr - handle of the container window
//         pmcrRecord - pointer to the container record
//         pbSelect - pointer to a BOOL specifying whether to select the
//                    record or not
//-------------------------------------------------------------------------
{
   WinSendMsg(hwndCnr,
              CM_SETRECORDEMPHASIS,
              MPFROMP(pmcrRecord),
              MPFROM2SHORT(*pbSelect,CRA_SELECTED));
   return FALSE;
}

BOOL initCnr(PCLIENTDATA pcdData)
//-------------------------------------------------------------------------
// This function creates and inserts all of the container records
//
// Input:  pcdData - points to the CLIENTDATA structure for the client
// Returns:  TRUE if successful, FALSE otherwise
//-------------------------------------------------------------------------
{
   USHORT usYear;
   PMYCNRREC pmcrYear;
   RECORDINSERT riInsert;
   USHORT usMonth;
   PMYCNRREC pmcrMonth;
   PMYCNRREC pmcrCurrent;

   //----------------------------------------------------------------------
   // As much as I would have liked to allocate 12*NUM_YEARS records
   // and simply call CM_INSERTRECORD the appropriate number of times to
   // establish the correct hierarchy, the container apparently doesn't
   // like it when you say there's only 1 record, yet the preccNextRecord
   // field is not NULL.
   //
   // So, we gotta allocate and insert one record for the year, then 12
   // records for that year's months, repeat ad nauseum.
   //----------------------------------------------------------------------

   for (usYear=0; usYear<NUM_YEARS; usYear++) {       // @2
      //-------------------------------------------------------------------
      // Allocate, initialize, and insert the year record
      //-------------------------------------------------------------------
      pmcrYear=(PMYCNRREC)PVOIDFROMMR(WinSendMsg(pcdData->hwndCnr,
                                                 CM_ALLOCRECORD,
                                                 MPFROMLONG(sizeof(MYCNRREC)-
                                                               sizeof(MINIRECORDCORE)),
                                                 MPFROMSHORT(1)));
      if (pmcrYear==NULL) {
         WinAlarm(HWND_DESKTOP,WA_ERROR);
         return FALSE;
      } /* endif */

      sprintf(pmcrYear->achText,"Year %04d",1993-usYear);

      pmcrYear->mrcCore.pszIcon=pmcrYear->achText;
      pmcrYear->mrcCore.hptrIcon=pcdData->hptrYear;

      riInsert.cb=sizeof(RECORDINSERT);
      riInsert.pRecordOrder=(PRECORDCORE)CMA_FIRST;
      riInsert.pRecordParent=NULL;
      riInsert.fInvalidateRecord=FALSE;
      riInsert.zOrder=CMA_TOP;
      riInsert.cRecordsInsert=1;

      WinSendMsg(pcdData->hwndCnr,
                 CM_INSERTRECORD,
                 MPFROMP(pmcrYear),
                 MPFROMP(&riInsert));

      //-------------------------------------------------------------------
      // Allocate, initialize, and insert the month records
      //-------------------------------------------------------------------
      pmcrMonth=(PMYCNRREC)PVOIDFROMMR(WinSendMsg(pcdData->hwndCnr,
                                                  CM_ALLOCRECORD,
                                                  MPFROMLONG(sizeof(MYCNRREC)-
                                                                sizeof(MINIRECORDCORE)),
                                                  MPFROMSHORT(12)));
      if (pmcrMonth==NULL) {
         WinAlarm(HWND_DESKTOP,WA_ERROR);
         return FALSE;
      } /* endif */

      pmcrCurrent=pmcrMonth;

      for (usMonth=0; usMonth<12; usMonth++) {
         strcpy(pmcrCurrent->achText,apchMonths[usMonth]);

         pmcrCurrent->mrcCore.pszIcon=pmcrCurrent->achText;
         pmcrCurrent->mrcCore.hptrIcon=pcdData->hptrMonth;

         pmcrCurrent=(PMYCNRREC)pmcrCurrent->mrcCore.preccNextRecord;
      } /* endfor */

      riInsert.cb=sizeof(RECORDINSERT);
      riInsert.pRecordOrder=(PRECORDCORE)CMA_FIRST;
      riInsert.pRecordParent=(PRECORDCORE)pmcrYear;
      riInsert.fInvalidateRecord=FALSE;
      riInsert.zOrder=CMA_TOP;
      riInsert.cRecordsInsert=12;

      WinSendMsg(pcdData->hwndCnr,
                 CM_INSERTRECORD,
                 MPFROMP(pmcrMonth),
                 MPFROMP(&riInsert));
   } /* endfor */

   //----------------------------------------------------------------------
   // Arrange the records and invalidate them so that they get redrawn in
   // their proper positions.
   //----------------------------------------------------------------------
   WinSendMsg(pcdData->hwndCnr,CM_ARRANGE,0,0);

   WinSendMsg(pcdData->hwndCnr,                       // @3
              CM_INVALIDATERECORD,
              MPFROMP(NULL),
              MPFROM2SHORT(0,CMA_REPOSITION|CMA_ERASE|CMA_TEXTCHANGED));
   return TRUE;
}

BOOL setCascadeDefault(HWND hwndMenu,USHORT usSubmenu,USHORT usDefault)
//-------------------------------------------------------------------------
// This function sets the default menuitem for the specified cascade menu.
//
// Input:  hwndMenu - specifies the menu window handle.
//         usSubmenu - specifies the id of the cascade menu.
//         usDefault - specifies the id of the default menuitem.
// Returns:  TRUE if successful, FALSE otherwise.
//-------------------------------------------------------------------------
{
   MENUITEM miItem;                                   // @4
   ULONG ulStyle;

   WinSendMsg(hwndMenu,
              MM_QUERYITEM,
              MPFROM2SHORT(usSubmenu,TRUE),
              MPFROMP(&miItem));
   ulStyle=WinQueryWindowULong(miItem.hwndSubMenu,QWL_STYLE);
   ulStyle|=MS_CONDITIONALCASCADE;
   WinSetWindowULong(miItem.hwndSubMenu,QWL_STYLE,ulStyle);

   WinSendMsg(miItem.hwndSubMenu,
              MM_SETDEFAULTITEMID,
              MPFROM2SHORT(usDefault,FALSE),
              0L);
   SETMENUATTR(miItem.hwndSubMenu,usDefault,MIA_CHECKED,TRUE);

   return TRUE;
}

PVOID searchCnr(HWND hwndCnr,PFNSRCH pfnSearch,PVOID pvUser,PVOID pvRsrvd)
//-------------------------------------------------------------------------
// This function performs a post-traversal search on the container records
// until either 1) pfnSearch() returns TRUE (specifying that the record
// matched the criteria in pvUser) or 2) the last record is checked without
// a match being found.  The record, if any, is returned.
//
// Input:  hwndCnr - handle of the container
//         pfnSearch - pointer to the search function
//         pvUser - pointer to user-data, passed to pfnSearch()
//         pvRsrvd - reserved and must be NULL
// Returns:  record found, or NULL otherwise
//-------------------------------------------------------------------------
{
   PVOID pvChild;                                     // @5
   PVOID pvResult;

   //----------------------------------------------------------------------
   // If the reserved parm is NULL, this is not a recursive call.  Get the
   // first child of the record specified here (or the first record if
   // pvRsrvd is NULL).
   //----------------------------------------------------------------------
   if (pvRsrvd==NULL) {
      pvChild=PVOIDFROMMR(WinSendMsg(hwndCnr,
                                     CM_QUERYRECORD,
                                     MPFROMP(NULL),
                                     MPFROM2SHORT(CMA_FIRST,CMA_ITEMORDER)));
   } else {
      pvChild=PVOIDFROMMR(WinSendMsg(hwndCnr,
                                     CM_QUERYRECORD,
                                     MPFROMP(pvRsrvd),
                                     MPFROM2SHORT(CMA_FIRSTCHILD,CMA_ITEMORDER)));
   } /* endif */

   //----------------------------------------------------------------------
   // Process each child
   //----------------------------------------------------------------------
   while (pvChild!=NULL) {
      pvResult=searchCnr(hwndCnr,pfnSearch,pvUser,pvChild);
      if (pvResult!=NULL) {
         return pvResult;
      } /* endif */

      pvChild=PVOIDFROMMR(WinSendMsg(hwndCnr,
                                     CM_QUERYRECORD,
                                     MPFROMP(pvChild),
                                     MPFROM2SHORT(CMA_NEXT,CMA_ITEMORDER)));
   } /* endwhile */

   //----------------------------------------------------------------------
   // Finally, check ourselves
   //----------------------------------------------------------------------
   if (pvRsrvd!=NULL) {
      if ((*pfnSearch)(hwndCnr,pvRsrvd,pvUser)) {
         return pvRsrvd;
      } else {
         return NULL;
      } /* endif */
   } else {
      //-------------------------------------------------------------------
      // If we reach here, it means we traversed the entire container
      // without finding a match
      //-------------------------------------------------------------------
      return NULL;
   } /* endif */
}

MRESULT EXPENTRY clientProc(HWND hwndWnd,
                            ULONG ulMsg,
                            MPARAM mpParm1,
                            MPARAM mpParm2)
{
   PCLIENTDATA pcdData;

   pcdData=WinQueryWindowPtr(hwndWnd,0);

   switch (ulMsg) {
   case WM_CREATE:
      {
         CHAR achFont[64];

         //----------------------------------------------------------------
         // Allocate memory for the instance data and initialize it
         //----------------------------------------------------------------
         pcdData=malloc(sizeof(CLIENTDATA));          // @6
         if (pcdData==NULL) {
            WinAlarm(HWND_DESKTOP,WA_ERROR);
            return MRFROMSHORT(TRUE);
         } /* endif */

         WinSetWindowPtr(hwndWnd,0,pcdData);

         pcdData->usSzStruct=sizeof(CLIENTDATA);
         pcdData->habAnchor=WinQueryAnchorBlock(hwndWnd);
         pcdData->hwndFrame=WinQueryWindow(hwndWnd,QW_PARENT);

         //----------------------------------------------------------------
         // Create the container.  Note the window styles used.
         //----------------------------------------------------------------
         pcdData->hwndCnr=WinCreateWindow(hwndWnd,
                                          WC_CONTAINER,
                                          "",
                                          CCS_MINIRECORDCORE | CCS_EXTENDSEL,
                                          0,
                                          0,
                                          0,
                                          0,
                                          hwndWnd,
                                          HWND_TOP,
                                          WND_CNR,
                                          NULL,
                                          NULL);
         if (pcdData->hwndCnr==NULLHANDLE) {
            free(pcdData);
            WinAlarm(HWND_DESKTOP,WA_ERROR);
            return MRFROMSHORT(TRUE);
         } /* endif */

         //----------------------------------------------------------------
         // Change the container's font to 8 point Helvetica
         //----------------------------------------------------------------
         strcpy(achFont,"8.Helv");
         WinSetPresParam(pcdData->hwndCnr,
                         PP_FONTNAMESIZE,
                         strlen(achFont)+1,
                         achFont);

         //----------------------------------------------------------------
         // Load the icons and the menu used
         //----------------------------------------------------------------
         pcdData->hptrYear=WinLoadPointer(HWND_DESKTOP,NULLHANDLE,ICO_YEAR);
         if (pcdData->hptrYear==NULLHANDLE) {
            WinDestroyWindow(pcdData->hwndCnr);
            free(pcdData);
            WinAlarm(HWND_DESKTOP,WA_ERROR);
            return MRFROMSHORT(TRUE);
         } /* endif */

         pcdData->hptrMonth=WinLoadPointer(HWND_DESKTOP,NULLHANDLE,ICO_MONTH);
         if (pcdData->hptrYear==NULLHANDLE) {
            WinDestroyPointer(pcdData->hptrYear);
            WinDestroyWindow(pcdData->hwndCnr);
            free(pcdData);
            WinAlarm(HWND_DESKTOP,WA_ERROR);
            return MRFROMSHORT(TRUE);
         } /* endif */

         pcdData->hwndWndMenu=WinLoadMenu(HWND_OBJECT,NULLHANDLE,RES_CLIENT);
         if (pcdData->hwndWndMenu==NULLHANDLE) {
            WinDestroyPointer(pcdData->hptrMonth);
            WinDestroyPointer(pcdData->hptrYear);
            WinDestroyWindow(pcdData->hwndCnr);
            free(pcdData);
            WinAlarm(HWND_DESKTOP,WA_ERROR);
            return MRFROMSHORT(TRUE);
         } /* endif */

         setCascadeDefault(pcdData->hwndWndMenu,M_VIEW,MI_ICONVIEW);

         initCnr(pcdData);
      }
      break;
   case WM_DESTROY:
      //-------------------------------------------------------------------
      // Cleanup
      //-------------------------------------------------------------------
      WinDestroyWindow(pcdData->hwndWndMenu);
      WinDestroyPointer(pcdData->hptrMonth);
      WinDestroyPointer(pcdData->hptrYear);
      WinDestroyWindow(pcdData->hwndCnr);
      free(pcdData);
      break;
   case WM_SIZE:
      //-------------------------------------------------------------------
      // Size the container to complete cover the client
      //-------------------------------------------------------------------
      WinSetWindowPos(pcdData->hwndCnr,
                      NULLHANDLE,
                      0,
                      0,
                      SHORT1FROMMP(mpParm2),
                      SHORT2FROMMP(mpParm2),
                      SWP_MOVE|SWP_SIZE|SWP_SHOW|SWP_ACTIVATE);
      break;
   case WM_PAINT:
      {
         HPS hpsWnd;

         //----------------------------------------------------------------
         // Since the container covers us completely, the call to GpiErase()
         // is unnecessary
         //----------------------------------------------------------------
         hpsWnd=WinBeginPaint(hwndWnd,NULLHANDLE,NULL);
         GpiErase(hpsWnd);
         WinEndPaint(hpsWnd);
      }
      break;
   case WM_CONTEXTMENU:
      {
         POINTL ptlMouse;

         //----------------------------------------------------------
         // The user requested that the menu be displayed.  Note
         // that there's a bug in the container such that if the
         // user uses the keyboard to do this, we don't get notified,
         // so that is why the code is duplicated here.
         //----------------------------------------------------------
         WinQueryPointerPos(HWND_DESKTOP,&ptlMouse);  // @7

         WinPopupMenu(HWND_DESKTOP,
                      hwndWnd,
                      pcdData->hwndWndMenu,
                      ptlMouse.x,
                      ptlMouse.y,
                      0,
                      PU_HCONSTRAIN | PU_VCONSTRAIN | PU_NONE |
                         PU_KEYBOARD | PU_MOUSEBUTTON1 | PU_MOUSEBUTTON2);
      }
      break;
   case WM_CONTROL:
      switch (SHORT1FROMMP(mpParm1)) {
      case WND_CNR:
         switch (SHORT2FROMMP(mpParm1)) {
         case CN_CONTEXTMENU:
            {
               POINTL ptlMouse;

               //----------------------------------------------------------
               // The user requested that the menu be displayed.  Note
               // that there's a bug in the container such that if the
               // user uses the keyboard to do this, we don't get notified.
               //----------------------------------------------------------
               WinQueryPointerPos(HWND_DESKTOP,&ptlMouse);

               WinPopupMenu(HWND_DESKTOP,
                            hwndWnd,
                            pcdData->hwndWndMenu,
                            ptlMouse.x,
                            ptlMouse.y,
                            0,
                            PU_HCONSTRAIN | PU_VCONSTRAIN | PU_NONE |
                               PU_KEYBOARD | PU_MOUSEBUTTON1 | PU_MOUSEBUTTON2);
            }
            break;
         default:
            return WinDefWindowProc(hwndWnd,ulMsg,mpParm1,mpParm2);
         } /* endswitch */
         break;
      default:
         return WinDefWindowProc(hwndWnd,ulMsg,mpParm1,mpParm2);
      } /* endswitch */
      break;
   case WM_COMMAND:
      //-------------------------------------------------------------------
      // Process the menu.  These are trivial things and are not commented.
      //-------------------------------------------------------------------
      switch (SHORT1FROMMP(mpParm1)) {
      case MI_ICONVIEW:
         {
            CNRINFO ciInfo;

            ciInfo.flWindowAttr=CV_ICON;
            WinSendMsg(pcdData->hwndCnr,
                       CM_SETCNRINFO,
                       MPFROMP(&ciInfo),
                       MPFROMLONG(CMA_FLWINDOWATTR));

            SETMENUATTR(pcdData->hwndWndMenu,MI_ARRANGE,MIA_DISABLED,FALSE);
            SETMENUATTR(pcdData->hwndWndMenu,MI_SELECTALL,MIA_DISABLED,FALSE);
            SETMENUATTR(pcdData->hwndWndMenu,MI_DESELECTALL,MIA_DISABLED,FALSE);
         }
         break;
      case MI_ICONTREE:
         {
            CNRINFO ciInfo;

            ciInfo.flWindowAttr=CV_TREE|CV_ICON;
            WinSendMsg(pcdData->hwndCnr,
                       CM_SETCNRINFO,
                       MPFROMP(&ciInfo),
                       MPFROMLONG(CMA_FLWINDOWATTR));

            SETMENUATTR(pcdData->hwndWndMenu,MI_ARRANGE,MIA_DISABLED,TRUE);
            SETMENUATTR(pcdData->hwndWndMenu,MI_SELECTALL,MIA_DISABLED,TRUE);
            SETMENUATTR(pcdData->hwndWndMenu,MI_DESELECTALL,MIA_DISABLED,TRUE);
         }
         break;
      case MI_ARRANGE:
         WinSendMsg(pcdData->hwndCnr,CM_ARRANGE,0,0);
         break;
      case MI_SELECTALL:
         {
            BOOL bSet;

            bSet=TRUE;                                // @8
            searchCnr(pcdData->hwndCnr,(PFNSRCH)searchSelect,&bSet,NULL);
         }
         break;
      case MI_DESELECTALL:
         {
            BOOL bSet;

            bSet=FALSE;
            searchCnr(pcdData->hwndCnr,(PFNSRCH)searchSelect,&bSet,NULL);
         }
         break;
      case MI_EXIT:
         WinPostMsg(hwndWnd,WM_CLOSE,0,0);
         break;
      default:
         return WinDefWindowProc(hwndWnd,ulMsg,mpParm1,mpParm2);
      } /* endswitch */
      break;
   default:
      return WinDefWindowProc(hwndWnd,ulMsg,mpParm1,mpParm2);
   } /* endswitch */

   return MRFROMSHORT(FALSE);
}

INT main(VOID)
//-------------------------------------------------------------------------
// Standard PM main(), blah, blah, blah...
//-------------------------------------------------------------------------
{
   HAB habAnchor;
   HMQ hmqQueue;
   ULONG ulCreate;
   HWND hwndFrame;
   HWND hwndClient;
   BOOL bLoop;
   QMSG qmMsg;

   habAnchor=WinInitialize(0);
   hmqQueue=WinCreateMsgQueue(habAnchor,0);

   WinRegisterClass(habAnchor,CLS_CLIENT,clientProc,0,sizeof(PVOID));

   ulCreate=FCF_SYSMENU | FCF_TITLEBAR | FCF_MINMAX | FCF_SIZEBORDER |
               FCF_TASKLIST | FCF_SHELLPOSITION;

   hwndFrame=WinCreateStdWindow(HWND_DESKTOP,
                                WS_VISIBLE,
                                &ulCreate,
                                CLS_CLIENT,
                                "Container Sample",
                                0,
                                NULLHANDLE,
                                RES_CLIENT,
                                &hwndClient);
   if (hwndFrame!=NULLHANDLE) {
      bLoop=WinGetMsg(habAnchor,&qmMsg,NULLHANDLE,0,0);
      while (bLoop) {
         WinDispatchMsg(habAnchor,&qmMsg);
         bLoop=WinGetMsg(habAnchor,&qmMsg,NULLHANDLE,0,0);
      } /* endwhile */

      WinDestroyWindow(hwndFrame);
   } /* endif */

   WinDestroyMsgQueue(hmqQueue);
   WinTerminate(habAnchor);
}
