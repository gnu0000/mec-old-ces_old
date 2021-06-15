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
 * swfix.c                                                              *
 *                                                                      *
 * Copyright (C) 1990-1992 Info Tech, Inc.                              *
 *                                                                      *
 * This module was literally thrown together for fixing up the metadata *
 * It lets you use the mouse to adjust the pos/size of labels and text  *
 * in text areas and list win labels. This module will probably         *
 * need a significant re-write if we plan to use it throughout.  please *
 * excuse the duct-tape ...                                             *
 *                         SuperKludge                                  *
 *                                                                      *
 * This file is part of the Window system of DS/SHELL. This is a        *
 * proprietary product of Info Tech. and no part of which may be        *
 * reproduced or transmitted in any form or by any means, including     *
 * photocopying and recording or in connection with any information     *
 * storage or retrieval system, without permission in writing           *
 * from Info Tech, Inc.                                                 *
 *                                                                      *
 ************************************************************************/

#define  INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "winmain.h"
#include "swmain.h"
#include "lwmain.h"
#include "windef.h"
#include "swutil.h"
#include "lwutil.h"
#include "lwClient.h"
#include <stdio.h>

/**************************************************************************/
/*   tsese are for any  area                                              */
/**************************************************************************/

BOOL InvertRcl (HWND hwnd, RECTL rcl)
   {
   HPS   hps;
   RECTL rclPaint;

   WinUpdateWindow (hwnd);
   WinInvalidateRect (hwnd, &rcl, FALSE);
   hps = WinBeginPaint (hwnd, NULL, &rclPaint);
   WinDrawBorder (hps, &rcl, 1, 1, CLR_BLACK, CLR_WHITE, DB_DESTINVERT);
   WinEndPaint (hps);
   return TRUE;
   }

BOOL DisplayRcl (HWND hwnd, RECTL rcl, USHORT uIndex, BOOL bLabel)
   {
   char sz[100];

   ItiCvtRcl (hwnd, PIX|RCL|OS2, DLG|RNG|WND, &rcl); 
   sprintf (sz, "{%ld, %ld, %ld, %ld} Index=%d <%s>",
            rcl.xLeft, rcl.yBottom, rcl.xRight, rcl.yTop, uIndex,
             (bLabel ? "Label" : "Data"));

   WinMessageBox (HWND_DESKTOP, hwnd, sz, "Item Rectangle Coordinates",
                     9998, MB_OK | MB_MOVEABLE | MB_APPLMODAL);

   return TRUE;
   }

BOOL DisplayWinRcl (HWND hwnd)
   {
   char  sz[100];
   RECTL rcl;
   SWP   swp;
   HWND  hwndFrame, hwndParent;

   hwndFrame  = WinQueryWindow (hwnd, QW_PARENT, FALSE);
   hwndParent = WinQueryWindow (hwndFrame, QW_PARENT, FALSE);
   WinQueryWindowPos (hwndFrame, &swp);
   rcl.xLeft   = swp.x;
   rcl.yBottom = swp.y;
   rcl.xRight  = swp.x + swp.cx;
   rcl.yTop    = swp.y + swp.cy;
   ItiCvtRcl (hwndParent, PIX|RCL|OS2, DLG|RNG|WND, &rcl); 
   sprintf (sz, "{%ld, %ld, %ld, %ld} <%s>",
            rcl.xLeft, rcl.yBottom, rcl.xRight, rcl.yTop, "Window");

   WinMessageBox (HWND_DESKTOP, hwnd, sz, "Window Rectangle Coordinates",
                     9998, MB_OK | MB_MOVEABLE | MB_APPLMODAL);

   return TRUE;
   }






/**************************************************************************/
/*   tsese are for the static area                                        */
/**************************************************************************/

BOOL FindRectangle (HWND hwnd, MPARAM mp1,
                    USHORT *i, BOOL *bLabel, PRECTL prcl)
   {
   PSWDAT   pswd;
   SHORT   ix, iy;

   ix = SHORT1FROMMP (mp1);
   iy = SHORT2FROMMP (mp1);
   pswd = WinQueryWindowPtr (hwnd, WNDPTR_SWDAT);
   *bLabel = TRUE;
   for (*i=0; *i < pswd->uLabels; (*i)++)
      {
      *prcl = pswd->pedtLabel[*i].rclPos;
      ItiCvtRcl (hwnd, PIX|RCL|WND, PIX|RCL|OS2, prcl);
      if (prcl->xLeft   <= ix && prcl->xRight >= ix &&
          prcl->yBottom <= iy && prcl->yTop >= iy)
         return TRUE;
      }
   *bLabel = FALSE;
   for (*i=0; *i < pswd->uDatas; (*i)++)
      {
      *prcl = pswd->pedtData[*i].rclPos;
      ItiCvtRcl (hwnd, PIX|RCL|WND, PIX|RCL|OS2, prcl);
      if (prcl->xLeft   <= ix && prcl->xRight >= ix &&
          prcl->yBottom <= iy && prcl->yTop >= iy)
         return TRUE;
      }
   return FALSE;
   }


BOOL UpdateData (HWND hwnd, USHORT iIndex, BOOL bLabel, RECTL rcl)
   {
   PSWDAT        pswd;

   pswd = WinQueryWindowPtr (hwnd, WNDPTR_SWDAT);
   ItiCvtRcl (hwnd, PIX|RCL|OS2, PIX|RCL|WND, &rcl);
   if (bLabel)
      pswd->pedtLabel[iIndex].rclPos = rcl;
   else
      pswd->pedtData[iIndex].rclPos = rcl;
   WinInvalidateRect (hwnd, NULL, TRUE);
   return TRUE;
   }




WNDPROC ItiWndFixStaticWndProc (HWND hwnd, USHORT umsg, MPARAM mp1, MPARAM mp2)
   {
   static USHORT bMouse = 0, bFound = 0;
   static POINTL ptlBase;
   static RECTL  rcl;
   static USHORT uIndex;
   static BOOL   bLabel;
   POINTL        ptl;
   RECTL          rclcpy;

   switch (umsg)
      {
      case WM_CREATE:
      case WM_ACTIVATE:
         {
         bFound = FALSE;
         bMouse = 0;
         break;
         }

      case WM_BUTTON1DOWN:    /* button 1 is size */
      case WM_BUTTON2DOWN:    /* button 2 is move */
         {
         WinSetActiveWindow (HWND_DESKTOP, hwnd);
         WinSetCapture (HWND_DESKTOP, hwnd);
         if (WinGetKeyState (HWND_DESKTOP, VK_SHIFT) & 0x8000 &&
             umsg == WM_BUTTON2DOWN)
            {
            DisplayWinRcl (hwnd);
            break;
            }

         bMouse = (umsg == WM_BUTTON1DOWN ? 1 : 2);
         if (bFound = FindRectangle (hwnd, mp1, &uIndex, &bLabel, &rcl))
            {
            InvertRcl (hwnd, rcl);

            if (WinGetKeyState (HWND_DESKTOP, VK_SHIFT) & 0x8000)
               {
               rclcpy = rcl;
               bFound = FALSE;
               bMouse = 0;
               DisplayRcl (hwnd, rclcpy, uIndex, bLabel);
               InvertRcl (hwnd, rclcpy);
               break;
               }

            ptlBase.x = (LONG)(SHORT1FROMMP (mp1));
            ptlBase.y = (LONG)(SHORT2FROMMP (mp1));
            WinMapDlgPoints (hwnd, &ptlBase, 1, FALSE);
            ptlBase.x = rcl.xRight - ptlBase.x;
            ptlBase.y = rcl.yTop   - ptlBase.y;
            }
         break;
         }

      case WM_BUTTON1UP:
      case WM_BUTTON2UP:
         {
         WinSetCapture (HWND_DESKTOP, NULL);
         bMouse = 0;
         if (bFound)
            {   
            InvertRcl (hwnd, rcl);
            UpdateData (hwnd, uIndex, bLabel, rcl);
            }
         break;
         }

      case WM_MOUSEMOVE:
         {

         if (!bMouse || !bFound) 
            break;

         InvertRcl (hwnd, rcl);

         ptl.x = (LONG)(SHORT1FROMMP (mp1));
         ptl.y = (LONG)(SHORT2FROMMP (mp1));
         WinMapDlgPoints (hwnd, &ptl, 1, FALSE);

         rcl.xLeft   += (bMouse == 2 ? (ptl.x + ptlBase.x) - rcl.xRight: 0);
         rcl.yBottom += (bMouse == 2 ? (ptl.y + ptlBase.y) - rcl.yTop  : 0);
         rcl.xRight  = ptl.x + ptlBase.x;
         rcl.yTop    = ptl.y + ptlBase.y;
         InvertRcl (hwnd, rcl);
         break;
         }
      }
   return ItiWndDefStaticWndProc (hwnd, umsg, mp1, mp2);
   }



/**************************************************************************/
/*   tsese are for the label area                                        */
/**************************************************************************/


BOOL FindLabelRectangle (HWND hwnd, MPARAM mp1, USHORT *i, PRECTL prcl)
   {
   PEDT   pedt;
   SHORT   ix, iy, uCols;

   ix = SHORT1FROMMP (mp1);
   iy = SHORT2FROMMP (mp1);
   pedt = LWGetPEDT (hwnd, TRUE, FALSE);
   uCols = LWQuery (hwnd, LWM_NumLabelCols, TRUE);
   for (*i=0; *i < (USHORT)uCols; (*i)++)
      {
      *prcl = pedt[*i].rclPos;
//      ItiCvtRcl (hwnd, PIX|RCL|WND, PIX|RCL|OS2, prcl);
      if (prcl->xLeft   <= ix && prcl->xRight >= ix &&
          prcl->yBottom <= iy && prcl->yTop >= iy)
         return TRUE;
      }
   return FALSE;
   }


BOOL UpdateLabelData (HWND hwnd, USHORT iIndex, RECTL rcl)
   {
   PEDT   pedt;

   pedt     = LWGetPEDT (hwnd, TRUE, FALSE);
//   ItiCvtRcl (hwnd, PIX|RCL|OS2, PIX|RCL|OS2, &rcl);
      pedt[iIndex].rclPos = rcl;
   WinInvalidateRect (hwnd, NULL, TRUE);
   return TRUE;
   }        



WNDPROC LWLabelFixProc  (HWND hwnd, USHORT umsg, MPARAM mp1, MPARAM mp2)
   {
   static USHORT bMouse = 0, bFound = 0;
   static POINTL ptlBase;
   static RECTL  rcl;
   static USHORT uIndex;
   POINTL        ptl;
   RECTL         rclcpy;

   switch (umsg)
      {
      case WM_CREATE:
      case WM_ACTIVATE:
         {
         bFound = FALSE;
         bMouse = 0;
         break;
         }

      case WM_BUTTON1DOWN:    /* button 1 is size */
      case WM_BUTTON2DOWN:    /* button 2 is move */
         {
         WinSetActiveWindow (HWND_DESKTOP, hwnd);
         WinSetCapture (HWND_DESKTOP, hwnd);
         if (WinGetKeyState (HWND_DESKTOP, VK_SHIFT) & 0x8000 &&
             umsg == WM_BUTTON2DOWN)
            {
            DisplayWinRcl (hwnd);
            break;
            }

         bMouse = (umsg == WM_BUTTON1DOWN ? 1 : 2);
         if (bFound = FindLabelRectangle (hwnd, mp1, &uIndex, &rcl))
            {
            InvertRcl (hwnd, rcl);

            if (WinGetKeyState (HWND_DESKTOP, VK_SHIFT) & 0x8000)
               {
               rclcpy = rcl;
               bFound = FALSE;
               bMouse = 0;
               DisplayRcl (hwnd, rclcpy, uIndex, TRUE);
               InvertRcl (hwnd, rclcpy);
               break;
               }

            ptlBase.x = (LONG)(SHORT1FROMMP (mp1));
            ptlBase.y = (LONG)(SHORT2FROMMP (mp1));
            WinMapDlgPoints (hwnd, &ptlBase, 1, FALSE);
            ptlBase.x = rcl.xRight - ptlBase.x;
            ptlBase.y = rcl.yTop   - ptlBase.y;
            }
         break;
         }

      case WM_BUTTON1UP:
      case WM_BUTTON2UP:
         {
         WinSetCapture (HWND_DESKTOP, NULL);
         bMouse = 0;
         if (bFound)
            {   
            InvertRcl (hwnd, rcl);
            UpdateLabelData (hwnd, uIndex, rcl);
            }
         break;
         }

      case WM_MOUSEMOVE:
         {

         if (!bMouse || !bFound) 
            break;

         InvertRcl (hwnd, rcl);

         ptl.x = (LONG)(SHORT1FROMMP (mp1));
         ptl.y = (LONG)(SHORT2FROMMP (mp1));
         WinMapDlgPoints (hwnd, &ptl, 1, FALSE);

         rcl.xLeft   += (bMouse == 2 ? (ptl.x + ptlBase.x) - rcl.xRight: 0);
         rcl.yBottom += (bMouse == 2 ? (ptl.y + ptlBase.y) - rcl.yTop  : 0);
         rcl.xRight  = ptl.x + ptlBase.x;
         rcl.yTop    = ptl.y + ptlBase.y;
         InvertRcl (hwnd, rcl);
         break;
         }
      }
   return LWLabelProc (hwnd, umsg, mp1, mp2);
   }







/**************************************************************************/
/*                                                                        */
/**************************************************************************/

