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
 * lwselect.c                                                           *
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
#include "..\include\itiwin.h"
#include "..\include\itidbase.h"
#include "winmain.h"
#include "lwmain.h"
#include "winutil.h"
#include "lwutil.h"
#include "lwsplit.h"
#include "lwselect.h"
#include "lwclient.h"
#include "lwkey.h"
#include "lwSize.h"
#include <stdlib.h>



/*****************************************************************/
/*                                                               */
/*                         Focus Procedures                      */
/*                                                               */
/*****************************************************************/




/* hwnd = client window if bClient = TRUE
 *        frame window if bClient = FALSE
 * uCmd:
 *    LWF_UP         - set focus to client above
 *    LWF_DOWN       - set focus to client below
 *    LWF_RIGHT      - set focus to client to right
 *    LWF_LEFT       - set focus to client to left
 *    LWF_DEFAULT    - set focus to top left (depends on initial split loc)
 *    LWF_CURRENT    - set focus to same window (but checks splits)
 *    LWF_NEXT       - set focus to next dir.
 *    LWF_SPECIFIED  - set focus to hwnd (check split pos)
 */
BOOL SetClientFocus (HWND hwnd, USHORT uCmd, BOOL bChild)
   {
   BOOL     fOptions, bHSplit, bVSplit;
   USHORT   uClientId, uTmp1, uTmp2, x, y;
   HWND     hwndFrame, hwndFocus;
   SHORT    sPos;

   /*--- get some window metrics ---*/
   fOptions  = LWQuery (hwnd, LWM_Options, bChild);
   hwndFrame = (bChild ? WinQueryWindow (hwnd, QW_PARENT, FALSE) : hwnd);
   bHSplit   = (fOptions & LWS_HSPLIT) == LWS_HSPLIT;
   bVSplit   = (fOptions & LWS_VSPLIT) == LWS_VSPLIT;

   if (uCmd == LWF_DEFAULT)
      {
      uClientId = LWID_CLIENT [!(fOptions & LWS_SPLITATTOP)][0];
      return WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwndFrame, uClientId));
      }

   /*--- check for bad states ---*/
   if (!bHSplit && (uCmd == LWF_UP || uCmd == LWF_DOWN))
      return FALSE;
   if (!bVSplit && (uCmd == LWF_LEFT || uCmd == LWF_RIGHT))
      return FALSE;
   if (!bVSplit && !bHSplit)
      uCmd = LWF_SPECIFIED;
   if (uCmd == LWF_SPECIFIED && !bChild)
      return FALSE;
   
   hwndFocus = (uCmd==LWF_SPECIFIED ? hwnd: WinQueryFocus(HWND_DESKTOP, 0));
   WndGetIds (hwndFocus, &uClientId, &uTmp1, &uTmp2);
   /*--- get origional client location ---*/
   y = !!(uClientId == 0x000B || uClientId == 0x000C);
   x = !!(uClientId == 0x000A || uClientId == 0x000C);

   /*--- adjust location depending on message ---*/
   y = (uCmd == LWF_UP    || uCmd == LWF_DOWN ? !!(uCmd == LWF_UP)    : y);
   x = (uCmd == LWF_RIGHT || uCmd == LWF_LEFT ? !!(uCmd == LWF_RIGHT) : x);

   if (uCmd == LWF_NEXT)
      {
      if (bVSplit && bHSplit)
         uTmp1=y, y=!x, x=uTmp1;
      else if (bHSplit)
         y = !y;
      else if (bVSplit)
         x = !x;
      }

   /*--- make sure active client is visible ---*/
   if (bHSplit && (sPos = SplitBarUse (hwndFrame, LWID_HSPLIT, &uTmp2)))
      y = !(sPos == -1);
   if (bVSplit && (sPos = SplitBarUse (hwndFrame, LWID_VSPLIT, &uTmp2)))
      x = (sPos == -1);

   return WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwndFrame, LWID_CLIENT [y][x]));
   }







/*****************************************************************/
/*                                                               */
/*                    Validation Procedures                      */
/*                                                               */
/*****************************************************************/

int GetUpdateRange (HWND hwnd, PRECTL prclUpdate, USHORT uYScrPos,
                USHORT *puFirst, USHORT *puLast,  USHORT *puInc)
   {
   USHORT      uWndTop, uMax;

   *puInc  = LWQuery (hwnd, LWM_YRowSize, TRUE);
   uWndTop = LWQuery (hwnd, LWM_YWindowSize, TRUE);
   *puFirst= uYScrPos + (uWndTop - (USHORT) prclUpdate->yTop) / (*puInc);
   *puLast = *puFirst + (USHORT)(prclUpdate->yTop -
                                prclUpdate->yBottom) / (*puInc);
   uMax = LWQuery (hwnd, LWM_NumRows, TRUE) - 1;
   *puLast = min ((*puLast + 1), uMax);
   return 0;
   }




USHORT InvalidateRow (HWND hwndFrame, USHORT uRow)
   {
   SHORT    x, y;
   USHORT   uYInc, fOptions;   
   HWND     hwndClient;
   RECTL    rclClient;
   POINTS   ptsScrPos, ptsMax;

   if (uRow >= LWQuery (hwndFrame, LWM_NumRows, FALSE))
      return 0;

   fOptions =  LWQuery (hwndFrame, LWM_Options, FALSE);
   for (y = 0; y < ((fOptions & LWS_HSPLIT) == LWS_HSPLIT ? 2 : 1); y++)
      {
      for (x = 0; x < ((fOptions & LWS_VSPLIT) == LWS_VSPLIT ? 2 : 1); x++)
         {
         hwndClient = WinWindowFromID (hwndFrame, LWID_CLIENT[1-y][x]);
         GetScrollPos (hwndClient, &ptsScrPos, &ptsMax);
         if ((USHORT)ptsScrPos.y > uRow)                    /*-up too high-*/
            continue;
         WinQueryWindowRect (hwndClient, &rclClient);
         if (rclClient.xRight == 0L | rclClient.yTop == 0L) /*-win not shown-*/
            continue;

         uYInc = LWQuery (hwndClient, LWM_YRowSize, TRUE);
         if (uRow > (USHORT)ptsScrPos.y + (USHORT)rclClient.yTop / uYInc)
            continue;                                       /*-row to low-*/

         rclClient.yTop   -= (uRow - ptsScrPos.y) * uYInc;
         rclClient.yBottom = rclClient.yTop - uYInc;

         WinInvalidateRect (hwndClient, &rclClient, FALSE);
         }
      }
   return 0;
   }





/* this routine returns 0xFFFF if the item is visible
 * or the abs scroll value that will just make it visible
 */
USHORT RowVisible (HWND hwndClient, USHORT uRow)
   {
   POINTS   ptsScrPos, ptsMax;
   USHORT   uWndSize, uYRowSize;
   RECTL    rclClient;

   GetScrollPos (hwndClient, &ptsScrPos, &ptsMax);

   /* row above window */
   if ((USHORT)ptsScrPos.y > uRow)
      return uRow;

   WinQueryWindowRect (hwndClient, &rclClient);
   uYRowSize = LWQuery (hwndClient, LWM_YRowSize, TRUE);
   uWndSize = (USHORT)rclClient.yTop / uYRowSize;

   /* select bottom if more than half visible */
   if ((USHORT)rclClient.yTop % uYRowSize > uYRowSize /2)
      uWndSize += 1;

   /* row below window */
   if ((USHORT)ptsScrPos.y + uWndSize <= uRow)
      return uRow - uWndSize + 1;

   /* row visible */
   return 0xFFFF;
   }



USHORT UpdateWindowsIfNeeded (HWND hwndFrame, USHORT uFirst, USHORT uLast)
   {
   USHORT i;

   for (i = uFirst; i <= uLast; i++)
      InvalidateRow (hwndFrame, i);
   return 0;
   }





/*****************************************************************/
/*                                                               */
/*                    Active Row Procedures                      */
/*                                                               */
/*****************************************************************/

/*
 * This function deactivated uOld and activates uNew
 * invalidating the correct rows
 * This proc also ensures that the active row is at least half visible 
 */
USHORT SetNewActive (HWND hwndClient, USHORT uVSBID, USHORT uOld, USHORT uNew)
   {
   USHORT uScroll;
   HWND   hwndFrame;

   hwndFrame = WinQueryWindow (hwndClient, QW_PARENT, FALSE);

   (LWGetPLWDAT (hwndFrame, FALSE))->uActive = uNew;
   InvalidateRow (hwndFrame, uOld);
   WinUpdateWindow (hwndClient);

   if ((uScroll = RowVisible (hwndClient, uNew)) != 0xFFFF)
      DoVScroll (hwndFrame, MPFROMSHORT (uVSBID),
                  MPFROM2SHORT (uScroll, SB_LWTRACK));

   InvalidateRow (hwndFrame, uNew);
   return uNew;
   }



/*****************************************************************/
/*                                                               */
/*                 Selected Rows Procedures                      */
/*                                                               */
/*****************************************************************/

/*--- Does NO Bounds or error checking ---*/
void LWSelectElement (HWND hwndClient, USHORT i, BOOL bSelect)
   {
   HWND hwndFrame;

   if (bSelect)
      LWGetSel (hwndClient, TRUE)[i/16] ^= 1 << i%16;
   else
      LWGetSel (hwndClient, TRUE)[i/16] &= ~(1 << i%16);

   hwndFrame = WinQueryWindow (hwndClient, QW_PARENT, FALSE);
   InvalidateRow (hwndFrame, i);
   }


static void SelectSubRange (HWND   hwndClient,
                            USHORT uAnchor,
                            USHORT uNew,
                            USHORT uOld)
   {
   BOOL  bBelow, bDown;

   /*--- both uNew and uOld on same side of anchor ---*/
   bDown  = (uOld < uNew);            // true = down
   bBelow = (uOld+bDown > uAnchor);   // true = selection below anchor
   LWSelectRange (hwndClient, uOld, uNew, !(bBelow == bDown));
   if (bBelow != bDown)
      LWSelectElement (hwndClient, uNew, TRUE);
   }




/*
 * This fn inverts the selection state of a range of rows
 * This fn assumes the multi select option is valid
 * uModes:
 *    ITI_SELECT
 *    ITI_UNSELECT
 *    ITI_INVERT
 */
USHORT LWSelectRange (HWND   hwndClient,
                      USHORT uStart,
                      USHORT uEnd,
                      USHORT uMode)
   {
   USHORT uRows, i, j;
   USHORT *puSel, uTmp;
   USHORT uStartWord, uStartBit;
   USHORT uEndWord,   uEndBit;
   HWND   hwndFrame;
   BOOL   bSelected;

   hwndFrame = WinQueryWindow (hwndClient, QW_PARENT, FALSE);
   uRows = LWQuery (hwndFrame, LWM_NumRows, FALSE);
   puSel = LWGetSel (hwndFrame, FALSE);

   if (uStart > uEnd)
      uTmp=uStart, uStart=uEnd, uEnd=uTmp;

   uStartWord = (uStart = max (0, min (uRows-1, uStart))) / 16;
   uEndWord   = (uEnd   = max (0, min (uRows-1, uEnd))) / 16;

   if (uEnd - uStart > 50)
      WndSetPtr (SPTR_WAIT);

   for (i = uStartWord; i <= uEndWord; i++)
      {
      uStartBit = (i == uStartWord ? uStart % 16 : 0);
      uEndBit   = (i == uEndWord   ? uEnd   % 16 : 15);

      for (j = uStartBit; j <= uEndBit; j++)
         {
         bSelected = (puSel[i] & 1 << j);
         switch (uMode)
            {
            case ITI_SELECT:
               puSel[i] |=   1 << j;
               if (!bSelected)
                  InvalidateRow (hwndFrame, i*16+j);
               break;
            case ITI_UNSELECT:
               puSel[i] &= ~(1 << j);
               if (bSelected)
                  InvalidateRow (hwndFrame, i*16+j);
               break;
            case ITI_INVERT:
               puSel[i] ^=   1 << j;
               InvalidateRow (hwndFrame, i*16+j);
               break;
            }
         }
      }
   WndSetPtr (SPTR_ARROW);
   return 0; 
   }



/* assumes:
 *        uNew != uOld
 *        uNew, uOld, uAnchor are valid values
 *
 * uAnchor - base point of current selection
 * uOld    - start of range to add to selection
 * uNew    - end of range to add to selection
 *
 */
void LWSelectBoundedRange (HWND   hwndClient,
                           USHORT uAnchor,
                           USHORT uNew,
                           USHORT uOld)
   {
   USHORT a, b;

   a = min (uNew, uOld);
   b = max (uNew, uOld);
   if (b - a > 50)
      WndSetPtr (SPTR_WAIT);

   if (a < uAnchor && b > uAnchor)
      {
      /*--- range above anchor ---*/
      SelectSubRange (hwndClient, uAnchor, a, uAnchor);

      /*--- range below anchor ---*/
      SelectSubRange (hwndClient, uAnchor, uAnchor, uOld);
      WndSetPtr (SPTR_ARROW);
      return;
      }

   /*--- both uNew and uOld on same side of anchor ---*/
   SelectSubRange (hwndClient, uAnchor, uNew, uOld);
   WndSetPtr (SPTR_ARROW);
   }

/*
 * This fn un-selects all rows
 */
USHORT LWDeselectAll (HWND hwnd, BOOL bChild)
   {
   USHORT sRows, i, j;
   USHORT *puSel;
   HWND hwndFrame;

   hwndFrame = (bChild ? WinQueryWindow (hwnd, QW_PARENT, FALSE) : hwnd);
   sRows = LWQuery (hwndFrame, LWM_NumRows, FALSE);
   puSel = LWGetSel (hwndFrame, FALSE);

   for (i = sRows/16+1; i > 0; i--)
      {
      for (j = 0; j < 16; j++)
         if (puSel[i-1] & 1 << j)
            InvalidateRow (hwndFrame, (i-1)*16 +j);
      puSel[i-1] = 0;
      }
   return 0;
   }








BOOL LWIsSelected (HWND hwndClient, USHORT i)
   {
   if (i == 0xFFFF)
      return FALSE;
   return !!(LWGetSel (hwndClient, TRUE)[i/16] & 1 << i%16);
   }




USHORT EXPORT ItiWndQuerySelection (HWND hwndFrame, USHORT uStart)
   {
   USHORT uRows, i, j;
   USHORT *puSel;
   USHORT uStartWord, uStartBit;
   USHORT uEndWord,   uEndBit;

   uRows = LWQuery (hwndFrame, LWM_NumRows, FALSE);
   puSel = LWGetSel (hwndFrame, FALSE);

   if (uStart >= uRows)
      return 0xFFFF;

   uStartWord = uStart / 16;
   uEndWord   = (uRows-1)  / 16;

   for (i = uStartWord; i <= uEndWord; i++)
      {
      if (!puSel[i])
         continue;

      uStartBit = (i == uStartWord ? uStart % 16 : 0);
      uEndBit   = (i == uEndWord   ? (uRows-1)  % 16 : 15);
      for (j = uStartBit; j <= uEndBit; j++)
         if (puSel[i] & (1 << j))
            return i * 16 + j;
      }
   return 0xFFFF;
   }




USHORT EXPORT ItiWndQueryBackSelection (HWND hwndFrame, USHORT uStart)
   {
   USHORT uRows, i, j;
   USHORT *puSel;
   USHORT uStartWord, uStartBit;

   uRows = LWQuery (hwndFrame, LWM_NumRows, FALSE);
   puSel = LWGetSel (hwndFrame, FALSE);

   uStart = min (uStart, uRows-1);
   uStartWord = uStart / 16;

   for (i = uStartWord+1; i > 0; i--)
      {
      if (!puSel[i-1])
         continue;

      uStartBit = (i-1 == uStartWord ? uStart % 16 : 0);
      for (j = uStartBit+1; j > 0; j--)
         if (puSel[i-1] & (1 << (j-1)))
            return (i-1) * 16 + (j-1);
      }
   return 0xFFFF;
   }
