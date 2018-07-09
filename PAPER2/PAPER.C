#define INCL_GPILCIDS
#define INCL_GPILOGCOLORTABLE
#define INCL_GPIPRIMITIVES
#define INCL_WINENTRYFIELDS
#define INCL_WININPUT
#define INCL_WINMESSAGEMGR
#define INCL_WINRECTANGLES
#define INCL_WINSYS
#define INCL_WINWINDOWMGR
#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include "paper.h"

#define PRVM_SETFOCUS               (WM_USER)

typedef struct _PAPERINSTDATA {
   USHORT usSzStruct;               // Size of the structure
   HWND hwndOwner;                  // Owner window handle
   HAB habAnchor;                   // Anchor block handle
   HWND hwndText;                   // Entryfield window handle
   PCHAR pchTitle;                  // Title text
   SHORT sMaxLines;                 // Maximum lines based on font size
   SHORT sLine;                     // Current line index
   CHAR aachLines[256][256];        // Line array
   FONTMETRICS fmFont;              // Font metrics
   LONG lForeClr;                   // Foreground color
   LONG lBackClr;                   // Background color
} PAPERINSTDATA, *PPAPERINSTDATA;

static MRESULT EXPENTRY pprWndProc(HWND hwndWnd,ULONG ulMsg,MPARAM mpParm1,MPARAM mpParm2)
{
   PPAPERINSTDATA ppidData;

   ppidData=(PPAPERINSTDATA)WinQueryWindowPtr(hwndWnd,1);

   switch (ulMsg) {
   case WM_CREATE:
      {
         PCREATESTRUCT pcsCreate;
         LONG lColor;
         HPS hpsWnd;

         pcsCreate=(PCREATESTRUCT)PVOIDFROMMP(mpParm2);

         //----------------------------------------------------------------
         // Allocate and initialize the instance data
         //----------------------------------------------------------------
         ppidData=calloc(1,sizeof(PAPERINSTDATA));
         if (ppidData==NULL) {
            WinAlarm(HWND_DESKTOP,WA_ERROR);
            return MRFROMSHORT(TRUE);
         } /* endif */

         WinSetWindowPtr(hwndWnd,1,ppidData);

         ppidData->usSzStruct=sizeof(PAPERINSTDATA);
         ppidData->hwndOwner=WinQueryWindow(hwndWnd,QW_OWNER);
         ppidData->habAnchor=WinQueryAnchorBlock(hwndWnd);

         ppidData->hwndText=WinCreateWindow(hwndWnd,
                                            WC_ENTRYFIELD,
                                            "",
                                            0,
                                            0,
                                            0,
                                            0,
                                            0,
                                            hwndWnd,
                                            HWND_TOP,
                                            PPR_CID_ENTRYFIELD,
                                            NULL,
                                            NULL);
         if (ppidData->hwndText==NULLHANDLE) {
            WinAlarm(HWND_DESKTOP,WA_ERROR);
            free(ppidData);
            return MRFROMSHORT(TRUE);
         } /* endif */

         WinSendMsg(ppidData->hwndText,
                    EM_SETTEXTLIMIT,
                    MPFROMSHORT(256),
                    0);

         ppidData->sLine=-1;

         hpsWnd=WinGetPS(hwndWnd);
         GpiQueryFontMetrics(hpsWnd,sizeof(FONTMETRICS),&ppidData->fmFont);
         WinReleasePS(hpsWnd);

         //----------------------------------------------------------------
         // Set up the presentation parameter initial values.
         //----------------------------------------------------------------
         lColor=CLR_WHITE;

         WinSetPresParam(ppidData->hwndText,
                         PP_BACKGROUNDCOLORINDEX,
                         sizeof(lColor),
                         &lColor);

         if ((pcsCreate->pszText!=NULL) && (strlen(pcsCreate->pszText)>0)) {
            ppidData->pchTitle=malloc(strlen(pcsCreate->pszText)+1);
            if (ppidData->pchTitle==NULL) {
               WinAlarm(HWND_DESKTOP,WA_ERROR);
               WinDestroyWindow(ppidData->hwndText);
               free(ppidData);
               return MRFROMSHORT(TRUE);
            } /* endif */

            strcpy(ppidData->pchTitle,pcsCreate->pszText);
         } else {
            ppidData->pchTitle=NULL;
         } /* endif */

         if (pcsCreate->cy>0) {
            ppidData->sMaxLines=(pcsCreate->cy-
                                   ppidData->fmFont.lMaxBaselineExt*2)/
                                   (ppidData->fmFont.lMaxBaselineExt*7/6);
         } else {
            ppidData->sMaxLines=0;
         } /* endif */

         ppidData->lForeClr=CLR_BLACK;
         ppidData->lBackClr=CLR_DARKGRAY;
      }
      break;
   case WM_DESTROY:
      if (ppidData->pchTitle!=NULL) {
         free(ppidData->pchTitle);
      } /* endif */

      WinDestroyWindow(ppidData->hwndText);
      free(ppidData);
      break;
   case WM_SIZE:
      {
         RECTL rclLine;

         //----------------------------------------------------------------
         // Recalculate the maximum number of lines and reposition the
         // entryfield.
         //----------------------------------------------------------------
         if (ppidData->sLine>-1) {
            WinSendMsg(hwndWnd,
                       PPM_QUERYLINERECT,
                       MPFROMP(&rclLine),
                       MPFROMSHORT(ppidData->sLine));

            WinSetWindowPos(ppidData->hwndText,
                            NULLHANDLE,
                            rclLine.xLeft,
                            rclLine.yBottom,
                            rclLine.xRight-rclLine.xLeft,
                            rclLine.yTop-rclLine.yBottom,
                            SWP_MOVE|SWP_SIZE|SWP_SHOW);
         } /* endif */

         ppidData->sMaxLines=(SHORT2FROMMP(mpParm2)-
                                ppidData->fmFont.lMaxBaselineExt*2)/
                                (ppidData->fmFont.lMaxBaselineExt*7/6);
      }
      break;
   case WM_SETFOCUS:
      WinPostMsg(hwndWnd,PRVM_SETFOCUS,mpParm1,mpParm2);
      break;
   case WM_BUTTON1DOWN:
      {
         RECTL rclWnd;

         //----------------------------------------------------------------
         // Before we change the line, update the text array from the
         // current line
         //----------------------------------------------------------------
         if (ppidData->sLine>-1) {
            WinQueryWindowText(ppidData->hwndText,
                               sizeof(ppidData->aachLines[ppidData->sLine]),
                               ppidData->aachLines[ppidData->sLine]);
         } /* endif */

         //----------------------------------------------------------------
         // Query the line clicked on
         //----------------------------------------------------------------
         rclWnd.xLeft=SHORT1FROMMP(mpParm1);
         rclWnd.yBottom=SHORT2FROMMP(mpParm1);

         WinSendMsg(hwndWnd,
                    PPM_CONVERTPOINTS,
                    MPFROMP(&rclWnd.xLeft),
                    MPFROMSHORT(1));

         //----------------------------------------------------------------
         // If the place clicked on is one of the lines, set the new
         // entryfield position to that line
         //----------------------------------------------------------------
         if (rclWnd.yBottom>-1) {
            WinSendMsg(hwndWnd,PPM_SETCURRENTLINE,MPFROMP(rclWnd.yBottom),0);
            WinShowWindow(ppidData->hwndText,TRUE);
            WinSetWindowText(ppidData->hwndText,
                             ppidData->aachLines[ppidData->sLine]);
         } else {
            ppidData->sLine=-1;
            WinShowWindow(ppidData->hwndText,FALSE);
         } /* endif */

         WinSetFocus(HWND_DESKTOP,hwndWnd);
      }
      break;
   case WM_BUTTON1UP:
      WinSendMsg(ppidData->hwndOwner,
                 WM_CONTROL,
                 MPFROM2SHORT(WinQueryWindowUShort(hwndWnd,QWS_ID),
                              PPN_CLICKED),
                 MPFROM2SHORT(ppidData->sLine,1));
      break;
   case WM_BUTTON2UP:
      WinSendMsg(ppidData->hwndOwner,
                 WM_CONTROL,
                 MPFROM2SHORT(WinQueryWindowUShort(hwndWnd,QWS_ID),
                              PPN_CLICKED),
                 MPFROM2SHORT(ppidData->sLine,2));
      break;
   case WM_BUTTON3UP:
      WinSendMsg(ppidData->hwndOwner,
                 WM_CONTROL,
                 MPFROM2SHORT(WinQueryWindowUShort(hwndWnd,QWS_ID),
                              PPN_CLICKED),
                 MPFROM2SHORT(ppidData->sLine,3));
      break;
   case WM_BUTTON1DBLCLK:
      WinSendMsg(ppidData->hwndOwner,
                 WM_CONTROL,
                 MPFROM2SHORT(WinQueryWindowUShort(hwndWnd,QWS_ID),
                              PPN_DBLCLICKED),
                 MPFROM2SHORT(ppidData->sLine,1));
      break;
   case WM_BUTTON2DBLCLK:
      WinSendMsg(ppidData->hwndOwner,
                 WM_CONTROL,
                 MPFROM2SHORT(WinQueryWindowUShort(hwndWnd,QWS_ID),
                              PPN_DBLCLICKED),
                 MPFROM2SHORT(ppidData->sLine,2));
      break;
   case WM_BUTTON3DBLCLK:
      WinSendMsg(ppidData->hwndOwner,
                 WM_CONTROL,
                 MPFROM2SHORT(WinQueryWindowUShort(hwndWnd,QWS_ID),
                              PPN_DBLCLICKED),
                 MPFROM2SHORT(ppidData->sLine,3));
      break;
   case WM_CONTEXTMENU:
      WinSendMsg(ppidData->hwndOwner,
                 WM_CONTROL,
                 MPFROM2SHORT(WinQueryWindowUShort(hwndWnd,QWS_ID),
                              PPN_CONTEXTMENU),
                 MPFROMSHORT(ppidData->sLine));
      break;
   case WM_HELP:
      WinSendMsg(ppidData->hwndOwner,
                 WM_CONTROL,
                 MPFROM2SHORT(WinQueryWindowUShort(hwndWnd,QWS_ID),PPN_HELP),
                 0);
      break;
   case WM_CHAR:
      if ((CHARMSG(&ulMsg)->fs & (KC_VIRTUALKEY | KC_KEYUP))==
          KC_VIRTUALKEY) {
         switch (CHARMSG(&ulMsg)->vkey) {
         case VK_UP:
            {
               RECTL rclLine;

               //----------------------------------------------------------
               // Remember, we can only go up if there is another line
               // above us
               //----------------------------------------------------------
               if (ppidData->sLine>0) {
                  WinQueryWindowText(ppidData->hwndText,
                                     sizeof(ppidData->aachLines[ppidData->sLine]),
                                     ppidData->aachLines[ppidData->sLine]);

                  ppidData->sLine--;

                  WinSendMsg(hwndWnd,
                             PPM_QUERYLINERECT,
                             MPFROMP(&rclLine),
                             MPFROMSHORT(ppidData->sLine));
                  WinSetWindowPos(ppidData->hwndText,
                                  NULLHANDLE,
                                  rclLine.xLeft,
                                  rclLine.yBottom,
                                  0,
                                  0,
                                  SWP_MOVE);
                  WinSetWindowText(ppidData->hwndText,
                                   ppidData->aachLines[ppidData->sLine]);

                  //-------------------------------------------------------
                  // We only invalidate the line we left because the
                  // entryfield paints itself
                  //-------------------------------------------------------
                  WinSendMsg(hwndWnd,
                             PPM_QUERYLINERECT,
                             MPFROMP(&rclLine),
                             MPFROMSHORT(ppidData->sLine+1));
                  WinInvalidateRect(hwndWnd,&rclLine,TRUE);
                  WinUpdateWindow(hwndWnd);

                  WinSendMsg(ppidData->hwndOwner,
                             WM_CONTROL,
                             MPFROM2SHORT(WinQueryWindowUShort(hwndWnd,QWS_ID),
                                          PPN_UP),
                             MPFROMSHORT(ppidData->sLine));
               } else
               if (ppidData->sLine==0) {
                  WinSendMsg(ppidData->hwndOwner,
                             WM_CONTROL,
                             MPFROM2SHORT(WinQueryWindowUShort(hwndWnd,QWS_ID),
                                          PPN_BEGINPAGE),
                             MPFROMSHORT(ppidData->sLine));
               } /* endif */
            }
            break;
         //----------------------------------------------------------------
         // We treat newline and enter the same as down arrow
         //----------------------------------------------------------------
         case VK_DOWN:
         case VK_NEWLINE:
         case VK_ENTER:
            {
               RECTL rclLine;

               //----------------------------------------------------------
               // Remember, we can only go down if there is another line
               // below us
               //----------------------------------------------------------
               if ((ppidData->sLine>-1) &&
                   (ppidData->sLine<ppidData->sMaxLines-1)) {
                  WinQueryWindowText(ppidData->hwndText,
                                     sizeof(ppidData->aachLines[ppidData->sLine]),
                                     ppidData->aachLines[ppidData->sLine]);

                  ppidData->sLine++;

                  WinSendMsg(hwndWnd,
                             PPM_QUERYLINERECT,
                             MPFROMP(&rclLine),
                             MPFROMSHORT(ppidData->sLine));
                  WinSetWindowPos(ppidData->hwndText,
                                  NULLHANDLE,
                                  rclLine.xLeft,
                                  rclLine.yBottom,
                                  0,
                                  0,
                                  SWP_MOVE);
                  WinSetWindowText(ppidData->hwndText,
                                   ppidData->aachLines[ppidData->sLine]);

                  //-------------------------------------------------------
                  // We only invalidate the line we left because the
                  // entryfield paints itself
                  //-------------------------------------------------------
                  WinSendMsg(hwndWnd,
                             PPM_QUERYLINERECT,
                             MPFROMP(&rclLine),
                             MPFROMSHORT(ppidData->sLine-1));
                  WinInvalidateRect(hwndWnd,&rclLine,TRUE);
                  WinUpdateWindow(hwndWnd);

                  WinSendMsg(ppidData->hwndOwner,
                             WM_CONTROL,
                             MPFROM2SHORT(WinQueryWindowUShort(hwndWnd,QWS_ID),
                                          PPN_DOWN),
                             MPFROMSHORT(ppidData->sLine));
               } else
               if (ppidData->sLine==ppidData->sMaxLines-1) {
                  WinSendMsg(ppidData->hwndOwner,
                             WM_CONTROL,
                             MPFROM2SHORT(WinQueryWindowUShort(hwndWnd,QWS_ID),
                                          PPN_ENDPAGE),
                             MPFROMSHORT(ppidData->sLine));
               } /* endif */
            }
            break;
         default:
            return WinDefWindowProc(hwndWnd,ulMsg,mpParm1,mpParm2);
         } /* endswitch */
      } else {
         return WinDefWindowProc(hwndWnd,ulMsg,mpParm1,mpParm2);
      } /* endif */
      break;
   case WM_PAINT:
      {
         ULONG ulStyle;
         HPS hpsPaint;
         RECTL rclPaint;
         RECTL rclWnd;
         POINTL ptlPoint;
         SHORT sIndex;
         RECTL rclLine;
         USHORT usMaxHoles;
         RECTL rclHole;

         ulStyle=WinQueryWindowULong(hwndWnd,QWL_STYLE);

         hpsPaint=WinBeginPaint(hwndWnd,NULLHANDLE,&rclPaint);
         WinFillRect(hpsPaint,&rclPaint,CLR_WHITE);

         WinQueryWindowRect(hwndWnd,&rclWnd);

         //----------------------------------------------------------------
         // Paint the border first
         //
         //               +------------+
         //              +|-----------+|
         //              ||           ||
         //              ||           ||
         //              ||           ||
         // White -----> ||           || <----- Dark gray
         //              ||           ||
         //              |+------------+
         //              +------------+
         //----------------------------------------------------------------
         if ((ulStyle & PPS_BORDER)!=0) {
            GpiSetColor(hpsPaint,CLR_WHITE);

            ptlPoint.x=rclWnd.xLeft+1;
            ptlPoint.y=rclWnd.yBottom;
            GpiMove(hpsPaint,&ptlPoint);

            ptlPoint.x=rclWnd.xRight;
            ptlPoint.y=rclWnd.yTop-1;
            GpiBox(hpsPaint,DRO_OUTLINE,&ptlPoint,0,0);

            GpiSetColor(hpsPaint,CLR_DARKGRAY);

            ptlPoint.x=rclWnd.xLeft;
            ptlPoint.y=rclWnd.yBottom+1;
            GpiMove(hpsPaint,&ptlPoint);

            ptlPoint.x=rclWnd.xRight-1;
            ptlPoint.y=rclWnd.yTop;
            GpiBox(hpsPaint,DRO_OUTLINE,&ptlPoint,0,0);

            WinInflateRect(ppidData->habAnchor,&rclWnd,-2,-2);
         } /* endif */

         //----------------------------------------------------------------
         // Paint the vertical line.  Check the window style to see what
         // side it is on.
         //----------------------------------------------------------------
         if ((ulStyle & PPS_HOLESNONE)!=0) {
            ptlPoint.x=rclWnd.xLeft+ppidData->fmFont.lAveCharWidth*5;
         } else
         if ((ulStyle & PPS_HOLESRIGHT)!=0) {
            ptlPoint.x=rclWnd.xRight-ppidData->fmFont.lAveCharWidth*5;
         } else {
            ptlPoint.x=rclWnd.xLeft+ppidData->fmFont.lAveCharWidth*5;
         } /* endif */

         ptlPoint.y=rclWnd.yBottom;
         GpiMove(hpsPaint,&ptlPoint);

         ptlPoint.y=rclWnd.yTop;
         GpiSetColor(hpsPaint,CLR_PINK);
         GpiLine(hpsPaint,&ptlPoint);

         //----------------------------------------------------------------
         // Paint the horizontal lines.  Our strategy is to query each
         // line rectangle, and draw a line along the top edge of this
         // rectangle.  This means the bottom edge of the bottom line
         // will not get painted, so explicitly handle this case.
         //----------------------------------------------------------------
         GpiSetColor(hpsPaint,CLR_DARKCYAN);

         for (sIndex=0; sIndex<ppidData->sMaxLines; sIndex++) {
            WinSendMsg(hwndWnd,
                       PPM_QUERYLINERECT,
                       MPFROMP(&rclLine),
                       MPFROMSHORT(sIndex));
            ptlPoint.x=rclWnd.xLeft;
            ptlPoint.y=rclLine.yTop;
            GpiMove(hpsPaint,&ptlPoint);
            ptlPoint.x=rclWnd.xRight;
            GpiLine(hpsPaint,&ptlPoint);
         } /* endfor */

         ptlPoint.x=rclWnd.xLeft;
         ptlPoint.y=rclLine.yBottom-1;
         GpiMove(hpsPaint,&ptlPoint);
         ptlPoint.x=rclWnd.xRight;
         GpiLine(hpsPaint,&ptlPoint);

         //----------------------------------------------------------------
         // Note that if PPS_HOLESNONE was specified, 0 is returned
         //----------------------------------------------------------------
         usMaxHoles=SHORT1FROMMR(WinSendMsg(hwndWnd,PPM_QUERYNUMHOLES,0,0));

         for (sIndex=0; sIndex<usMaxHoles; sIndex++) {
            WinSendMsg(hwndWnd,
                       PPM_QUERYHOLERECT,
                       MPFROMP(&rclHole),
                       MPFROMSHORT(sIndex));

            ptlPoint.x=rclHole.xLeft+(rclHole.xRight-rclHole.xLeft)/2;
            ptlPoint.y=rclHole.yBottom+(rclHole.yTop-rclHole.yBottom)/2;
            GpiMove(hpsPaint,&ptlPoint);
            GpiSetColor(hpsPaint,CLR_PALEGRAY);
            GpiFullArc(hpsPaint,
                       DRO_FILL,
                       MAKEFIXED(ppidData->fmFont.lAveCharWidth,0));
            GpiSetColor(hpsPaint,CLR_DARKGRAY);
            GpiFullArc(hpsPaint,
                       DRO_OUTLINE,
                       MAKEFIXED(ppidData->fmFont.lAveCharWidth,0));
         } /* endfor */

         if (ppidData->pchTitle!=NULL) {
            WinSendMsg(hwndWnd,
                       PPM_QUERYLINERECT,
                       MPFROMP(&rclLine),
                       MPFROMSHORT(0));
            rclLine.yBottom=rclLine.yTop+1;
            rclLine.yTop=rclLine.yBottom+ppidData->fmFont.lMaxBaselineExt;

            WinDrawText(hpsPaint,
                        -1,
                        ppidData->pchTitle,
                        &rclLine,
                        ppidData->lForeClr,
                        0,
                        DT_CENTER);
         } /* endif */

         WinSendMsg(hwndWnd,
                    PPM_CONVERTPOINTS,
                    MPFROMP(&rclPaint),
                    MPFROMSHORT(2));

         if (rclPaint.yTop<0) {
            rclPaint.yTop=0;
         } /* endif */

         if ((rclPaint.yBottom>ppidData->sMaxLines-1) ||
             (rclPaint.yBottom<0)) {
            rclPaint.yBottom=ppidData->sMaxLines-1;
         } /* endif */

         while (rclPaint.yTop<=rclPaint.yBottom) {
            WinSendMsg(hwndWnd,
                       PPM_QUERYLINERECT,
                       MPFROMP(&rclLine),
                       MPFROMSHORT(rclPaint.yTop));

            if (strlen(ppidData->aachLines[rclPaint.yTop])>0) {
               WinDrawText(hpsPaint,
                           -1,
                           ppidData->aachLines[rclPaint.yTop],
                           &rclLine,
                           ppidData->lBackClr,
                           0,
                           DT_LEFT|DT_BOTTOM);
            } /* endif */

            rclPaint.yTop++;
         } /* endwhile */

         WinEndPaint(hpsPaint);
      }
      break;
   case WM_QUERYWINDOWPARAMS:
      {
         PWNDPARAMS pwpParms;

         pwpParms=(PWNDPARAMS)PVOIDFROMMP(mpParm1);

         //----------------------------------------------------------------
         // Check for the window text
         //----------------------------------------------------------------
         if ((pwpParms->fsStatus & WPM_TEXT)!=0) {
            pwpParms->pszText[0]=0;
            strncat(pwpParms->pszText,ppidData->pchTitle,pwpParms->cchText);
            WinDefWindowProc(hwndWnd,ulMsg,mpParm1,mpParm2);
            return MRFROMSHORT(TRUE);
         } /* endif */

         return WinDefWindowProc(hwndWnd,ulMsg,mpParm1,mpParm2);
      }
   case WM_SETWINDOWPARAMS:
      {
         BOOL bProcessed;
         PWNDPARAMS pwpParms;

         bProcessed=FALSE;
         pwpParms=(PWNDPARAMS)PVOIDFROMMP(mpParm1);

         //----------------------------------------------------------------
         // Check for the window text
         //----------------------------------------------------------------
         if ((pwpParms->fsStatus & WPM_TEXT)!=0) {
            if (ppidData->pchTitle!=NULL) {
               free(ppidData->pchTitle);
               ppidData->pchTitle=NULL;
            } /* endif */

            if ((pwpParms->pszText!=NULL) && (strlen(pwpParms->pszText)>0)) {
               ppidData->pchTitle=malloc(strlen(pwpParms->pszText)+1);
               strcpy(ppidData->pchTitle,pwpParms->pszText);
            } /* endif */

            bProcessed=TRUE;
         } /* endif */

         if (bProcessed) {
            WinInvalidateRect(hwndWnd,NULL,TRUE);
            WinUpdateWindow(hwndWnd);
            WinDefWindowProc(hwndWnd,ulMsg,mpParm1,mpParm2);
            return MRFROMSHORT(TRUE);
         } else {
            return WinDefWindowProc(hwndWnd,ulMsg,mpParm1,mpParm2);
         } /* endif */
      }
   case WM_PRESPARAMCHANGED:
      switch (LONGFROMMP(mpParm1)) {
      case PP_FOREGROUNDCOLOR:
      case PP_FOREGROUNDCOLORINDEX:
         {
            ULONG ulId;
            LONG lColor;
            HPS hpsWnd;
            SHORT sIndex;
            RECTL rclLine;

            //-------------------------------------------------------------
            // The pres param has already been changed, so we can query
            // it.
            //-------------------------------------------------------------
            WinQueryPresParam(hwndWnd,
                              PP_FOREGROUNDCOLORINDEX,
                              LONGFROMMP(mpParm1),
                              &ulId,
                              sizeof(lColor),
                              &lColor,
                              QPF_NOINHERIT);

            if (ulId==PP_FOREGROUNDCOLOR) {
               hpsWnd=WinGetPS(hwndWnd);
               lColor=GpiQueryColorIndex(hpsWnd,0,lColor);
               WinReleasePS(hpsWnd);
            } /* endif */

            ppidData->lForeClr=lColor;

            for (sIndex=0; sIndex<ppidData->sMaxLines; sIndex++) {
               if (ppidData->aachLines[sIndex][0]!=0) {
                  WinSendMsg(hwndWnd,
                             PPM_QUERYLINERECT,
                             MPFROMP(&rclLine),
                             MPFROMSHORT(sIndex));
                  WinInvalidateRect(hwndWnd,NULL,TRUE);
               } /* endif */
            } /* endfor */

            WinUpdateWindow(hwndWnd);
         }
         break;
      case PP_BACKGROUNDCOLOR:
      case PP_BACKGROUNDCOLORINDEX:
         {
            ULONG ulId;
            LONG lColor;
            HPS hpsWnd;
            SHORT sIndex;
            RECTL rclLine;

            //-------------------------------------------------------------
            // The pres param has already been changed, so we can query
            // it.
            //-------------------------------------------------------------
            WinQueryPresParam(hwndWnd,
                              PP_BACKGROUNDCOLORINDEX,
                              LONGFROMMP(mpParm1),
                              &ulId,
                              sizeof(lColor),
                              &lColor,
                              QPF_NOINHERIT);

            if (ulId==PP_BACKGROUNDCOLOR) {
               hpsWnd=WinGetPS(hwndWnd);
               lColor=GpiQueryColorIndex(hpsWnd,0,lColor);
               WinReleasePS(hpsWnd);
            } /* endif */

            ppidData->lBackClr=lColor;

            for (sIndex=0; sIndex<ppidData->sMaxLines; sIndex++) {
               if (ppidData->aachLines[sIndex][0]!=0) {
                  WinSendMsg(hwndWnd,
                             PPM_QUERYLINERECT,
                             MPFROMP(&rclLine),
                             MPFROMSHORT(sIndex));
                  WinInvalidateRect(hwndWnd,NULL,TRUE);
               } /* endif */
            } /* endfor */

            WinUpdateWindow(hwndWnd);
         }
         break;
      case PP_FONTNAMESIZE:
      case PP_FONTHANDLE:
         {
            HPS hpsWnd;
            RECTL rclWnd;

            //-------------------------------------------------------------
            // The pres param has already been changed, so we can get a
            // micro-cached HPS and query its FONTMETRICS.
            //-------------------------------------------------------------
            hpsWnd=WinGetPS(hwndWnd);
            GpiQueryFontMetrics(hpsWnd,sizeof(FONTMETRICS),&ppidData->fmFont);
            WinReleasePS(hpsWnd);

            WinQueryWindowRect(hwndWnd,&rclWnd);
            WinSendMsg(hwndWnd,
                       WM_SIZE,
                       0,
                       MPFROM2SHORT((SHORT)rclWnd.xRight,
                                    (SHORT)rclWnd.yTop));
            WinInvalidateRect(hwndWnd,NULL,TRUE);
            WinUpdateWindow(hwndWnd);
         }
         break;
      default:
         return WinDefWindowProc(hwndWnd,ulMsg,mpParm1,mpParm2);
      } /* endswitch */
      break;
   case PPM_QUERYMAXLINES:
      return MRFROMSHORT(ppidData->sMaxLines);
   case PPM_QUERYCURRENTLINE:
      return MRFROMSHORT(ppidData->sLine);
   case PPM_QUERYLINERECT:
      {
         PRECTL prclLine;
         SHORT sLine;
         ULONG ulStyle;
         ULONG ulCyLine;

         prclLine=(PRECTL)PVOIDFROMMP(mpParm1);
         sLine=SHORT1FROMMP(mpParm2);

         ulStyle=WinQueryWindowULong(hwndWnd,QWL_STYLE);

         if ((sLine<0) || (sLine>ppidData->sMaxLines-1)) {
            return MRFROMSHORT(FALSE);
         } /* endif */

         WinQueryWindowRect(hwndWnd,prclLine);
         WinInflateRect(ppidData->habAnchor,prclLine,-2,-2);

         prclLine->yTop-=ppidData->fmFont.lMaxBaselineExt*2;

         ulCyLine=ppidData->fmFont.lMaxBaselineExt*7/6;

         if ((ulStyle & PPS_HOLESNONE)!=0) {
            prclLine->xLeft+=ppidData->fmFont.lAveCharWidth*5+2;
            prclLine->xRight-=ppidData->fmFont.lAveCharWidth;
         } else
         if ((ulStyle & PPS_HOLESRIGHT)!=0) {
            prclLine->xLeft+=ppidData->fmFont.lAveCharWidth;
            prclLine->xRight-=ppidData->fmFont.lAveCharWidth*5+2;
         } else {
            prclLine->xLeft+=ppidData->fmFont.lAveCharWidth*5+2;
            prclLine->xRight-=ppidData->fmFont.lAveCharWidth;
         } /* endif */

         prclLine->yTop-=sLine*ulCyLine;
         prclLine->yBottom=prclLine->yTop-ulCyLine+1;

         //----------------------------------------------------------------------
         // Adjust the top so that the rectangle doesn't interfere with the
         // dividing lines that are drawn
         //----------------------------------------------------------------------
         prclLine->yTop--;

         return MRFROMSHORT(TRUE);
      }
   case PPM_QUERYNUMHOLES:
      {
         ULONG ulStyle;

         ulStyle=WinQueryWindowULong(hwndWnd,QWL_STYLE);
         return MRFROMSHORT(((ulStyle & PPS_HOLESNONE)!=0)?0:3);
      }
   case PPM_QUERYHOLERECT:
      {
         USHORT usHole;
         PRECTL prclWnd;
         RECTL rclWnd;
         ULONG ulStyle;

         usHole=SHORT1FROMMP(mpParm2);
         if (usHole>2) {
            return MRFROMSHORT(FALSE);
         } /* endif */

         prclWnd=(PRECTL)PVOIDFROMMP(mpParm1);

         WinQueryWindowRect(hwndWnd,&rclWnd);
         WinInflateRect(ppidData->habAnchor,&rclWnd,-2,-2);

         ulStyle=WinQueryWindowULong(hwndWnd,QWL_STYLE);

         //----------------------------------------------------------------
         // First calculate the center of the hole and then expand the
         // recentangle from there
         //----------------------------------------------------------------
         if ((ulStyle & PPS_HOLESNONE)!=0) {
            return MRFROMSHORT(FALSE);
         } else
         if ((ulStyle & PPS_HOLESRIGHT)!=0) {
            prclWnd->xLeft=rclWnd.xRight-ppidData->fmFont.lAveCharWidth*3;
         } else {
            prclWnd->xLeft=rclWnd.xLeft+ppidData->fmFont.lAveCharWidth*3;
         } /* endif */

         prclWnd->yBottom=(rclWnd.yTop-rclWnd.yBottom)/8+usHole*
                              (rclWnd.yTop-rclWnd.yBottom)*3/8;

         prclWnd->xLeft-=ppidData->fmFont.lAveCharWidth;
         prclWnd->yBottom-=ppidData->fmFont.lAveCharWidth;
         prclWnd->xRight=prclWnd->xLeft+ppidData->fmFont.lAveCharWidth*2;
         prclWnd->yTop=prclWnd->yBottom+ppidData->fmFont.lAveCharWidth*2;

         return MRFROMSHORT(TRUE);
      }
   case PPM_QUERYLINETEXT:
      {
         SHORT sLine;
         USHORT usSzBuf;
         PCHAR pchBuf;

         sLine=SHORT1FROMMP(mpParm1);
         usSzBuf=SHORT2FROMMP(mpParm1);
         pchBuf=(PCHAR)PVOIDFROMMP(mpParm2);

         if (sLine<ppidData->sMaxLines) {
            pchBuf[0]=0;
            strncat(pchBuf,ppidData->aachLines[sLine],usSzBuf);
            return MRFROMSHORT(TRUE);
         } else {
            return MRFROMSHORT(FALSE);
         } /* endif */
      }
   case PPM_QUERYLINETEXTLEN:
      {
         SHORT sLine;

         sLine=SHORT1FROMMP(mpParm1);

         if (sLine<ppidData->sMaxLines) {
            return MRFROMSHORT(strlen(ppidData->aachLines[sLine]));
         } else {
            return MRFROMSHORT(-1);
         } /* endif */
      }
   case PPM_QUERYPAPERTEXT:
      {
         PCHAR pchBuf;
         SHORT sSzBuf;
         SHORT sEndLine;
         SHORT sIndex;

         pchBuf=(PCHAR)PVOIDFROMMP(mpParm1);
         *pchBuf=0;

         sSzBuf=SHORT1FROMMP(mpParm2);

         sEndLine=ppidData->sMaxLines-1;
         while ((sEndLine>=0) && (strlen(ppidData->aachLines[sEndLine])==0)) {
            sEndLine--;
         } /* endwhile */

         if (sEndLine<0) {
            return MRFROMSHORT(FALSE);
         } /* endif */

         sEndLine++;

         for (sIndex=0; (sIndex<sEndLine) && (sSzBuf>0); sIndex++) {
            strncat(pchBuf,ppidData->aachLines[sIndex],sSzBuf-1);
            sSzBuf-=strlen(ppidData->aachLines[sIndex]);

            if (sSzBuf>1) {
               strncat(pchBuf,"\n",sSzBuf-1);
               sSzBuf-=strlen("\n");
            } /* endif */
         } /* endfor */

         return MRFROMSHORT(TRUE);
      }
   case PPM_QUERYPAPERTEXTLEN:
      {
         SHORT sTextLen;
         SHORT sIndex;

         sTextLen=0;

         for (sIndex=0; sIndex<ppidData->sMaxLines; sIndex++) {
            sTextLen+=strlen(ppidData->aachLines[sIndex])+1;
         } /* endfor */

         return MRFROMSHORT(sTextLen);
      }
   case PPM_SETCURRENTLINE:
      {
         SHORT sLine;
         RECTL rclLine;

         sLine=SHORT1FROMMP(mpParm1);
         if ((sLine<0) && (sLine>ppidData->sMaxLines-1)) {
            return MRFROMSHORT(FALSE);
         } /* endif */

         ppidData->sLine=sLine;

         WinSendMsg(hwndWnd,
                    PPM_QUERYLINERECT,
                    MPFROMP(&rclLine),
                    MPFROMSHORT(ppidData->sLine));

         WinSetWindowPos(ppidData->hwndText,
                         NULLHANDLE,
                         rclLine.xLeft,
                         rclLine.yBottom,
                         rclLine.xRight-rclLine.xLeft,
                         rclLine.yTop-rclLine.yBottom,
                         SWP_MOVE|SWP_SIZE);
      }
      break;
   case PPM_SETLINETEXT:
      {
         SHORT sLine;
         RECTL rclText;

         sLine=SHORT1FROMMP(mpParm1);

         //----------------------------------------------------------------
         // Update the line array if needed and repaint
         //----------------------------------------------------------------
         if (sLine<ppidData->sMaxLines) {
            strcpy(ppidData->aachLines[sLine],PVOIDFROMMP(mpParm2));

            WinSendMsg(hwndWnd,
                       PPM_QUERYLINERECT,
                       MPFROMP(&rclText),
                       MPFROMSHORT(sLine));
            WinInvalidateRect(hwndWnd,&rclText,TRUE);
            WinUpdateWindow(hwndWnd);

            return MRFROMSHORT(TRUE);
         } else {
            return MRFROMSHORT(FALSE);
         } /* endif */
      }
   case PPM_SETPAPERTEXT:
      {
         PCHAR pchText;
         SHORT sLine;
         PCHAR pchBegin;
         PCHAR pchEnd;

         pchText=(PCHAR)PVOIDFROMMP(mpParm1);

         memset(ppidData->aachLines,0,sizeof(ppidData->aachLines));

         if (pchText==NULL) {
            return MRFROMSHORT(TRUE);
         } /* endif */

         sLine=0;
         pchBegin=pchText;
         pchEnd=strchr(pchBegin,'\n');
         if (pchEnd==NULL) {
            pchEnd=pchBegin+strlen(pchBegin);
         } /* endif */

         while (sLine<ppidData->sMaxLines) {
            strncat(ppidData->aachLines[sLine],pchBegin,pchEnd-pchBegin);

            if (*pchEnd==0) {
               sLine=ppidData->sMaxLines;
            } else {
               sLine++;
               pchBegin=pchEnd+1;
               pchEnd=strchr(pchBegin,'\n');
               if (pchEnd==NULL) {
                  pchEnd=pchBegin+strlen(pchBegin);
               } /* endif */
            } /* endif */
         } /* endwhile */

         if (ppidData->sLine>-1) {
            WinSetWindowText(ppidData->hwndText,
                             ppidData->aachLines[ppidData->sLine]);
         } /* endif */

         WinInvalidateRect(hwndWnd,NULL,FALSE);
         WinUpdateWindow(hwndWnd);
         return MRFROMSHORT(TRUE);
      }
   case PPM_CONVERTPOINTS:
      {
         PPOINTL pptlPoint;
         USHORT usNumPoints;
         RECTL rclWnd;
         ULONG ulCyLine;
         USHORT usIndex;

         //----------------------------------------------------------------
         // This message converts the y-coordinates to the corresponding
         // line numbers.  The x-coordinates are ignored.
         //----------------------------------------------------------------

         pptlPoint=(PPOINTL)PVOIDFROMMP(mpParm1);
         usNumPoints=SHORT1FROMMP(mpParm2);

         WinQueryWindowRect(hwndWnd,&rclWnd);
         WinInflateRect(ppidData->habAnchor,&rclWnd,-2,-2);

         //----------------------------------------------------------------
         // Adjust for the top margin
         //----------------------------------------------------------------
         rclWnd.yTop-=ppidData->fmFont.lMaxBaselineExt*2;

         ulCyLine=ppidData->fmFont.lMaxBaselineExt*7/6;

         for (usIndex=0; usIndex<usNumPoints; usIndex++) {
            if ((pptlPoint->y<rclWnd.yTop) &&
                (pptlPoint->y>=rclWnd.yBottom)) {
               pptlPoint->y=(rclWnd.yTop-pptlPoint->y)/ulCyLine;
            } else {
               pptlPoint->y=-1;
            } /* endif */

            pptlPoint++;
         } /* endfor */
      }
      break;
   case PRVM_SETFOCUS:
      if (SHORT1FROMMP(mpParm2)) {
         if (ppidData->sLine>-1) {
            WinShowWindow(ppidData->hwndText,TRUE);
            WinFocusChange(HWND_DESKTOP,
                           ppidData->hwndText,
                           FC_NOLOSEACTIVE|FC_NOLOSEFOCUS|FC_NOLOSESELECTION);
         } /* endif */

         WinSendMsg(ppidData->hwndOwner,
                    WM_CONTROL,
                    MPFROM2SHORT(WinQueryWindowUShort(hwndWnd,QWS_ID),
                                 PPN_SETFOCUS),
                    0);
      } else {
         //----------------------------------------------------------------
         // If we're losing the focus, update the text array, but leave the
         // entryfield text alone.
         //----------------------------------------------------------------
         if (ppidData->sLine>-1) {
            WinQueryWindowText(ppidData->hwndText,
                               sizeof(ppidData->aachLines[ppidData->sLine]),
                               ppidData->aachLines[ppidData->sLine]);
            WinShowWindow(ppidData->hwndText,FALSE);
         } /* endif */

         WinSendMsg(ppidData->hwndOwner,
                    WM_CONTROL,
                    MPFROM2SHORT(WinQueryWindowUShort(hwndWnd,QWS_ID),
                                 PPN_KILLFOCUS),
                    0);
      } /* endif */
      break;
   default:
      return WinDefWindowProc(hwndWnd,ulMsg,mpParm1,mpParm2);
   } /* endswitch */

   return MRFROMSHORT(FALSE);
}

BOOL PprInitialize(HAB habAnchor)
{
   WinRegisterClass(habAnchor,
                    WC_PAPERPAGE,
                    pprWndProc,
                    CS_SIZEREDRAW|CS_CLIPSIBLINGS|CS_PARENTCLIP,
                    sizeof(PVOID)*2);
   return TRUE;
}

BOOL PprTerminate(HAB habAnchor)
{
   return TRUE;
}
