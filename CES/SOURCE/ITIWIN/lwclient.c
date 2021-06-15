/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1992 by Info Tech, Inc.  All Rights Reserved.         |
|                                                                           |
|       This program module is part of DS/Shell (PC), Info Tech's           |
|       database interfaces for OS/2, a proprietary product of              |
|       Info Tech, Inc., no part of which may be reproduced or              |
|       transmitted in any form or by any means, electronic,                |
|       mechanical, or otherwise, including photocopying and recording      |
|       or in connection with any information storage or retrieval          |
|       system, without permission in writing from Info Tech, Inc.          |
|                                                                           |
+--------------------------------------------------------------------------*/


/************************************************************************
 *                                                                      *
 * lwclient.c                                                           *
 *                                                                      *
 * Copyright (C) 1990-1992 Info Tech, Inc.                              *
 *                                                                      *
 *  This file contains the code to paint the client and label windows   *
 *    and contains the client and label window procs                    *
 *                                                                      *
 * This file is part of the Window system of DS/SHELL. This is a        *
 * proprietary product of Info Tech. and no part of which may be        *
 * reproduced or transmitted in any form or by any means, including     *
 * photocopying and recording or in connection with any information     *
 * storage or retrieval system, without permission in writing           *
 * from Info Tech, Inc.                                                 *
 *                                                                      *
 *                                                                      *
 ************************************************************************/

#include <stdlib.h>   
#include <stdio.h>   
#define  INCL_WIN
#include "..\include\iti.h"
#include "..\include\winid.h"
#include "..\include\itidbase.h"
#include "..\include\itiwin.h"
#include "..\include\itibase.h"
#include "..\include\itierror.h"
#include "..\include\itiglob.h"
#include "initdll.h"
#include "winmain.h"
#include "lwmain.h"
#include "lwclient.h"
#include "winutil.h"
#include "lwutil.h"
#include "lwsplit.h"
#include "lwselect.h"
#include "lwkey.h"
#include "lwmouse.h"
#include "lwkey.h"


char pszDefStr[] = "Element X,Y";
#define ITILW_RCLBUF  10
#define ITILW_NOPAINT -1000



static USHORT okCliErr (PSZ psz, USHORT uId)
   {
   char  szTmp [STRLEN];

   sprintf (szTmp, "Client.c : %s%d", psz, uId);
   ItiErrWriteDebugMessage (szTmp);
   WinMessageBox (pglobals->hwndDesktop, pglobals->hwndAppFrame, szTmp,
                  "List Client Error", 0, MB_OK | MB_APPLMODAL);
   return TRUE;
   }



int EXPORT GetTextElement  (HWND   hwndChild,
                            USHORT uRow,
                            USHORT uCol,
                            PSZ    *ppszText)
   {
   *ppszText = pszDefStr;
   (*ppszText)[ 8] = (unsigned char) 'A' + (unsigned char) uRow;
   (*ppszText)[10] = (unsigned char) 'A' + (unsigned char) uCol;
   return 0;
   }
              
              



/*
 * return TRUE=will paint
 * FALSE= will not paint
 */
static int XShift (RECTL *rclUpdate, USHORT uXInc, USHORT uXPos, RECTL *prcl)
   {
   WinOffsetRect (NULL, prcl, - (SHORT)(uXPos * uXInc), 0);
   return (rclUpdate->xRight > prcl->xLeft &&
           rclUpdate->xLeft  < prcl->xRight);
   }


/*
 * uYPos is row rel to screen
 */
static int YShift (USHORT uYInc, USHORT uYPos, RECTL *prcl)
   {
   WinOffsetRect (NULL, prcl, 0, - (SHORT)(uYPos * uYInc));
   return 0;
   }




static int PaintClient (HWND hwnd)
   {
   HHEAP    hheap;
   HPS      hps;
   PEDT     pedt;
   PLWDAT   plwd;
   PSZ      pszText;
   RECTL    *prcl,     rclUpdate,  rclRow,    arcl[ITILW_RCLBUF];
   POINTS   ptsScrPos, ptsMax;
   LONG     lBColor,   lBkgColor,  lSColor;
   BOOL     bMSel;
   USHORT   uYWndSize, uActive,    uNumCols,  uNumRows;
   USHORT   uCol,      uRow,       uFirstScr, uLastScr;
   USHORT   uXPos,     uYPos,      uXInc,     uYInc;
   USHORT   fOptions,  uXWndSize,  uXMaxRight;

   pedt     = LWGetPEDT   (hwnd, TRUE, TRUE);
   plwd     = LWGetPLWDAT (hwnd, TRUE);
   hheap    = LWGetHHEAP  (hwnd, TRUE);
   uNumCols = LWQuery (hwnd, LWM_NumCols, TRUE);
   uNumRows = LWQuery (hwnd, LWM_NumRows, TRUE);
   uActive  = LWQuery (hwnd, LWM_Active, TRUE);
   uXInc    = LWQuery (hwnd, LWM_XScrollInc, TRUE);
   uYInc    = LWQuery (hwnd, LWM_YScrollInc, TRUE);
   uYWndSize= LWQuery (hwnd, LWM_YWindowSize, TRUE);
   uXWndSize= LWQuery (hwnd, LWM_XWindowSize, TRUE);
   lBColor  = LWColor (hwnd, LWC_CLIENT, TRUE);
   lSColor  = LWColor (hwnd, LWC_SELECT, TRUE);
   fOptions = LWQuery (hwnd, LWM_Options, TRUE);
   GetScrollPos (hwnd, &ptsScrPos, &ptsMax);
   uXPos    = (USHORT) ptsScrPos.x;
   uYPos    = (USHORT) ptsScrPos.y;
   bMSel    = (fOptions & LWS_MULTISELECT) == LWS_MULTISELECT;


   /*--- set ptr for temp rcls for the columns ---*/
   prcl = &(arcl[0]);
   if (uNumCols >= ITILW_RCLBUF)
      prcl = (RECTL *)ItiMemAlloc (hheap, sizeof (RECTL) * uNumCols, 0);
      
   /*--- start paint ---*/
   hps = WinBeginPaint (hwnd, NULL, &rclUpdate);

   WinFillRect (hps, &rclUpdate, lBColor);

   if (uNumRows == 0)
      return !WinEndPaint (hps);

   /*--- call init draw fn if defined by the user ---*/
   if (plwd->pfnInitDraw != NULL)
      (*(plwd->pfnInitDraw))(hwnd, hps);

   /*--- calc starting and ending rows to paint ---*/
   uFirstScr = min ((uYWndSize - (USHORT)rclUpdate.yTop)    / uYInc,
                     uNumRows -1 -uYPos);
   uLastScr  = min ((uYWndSize - (USHORT)rclUpdate.yBottom) / uYInc,
                     uNumRows -1 -uYPos);

   /*--- this will set all the columns rectangles to be in the  ---*/
   /*--- correct position on the first painted line in the list ---*/
   for (uCol = 0; uCol < uNumCols; uCol++)
      {
      prcl[uCol] = pedt[uCol].rcl;               
      ItiWndPrepRcl (hwnd, pedt[uCol].uCoordType, &prcl[uCol]);
      /*--- x shift due to hScroll ---*/
      if (!XShift (&rclUpdate, uXInc, uXPos, &prcl[uCol]))
         prcl[uCol].xLeft = ITILW_NOPAINT;
      /*-- move from top line to first line to be painted---*/
      YShift (uYInc, uFirstScr, &prcl[uCol]);
      }

/*************************************NEW***********************/
   rclRow         = rclUpdate;
   rclRow.yBottom = 0;
   rclRow.yTop    = uYInc;
   ItiWndPrepRcl (hwnd, TL|PIX|X0ABS|Y0ABS|X1ABS|Y1ABS, &rclRow);
   YShift (uYInc, uFirstScr, &rclRow);
/*************************************NEW***********************/

   for (uRow = uFirstScr + uYPos; uRow <= uLastScr + uYPos; uRow++)
      {
/*************************************NEW***********************/
      lBkgColor = (bMSel && LWIsSelected(hwnd, uRow) ? lSColor : lBColor);
      WinFillRect (hps,  &rclRow, lBkgColor);
      WinOffsetRect (NULL, &rclRow, 0, - (SHORT) uYInc);
/*************************************NEW***********************/

      for (uCol = 0; uCol < uNumCols; uCol++)
         {
         if (prcl[uCol].xLeft == ITILW_NOPAINT)  /* not in update region */
            continue;
         if (pedt[uCol].lFlags & ITIWND_HIDE)
            continue;

         if (plwd->pfnDraw == NULL)
            {
            (*(plwd->pfnTxt))(hwnd, uRow, uCol, &pszText);
            WinDrawText (hps,
                         -1,
                         pszText,
                         &prcl[uCol],
                         pedt[uCol].lFColor,
                         lBkgColor,
                         LOUSHORT (pedt[uCol].lFlags));
            }
         else /* (plwd->DrawClientText != NULL) */
            {
            DRAWINFO diInfo;

            diInfo.hps     = hps;
            diInfo.uRow    = uRow;
            diInfo.uCol    = uCol;
            diInfo.prcl    = &prcl[uCol];
            diInfo.lFColor = pedt[uCol].lFColor;
            diInfo.lBColor = lBColor;
            diInfo.lFlags  = pedt[uCol].lFlags;

            (*(plwd->pfnDraw))(hwnd, &diInfo);
            }
         WinOffsetRect (NULL, &prcl[uCol], 0, - (SHORT) uYInc);
         }
      }
   if ((uActive >= uFirstScr + uYPos && uActive <= uLastScr + uYPos) &&
       ((fOptions & LWS_SELECT)==LWS_SELECT))
      {
      lBColor  = LWColor (hwnd, LWC_ACTIVE, TRUE);
      /*--- added to always bound full screen ---*/
      uXMaxRight = uXWndSize + ptsMax.x * uXInc;
      WndAssignRcl (&rclUpdate, 0, 0, uXMaxRight, uYInc);
      ItiWndPrepRcl (hwnd, TL, &rclUpdate);                /*-top line-*/
      XShift (&rclUpdate, uXInc, uXPos, &rclUpdate);
      YShift (uYInc, uActive-uYPos, &rclUpdate);
      WinDrawBorder (hps, &rclUpdate, 1, 1, lBColor, lBColor, DB_STANDARD);
      }
   WinEndPaint (hps);
   return 0;
   }





static int PaintLabel (HWND hwnd)
   {
   HPS    hps;
   RECTL  rcl, rclUpdate;
   POINTS ptsScrPos, ptsMax;
   USHORT uNumCols, uCol, uFlags, uOptions, uXInc;
   LONG   lFColor, lBColor;
   PEDT   pedt;
   PSZ    *ppszTxt, pszText;

   pedt     = LWGetPEDT (hwnd, TRUE, FALSE);
   ppszTxt  = LWGetPPSZ (hwnd);
   GetScrollPos (hwnd, &ptsScrPos, &ptsMax);
   uNumCols = LWQuery (hwnd, LWM_NumLabelCols, TRUE);
   lBColor  = LWColor (hwnd, LWC_LABEL, TRUE);
   uOptions = LWQuery (hwnd, LWM_Options, TRUE);
   uXInc    = LWQuery (hwnd, LWM_XScrollInc, TRUE);

   if ((hps = WinBeginPaint (hwnd, NULL, &rclUpdate)) == NULL)
      return okCliErr ("Unable To Obtain Presentation Space", 0);
   WinFillRect (hps, &rclUpdate, LWColor (hwnd, LWC_LABEL, TRUE));

   for (uCol = 0; uCol < uNumCols; uCol++)
      {
      rcl = pedt[uCol].rcl;
      ItiWndPrepRcl (hwnd, pedt[uCol].uCoordType, &rcl);

      if (pedt[uCol].lFlags & ITIWND_HIDE)
         continue;

      if (XShift (&rclUpdate, uXInc, ptsScrPos.x, &rcl))
         {
         pszText = ppszTxt[uCol];
         lFColor = pedt[uCol].lFColor;
         uFlags  = LOUSHORT (pedt[uCol].lFlags);
         WinDrawText (hps, -1, pszText, &rcl, lFColor, lBColor, uFlags);
         }
      }

//   if (uOptions & LWS_UNDERLINELABEL)
//      {
//      WinQueryWindowRect (hwnd, &rcl);
//      rcl.yTop = rcl.yBottom + 1;
//      WinDrawBorder (hps, &rcl, 1, 1, SYSCLR_WINDOWFRAME,
//                     SYSCLR_BACKGROUND, DB_STANDARD);
//      }
   WinEndPaint (hps);

   return 0;
   }




/*
 *      ------------------------------------------------
 *      --- WINDOW PROCEDURE FOR LIST WINDOW CLIENTS ---
 *      ------------------------------------------------
 */

WNDPROC LWClientProc (HWND hwnd, USHORT umsg, MPARAM mp1, MPARAM mp2)
   {
   switch (umsg)
      {
      case WM_PAINT:
         PaintClient (hwnd);
         return 0;
         break;

      case WM_ERASEBACKGROUND:
         return (MRESULT)TRUE;
         break;

      case WM_BUTTON1DOWN:
      case WM_BUTTON2DBLCLK:
         {
         USHORT   uNewRow, fOptions, uDistance;

         WinSetActiveWindow (HWND_DESKTOP, hwnd);
         SetClientFocus (hwnd, LWF_SPECIFIED, TRUE);
         if ((uNewRow = LWRowFromMouse (hwnd, mp1, &uDistance)) > LWMP_ERROR)
            return (MRESULT)TRUE;

         fOptions = LWQuery (hwnd, LWM_Options, TRUE);
         if ((fOptions & LWS_MULTISELECT)== LWS_MULTISELECT ||
             (fOptions & LWS_SELECT)     == LWS_SELECT)
            {
            LWDoMouseSelect (hwnd, mp1, umsg);
            LWSendActiveMsg (hwnd, 1);
            }

         /*--- FALL THROUGH ---*/
         }

      case WM_BUTTON2DOWN:
      case WM_BUTTON1DBLCLK:
         LWMouseClick (hwnd, mp1, mp2, TRUE,
                     umsg == WM_BUTTON1DBLCLK || umsg == WM_BUTTON2DBLCLK,
                     umsg == WM_BUTTON1DBLCLK || umsg == WM_BUTTON1DOWN);
         return (MRESULT) TRUE;

      case WM_CHAR:
         if (!WndDoKey (hwnd, mp1, mp2))
            {
            HWND hwndX;

            hwndX = WinQueryWindow (WinQueryWindow(hwnd,QW_PARENT,0),QW_PARENT,0);
            WinPostMsg (hwndX, umsg, mp1, mp2);
            }
         return 0;
         break;

      default:
         return WinDefWindowProc (hwnd, umsg, mp1, mp2);
      }
   return 0L;
   }




/*
 *      ------------------------------------------------
 *      --- WINDOW PROCEDURE FOR LIST WINDOW LABELS ---
 *      ------------------------------------------------
 */

WNDPROC LWLabelProc  (HWND hwnd, USHORT umsg, MPARAM mp1, MPARAM mp2)
   {
   switch (umsg)
      {
      case WM_PAINT:
         return (MRESULT) PaintLabel (hwnd);
      case WM_ERASEBACKGROUND:
         return (MRESULT) TRUE;

      case WM_BUTTON1DOWN:
      case WM_BUTTON1DBLCLK:
      case WM_BUTTON2DBLCLK:
         WinSetActiveWindow (HWND_DESKTOP, hwnd);
         LWMouseClick (hwnd, mp1, mp2, FALSE,
                     umsg == WM_BUTTON1DBLCLK || umsg == WM_BUTTON2DBLCLK,
                     umsg == WM_BUTTON1DBLCLK || umsg == WM_BUTTON1DOWN);
         return (MRESULT) TRUE;
      }
   return WinDefWindowProc (hwnd, umsg, mp1, mp2);
   }

