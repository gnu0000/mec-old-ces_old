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
 * lwkey.c                                                              *
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
#include "lwmain.h"
#include "lwutil.h"
#include "lwsplit.h"
#include "lwsize.h"
#include "lwselect.h"
#include "lwkey.h"
#include "winutil.h"
#include <stdlib.h>






/* This proc. handles keyboard input when a list window is active.
 * Up/Down Arrow - line
 * Pg Up/Down    - page
 * Home/End      - list
 * Tab           - switch client focus to another viewed client
 * letter/number - search on a selected field (1st in buffer?)
 *
 *
 *
 *  returns TRUE if the msg is handled.
 *  returns FALSE if msg should be passed to parent
 */
BOOL WndDoKey (HWND hwndClient, MPARAM mp1, MPARAM mp2)
   {
   USHORT uXMin, uX, uXMax, uNewX, uXWinSize, uXRowSize;
   USHORT uYMin, uY, uYMax, uNewY, uYWinSize, uYRowSize;
   LONG   sDelX, sDelY;
   BOOL   bShift, bAlt, bCtrl, bVirtual, bSel;
   USHORT fFlags, fOptions, uRepeat, uScanCode, uVKey;
   USHORT uClientId, uVSBId, uHSBId, uYPrime;
   UCHAR  cChr1, cChr2;
   POINTS ptsPos, ptsMax;
   RECTL  rclClient;
   HWND   hwndFrame;

   /*--- KEY INFORMATION ---*/
   fFlags    = (USHORT) SHORT1FROMMP (mp1);
   uRepeat   = (UCHAR)  CHAR3FROMMP  (mp1);
   uScanCode = (UCHAR)  CHAR4FROMMP  (mp1);
   cChr1     = (UCHAR)  CHAR1FROMMP  (mp2);
   cChr2     = (UCHAR)  CHAR2FROMMP  (mp2);
   uVKey     = (USHORT) SHORT2FROMMP (mp2);
   bShift    = !!(fFlags & KC_SHIFT);
   bAlt      = !!(fFlags & KC_ALT);
   bCtrl     = !!(fFlags & KC_CTRL);
   bVirtual  = !!(fFlags & KC_VIRTUALKEY);

   if (fFlags & KC_KEYUP)        /*--- only process keydowns ---*/
      return 0;


   if (bCtrl && bAlt && bShift && bVirtual && uVKey == VK_SPACE)
      {
      DEBUG_BREAK;
      }

   /*--- Window Metric Information ---*/
   fOptions = LWQuery (hwndClient, LWM_Options, TRUE);

   hwndFrame = WinQueryWindow (hwndClient, QW_PARENT, FALSE);
   GetScrollPos (hwndClient, &ptsPos, &ptsMax);
   WinQueryWindowRect (hwndClient, &rclClient);
   uYRowSize = LWQuery (hwndClient, LWM_YRowSize, TRUE);
   uXRowSize = LWQuery (hwndClient, LWM_XRowSize, TRUE);

   uXMin = 0;
   uX    = ptsPos.x;
   uXMax = ptsMax.x;

   uYMin = 0;
   uY    = LWQuery (hwndClient, LWM_Active, TRUE);
   uYMax = (USHORT) max (0L, (LONG)LWQuery (hwndClient, LWM_NumRows, TRUE) - 1L);

   uYWinSize = (USHORT)rclClient.yTop   / uYRowSize;;
   uXWinSize = (USHORT)rclClient.xRight / uXRowSize;;
   WndGetIds (hwndClient, &uClientId, &uVSBId, &uHSBId);
   sDelX = sDelY = 0L;

   bSel = (fOptions & LWS_MULTISELECT) == LWS_MULTISELECT;

   if (bSel                          &&
       !bShift                       &&
       !bCtrl                        &&
       !bAlt                         &&
       !(!bVirtual && cChr1 == ' ')  &&
       !(bVirtual  && uVKey == VK_SPACE))
      LWDeselectAll (hwndClient, TRUE);

   if (!bVirtual)               /* Ctrl-Digit is 10xPercentile */
      {
      if (cChr1 >= '0' && cChr1 <= '9' && bCtrl)
         sDelY = (LONG) (uYMax * (cChr1 - '0') / 10) - (LONG)uY;
      else 
         switch (cChr1)
            {
            case '.':
               SW (hwndFrame, ITIWND_ACTIVE, 0, uY, 0);
               return TRUE;

            default:
               return FALSE;
            }
      }
   else
      {
      switch (uVKey)
         {
         case VK_TAB:
            return SetClientFocus (hwndClient, LWF_NEXT, TRUE); 
            break;

         case VK_UP:
            if (bAlt) 
               return SetClientFocus (hwndClient, LWF_UP, TRUE); 
            sDelY = -1L;
            break;

         case VK_DOWN:
            if (bAlt) 
               return SetClientFocus (hwndClient, LWF_DOWN, TRUE); 
            sDelY =  1L;
            break;

         case VK_RIGHT:
            if (bAlt) 
               return SetClientFocus (hwndClient, LWF_RIGHT, TRUE); 
            sDelX = 1L;
            break;

         case VK_LEFT:
            if (bAlt) 
               return SetClientFocus (hwndClient, LWF_RIGHT, TRUE); 
            sDelX = -1L;
            break;

         case VK_PAGEUP:
            sDelY = -(LONG)uYWinSize;
            break;

         case VK_PAGEDOWN:
            sDelY =  (LONG)uYWinSize;
            break;

         case VK_HOME:
            sDelY = -(LONG)uY;
            break;

         case VK_END:
            sDelY =  (LONG)uYMax - (LONG)uY;
            break;

         case VK_SPACE:
            if (uY < 0xFFFA && bSel)
               LWSelectElement (hwndClient, uY, !LWIsSelected(hwndClient, uY));
            return TRUE;

         default:
            return FALSE;
         }
      }
   uNewX = (USHORT)max ((LONG)0, min ((LONG)uXMax, (LONG)uX + sDelX));
   uNewY = (USHORT)max ((LONG)0, min ((LONG)uYMax, (LONG)uY + sDelY));

   if (bCtrl)
      {
      SetNewActive (hwndClient, uVSBId, uY, uNewY);
      }
   else if (bShift)
      {
      uYPrime = uY + (uY<uNewY ? 1 : (uY>uNewY ? -1 : 0));
      LWSelectRange (hwndClient, uYPrime, uNewY, ITI_SELECT);
      SetNewActive (hwndClient, uVSBId, uY, uNewY);
      }
   else /*--- normal ---*/
      {
      if (bSel)
         LWSelectElement (hwndClient, uNewY, TRUE);
      SetNewActive (hwndClient, uVSBId, uY, uNewY);
      }
   LWSendActiveMsg (hwndFrame, 0);

   if (uNewX != uX)
      DoHScroll (hwndFrame, MPFROMSHORT (uHSBId),
                 MPFROM2SHORT (uNewX, SB_LWTRACK));
   return TRUE;
   }
