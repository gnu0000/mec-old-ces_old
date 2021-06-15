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
 * lwmouse.c                                                            *
 *                                                                      *
 * Copyright (C) 1990-1992 Info Tech, Inc.                              *
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
#include "..\include\itidbase.h"
#include "..\include\itiwin.h"
#include "winmain.h"
#include "winutil.h"
#include "lwmain.h"
#include "lwutil.h"
#include "lwsplit.h"
#include "lwsize.h"
#include "lwselect.h"
#include "lwkey.h"
#include <stdlib.h>
#include "lwmouse.h"


#define LW_TIMERID   10



/* returns either
 *
 * uRow       - if a row is selected
 * LWMP_BLANK - if mouse selects blank area
 * LWMP_BELOW - if mouse selects below window
 * LWMP_ABOVE - if mouse selects above window
 */
USHORT LWRowFromMouse (HWND hwndClient, MPARAM mp1, USHORT *uDistance)
   {
   SHORT    iYMouse;
   USHORT   uYPos, uRow;
   POINTS   ptsScrPos, ptsMax;
   RECTL    rclClient;

   WinQueryWindowRect (hwndClient, &rclClient);
   iYMouse = (SHORT)(SHORT2FROMMP (mp1));

   if (iYMouse < 0)
      {
      *uDistance = (USHORT)(-iYMouse); 
      return LWMP_BELOW; /* mouse is below window */
      }
   else if (iYMouse > (SHORT) rclClient.yTop)
      {
      *uDistance = (USHORT)(iYMouse - (SHORT) rclClient.yTop);
      return LWMP_ABOVE; /* mouse is above window */
      }
   uYPos =  (USHORT)rclClient.yTop - (USHORT)iYMouse;
   GetScrollPos (hwndClient, &ptsScrPos, &ptsMax);
   uRow = ptsScrPos.y + uYPos / LWQuery (hwndClient, LWM_YRowSize, TRUE);
   *uDistance = 0;
   return (uRow >= LWQuery (hwndClient, LWM_NumRows, TRUE) ? LWMP_BLANK : uRow);
   }




/* returns either
 *
 * uCol       - if a row is selected
 * LWMP_BLANK - if mouse selects blank area
 * LWMP_BELOW - if mouse selects below window
 * LWMP_ABOVE - if mouse selects above window
 */
USHORT LWColFromMouse (HWND hwndClient, MPARAM mp1, BOOL bClient)
   {
   USHORT   uXSize, uYSize, uXMouse, uYMouse,
            uCol, uNumCols, i, uYWinSize;
   RECTL    rcl;
   POINTS   ptsScrPos, ptsMax;
   PEDT     pedt;

   GetScrollPos (hwndClient, &ptsScrPos, &ptsMax);
   uCol    = LWMP_BLANK;
   uXSize  = LWQuery (hwndClient, LWM_XScrollInc, TRUE);
   uYSize  = LWQuery (hwndClient, LWM_YRowSize, TRUE);
   uYWinSize= LWQuery (hwndClient, LWM_YWindowSize, TRUE);
   uXMouse = SHORT1FROMMP (mp1) + uXSize * ptsScrPos.x;
   uYMouse = uYWinSize - (uYWinSize - SHORT2FROMMP (mp1)) % uYSize;
   uNumCols= LWQuery (hwndClient, (bClient ? LWM_NumCols : LWM_NumLabelCols), TRUE);
   pedt    = LWGetPEDT (hwndClient, TRUE, bClient);

   for (i = 0; i < uNumCols; i++)
      {
      rcl = pedt[i].rcl;
      ItiWndPrepRcl (hwndClient, pedt[i].uCoordType, &rcl);
      if (rcl.xLeft   > (LONG) uXMouse ||
          rcl.yBottom > (LONG) uYMouse ||
          rcl.xRight  < (LONG) uXMouse ||
          rcl.yTop    < (LONG) uYMouse  )
         continue;
      uCol  = i;
      break;
      }
   return uCol;
   }





void LWMouseClick (HWND hwndClient,
                   MPARAM mp1,
                   MPARAM mp2,
                   BOOL bClient,
                   BOOL bDblClk,
                   BOOL bLeftMouse)
   {
   USHORT   uCol, uRow, uType, uWinId, uDistance, uData;
   HWND     hwndLW, hwndFrame;

   uCol  = uRow = 0;
   uType = (bClient ? LW_DATAAREA : LW_LABELAREA);

   uRow  = (bClient ? LWRowFromMouse (hwndClient, mp1, &uDistance) :
                      LWQuery (hwndClient, LWM_Active, TRUE));
   if (uRow == LWMP_BLANK || uRow == LWMP_ABOVE || uRow == LWMP_BELOW)
      uRow = 0xFFFF;
   else
      {
      uCol = LWColFromMouse (hwndClient, mp1, bClient);
      if (uCol == LWMP_BLANK || uCol == LWMP_ABOVE || uCol == LWMP_BELOW)
         uCol = 0;
      else
         uType = (bClient ? LW_DATA : LW_LABEL);
      }
   hwndLW    = WinQueryWindow (hwndClient, QW_PARENT, 0);
   hwndFrame = WinQueryWindow (hwndLW, QW_PARENT, 0);
   uWinId    = WinQueryWindowUShort (hwndLW, QWS_ID);

   uData = uType | (bDblClk ? 0x0100 : 0) | (bLeftMouse ? 0 : 0x0200);
   WinSendMsg (hwndFrame, WM_ITILWMOUSE, MPFROM2SHORT (uRow, uCol),
                                MPFROM2SHORT (uWinId, uData));
   }









static USHORT StopTimer (HWND hwndClient, USHORT uTimerDir)
   {
   if (uTimerDir)
      WinStopTimer (WinQueryAnchorBlock (hwndClient), hwndClient, LW_TIMERID);
   return 0;
   }



static USHORT StartTimer (HWND hwndClient, USHORT uNew, USHORT uDistance)
   {
   USHORT uDelay;

   uDelay = 3;
   WinStartTimer (WinQueryAnchorBlock (hwndClient),
                  hwndClient, LW_TIMERID, uDelay);
   return uNew;
   }



static void DoSel (HWND hwnd,
                   BOOL bSel,
                   USHORT uAnchor, USHORT uNew, USHORT uOld,
                   BOOL bCtrl,
                   BOOL bStartSel)
   {
   if (!bSel)
      return;

   if (bCtrl)
      LWSelectRange (hwnd, uNew, uOld, (bStartSel ? ITI_UNSELECT : ITI_SELECT));
   else
      LWSelectBoundedRange (hwnd, uAnchor, uNew, uOld);
   }




USHORT LWDoMouseSelect (HWND hwndClient, MPARAM mp1, USHORT umsg)
   {
   QMSG     qmsg;
   USHORT   uNew, uOld, uMax, uDistance, uAnchor;
   USHORT   uVSBID, uHSBID, uClientID, fOptions;
   HWND     hwndFrame;
   USHORT   uTimerDir = 0;
   BOOL     bShift, bCtrl, bSel, bStartSel;

   if (umsg == WM_BUTTON1DOWN)
      WinSetCapture (HWND_DESKTOP, hwndClient);

   hwndFrame = WinQueryWindow (hwndClient, QW_PARENT, FALSE);
   fOptions  = LWQuery (hwndClient, LWM_Options, TRUE);
   WndGetIds (hwndClient, &uClientID, &uVSBID, &uHSBID);
   uAnchor   = LWQuery (hwndFrame, LWM_Active, FALSE);
   uMax      = LWQuery (hwndFrame, LWM_NumRows, FALSE) - 1;

   bShift    = (WinGetKeyState (HWND_DESKTOP, VK_SHIFT) < 0);
   bCtrl     = (WinGetKeyState (HWND_DESKTOP, VK_CTRL)  < 0);
   bSel      = (fOptions & LWS_MULTISELECT) == LWS_MULTISELECT;

   if (bSel && !bShift && !bCtrl)
      LWDeselectAll (hwndClient, TRUE);

   uOld = uNew = LWRowFromMouse (hwndClient, mp1, &uDistance);
   SetNewActive (hwndClient, uVSBID, uAnchor, uNew);

   if (!bShift)
      uAnchor = uNew;

   bStartSel = (bSel ? LWIsSelected (hwndClient, uNew) : FALSE);


   /*--- Select first element clicked on ---*/
   if (bSel && bShift && uNew != 0xFFFF)
      LWSelectRange (hwndClient, uAnchor, uNew, ITI_SELECT);
   else if (bSel && uNew != 0xFFFF)
      LWSelectElement (hwndClient, uNew, !(bCtrl && bStartSel));

   if (umsg != WM_BUTTON1DOWN)
      return TRUE;


   while (WinGetMsg (NULL, (PQMSG)&qmsg, NULL, 0, 0))
      switch (qmsg.msg)
      {
      case WM_BUTTON1UP:
         WinSetCapture (HWND_DESKTOP, NULL);
         StopTimer (hwndClient, uTimerDir);
         return TRUE;

      case WM_MOUSEMOVE:
         if ((uNew = LWRowFromMouse (hwndClient, qmsg.mp1, &uDistance)) == uOld)
            break;

         else if (uNew == LWMP_BLANK)
            uTimerDir = StopTimer (hwndClient, uTimerDir);

         else if (uNew == LWMP_BELOW && uOld < uMax)   /* below window */
            uTimerDir = StartTimer (hwndClient, uNew, uDistance);

         else if (uNew == LWMP_ABOVE && uOld > 0)      /* above window */
            uTimerDir = StartTimer (hwndClient, uNew, uDistance);

         else if (uNew < LWMP_ERROR)                   /* on screen */
            {
            uTimerDir = StopTimer (hwndClient, uTimerDir);
            DoSel (hwndClient, bSel, uAnchor, uNew, uOld, bCtrl, bStartSel);
            uOld = SetNewActive (hwndClient, uVSBID, uOld, uNew);
            }

         break;

      case WM_TIMER:
         if (LW_TIMERID != SHORT1FROMMP (qmsg.mp1))
            WinDispatchMsg (NULL, (PQMSG)&qmsg);

         if (uTimerDir == LWMP_BELOW && uOld == uMax ||
             uTimerDir == LWMP_ABOVE && uOld == 0)
            uTimerDir = StopTimer (hwndClient, uTimerDir);

         else if (uTimerDir == LWMP_BELOW)
            {
            DoSel (hwndClient, bSel, uAnchor, uOld+1, uOld, bCtrl, bStartSel);
            uOld = SetNewActive (hwndClient, uVSBID, uOld, uOld +1);
            }

         else /*(uTimerDir == LWMP_ABOVE)*/
            {
            DoSel (hwndClient, bSel, uAnchor, uOld-1, uOld, bCtrl, bStartSel);
            uOld = SetNewActive (hwndClient, uVSBID, uOld, uOld -1);
            }
         break;

      default:
         WinDispatchMsg (NULL, (PQMSG)&qmsg);
         break;
      }
   WinSetCapture (HWND_DESKTOP, NULL);
   return TRUE;
   }





