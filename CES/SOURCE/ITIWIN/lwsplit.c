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
 * lwsplit.c                                                            *
 *                                                                      *
 * Copyright (C) 1990-1992 Info Tech, Inc.                              *
 *                                                                      *
 *  This file contains the code to move split windows                   *
 *    and contains the split window proc                                *
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
#include "..\include\itidbase.h"
#include "..\include\itierror.h"
#include "..\include\itiglob.h"
#include "initdll.h"
#include "winmain.h"
#include "lwmain.h"
#include "lwsplit.h"
#include "lwsize.h"
#include "lwutil.h"
#include "initdll.h"
#include "winutil.h"
#include <stdlib.h>


#define DEFAULTPOS  0


HPOINTER hHptr;
HPOINTER hVptr;
HPOINTER hHVptr;






USHORT okSplErr (PSZ psz)
   {
   ItiErrWriteDebugMessage (psz);
   WinMessageBox (pglobals->hwndDesktop, pglobals->hwndAppFrame, psz,
                   "List Win Split Error", 0, MB_OK | MB_APPLMODAL);
   return 1;
   }


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

void _SetOdd (HWND hwnd, BOOL bSet)
   {
   PLWDAT  plwd;
   USHORT  uID;

   if ((plwd = LWGetPLWDAT (hwnd, TRUE)) == NULL)
      return;

   uID = WinQueryWindowUShort (hwnd, QWS_ID);
   if (uID == LWID_VSB[1])
      plwd->bV1Odd = bSet;
   else
      plwd->bV0Odd = bSet;
   }


USHORT _GetOdd (HWND hwnd)
   {
   PLWDAT  plwd;
   USHORT uID;

   if ((plwd = LWGetPLWDAT (hwnd, TRUE)) == NULL)
      return 0;

   uID = WinQueryWindowUShort (hwnd, QWS_ID);
   if (uID == LWID_VSB[1])
      return !!plwd->bV1Odd;
   return !!plwd->bV0Odd;
   }


/*
 * returns true if this is a vertical scroll bar and
 * uRows is > 32K
 */
BOOL _ScrollDoubled (HWND hwnd)
   {
   USHORT uID, uNumRows;

   /*--- make sure the window is a vertical scroll bar ---*/
   uID = WinQueryWindowUShort (hwnd, QWS_ID);
   if (uID != LWID_VSB[0] && uID != LWID_VSB[1])
      return FALSE;

   uNumRows = LWQuery (hwnd, LWM_NumRows, TRUE);
   return (uNumRows > 32767);
// return (uNumRows > 100);
   }




USHORT _GetScrollPos  (HWND hwnd, USHORT *puMax, USHORT *puPos)
   {   
   USHORT uPos, uMax;
   BOOL bScrollDoubled;

   uPos = SHORT1FROMMP (WinSendMsg (hwnd, SBM_QUERYPOS,   0L, 0L));
   uMax = SHORT2FROMMP (WinSendMsg (hwnd, SBM_QUERYRANGE, 0L, 0L));
   bScrollDoubled = _ScrollDoubled (hwnd);

   if (puPos)
      {
      *puPos = (bScrollDoubled ? uPos * 2 : uPos);
      *puPos +=(bScrollDoubled ? _GetOdd (hwnd) : 0);
      }
   if (puMax)
      *puMax = (bScrollDoubled ? uMax * 2 : uMax);
   return (bScrollDoubled ? uPos * 2 : uPos);
   }


BOOL _SetScrollThumbSize (HWND hwnd, USHORT uMax, USHORT uVis)
   {
   if (_ScrollDoubled (hwnd))
      {
      uMax /= 2;
      uVis /= 2;
      }
   WinSendMsg (hwnd, SBM_SETTHUMBSIZE, MPFROM2SHORT (uVis, uMax), 0L);
   return TRUE;
   }

BOOL _SetScrollRange (HWND hwnd, USHORT uMax, USHORT uPos)
   {
   if (_ScrollDoubled (hwnd))
      {
      _SetOdd (hwnd, uPos % 2);
      uPos /= 2;
      uMax /= 2;
      }
      
   WinSendMsg (hwnd, SBM_SETSCROLLBAR, MPFROMSHORT (uPos), MPFROM2SHORT (0, uMax));
   return TRUE;
   }


BOOL _SetScrollPos (HWND hwnd, USHORT uPos)
   {
   if (_ScrollDoubled (hwnd))
      {
      _SetOdd (hwnd, uPos % 2);
      uPos /= 2;
      }

   WinSendMsg (hwnd, SBM_SETPOS, MPFROMSHORT (uPos), 0L);
   return TRUE;
   }




/**********************************************************************/
/**********************************************************************/
/**********************************************************************/


/***********************************************scrollbars*************/
/* valid uses:
 *  hwnd, LWSB_SETTHUMBPOS,       childID, uNewPos
 *  hwnd, LWSB_UPDATETHUMBSIZES,   0, 0
 *  hwnd, LWSB_UPDATESCROLLRANGES, 0, 0
 *
 *  the childID is the the actual scroll bar to change
 */
int UpdateScrollBars   (HWND hwndFrame,
                        USHORT ucmd,
                        USHORT uChildID,
                        USHORT u1)
   {
   HWND   hwndChild, hwndClient;

   USHORT fOptions, uNumRows, uMax, uVis, uPos, x;
   USHORT uXRowSize, uXWinSize, uXScrInc,
          uYRowSize, uYWinSize, uYScrInc;
   USHORT uHWindows, uVWindows;

   uHWindows = ((fOptions & LWS_HSPLIT) == LWS_HSPLIT ? 2 : 1);
   uVWindows = ((fOptions & LWS_VSPLIT) == LWS_VSPLIT ? 2 : 1);
   uNumRows  = LWQuery (hwndFrame, LWM_NumRows,    FALSE);
   uXRowSize = LWQuery (hwndFrame, LWM_XRowSize,   FALSE);
   uYRowSize = LWQuery (hwndFrame, LWM_YRowSize,   FALSE);
   uXScrInc  = LWQuery (hwndFrame, LWM_XScrollInc, FALSE);
   uYScrInc  = LWQuery (hwndFrame, LWM_YScrollInc, FALSE);

   switch (ucmd)
      {
      case LWSB_SETTHUMBPOS:
         hwndChild = WinWindowFromID (hwndFrame, uChildID);
         _SetScrollPos (hwndChild, u1);
         return 0;

      case LWSB_UPDATETHUMBSIZES:
         {
         fOptions = LWQuery (hwndFrame, LWM_Options, FALSE);

         if (fOptions & LWS_HSCROLL)
            {
            for (x = 0; x < uVWindows; x++)
               {
               hwndChild = WinWindowFromID (hwndFrame, LWID_HSB[x]);
               uXWinSize = LWQuery (hwndChild, LWM_XWindowSize, FALSE);
               uMax = uXRowSize / uXScrInc + !!(uXRowSize % uXScrInc);
               uVis = max (1, uXWinSize / uXScrInc);
               _SetScrollThumbSize (hwndChild, uMax, uVis);
               }
            }
         if (fOptions & LWS_VSCROLL)
            {
            for (x = 0; x < uHWindows; x++)
               {
               hwndChild = WinWindowFromID (hwndFrame, LWID_VSB[1-x]);
               hwndClient= WinWindowFromID (hwndFrame, LWID_CLIENT[1-x][0]);
               uYWinSize = LWQuery (hwndClient, LWM_YWindowSize, TRUE);
               uMax = (USHORT)(((LONG) uNumRows * (LONG) uYRowSize) / (LONG) uYScrInc);
               uVis = uYWinSize / uYScrInc;
               _SetScrollThumbSize (hwndChild, uMax, uVis);
               }
            }
         return 0;
         }

      case LWSB_UPDATESCROLLRANGES:
         {
         fOptions = LWQuery (hwndFrame, LWM_Options, FALSE);

         if (fOptions & LWS_HSCROLL)
            {
            for (x = 0; x < uVWindows; x++)
               {
               hwndChild = WinWindowFromID (hwndFrame, LWID_HSB[x]);
               uPos = _GetScrollPos (hwndChild, NULL, NULL);

               uXWinSize = LWQuery (hwndChild, LWM_XWindowSize, TRUE);
               uMax = (USHORT) max (0, ((SHORT)uXRowSize-(SHORT)uXWinSize)/(SHORT)uXScrInc + !!(((SHORT)uXRowSize- (SHORT)uXWinSize) % (SHORT)uXScrInc));
               uVis = max (1, uXWinSize/uXScrInc);

               if (uPos > uMax)
                  {
                  MPARAM  mp2;
                  mp2 = MPFROM2SHORT ((SHORT)uMax-(SHORT)uPos, 101 /*LWSB_CUSTOMSCROLL*/);
                  DoHScroll (hwndFrame, MPFROM2SHORT (LWID_HSB[x], 0), mp2);
                  uPos = uMax;
                  }
               _SetScrollRange (hwndChild, uMax, uPos);
               }
            }

         if (fOptions & LWS_VSCROLL)
            {
            for (x = 0; x < uHWindows; x++)
               {
               hwndChild  = WinWindowFromID (hwndFrame, LWID_VSB[1-x]);
               hwndClient = WinWindowFromID (hwndFrame, LWID_CLIENT[1-x][0]);

               uPos = _GetScrollPos (hwndChild, NULL, NULL);
               uYWinSize = LWQuery (hwndClient, LWM_YWindowSize, TRUE);

               uMax = (USHORT)(((LONG)uYRowSize * (LONG)uNumRows) /
                               (LONG)uYScrInc);
               uVis = uYWinSize / uYScrInc;
               uMax = (USHORT) max (0L, ((LONG) uMax - (LONG) uVis));
               if (uPos > uMax)
                  {
                  MPARAM  mp2;
                  mp2 = MPFROM2SHORT ((int)((LONG)uMax-(LONG)uPos), 101 /*LWSB_CUSTOMSCROLL*/);
                  DoVScroll (hwndFrame, MPFROM2SHORT (LWID_VSB[1-x], 0), mp2);
                  }
               uPos = min (uPos, uMax);
               _SetScrollRange (hwndChild, uMax, uPos);
               }
            }
         return 0;
         }

      default:
         return 1;
      }
   }






/* this proc uses a handle to a scroll bar and returns its
 * pos, and max pos
 * nulls are ok, uPos is returned
 */

int GetScrollInfo (HWND hwndScroll,
                   USHORT *puScrollPos,
                   USHORT *puScrollMax)
   {
   _GetScrollPos (hwndScroll, puScrollMax, puScrollPos);
   return 0;
   }



/* this procedure gets a child win of a client window or a label window
   anb returns the scroll bar positions.
*/
int GetScrollPos (HWND hwndChild, POINTS *pptsScrollPos, POINTS *pptsMax)
   {
   USHORT uHpos, uVpos, uHMax, uVMax, x, y, uChildID;
   HWND   hwndFrame;
   POINTS   ptsDummy;

   hwndFrame = WinQueryWindow (hwndChild, QW_PARENT, FALSE);
   uChildID  = WinQueryWindowUShort (hwndChild, QWS_ID);
   
   for (x = 0; x < 2; x++)
      {
      if (uChildID == LWID_LABEL[x])
         {
         GetScrollInfo (WinWindowFromID (hwndFrame, LWID_HSB[x]), &uHpos, &uHMax);
         return WndAssignPts (pptsScrollPos, &ptsDummy, uHpos, 0, uHMax, 0);
         }
   
      for (y = 0; y < 2; y++)
         if (uChildID == LWID_CLIENT[y][x])
            {
            GetScrollInfo (WinWindowFromID (hwndFrame, LWID_HSB[x]), &uHpos, &uHMax);
            GetScrollInfo (WinWindowFromID (hwndFrame, LWID_VSB[y]), &uVpos, &uVMax);
            return WndAssignPts (pptsScrollPos, pptsMax, uHpos, uVpos, uHMax, uVMax);
            }
      }
   return 1;
   }


/**************************************************************************/

/* Mouse Ptr Resources are packaged
 * in the window dll.
 */

int InitMousePointers (void)
   {
   if ((hHptr  = WinLoadPointer (HWND_DESKTOP, hmodModule, PTR_HORZ)) == NULL)
      return okSplErr ("Unable To Load PTR_HORZ ptr");
   if ((hVptr  = WinLoadPointer (HWND_DESKTOP, hmodModule, PTR_VERT)) == NULL)
      return okSplErr ("Unable To Load PTR_VERT ptr");
   if ((hHVptr = WinLoadPointer (HWND_DESKTOP, hmodModule, PTR_HV)) == NULL)
      return okSplErr ("Unable To Load PTR_HV ptr");
   return 0;
   }


int DeInitMousePointers (void)
   {
   WinDestroyPointer (hHptr);
   WinDestroyPointer (hVptr);
   WinDestroyPointer (hHVptr);
   return 0;
   }



int InvertRect (HPS hps, PRECTL prclV, PRECTL prclH,
                         BOOL bVSplit, BOOL bHSplit)
   {
   if (bVSplit)
      WinInvertRect (hps, prclV);
   if (bHSplit)
      WinInvertRect (hps, prclH);
   return 0;
   }



int  SetSplitLocation (HWND hwndChild, SHORT iPos, SHORT bLock)
   {
   WinSetWindowUShort (hwndChild, QW_POS,      iPos);
   WinSetWindowUShort (hwndChild, QW_HOMELOCK, bLock);
   return 0;
   }

int  GetSplitLocation (HWND hwndChild, SHORT *iPos, SHORT *bLock)
   {
   *iPos  = WinQueryWindowUShort (hwndChild, QW_POS);
   *bLock = WinQueryWindowUShort (hwndChild, QW_HOMELOCK);
   return *iPos;
   }



int  SetSplitLocation2 (HWND hwndChild, SHORT iPos)
   {
   /* if it goes from hedge->hedge, lock, dont change preferred location */
   /* if it goes from ~hedge->hedge then lock and viseversa a update loc*/
   USHORT uChildID, bOldLock, fOptions;
   SHORT  iNewEdge, iOldPos, iLockVal;
   HWND   hwndFrame;


   hwndFrame = WinQueryWindow (hwndChild, QW_PARENT, FALSE);
   fOptions = LWQuery (hwndFrame, LWM_Options, FALSE);
   iLockVal = (fOptions & LWS_SPLITATTOP ? -1 : 1);
   uChildID  = WinQueryWindowUShort (hwndChild, QWS_ID);

   GetSplitLocation (hwndChild, &iOldPos, &bOldLock);
   iNewEdge = SplitBarAtEdge (hwndFrame, uChildID, iPos);

   WinSetWindowUShort (hwndChild, QW_HOMELOCK, iNewEdge == iLockVal);

   if (!(bOldLock && iNewEdge == iLockVal))
         WinSetWindowUShort (hwndChild, QW_POS, iPos);
   return 0;
   }





BOOL OtherSplitClose(HWND hwndSplit, USHORT uChildID, MPARAM mp1)
   {
   USHORT   fOptions;
   SHORT    XPos, YPos;
   SWP      swpH, swpV;
   HWND     hwndFrame;

   hwndFrame = WinQueryWindow (hwndSplit, QW_PARENT, FALSE);
   fOptions = LWQuery (hwndFrame, LWM_Options, FALSE);

   if ((fOptions & LWS_HSPLIT)!= LWS_HSPLIT ||
       (fOptions & LWS_VSPLIT)!= LWS_VSPLIT)
      return FALSE;

   WinQueryWindowPos (WinWindowFromID (hwndFrame, LWID_HSPLIT), &swpH);
   WinQueryWindowPos (WinWindowFromID (hwndFrame, LWID_VSPLIT), &swpV);
   YPos = (SHORT)SHORT2FROMMP (mp1) + swpV.y;
   XPos = (SHORT)SHORT1FROMMP (mp1) + swpH.x;

   if (uChildID == LWID_VSPLIT)
      return (YPos + LW_EXTRACENTER >= swpH.y &&
              YPos <= swpH.y + swpH.cy + LW_EXTRACENTER);
   else /* (uChildID == LWID_HSPLIT) */
      return (XPos + LW_EXTRACENTER >= swpV.x &&
              XPos <= swpV.x + swpV.cx + LW_EXTRACENTER);
   }
   







int DoSplitMove (HWND hwndSplit, MPARAM mp1)
   {
   USHORT   uSplitID, uYLabel, fOptions;
   HWND     hwndFrame, hVSplitWnd, hHSplitWnd;
   SWP      swpH, swpV;
   QMSG     qmsg;
   HPS      hps;
   RECTL    rclV, rclH, rclClient;
   LONG     lxMin, lxMax, lxMouse;
   LONG     lyMin, lyMax, lyMouse;
   BOOL     bHSplit, bVSplit, bVMain;
   SHORT    xExtraOffset, yExtraOffset;

   hwndFrame = WinQueryWindow (hwndSplit, QW_PARENT, FALSE);
   uSplitID  = WinQueryWindowUShort (hwndSplit, QWS_ID);
   uYLabel   = LWQuery (hwndSplit, LWM_YLabelSize, TRUE);
   fOptions  = LWQuery (hwndFrame, LWM_Options, FALSE);

   LWQueryClientRect (hwndFrame, &rclClient);

   /* lock clients */
   WinLockWindowUpdate (HWND_DESKTOP,
                        WinWindowFromID (hwndFrame, LWID_CLIENT[1][0]));
   if ((fOptions & LWS_VSPLIT) == LWS_VSPLIT)
      WinLockWindowUpdate (HWND_DESKTOP,
                        WinWindowFromID (hwndFrame, LWID_CLIENT[1][1]));

   if ((fOptions & LWS_HSPLIT) == LWS_HSPLIT)
      WinLockWindowUpdate (HWND_DESKTOP,
                        WinWindowFromID (hwndFrame, LWID_CLIENT[0][0]));

   if ((fOptions & LWS_HSPLIT)== LWS_HSPLIT &&
       (fOptions & LWS_VSPLIT)== LWS_VSPLIT)
      WinLockWindowUpdate (HWND_DESKTOP,
                        WinWindowFromID (hwndFrame, LWID_CLIENT[0][1]));

   WinSetCapture (HWND_DESKTOP, hwndSplit);


   bHSplit = !(bVSplit = (bVMain = (uSplitID == LWID_VSPLIT)));
   if (bVMain)
      bHSplit = OtherSplitClose (hwndSplit, uSplitID, mp1);
   else
      bVSplit = OtherSplitClose (hwndSplit, uSplitID, mp1);

   if (bVSplit)
      {
      hVSplitWnd = WinWindowFromID (hwndFrame, LWID_VSPLIT);
      WinQueryWindowPos (hVSplitWnd, &swpV);
      xExtraOffset = (SHORT)SHORT1FROMMP (mp1);
      WndAssignRcl (&rclV, swpV.x, rclClient.yBottom,
                 swpV.x + swpV.cx, rclClient.yTop + uYLabel);
      }
   if (bHSplit)
      {
      hHSplitWnd = WinWindowFromID (hwndFrame, LWID_HSPLIT);
      WinQueryWindowPos (hHSplitWnd, &swpH);
      yExtraOffset = (SHORT)SHORT2FROMMP (mp1);
      WndAssignRcl (&rclH, rclClient.xLeft, swpH.y,
                 rclClient.xRight, swpH.y + swpH.cy);
      }

   if (bHSplit && bVSplit && bVMain)
      yExtraOffset += swpV.y - swpH.y;
   if (bHSplit && bVSplit && !bVMain)
      xExtraOffset += swpH.x - swpV.x;

   hps = WinGetClipPS (hwndFrame, NULL,
                       PSF_LOCKWINDOWUPDATE | PSF_PARENTCLIP);

   InvertRect (hps, &rclV, &rclH, bVSplit, bHSplit);

   while (WinGetMsg (NULL, (PQMSG)&qmsg, NULL, 0, 0))
      switch (qmsg.msg)
      {
      case WM_BUTTON1UP:
         InvertRect (hps, &rclV, &rclH, bVSplit, bHSplit);
         WinReleasePS (hps);
         WinLockWindowUpdate (HWND_DESKTOP, NULL); /* unlock windows */
         WinSetCapture (HWND_DESKTOP, NULL);
         if (bVSplit)
            SetSplitLocation2 (hVSplitWnd, (SHORT)rclV.xLeft);
         if (bHSplit)
            SetSplitLocation2 (hHSplitWnd, (SHORT)(rclH.yBottom));
//            SetSplitLocation2 (hHSplitWnd, (SHORT)(rclClient.yTop - rclH.yBottom));
         return 0;

      case WM_MOUSEMOVE:
         InvertRect (hps, &rclV, &rclH, bVSplit, bHSplit);
         if (bVSplit)
            {
            lxMin   = rclClient.xLeft;
            lxMax   = rclClient.xRight - swpV.cx;
            lxMouse = (LONG)((SHORT)SHORT1FROMMP (qmsg.mp1) +
                             (bVMain?swpV.x:swpH.x)         -
                              xExtraOffset);
            rclV.xLeft  = max (lxMin,  min (lxMax, lxMouse));
            rclV.xRight = rclV.xLeft + (LONG)swpV.cx;
            }
         if (bHSplit)
            {
            lyMin    = rclClient.yBottom;
            lyMax    = rclClient.yTop - swpH.cy;
            lyMouse  = (LONG)((SHORT)SHORT2FROMMP (qmsg.mp1) +
                              (bVMain?swpV.y:swpH.y)         -
                              yExtraOffset);
            rclH.yBottom  = max (lyMin,  min (lyMax, lyMouse));
            rclH.yTop     = rclH.yBottom + (LONG)swpH.cy;
            }
         InvertRect (hps, &rclV, &rclH, bVSplit, bHSplit);
         break;

      default:
         WinDispatchMsg (NULL, (PQMSG)&qmsg);
         break;
      }
   WinReleasePS (hps);
   if (bVSplit)
      SetSplitLocation2 (hVSplitWnd, (SHORT)rclV.xLeft);
   if (bHSplit)
      SetSplitLocation2 (hHSplitWnd, (SHORT)(rclH.yBottom));
//      SetSplitLocation2 (hHSplitWnd, (SHORT)(rclClient.yTop - rclH.yBottom));
   WinLockWindowUpdate (HWND_DESKTOP, NULL); /* unlock windows */
   WinSetCapture (HWND_DESKTOP, NULL);
   return 0;
   }





int PaintSplitBar (HWND hwnd)
   {
   HPS    hps;
   RECTL  rclUpdate, rclWnd;
   HWND   hwndOther, hwndFrame;
   USHORT uChildID, uOtherID, fOptions;
   SHORT  iDummy;
   SWP    swp0, swp1;
   long   lBColor, lFColor;
   SHORT  iState;

   hwndFrame = WinQueryWindow (hwnd, QW_PARENT, FALSE);
   uChildID = WinQueryWindowUShort (hwnd, QWS_ID);
   uOtherID = (uChildID == LWID_HSPLIT ? LWID_VSPLIT : LWID_HSPLIT);

   iState = SplitBarUse (hwndFrame, uChildID, &iDummy);
   lBColor = LWColor (hwnd, LWC_SPLITBORDER, TRUE);
   lFColor = LWColor (hwnd, (iState? LWC_SPLITMIN: LWC_SPLITMAX), TRUE);

   hps = WinBeginPaint (hwnd, NULL, &rclUpdate);

   WinQueryWindowRect (hwnd, &rclWnd);

   /*---expand rcl so edges are open---*/
   rclWnd.yBottom -= (uChildID == LWID_VSPLIT ? 1 : 0);
   rclWnd.yTop    += (uChildID == LWID_VSPLIT && !iState ? 1 : 0);
   rclWnd.xLeft   -= (uChildID == LWID_HSPLIT && !iState ? 1 : 0);
   rclWnd.xRight  += (uChildID == LWID_HSPLIT ? 1 : 0);
   
   WinFillRect (hps, &rclUpdate, lFColor);
   WinDrawBorder (hps, &rclWnd, 1, 1, lBColor, lBColor, DB_STANDARD);

   fOptions = LWQuery (hwndFrame, LWM_Options, FALSE);
   if (!(  (fOptions & LWS_HSPLIT)== LWS_HSPLIT   &&
           (fOptions & LWS_VSPLIT)== LWS_VSPLIT  ))
      {
      WinEndPaint (hps);
      return FALSE;
      }

   if (!iState && !SplitBarUse (hwndFrame, uOtherID, &iDummy))
      {
      hwndOther = WinWindowFromID (hwndFrame, uOtherID);

      WinQueryWindowPos (hwnd, &swp0);
      WinQueryWindowPos (hwndOther, &swp1);
      WndAssignRcl (&rclWnd, swp1.x  - swp0.x + 1,
                          swp1.y  - swp0.y + 1,
                          swp1.x  - swp0.x - 1 + swp1.cx,
                          swp1.y  - swp0.y - 1 + swp1.cy);
      WinFillRect (hps, &rclWnd, lFColor);
      }

   WinEndPaint (hps);
   return 0;
   }



WNDPROC LWSplitProc  (HWND hwnd, USHORT umsg, MPARAM mp1, MPARAM mp2)
   {
   switch (umsg)
      {
      case WM_CREATE:
         {
         WinSetWindowUShort (hwnd, QW_POS,      DEFAULTPOS);
         WinSetWindowUShort (hwnd, QW_HOMELOCK, TRUE);
         return 0;
         }

      case WM_PAINT:
         PaintSplitBar (hwnd);
         return 0;

      case WM_BUTTON1DBLCLK:
         {
         USHORT bLock;
         USHORT   uChildID;

         /* possibly move both split bars */
         uChildID = WinQueryWindowUShort (hwnd, QWS_ID);
         if (OtherSplitClose(hwnd, uChildID, mp1) &&
             mp1 != (MPARAM) 0xFFFFFFFFL)
            {
            HWND hwndFrame, hwndChild;

            hwndFrame = WinQueryWindow (hwnd, QW_PARENT, FALSE);
            hwndChild = WinWindowFromID (hwndFrame,
                (uChildID == LWID_HSPLIT ? LWID_VSPLIT : LWID_HSPLIT));
            WinSendMsg (hwndChild, umsg, (MPARAM)0xFFFFFFFFL, 0L); 
            }

         WinUpdateWindow (hwnd);
         bLock = WinQueryWindowUShort (hwnd, QW_HOMELOCK);
         WinSetWindowUShort (hwnd, QW_HOMELOCK, !bLock);

         WinSendMsg (WinQueryWindow (hwnd, QW_PARENT, FALSE),
                                     WM_SPLITMOVE, 0L, 0L);
         return 0;
         }
      case WM_BUTTON1DOWN:
         {
         WinUpdateWindow (hwnd);
         DoSplitMove (hwnd, mp1);
         WinSendMsg (WinQueryWindow (hwnd, QW_PARENT, FALSE),
                                     WM_SPLITMOVE, 0L, 0L);
         return 0;
         }
      case WM_MOUSEMOVE:
         {
         HPOINTER hptr;
         USHORT   uChildID;

         uChildID = WinQueryWindowUShort (hwnd, QWS_ID);
         hptr = ( uChildID == LWID_VSPLIT ? hVptr : hHptr);
         if (OtherSplitClose(hwnd, uChildID, mp1))
            hptr = hHVptr;
         WinSetPointer (HWND_DESKTOP, hptr);
         return 0;
         }
      case WM_MOVE:
         {
         WinInvalidateRect (hwnd, NULL, FALSE);
         return 0;
         }
      }
   return WinDefWindowProc (hwnd, umsg, mp1, mp2);
   }





