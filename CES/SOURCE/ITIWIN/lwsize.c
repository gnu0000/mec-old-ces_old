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
 * lwsize.c                                                             *
 *                                                                      *
 * Copyright (C) 1990-1992 Info Tech, Inc.                              *
 *                                                                      *
 * This file contains utility procedures for sizing list window         *
 * elements                                                             *
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
#include "winutil.h"
#include "lwmain.h"
#include "lwutil.h"
#include "lwsplit.h"
#include "lwsize.h"
#include <stdio.h>


USHORT okSizeErr (PSZ psz, USHORT uId)
   {
   char  szTmp [STRLEN];

   sprintf (szTmp, "ITIWIN: lwsize.c : %s%d", psz, uId);
   ItiErrWriteDebugMessage (szTmp);
   WinMessageBox (pglobals->hwndDesktop, pglobals->hwndAppFrame, szTmp,
                  "List Util Error", 0, MB_OK | MB_APPLMODAL);
   return 1;
   }


USHORT LWQueryFullRect (HWND hwndFrame, PRECTL prcl)
   {
   SHORT fOptions;

   if (! WinQueryWindowRect (hwndFrame, prcl))
      return okSizeErr ("Unable to obtain window rect", 0);

   fOptions = LWQuery (hwndFrame, LWM_Options, FALSE);
   if (fOptions & LWS_BORDER)
      {
      prcl->xLeft++;
      prcl->yTop-- ;
      prcl->xRight--;
      prcl->yBottom++;
      }
   return 0;
   }


USHORT LWQueryClientRect (HWND hwndFrame, PRECTL prcl)
   {
   SHORT iHSBWidth, iVSBWidth, iYLabel;
   SHORT fOptions, iLineWidth; 

   if (LWQueryFullRect (hwndFrame, prcl))
      return 10;

   fOptions = LWQuery (hwndFrame, LWM_Options, FALSE);
   iHSBWidth    =((fOptions & LWS_HSCROLL)== LWS_HSCROLL ? yHSB-1: 0);
   iVSBWidth    =((fOptions & LWS_VSCROLL)== LWS_VSCROLL ? xVSB-1: 0);
   iYLabel      =((fOptions & LWS_LABEL  )== LWS_LABEL   ? LWQuery (hwndFrame, LWM_YLabelSize, FALSE): 0);
   iLineWidth   =!!((fOptions & LWS_UNDERLINELABEL)== LWS_UNDERLINELABEL);

   prcl->yBottom += (LONG) iHSBWidth;
   prcl->xRight  -= (LONG) iVSBWidth;
   prcl->yTop    -= (LONG) (iYLabel + iLineWidth);
   return 0;
   }



/* tells how split bar should be positioned
 *         -1 = far left or top
 *          0 = in use
 *          1 = far right or bottom
 */
SHORT SplitBarUse (HWND hwndFrame, USHORT uChildID, int *iNewPos)
   {
   HWND   hwndSplit;
   RECTL  rcl;
   BOOL   bHSB, bVSB, bHSP, bVSP, bQB, bIsHSplit, bLock;
   SHORT  fOptions, iLockVal, iEdge, iWndPos; 

   if (LWQueryClientRect (hwndFrame, &rcl))
      return 10;

   bIsHSplit = (uChildID == LWID_HSPLIT);

   fOptions  = LWQuery (hwndFrame, LWM_Options, FALSE);
   iLockVal  = (fOptions & LWS_SPLITATTOP ? -1 : 1);
   bHSB      = (fOptions & LWS_HSCROLL) == LWS_HSCROLL;
   bVSB      = (fOptions & LWS_VSCROLL) == LWS_VSCROLL;
   bHSP      = (fOptions & LWS_HSPLIT)  == LWS_HSPLIT;
   bVSP      = (fOptions & LWS_VSPLIT)  == LWS_VSPLIT;
   bQB       = (fOptions & LWS_QBUTTON) == LWS_QBUTTON;

   rcl.xRight  -= (bVSP ? xVSPLIT : 0) + (bQB && !bVSB ? xVSB-1 : 0);
   rcl.yBottom += (bQB && !bHSB ? yHSB-1 : 0);
   rcl.yTop    -= (bHSP ? yHSPLIT : 0);

   /* --- if illegal or split does not exist --- */
   if ((!bIsHSplit && !bVSP) || (bIsHSplit && !bHSP) ||
       ((hwndSplit = WinWindowFromID (hwndFrame, uChildID)) == NULL))
      {
      *iNewPos = (SHORT)(bIsHSplit ? rcl.yBottom : rcl.xRight);
      return 1;
      }

   GetSplitLocation (hwndSplit, &iWndPos, &bLock);
   iEdge = (bLock ? iLockVal : SplitBarAtEdge (hwndFrame, uChildID, iWndPos));

   switch (iEdge)
      {
      case -1 : *iNewPos = (SHORT)(bIsHSplit ? rcl.yTop : rcl.xLeft);     break;
      case  0 : *iNewPos = iWndPos;                                       break;
      case  1 : *iNewPos = (SHORT)(bIsHSplit ? rcl.yBottom : rcl.xRight); break;
      }
   return iEdge;
   }













int CalcSplit ( HWND hwndFrame,
                USHORT uChildID,
                POINTS *pptsPos,
                POINTS *pptsSiz)
   {                  
   RECTL  rclFull;
   BOOL   bUse;
   SHORT  iNewPos, fOptions;
   SHORT  xLeft,   xRight,
          yTop,    yBottom;
   SHORT  iVSplitWidth, iHSplitWidth;
   SHORT  iHSBWidth,    iVSBWidth;

   if (LWQueryFullRect (hwndFrame, &rclFull))
      return 1;
   xLeft   = (SHORT) rclFull.xLeft;
   xRight  = (SHORT) rclFull.xRight;
   yBottom = (SHORT) rclFull.yBottom;
   yTop    = (SHORT) rclFull.yTop;
   fOptions     = LWQuery (hwndFrame, LWM_Options, FALSE);
   iVSplitWidth =((fOptions & LWS_VSPLIT )== LWS_VSPLIT  ? xVSPLIT: 0);
   iHSplitWidth =((fOptions & LWS_HSPLIT )== LWS_HSPLIT  ? yHSPLIT: 0);
   iHSBWidth    =((fOptions & LWS_HSCROLL)== LWS_HSCROLL ? yHSB-1: 0);
   iVSBWidth    =((fOptions & LWS_VSCROLL)== LWS_VSCROLL ? xVSB-1: 0);

   fOptions = LWQuery (hwndFrame, LWM_Options, FALSE);

   bUse = SplitBarUse (hwndFrame, uChildID, &iNewPos);
   if (uChildID == LWID_VSPLIT)
      {
      pptsSiz->x = iVSplitWidth;
      pptsSiz->y = (bUse ? iHSBWidth: yTop - yBottom);
      pptsPos->x = iNewPos;
      pptsPos->y = yBottom;
      return 0;
      }

   else /* LWID_HSPLIT */
      {
      pptsSiz->x = (bUse ? iVSBWidth: xRight - xLeft);
      pptsSiz->y = iHSplitWidth;
      pptsPos->x = (bUse ? xRight - iVSBWidth : xLeft);
      pptsPos->y = iNewPos;
      return 0;
      }
   return 1;
   }



















int CalcWndPosSize ( HWND   hwndFrame,
                     USHORT uChildID,
                     POINTS *pptsPos,
                     POINTS *pptsSize)
   {
   RECTL  rclFull,       rclClient;
   SHORT  iHSplitUse,    iVSplitUse;
   SHORT  iVSplitWidth,  iHSplitWidth;
   SHORT  iYSplitBottom, iXSplitLeft;
         
   SHORT  iHSBWidth,     iVSBWidth;
   SHORT  iYLabel,       iLineWidth;
         
   SHORT  iXButton,      iYButton;
   SHORT  xLeft, xRight, yBottom, yTop;
   SHORT  fOptions;
   SHORT  X0, Y0, X1, Y1;

   /*=================================================================*/

   if (hwndFrame == NULL)
      return 1;

   pptsPos->x = pptsPos->y = pptsSize->x = pptsSize->y = 0;

   if (LWQueryFullRect (hwndFrame,  &rclFull))
      return 1;
   if (LWQueryClientRect (hwndFrame,  &rclClient))
      return 1;
   fOptions = LWQuery (hwndFrame, LWM_Options, FALSE);
   xLeft   = (SHORT) rclFull.xLeft;
   xRight  = (SHORT) rclFull.xRight;
   yBottom = (SHORT) rclFull.yBottom;
   yTop    = (SHORT) rclFull.yTop;

   if (uChildID == 0x0001 /*--LWID_VSPLIT--*/ ||
       uChildID == 0x0002 /*--LWID_HVSPLIT--*/)
      return CalcSplit (hwndFrame, uChildID, pptsPos, pptsSize);

   iHSplitUse  = SplitBarUse (hwndFrame, LWID_HSPLIT, &iYSplitBottom);
   iVSplitUse  = SplitBarUse (hwndFrame, LWID_VSPLIT, &iXSplitLeft);
            
   iHSBWidth    =((fOptions & LWS_HSCROLL)== LWS_HSCROLL ? yHSB-1: 0);
   iVSBWidth    =((fOptions & LWS_VSCROLL)== LWS_VSCROLL ? xVSB-1: 0);
   iVSplitWidth =((fOptions & LWS_VSPLIT )== LWS_VSPLIT  ? xVSPLIT: 0);
   iHSplitWidth =((fOptions & LWS_HSPLIT )== LWS_HSPLIT  ? yHSPLIT: 0);
   iYLabel      =((fOptions & LWS_LABEL  )== LWS_LABEL   ? LWQuery (hwndFrame, LWM_YLabelSize, FALSE): 0);
   iLineWidth   =!!((fOptions & LWS_UNDERLINELABEL)== LWS_UNDERLINELABEL);
   iXButton     =((fOptions & LWS_QBUTTON)== LWS_QBUTTON && !iVSBWidth ? xVSB-1: 0);
   iYButton     =((fOptions & LWS_QBUTTON)== LWS_QBUTTON && !iHSBWidth ? yHSB-1: 0);

   /*=================================================================*/

   /* --- X Left --- */
   switch (uChildID)
      {
      case 0x0009: /*--LWID_CLIENT[0][0]--*/
      case 0x000B: /*--LWID_CLIENT[1][0]--*/
      case 0x0003: /*--LWID_LABEL[0]    --*/
      case 0x0005: /*--LWID_HSB[0]      --*/
         X0 = (SHORT) rclClient.xLeft;
         break;

      case 0x0004: /*--LWID_LABEL[1]    --*/
      case 0x000A: /*--LWID_CLIENT[0][1]--*/
      case 0x000C: /*--LWID_CLIENT[1][1]--*/
         switch (iVSplitUse)
            {
            case -1: X0 = (SHORT)rclClient.xLeft;             break;
            case  0: X0 = iXSplitLeft + iVSplitWidth;  break;
            case  1: X0 = (SHORT)rclClient.xRight;            break;
            }
         break;

      case 0x0006: /*--LWID_HSB[1]      --*/
         X0 = iXSplitLeft + iVSplitWidth;
         break;

      case 0x0007: /*--LWID_VSB[0]      --*/
      case 0x0008: /*--LWID_VSB[1]      --*/
         X0 = (SHORT)rclClient.xRight;
         break;

      case 0x000D: /*--LWID_QBUTTON     --*/
//         X0 = (SHORT)rclClient.xRight - iXButton;
         X0 = xRight - (xVSB-1);
         break;
      }




   /* --- Y Bottom --- */
   switch (uChildID)
      {
      case 0x0005: /*--LWID_HSB[0]      --*/
      case 0x0006: /*--LWID_HSB[1]      --*/
         Y0 = yBottom-1;
         break;
      case 0x000D: /*--LWID_QBUTTON     --*/
         Y0 = yBottom;
         break;

      case 0x0009: /*--LWID_CLIENT[0][0]--*/
      case 0x000A: /*--LWID_CLIENT[0][1]--*/
         Y0 = (SHORT)rclClient.yBottom;
         break;

      case 0x0007: /*--LWID_VSB[0]      --*/
         Y0 = (SHORT)rclClient.yBottom + iYButton;
         break;

      case 0x000B: /*--LWID_CLIENT[1][0]--*/
      case 0x000C: /*--LWID_CLIENT[1][1]--*/
         switch (iHSplitUse)
            {
            case -1: Y0 = (SHORT)rclClient.yTop;               break;
            case  0: Y0 = iYSplitBottom + iHSplitWidth; break;
            case  1: Y0 = (SHORT)rclClient.yBottom;            break;
            }
         break;

      case 0x0008: /*--LWID_VSB[1]      --*/
         case  0: Y0 = iYSplitBottom + iHSplitWidth;
         break;

      case 0x0003: /*--LWID_LABEL[0]    --*/
      case 0x0004: /*--LWID_LABEL[1]    --*/
         Y0 = yTop - iYLabel;
         break;
      }




   /* --- X Right --- */
   switch (uChildID)
      {
      case 0x0009: /*--LWID_CLIENT[0][0]--*/
      case 0x000B: /*--LWID_CLIENT[1][0]--*/
      case 0x0003: /*--LWID_LABEL[0]    --*/
         switch (iVSplitUse)
            {
            case -1: X1 = (SHORT)rclClient.xLeft;     break;
            case  0: X1 = iXSplitLeft;         break;
            case  1: X1 = (SHORT)rclClient.xRight;    break;
            }
         break;

      case 0x0005: /*--LWID_HSB[0]      --*/
         X1 = iXSplitLeft;
         break;

      case 0x0004: /*--LWID_LABEL[1]    --*/
      case 0x000A: /*--LWID_CLIENT[0][1]--*/
      case 0x000C: /*--LWID_CLIENT[1][1]--*/
         X1 = (SHORT)rclClient.xRight;
         break;

      case 0x0006: /*--LWID_HSB[1]      --*/
         X1 = (SHORT)rclClient.xRight - iXButton;
         break;

      case 0x0007: /*--LWID_VSB[0]      --*/
      case 0x0008: /*--LWID_VSB[1]      --*/
         X1 = xRight+1;
         break;
      case 0x000D: /*--LWID_QBUTTON     --*/
         X1 = xRight;
         break;
      }


   /* --- Y Top --- */
   switch (uChildID)
      {
      case 0x0005: /*--LWID_HSB[0]      --*/
      case 0x0006: /*--LWID_HSB[1]      --*/
         Y1 = (SHORT)rclClient.yBottom;
         break;

      case 0x000D: /*--LWID_QBUTTON     --*/
           Y1 = yBottom + yHSB-1;
//         Y1 = (SHORT)rclClient.yBottom - iYButton;
         break;

      case 0x0009: /*--LWID_CLIENT[0][0]--*/
      case 0x000A: /*--LWID_CLIENT[0][1]--*/
         switch (iHSplitUse)
            {
            case -1: Y1 =(SHORT) rclClient.yTop;          break;
            case  0: Y1 = iYSplitBottom;           break;
            case  1: Y1 = (SHORT)rclClient.yBottom;       break;
            }
         break;

      case 0x0007: /*--LWID_VSB[0]      --*/
         Y1 = iYSplitBottom;
         break;

      case 0x000B: /*--LWID_CLIENT[1][0]--*/
      case 0x000C: /*--LWID_CLIENT[1][1]--*/
      case 0x0008: /*--LWID_VSB[1]      --*/
         Y1 = (SHORT)rclClient.yTop;
         break;

      case 0x0003: /*--LWID_LABEL[0]    --*/
      case 0x0004: /*--LWID_LABEL[1]    --*/
         Y1 = yTop;
         break;
      }
   /*=================================================================*/

   return WndAssignPts (pptsPos, pptsSize, X0, Y0, X1 - X0, Y1 - Y0);
   }







/* this routine determines if the split bar is at an edge
 * iPos = x or y position of the split bar
 * this proc assumes y has been switched around so y=0 is a top
 *
 *               -1 = far left or top
 *                0 = in use
 *                1 = far right or bottom
 */

USHORT SplitBarAtEdge (HWND hwndFrame, USHORT uChildID, SHORT iPos)
   {
   RECTL rclClient;
   SHORT fOptions;
   BOOL  bVert;

   if (LWQueryClientRect (hwndFrame, &rclClient))
      return 1;
   rclClient.xLeft   += LW_XMINWINDOW;
   rclClient.yBottom += LW_YMINWINDOW;
   rclClient.xRight  -= LW_XMINWINDOW;
   rclClient.yTop    -= LW_YMINWINDOW;

   fOptions = LWQuery (hwndFrame, LWM_Options, FALSE);
   bVert = (uChildID == LWID_VSPLIT);

   if (iPos < (SHORT)(bVert ? rclClient.xLeft : rclClient.yBottom))
      return (bVert ? -1 : 1);
   if (iPos > (SHORT)(bVert ? rclClient.xRight: rclClient.yTop))
      return (bVert ? 1 : -1);
   return 0;
   }


