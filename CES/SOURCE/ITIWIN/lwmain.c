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
 * lwmain.c                                                             *
 *                                                                      *
 * Copyright (C) 1990-1992 Info Tech, Inc.                              *
 *                                                                      *
 *  This file contains the code to paint the frame window               *
 *    and contains the frame window and init procs                      *
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

#define  INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itiutil.h"
#include "..\include\itibase.h"
#include "..\include\itiglob.h"
#include "..\include\itierror.h"
#include "..\include\itimbase.h"
#include "..\include\itidbase.h"
#include "winmain.h"
#include "lwmain.h"
#include "lwutil.h"
#include "lwclient.h"
#include "lwsplit.h"
#include "lwselect.h"
#include "windef.h"
#include "initdll.h"
#include "winutil.h"
#include "winupdat.h"
#include "lwsize.h"
#include "lwkey.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define WORKING_COLOR   CLR_PALEGRAY
#define DONE_COLOR      LWCLR_DEFCLIENT


WNDPROC LWLabelFixProc (HWND hwnd, USHORT umsg, MPARAM mp1, MPARAM mp2);


USHORT LWID_VSPLIT       =  0x0001;
USHORT LWID_HSPLIT       =  0x0002;

USHORT LWID_LABEL[2]     =  {0x0003, 0x0004};
USHORT LWID_HSB[2]       =  {0x0005, 0x0006};
USHORT LWID_VSB[2]       =  {0x0007, 0x0008};
USHORT LWID_CLIENT[2][2] = {{0x0009, 0x000A},
                            {0x000B, 0x000C}};
USHORT LWID_QBUTTON      =  0x000D;


char szLWLabelClass[]  = "LWLabel";
char szLWDataClass[]   = "LWData";
char szLWSplitClass[]  = "LWSplit";

SHORT xVSB;
SHORT yHSB;
SHORT yHSPLIT;
SHORT xVSPLIT;


#define LWSB_CUSTOMSCROLL 101

int CreateKids (HWND hwndFrame);
USHORT LWSetColor (HWND hwnd, USHORT uColorType, long lColor);



USHORT okLwErr (PSZ psz, USHORT uId)
   {
   char  szTmp [512];

   sprintf (szTmp, "ITIWIN: LwMain.c : %s%d", psz, uId);
   ItiErrWriteDebugMessage (szTmp);
   WinMessageBox (pglobals->hwndDesktop, pglobals->hwndAppFrame, szTmp,
                  "List Client Error", 0, MB_OK | MB_APPLMODAL);
   return TRUE;
   }


int GlobalsInit (void)
   {
//   xVSB     = (USHORT) WinQuerySysValue (HWND_DESKTOP, SV_CXVSCROLL);
//   yHSB     = (USHORT) WinQuerySysValue (HWND_DESKTOP, SV_CYHSCROLL);
//   yHSPLIT  = (USHORT) WinQuerySysValue (HWND_DESKTOP, SV_CYSIZEBORDER);
//   xVSPLIT  = (USHORT) WinQuerySysValue (HWND_DESKTOP, SV_CXSIZEBORDER);

   /*--- These are copied because they can be set independently ---*/
   xVSB     = (USHORT) pglobals->alSysValues [SV_CXVSCROLL];
   yHSB     = (USHORT) pglobals->alSysValues [SV_CYHSCROLL];
   yHSPLIT  = (USHORT) pglobals->alSysValues [SV_CYSIZEBORDER];
   xVSPLIT  = (USHORT) pglobals->alSysValues [SV_CXSIZEBORDER];
   return 0;
   }


/* returns 0 if all is ok
 */
USHORT EXPORT LWRegister (HAB hab)
   {
   GlobalsInit ();
   InitMousePointers();
   if (WinRegisterClass (hab, ITIWND_LISTCLASS,  LWFrameProc,  0 /*CS_SIZEREDRAW*/,
      sizeof (PLWDAT)) == FALSE)
      return 1;

/*---temporary replace LWLabelProc with LWLabelFixProc---*/
//   if (WinRegisterClass (hab, szLWLabelClass, LWLabelFixProc, 0, 0) == FALSE)

   if (WinRegisterClass (hab, szLWLabelClass, LWLabelProc, 0, 0) == FALSE)
      return 1;
   if (WinRegisterClass (hab, szLWDataClass, LWClientProc, 0, 0) == FALSE)
      return 1;
   if (WinRegisterClass (hab, szLWSplitClass,  LWSplitProc,
       CS_SIZEREDRAW| CS_MOVENOTIFY| CS_SYNCPAINT, 2*sizeof(USHORT))==FALSE)
      return 1;
   ItiMbRegisterListWnd (ITI_DEFAULTWND, ItiWndDefListWndTxtProc, 0);

   return 0;
   }


USHORT EXPORT ItiWndTerminate (void)
   {
   DeInitMousePointers ();
   return 0;
   }


void NullPLWD (HWND hwnd)
   {
   PLWDAT   plwd;

   if (!(plwd = LWGetPLWDAT (hwnd, FALSE)))
      return;

   plwd->Init = FALSE;
   plwd->pfnDraw    = NULL; plwd->pfnInitDraw = NULL; 
   plwd->pedtData   = NULL; plwd->pedtLabel   = NULL; 
   plwd->ppszLabels = NULL; plwd->hbuf        = NULL; 
   plwd->pfnTxt     = NULL; plwd->puSel       = NULL;
   plwd->pExtra     = NULL;

   plwd->uDataRows   = 0;
   plwd->uDataCols   = 0;
   plwd->uLabelCols  = 0;
   plwd->uActive     = 0;
   plwd->uStartSel   = 0;
   plwd->uScrollType = 0;
   plwd->uOptions    = 0;

   plwd->lSplitMin   = 0;
   plwd->lSplitMax   = 0;

   plwd->bV0Odd      = FALSE;
   plwd->bV1Odd      = FALSE;
   }




void FreePLWDAT (HWND hwnd)
   {
   PLWDAT plwd;

   if (!(plwd = LWGetPLWDAT (hwnd, FALSE)))
      return;

   /*--- Free Buffer ---*/
   if (plwd->hbuf)
      ItiDbDeleteBuffer (plwd->hbuf, hwnd);

   /*--- Free Select Array ---*/
   if ((LWQuery (hwnd, LWM_Options, FALSE) & LWS_MULTISELECT)==LWS_MULTISELECT)
      ItiMemFree (plwd->hheap, plwd->puSel);

   /*--- Free Labels ---*/
   ItiFreeStrArray (plwd->hheap, plwd->ppszLabels, plwd->uLabelCols);

   /*--- Free Data/Label Edt's ---*/
   if (plwd->pedtLabel != plwd->pedtData)
      ItiMemFree (plwd->hheap, plwd->pedtLabel);
   ItiMemFree (plwd->hheap, plwd->pedtData);

   /*--- Free Extra ---*/
   if (plwd->pExtra)
      ItiMemFree (plwd->hheap, plwd->pExtra);

   NullPLWD (hwnd);
   }


//int FreePLWDAT (HWND hwnd)
//   {
//   USHORT   i;
//   PLWDAT     plwd;
//
//   plwd = LWGetPLWDAT (hwnd, FALSE);
//
//   if (plwd->pedtLabel != plwd->pedtData)
//      ItiMemFree (plwd->hheap, plwd->pedtLabel);
//   ItiMemFree (plwd->hheap, plwd->pedtData);
//
//   for (i = 0; i < plwd->uLabelCols; i++)
//      ItiMemFree (plwd->hheap, plwd->ppszLabels[i]);
//
//   ItiMemFree (plwd->hheap, plwd->ppszLabels);
//   plwd->Init = FALSE;
//   ItiMemFree (plwd->hheap, plwd->puSel);
//   return 0;
//   }



int KillKids (HWND hwnd)
   {
   HENUM henum;
   HWND  hwndChild;

   henum = WinBeginEnumWindows (hwnd);
   while ((hwndChild = WinGetNextWindow (henum)) != NULL)
      WinDestroyWindow (hwndChild);
   WinEndEnumWindows (henum);
   return 0;                 
   }



/*
 *  excludes pct coordinates from rcl sum.
 */
RECTL WinUnionRectangles (PEDT pedt, USHORT uCount)
   {
   RECTL    rclSum = {0, 0, 1, 1};
   RECTL    rclTmp;
   USHORT   i;

   ItiWndPrepRcl (HWND_DESKTOP, TL|PIX|X0ABS|Y0ABS|X1ABS|Y1ABS, &rclSum);
   for (i = 0; i < uCount; i++)
      {
      rclTmp = pedt[i].rcl; 
      ItiWndPrepRcl (HWND_DESKTOP, pedt[i].uCoordType, &rclTmp);
      if ((pedt[i].uCoordType & COORD_X0TYPE) == X0PCT)
         rclTmp.xLeft  = rclSum.xLeft;
      if ((pedt[i].uCoordType & COORD_Y0TYPE) == Y0PCT)
         rclTmp.yBottom= rclSum.yBottom;
      if ((pedt[i].uCoordType & COORD_X1TYPE) == X1PCT)
         rclTmp.xRight = rclSum.xRight;
      if ((pedt[i].uCoordType & COORD_Y1TYPE) == Y1PCT)
         rclTmp.yTop   = rclSum.yTop;
      WinUnionRect (NULL, &rclSum, &rclTmp, &rclSum);
      }
   WndOs2WinRcl (HWND_DESKTOP, &rclSum);
   return rclSum;
   }


/* This function sets the row count
 * it will also reallocate the multi selection
 * bit array if it is enabled
 */
USHORT LWSetRowCount (HWND hwndFrame, USHORT uCount)
   {
   PLWDAT   plwd;
   USHORT   fOptions, uSelSize, i;
   
   plwd     = LWGetPLWDAT (hwndFrame, FALSE);
   plwd->uDataRows = uCount;

   fOptions = LWQuery (hwndFrame, LWM_Options, FALSE);
   if ((fOptions & LWS_MULTISELECT) != LWS_MULTISELECT)
      return uCount;

   uSelSize = 16 * (1 + uCount / 256);
   if (uSelSize == plwd->uSelSize)
      return uCount;

   plwd->puSel = ItiMemRealloc (plwd->hheap, plwd->puSel,
                                sizeof (USHORT) * uSelSize, 0);

   for (i = plwd->uSelSize; i < uSelSize; i++)
      plwd->puSel[max (0, (SHORT)i - 1)] = 0;

   plwd->uSelSize = uSelSize;
   return uCount;
   }
















int EXPORT LWInit (HWND hwnd,
                  HMODULE hmod,
                  PLWINIT plwInit,
                  EDT aedtData[],
                  EDT aedtLabel[],
                  PSZ pszLabelTxt[],
                  PFNTXT pfnTxt)
   {
   PLWDAT   plwd;
   USHORT   i, uSize, uId;
   RECTL    rclTmp;
   HHEAP    hheap;

   plwd = LWGetPLWDAT (hwnd, FALSE);
   uId  = WinQueryWindowUShort (hwnd, QWS_ID);

   if (plwd->Init)
      {
      KillKids (hwnd);
      FreePLWDAT (hwnd);
      }

   hheap = plwd->hheap;
   plwd->uDataCols = plwInit->uDataCols;
   plwd->uOptions  = plwInit->uOptions;
   plwd->uActive   = 0;
   plwd->uStartSel = 0;
   plwd->pfnTxt    = (pfnTxt == NULL ? GetTextElement : pfnTxt);
   plwd->hbuf  = NULL;
   plwd->uScrollType = SB_SLIDERTRACK;
   plwd->pExtra = NULL;

   /*--- Alloc memory for multi selection bit array if enabled ---*/
   plwd->puSel = NULL;
   plwd->uSelSize = 0;
   LWSetRowCount (hwnd, plwInit->uDataRows);

   if (!(plwInit->uOptions & LWS_LABEL))
      plwd->uLabelCols  = 0;
//   else if (plwInit->uLabelCols == 0)              /* default */
//      plwd->uLabelCols  = plwInit->uDataCols;
   else
      plwd->uLabelCols  = plwInit->uLabelCols;

   if (plwInit->uXScrollInc == 0)                  /* default */
      plwd->uXScrollInc = pglobals->xChar * 3;
   else
      plwd->uXScrollInc = (plwInit->uXScrollInc * pglobals->xChar) / 2; /*dlg*/

   LWSetColor (hwnd, LWC_RESET, 0L);

   uSize = sizeof (EDT) * plwd->uDataCols;
   if ((plwd->pedtData = memcpy (ItiMemAlloc (hheap, uSize, 0),
                                 aedtData, uSize)) == NULL)
      return okLwErr ("Unable To alloc memory for label data. Id:",uId);

   if (aedtLabel == NULL || !(plwInit->uOptions & LWS_LABEL)) /* default */
      plwd->pedtLabel = plwd->pedtData;
   else
      {
      uSize = sizeof (EDT) * plwd->uLabelCols;
      if ((plwd->pedtLabel = memcpy (ItiMemAlloc (hheap, uSize, 0), aedtLabel, uSize)) == NULL)
         return okLwErr ("Unable To alloc memory for data cols. Id:", uId);
      }

   plwd->rclData = WinUnionRectangles (plwd->pedtData, plwd->uDataCols);
   if (plwInit->uDataYSize > 0)             /* un-default */
      plwd->rclData.yTop = (plwInit->uDataYSize * pglobals->yChar) / 4; /*dlg*/;

   if (!(plwInit->uOptions & LWS_LABEL))
      plwd->uYLabel = 0;
   else if (plwInit->uLabelYSize > 0)
      plwd->uYLabel = (plwInit->uLabelYSize * pglobals->yChar) / 4; /*dlg*/
   else  /* default */
      {
      rclTmp = WinUnionRectangles (plwd->pedtLabel, plwd->uLabelCols);
      plwd->uYLabel = (USHORT)rclTmp.yTop;
      }


   if (plwInit->uOptions & LWS_LABEL)
      {
      if ((plwd->ppszLabels = (PSZ *) ItiMemAlloc (hheap, sizeof (PSZ) * plwd->uLabelCols, 0)) == NULL)
         return okLwErr ("Unable to alloc mem for labels. Id:", uId);

      for (i = 0; i < plwd->uLabelCols; i++)
         plwd->ppszLabels[i] = ItiStrDup (hheap, pszLabelTxt[i]);
      }
   else
      plwd->ppszLabels = NULL;

   plwd->Init = TRUE;

   CreateKids (hwnd);
   WinSendMsg (hwnd, WM_RESET, 0L, 0L);

   return 0;
   }






static void WinInvalidateChild (HWND hwndFrame, USHORT uChildType)
   {
   SHORT   fOptions, x, y;

   fOptions = LWQuery (hwndFrame, LWM_Options, FALSE);

   switch (uChildType)
      {
      case LWC_CLIENT:
         for (x = 0; x < ((fOptions & LWS_VSPLIT) == LWS_VSPLIT ? 2 : 1); x++)
            for (y = 0; y < ((fOptions & LWS_HSPLIT) == LWS_HSPLIT ? 2 : 1); y++)
               WinInvalidateRect (WinWindowFromID (hwndFrame, LWID_CLIENT[1-y][x]), NULL, 0);
         return;

      case LWC_LABEL:
         for (x = 0; x < ((fOptions & LWS_VSPLIT) == LWS_VSPLIT ? 2 : 1); x++)
            if (fOptions & LWS_LABEL)
               WinInvalidateRect (WinWindowFromID (hwndFrame, LWID_LABEL[x]), NULL, 0);
         return;
      }
   return;
   }








//int LWInitDataDraw (HWND hwnd,
//                      FNINITDRAW InitDataDraw,
//                      FNDRAWTEXT DrawDataText)
//   {
//   PLWDAT  plwd;
//
//   plwd = LWGetPLWDAT (hwnd, FALSE);
//
//   plwd->InitDataDraw = InitDataDraw;
//   plwd->DrawDataText = DrawDataText;
//   return 0;
//   }
//


/* valid uColorTypes Are:
 *    LWC_RESET     resets all 4 colors to default values   
 *    LWC_CLIENT    sets client background color
 *    LWC_LABEL     sets label background color
 *    LWC_SELECT    selects selected element background color
 *    LWC_ACTIVE    selects active element outline color
 *
 * any valid color may be used for the color value including
 * the default values defined above
 */                      
                       



int CreateKids (HWND hwndFrame)
   {
   int    x, y;
   POINTS ptsPos, ptsSize;
   HWND   hwnd;
   HAB    hab;
   LONG   *pFlags = NULL;
   USHORT fOptions, uId;


   uId  = WinQueryWindowUShort (hwndFrame, QWS_ID);
   if ((hab = WinQueryAnchorBlock (hwndFrame)) == NULL)
      return okLwErr ("Unable To obtain a hab... This is bad. Id:", uId);

   fOptions =   LWQuery (hwndFrame, LWM_Options, FALSE);

   /* make split windows */
   if ((fOptions & LWS_VSPLIT) == LWS_VSPLIT)
      {
      CalcWndPosSize (hwndFrame, LWID_VSPLIT, &ptsPos, &ptsSize);
      hwnd = WinCreateWindow (hwndFrame, szLWSplitClass, "",
             WS_VISIBLE, ptsPos.x, ptsPos.y, ptsSize.x, ptsSize.y,
             hwndFrame, /*HWND_BOTTOM*/ HWND_TOP, LWID_VSPLIT, pFlags, NULL);
      if (hwnd == NULL)   
         ItiErrDisplayWindowError (hab, NULL, 0);
      }

   if ((fOptions & LWS_HSPLIT) == LWS_HSPLIT)
      {
      CalcWndPosSize (hwndFrame, LWID_HSPLIT, &ptsPos, &ptsSize);
      hwnd = WinCreateWindow (hwndFrame, szLWSplitClass, "",
             WS_VISIBLE, ptsPos.x, ptsPos.y, ptsSize.x, ptsSize.y,
             hwndFrame, /*HWND_BOTTOM*/ HWND_TOP, LWID_HSPLIT, pFlags, NULL);
      if (hwnd == NULL)   
         ItiErrDisplayWindowError (hab, NULL, 0);
      }
   /* make VSB Windows */
   for (y = 0; y < ((fOptions & LWS_HSPLIT) == LWS_HSPLIT ? 2 : 1); y++)
      {
      if (fOptions & LWS_VSCROLL)
         {
         CalcWndPosSize (hwndFrame, LWID_VSB[y], &ptsPos, &ptsSize);
         hwnd = WinCreateWindow (hwndFrame, WC_SCROLLBAR, "",
                SBS_VERT | WS_VISIBLE, ptsPos.x, ptsPos.y,ptsSize.x, ptsSize.y,
                hwndFrame, HWND_TOP, LWID_VSB[1-y], pFlags, NULL);
         if (hwnd == NULL)   
            ItiErrDisplayWindowError (hab, NULL, 0);
         }
      }

   for (x = 0; x < ((fOptions & LWS_VSPLIT) == LWS_VSPLIT ? 2 : 1); x++)
      {
      /* make HSB Windows */
      if (fOptions & LWS_HSCROLL)
         {
         CalcWndPosSize (hwndFrame, LWID_HSB[x], &ptsPos, &ptsSize);
         hwnd = WinCreateWindow (hwndFrame, WC_SCROLLBAR, "",
                SBS_HORZ | WS_VISIBLE, ptsPos.x, ptsPos.y, ptsSize.x, ptsSize.y,
                hwndFrame, HWND_TOP, LWID_HSB[x], pFlags, NULL);
         if (hwnd == NULL)   
            ItiErrDisplayWindowError (hab, NULL, 0);
         }


      /* make Label Windows */
      if (fOptions & LWS_LABEL)
         {
         CalcWndPosSize (hwndFrame, LWID_LABEL[x], &ptsPos, &ptsSize);
         hwnd = WinCreateWindow (hwndFrame, szLWLabelClass, "",
                WS_VISIBLE, ptsPos.x, ptsPos.y, ptsSize.x, ptsSize.y,
                hwndFrame, HWND_TOP, LWID_LABEL[x], pFlags, NULL);
         if (hwnd == NULL)   
            ItiErrDisplayWindowError (hab, NULL, 0);
         }

      for (y = 0; y < ((fOptions & LWS_HSPLIT) == LWS_HSPLIT ? 2 : 1); y++)
         {
         /* make client windows */
         CalcWndPosSize (hwndFrame, LWID_CLIENT[y][x], &ptsPos, &ptsSize);
         hwnd = WinCreateWindow (hwndFrame, szLWDataClass, "",
                WS_VISIBLE, ptsPos.x, ptsPos.y, ptsSize.x, ptsSize.y,
                hwndFrame, HWND_TOP, LWID_CLIENT[1-y][x], pFlags, NULL);
         if (hwnd == NULL)   
            ItiErrDisplayWindowError (hab, NULL, 0);
         }
      }
   /* new buttons */
   if (fOptions & LWS_QBUTTON) 
      {
      CalcWndPosSize (hwndFrame, LWID_QBUTTON, &ptsPos, &ptsSize);
      hwnd = WinCreateWindow (hwndFrame, WC_BUTTON, "?",
                               WS_VISIBLE | BS_PUSHBUTTON | BS_NOPOINTERFOCUS,
                               ptsPos.x, ptsPos.y,
                               ptsSize.x, ptsSize.y,
                               hwndFrame, HWND_BOTTOM,
                               LWID_QBUTTON, pFlags, NULL);
      if (hwnd == NULL)   
            ItiErrDisplayWindowError (hab, NULL, 0);
       }
   SetClientFocus (hwndFrame, LWF_DEFAULT, FALSE);
   return 0;
   }



int ScrollDataWindow (HWND hwndFrame,
                        USHORT uScrollId,
                        SHORT iX, SHORT iY)
   {
   HWND hwndScroll;

   hwndScroll = WinWindowFromID (hwndFrame, uScrollId);

   if ((iY > 100) || (iY < -100))   // I.E. dont bother
      {
      WinInvalidateRect (hwndScroll, NULL, 0);
      }
   else
      {
      WinScrollWindow (hwndScroll,
                    -iX * LWQuery (hwndScroll, LWM_XScrollInc, TRUE), 
                     iY * LWQuery (hwndScroll, LWM_YScrollInc, TRUE),
                     (PRECTL) NULL, (PRECTL) NULL, (HRGN) NULL,
                     (PRECTL) NULL, SW_INVALIDATERGN);
      }
   return 0;
   }




/* mp1L = uScrollId
 * mp2l = SliderTrackPosition
 * mp2H = ScrollCmd
 */

int DoVScroll (HWND hwndFrame, MPARAM mp1, MPARAM mp2)
   {
   USHORT   uScrollId, uMax, uPos, uRow;
   HWND     hwndScroll, hwndParent;
   SHORT    i, x;
   USHORT   fOptions, uId, uSliderPos;
   PLWDAT   plwd;

   fOptions = LWQuery (hwndFrame, LWM_Options, FALSE);

   if ((fOptions & LWS_VSCROLL) != LWS_VSCROLL)
      return 0;

   plwd = LWGetPLWDAT (hwndFrame, FALSE);
   uScrollId = SHORT1FROMMP (mp1);

   if ((hwndScroll = WinWindowFromID (hwndFrame, uScrollId)) == NULL)
      return okLwErr ("Unable to obtain window from Id. Id:", uScrollId);

   GetScrollInfo (hwndScroll, &uPos, &uMax);

   switch (SHORT2FROMMP (mp2))
      {
      case SB_LINEUP:
         i = -1;
         break;
      case SB_LINEDOWN:
         i =  1;
         break;
      case SB_PAGEUP:
         i = 0-(SHORT)LWQuery (hwndScroll, LWM_YWindowSize, TRUE) /
               (SHORT)LWQuery (hwndScroll, LWM_YScrollInc, TRUE);
         break;
      case SB_PAGEDOWN:
         i =  (SHORT)LWQuery (hwndScroll, LWM_YWindowSize, TRUE) /
              (SHORT)LWQuery (hwndScroll, LWM_YScrollInc, TRUE);
         break;
      case SB_SLIDERPOSITION:
      case SB_SLIDERTRACK:
         i = 0;
         if (plwd->uScrollType==SHORT2FROMMP (mp2))
            {
            uSliderPos = SHORT1FROMMP(mp2);
            if (_ScrollDoubled (hwndScroll))
               uSliderPos *= 2;
            i = (uSliderPos - uPos);
            }
         break;
      case SB_LWTRACK:
         i = (SHORT1FROMMP(mp2)-uPos);
         break;

      case LWSB_CUSTOMSCROLL:
         i =  SHORT1FROMMP (mp2);
         break;
      default:
         i =  0;
         break;
      }

   if (!(i = max (-((LONG) uPos), min ((LONG)uMax - (LONG)uPos, (LONG)i))))
      return 0;
   uRow = (uScrollId==LWID_VSB[0] ? 0 : 1);

   _SetScrollPos (hwndScroll, uPos + i);

   hwndParent = WinQueryWindow (hwndFrame, QW_PARENT, 0);
   uId = WinQueryWindowUShort (hwndFrame, QWS_ID);

   WinSendMsg (hwndParent, WM_CONTROL, MPFROM2SHORT (uId, LN_SCROLL),
                                       MPFROM2SHORT (uScrollId, uPos+i));

   for (x = 0; x < ((fOptions & LWS_VSPLIT) == LWS_VSPLIT ? 2 : 1); x++)
      ScrollDataWindow (hwndFrame,  LWID_CLIENT[uRow][x], 0, i);
   return 0;
   }


   
int DoHScroll (HWND hwndFrame, MPARAM mp1, MPARAM mp2)
   {
   USHORT   uScrollId, uMax, uPos, uCol;
   HWND     hwndScroll, hwndParent;
   SHORT    i, x;
   USHORT   fOptions, uId;

   uScrollId = SHORT1FROMMP (mp1);
   if ((hwndScroll = WinWindowFromID (hwndFrame, uScrollId)) == NULL)
      return okLwErr ("Unable to obtain window from Id. Id:", uScrollId);
   fOptions = LWQuery (hwndFrame, LWM_Options, FALSE);

   GetScrollInfo (hwndScroll, &uPos, &uMax);

   switch (SHORT2FROMMP (mp2))
      {
      case SB_LINELEFT:
         i = -1;
         break;
      case SB_LINERIGHT:
         i =  1;
         break;
      case SB_PAGELEFT:
         i = 0- (SHORT)LWQuery (hwndScroll, LWM_XWindowSize, TRUE) /
                (SHORT)LWQuery (hwndScroll, LWM_XScrollInc, TRUE);
         break;
      case SB_PAGERIGHT:
         i =  LWQuery (hwndScroll, LWM_XWindowSize, TRUE) /
              LWQuery (hwndScroll, LWM_XScrollInc, TRUE);
         break;
      case SB_SLIDERTRACK:
         i =  SHORT1FROMMP (mp2) - uPos;
         break;
      case LWSB_CUSTOMSCROLL:
         i =  SHORT1FROMMP (mp2);
         break;
      case SB_LWTRACK:
         i = SHORT1FROMMP(mp2)-uPos;
         break;
    default:
         return 0;
      }

   if (!(i = max (-(SHORT)uPos, min ((SHORT)uMax - (SHORT)uPos, i))))
      return 0;
   uCol = (uScrollId==LWID_HSB[0] ? 0 : 1);
   WinSendMsg (hwndScroll, SBM_SETPOS, MPFROMSHORT (uPos + i), 0L);

   hwndParent = WinQueryWindow (hwndFrame, QW_PARENT, 0);
   uId = WinQueryWindowUShort (hwndFrame, QWS_ID);
   WinSendMsg (hwndParent, WM_CONTROL, MPFROM2SHORT (uId, LN_SCROLL),
                                       MPFROM2SHORT (uScrollId, uPos+i));

   for (x = 0; x < ((fOptions &LWS_HSPLIT) == LWS_HSPLIT ? 2 : 1); x++)
      ScrollDataWindow (hwndFrame, LWID_CLIENT[1-x][uCol], i, 0);
   if (fOptions & LWS_LABEL)
      ScrollDataWindow (hwndFrame, LWID_LABEL [uCol], i, 0);
   WinUpdateWindow (WinWindowFromID (hwndFrame, LWID_LABEL[uCol]));
   return 0;
   }



int PaintFrameWnd (HWND hwnd)
   {
   HPS      hps;
   RECTL    rectl, rcl, rclP;
   USHORT   fOptions;

   if ((hps = WinBeginPaint (hwnd, (HPS)NULL, &rectl)) == NULL)
      return okLwErr ("Unable to paint Frame. Id:", 0);

   WinQueryWindowRect (hwnd, &rcl);

   /*--- spot at right of label area ---*/
   rclP = rcl;
   rclP.xLeft  = rclP.xRight - xVSB; 
   WinFillRect (hps, &rclP, LWColor (hwnd, LWC_LABEL, FALSE));

//   rclP = rcl;
//   rclP.yBottom = rclP.yTop   - 50;
//   rclP.xLeft   = rclP.xRight - 30; 
//   WinFillRect (hps, &rclP, LWColor (hwnd, LWC_LABEL, FALSE));
//
//   /*--- spot under q button ---*/
//   rclP.yBottom = rcl.yBottom;
//   rclP.yTop    = rclP.yBottom + 30;
//   WinFillRect (hps, &rclP, LWColor (hwnd, LWC_LABEL, FALSE));

   fOptions = LWQuery (hwnd, LWM_Options, FALSE);

   if (fOptions & LWS_BORDER)
      {
      WinQueryWindowRect (hwnd, &rcl);
      WinDrawBorder (hps, &rcl, 1, 1, SYSCLR_WINDOWFRAME,
                     SYSCLR_BACKGROUND, DB_STANDARD);
      }

   if (fOptions & LWS_UNDERLINELABEL)
      {
      LWQueryFullRect (hwnd, &rcl);
      rcl.yTop = rcl.yTop - LWQuery (hwnd, LWM_YLabelSize, FALSE);
      rcl.yBottom = rcl.yTop -1;
      WinDrawBorder (hps, &rcl, 1, 1, SYSCLR_WINDOWFRAME,
                     SYSCLR_BACKGROUND, DB_STANDARD);
      }
   WinEndPaint (hps);
   return 0;
   }



int SizeWnd (HWND hwndFrame, HWND hOrder,
             USHORT uChildId, USHORT uWndFlags)
   {
   HWND     hwndChild;
   POINTS   ptsPos, ptsSize;

   if ((hwndChild = WinWindowFromID (hwndFrame, uChildId)) == NULL)
      return okLwErr ("Unable to obtain child window. Id:", uChildId);
   CalcWndPosSize (hwndFrame, uChildId, &ptsPos, &ptsSize);
   WinSetWindowPos (hwndChild, hOrder, ptsPos.x, ptsPos.y,
                     ptsSize.x, ptsSize.y, uWndFlags);
   return 0;
   }



/*
   this uses the current size of the frame window and the position of
   the splitter windows to resize all the other windows.
   mp contains: low:frame win width,
                hi :frame win height.
*/
int SizeAllChildWindows (HWND hwndFrame, MPARAM mp)
   {
   int      x, y;
   USHORT   fOptions, fWndFlags;

   if (Init (hwndFrame) == FALSE)
      return 0;

   fOptions =   LWQuery (hwndFrame, LWM_Options, FALSE);
   fWndFlags =  SWP_MOVE | SWP_SIZE;

   for (x = 0; x < ((fOptions & LWS_VSPLIT) == LWS_VSPLIT ? 2 : 1); x++)
      {
      for (y = 0; y < ((fOptions & LWS_HSPLIT) == LWS_HSPLIT ? 2 : 1); y++)
         SizeWnd (hwndFrame, HWND_TOP, LWID_CLIENT[1-y][x], fWndFlags);
      if (fOptions & LWS_LABEL)
         SizeWnd (hwndFrame, HWND_TOP, LWID_LABEL[x], fWndFlags);
      if (fOptions & LWS_HSCROLL)
         SizeWnd (hwndFrame, HWND_TOP, LWID_HSB[x], fWndFlags);
      }

   /* --- paint splits after clients so clients paint correctly --- */
   if ((fOptions & LWS_VSPLIT) == LWS_VSPLIT)
      SizeWnd (hwndFrame, HWND_BOTTOM, LWID_VSPLIT, fWndFlags);

   if ((fOptions & LWS_HSPLIT) == LWS_HSPLIT)
      SizeWnd (hwndFrame, HWND_BOTTOM, LWID_HSPLIT, fWndFlags);

   for (y = 0; y < ((fOptions & LWS_HSPLIT) == LWS_HSPLIT ? 2 : 1); y++)
      {
      if (fOptions & LWS_VSCROLL)
         SizeWnd (hwndFrame, HWND_TOP, LWID_VSB[1-y], fWndFlags);
      }

   UpdateScrollBars (hwndFrame, LWSB_UPDATETHUMBSIZES, 0, 0);
   UpdateScrollBars (hwndFrame, LWSB_UPDATESCROLLRANGES, 0, 0);

   if (fOptions & LWS_QBUTTON)
      {
      SizeWnd (hwndFrame, HWND_BOTTOM, LWID_QBUTTON, fWndFlags);
      WinInvalidateRect (WinWindowFromID (hwndFrame, LWID_QBUTTON), NULL, FALSE);
      }

   SetClientFocus (hwndFrame, LWF_CURRENT, FALSE);
   return 0;
   }




/************************************************************************/
/******************LWFrameProc support routines end**********************/
/************************************************************************/

/************************************************************************/
/*****************LWFrameProc message routines start*********************/
/************************************************************************/

MRESULT WinSendParentMsg (HWND hwnd, USHORT uCmd, MPARAM mp1, MPARAM mp2)
   {
   return WinSendMsg (WinQueryWindow (hwnd, QW_PARENT, 0), uCmd, mp1, mp2);
   }




USHORT LWSetColor (HWND hwnd, USHORT uColorType, long lColor)
   {
   PLWDAT     plwd;

   plwd = LWGetPLWDAT (hwnd, FALSE);

   switch (uColorType)
      {
      case LWC_RESET:
         plwd->lDataColor = LWCLR_DEFCLIENT;
         plwd->lLabelColor  = LWCLR_DEFLABEL ;
         plwd->lSelectColor = LWCLR_DEFSELECT;
         plwd->lActiveColor = LWCLR_DEFACTIVE;
         plwd->lSplitBColor = LWCLR_DEFSPLITBORDER;
         plwd->lSplitMin    = LWCLR_DEFSPLITMIN;
         plwd->lSplitMax    = LWCLR_DEFSPLITMAX;
         WinInvalidateRect (hwnd, NULL, FALSE);
         break;

      case LWC_CLIENT:
         if (plwd->lDataColor != lColor)
            WinInvalidateChild (hwnd, uColorType);
         plwd->lDataColor = lColor;
         break;

      case LWC_LABEL: 
         if (plwd->lLabelColor != lColor)
            WinInvalidateChild (hwnd, uColorType);
         plwd->lLabelColor  = lColor;
         break;

      case LWC_SELECT:
         if (plwd->lSelectColor != lColor)
            WinInvalidateChild (hwnd, LWC_CLIENT);
         plwd->lSelectColor = lColor;
         break;

      case LWC_ACTIVE:
         if (plwd->lActiveColor != lColor)
            WinInvalidateChild (hwnd, LWC_CLIENT);
         plwd->lActiveColor = lColor;
         break;

      case LWC_SPLITBORDER:
         plwd->lSplitBColor = lColor;
         break;

      case LWC_SPLITMIN:
         plwd->lSplitMin    = lColor;
         break;

      case LWC_SPLITMAX:
         plwd->lSplitMax    = lColor;
         break;

      default:
         return 1;
      }
   return 0;
   }



/************************************************************************/
/******************LWFrameProc message routines end**********************/
/************************************************************************/

USHORT LWColIdToRelCol (HWND hwnd, USHORT u)
   {
   PLWDAT   plwd;
   USHORT   i;

   if (u >= 0x8000)
      return u & 0x7FFF;

   plwd = LWGetPLWDAT (hwnd, FALSE);
   for (i=0; i < plwd->uDataCols; i++)
      if (plwd->pedtData [i].uIndex == u)
         return i;
   return 0;
   }



MRESULT ProcessListQuery (HWND hwnd, MPARAM mp1, MPARAM mp2)
   {
   PLWDAT   plwd;
   USHORT   i, uId;

   plwd = LWGetPLWDAT (hwnd, FALSE);
   i = SHORT2FROMMP (mp1);

   switch (SHORT1FROMMP (mp1))
      {
      case ITIWND_PSTRUCT:
         return MRFROMP (plwd);

      case ITIWND_NUMSELECTIONS:
         return 0;                       /*--- not done ---*/

      case ITIWND_SELECTION:
         if (mp2)
            return MRFROMSHORT (ItiWndQueryBackSelection (hwnd, i));
         else
            return MRFROMSHORT (ItiWndQuerySelection (hwnd, i));

      case ITIWND_NUMROWS:
         return MRFROMSHORT (plwd->uDataRows);

      case ITIWND_NUMCOLS:
         return MRFROMSHORT (plwd->uDataCols);

      case ITIWND_LABELCOLS:
         return MRFROMSHORT (plwd->uLabelCols);

      case ITIWND_ACTIVE:
         return MRFROMSHORT (plwd->uDataRows ? plwd->uActive : 0xFFFF);

      case ITIWND_SCROLLTYPE:
         return MRFROMSHORT (plwd->uScrollType);

      case ITIWND_COLOR:
         return 0;                      /*--- not done ---*/

      case ITIWND_LABELEDT:
         if (i >= plwd->uLabelCols)
            return (MRESULT) NULL;
         return (MRESULT) &((plwd->pedtLabel)[i]);

      case ITIWND_DATAEDT:
         if (i >= plwd->uDataCols)
            return (MRESULT) NULL;
         return (MRESULT) &((plwd->pedtData)[i]);

      case ITIWND_LABELHIDDEN:
         i = i & 0x7FFF;
         if (i >= plwd->uLabelCols)
            return (MRESULT) 0;
         return (MRESULT) ((plwd->pedtLabel)[i].lFlags & ITIWND_HIDE);

      case ITIWND_DATAHIDDEN:
         i = LWColIdToRelCol (hwnd, i);
         if (i >= plwd->uDataCols)
            return (MRESULT) 0;
         return (MRESULT) ((plwd->pedtData)[i].lFlags & ITIWND_HIDE);

      case ITIWND_LABEL:
         if (i >= plwd->uLabelCols)
            return (MRESULT) NULL;
         return (MRESULT) ((plwd->ppszLabels)[i]);

      case ITIWND_BUFFER:
         return (MRESULT) plwd->hbuf;

      case ITIWND_HEAP:
         return (MRESULT) (plwd->hheap);

      case ITIWND_BUTTON:
         return MRFROMSHORT (plwd->uButtonId);

      case ITIWND_TXTPROC:
         return (MRESULT) (plwd->pfnTxt);

      case ITIWND_INITDRAWPROC:
         return (MRESULT) (plwd->pfnInitDraw);

      case ITIWND_DRAWPROC:
         return (MRESULT) (plwd->pfnDraw);

      case ITIWND_ID:
         return (MRESULT) WinQueryWindowUShort (hwnd, QWS_ID);

      case ITIWND_TYPE:
         return (MRESULT) ITI_LIST;

      case ITIWND_OWNERWND:
         {
         HWND     hwndParent;

         hwndParent = (HWND) WinQueryWindow (hwnd, QW_OWNER, 0);
         if (!i)
            return hwndParent;

         return QW (hwndParent, ITIWND_OWNERWND, i-1, 0, 0);
         }

      case ITIWND_ADDID:
      case ITIWND_CHANGEID:
         {
         HWND  hwndFrame;

         hwndFrame = WinQueryWindow (hwnd, QW_PARENT, FALSE);
         uId = WinQueryWindowUShort (hwnd, QWS_ID);
         mp1 = MPFROM2SHORT (SHORT1FROMMP (mp1), uId);
         return WinSendMsg (hwndFrame, WM_ITIWNDQUERY, mp1, mp2);
         }
      case ITIWND_DATA:
         {
         USHORT   uRow, uCol;
         CHAR     szTmp [256];

         if (plwd->hbuf == NULL)
            return NULL;

         uRow = SHORT1FROMMP (mp2);
         uCol = SHORT2FROMMP (mp2);

         if (uRow >= plwd->uDataRows)
            return NULL;
         if (!ItiDbColExists(plwd->hbuf, uCol))
            return NULL;

         ItiDbGetBufferRowCol (plwd->hbuf, uRow, uCol, szTmp);

         return MRFROMP (ItiStrDup (plwd->hheap, szTmp));
         }

      case ITIWND_FLAGS:
         return (MRESULT) (plwd->uOptions);

      default:              
         okLwErr ("unknown WM_ITIWNDQUERY Cmd:", SHORT1FROMMP (mp1));
         return 0;
      }
   return 0;
   }





MRESULT ProcessListSet (HWND hwnd, MPARAM mp1, MPARAM mp2)
   {
   PLWDAT   plwd;
   MRESULT  Err = MRFROMSHORT (1);
   USHORT   i;

   plwd = LWGetPLWDAT (hwnd, FALSE);
   i = SHORT2FROMMP (mp1);

   switch (SHORT1FROMMP (mp1))
      {
      case ITIWND_NUMROWS:
         LWSetRowCount (hwnd, (USHORT) SHORT1FROMMP (mp2));
         UpdateScrollBars (hwnd, LWSB_UPDATETHUMBSIZES, 0, 0);
         UpdateScrollBars (hwnd, LWSB_UPDATESCROLLRANGES, 0, 0);

         if (i)
            WinInvalidateRect (hwnd, NULL, TRUE);
         break;

      case ITIWND_ACTIVE:
         {
         HWND     hwndClient;
         USHORT   uVSBID, uOld; //, uScroll;


         /* --- a better solution would be to use the
                SetNewActive procedure in lwselect.c
                as it would ensure the newly activated row
                became visible. Unfortunately, as of now i
                don't know which of the (as many as) 4 clients
                is currently active. I will have to review this
                when adding keyboard support later. CLF
            --- */

         if (SHORT1FROMMP (mp2) != -1 &&
             (USHORT) SHORT1FROMMP (mp2) >= plwd->uDataRows)
            return Err;

         uOld = plwd->uActive;
         plwd->uActive = (USHORT) SHORT1FROMMP (mp2);
         /*--- conditionally update window---*/
         hwndClient = WinWindowFromID (hwnd, LWID_CLIENT[1][0]);
         uVSBID     = LWID_VSB[1];

         SetNewActive (hwndClient, uVSBID, uOld, plwd->uActive);


//         if ((uScroll = RowVisible (hwndClient, plwd->uActive)) != 0xFFFF)
//            DoVScroll (hwnd, MPFROMSHORT (uVSBID),
//                        MPFROM2SHORT (uScroll, SB_LWTRACK));
//
////         InvalidateRow (hwndFrame, uNew);
//// NO! We do not want never ending message loops! -clf
////         LWSendActiveMsg (hwnd, 0);
//
//         WinInvalidateRect (hwnd, NULL, TRUE);
         break;
         }

      case ITIWND_SCROLLTYPE:
         if (SHORT1FROMMP (mp2) != SB_SLIDERPOSITION &&
             SHORT1FROMMP (mp2) != SB_SLIDERTRACK)
            return Err;
         plwd->uScrollType = SHORT1FROMMP (mp2);
         break;

      case ITIWND_COLOR:
         LWSetColor (hwnd, SHORT2FROMMP (mp1), (long) mp2);
         break;

      case ITIWND_LABELEDT:
         if (i >= plwd->uLabelCols)
            return Err;
         (plwd->pedtLabel)[i] = *((PEDT)mp2);
         break;

      case ITIWND_DATAEDT:
         if (i >= plwd->uDataCols)
            return Err;
         (plwd->pedtData)[i] = *((PEDT)mp2);
         break;

      case ITIWND_LABELHIDDEN:
         i = i & 0x7FFF;
         if (i >= plwd->uLabelCols)
            return Err;

         if (mp2)
            plwd->pedtLabel [i].lFlags |= ITIWND_HIDE;
         else
            plwd->pedtLabel [i].lFlags &= ~ITIWND_HIDE;
         break;

      case ITIWND_DATAHIDDEN:
         i = LWColIdToRelCol (hwnd, i);
         if (i >= plwd->uDataCols)
            return Err;
         if (mp2)
            plwd->pedtData [i].lFlags |= ITIWND_HIDE;
         else
            plwd->pedtData [i].lFlags &= ~ITIWND_HIDE;
         break;

      case ITIWND_LABEL:
         if (i >= plwd->uLabelCols)
            return Err;
         if (!mp2)
            return Err;

         ItiMemFree (plwd->hheap, (plwd->ppszLabels)[i]);
         (plwd->ppszLabels)[i] = ItiStrDup (plwd->hheap, (PSZ) mp2);
         break;

      case ITIWND_BUFFER:
         plwd->hbuf = (HBUF) mp2;
         break;

      case ITIWND_BUTTON:
         plwd->uButtonId = SHORT1FROMMP (mp2);
         break;

      case ITIWND_TXTPROC:
         plwd->pfnTxt = (PFNTXT) mp2;
         break;

      case ITIWND_INITDRAWPROC:
         plwd->pfnInitDraw  = (PFNINITDRAW) mp2;
         break;

      case ITIWND_DRAWPROC:
         plwd->pfnDraw = (PFNDRAW) mp2;
         break;

      case ITIWND_REDRAW:
         WinInvalidateRect (hwnd, NULL, TRUE);
         break;

      case ITIWND_MAXSIZE:
         {
         RECTL rcl, rclAdj = { 0,0,0,0 };

         rclAdj = (mp2 == NULL ? rclAdj : *((PRECTL) mp2));
         WinQueryWindowRect (WinQueryWindow (hwnd, QW_PARENT, FALSE), &rcl);
         WinSetWindowPos (hwnd, HWND_TOP,
                          (SHORT) rclAdj.xLeft,
                          (SHORT) rclAdj.yBottom,
                          (SHORT) (rcl.xRight - rclAdj.xRight),
                          (SHORT) (rcl.yTop - rclAdj.yTop),
                          SWP_MOVE | SWP_SIZE);
         break;
         }

      case ITIWND_VIEWROW:
         break;

      case ITIWND_SCROLL:
         break;

      case ITIWND_ROWCHANGED:
         break;

      case ITIWND_ROWINSERTED:
         break;

      case ITIWND_ROWDELETED:
         break;


      case ITIWND_SELECTION:
         {
         USHORT uStart, uEnd;
         HWND   hwndClient;

         /*--- i=0 deselect, i=1 select ---*/
         uStart = min ((USHORT)SHORT1FROMMP (mp2), plwd->uDataRows);
         uEnd   = min ((USHORT)SHORT2FROMMP (mp2), plwd->uDataRows);

         hwndClient = WinWindowFromID (hwnd, LWID_CLIENT[1][0]);
         LWSelectRange (hwndClient, uStart, uEnd, i);
         }


      }
   return MRFROMSHORT (0);
   }










/* this is the window procedure for the LW frame */

WNDPROC LWFrameProc (HWND hwnd, USHORT umsg, MPARAM mp1, MPARAM mp2)
   {
   PLWDAT  plwd;

   switch (umsg)
      {
      case WM_CREATE:
         {
         HHEAP hheap;

         /* init met structure */
         if ((hheap = ItiMemCreateHeap (LW_INITIALHEAPSIZE)) == NULL)
            okLwErr ("Unable To Create Heap for new list window!", 0);
         if ((plwd = (PLWDAT) ItiMemAlloc (hheap, sizeof (LWDAT), 0)) == NULL)
            okLwErr ("Unable to alloc mem for new LW", 0);
         if (!WinSetWindowPtr (hwnd, QWP_LWDAT, plwd))
            okLwErr ("Unable to set window ptr", 0);

         NullPLWD (hwnd);
         plwd->hheap = hheap;

         plwd->Init = FALSE;  /*--- redundant but safe ---*/
         return FALSE;
         }

      case WM_SIZE:
         SizeAllChildWindows (hwnd, mp2);
         return 0;
         break;

      case WM_RESET:
         {
         RECTL rcl;

         WinQueryWindowRect (hwnd, &rcl);
         mp2 = MPFROM2SHORT (rcl.xRight, rcl.yTop);
         SizeAllChildWindows (hwnd, mp2);
         WinInvalidateRegion (hwnd, NULL, TRUE);
         return 0; 
         break;
         }

      case WM_SPLITMOVE:
         {
         SWP swp;

         WinQueryWindowPos (hwnd, &swp);
         SizeAllChildWindows (hwnd, MPFROM2SHORT (swp.cx,swp.cy));
         break;
         }

      case WM_VSCROLL:
         DoVScroll (hwnd, mp1, mp2);
         return 0;

      case WM_HSCROLL:
         DoHScroll (hwnd, mp1, mp2);
         break;

      case WM_PAINT:
         PaintFrameWnd (hwnd);
         return 0;


      case WM_ITIWNDQUERY:
         return ProcessListQuery (hwnd, mp1, mp2);

      case WM_ITIWNDSET:
         return ProcessListSet (hwnd, mp1, mp2);




   /* --- The next 4 messages come from the database module --- */


      case WM_ITIDBGRAY:          /*--- ignore this message ---*/
         {
         return 0;
         break;
         }
        
      case WM_ITIDBCHANGED:       /*--- buffer starting ---*/
         {
         plwd = LWGetPLWDAT (hwnd, FALSE);
         if (PVOIDFROMMP (mp2) != plwd->hbuf)
            return 0;

         /*--- update numrows if not already zero ---*/
//         if (plwd->uDataRows)
//            {
//            plwd->uActive = 0xFFFF;
//            SW (hwnd, ITIWND_NUMROWS, 1, 0, 0);
//            }
         
         /*--- make window gray if it is not already ---*/
         if (plwd->lDataColor != WORKING_COLOR)
            {
            SWL (hwnd, ITIWND_COLOR, LWC_CLIENT, WORKING_COLOR);
            WinInvalidateChild (hwnd, LWC_CLIENT);
            }
         return 0;
         }


      case WM_ITIDBAPPENDEDROWS:  /*--- more data coming in ---*/
      case WM_ITIDBBUFFERDONE:    /*--- buffer finished ---*/
         {
         USHORT uOldRows;

         plwd = LWGetPLWDAT (hwnd, FALSE);
         if (PVOIDFROMMP (mp2) != plwd->hbuf)
            return 0;

         uOldRows = plwd->uDataRows;

//         SW (hwnd, ITIWND_NUMROWS, 0, SHORT2FROMMP (mp1), 0);
//         if (SHORT2FROMMP (mp1) < plwd->uActive)
//            SW (hwnd, ITIWND_ACTIVE, 0, SHORT2FROMMP (mp1) - 1, 0);


         if (umsg == WM_ITIDBBUFFERDONE)
            {
            USHORT uWinId;

            SW (hwnd, ITIWND_NUMROWS, 0, SHORT2FROMMP (mp1), 0);
            if (SHORT2FROMMP (mp1) <= plwd->uActive)
               SW (hwnd, ITIWND_ACTIVE, 0, SHORT2FROMMP (mp1) - 1, 0);

            SWL (hwnd, ITIWND_COLOR, LWC_CLIENT, DONE_COLOR);
            uWinId = WinQueryWindowUShort (hwnd, QWS_ID);
            WinSendParentMsg (hwnd, WM_CHILDQUERYDONE, MPFROMSHORT(uWinId), mp2);
            WinInvalidateRect (hwnd, NULL, TRUE);
            }
         else
            {
            UpdateWindowsIfNeeded (hwnd, uOldRows, plwd->uDataRows);
            }
         return 0;
         }


      case WM_ITIDBDELETE:        /*--- buffer going away ---*/
         {
         plwd = LWGetPLWDAT (hwnd, FALSE);
         if (PVOIDFROMMP (mp2) != plwd->hbuf)
            return 0;

         SWL (hwnd, ITIWND_COLOR, LWC_CLIENT, WORKING_COLOR);
         SW (hwnd, ITIWND_NUMROWS, 0, 0, 0);
         plwd->hbuf = NULL;

         return 0;
         }



      case WM_CHANGEQUERY:
         plwd = LWGetPLWDAT (hwnd, FALSE);
         if (!mp1 && !mp2)
            {
            DosBeep (1666,35);
            if (plwd->hbuf != NULL)
               ItiDbDeleteBuffer (plwd->hbuf, hwnd);

            SWL (hwnd, ITIWND_COLOR, LWC_CLIENT, WORKING_COLOR);
            SW (hwnd, ITIWND_NUMROWS, 0, 0, 0);
            return 0;
            }

         if (mp1 == (PVOID)0xFFFFFFFF && mp1 == mp2)
            mp2 = (PSZ) ItiDbQueryBuffer (plwd->hbuf, ITIDB_QUERY, 0);

         if (plwd->hbuf != NULL)
            ItiDbDeleteBuffer (plwd->hbuf, hwnd);

         plwd->hbuf = NULL;
         SW (hwnd, ITIWND_NUMROWS, 0, 0, 0);

         /* --- FALL THROUGH --- */
         /* --- CODE BY SuperKludge (tm) --- */


      case WM_INITQUERY:
         {
         USHORT   uWinId, uNumRows;
         char     szQuery[BIGSTRLEN];
         char     szStr[BIGSTRLEN];
         HMODULE  hmodWnd;
         HBUF     hbufParent, hbufStatic;
         USHORT   uRowParent, uRet, uOwnerId;
         HWND     hwndClient, hwndFrame, hwndStatic, hwndOwner;

         WinUpdateWindow (hwnd);
         plwd        = LWGetPLWDAT (hwnd, FALSE);
         uWinId      = WinQueryWindowUShort (hwnd, QWS_ID);
         hbufParent  = (HBUF) PVOIDFROMMP  (mp1);
         uRowParent  = SHORT1FROMMP (mp2);

         hwndClient  = WinQueryWindow (hwnd, QW_OWNER, 0);

         /*--- check to see if we are actually in a dialog box ---*/
         if (FS_DLGBORDER & WinQueryWindowULong (hwndClient, QWL_STYLE))
            hwndFrame = hwndClient;
         else
            hwndFrame = WinQueryWindow (hwndClient, QW_PARENT, 0);

         hwndOwner = (HWND)QW (hwndFrame, ITIWND_OWNERWND, 0, 0, 0);
         uOwnerId = WinQueryWindowUShort (hwndOwner, QWS_ID);

         if (ItiMbQueryWindowType (uOwnerId) == ITI_LIST)
            hwndStatic = WinQueryWindow (hwndOwner, QW_PARENT, 0);
         else
            hwndStatic = hwndOwner;

////unchanged       hwndStatic  = (HWND)QW (hwndFrame, ITIWND_OWNERWND, 0, 0, 0);
//         hwndStatic  = WinQueryWindow (hwndFrame,  QW_OWNER,  0);
//
         hbufStatic  = (HBUF) QW (hwndStatic, ITIWND_BUFFER, 0, 0, 0);

         ItiMbQueryHMOD (uWinId, &hmodWnd);
         if (hbufParent == (HBUF)0xFFFFFFFF)
            {
            strcpy (szQuery, (PSZ) mp2);
            uWinId = WinQueryWindowUShort (hwnd, QWS_ID);
            }
         else
            {
            if (ItiMbQueryQueryText (hmodWnd, ITIRID_WND, uWinId,
                    szStr, sizeof szStr) ||  *szStr == '\0')
               return 0;

            if (uRet = ItiWndReplaceAllParams (szQuery, szStr, NULL, NULL, 
                                               hbufParent, hbufStatic,
                                               uRowParent, FALSE, sizeof szQuery))
               {
               if (uRet == ITIERR_NOCOL)
                  okLwErr ("Parent does not have requested column for list query ", uWinId);
               okLwErr (szQuery, 0);
               return 0;
               }
            }

         plwd->hbuf = ItiDbCreateBuffer (hwnd, uWinId, ITI_LIST,
                                         szQuery, &uNumRows);

//         /*---gray while buffer is filling---*/
//         SWL (hwnd, ITIWND_COLOR, LWC_CLIENT, WORKING_COLOR);
//         SW (hwnd, ITIWND_NUMROWS, 1, uNumRows, 0);
         return 0;
         }


      case WM_COMMAND:
         {
         switch (SHORT1FROMMP(mp1))
            {
            case 0x000D: /*--- Q Button ---*/
               {
//               HWND hwndStatic;
               PLWDAT plwd;
//
//               hwndStatic = WinQueryWindow (hwnd, QW_PARENT, 0);
               plwd = LWGetPLWDAT (hwnd, FALSE);
//               WinSendMsg (hwndStatic, WM_ITILWQBUTTON,
//                           MPFROM2SHORT (plwd->uButtonId,
//                                       WinQueryWindowUShort (hwnd, QWS_ID)),
//                           MPFROMP (hwnd));


//               if (pglobals->usUserType >= USER_DEVELOPER)
//                  {
                  ItiDbQButton (hwnd, plwd->hbuf);
                  WinInvalidateRect (hwnd, NULL, TRUE);
//                  }
//               else
//                  {
//                  /*--- Simulate F1 Key ---*/
//                  WinShowWindow (pglobals->hwndHelp, TRUE);
//                  }
//
               return 0;
               }
            }
         }

      case LM_QUERYTOPINDEX:
         {
         USHORT uPos, uMax, uScrollId = 8;
         HWND   hwndScroll;

         if ((hwndScroll = WinWindowFromID (hwnd, uScrollId)) == NULL)
            return (MRESULT) okLwErr ("Unable to obtain window from Id. Id:",
                                      uScrollId);

         GetScrollInfo (hwndScroll, &uPos, &uMax);

         return (MRESULT) uPos;
         }

      case WM_CLOSE:
         WinDestroyWindow (WinQueryWindow (hwnd, QW_PARENT, FALSE));
         break;

      case WM_DESTROY:
         FreePLWDAT (hwnd);
         ItiMemDestroyHeap (LWGetPLWDAT (hwnd, FALSE)->hheap);
         break;
      }
   return WinDefWindowProc (hwnd, umsg, mp1, mp2);
   }

 
