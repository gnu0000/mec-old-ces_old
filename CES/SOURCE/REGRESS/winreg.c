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
 * winreg.c
 */

#define INCL_GPI
#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itiutil.h"
#include "..\include\itifmt.h"
#include "..\include\itiwin.h"
#include "..\include\itiest.h"
#include "..\include\winid.h"
#include "..\include\colid.h"
#include "..\include\itiglob.h"
#include "..\include\itiimp.h"
#include "..\include\message.h"
#include "..\include\cesutil.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <float.h>
#include "regress.h"
#include "initdll.h"
#include "winreg.h"

#define COORD_CHRRNG  TL | CHR | X0ABS | Y0ABS | X1RNG | Y1RNG

#define POINTS   50

typedef struct
   {
   DOUBLE dQuan;
   DOUBLE dPrice;
   } POINTD;


typedef struct
   {
   POINTD  ptd[POINTS+1]; /*--- points used in graph including end points ---*/
                          /*--- the last point contains the current point ---*/
                          /*--- in quan / cost format                     ---*/  
   DOUBLE  dMinQuan;      /*--- these are endpoints of x axis as well     ---*/
   DOUBLE  dMaxQuan;

   DOUBLE  dMinCost;    /*--- these are not endpoints of y axis         ---*/
   DOUBLE  dMaxCost;    /*--- y axis is adjusted to reasonable numbers  ---*/

   /*--- The following fields are used so that we can  ---*/
   /*--- interactively set the price from the quantity ---*/
   char     szBaseDate   [32];
   char     szImportDate [32];
   DOUBLE   dBasePrice;       
   DOUBLE   dMedQuan;    
   DOUBLE   dQuantityAdj;
   DOUBLE   dAnnualInflation;
   DOUBLE   dAreaAdj;
   DOUBLE   dWorkTypeAdj;
   DOUBLE   dSeasonAdj;

   BOOL    bOk;
   } REGWINDAT;
typedef REGWINDAT *PREGWINDAT;


ENTRYFDATA    efdQuantity;

static USHORT uMBReturn = 0;



/*-- this function maps a x,y graph point to a physical  --*/
/*--     display pixel position on the window            --*/

void EXPORT GetPt (PPOINTL ppt, RECTL rcl, PREGWINDAT pawd, USHORT uIndex)
   {
   DOUBLE dQuanRange, dCostRange, dXRange, dYRange;

   dQuanRange = pawd->dMaxQuan - pawd->dMinQuan;   /*--- data ranges  ---*/
   dCostRange = pawd->dMaxCost - pawd->dMinCost;
   dXRange    = (DOUBLE) (rcl.xRight - rcl.xLeft); /*--- pixel ranges ---*/
   dYRange    = (DOUBLE) (rcl.yTop - rcl.yBottom);


   if (uIndex == 0xFFFF) /*--- bottom right request ---*/
      {
      ppt->x = (long)((DOUBLE)rcl.xLeft + dXRange); 
      ppt->y = (long)(DOUBLE)rcl.yBottom;
      return;
      }
   else if (uIndex == 0xFFFE) /*--- bottom left rwquest ---*/
      {
      ppt->x = (long)(DOUBLE)rcl.xLeft ;
      ppt->y = (long)(DOUBLE)rcl.yBottom ;
      return;
      }

   ppt->x = (long) ((DOUBLE)rcl.xLeft +
            ((pawd->ptd[uIndex].dQuan - pawd->dMinQuan) / (dQuanRange)) * dXRange);
   ppt->y = (long)((DOUBLE)rcl.yBottom +
            ((pawd->ptd[uIndex].dPrice - pawd->dMinCost) / (dCostRange)) * dYRange);
   }




DOUBLE EXPORT QuanFromX (LONG lx, RECTL rcl, PREGWINDAT pawd)
   {
   DOUBLE dQuan;

   lx -= rcl.xLeft;
   dQuan = pawd->dMinQuan +
           (pawd->dMaxQuan - pawd->dMinQuan) *
           ((DOUBLE)lx / (DOUBLE)(rcl.xRight - rcl.xLeft));
   return dQuan;
   }





void EXPORT PaintJobPemethRegGraph (HWND hwnd, BOOL bJob)
   {
   HPS         hps;
   RECTL       rcltmp, rcl = {17, 5, 50, 9};
   POINTL      pt, ptmin;
   PREGWINDAT  pawd;
   USHORT      i;

   if (!bJob)
      {
      rcl.yBottom += 1;
      rcl.yTop += 1;
      }

   ItiWndPrepRcl (hwnd, COORD_CHRRNG, &rcl);

   WinInvalidateRect (hwnd, &rcl, 1);

   hps  = WinBeginPaint (hwnd, (HPS)NULL, &rcltmp);

   WinDrawBorder (hps, &rcl, 2, 2, CLR_BLACK, CLR_BLACK, DB_STANDARD);

   if (!((pawd = (PREGWINDAT) ItiWndGetExtra (hwnd, ITI_STATIC)) && pawd->bOk))
      {
      WinEndPaint (hps);
      return;
      }


   /*--- Paint the graph ---*/

   GpiSetColor (hps, CLR_BLUE);
   GpiSetPattern (hps, PATSYM_DIAG1);
   GpiBeginArea (hps, BA_BOUNDARY);

   GetPt (&pt, rcl, pawd, 0);
   GpiMove (hps, &pt);

   for (i=1; i< POINTS; i++)
      {
      GetPt (&pt, rcl, pawd, i);
      GpiLine (hps, &pt);
      }

   GetPt (&pt, rcl, pawd, 0xFFFF);  /*--- bottom right point ---*/
   GpiLine (hps, &pt);
   GetPt (&ptmin, rcl, pawd, 0xFFFE);  /*--- bottom left point ---*/
   GpiLine (hps, &ptmin);
   GetPt (&pt, rcl, pawd, 0);       /*--- back to point 0 ---*/
   GpiLine (hps, &pt);
   GpiMove (hps, &pt);
   GpiEndArea (hps);

   /*--- paint the price line ---*/
   GpiSetColor (hps, CLR_BLACK);
//   GpiSetLineType (hps, LINETYPE_DOT);
   GetPt (&pt, rcl, pawd, POINTS);
   GpiMove (hps, &pt);
   pt.y = ptmin.y;
   GpiLine (hps, &pt);
   GetPt (&pt, rcl, pawd, POINTS);
   GpiMove (hps, &pt);
   pt.x = ptmin.x;
   GpiLine (hps, &pt);
//   GpiSetLineType (hps, LINETYPE_SOLID);

   WinDrawBorder (hps, &rcl, 2, 2, CLR_BLACK, CLR_BLACK, DB_STANDARD);
   WinEndPaint (hps);
   }



/*  sets current quan/price 
 *  in labels and in structure
 */



void EXPORT SetPrice (HWND hwnd, DOUBLE dQuan, LONG lx, LONG ly, BOOL bJob)
   {
   RECTL  rcl = {17, 5, 50, 9};
   char   szTmp1 [32], szTmp2 [32];
   PREGWINDAT  pawd;

   if (!bJob)
      {
      rcl.yBottom += 1;
      rcl.yTop += 1;
      }

   if (!((pawd = (PREGWINDAT) ItiWndGetExtra (hwnd, ITI_STATIC)) && pawd->bOk))
      return ;

   ItiWndPrepRcl (hwnd, COORD_CHRRNG, &rcl);

   if (dQuan == 0)
      {
      if (lx<rcl.xLeft || lx>rcl.xRight || ly<rcl.yBottom || ly>rcl.yTop)
         return ;
      else
         pawd->ptd[POINTS].dQuan = QuanFromX (lx, rcl, pawd);
      }
   else
      pawd->ptd[POINTS].dQuan = dQuan;

   pawd->ptd[POINTS].dPrice =
            CalcReg (pawd->szBaseDate,       pawd->szImportDate,
                     pawd->dBasePrice,       pawd->ptd[POINTS].dQuan,
                     pawd->dMedQuan,         pawd->dQuantityAdj,
                     pawd->dAnnualInflation, pawd->dAreaAdj,
                     pawd->dWorkTypeAdj,     pawd->dSeasonAdj);

   /*---  Unit Price Field In window ---*/
   sprintf (szTmp1, "%.2lf", pawd->ptd[POINTS].dPrice);
   ItiFmtFormatString (szTmp1, szTmp2, 32, "Number,$,..", NULL);
   SWL(hwnd, ITIWND_LABEL, 3, szTmp2);

   /*---  Quantity Field In window ---*/
   sprintf (szTmp1, "%.2lf", pawd->ptd[POINTS].dQuan);
   ItiFmtFormatString (szTmp1, szTmp2, 32, "Number,,...", NULL);
   SWL(hwnd, ITIWND_LABEL, 4, szTmp2);

   WinSetWindowText (WinWindowFromID (hwnd, PemethRegC5), szTmp2);

   WinInvalidateRect (hwnd, NULL, 1);
   }





/*************************************************************************/
/*************************************************************************/


void EXPORT GrRng (DOUBLE *pdTop, DOUBLE *pdBottom)
   {
   USHORT i, j;
   DOUBLE d;

   d = *pdTop;
   for (i=0; d > 10; i++)
      d /= 10;
// if (d < .5)

      d *= 1.3;
// else
//    d = ceil (d);

   for (j=0; j<i; j++)
      d *= 10;
   *pdTop = d;

   d = *pdBottom;
   for (i=0; d > 10; i++)
      d /= 10;
   d = floor (d);
   for (j=0; j<i; j++)
      d *= 10;
   *pdBottom = d;
   }


static void EXPORT GetSelectedKey (HWND    hwnd,
                            USHORT  usCombo,
                            PSZ     pszKey,
                            USHORT  usKeySize)
   {
   USHORT i;
   PSZ    psz;

   i = (UM) WinSendDlgItemMsg (hwnd, usCombo, LM_QUERYSELECTION,
                               (MPARAM) LIT_FIRST, 0);
   psz = (PSZ) WinSendDlgItemMsg (hwnd, usCombo, LM_QUERYITEMHANDLE,
                                  (MPARAM) i, 0);
   *pszKey = '\0';
   if (psz != NULL)
      ItiStrCpy (pszKey, psz, usKeySize);
   }


void EXPORT DoLabels (HWND hwnd, HBUF hbuf, BOOL bJob)
   {
   PREGWINDAT  pawd;
   HHEAP       hheap;
   USHORT      i;
   char szMinQuan         [32] = "";
   char szMaxQuan         [32] = "";
   char szMedQuan         [32] = "";
   char szQuantity        [32] = "";
   char szJobKey          [32] = "";
   char szJobBreakdownKey [32] = "";
   char szStandardItemKey [32] = "";
   char szAreaKey         [32] = "";
   char szWorkTypeKey     [32] = "";
   char szSeasonKey       [32] = "";
   char szQuantityAdj     [32] = "";
   char szAnnualInflation [32] = "";
   char szBasePrice       [32] = "";
   char szTmp1            [32] = "";
   char szTmp2            [32] = "";
   DOUBLE      dInc;
   PSZ   psz;

   if (!ItiDbQueryBuffer (hbuf, ITIDB_NUMROWS, 0))
      {
      WinMessageBox (HWND_DESKTOP, hwnd, "This window cannot be displayed "
                     "due to data errors",
                     "REGRESSION ERROR",
                      666, MB_ICONHAND | MB_MOVEABLE | MB_OK);
      WinPostMsg (hwnd, WM_CLOSE, 0, 0);
      return;
      }


   hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
   pawd  = (PREGWINDAT) ItiWndGetExtra (hwnd, ITI_STATIC);

   ItiDbGetBufferRowCol (hbuf, 0, cMinimumQuantity,    szMinQuan);
   ItiDbGetBufferRowCol (hbuf, 0, cMaximumQuantity,    szMaxQuan);
   ItiDbGetBufferRowCol (hbuf, 0, cMedianQuantity,     szMedQuan);
   ItiDbGetBufferRowCol (hbuf, 0, cBaseDate,           pawd->szBaseDate);
   ItiDbGetBufferRowCol (hbuf, 0, cImportDate,         pawd->szImportDate);
   ItiDbGetBufferRowCol (hbuf, 0, cStandardItemKey,    szStandardItemKey);
   ItiDbGetBufferRowCol (hbuf, 0, cBaseUnitPrice,      szBasePrice);
   ItiDbGetBufferRowCol (hbuf, 0, cAnnualInflation,    szAnnualInflation);
   ItiDbGetBufferRowCol (hbuf, 0, cQuantityAdjustment, szQuantityAdj);

   if (bJob)
      {
      ItiDbGetBufferRowCol (hbuf, 0, cJobKey,             szJobKey);
      ItiDbGetBufferRowCol (hbuf, 0, cJobBreakdownKey,    szJobBreakdownKey);
      ItiDbGetBufferRowCol (hbuf, 0, 0x8003,              szSeasonKey);
      ItiDbGetBufferRowCol (hbuf, 0, cQuantity,           szQuantity);
      ItiDbGetBufferRowCol (hbuf, 0, cAreaKey,            szAreaKey);
      ItiDbGetBufferRowCol (hbuf, 0, cWorkType,           szWorkTypeKey);
      }
   else
      {
      GetSelectedKey (hwnd, PemethRegC1, szSeasonKey,   sizeof szSeasonKey);
      GetSelectedKey (hwnd, PemethRegC2, szWorkTypeKey, sizeof szWorkTypeKey);
      GetSelectedKey (hwnd, PemethRegC3, szAreaKey,     sizeof szAreaKey);
      GetSelectedKey (hwnd, PemethRegC4, pawd->szBaseDate, sizeof pawd->szBaseDate);
      psz = QW (hwnd, ITIWND_LABEL, 4, 0, 0);
      if (psz == NULL || !szSeasonKey [0] || !szWorkTypeKey [0] ||
          !szAreaKey [0] || !pawd->szBaseDate [0])
         return;
      ItiStrCpy (szQuantity, psz, sizeof szQuantity);
      }

   pawd->dMinQuan         = ItiStrToDouble (szMinQuan);
   pawd->dMaxQuan         = ItiStrToDouble (szMaxQuan);
   pawd->dMinCost         = DBL_MAX;
   pawd->dMaxCost         = DBL_MIN;
   pawd->dBasePrice       = ItiStrToDouble (szBasePrice);
   pawd->dAnnualInflation = ItiStrToDouble (szAnnualInflation);
   pawd->dQuantityAdj     = ItiStrToDouble (szQuantityAdj);
   pawd->dMedQuan         = ItiStrToDouble (szMedQuan);
   pawd->dAreaAdj         = GetAdjustment (hheap, pawd->szImportDate, szStandardItemKey, 
                                     "Area", "AreaKey", szAreaKey);
   pawd->dWorkTypeAdj     = GetAdjustment (hheap, pawd->szImportDate, szStandardItemKey, 
                                     "WorkType", "WorkType", szWorkTypeKey);
   pawd->dSeasonAdj       = GetAdjustment (hheap, pawd->szImportDate, szStandardItemKey, 
                                     "Season", "Season", szSeasonKey);
   dInc = (pawd->dMaxQuan - pawd->dMinQuan) / (POINTS - 1);

   for (i=0; i<=POINTS; i++)
      {
      if (i==POINTS)
         pawd->ptd[i].dQuan = ItiStrToDouble (szQuantity);
      else
         pawd->ptd[i].dQuan = pawd->dMinQuan + (dInc * i);

      pawd->ptd[i].dPrice = CalcReg (pawd->szBaseDate,
                                     pawd->szImportDate,
                                     pawd->dBasePrice,         
                                     pawd->ptd[i].dQuan,
                                     pawd->dMedQuan,    
                                     pawd->dQuantityAdj,
                                     pawd->dAnnualInflation,
                                     pawd->dAreaAdj,
                                     pawd->dWorkTypeAdj,
                                     pawd->dSeasonAdj);

      if (pawd->dMinCost > pawd->ptd[i].dPrice)
          pawd->dMinCost = pawd->ptd[i].dPrice;
      if (pawd->dMaxCost < pawd->ptd[i].dPrice)
          pawd->dMaxCost = pawd->ptd[i].dPrice;
      }
   GrRng (&pawd->dMaxCost, &pawd->dMinCost);

   /*---  Max Cost Label ---*/
   sprintf (szTmp1, "%.2lf", pawd->dMaxCost);
   ItiFmtFormatString (szTmp1, szTmp2, 32, "Number,$,..", NULL);
   SWL(hwnd, ITIWND_LABEL, 0, szTmp2);

   /*---  Med Cost label ---*/
   sprintf (szTmp1, "%.2lf", (pawd->dMaxCost + pawd->dMinCost) / 2.0);
   ItiFmtFormatString (szTmp1, szTmp2, 32, "Number,$,..", NULL);
   SWL(hwnd, ITIWND_LABEL, 1, szTmp2);

   /*---  Min Cost Label ---*/
   sprintf (szTmp1, "%.2lf", pawd->dMinCost);
   ItiFmtFormatString (szTmp1, szTmp2, 32, "Number,$,..", NULL);
   SWL(hwnd, ITIWND_LABEL, 2, szTmp2);

   /*---  Unit Price Field In window ---*/
   sprintf (szTmp1, "%.2lf", pawd->ptd[POINTS].dPrice);
   ItiFmtFormatString (szTmp1, szTmp2, 32, "Number,$,..", NULL);
   SWL(hwnd, ITIWND_LABEL, 3, szTmp2);

   /*---  Quantity Field In window ---*/
   sprintf (szTmp1, "%.2lf", pawd->ptd[POINTS].dQuan);
   ItiFmtFormatString (szTmp1, szTmp2, 32, "Number,,...", NULL);
   SWL (hwnd, ITIWND_LABEL, 4, szTmp2);

   pawd->bOk = TRUE;
   }


   
void EXPORT ReCalcQuantity (HWND hwnd, DOUBLE dQuan)

   {
   HHEAP    hheap, hheapList;
   HWND     hwndStatic, hwndList, hwnd2 = NULL; 
   USHORT   uRow, uRet=0;
   USHORT   uKeys [] = {cJobKey, cJobItemKey, 0};
   PSZ      pszWinStandardItemKey, pszJobKey, pszJobItemKey;
   PSZ      pszDlgStandardItemKey, pszItemEstMethodKey;
   PSZ      pszQuantity;
   CHAR     szQty[32];
   CHAR     szTemp [512], szTmp[256] = "";
   CHAR     szJobKey[16], szJobItemKey[16];
   CHAR     szJBKey[16],  szItemEstMethodKey[20];
   CHAR     szDlgStdItemKey[16];
   PSZ      pszJCBEKey = NULL;
   PSZ      pszJobBreakdnKey = NULL;
   PSZ      *ppszLocalPNames, *ppszLocalParams;
   PSZ      *ppszQtyPNames, *ppszQtyParams;

   hwndStatic = (HWND) QW (hwnd, ITIWND_OWNERWND, 1, 0, 0);
   hwndList   = WinWindowFromID (hwndStatic, JobItemsL);

   if (hwndList == NULL)
      return;

   hheap = QW (hwndList, ITIWND_HEAP, 0, 0, 0);

   if (hheap == NULL)
      return;

   uRow = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);
   pszItemEstMethodKey = (PSZ) QW (hwndList, ITIWND_DATA,
                                   0, uRow, cEstimationMethodAbbreviation);

   if (ItiStrCmp (pszItemEstMethodKey, "Reg"))
      {
      sprintf (szTmp, "This Item will be recalculated using "
                      "the Regression method. Select OK if "
                      "this is what you want to do.");

      uMBReturn = WinMessageBox (HWND_DESKTOP, hwnd, szTmp,  
                                 "PEMETH Regression", 667,
                                 MB_MOVEABLE | MB_OKCANCEL | MB_DEFBUTTON2 |
                                 MB_APPLMODAL | MB_ICONEXCLAMATION);
      if (uMBReturn == MBID_CANCEL)
         return;
      }

   sprintf (szItemEstMethodKey, "1000002");
   pszItemEstMethodKey = szItemEstMethodKey;
   sprintf (szQty, "%.3f", dQuan);
   pszQuantity = szQty; 

   ItiWndSetHourGlass (TRUE);

   pszJobKey = (PSZ) QW (hwndList, ITIWND_DATA, 0, 0, cJobKey);
   ItiStrCpy(szJobKey, pszJobKey, sizeof szJobKey);


   pszWinStandardItemKey = (PSZ) QW (hwndList, ITIWND_DATA, 0, uRow, cStandardItemKey);

   pszJobItemKey = (PSZ) QW (hwndList, ITIWND_DATA, 0, uRow, cJobItemKey);

   if (pszJobItemKey == NULL)
      return;
             
   ItiStrCpy(szJobItemKey, pszJobItemKey, sizeof szJobItemKey);


   uRow = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);
   hheapList = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0);
   pszDlgStandardItemKey = (PSZ) QW (hwndList, ITIWND_DATA, 0, uRow, cStandardItemKey);
   ItiStrCpy(szDlgStdItemKey, pszDlgStandardItemKey, sizeof szDlgStdItemKey);

   hwnd2 = QW (hwndList, ITIWND_OWNERWND, 1, 0, 0);
   pszJobBreakdnKey = (PSZ) QW (hwnd2, ITIWND_DATA, 0, 0, cJobBreakdownKey);
   ItiStrCpy(szJBKey, pszJobBreakdnKey, sizeof szJBKey);

   ppszQtyPNames = ItiStrMakePPSZ (hheapList,
                                   6,
                                   "Quantity",
                                   "ItemEstimationMethodKey",
                                   "JobKey",
                                   "JobBreakdownKey",
                                   "JobItemKey",
                                   NULL);

   ppszQtyParams = ItiStrMakePPSZ (hheapList,
                                   6,
                                   szQty,
                                   szItemEstMethodKey,
                                   szJobKey,
                                   szJBKey, 
                                   szJobItemKey,
                                   NULL);

   uRet = ItiStrReplaceParams (szTemp,
                              "UPDATE JobItem "
                              " SET Quantity = %Quantity, "
                              " ItemEstimationMethodKey = %ItemEstimationMethodKey "
                              " WHERE JobKey = %JobKey "
                              " AND JobItemKey = %JobItemKey "
                              " AND JobBreakdownKey = %JobBreakdownKey ",
                              sizeof szTemp,
                              ppszQtyPNames,
                              ppszQtyParams,
                              100);
   if (uRet)
      return;

   ItiDbBeginTransaction (hheapList);
   if (ItiDbExecSQLStatement (hheapList, szTemp))
      {
      ItiDbRollbackTransaction (hheapList);
      return;
      }
   ItiDbCommitTransaction (hheapList);

   ItiDbUpdateBuffers ("JobItem");

   RecalcJobItem (hheap,
                  szJobKey,
                  szJBKey,
                  szJobItemKey,
                  pszQuantity,
                  szDlgStdItemKey,
                  pszItemEstMethodKey);

   ppszLocalPNames = ItiStrMakePPSZ (hheap, 3, "JobKey", "JobBreakdownKey", NULL);
   ppszLocalParams = ItiStrMakePPSZ (hheap, 3, szJobKey, szJBKey,  NULL);

   CalcJobItemChgJobBrkdwn (hheap, ppszLocalPNames, ppszLocalParams);
   CalcJobItemChgJob (hheap, ppszLocalPNames, ppszLocalParams);

   ItiDbUpdateBuffers ("Job");
   ItiDbUpdateBuffers ("JobBreakdown");

   ItiWndSetHourGlass (FALSE);

   }


void EXPORT DoQuantityChange (HWND hwnd, BOOL bUpdate)
   {
   DOUBLE        dQuan;
   char          szQty [128], szTmp [128];
   USHORT        uError;
   PREGWINDAT    pawd;
   HWND          hwndQty;

   if (uMBReturn)
      {
      uMBReturn = 0;
      return;
      }

   hwndQty = WinWindowFromID (hwnd, PemethRegC5);
   WinQueryWindowText (hwndQty, sizeof szQty, szQty);
   uError = ItiFmtCheckString (szQty, szTmp, sizeof szTmp,
                               "Number,,...", NULL);
   if (uError)
      uMBReturn = WinMessageBox (HWND_DESKTOP, hwnd, 
                                 "Invalid Quantity Entered",
                                 "PEMETH Regression", 668,
                                 MB_MOVEABLE | MB_OK |
                                 MB_APPLMODAL | MB_ICONHAND);
   else
      {
      pawd  = (PREGWINDAT) ItiWndGetExtra (hwnd, ITI_STATIC);
      dQuan = ItiStrToDouble (szQty);
      if (dQuan < pawd->dMinQuan || dQuan > pawd->dMaxQuan)
         {
         sprintf (szTmp,
                  "The quantity range for this item must be "
                  "between %.3f and %.3f in order to ensure "
                  "statistical validity.", 
                  pawd->dMinQuan, pawd->dMaxQuan);
         uMBReturn = WinMessageBox (HWND_DESKTOP, hwnd, szTmp, 
                                    "PEMETH Regression", 669,
                                    MB_MOVEABLE | MB_OK |
                                    MB_APPLMODAL | MB_ICONEXCLAMATION);

         }
      else
         {
         SetPrice (hwnd, dQuan, 0, 0, TRUE);

         if (bUpdate)
            ReCalcQuantity (hwnd, dQuan);
         }
      }
   return;
   }
   


MRESULT EXPORT JobPemethRegSProc (HWND    hwnd,
                                  USHORT  usMessage,
                                  MPARAM  mp1,
                                  MPARAM  mp2)
   {
   switch (usMessage)
      {
      HWND     hwndQty;
      HWND     hwndUpdate;
      case WM_INITQUERY:
         {
         PVOID p;
         HHEAP hheap;

         hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
         p = ItiMemAlloc (hheap, sizeof (REGWINDAT), 0);

         ((PREGWINDAT)p)->bOk = FALSE;
         ItiWndSetExtra (hwnd, ITI_STATIC, p);
         }
         break;

      case WM_ITIDBBUFFERDONE:
         {
         RECTL    rclQty    = {32, 3, 12, 1};
         RECTL    rclUpdate;   

         DoLabels (hwnd, mp2, TRUE);

         hwndQty = WinWindowFromID (hwnd, PemethRegC5);

         if (hwndQty)
            break;

         ItiWndPrepRcl (hwnd, COORD_CHRRNG, &rclQty);

         efdQuantity.cb = 8;
         efdQuantity.cchEditLimit = 16;
         efdQuantity.ichMinSel = 0;
         efdQuantity.ichMaxSel = 16;

         rclUpdate.xLeft   = rclQty.xLeft   + 150;
         rclUpdate.yBottom = rclQty.yBottom -   6;
         rclUpdate.xRight  = rclQty.xRight  +  96;
         rclUpdate.yTop    = rclQty.yTop    +   8;

         WinCreateWindow (hwnd, WC_ENTRYFIELD,
                          NULL, WS_VISIBLE | ES_MARGIN,
                          (USHORT) rclQty.xLeft,
                          (USHORT) rclQty.yBottom,
                          (USHORT) rclQty.xRight - (USHORT) rclQty.xLeft,
                          (USHORT) rclQty.yTop   - (USHORT) rclQty.yBottom,
                          hwnd, HWND_TOP, PemethRegC5, &efdQuantity, NULL);

         WinCreateWindow (hwnd, WC_BUTTON, "Update",
                          WS_VISIBLE | BS_PUSHBUTTON,
                          (USHORT) rclUpdate.xLeft,
                          (USHORT) rclUpdate.yBottom,
                          (USHORT) rclUpdate.xRight - (USHORT) rclUpdate.xLeft,
                          (USHORT) rclUpdate.yTop   - (USHORT) rclUpdate.yBottom,
                          hwnd, HWND_TOP, DID_UPDATEBUTTON, NULL, NULL);

         WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, PemethRegC5));
         }
         break;

      case WM_BUTTON1DOWN:
         SetPrice (hwnd, 0, (LONG)SHORT1FROMMP (mp1), (LONG)SHORT2FROMMP (mp1), TRUE);
         break;

      case WM_CHAR:
         {
         USHORT uVKey = (USHORT) SHORT2FROMMP (mp2);

         if (uVKey == VK_NEWLINE)
            DoQuantityChange (hwnd, FALSE);
         }
         break;

      case WM_PAINT:
         ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
         PaintJobPemethRegGraph (hwnd, TRUE);
//         WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, PemethRegC5));
         return 0;

      case WM_SIZE:
         {
         RECTL    rclQty    = {32, 3, 12, 1};
//         RECTL    rclUpdate = {414, 268,  468, 298};   
         RECTL    rclUpdate;   

         hwndQty    = WinWindowFromID (hwnd, PemethRegC5);
         hwndUpdate = WinWindowFromID (hwnd, DID_UPDATEBUTTON);

         if (hwndQty)
            {
            ItiWndPrepRcl (hwnd, COORD_CHRRNG, &rclQty);

            rclUpdate.xLeft   = rclQty.xLeft   + 150;
            rclUpdate.yBottom = rclQty.yBottom -   6;
            rclUpdate.xRight  = rclQty.xRight  +  96;
            rclUpdate.yTop    = rclQty.yTop    +   8;

            WinSetWindowPos (hwndQty, HWND_TOP,
                             (SHORT) rclQty.xLeft,   (SHORT) rclQty.yBottom,
                             (SHORT) rclQty.xRight - (SHORT) rclQty.xLeft,
                             (SHORT) rclQty.yTop   - (SHORT) rclQty.yBottom,
                             SWP_MOVE | SWP_SIZE);

            WinSetWindowPos (hwndUpdate, HWND_TOP,
                             (SHORT) rclUpdate.xLeft,   (SHORT) rclUpdate.yBottom,
                             (SHORT) rclUpdate.xRight - (SHORT) rclUpdate.xLeft,
                             (SHORT) rclUpdate.yTop   - (SHORT) rclUpdate.yBottom,
                             SWP_MOVE | SWP_SIZE);
            }
         }
         break;

      case WM_DESTROY:
         {
         PVOID p;
         HHEAP hheap;

         hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);

         p = ItiWndGetExtra (hwnd, ITI_STATIC);
         ItiMemFree (hheap, p);
         }
         break;

      case WM_COMMAND:
         if (SHORT1FROMMP (mp2) == CMDSRC_PUSHBUTTON)
            {
            DoQuantityChange (hwnd, TRUE);
            uMBReturn = 0;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }



static void EXPORT FillSeason (HWND hwnd)
   {
   HWND hwndC;

   hwndC = WinWindowFromID (hwnd, PemethRegC1);
   if (hwndC == NULL)
      return;

   WinSendMsg (hwndC, LM_DELETEALL, 0, 0);
   WinSendMsg (hwndC, LM_INSERTITEM, (MPARAM) 0, MPFROMP ("Winter"));
   WinSendMsg (hwndC, LM_INSERTITEM, (MPARAM) 1, MPFROMP ("Spring"));
   WinSendMsg (hwndC, LM_INSERTITEM, (MPARAM) 2, MPFROMP ("Summer"));
   WinSendMsg (hwndC, LM_INSERTITEM, (MPARAM) 3, MPFROMP ("Fall"));
   WinSendMsg (hwndC, LM_SETITEMHANDLE,  (MPARAM) 0, MPFROMP ("1"));
   WinSendMsg (hwndC, LM_SETITEMHANDLE,  (MPARAM) 1, MPFROMP ("2"));
   WinSendMsg (hwndC, LM_SETITEMHANDLE,  (MPARAM) 2, MPFROMP ("3"));
   WinSendMsg (hwndC, LM_SETITEMHANDLE,  (MPARAM) 3, MPFROMP ("4"));
   WinSendMsg (hwndC, LM_SELECTITEM, (MPARAM) 0, (MPARAM) TRUE);
   }


static void EXPORT FillCombo (HWND     hwnd,
                       HHEAP    hheap,
                       USHORT   usComboID,
                       PSZ      pszKey)
   {
   char     szTemp [510] = "";
   char     szQuery [510] = "";
   HQRY     hqry;
   HWND     hwndC;
   USHORT   usNumCols, usState;
   PPSZ     ppsz = NULL;
   PSZ      psz = NULL;
   USHORT   i;

   hwndC = WinWindowFromID (hwnd, usComboID);
   if (hwndC == NULL)
      return;

   /* delete all items */
   i = (UM) WinSendMsg (hwndC, LM_QUERYITEMCOUNT, 0, 0);
   while (i)
      {
      i--;
      psz = (PSZ) WinSendMsg (hwndC, LM_QUERYITEMHANDLE, (MPARAM) i, 0);
      ItiMemFree (hheap, psz);
      }
   WinSendMsg (hwndC, LM_DELETEALL, 0, 0);

   ItiMbQueryQueryText (hmod, ITIRID_WND, usComboID, szTemp, sizeof szTemp);
   sprintf (szQuery, szTemp, pszKey);



   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usNumCols, &usState);
   if (hqry == NULL)
      return;

   i = 0;
   while (ItiDbGetQueryRow (hqry, &ppsz, &usState))
      {
      WinSendMsg (hwndC, LM_INSERTITEM, (MPARAM) i, MPFROMP (ppsz [0]));
      WinSendMsg (hwndC, LM_SETITEMHANDLE,  (MPARAM) i, MPFROMP (ppsz [1]));
      ItiMemFree (hheap, ppsz [0]);
      ItiMemFree (hheap, ppsz);
      i++;
      }
   WinSendMsg (hwndC, LM_SELECTITEM, (MPARAM) 0, (MPARAM) TRUE);
   }


MRESULT EXPORT PemethRegSProc (HWND    hwnd,
                               USHORT  usMessage,
                               MPARAM  mp1,
                               MPARAM  mp2)
   {
   switch (usMessage)
      {
      case WM_ITIDBBUFFERDONE:
         {
         PVOID p;
         HHEAP hheap;
         ULONG ulStyle;
         PSZ   psz;

         ulStyle = WinQueryWindowULong (hwnd, QWL_STYLE);
         ulStyle &= ~WS_CLIPCHILDREN;
         WinSetWindowULong (hwnd, QWL_STYLE, ulStyle);

         hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
         FillSeason (hwnd);

         FillCombo (hwnd, hheap, PemethRegC2, NULL);

         psz = (PSZ) QW (hwnd, ITIWND_DATA, 0, 0, cAreaTypeKey);
         if ((psz == NULL) || ((*psz) == '\0') )
            {
            FillCombo (hwnd, hheap, PemethRegC3, " 1 ");
            }                        /* default area type key is 1 */
         else
            {
            FillCombo (hwnd, hheap, PemethRegC3, psz);
            ItiMemFree (hheap, psz);
            }

         psz = (PSZ) QW (hwnd, ITIWND_DATA, 0, 0, cStandardItemKey);
         if ((psz == NULL) || ((*psz) == '\0') )
            {
            FillCombo (hwnd, hheap, PemethRegC4,
              " (select MIN(StandardItemKey) from StandardItemPEMETHRegression ) ");
            }
         else
            {
            FillCombo (hwnd, hheap, PemethRegC4, psz);
            ItiMemFree (hheap, psz);
            }

         psz = (PSZ) QW (hwnd, ITIWND_DATA, 0, 0, cMedianQuantity);
         SWL (hwnd, ITIWND_LABEL, 4, psz);
         ItiMemFree (hheap, psz);

         p = ItiMemAlloc (hheap, sizeof (REGWINDAT), 0);
         ((PREGWINDAT)p)->bOk = FALSE;
         ItiWndSetExtra (hwnd, ITI_STATIC, p);
         DoLabels (hwnd, mp2, FALSE);
         }
         break;

      case WM_BUTTON1DOWN:
         SetPrice (hwnd, 0, (LONG)SHORT1FROMMP (mp1), (LONG)SHORT2FROMMP (mp1), FALSE);
         break;

      case WM_CONTROL:
         if ((SHORT2FROMMP (mp1) == LN_ENTER   ||
              SHORT2FROMMP (mp1) == LN_SELECT) &&
             !(UM) WinSendDlgItemMsg (hwnd, SHORT1FROMMP (mp1), (USHORT) CBM_ISLISTSHOWING, 0, 0))
            {
            HBUF  hbuf;
            PVOID p;
           
            p = ItiWndGetExtra (hwnd, ITI_STATIC);
            if (p == NULL)
               break;

            hbuf = (HBUF) QW (hwnd, ITIWND_BUFFER, 0, 0, 0);
            DoLabels (hwnd, hbuf, FALSE);
            }
         break;

      case WM_PAINT:
         ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
         PaintJobPemethRegGraph (hwnd, FALSE);
         return 0;

      case WM_DESTROY:
         {
         PVOID p;
         HHEAP hheap;

         hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);

         p = ItiWndGetExtra (hwnd, ITI_STATIC);
         ItiMemFree (hheap, p);
         }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }

