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
 * winutil.c                                                            *
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
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\colid.h"
#include "..\include\itierror.h"
#include "winmain.h"
#include "lwmain.h"
#include "swmain.h"
#include "winutil.h"
#include <string.h>
#include <stdio.h>
#include "..\include\itiglob.h"
#include "initdll.h"

USHORT UDLGMODE = ITI_OTHER;


char ITIWND_STATICCLASS    [] = "STATICWINDOW";
char ITIWND_LISTCLASS      [] = "LISTWINDOW";
//char ITIWND_TEXTBOXCLASS [] = "TEXTBOX";


#define Pct(x,pct) ( (((LONG)(x)) * ((LONG)(pct))) / 100L)





/*  This xlates UL -> BL       I.E. normal os2 coordinates to 
 *           or UR -> BR       window coordinates and normal
 *           or BL -> UL       win coordinates to os2 coordinates
 *           or BR -> UR       within hwnd
 */
void WndOs2WinRcl (HWND hwnd, PRECTL prcl)
   {
   RECTL  rclWnd;
   LONG   tmp1;  

   WinQueryWindowRect (hwnd, &rclWnd);   /* parent 0,0,xs,ys */
   tmp1          = rclWnd.yTop - prcl->yBottom;
   prcl->yBottom = rclWnd.yTop - prcl->yTop;
   prcl->yTop    = tmp1;
   }




void CvtScale (PRECTL prcl, USHORT uType)
   {
   POINTL   aptl[2];

   aptl[0].x = prcl->xLeft;
   aptl[0].y = prcl->yBottom;
   aptl[1].x = prcl->xRight;
   aptl[1].y = prcl->yTop;

   /*--- First adjust Scale ---*/
   switch (ItiWndGetScale(uType))
      {
      case PIX:
         break;
      case DLG:
         WinMapDlgPoints (HWND_DESKTOP, aptl, 2, TRUE);
         break;
      case CHR:
         aptl[0].x *= pglobals->xChar; 
         aptl[0].y *= pglobals->yChar; 
         aptl[1].x *= pglobals->xChar; 
         aptl[1].y *= pglobals->yChar;
         break;
      case BLK:
         aptl[0].x = (aptl[0].x * pglobals->xChar) / 2L; 
         aptl[0].y = (aptl[0].y * pglobals->yChar) / 2L; 
         aptl[1].x = (aptl[1].x * pglobals->xChar) / 2L; 
         aptl[1].y = (aptl[1].y * pglobals->yChar) / 2L;
      }
   if (ItiWndGetX0(uType) != X0PCT) prcl->xLeft   = (SHORT)aptl[0].x;
   if (ItiWndGetY0(uType) != Y0PCT) prcl->yBottom = (SHORT)aptl[0].y;
   if (ItiWndGetX1(uType) != X1PCT) prcl->xRight  = (SHORT)aptl[1].x;
   if (ItiWndGetY1(uType) != Y1PCT) prcl->yTop    = (SHORT)aptl[1].y;
   }




USHORT EXPORT ItiWndPrepRcl (HWND hwnd, USHORT uType, PRECTL prcl)
   {
   RECTL    rclWnd;
   LONG     x1,y1, tmp1;
   USHORT   uOrigin;

   CvtScale (prcl, uType);
   if (uType == (X0ABS | Y0ABS | X1ABS | Y1ABS | BL | PIX))
      return 0;

   WinQueryWindowRect (hwnd, &rclWnd);   /* parent 0,0,xs,ys */
   x1 = rclWnd.xRight;
   y1 = rclWnd.yTop;
   
   uOrigin = ItiWndGetOrigin(uType);

   /* convert coordinate types */

   switch (ItiWndGetX0(uType))
      {
      case X0ABS:                                          break;
      case X0REL:                                          break;
      case X0PCT: prcl->xLeft = Pct (x1, prcl->xLeft);     break;
      }
   switch (ItiWndGetY0(uType))
      {
      case Y0ABS:                                          break;
      case Y0REL:                                          break;
      case Y0PCT: prcl->yBottom = Pct (y1, prcl->yBottom); break;
      }
   switch (ItiWndGetX1(uType))
      {
      case X1ABS:                                          break;
      case X1RNG: prcl->xRight += prcl->xLeft;             break;
      case X1REL: prcl->xRight = x1 - prcl->xRight;        break;
      case X1PCT: prcl->xRight = Pct (x1, prcl->xRight);   break;
      }
   switch (ItiWndGetY1(uType))
      {
      case Y1ABS:                                          break;
      case Y1RNG: prcl->yTop += prcl->yBottom;             break;
      case Y1REL: prcl->yTop = y1 - prcl->yTop;            break;
      case Y1PCT: prcl->yTop = Pct (y1, prcl->yTop);       break;
      }
   
   /* convert origin to BL */
   if (uOrigin == TL || uOrigin == TR)
      {
      tmp1           = y1 - prcl->yBottom;
      prcl->yBottom  = y1 - prcl->yTop;
      prcl->yTop     = tmp1;
      }
   if (uOrigin == TR || uOrigin == BR)
      {
      tmp1           = x1 - prcl->xLeft;
      prcl->xLeft    = x1 - prcl->xRight;
      prcl->xRight   = tmp1;
      }
   return 0;
   }




BOOL EXPORT ItiWndIsSingleClick (MPARAM mp)
   {
   return !((LONG)mp & 0x01000000);
   }

BOOL EXPORT ItiWndIsLeftClick  (MPARAM mp)
   {
   return !((LONG)mp & 0x02000000);
   }



void EXPORT ItiWndSetDlgMode (USHORT uMode)
   {
   UDLGMODE = uMode;
   }

USHORT EXPORT ItiWndGetDlgMode (void)
   {
   return UDLGMODE;
   }






BOOL EXPORT ItiWndQueryActive (HWND hwndStatic,
                               USHORT uListId,
                               USHORT *uActive)
   {
   HWND   hwndList;
   USHORT uAct;

   if (uActive != NULL)
      *uActive = 0xFFFF;

   if ((hwndList = WinWindowFromID (hwndStatic, uListId)) == NULL)
      return FALSE;

   uAct = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);
   if (uActive != NULL)
      *uActive = uAct;

   return !(uAct == 0xFFFF);
   }




USHORT WndAssignPts (POINTS *ppts1, POINTS *ppts2,
                     short xp1, short yp1,
                     short xp2, short yp2)
   {
   ppts1->x = xp1 , ppts1->y = yp1;
   ppts2->x = xp2 , ppts2->y = yp2;
   return 0;
   }


USHORT WndAssignRcl (RECTL *rcl,   LONG xLeft, LONG yBottom,
                     LONG  xRight, LONG yTop)
   {
   rcl->xLeft   = xLeft;
   rcl->yBottom = yBottom;
   rcl->xRight  = xRight;
   rcl->yTop    = yTop;
   return 0;
   }



BOOL WndGetIds (HWND hwndClient, USHORT *puClientId,
                USHORT *puVSBId,  USHORT *puHSBId)
   {
   *puClientId = WinQueryWindowUShort (hwndClient, QWS_ID);
   *puVSBId = LWID_VSB[!!(*puClientId == 0x000B || *puClientId == 0x000C)];
   *puHSBId = LWID_HSB[!!(*puClientId == 0x000A || *puClientId == 0x000C)];
   return TRUE;
   }



BOOL WndSetPtr (SHORT sPtr)
   {
   return WinSetPointer (HWND_DESKTOP,
                         WinQuerySysPointer (HWND_DESKTOP, sPtr, 0));
   }


BOOL EXPORT ItiWndSetHourGlass (BOOL bHour)
   {
   return WndSetPtr (bHour ? SPTR_WAIT : SPTR_ARROW);
//   return TRUE;
   }






PVOID EXPORT ItiWndGetExtra (HWND hwnd, USHORT uType)
   {
   PVOID p;

   if (!(p = QW (hwnd, ITIWND_PSTRUCT, 0, 0, 0)))
      return NULL;

   switch (uType)
      {
      case ITI_STATIC:  return ((PSWDAT) p)->pExtra;
      case ITI_LIST:    return ((PLWDAT) p)->pExtra;
      case ITI_DIALOG:  return ((PDWDAT) p)->pExtra;
      }
   return NULL;
   }



BOOL EXPORT ItiWndSetExtra (HWND hwnd, USHORT uType, PVOID pData)
   {
   PVOID p;

   if (!(p = QW (hwnd, ITIWND_PSTRUCT, 0, 0, 0)))
      return FALSE;

   switch (uType)
      {
      case ITI_STATIC: ((PSWDAT) p)->pExtra = pData; return TRUE;
      case ITI_LIST:   ((PLWDAT) p)->pExtra = pData; return TRUE;
      case ITI_DIALOG: ((PDWDAT) p)->pExtra = pData; return TRUE;
      }
   return FALSE;
   }





/* This function is very much like WinDrawText.
 *
 * The only modification to the fn is that the last param is a ULONG
 * rather than a USHORT. This allows 16 extra flags for me to do 
 * whatever I want. As of now there are only 2 defined. They are:
 *
 *       ITIWND_HIDE       --- Do not actually display text
 *       ITIWND_WORDBREAK  --- Allow word breaks in the text
 *
 */

SHORT ItiWndDrawText (HPS    hps,
                      SHORT  sLen,
                      PSZ    pszTxt,
                      PRECTL prcl,
                      LONG   clrFore,
                      LONG   clrBkg,
                      ULONG  ulFlags)

   {
   SHORT i, sDrawn;
   LONG  lYChar;
   RECTL rcl;

   lYChar = pglobals->yChar;

   if (ulFlags & ITIWND_HIDE)
      return 0;

   if (!(ulFlags & ITIWND_WORDBREAK))
      return WinDrawText (hps, -1, pszTxt, prcl,
                          clrFore, clrBkg, LOUSHORT (ulFlags));

   if (sLen == -1)
      sLen = strlen (pszTxt);

   rcl = *prcl;
   for (i=0; i < sLen; rcl.yTop -= lYChar)
      {
      sDrawn = WinDrawText (hps, sLen - i, pszTxt + i,
                            &rcl, clrFore, clrBkg,
                            DT_WORDBREAK | LOUSHORT (ulFlags));
      if (!sDrawn)
         break;

      i+=sDrawn;
      }
   }





/*
 * This function should be called with the static
 * window handle.
 *
 * Returns the count of windows deleted.
 */

USHORT ItiWndAxeKids (HWND hwnd)
   {
   HENUM  henum;
   HWND   hwndFrame, hwndStatic, hwndOwner;
   USHORT i, uRet = 0;
   BOOL   bMatch;
   PSWDAT pswd;

   pswd = WinQueryWindowPtr (hwnd, WNDPTR_SWDAT);
   if (pswd == NULL)
      return 0;

   /*--- we don't wand browser windows hosing things up -clf ---*/
   if (pglobals->hwndAppClient == HWND_DESKTOP)
      return 0;

   henum = WinBeginEnumWindows (pglobals->hwndAppClient);

   while ((hwndFrame = WinGetNextWindow (henum)) != NULL)
      {
      hwndStatic = WinWindowFromID (hwndFrame, FID_CLIENT);
      hwndOwner  = (HWND) QW (hwndStatic, ITIWND_OWNERWND, 0, 0, 0);

      /*-- Notice here that we have to check both the static window and --*/
      /*-- the controls of the static because either could be the owner --*/
      bMatch = (hwndOwner == hwnd);  /*--compare owner to current client--*/
      for (i=0; !bMatch && i<pswd->uChildren; i++ )
         if (WinWindowFromID (hwnd, pswd->pchdChildren[i].uId) == hwndOwner)
            bMatch = TRUE;

      if (bMatch)
         {
//
//  not needed because killing
//
//         uRet += 1 + ItiWndAxeKids (hwndStatic);
//
         uRet += 1;
         WinDestroyWindow (hwndFrame);
         }
      }
   WinEndEnumWindows (henum);
   return uRet;
   }



/**************************************************************/
/***    The following procs are useful for 2nd level dlgs   ***/
/**************************************************************/



USHORT ItiWndSetLBHandles (HWND hwnd, USHORT uCtlId, ULONG ulHandle)
   {
   USHORT uRows, i;

   uRows = (UM) WinSendDlgItemMsg (hwnd, uCtlId, LM_QUERYITEMCOUNT, 0, 0);
   for (i=0; i<uRows; i++)
      WinSendDlgItemMsg (hwnd, uCtlId, LM_SETITEMHANDLE,
                         MPFROMSHORT (i), (MPARAM)ulHandle);
   return uRows;
   }



/*
 * returns true if item was moved
 */

USHORT ItiWndMoveSelectedItem (HWND hwnd, USHORT uCtlDest, USHORT uCtlSrc)
   {
   char     szTmp [256];
   USHORT   uRow, uNewPos;
   ULONG    ulHandle;

   uRow = (UM) WinSendDlgItemMsg (hwnd, uCtlSrc, LM_QUERYSELECTION,
                                  (MPARAM)LIT_FIRST, 0L);
   if (uRow == 0xFFFF)
      return DosBeep (800, 30);

   WinSendDlgItemMsg (hwnd, uCtlSrc, LM_QUERYITEMTEXT,
                      MPFROM2SHORT (uRow, sizeof szTmp), MPFROMP (szTmp));
   ulHandle = (ULONG) WinSendDlgItemMsg (hwnd, uCtlSrc, LM_QUERYITEMHANDLE,
                                         MPFROMSHORT (uRow), 0);

   uNewPos = (UM) WinSendDlgItemMsg (hwnd, uCtlDest, LM_INSERTITEM,
                                     MPFROMSHORT (LIT_SORTASCENDING),
                                     MPFROMP (szTmp));
   WinSendDlgItemMsg (hwnd, uCtlDest, LM_SETITEMHANDLE,
                      MPFROMSHORT (uNewPos), (MPARAM)ulHandle);

   WinSendDlgItemMsg (hwnd, uCtlSrc, LM_DELETEITEM,
                      MPFROMSHORT (uRow), 0);

   return TRUE;
   }









/*
 * Copies data from hbuf to pszResults
 * if the hbuf is a list buffer all selected rows are copied
 *
 * Format
 * ------
 *    StaticWindowId \t ColName1 \t ColName2 \t ... \n
 *    StaticWindowId \t ColData1 \t ColData2 \t ... \0
 *
 * OR
 * --
 *    ListWindow1 \t ColName1 \t ColName2 \t ...    \n
 *    ListWindow1 \t ColData1 \t ColData2 \t ...    \n
 *    ListWindow1 \t ColData1 \t ColData2 \t ...    \n
 *    .                                             .
 *    .                                             .
 *    .                                             \0
 *
 * The value returned is the length of the result string
 */
static USHORT CopySelectData (HWND hwnd,    HBUF hbuf,
                            PSZ  pszData, USHORT uMaxLen)
   {
   USHORT   uType, uId, uCols, i, uColId, uBufRow, uLen;
   char     szCol [256];
   PSZ      pszColName;

   uLen = 0;

   uType = (UM) ItiDbQueryBuffer (hbuf, ITIDB_TYPE, 0);
   uId   = (UM) ItiDbQueryBuffer (hbuf, ITIDB_ID, 0);
   uCols = (UM) ItiDbQueryBuffer (hbuf, ITIDB_NUMCOL, 0);
   if (!uCols)
      return 0;

   /*--- 1st Line Is Column names ---*/
   uLen += sprintf (pszData, "%d\t", uId);
   for (i=0; i < uCols; i++)
      {
      if (i)
         {
         strcat (pszData, "\t");
         uLen++;
         }
      uColId = (UM) ItiDbQueryBuffer (hbuf, ITIDB_COLID, i);

      if (pszColName = ItiColColIdToString (uColId))
         {
         strcat (pszData, pszColName);
         uLen += strlen (pszColName);
         }
      }
   uBufRow = 0;
   do
      {
      /*--- 2nd Line... Is Column data ---*/
      uBufRow = (UM) QW (hwnd, ITIWND_SELECTION, uBufRow, 0, 0);
      if (uBufRow == 0xFFFF)
         break;
      if (uLen > uMaxLen - 1024)
         break;

      uLen += sprintf (pszData + uLen , "\n%d\t", uId);

      for (i=0; i < uCols; i++)
         {
         if (i)
            {
            strcat (pszData, "\t");
            uLen++;
            }
         ItiDbGetBufferRowCol (hbuf, uBufRow, 0x8001 + i, szCol);
         strcat (pszData, szCol);
         uLen += strlen (szCol);
         }
      uBufRow++;
      } while (uType == ITI_LIST);
   return uLen;
   }








#define CLPSEGSIZE   0xFFFF


USHORT ItiWndCopyIt (HWND hwnd)
   {
   USHORT   i, uKids, uDataLen = 0;
   HWND     hwndKid;
   HBUF     hbuf;
   PSZ      pszData;

   if (ITI_STATIC != (UM) QW (hwnd, ITIWND_TYPE, 0, 0, 0))
      return 0;

   pszData = (PSZ) ItiMemAllocSeg (CLPSEGSIZE, SEG_GIVEABLE | SEG_SIZEABLE, 0);
   if (!pszData) return 0;

   /*--- Do Static Buffer First ---*/
   if (hbuf = (HBUF) QW (hwnd, ITIWND_BUFFER, 0, 0, 0))
      uDataLen += CopySelectData (hwnd, hbuf, pszData, CLPSEGSIZE - uDataLen);

   uKids = (UM) QW (hwnd, ITIWND_NUMKIDS, 0, 0, 0);

   /*--- Do Child Lists Next ---*/
   for (i=0; i<uKids; i++)
      {
      if (ITIWC_LIST != (UM) QW (hwnd, ITIWND_KIDTYPE, i, 0, 0))
         continue;

      hwndKid = (HWND) QW (hwnd, ITIWND_KID, i, 0, 0);
      if (hbuf = (HBUF) QW (hwndKid, ITIWND_BUFFER, 0, 0, 0))
         uDataLen += CopySelectData (hwndKid, hbuf, pszData + uDataLen,
                                     CLPSEGSIZE - uDataLen);
      }
   ItiMemReallocSeg (pszData, uDataLen + 1, 0);

   if (!WinOpenClipbrd (pglobals->habMainThread))
      return 0;

   if (!WinEmptyClipbrd (pglobals->habMainThread))
      return WinCloseClipbrd (pglobals->habMainThread);

   WinSetClipbrdData (pglobals->habMainThread, (ULONG) SELECTOROF (pszData),
                      CF_TEXT, CFI_SELECTOR);

   if (!WinCloseClipbrd (pglobals->habMainThread))
      return 0;
   return uDataLen;
   }



