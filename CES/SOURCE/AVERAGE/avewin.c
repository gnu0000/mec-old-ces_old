/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1992 by AASHTO.  All Rights Reserved.                 |
|                                                                           |
|       This program module is part of BAMS/CES, a module of BAMS,          |
|       The American Association of State Highway and Transportation        |
|       Officials' (AASHTO) Information System for Managing Transportation  |
|       Programs, a proprietary product of AASHTO, no part of which may be  |
|       reproduced or transmitted in any form or by any means, electronic,  |
|       mechanical, or otherwise, including photocopying and recording      |
|       or in connection with any information storage or retrieval          |
|       system, without permission in writing from AASHTO.                  |
|                                                                           |
+--------------------------------------------------------------------------*/


/*
 * avewin.c
 */

#define INCL_GPI
#define INCL_WIN
#include <math.h>
#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"
#include "..\include\itifmt.h"
#include "..\include\itiwin.h"
#include "..\include\itiest.h"
#include "..\include\winid.h"
#include "..\include\colid.h"
#include "..\include\itiglob.h"
#include "..\include\itiimp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <float.h>
#include "average.h"
#include "initdll.h"
#include "avewin.h"

#define COORD_CHRRNG  TL | CHR | X0ABS | Y0ABS | X1RNG | Y1RNG



/*
 *  This structure is for window specific data and is stored/retrieved
 *  from ItiWnd{Get|Set}Extra functions
 *  ----dCost values----
 *  1 - all non specified
 *  2 - 00-05
 *  3 - 05-25
 *  4 - 25-50
 *  5 - 50-75
 *  6 - 75-95
 *  7 - 95-100
 */

typedef struct
   {
   double   dQuan[5];   /*--- 0=5%  1=25%  2=50%  3=75%  4=95%  ---*/
   double   dCost[8];   /*--- see above ---*/
   double   dMinQuan;   /*--- graph ranges ---*/
   double   dMaxQuan;
   double   dMinCost;
   double   dMaxCost;
   double   dCurrCost;  /*--- active graph point ---*/
   double   dCurrQuan;
   BOOL     bOk;
   BOOL     bStarted;
   BOOL     bBaseDateOnly; /*--- set to keep combos from calling GetData ---*/
   } AVEWINDAT;
typedef AVEWINDAT *PAVEWINDAT;



USHORT AveErr (PSZ psz, USHORT uId)
   {
   char  szTmp [1024], szTmp2 [1024];;

   sprintf (szTmp2, psz, uId);
   strcpy  (szTmp,  "AVERAGE:AveWin.c\n");
   strcat  (szTmp, szTmp2);
   ItiErrWriteDebugMessage (szTmp);
   WinMessageBox (pglobals->hwndDesktop, pglobals->hwndAppFrame, szTmp,
                  "Average Window Error", 0, MB_OK | MB_APPLMODAL);
   return FALSE;
   }





/*-- this function maps a x,y graph point to a physical  --*/
/*--     display pixel position on the window            --*/

void SetPt (PPOINTL ppt, RECTL rcl, PAVEWINDAT pawd, double x, double y)
   {
   double dQuanRange, dCostRange, dXRange, dYRange;

   dQuanRange = pawd->dMaxQuan - pawd->dMinQuan;  /*--- data ranges  ---*/
   dCostRange = pawd->dMaxCost - pawd->dMinCost;
   dXRange    = (double) (rcl.xRight - rcl.xLeft);           /*--- pixel ranges ---*/
   dYRange    = (double) (rcl.yTop - rcl.yBottom);

   ppt->x = (long) ((double)rcl.xLeft +
          ((x - pawd->dMinQuan) / (dQuanRange ? dQuanRange : 1)) * dXRange);

   ppt->y = (long)((double)rcl.yBottom +
          ((y - pawd->dMinCost) / (dCostRange ? dCostRange : 1)) * dYRange);

   }


void PaintPemethAvgGraph (HWND hwnd, BOOL bJob)
   {
   HPS         hps;
   RECTL       rcltmp, rcl = {17, 5, 50, 9};
   POINTL      pt, pta;
   PAVEWINDAT  pawd;
   char        szQuantity [32];
   int         i;
   LONG        lSaveY [5];
   SIZEF       sfTextSize;
   GRADIENTL   glTextAngle;
   DOUBLE      dTemp;

   if (!bJob)
      rcl.yBottom += 2;

   ItiWndPrepRcl (hwnd, COORD_CHRRNG, &rcl);
   WinInvalidateRect (hwnd, &rcl, 1);

   pawd = (PAVEWINDAT) ItiWndGetExtra (hwnd, ITI_STATIC);
   hps  = WinBeginPaint (hwnd, (HPS)NULL, &rcltmp);

   WinDrawBorder (hps, &rcl, 2, 2, CLR_BLACK, CLR_BLACK, DB_STANDARD);

   GpiSetColor (hps, CLR_BLUE);

   if (pawd && pawd->bOk)
      {
      SetPt (&pt, rcl, pawd, pawd->dQuan[0], pawd->dCost[3]);
      lSaveY [0] = pt.y;
      GpiMove (hps, &pt);

      if (pawd->dMaxQuan - pawd->dMinQuan)
         {
         /*--- 5-25% bar ---*/
         SetPt (&pt, rcl, pawd, pawd->dQuan[1], pawd->dCost[3]);
         GpiLine (hps, &pt);
         SetPt (&pt, rcl, pawd, pawd->dQuan[1], pawd->dCost[4]);
         lSaveY [1] = pt.y;
         GpiLine (hps, &pt);


         /*--- 25-50% bar ---*/
         SetPt (&pt, rcl, pawd, pawd->dQuan[2], pawd->dCost[4]);
         GpiLine (hps, &pt);
         SetPt (&pt, rcl, pawd, pawd->dQuan[2], pawd->dCost[5]);
         lSaveY [2] = pt.y;
         GpiLine (hps, &pt);

         /*--- 50-75% bar ---*/
         SetPt (&pt, rcl, pawd, pawd->dQuan[3], pawd->dCost[5]);
         GpiLine (hps, &pt);
         SetPt (&pt, rcl, pawd, pawd->dQuan[3], pawd->dCost[6]);
         lSaveY [3] = pt.y;
         GpiLine (hps, &pt);

         /*--- 75-95% bar ---*/
         SetPt (&pt, rcl, pawd, pawd->dQuan[4], pawd->dCost[6]);
         lSaveY [4] = pt.y;
         GpiLine (hps, &pt);

         GpiSetCharMode  (hps, CM_MODE3);                                                     
         GpiSetColor     (hps, CLR_NEUTRAL);

         GpiQueryCharBox (hps, &sfTextSize);
         sfTextSize.cx = MAKEFIXED (FIXEDINT (sfTextSize.cx) * .9, 0);
         sfTextSize.cy = MAKEFIXED (FIXEDINT (sfTextSize.cy) * .9, 0);
         GpiSetCharBox (hps, &sfTextSize);

         glTextAngle.x = 4;
         glTextAngle.y = 4;
         GpiSetCharAngle (hps, &glTextAngle);

         for (i = 1 ; i < 4 ; i++)
            {
            if (lSaveY[i] != lSaveY[i-1])
               {
               dTemp = (pawd->dCost[i+2] > pawd->dCost[i+3] ?
                        pawd->dCost[i+2] : pawd->dCost[i+3]);
               SetPt (&pt, rcl, pawd, pawd->dQuan[i], dTemp);
               sprintf(szQuantity, "%.3f", pawd->dQuan[i]);
               GpiCharStringAt(hps, &pt, strlen (szQuantity), szQuantity);
               }
            }
         GpiSetColor (hps, CLR_BLUE);

         }
      else
         {
         pt.x = rcl.xRight;
         GpiLine (hps, &pt);
         }


      /*--- try filling under it ---*/
      GpiSetPattern (hps, PATSYM_DIAG1);

      if (pawd->dMaxQuan - pawd->dMinQuan)
         {
         SetPt (&pt, rcl, pawd, pawd->dMinQuan, pawd->dMinCost);
         GpiMove (hps, &pt);
         SetPt (&pta, rcl, pawd, pawd->dQuan[1], pawd->dCost[3]);
         GpiBox (hps, DRO_FILL, &pta, 0, 0);

         SetPt (&pt, rcl, pawd, pawd->dQuan[1], pawd->dMinCost);
         GpiMove (hps, &pt);
         SetPt (&pta, rcl, pawd, pawd->dQuan[2], pawd->dCost[4]);
         GpiBox (hps, DRO_FILL, &pta, 0, 0);

         SetPt (&pt, rcl, pawd, pawd->dQuan[2], pawd->dMinCost);
         GpiMove (hps, &pt);
         SetPt (&pta, rcl, pawd, pawd->dQuan[3], pawd->dCost[5]);
         GpiBox (hps, DRO_FILL, &pta, 0, 0);

         SetPt (&pt, rcl, pawd, pawd->dQuan[3], pawd->dMinCost);
         GpiMove (hps, &pt);
         SetPt (&pta, rcl, pawd, pawd->dQuan[4], pawd->dCost[6]);
         GpiBox (hps, DRO_FILL, &pta, 0, 0);
         }
      else
         {
         SetPt (&pta, rcl, pawd, pawd->dQuan[0], pawd->dCost[3]);
         GpiMove (hps, &pta);

         pta.x = rcl.xRight;
         pta.y = rcl.yBottom;

         GpiBox (hps, DRO_FILL, &pta, 0, 0);
         }
      }
   WinEndPaint (hps);
   }


/*************************************************************************/
/*************************************************************************/


void GrRng (double *pdTop, double *pdBot)
   {
   USHORT i, j;
   double d;
   BOOL  bTopSign, bBotSign;

   if (*pdTop == *pdBot)
      {
      *pdTop *= 2.0;
      *pdBot  = 0;
      }
   else
      {
      *pdTop *= 1.10;
      *pdBot *= 0.90;
      }

   bTopSign = *pdTop > 0;
   bBotSign = *pdBot > 0;

   d = fabs (*pdTop);
   if (d > 1.0)
      {
      for (i=0; d > 10; i++) d /= 10;
      d = ceil (d);
      for (j=0; j<i; j++)    d *= 10;
      }
   else if (d > 0)
      {
      for (i=0; d < 10; i++) d *= 10;
      d = ceil (d);
      for (j=0; j<i; j++)    d /= 10;
      }
   *pdTop = (bTopSign ? d : 0-d);

   d = fabs (*pdBot);
   if (d > 1.0)
      {
      for (i=0; d > 10; i++) d /= 10;
      d = floor (d);
      for (j=0; j<i; j++)    d *= 10;
      }
   else if (d > 0)
      {
      for (i=0; d < 10; i++) d *= 10;
      d = floor (d);
      for (j=0; j<i; j++)    d /= 10;
      }
   *pdBot = (bBotSign ? d : 0-d);
   }




PSZ CurrentComboVal (HWND hwnd, USHORT uComboID, BOOL bKey)
   {
   HWND   hwndC;
   USHORT i;
   PSZ    psz;

   if (!(hwndC = WinWindowFromID (hwnd, uComboID)))
      return NULL;
   if (LIT_NONE == (i = (UM) WinSendMsg (hwndC, LM_QUERYSELECTION, 0, 0)))
      return NULL;
   if (!(psz = WinSendMsg (hwndC, LM_QUERYITEMHANDLE, MPFROMSHORT (i), 0)))
      return NULL;
   return psz;
   }

/*
 * uType Values
 *
 * 0 - no formatting
 * 1 - format as money value
 * 2 - format as number
 * 3 - crash and burn
 */
void SetLabel (HWND hwnd, USHORT uLabel, PSZ pszNew, USHORT uType)
   {
   HHEAP hheap;
   char  szTmp [128];

   hheap  = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);

   switch (uType)
      {
      case 0:
         SWL (hwnd, ITIWND_LABEL, uLabel, pszNew);
         return;

      case 1:
         /*--- $ ---*/
         ItiFmtFormatString (pszNew, szTmp, 128, "Number,$,..", NULL);
         break;

      case 2:
         /*--- , ---*/
         ItiFmtFormatString (pszNew, szTmp, 128, "Number,,...", NULL);
         break;
      }
   SWL (hwnd, ITIWND_LABEL, uLabel, szTmp);
   }




/*
 * This procedure:
 *   ex 1 query for percentiles + import date and area type key)
 *   ex 1 query for each quan level to get unit prices
 *
 *     where the keys for these querys come from depend on what 
 *     window is up. (job vs cat)
 */
static BOOL GetData (HWND hwnd, BOOL bJob)
   {
   PAVEWINDAT pawd;
   PSZ    pszSIK, pszBD, pszWTK, pszAK, pszATK, pszJK, pszID, pszAID;
   char   szRaw [1024], szQuery [1024];
   USHORT i, uCols, uState;
   HBUF   hbuf;
   HHEAP  hheap;
   PPSZ   ppsz;
   HQRY   hqry;
   BOOL   bOk;


   hbuf   = (HBUF) QW (hwnd, ITIWND_BUFFER, 0, 0, 0);
   hheap  = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
   pawd   = (PAVEWINDAT) ItiWndGetExtra (hwnd, ITI_STATIC);
   pszSIK = QW (hwnd, ITIWND_DATA, 0, 0, cStandardItemKey);

   pawd->bOk = FALSE;

   if (bJob)
      {
      pszBD  = QW (hwnd, ITIWND_DATA, 0, 0, cBaseDate);
      pszWTK = QW (hwnd, ITIWND_DATA, 0, 0, cWorkType);
      pszJK  = QW (hwnd, ITIWND_DATA, 0, 0, cJobKey);
      }
   else
      {
      bOk = TRUE;
      if (!(pszBD  = CurrentComboVal (hwnd, PABaseDateC, TRUE)))
         bOk = AveErr ("Null Combo Val for BaseDate (Fn GetData)", 0);
      if (!(pszWTK = CurrentComboVal (hwnd, PAWorkTypeC, TRUE)))
         bOk = AveErr ("Null Combo Val for Work Type Key (Fn GetData)", 0);
      if (!(pszAK  = CurrentComboVal (hwnd, PAAreaC, TRUE)))
         bOk = AveErr ("Null Combo Val for AreaKey (Fn GetData)", 0);
      if (!bOk)
         return FALSE;
      }

   ItiMbQueryQueryText (hmod, ITIRID_WND, PemethAvgS2, szRaw, sizeof szRaw);
   sprintf (szQuery, szRaw, pszSIK, pszBD);
   if (!(hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &uCols, &uState)))
      return AveErr ("Query Exec Error %d", PemethAvgS2);
   ItiDbGetQueryRow (hqry, &ppsz, &uState);

   if (!ppsz)
      {
      sprintf (szRaw, "Insufficient data for this item \n %s", szQuery);
      return AveErr (szRaw, PemethAvgS2);
      }

   pszID  = ItiStrDup (hheap, ppsz[0]); /*-- ImportDate,  Must be freed  --*/
   pszATK = ItiStrDup (hheap, ppsz[1]); /*-- AreaTypeKey, Must be freed --*/

   /*-- Get Percentile Quantities ---*/
   pawd->dQuan[0] = ItiStrToDouble (ppsz[2]);
   pawd->dQuan[1] = ItiStrToDouble (ppsz[3]);
   pawd->dQuan[2] = ItiStrToDouble (ppsz[4]);
   pawd->dQuan[3] = ItiStrToDouble (ppsz[5]);
   pawd->dQuan[4] = ItiStrToDouble (ppsz[6]);
   ItiFreeStrArray (hheap, ppsz, uCols);

   /*--- clean up after querying --*/
   while (ItiDbGetQueryRow (hqry, &ppsz, &uState))
      ItiFreeStrArray (hheap, ppsz, uCols);

   /*--- Job has to figure out the area key ---*/
   if (bJob)
      {
      pszAK  = ItiEstGetJobArea (hheap, pszJK, pszATK, pszBD);
      /*
       * We have to do some work to display the area and the import date
       * beause we don't get this data from the static window query
       */

      /*--- set Area field in job window ---*/
      sprintf (szQuery, "SELECT AreaID from Area WHERE AreaTypeKey=%s AND AreaKey=%s", pszATK, pszAK);
      pszAID = ItiDbGetRow1Col1 (szQuery, hheap, 0, 0, 0);
      SetLabel (hwnd, 6, pszAID, 0);     /*-- area        --*/
      ItiMemFree (hheap, pszAID);

      /*--- set Import Date field in job window ---*/
      ItiFmtFormatString (pszID, szRaw, sizeof szRaw, "DateTime,mm/dd/yyyy hh:nn:ssa", NULL);
      SetLabel (hwnd, 7, szRaw, 0);     /*-- import date --*/
      }

   /*--- Calculate minimum and maximum quantities ---*/
   if ((pawd->dMinQuan = pawd->dQuan[0]) < 0)
      pawd->dMinQuan = 0;
   pawd->dMaxQuan = pawd->dQuan[4];

   /*---Diaplay Quan at bottom of graph---*/
   sprintf (szRaw, "%.3lf", pawd->dMinQuan);
   SetLabel (hwnd, 0, szRaw, 2);

   sprintf (szRaw, "%.3lf", (pawd->dMaxQuan + pawd->dMinQuan) / 2);
   SetLabel (hwnd, 1, szRaw, 2);

   sprintf (szRaw, "%.3lf", pawd->dMaxQuan);
   SetLabel (hwnd, 2, szRaw, 2);



   /*--- Get Weighted Averages ---*/
   ItiMbQueryQueryText (hmod, ITIRID_WND, PemethAvgS3, szRaw, sizeof szRaw);
   sprintf (szQuery, szRaw, pszSIK, pszID, pszAK, pszWTK);

   if (!(hqry = ItiDbExecQuery (szQuery, hheap, hmod, 0, 0, &uCols, &uState)))
      return AveErr ("Query Exec Error %d", PemethAvgS3);

   for (i=0; i<8; i++)
      pawd->dCost[i] = 0;

   /*--- Get costs for each quantity range ---*/
   bOk = FALSE;
   while (ItiDbGetQueryRow (hqry, &ppsz, &uState))
      {
      bOk = TRUE;
      pawd->dCost[atoi (ppsz[0])] = ItiStrToDouble (ppsz [1]);
      ItiFreeStrArray (hheap, ppsz, uCols);
      }
   if (!bOk)
      {
      sprintf (szRaw, "Insufficient data for this item \n %s", szQuery);
      return AveErr (szRaw, PemethAvgS2);
      }

   /*--- Get Cost range ---*/
   pawd->dMinCost = DBL_MAX;
   pawd->dMaxCost = DBL_MIN;
   for (i=3; i<7; i++)
      {
      if (pawd->dCost[i] == 0)
         pawd->dCost[i] = pawd->dCost[1];

      pawd->dMinCost = (pawd->dCost[i] < pawd->dMinCost ?
                        pawd->dCost[i] : pawd->dMinCost);
      pawd->dMaxCost = (pawd->dCost[i] > pawd->dMaxCost ?
                        pawd->dCost[i] : pawd->dMaxCost);
      }

   GrRng (&pawd->dMaxCost, &pawd->dMinCost);

   /*--- Display Cost ranges ---*/
   sprintf (szRaw, "%.2lf", pawd->dMaxCost);
   SetLabel(hwnd, 3, szRaw, 1);

   sprintf (szRaw, "%.2lf", (pawd->dMaxCost + pawd->dMinCost) / 2);
   SetLabel(hwnd, 4, szRaw, 1);

   sprintf (szRaw, "%.2lf", pawd->dMinCost);
   SetLabel (hwnd, 5, szRaw, 1);

   pawd->bOk = TRUE;

   ItiMemFree (hheap, pszID);
   ItiMemFree (hheap, pszATK);

///***************************** DEBUG ******************************/
//   sprintf (szRaw, "HHEAP:%p, Avail:%ud, Size:%ud",
//            hheap, ItiMemQueryAvail (hheap), ItiMemQuerySeg (hheap));
///   AveErr (szRaw, 0);
//
   return TRUE;
   }






MRESULT EXPORT JobPemethAvgSProc (HWND    hwnd,
                                  USHORT  uMsg,
                                  MPARAM  mp1,
                                  MPARAM  mp2)
   {
   switch (uMsg)
      {
      case WM_INITQUERY:
         {
         PVOID p;
         HHEAP hheap;

         hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);

         p = ItiMemAlloc (hheap, sizeof (AVEWINDAT), 0);
         ((PAVEWINDAT)p)->bOk = FALSE;
         ItiWndSetExtra (hwnd, ITI_STATIC, p);
         break;
         }

      case WM_ITIDBBUFFERDONE:
         {
         if (!GetData (hwnd, TRUE))
            WinPostMsg (hwnd, WM_CLOSE, 0, 0);
         break;
         }

      case WM_PAINT:
         ItiWndDefStaticWndProc (hwnd, uMsg, mp1, mp2);
         PaintPemethAvgGraph (hwnd, TRUE);
         return 0;

      case WM_DESTROY:
         {
         PVOID p;
         HHEAP hheap;

         hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
         p = ItiWndGetExtra (hwnd, ITI_STATIC);
         ItiMemFree (hheap, p);
         }
      }
   return ItiWndDefStaticWndProc (hwnd, uMsg, mp1, mp2);
   }






static BOOL FillAvgCombo (HWND hwnd, USHORT uCombo, PSZ pszSIK, PSZ pszBD)
   {
   char     szTemp [512], szQuery [512], szOld [128];
   USHORT   uNumCols, uState, uSetSel, i;
   HQRY     hqry;
   HWND     hwndC;
   HHEAP    hheap;
   PPSZ     ppsz;
   PSZ      psz;

   if (!pszSIK || !*pszSIK)
      return AveErr ("Aborting combo fill due to null SIK ID:%d", uCombo);
   if (!pszBD || !*pszBD)
      return AveErr ("Aborting combo fill due to null BD key ID:%d", uCombo);

   if (!(hwndC = WinWindowFromID (hwnd, uCombo)))
      return AveErr ("Aborting combo fill, unable to find combo handle ID:%d", uCombo);

   hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);

   /*--- store the original selection if there is one ---*/
   *szOld = '\0';
   if (psz = CurrentComboVal (hwnd, uCombo, TRUE))
      strcpy (szOld, psz);

   /* delete all items */
   for (i = (UM) WinSendMsg (hwndC, LM_QUERYITEMCOUNT, 0, 0); i; i--)
      {
      psz = (PSZ) WinSendMsg (hwndC, LM_QUERYITEMHANDLE, (MPARAM)(i-1), 0);
      ItiMemFree (hheap, psz);
      }

   WinSendMsg (hwndC, LM_DELETEALL, 0, 0);

   ItiMbQueryQueryText (hmod, ITIRID_WND, uCombo, szTemp, sizeof szTemp);
   sprintf (szQuery, szTemp, pszSIK, pszBD);

   if (!(hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &uNumCols, &uState)))
      return AveErr ("Aborting combo fill due to bad query ID:%d", uCombo);

   uSetSel = i = 0;
   while (ItiDbGetQueryRow (hqry, &ppsz, &uState))
      {
      WinSendMsg (hwndC, LM_INSERTITEM,     (MPARAM) i, MPFROMP (ppsz [0]));
      WinSendMsg (hwndC, LM_SETITEMHANDLE,  (MPARAM) i, MPFROMP (psz = ppsz [1]));
      ItiMemFree (hheap, ppsz[0]);  /*--- free copied value ---*/
      ItiMemFree (hheap, ppsz);     /*--- free array, not strings ---*/

      if (psz && *szOld && !strcmp (psz, szOld))
         uSetSel = i;            /*--- restore selection if we can ---*/
      i++;
      }
   if (!i)
      return AveErr ("There is no data to put in the combobox ID:%d", uCombo);

   WinSendMsg (hwndC, LM_SELECTITEM, (MPARAM) uSetSel, (MPARAM) TRUE);
   return TRUE;
   }




MRESULT EXPORT PemethAvgSProc (HWND    hwnd,
                               USHORT  uMsg,
                               MPARAM  mp1,
                               MPARAM  mp2)
   {
   switch (uMsg)
      {
      case WM_ITIDBBUFFERDONE:
         {
         ULONG ulStyle;
         PSZ   pszSIK = NULL, pszBD = NULL;
         PVOID p;
         HHEAP hheap;

         hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);

         p = ItiMemAlloc (hheap, sizeof (AVEWINDAT), 0);
         ((PAVEWINDAT)p)->bOk = FALSE;
         ((PAVEWINDAT)p)->bStarted = FALSE;
         ((PAVEWINDAT)p)->bBaseDateOnly = FALSE;
         ItiWndSetExtra (hwnd, ITI_STATIC, p);

         ulStyle = WinQueryWindowULong (hwnd, QWL_STYLE);
         ulStyle &= ~WS_CLIPCHILDREN;
         WinSetWindowULong (hwnd, QWL_STYLE, ulStyle);
         WinInvalidateRect (hwnd, NULL, TRUE);
         WinUpdateWindow (hwnd);

         /*--- kick off the queries for the comboboxes ---*/
         /*--- WorkType Combo ---*/
         /*--- Area Combo ---*/
         /*--- Base Date Combo ---*/
         if (!(pszSIK = QW (hwnd, ITIWND_DATA, 0, 0, cStandardItemKey)))
            AveErr ("Got Null for SIK", 0);

         FillAvgCombo (hwnd, PABaseDateC, pszSIK, pszSIK); /*-- last param must be non null, bit it isn't used --*/

         if (!(pszBD  = CurrentComboVal (hwnd, PABaseDateC, TRUE)))
            AveErr ("Got Null for BD", 0);

         FillAvgCombo (hwnd, PAAreaC,     pszSIK, pszBD);    
         FillAvgCombo (hwnd, PAWorkTypeC, pszSIK, pszBD);

         if (!GetData (hwnd, FALSE))
            WinPostMsg (hwnd, WM_CLOSE, 0, 0);

         ((PAVEWINDAT)p)->bStarted = TRUE;
         WinInvalidateRect (hwnd, NULL, TRUE);
         break;
         }


      case WM_PAINT:
         ItiWndDefStaticWndProc (hwnd, uMsg, mp1, mp2);
         PaintPemethAvgGraph (hwnd, FALSE);
         return 0;


      case WM_CONTROL:
         {
         PSZ    pszSIK, pszBD;
         USHORT uCtlMsg, uCtlID;
         PAVEWINDAT  pawd;

         uCtlMsg = SHORT2FROMMP (mp1);
         uCtlID = SHORT1FROMMP (mp1);

         if ((uCtlMsg != LN_ENTER   && uCtlMsg != LN_SELECT) ||
              WinSendDlgItemMsg (hwnd, uCtlID, (USHORT) CBM_ISLISTSHOWING, 0L, 0L))
            break;

         pawd = (PAVEWINDAT) ItiWndGetExtra (hwnd, ITI_STATIC);
         /*--- dont allow this code until the combo's have been filled ---*/
         if (!pawd->bStarted)
            break;

         switch (uCtlID)
            {
            case PABaseDateC:
               pawd->bBaseDateOnly = TRUE;
               if (!(pszSIK = QW (hwnd, ITIWND_DATA, 0, 0, cStandardItemKey)))
                  AveErr ("Got Null for SIK", 0);
               if (!(pszBD  = CurrentComboVal (hwnd, PABaseDateC, TRUE)))
                  AveErr ("Got Null for BD", 0);
               FillAvgCombo (hwnd, PAAreaC,     pszSIK, pszBD);
               FillAvgCombo (hwnd, PAWorkTypeC, pszSIK, pszBD);
               if (!GetData (hwnd, FALSE))
                  WinPostMsg (hwnd, WM_CLOSE, 0, 0);
               pawd->bBaseDateOnly = FALSE;
               break;
            case PAWorkTypeC:
               if (pawd->bBaseDateOnly)
                  break;
               if (!GetData (hwnd, FALSE))
                  WinPostMsg (hwnd, WM_CLOSE, 0, 0);
               break;
            case PAAreaC:
               if (pawd->bBaseDateOnly)
                  break;
               if (!GetData (hwnd, FALSE))
                  WinPostMsg (hwnd, WM_CLOSE, 0, 0);
               break;
            }
         break;
         }

      case WM_DESTROY:
         {
         PVOID p;
         HHEAP hheap;

         hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
         p = ItiWndGetExtra (hwnd, ITI_STATIC);
         ItiMemFree (hheap, p);
         break;
         }
      }
   return ItiWndDefStaticWndProc (hwnd, uMsg, mp1, mp2);
   }

