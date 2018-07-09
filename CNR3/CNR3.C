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
#include <time.h>
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

typedef struct _MYCNRREC {                            // @1
   MINIRECORDCORE mrcCore;    // Base structure
   CHAR achText[64];          // Icon text
   ULONG ulNumSold;           // Number of widgets sold
   float fSales;              // Sales in dollars
   CHAR achSales[64];         // ASCII representation of fSales
   PCHAR pchSales;            // Pointer to achSales for details view
} MYCNRREC, *PMYCNRREC;

typedef struct _CLIENTDATA {
   USHORT usSzStruct;         // Size of the structure
   HAB habAnchor;             // Anchor block of the window
   HWND hwndFrame;            // Frame of the client (== parent)
   HWND hwndCnr;              // Container window
   HPOINTER hptrYear;         // Icon for the year records
   HPOINTER hptrMonth;        // Icon for the month records
   HWND hwndWndMenu;          // Menu window
   PMYCNRREC pmcrMenu;        // Record to apply menu to
} CLIENTDATA, *PCLIENTDATA;

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

const PCHAR apchTitles[]={
   "Year",
   "Widgets sold",
   "Total Sales"
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

BOOL searchSource(HWND hwndCnr,PMYCNRREC pmcrRecord,PBOOL pbSource)
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
   if ((pmcrRecord->mrcCore.flRecordAttr & CRA_SELECTED)!=0) {
      WinSendMsg(hwndCnr,
                 CM_SETRECORDEMPHASIS,
                 MPFROMP(pmcrRecord),
                 MPFROM2SHORT(*pbSource,CRA_SOURCE));
   } /* endif */

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
   ULONG ulTotalSold;
   float fTotalSales;
   USHORT usMonth;
   PMYCNRREC pmcrMonth;
   PMYCNRREC pmcrCurrent;
   PFIELDINFO pfiFields;
   PFIELDINFO pfiCurrent;
   FIELDINFOINSERT fiiInsert;

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

   for (usYear=0; usYear<NUM_YEARS; usYear++) {
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
      pmcrYear->ulNumSold=0;
      pmcrYear->fSales=0.0;
      pmcrYear->achSales[0]=0;
      pmcrYear->pchSales=pmcrYear->achSales;

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

      ulTotalSold=0;
      fTotalSales=0.0;

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
         pmcrCurrent->ulNumSold=rand()%900+100;
         pmcrCurrent->fSales=pmcrCurrent->ulNumSold*5.95;
         sprintf(pmcrCurrent->achSales,"$%5.2f",pmcrCurrent->fSales);
         pmcrCurrent->pchSales=pmcrCurrent->achSales;

         ulTotalSold+=pmcrCurrent->ulNumSold;
         fTotalSales+=pmcrCurrent->fSales;

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

      pmcrYear->ulNumSold=ulTotalSold;
      pmcrYear->fSales=fTotalSales;
      sprintf(pmcrYear->achSales,"$%5.2f",pmcrYear->fSales);
   } /* endfor */

                                                      // @2
   pfiFields=(PFIELDINFO)PVOIDFROMMR(WinSendMsg(pcdData->hwndCnr,
                                                CM_ALLOCDETAILFIELDINFO,
                                                MPFROMSHORT(3),
                                                0));
   if (pfiFields==NULL) {
      WinAlarm(HWND_DESKTOP,WA_ERROR);
      return FALSE;
   } /* endif */

   pfiCurrent=pfiFields;

   pfiCurrent->cb=sizeof(FIELDINFO);
   pfiCurrent->flData=CFA_SEPARATOR|CFA_HORZSEPARATOR|CFA_STRING|CFA_FIREADONLY;
   pfiCurrent->flTitle=CFA_FITITLEREADONLY;
   pfiCurrent->pTitleData=apchTitles[0];
   pfiCurrent->offStruct=FIELDOFFSET(MYCNRREC,mrcCore.pszIcon);
   pfiCurrent->pUserData=NULL;
   pfiCurrent=pfiCurrent->pNextFieldInfo;

   pfiCurrent->cb=sizeof(FIELDINFO);
   pfiCurrent->flData=CFA_SEPARATOR|CFA_HORZSEPARATOR|CFA_ULONG;
   pfiCurrent->flTitle=CFA_FITITLEREADONLY;
   pfiCurrent->pTitleData=apchTitles[1];
   pfiCurrent->offStruct=FIELDOFFSET(MYCNRREC,ulNumSold);
   pfiCurrent->pUserData=NULL;
   pfiCurrent=pfiCurrent->pNextFieldInfo;

   pfiCurrent->cb=sizeof(FIELDINFO);
   pfiCurrent->flData=CFA_STRING|CFA_HORZSEPARATOR;
   pfiCurrent->flTitle=CFA_FITITLEREADONLY;
   pfiCurrent->pTitleData=apchTitles[2];
   pfiCurrent->offStruct=FIELDOFFSET(MYCNRREC,pchSales);
   pfiCurrent->pUserData=NULL;

   fiiInsert.cb=sizeof(FIELDINFOINSERT);
   fiiInsert.pFieldInfoOrder=(PFIELDINFO)CMA_FIRST;
   fiiInsert.fInvalidateFieldInfo=FALSE;
   fiiInsert.cFieldInfoInsert=3;

   WinSendMsg(pcdData->hwndCnr,
              CM_INSERTDETAILFIELDINFO,
              MPFROMP(pfiFields),
              MPFROMP(&fiiInsert));

   //----------------------------------------------------------------------
   // Arrange the records and invalidate them so that they get redrawn in
   // their proper positions.
   //----------------------------------------------------------------------
   WinSendMsg(pcdData->hwndCnr,CM_ARRANGE,0,0);

   WinSendMsg(pcdData->hwndCnr,
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
   MENUITEM miItem;
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
   PVOID pvChild;
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

BOOL doPopupMenu(HWND hwndClient,PCLIENTDATA pcdData)
{
   POINTL ptlMouse;
   CNRINFO ciInfo;
   PMYCNRREC pmcrRecord;
   QUERYRECORDRECT qrrDelta;
   RECTL rclDelta;
   QUERYRECFROMRECT qrfrQuery;
   BOOL bSet;
                                                      // @3
   //----------------------------------------------------------------------
   // The user requested that the menu be displayed.  Note that there's a
   // bug in the container such that if the user uses the keyboard to do
   // this, we don't get notified, so that is why this function is called
   // from both WM_CONTROL/CN_CONTEXTMENU and WM_CONTEXTMENU.
   //----------------------------------------------------------------------
   WinQueryPointerPos(HWND_DESKTOP,&ptlMouse);

   qrfrQuery.cb=sizeof(QUERYRECFROMRECT);

   qrfrQuery.rect.xLeft=ptlMouse.x;
   qrfrQuery.rect.xRight=ptlMouse.x+1;
   qrfrQuery.rect.yBottom=ptlMouse.y;
   qrfrQuery.rect.yTop=ptlMouse.y+1;
   WinMapWindowPoints(HWND_DESKTOP,hwndClient,(PPOINTL)&qrfrQuery.rect,2);

   qrfrQuery.fsSearch=CMA_PARTIAL|CMA_ITEMORDER;

   WinSendMsg(pcdData->hwndCnr,
              CM_QUERYCNRINFO,
              MPFROMP(&ciInfo),
              MPFROMSHORT(sizeof(ciInfo)));
   if ((ciInfo.flWindowAttr & CV_DETAIL)!=0) {        // @4
      pmcrRecord=(PMYCNRREC)PVOIDFROMMR(WinSendMsg(pcdData->hwndCnr,
                                                   CM_QUERYRECORD,
                                                   MPFROMP(NULL),
                                                   MPFROM2SHORT(CMA_FIRST,
                                                                CMA_ITEMORDER)));
      qrrDelta.cb=sizeof(QUERYRECORDRECT);
      qrrDelta.pRecord=(PRECORDCORE)pmcrRecord;
      qrrDelta.fRightSplitWindow=FALSE;
      qrrDelta.fsExtent=CMA_ICON;

      WinSendMsg(pcdData->hwndCnr,
                 CM_QUERYRECORDRECT,
                 MPFROMP(&rclDelta),
                 MPFROMP(&qrrDelta));

      qrfrQuery.rect.yBottom-=(rclDelta.yTop-rclDelta.yBottom);
      qrfrQuery.rect.yTop-=(rclDelta.yTop-rclDelta.yBottom);
   } /* endif */

   pcdData->pmcrMenu=(PMYCNRREC)PVOIDFROMMR(WinSendMsg(pcdData->hwndCnr,
                                              CM_QUERYRECORDFROMRECT,
                                              MPFROMP(CMA_FIRST),
                                              MPFROMP(&qrfrQuery)));
   if (pcdData->pmcrMenu!=NULL) {
      if ((pcdData->pmcrMenu->mrcCore.flRecordAttr & CRA_SELECTED)!=0) {
         bSet=TRUE;
         searchCnr(pcdData->hwndCnr,(PFNSRCH)searchSource,&bSet,NULL);
      } else {
         WinSendMsg(pcdData->hwndCnr,
                    CM_SETRECORDEMPHASIS,
                    MPFROMP(pcdData->pmcrMenu),
                    MPFROM2SHORT(TRUE,CRA_SOURCE));
      } /* endif */
   } else {
      WinSendMsg(pcdData->hwndCnr,
                 CM_SETRECORDEMPHASIS,
                 MPFROMP(NULL),
                 MPFROM2SHORT(TRUE,CRA_SOURCE));
   } /* endif */

   WinPopupMenu(HWND_DESKTOP,
                hwndClient,
                pcdData->hwndWndMenu,
                ptlMouse.x,
                ptlMouse.y,
                0,
                PU_HCONSTRAIN | PU_VCONSTRAIN | PU_NONE |
                   PU_KEYBOARD | PU_MOUSEBUTTON1 | PU_MOUSEBUTTON2);
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
         pcdData=malloc(sizeof(CLIENTDATA));
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

         srand(time(NULL));
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
      doPopupMenu(hwndWnd,pcdData);
      break;
   case WM_MENUEND:
      {
         BOOL bSet;

         if (pcdData->pmcrMenu!=NULL) {
            if ((pcdData->pmcrMenu->mrcCore.flRecordAttr & CRA_SELECTED)!=0) {
               bSet=FALSE;
               searchCnr(pcdData->hwndCnr,(PFNSRCH)searchSource,&bSet,NULL);
            } else {
               WinSendMsg(pcdData->hwndCnr,
                          CM_SETRECORDEMPHASIS,
                          MPFROMP(pcdData->pmcrMenu),
                          MPFROM2SHORT(FALSE,CRA_SOURCE));
            } /* endif */
         } else {
            WinSendMsg(pcdData->hwndCnr,
                       CM_SETRECORDEMPHASIS,
                       MPFROMP(NULL),
                       MPFROM2SHORT(FALSE,CRA_SOURCE));
         } /* endif */
      }
      break;
   case WM_CONTROL:
      switch (SHORT1FROMMP(mpParm1)) {
      case WND_CNR:
         switch (SHORT2FROMMP(mpParm1)) {
         case CN_REALLOCPSZ:                          // @5
            {
               PCNREDITDATA pcdEdit;
               PMYCNRREC pmcrRecord;

               pcdEdit=(PCNREDITDATA)PVOIDFROMMP(mpParm2);
               pmcrRecord=(PMYCNRREC)pcdEdit->pRecord;

               //----------------------------------------------------------
               // Check to see if the pointer is pointing to a "static"
               // buffer.  If not, free() the memory pointed to by ppszText.
               //----------------------------------------------------------
               if (((*pcdEdit->ppszText)!=pmcrRecord->achText) &&
                   ((*pcdEdit->ppszText)!=pmcrRecord->achSales)) {
                  free(*pcdEdit->ppszText);
               } /* endif */

               (*pcdEdit->ppszText)=malloc(pcdEdit->cbText+1);
               return MRFROMSHORT((*pcdEdit->ppszText)!=NULL);
            }
            break;
         case CN_CONTEXTMENU:
            doPopupMenu(hwndWnd,pcdData);
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
      case MI_TREEVIEW:
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
      case MI_DETAILVIEW:
         {
            CNRINFO ciInfo;
                                                      // @6
            ciInfo.flWindowAttr=CV_DETAIL|CA_DETAILSVIEWTITLES;
            WinSendMsg(pcdData->hwndCnr,
                       CM_SETCNRINFO,
                       MPFROMP(&ciInfo),
                       MPFROMLONG(CMA_FLWINDOWATTR));

            SETMENUATTR(pcdData->hwndWndMenu,MI_ARRANGE,MIA_DISABLED,TRUE);
            SETMENUATTR(pcdData->hwndWndMenu,MI_SELECTALL,MIA_DISABLED,FALSE);
            SETMENUATTR(pcdData->hwndWndMenu,MI_DESELECTALL,MIA_DISABLED,FALSE);
         }
         break;
      case MI_ARRANGE:
         WinSendMsg(pcdData->hwndCnr,CM_ARRANGE,0,0);
         break;
      case MI_SELECTALL:
         {
            BOOL bSet;

            bSet=TRUE;
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