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


/*
 * Scr.c
 *
 * (C) 1992 Info Tech Inc.
 * This file is part if the EBS module
 */


#define INCL_VIO
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>
#include "ebs.h"
#include "util.h"
#include "scr.h"





#define COL_LINE           1
#define COL_ITEM           6
#define COL_QUAN           38
#define COL_UNIT           40
#define COL_UNITPRICE      62
#define COL_EXT            78
#define COL_ALT            2
#define COL_DESCRIPTION    6

#define COL_SECTIONNUM     9
#define COL_SECTIONALT     25


BYTE NORMAL        = 0x07;
BYTE BRIGHT        = 0x0F;
BYTE INVERSENORMAL = 0x70;
BYTE INVERSEBRIGHT = 0x70;


BYTE   bcTL [2]  = {'É', 0x07};
BYTE   bcTR [2]  = {'»', 0x07};
BYTE   bcBL [2]  = {'È', 0x07};
BYTE   bcBR [2]  = {'¼', 0x07};
BYTE   bcH  [2]  = {'Í', 0x07};
BYTE   bcV  [2]  = {'º', 0x07};
BYTE   bcSPC[2]  = {' ', 0x07};


MET  SCREENMETRICS;

BOOL bShowSTotal = TRUE;




/********************************************************************/
/*                                                                  */
/*  Generic Screen Utilities                                        */
/*                                                                  */
/********************************************************************/


/* fills in pmet structure
 * for the screen
 * pprop valid: set ListLines and ScrollMax
 * pprop NULL:  ListLines=ScrollMax=0;
 */
USHORT InitScreenMetrics (void)
   {
   VIOCONFIGINFO vioc;
   VIOMODEINFO   viom;

   vioc.cb = sizeof (vioc);
   VioGetConfig (0, &vioc, (HVIO)NULL);
   SCREENMETRICS.uDisplay = !(vioc.display == MONITOR_MONOCHROME);

   if (SCREENMETRICS.uDisplay)
      {
      NORMAL        = 0x1E;
      BRIGHT        = 0x1F;
      INVERSENORMAL = 0x4E;
      INVERSEBRIGHT = 0x4F;

      bcTL [1] = NORMAL;
      bcTR [1] = NORMAL; 
      bcBL [1] = NORMAL;
      bcBR [1] = NORMAL;
      bcH  [1] = NORMAL;
      bcV  [1] = NORMAL;
      bcSPC[1] = NORMAL;
      }

   viom.cb = sizeof (viom);
   VioGetMode (&viom, (HVIO) NULL);
   SCREENMETRICS.uYScreen   = viom.row;

   SCREENMETRICS.uYListPos  = 7;
   SCREENMETRICS.uYListSize = (viom.row - 11) / 2;
   SCREENMETRICS.uScrollPos = 0;
   SCREENMETRICS.uSelected  = 0;
   SCREENMETRICS.uItemCount = 0;
   SCREENMETRICS.uScrollMax = 0;

   VioGetCurType (&(SCREENMETRICS.viou), (HVIO)NULL);
   return 0;
   }



//USHORT UpdateScreenMetrics (PMET pmet, PPROP pprop)
//   {
//   pmet->uItemCount = (pprop ? pprop->uItemCount : 0);
//   pmet->uScrollMax = (pprop ? max (0, (SHORT)pprop->uItemCount - (SHORT)pmet->uYListSize) : 0);
//   return 0;
//   }



/*
 * returns the hidden global pointer of the global metrics
 * NULL if error (not initialized)
 */
PMET GetScreenMetrics (void)
   {
   return &SCREENMETRICS;
   }



void ShowCursor (BOOL b)
   {
   VIOCURSORINFO viocpy;
   PMET   pmet = GetScreenMetrics ();

   if (b)
      return (void) VioSetCurType (&(pmet->viou), (HVIO)NULL);
   viocpy = pmet->viou;
   viocpy.attr = 0xFFFF;
   VioSetCurType (&viocpy, (HVIO)NULL);
   }



USHORT DrawBox (USHORT uXLeft, USHORT uYTop, USHORT uXSize, USHORT uYSize)
   {
   USHORT i, uYBottom;

   VioWrtNCell (bcTL, 1,        uYTop, uXLeft,            (HVIO)NULL);
   VioWrtNCell (bcH,  uXSize-2, uYTop, uXLeft + 1 ,       (HVIO)NULL);
   VioWrtNCell (bcTR, 1,        uYTop, uXLeft + uXSize-1, (HVIO)NULL);

   uYBottom = uYTop + uYSize - 1;
   for (i=uYTop+1; i< uYBottom; i++)
      {
      VioWrtNCell (bcV,  1,  i,  uXLeft,              (HVIO)NULL);
      VioWrtNCell (bcV,  1,  i,  uXLeft + uXSize-1,   (HVIO)NULL);
      }
   VioWrtNCell (bcBL, 1,        uYBottom, uXLeft,            (HVIO)NULL);
   VioWrtNCell (bcH,  uXSize-2, uYBottom, uXLeft + 1 ,       (HVIO)NULL);
   VioWrtNCell (bcBR, 1,        uYBottom, uXLeft + uXSize-1, (HVIO)NULL);
   return i;
   }


/*  this is used to paint a large multi line string
 */

USHORT PaintBigText (PGNUWIN pgw,    
                     USHORT  uLeft,   
                     USHORT  uTop,
                     USHORT  uRight,
                     USHORT  uBottom,
                     USHORT  uAtt,   
                     PSZ     pszText)
   {
   USHORT y,uSize;
   PSZ   pStart, pEnd, pTmp;

   y    = uTop;
   uSize = uRight - uLeft;
   pStart = pszText;
   pEnd = strchr (pszText, '\0');

   while (TRUE)
      {
      if (y > uBottom)
         return 1;
      if (!*pStart)
         return 0;

      pTmp = pEnd;
      if (pTmp - pStart >= (int)uSize)
         {
         pTmp = pStart + uSize - 1;
         while (pTmp > pStart && *pTmp != ' ')
            pTmp--;
         pTmp = (pTmp == pStart ? pStart + uSize - 1: pTmp);
         }
      PaintText2 (pgw, y++, uLeft, 0, uAtt, pStart, pTmp - pStart + 1);
      if (!*pTmp)
         return 0;
      pStart = pTmp+1;
      }
   }



/*
 *  almost lowest level screen printing
 *
 * returns count of char printed or -1 on error
 */
USHORT PaintText2(PGNUWIN pgw,      // local window ?
                  USHORT  uRow,     // 0 based row
                  USHORT  uCol,     // 0 based col
                  USHORT  uJust,    // 0=lt 1=rt 2=ctr
                  USHORT  uAtt,     // 0=normal 1=brt 2=invertnormal 3=inv brt
                  PSZ     pszText,  // \0 term string
                  USHORT  uMaxLen)  // max len to print
   {
   char     szTmp [256];
   USHORT   uLen;

   uLen = min (uMaxLen, strlen (pszText));
   strncpy (szTmp, pszText, uLen);
   szTmp[uLen] = '\0';
   return PaintText (pgw, uRow, uCol, uJust, uAtt, szTmp);
   }

/*
 *  lowest level screen printing
 *
 * returns count of char printed or -1 on error
 */
USHORT PaintText (PGNUWIN pgw,      // local window ?
                  USHORT  uRow,     // 0 based row
                  USHORT  uCol,     // 0 based col
                  USHORT  uJust,    // 0=lt 1=rt 2=ctr
                  USHORT  uAtt,     // 0=normal 1=brt 2=invertnormal 3=inv brt
                  PSZ     pszText)  // \0 term string
   {
   USHORT ucol, uLen;
   BYTE   uatt;

   uLen = strlen (pszText);

   if (pgw)
      {
      uRow += pgw->uYTop ;
      uCol += pgw->uXLeft;
      }

   switch (uJust)
      {
      case 0: ucol = uCol;                                   break;
      case 1: ucol = max (0, (SHORT)uCol - (SHORT)uLen);     break;
      case 2: ucol = max (0, (SHORT)uCol - (SHORT)uLen / 2); break;
      }

   switch (uAtt)
      {
      case 0: uatt = NORMAL;        break;
      case 1: uatt = BRIGHT;        break;
      case 2: uatt = INVERSENORMAL; break;
      case 3: uatt = INVERSEBRIGHT; break;
      }

   if (!VioWrtCharStrAtt (pszText, uLen, uRow, ucol, &uatt, (HVIO)NULL))
      return uLen;
   else
      return 0xFFFF;
   }



void ClearWin (PGNUWIN pgw)
   {
   USHORT  y;

   for (y=0; y<pgw->uYSize+2; y++)
      VioWrtNCell (bcSPC, pgw->uXSize/2+2, pgw->uYTop+y-1,
                   pgw->uXLeft-1, (HVIO)NULL);
      
   DrawBox (pgw->uXLeft, pgw->uYTop, pgw->uXSize / 2, pgw->uYSize);
   }



/* max x size = 78
 * max y size = screen rows - 2
 */

PGNUWIN CreateGnuWin (USHORT uXSize, USHORT uYSize)
   {
   PGNUWIN pgw;
   USHORT  x,y,uSiz;
   PMET   pmet;

   pmet = GetScreenMetrics ();
   uXSize *= 2;
   uSiz = uXSize + 4;

   pgw = (PGNUWIN) malloc (sizeof (GNUWIN));
   pgw->pScrDat = (PCH) malloc ((uXSize+4) * (uYSize+2) * sizeof (USHORT));

   pgw->uXLeft = (80 - uXSize/2) / 2;
   pgw->uYTop  = (pmet->uYScreen - uYSize) / 2;
   pgw->uXSize = uXSize;
   pgw->uYSize = uYSize;

   for (x=y=0; y<uYSize+2; y++, x += uSiz)
      VioReadCellStr (pgw->pScrDat+x, &uSiz, pgw->uYTop+y-1,
                      pgw->uXLeft-1, (HVIO)NULL);

   ClearWin (pgw);
   return pgw;
   }



void DestroyGnuWin (PGNUWIN pgw)
   {
   USHORT  x,y;

   if (!pgw)
      return;

   for (x=y=0; y<pgw->uYSize+2; y++, x += pgw->uXSize + 4)
      VioWrtCellStr (pgw->pScrDat+x, pgw->uXSize+4, 
                     pgw->uYTop+y-1, pgw->uXLeft-1, (HVIO)NULL);

   free (pgw->pScrDat);
   free (pgw);
   }


USHORT YWinPos(PGNUWIN pgw)
   {
   return pgw->uYTop;
   }

USHORT XWinPos(PGNUWIN pgw)
   {
   return pgw->uXLeft;
   }




