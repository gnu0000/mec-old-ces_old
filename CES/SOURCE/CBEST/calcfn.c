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
 *  CalcFn.c
 *
 *  This module contains the CBE functions that perform calculations
 *
 */
#define  INCL_DOSMISC

#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itidbase.h"
#include "..\include\itiutil.h"
#include "..\include\itiglob.h"
#include "..\include\itibase.h"
#include "..\include\itierror.h"
#include "..\include\itiest.h"
#include "..\include\itifmt.h"
#include "..\include\colid.h"
#include "..\include\cbest.h"
#include "..\include\cesutil.h"
#include "initcat.h"
#include "calcfn.h"
#include "cbest.h"
#include "dialog.h"
#include <stdio.h>
#include <process.h>
#include <stddef.h>
#include <math.h>

/* These are used by the ConversionFactor routines. */
#define DEF_HRS    8.0
#define HALF_DAY   0.5
#define DAY        1.0
#define WEEK       5.0
#define MONTH     20.83
#define QTR       62.5
#define YEAR     250.0


#define UNFUNDEDKEY    1L
#define KEYLENGTH      10
#define RETURN_ERROR   13
#define MAXCREWSPERJCS 64
               // ^was 32
#define MAXLABORERSPERJCS 50
#define MAXEQUIPSPERJCS 50
#define MAXMATERIALSPERJCS 128

/* -- Character buffer space. */
static  CHAR  szTemp_ErrCalc    [1150];
static  CHAR  szTmpJIReCalcCBEST [200];
static  CHAR  szTempCMUP         [600];
static  CHAR  szCLRTemp         [1020];
static  CHAR  szCLRTemp2         [120];
static  CHAR  szCLRTemp3        [800];
static  CHAR  szCLRTemp4        [800];
static  CHAR  szCERTemp          [960];
static  CHAR  szTempRecal        [450];
static  CHAR  szTempRecal2       [450];

static  char   szTemp  [450];
static  char   szTemp2 [450];

static  CHAR   aszJobCrewLaborKeys     [MAXLABORERSPERJCS] [KEYLENGTH];
static  CHAR   aszJobCrewEquipmentKeys [MAXEQUIPSPERJCS]   [KEYLENGTH];
static  CHAR   aszCrewKeys             [MAXCREWSPERJCS]    [KEYLENGTH];

/* -- */

int EXPORT DoLaborersExist (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey);
int EXPORT DoesEquipExist (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey);
int EXPORT DoJobCrewsExist (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey);
int EXPORT DoJobMaterialsExist (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey);



/*
 * This file contains two separate types of functions:
 *
 * 1> High level functions, such as DoJobMaterialsUsedRecalc, which get keys
 *     and call the ItiDBDoCalc function.
 *    These are called by you and return immediately 
 *
 * 2> Low Level functions, such as CalcDetailedEstimateCost, which
 *     are called by the recalc thread to do the actual calculation.
 *    These block.
 *
 *
 */


USHORT ErrCalc (PSZ psz1, PSZ psz2)
   {
//   char  szTemp_ErrCalc [2048];

   szTemp_ErrCalc[0] = '\0';
   sprintf (szTemp_ErrCalc, "CALC: %s %s", psz1, psz2);
   ItiErrWriteDebugMessage (szTemp_ErrCalc);

   DosBeep (1000, 10);
   DosBeep (1502, 22);
   if (pglobals && pglobals->hwndDesktop && pglobals->hwndAppFrame)
      WinMessageBox (pglobals->hwndDesktop,
                     pglobals->hwndAppFrame, szTemp_ErrCalc,
                     "CALC", 0, MB_OK | MB_SYSTEMMODAL | MB_MOVEABLE);
   return 1;
   }




/*******************************************************************/
/*                    HIGH LEVEL FUNCTIONS                         */
/*******************************************************************/

void EXPORT DoJobMaterialsUsedReCalc (HWND hwnd, HWND hwndList)
   {
//    USHORT usRow;
//    PSZ    pszJobKey, pszJobCostSheetKey, pszMaterialKey;
//    PSZ    pszJobBrkdwnKey, pszJobItemKey;
//    PSZ    *ppszPNames, *ppszParams;
//    HHEAP  hheap;
//    HHEAP  hheapList;
//    HWND   hwndParent, hwndStatic;

//    hwndParent = QW(hwnd, ITIWND_OWNERWND, 1, 0, 0);
//    ppszPNames = ItiStrMakePPSZ (hheap, 6, "JobKey", "JobCostSheetKey", "MaterialKey", "JobItemKey", "JobBreakdownKey", NULL);
//    ppszParams = ItiStrMakePPSZ (hheap, 6, pszJobKey, pszJobCostSheetKey, pszMaterialKey, pszJobItemKey, pszJobBrkdwnKey, NULL);

//   ItiDBDoCalc (CalcMaterialsUnitPrice, ppszPNames, ppszParams, 0);
//   ItiDBDoCalc (CalcMaterialsUnitCost, ppszPNames, ppszParams, 0);

//    ReCalcJobCstShtMaterials (pszJobKey, pszJobCostSheetKey);
//    ItiDBDoCalc (CalcJobCostShtUnitPrice, ppszPNames, ppszParams, 0);

//   DoChgJobCostShtCalc (hheap, ppszPNames, ppszParams);
//   ItiDBDoCalc (CalcJobItem, ppszPNames, ppszParams, 0);

                           
//    ItiFreeStrArray (hheap, ppszPNames, 6);
//    ItiFreeStrArray (hheap, ppszParams, 6);


   }/* End of Function DoJobMaterialsUsedReCalc */







void DoJobCrewLaborUsedReCalc (HWND hwnd, HWND hwndList)
   {
   USHORT usRow;
   PSZ    pszJobKey, pszJobCostSheetKey, pszJobCrewKey, pszLaborerKey;
   PSZ    pszJobItemKey;
   PSZ    *ppszPNames, *ppszParams;
   HHEAP  hheap;
   HHEAP  hheapList;

   hheap            = (HHEAP) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_HEAP), 0);

   usRow             = (UM)    WinSendMsg (hwnd, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_ACTIVE), 0);

   pszJobKey        = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_DATA), 
                                   MPFROM2SHORT (usRow, cJobKey));
   
   pszJobItemKey    = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_DATA), 
                                   MPFROM2SHORT (usRow, cJobItemKey));
   
   pszJobCostSheetKey = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_DATA), 
                                   MPFROM2SHORT (usRow, cJobCostSheetKey));

   pszJobCrewKey = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_DATA), 
                                   MPFROM2SHORT (usRow, cJobCrewKey));


   usRow = SHORT1FROMMR (WinSendMsg (hwndList, WM_ITIWNDQUERY,
                        MPFROMSHORT (ITIWND_ACTIVE),0));

   hheapList = WinSendMsg (hwndList, WM_ITIWNDQUERY,
                           MPFROMSHORT (ITIWND_HEAP), 0);


   pszLaborerKey = (PSZ)   WinSendMsg (hwndList, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_DATA), 
                                   MPFROM2SHORT (usRow, cLaborerKey));
   

   ppszPNames = ItiStrMakePPSZ (hheap, 6, "JobKey", "JobCostSheetKey", "JobCrewKey", "LaborerKey", "JobItemKey", NULL);
   ppszParams = ItiStrMakePPSZ (hheap, 6, pszJobKey, pszJobCostSheetKey, pszJobCrewKey, pszLaborerKey, pszJobItemKey, NULL);

//   ItiDBDoCalc (CalcLaborRate, ppszPNames, ppszParams, 0);
//   ItiDBDoCalc (CalcLaborDailyCost, ppszPNames, ppszParams, 0);
//   ItiDBDoCalc (CalcJobCostShtUnitPrice, ppszPNames, ppszParams,0);
//   ItiDBDoCalc (CalcJobItem, ppszPNames, ppszParams, 0);

   ItiFreeStrArray (hheap, ppszPNames, 6);
   ItiFreeStrArray (hheap, ppszParams, 6);

   if (pszJobKey)
      ItiMemFree(hheap, pszJobKey);
   if (pszJobItemKey)
      ItiMemFree(hheap, pszJobItemKey);
   if (pszJobCostSheetKey)
      ItiMemFree(hheap, pszJobCostSheetKey);
   if (pszJobCrewKey)
      ItiMemFree(hheap, pszJobCrewKey);
   if (pszLaborerKey)
      ItiMemFree(hheapList, pszLaborerKey);
   }





void DoJobCrewEquipUsedReCalc (HWND hwnd, HWND hwndList)
   {
   USHORT usRow;
   PSZ    pszJobKey, pszJobCostSheetKey, pszJobCrewKey, pszEquipmentKey;
   PSZ    pszJobItemKey;
   PSZ    *ppszPNames, *ppszParams;
   HHEAP  hheap;
   HHEAP  hheapList;


   hheap            = (HHEAP) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_HEAP), 0);

   usRow             = (UM)    WinSendMsg (hwnd, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_ACTIVE), 0);

   pszJobKey        = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_DATA), 
                                   MPFROM2SHORT (usRow, cJobKey));

   pszJobItemKey    = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_DATA), 
                                   MPFROM2SHORT (usRow, cJobItemKey));

   pszJobCostSheetKey = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_DATA), 
                                   MPFROM2SHORT (usRow, cJobCostSheetKey));

   pszJobCrewKey = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_DATA), 
                                   MPFROM2SHORT (usRow, cJobCrewKey));


   usRow = SHORT1FROMMR (WinSendMsg (hwndList, WM_ITIWNDQUERY,
                        MPFROMSHORT (ITIWND_ACTIVE),0));

   hheapList = WinSendMsg (hwndList, WM_ITIWNDQUERY,
                           MPFROMSHORT (ITIWND_HEAP), 0);


   pszEquipmentKey = (PSZ)   WinSendMsg (hwndList, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_DATA), 
                                   MPFROM2SHORT (usRow, cEquipmentKey));
   

   ppszPNames = ItiStrMakePPSZ (hheap, 6, "JobKey", "JobCostSheetKey", "JobCrewKey", "EquipmentKey", "JobItemKey", NULL);
   ppszParams = ItiStrMakePPSZ (hheap, 6, pszJobKey, pszJobCostSheetKey, pszJobCrewKey, pszEquipmentKey, pszJobItemKey, NULL);

//   ItiDBDoCalc (CalcEquipmentRate, ppszPNames, ppszParams, 0);
//   ItiDBDoCalc (CalcEquipmentDailyCost, ppszPNames, ppszParams, 0);
//   ItiDBDoCalc (CalcJobCostShtUnitPrice, ppszPNames, ppszParams,0);
//   ItiDBDoCalc (CalcJobItem, ppszPNames, ppszParams, 0);

   ItiFreeStrArray (hheap, ppszPNames, 6);
   ItiFreeStrArray (hheap, ppszParams, 6);

   if (pszJobKey)
      ItiMemFree(hheap, pszJobKey);
   if (pszJobItemKey)
      ItiMemFree(hheap, pszJobItemKey);
   if (pszJobCostSheetKey)
      ItiMemFree(hheap, pszJobCostSheetKey);
   if (pszJobCrewKey)
      ItiMemFree(hheap, pszJobCrewKey);
   if (pszEquipmentKey)
      ItiMemFree(hheapList, pszEquipmentKey);

   }





void EXPORT DoJobCostShtReCalc (HWND hwnd)
   {
   USHORT usRow, usLabExists = 0, usEquipExists = 0;
   PSZ    pszJobKey, pszJobCostSheetKey, pszJobItemKey;
   PSZ    *ppszPNames, *ppszParams;
   HHEAP  hheap;
	HWND   hwndX;
	BOOL   bFreeJIK = TRUE;


   hheap            = (HHEAP) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_HEAP), 0);

   usRow             = (UM)    WinSendMsg (hwnd, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_ACTIVE), 0);

   pszJobKey        = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_DATA), 
                                   MPFROM2SHORT (usRow, cJobKey));

   pszJobItemKey    = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_DATA), 
                                   MPFROM2SHORT (usRow, cJobItemKey));
   if (pszJobItemKey == NULL)
      {                             //Inserted as a result of BSR#920612-4105
      hwndX = QW(hwnd,ITIWND_OWNERWND,3,0,0);
      pszJobItemKey = (PSZ) QW(hwndX, ITIWND_DATA,0, 0, cJobItemKey);	  //
      bFreeJIK = FALSE;
      }


   pszJobCostSheetKey = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_DATA), 
                                   MPFROM2SHORT (usRow, cJobCostSheetKey));

   ppszPNames = ItiStrMakePPSZ (hheap, 4, "JobKey", "JobCostSheetKey", "JobItemKey", NULL);
   ppszParams = ItiStrMakePPSZ (hheap, 4, pszJobKey, pszJobCostSheetKey, pszJobItemKey, NULL);

   ReCalcJobCstShtCrew (pszJobKey, pszJobCostSheetKey);
   ReCalcJobCstShtMaterials (pszJobKey, pszJobCostSheetKey);


//    usLabExists = DoLaborersExist (hheap, pszJobKey, pszJobCostSheetKey);
// 
//    if (usLabExists == 0)
//       ItiDBDoCalc (CalcLaborDailyCost, ppszPNames, ppszParams, 0);
//    else 
//       ItiDBDoCalc (SetLaborDailyCostToZero, ppszPNames, ppszParams, 0);
//    
//    usEquipExists = DoesEquipExist (hheap, pszJobKey, pszJobCostSheetKey);
// 
//    if (usEquipExists == 0)
//       ItiDBDoCalc (CalcEquipmentDailyCost, ppszPNames, ppszParams, 0);
//    else 
//       ItiDBDoCalc (SetEquipmentDailyCostToZero, ppszPNames, ppszParams, 0);


   CalcJobCostShtUnitPrice (hheap, ppszPNames, ppszParams);
//   ItiDBDoCalc (CalcJobItem, ppszPNames, ppszParams, 0);


   if (pszJobKey)
      ItiMemFree (hheap, pszJobKey);

	if (bFreeJIK)  //Do NOT free ptr if we had to get it from another window.
		{
      if (pszJobItemKey)
         ItiMemFree (hheap, pszJobItemKey);
		}

   if (pszJobCostSheetKey)
      ItiMemFree (hheap, pszJobCostSheetKey);

   ItiFreeStrArray (hheap, ppszPNames, 4);
   ItiFreeStrArray (hheap, ppszParams, 4);
   }/* End of Function DoJobCostShtReCalc */





void EXPORT DoAddJobCostShtCalc (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey)

   {
   HHEAP  hhp;
   PSZ    *ppszPNames, *ppszParams;
   USHORT usCrewExists, usMaterialExists;

   hhp = ItiMemCreateHeap (MAX_HEAP_SIZE);
   if (hhp == NULL)
      {
      ItiErrWriteDebugMessage ("*** Failed to create heap in cbest.calcfn.DoAddJobCostShtCalc. ");
      return;
      }

   ppszPNames = ItiStrMakePPSZ (hhp, 3, "JobKey", "JobCostSheetKey", NULL);
   ppszParams = ItiStrMakePPSZ (hhp, 3, pszJobKey, pszJobCostSheetKey, NULL);

   usCrewExists = DoJobCrewsExist(hhp, pszJobKey, pszJobCostSheetKey);

   if (usCrewExists == 0)
      {
      CalcLaborDailyCost (hheap, ppszPNames, ppszParams);
      CalcEquipmentDailyCost (hheap, ppszPNames, ppszParams);
      }

   usMaterialExists = DoJobMaterialsExist(hhp, pszJobKey, pszJobCostSheetKey);

   if (usMaterialExists == 0)
      {
      CalcMaterialsUnitCost (hheap, ppszPNames, ppszParams);
      }

   CalcJobCostShtUnitPrice (hheap, ppszPNames, ppszParams);

   ItiFreeStrArray (hhp, ppszPNames, 3);
   ItiFreeStrArray (hhp, ppszParams, 3);

   if (NULL != hhp)
      ItiMemDestroyHeap (hhp);
   }/* End of Function DoAddJobCostShtCalc */



int EXPORT DoJobItemReCalcCBEST(HHEAP hheap, PSZ pszJobKey, PSZ pszJobItemKey)
   {
//   CHAR  szTmpJIReCalcCBEST[256] = "";
   PSZ   pszJCBEKey, pszJBKey;
   PSZ   *ppszPNames, *ppszParams;
   HHEAP  hhp = NULL;

   hhp = ItiMemCreateHeap (MAX_HEAP_SIZE);
   if (hhp == NULL)
      return 13;

   ItiStrCpy (szTmpJIReCalcCBEST,
    "/* cbest.calcfn.DoJobItemReCalcCBEST */ "
    "SELECT JobItemCostBasedEstimateKey "
    " FROM JobCostBasedEstimate WHERE Active = 1 "
    " AND JobKey = ",  sizeof szTmpJIReCalcCBEST);
   ItiStrCpy (szTmpJIReCalcCBEST, pszJobKey, sizeof szTmpJIReCalcCBEST);
   ItiStrCpy (szTmpJIReCalcCBEST," AND JobItemKey = ", sizeof szTmpJIReCalcCBEST);
   ItiStrCpy (szTmpJIReCalcCBEST, pszJobItemKey, sizeof szTmpJIReCalcCBEST);

   pszJCBEKey = ItiDbGetRow1Col1 (szTmpJIReCalcCBEST, hhp, 0, 0, 0);

   ItiStrCpy (szTmpJIReCalcCBEST,
         "/* cbest.calcfn.DoJobItemReCalcCBEST */ "
         "SELECT JobBreakdownKey FROM JobItem WHERE JobKey = "
         , sizeof szTmpJIReCalcCBEST);
   ItiStrCat (szTmpJIReCalcCBEST, pszJobKey, sizeof szTmpJIReCalcCBEST);
   ItiStrCat (szTmpJIReCalcCBEST, " AND JobItemKey = ",sizeof szTmpJIReCalcCBEST);
   ItiStrCat (szTmpJIReCalcCBEST, pszJobItemKey, sizeof szTmpJIReCalcCBEST);

   pszJBKey = ItiDbGetRow1Col1 (szTmpJIReCalcCBEST, hhp, 0, 0, 0);

   ppszPNames = ItiStrMakePPSZ (hhp, 5, "JobBreakdownKey", "JobKey", "JobItemKey",  "JobItemCostBasedEstimatekey", NULL);
   ppszParams = ItiStrMakePPSZ (hhp, 5, pszJBKey,          pszJobKey, pszJobItemKey, pszJCBEKey, NULL);

   CalcJobItemActiveCostSht (hheap, ppszPNames, ppszParams);
   CalcJobItemUnitPrice (hheap, ppszPNames, ppszParams);

   CalcJobItemChgJobBrkdwn (hheap, ppszPNames, ppszParams);
   CalcJobItemChgJob (hheap, ppszPNames, ppszParams);

   ItiFreeStrArray (hhp, ppszPNames, 5);
   ItiFreeStrArray (hhp, ppszParams, 5);
   if (hhp != NULL)
      ItiMemDestroyHeap(hhp);

   return 0;
   } /* End of Function DoJobItemReCalcCBEST */





void EXPORT DoDelJobMaterialsUsedReCalc (HWND hwnd)
   {
   USHORT usRow, usMaterialExists;
   PSZ    pszJobKey, pszJobCostSheetKey;
   PSZ    *ppszPNames, *ppszParams;
   HHEAP  hheap;

   hheap            = (HHEAP) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_HEAP), 0);

   usRow             = (UM)    WinSendMsg (hwnd, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_ACTIVE), 0);

   pszJobKey        = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_DATA), 
                                   MPFROM2SHORT (0, cJobKey));

   pszJobCostSheetKey = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_DATA), 
                                   MPFROM2SHORT (0, cJobCostSheetKey));


   ppszPNames = ItiStrMakePPSZ (hheap, 3, "JobKey", "JobCostSheetKey",  NULL);
   ppszParams = ItiStrMakePPSZ (hheap, 3, pszJobKey, pszJobCostSheetKey, NULL);

   /*  ********************* */

   usMaterialExists = DoJobMaterialsExist(hheap, pszJobKey, pszJobCostSheetKey);

   if (usMaterialExists == 0)
      CalcMaterialsUnitCost (hheap, ppszPNames, ppszParams);
   else
      SetMaterialsUnitCostToZero (hheap, ppszPNames, ppszParams);

   CalcJobCostShtUnitPrice (hheap, ppszPNames, ppszParams);

   ItiFreeStrArray (hheap, ppszPNames, 3);
   ItiFreeStrArray (hheap, ppszParams, 3);
   if (pszJobKey)
      ItiMemFree(hheap, pszJobKey);
   if (pszJobCostSheetKey)
      ItiMemFree(hheap, pszJobCostSheetKey);
   }





void EXPORT DoCalcJobCrewLaborRate (HHEAP hheap, HWND hwndList, PSZ pszJobKey, PSZ pszJobCostSheetKey, PSZ pszJobCrewKey)
   {
   USHORT usRow;
   PSZ    pszLaborerKey;
   PSZ    *ppszPNames, *ppszParams;
   HHEAP  hheapList;

   usRow = SHORT1FROMMR (WinSendMsg (hwndList, WM_ITIWNDQUERY,
                        MPFROMSHORT (ITIWND_ACTIVE),0));

   hheapList = WinSendMsg (hwndList, WM_ITIWNDQUERY,
                           MPFROMSHORT (ITIWND_HEAP), 0);

   pszLaborerKey = (PSZ)   WinSendMsg (hwndList, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_DATA), 
                                   MPFROM2SHORT (usRow, cLaborerKey));
   

   ppszPNames = ItiStrMakePPSZ (hheap, 5, "JobKey", "JobCostSheetKey", "JobCrewKey", "LaborerKey", NULL);
   ppszParams = ItiStrMakePPSZ (hheap, 5, pszJobKey, pszJobCostSheetKey, pszJobCrewKey,  pszLaborerKey, NULL);

   CalcLaborRate (hheap, ppszPNames, ppszParams);

   ItiFreeStrArray (hheap, ppszPNames, 5);
   ItiFreeStrArray (hheap, ppszParams, 5);

//   if ((hheapList != NULL) && (pszLaborerKey != NULL))
//      ItiMemFree(hheapList, pszLaborerKey);

   }





void EXPORT DoCalcJobCrewEquipRate (HHEAP hheap, HWND hwndList, PSZ pszJobKey, PSZ pszJobCostSheetKey, PSZ pszJobCrewKey)
   {
   USHORT usRow;
   PSZ    pszEquipmentKey;
   PSZ    *ppszPNames, *ppszParams;
   HHEAP  hheapList;




   usRow = SHORT1FROMMR (WinSendMsg (hwndList, WM_ITIWNDQUERY,
                        MPFROMSHORT (ITIWND_ACTIVE),0));

   hheapList = WinSendMsg (hwndList, WM_ITIWNDQUERY,
                           MPFROMSHORT (ITIWND_HEAP), 0);


   pszEquipmentKey = (PSZ)   WinSendMsg (hwndList, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_DATA), 
                                   MPFROM2SHORT (usRow, cEquipmentKey));
   

   ppszPNames = ItiStrMakePPSZ (hheap, 5, "JobKey", "JobCostSheetKey", "JobCrewKey", "EquipmentKey", NULL);
   ppszParams = ItiStrMakePPSZ (hheap, 5, pszJobKey, pszJobCostSheetKey, pszJobCrewKey,  pszEquipmentKey, NULL);

   CalcEquipmentRate (hheap, ppszPNames, ppszParams);

   ItiFreeStrArray (hheap, ppszPNames, 5);
   ItiFreeStrArray (hheap, ppszParams, 5);
   if (pszEquipmentKey)
      ItiMemFree(hheapList, pszEquipmentKey);

   }





/*  This function requires the following keys:
 *    JobKey
 *    JobItemKey
 *    JobBreakdownKey
 */
void EXPORT DoChgJobCostShtCalc (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
   CHAR szTmp[276] = "";

   if ((ppszPNames == NULL) || (ppszParams == NULL))
      return;


//   ItiDBDoCalc (CalcJobItemUnitPrice,    ppszPNames, ppszParams, 0);
//   ItiDBDoCalc (CalcJobItemChgJobBrkdwn, ppszPNames, ppszParams, 0);
//   ItiDBDoCalc (CalcJobItemChgJob,       ppszPNames, ppszParams, 0);

   ItiDbUpdateBuffers ("JobCostSheet");
   ItiDbUpdateBuffers ("Task");
   ItiDbUpdateBuffers ("JobItem");
   ItiDbUpdateBuffers ("JobBreakdown");
   ItiDbUpdateBuffers ("Job");

   } /* End of Function DoChgJobCostShtCalc */




 void EXPORT DoSetJobItemUnitPriceToZero(HHEAP hheap, PSZ pszJobKey, PSZ pszJobItemKey)
   
   {
   PSZ   *ppszPNames, *ppszParams;
   ppszPNames = ItiStrMakePPSZ (hheap, 3, "JobKey", "JobItemKey", NULL);
   ppszParams = ItiStrMakePPSZ (hheap, 3, pszJobKey, pszJobItemKey, NULL);

   SetJobItemUnitPriceToZero (hheap, ppszPNames, ppszParams);

   ItiFreeStrArray (hheap, ppszPNames, 3);
   ItiFreeStrArray (hheap, ppszParams, 3);
   }




void EXPORT DoJobAndJobBreakReCalcCBEST(HHEAP hheap, PSZ pszJobKey, PSZ pszJobBreakdownKey)
   
   {
   PSZ   *ppszPNames, *ppszParams;
   PSZ   pszProgramKey = NULL;
   char  szTemp [124]; /* was 1024 */


   ppszPNames = ItiStrMakePPSZ (hheap, 3, "JobKey", "JobBreakdownKey", NULL);
   ppszParams = ItiStrMakePPSZ (hheap, 3, pszJobKey, pszJobBreakdownKey, NULL);

   CalcDetailedEstimateCost (hheap, ppszPNames, ppszParams);

   ItiFreeStrArray (hheap, ppszPNames, 3);
   ItiFreeStrArray (hheap, ppszParams, 3);


   /* Determines whether the Job is currently in a Program */
                                                    /* RETHINK THIS SECTION */
   ItiStrCpy (szTemp,
      "SELECT ProgramKey FROM ProgramJob WHERE JobKey = ", sizeof szTemp);
   ItiStrCpy (szTemp, pszJobKey, sizeof szTemp);

   pszProgramKey = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);
   if (pszProgramKey == NULL)
      return;
   
   ppszPNames = ItiStrMakePPSZ (hheap, 4, "JobKey",  "JobBreakdownKey",  "ProgramKey", NULL);
   ppszParams = ItiStrMakePPSZ (hheap, 4, pszJobKey, pszJobBreakdownKey, pszProgramKey, NULL);

   CalcAllJobBreakFundsForJob (hheap, ppszPNames, ppszParams);

   ItiFreeStrArray (hheap, ppszPNames, 4);
   ItiFreeStrArray (hheap, ppszParams, 4);
   if (pszProgramKey)
      ItiMemFree (hheap, pszProgramKey);
   }



/*******************************************************************/
/*                     LOW LEVEL FUNCTIONS                         */
/*    These functions must all have the same header definition,    */
/*    (I.E. hheap, ppsz, ppsz, and return an int)                  */
/*******************************************************************/



/* Calculates the MaterialsUnitPrice and the Unit Price in the
 * JobCostSheet table
 *
 * Needs: JobCostSheetKey
 *        JobKey
 *        MaterialKey
 *
 */

int EXPORT CalcMaterialsUnitPrice (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
//   char   szTempCMUP [2048];
   USHORT usRet;
   DOUBLE dRes;
	PSZ    pszRes = NULL;

	/* -- Test if have a value for the UnitPrice, otherwise the subqurey   */
	/* -- would cause the update to fail by trying to insert a null value. */
   usRet = ItiStrReplaceParams (szTempCMUP,
                " /* cbest.calcfn.CalcMaterialsUnitPrice */ "
                " SELECT UnitPrice"
                " FROM MaterialPrice MP, Job, Material M, JobZone JZ"
                " WHERE MP.MaterialKey = %MaterialKey"
                " AND MP.ZoneKey = JZ.ZoneKey"
                " AND JZ.ZoneTypeKey = M.ZoneTypeKey"
                " AND JZ.JobKey = %JobKey"
                " AND MP.MaterialKey = M.MaterialKey"
                " AND MP.BaseDate = Job.BaseDate"
                " AND Job.JobKey = %JobKey ",
                sizeof szTempCMUP, 
                ppszPNames, 
                ppszParams, 
                100);
   if (usRet)
      {
      ErrCalc ("Not all params available for CalcMaterialsUnitPrice", szTempCMUP);
      return 1;
      }

   pszRes = ItiDbGetRow1Col1(szTempCMUP, hheap, 0, 0, 0);
ItiErrWriteDebugMessage(szTempCMUP);
ItiErrWriteDebugMessage(pszRes);

	if (pszRes != NULL)
      {
      dRes = ItiStrToDouble (pszRes);
      ItiMemFree (hheap, pszRes);
      }
   else
      dRes = 0.0;

   if (dRes == 0.0)
		{
      usRet = ItiStrReplaceParams (szTempCMUP,
                               " /* cbest.calcfn.CalcMaterialsUnitPrice */ "
                               "UPDATE JobMaterialUsed"
                               " SET UnitPrice = 0.0"
                               " WHERE JobKey = %JobKey"
                               " AND JobCostSheetKey = %JobCostSheetKey"
                               " AND MaterialKey = %MaterialKey",
                               sizeof szTempCMUP, 
                               ppszPNames, 
                               ppszParams, 
                               100);
		}
	else
      usRet = ItiStrReplaceParams (szTempCMUP,
                               " /* cbest.calcfn.CalcMaterialsUnitPrice */ "
                               "UPDATE JobMaterialUsed"
                               " SET UnitPrice ="
                               " (SELECT UnitPrice"
                               " FROM MaterialPrice, Job, Material, JobZone"
                               " WHERE MaterialPrice.MaterialKey = %MaterialKey"
                               " AND MaterialPrice.ZoneKey = JobZone.ZoneKey"
                               " AND JobZone.ZoneTypeKey = Material.ZoneTypeKey"
                               " AND JobZone.JobKey = %JobKey"
                               " AND MaterialPrice.MaterialKey = Material.MaterialKey"
                               " AND MaterialPrice.BaseDate = Job.BaseDate"
                               " AND Job.JobKey = %JobKey)"
                               " WHERE JobKey = %JobKey"
                               " AND JobCostSheetKey = %JobCostSheetKey"
                               " AND MaterialKey = %MaterialKey",
                               sizeof szTempCMUP, 
                               ppszPNames, 
                               ppszParams, 
                               100);

   if (usRet)
      {
      ErrCalc ("Not all params available for CalcMaterialsUnitPrice", szTempCMUP);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTempCMUP))
      {
      ErrCalc ("Bad Exec for CalcMaterialUnitPrice", szTempCMUP);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   /*--- do buffer updates after all calcs ---*/
//   ItiDbUpdateBuffers ("JobCostSheet");
   ItiDbUpdateBuffers ("JobMaterialUsed");
   
   return 0;
   } /* End of Function CalcMaterialsUnitPrice */






/* Inserts the Rate and OvertimeRate in the JobCrewLaborUsed table
 * 
 *
 * Needs: JobCostSheetKey
 *        JobKey
 *        JobCrewKey
 *        LaborerKey
 */

int EXPORT CalcLaborRate (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
//   char   szCLRTemp   [1024];
//   char   szCLRTemp2  [1024];
//   char   szCLRTemp3  [1024];
//   char   szCLRTemp4  [1024];
   HWND   hwnd = NULL;
   PSZ    pszDavisBacon, pszPrefix, pszJobKey, pszRes = NULL;
	USHORT usRet;
   DOUBLE dRes;

	/* -- Test if have a value for the LaborRate, otherwise the subqurey   */
	/* -- would cause the update to fail by trying to insert a null value. */
   usRet = ItiStrReplaceParams (szCLRTemp,
                               " SELECT %%sDavisBaconRate"
                               " FROM LaborerWageAndBenefits, Job, Labor, JobZone"
                               " WHERE LaborerWageAndBenefits.LaborerKey = %LaborerKey"
                               " AND LaborerWageAndBenefits.ZoneKey= JobZone.ZoneKey"
                               " AND JobZone.ZoneTypeKey = Labor.ZoneTypeKey"
                               " AND JobZone.JobKey = %JobKey"
                               " AND LaborerWageAndBenefits.LaborerKey = Labor.LaborerKey"       
                               " AND LaborerWageAndBenefits.BaseDate = Job.BaseDate"
                               " AND Job.JobKey = %JobKey ",
                               sizeof szCLRTemp, 
                               ppszPNames, 
                               ppszParams, 
                               100);

   /* Determines whether the Job uses DavisBacon or NonDavisBacon wages  */

   pszJobKey = ppszParams[0];

   ItiStrCpy (szCLRTemp2,
              "SELECT DavisBaconWages FROM Job WHERE JobKey = "
              , sizeof szCLRTemp2);
   ItiStrCpy (szCLRTemp2, pszJobKey, sizeof szCLRTemp2);

   pszDavisBacon = ItiDbGetRow1Col1(szCLRTemp2, hheap, 0, 0, 0);

   /* if the job uses NonDavisBacon wages, the prefix, Non, is attached */
   if (pszDavisBacon != NULL)
      {
      pszPrefix = ((pszDavisBacon[0] == '1') ? "" : "Non");
      sprintf(szCLRTemp3, szCLRTemp, pszPrefix, pszPrefix);
      ItiMemFree (hheap, pszDavisBacon);
      }
  else
      sprintf(szCLRTemp3, szCLRTemp, "Non", "Non");


   usRet = ItiWndReplaceAllParams(szCLRTemp4, szCLRTemp3, hwnd, hwnd,
                                 NULL, NULL, 0, FALSE, sizeof szCLRTemp4);
   if (usRet)
      {
      ErrCalc ("Not all params available for CalcLaborRates", szCLRTemp);
      return 1;
      }

   pszRes = ItiDbGetRow1Col1(szCLRTemp4, hheap, 0, 0, 0);
	if (pszRes != NULL)
      {
      dRes = ItiStrToDouble (pszRes);
      ItiMemFree (hheap, pszRes);
      }
   else
      dRes = 0.0;

   if (dRes == 0.0)
		{
      usRet = ItiStrReplaceParams (szCLRTemp,
                               " UPDATE JobCrewLaborUsed"
                               " SET Rate = 0.00"
                               ", OvertimeRate = 0.00"
                               " WHERE JobKey = %JobKey"
                               " AND JobCostSheetKey = %JobCostSheetKey"
                               " AND JobCrewKey = %JobCrewKey"
                               " AND LaborerKey = %LaborerKey",
                               sizeof szCLRTemp, 
                               ppszPNames, 
                               ppszParams, 
                               100);
//      ItiErrWriteDebugMessage
//			("The following query was used in place of a NULL subquery value: ");
//      ItiErrWriteDebugMessage(szCLRTemp);
      }
   else
      {
      usRet = ItiStrReplaceParams (szCLRTemp,
                               " UPDATE JobCrewLaborUsed"
                               " SET Rate ="
                               " (SELECT %%sDavisBaconRate"
                               " FROM LaborerWageAndBenefits, Job, Labor, JobZone"
                               " WHERE LaborerWageAndBenefits.LaborerKey = %LaborerKey"
                               " AND LaborerWageAndBenefits.ZoneKey= JobZone.ZoneKey"
                               " AND JobZone.ZoneTypeKey = Labor.ZoneTypeKey"
                               " AND JobZone.JobKey = %JobKey"
                               " AND LaborerWageAndBenefits.LaborerKey = Labor.LaborerKey"       
                               " AND LaborerWageAndBenefits.BaseDate = Job.BaseDate"
                               " AND Job.JobKey = %JobKey),"
                               " OvertimeRate ="
                               " (SELECT %%sDavisBaconOvertimeRate"
                               " FROM LaborerWageAndBenefits, Job, Labor, JobZone"
                               " WHERE LaborerWageAndBenefits.LaborerKey = %LaborerKey"
                               " AND LaborerWageAndBenefits.ZoneKey= JobZone.ZoneKey"
                               " AND JobZone.ZoneTypeKey = Labor.ZoneTypeKey"
                               " AND JobZone.JobKey = %JobKey"
                               " AND LaborerWageAndBenefits.LaborerKey = Labor.LaborerKey"       
                               " AND LaborerWageAndBenefits.BaseDate = Job.BaseDate"
                               " AND Job.JobKey = %JobKey)"
                               " WHERE JobKey = %JobKey"
                               " AND JobCostSheetKey = %JobCostSheetKey"
                               " AND JobCrewKey = %JobCrewKey"
                               " AND LaborerKey = %LaborerKey",
                               sizeof szCLRTemp, 
                               ppszPNames, 
                               ppszParams, 
                               100);

      /* Determines whether the Job uses DavisBacon or NonDavisBacon wages  */

      pszJobKey = ppszParams[0];

      ItiStrCpy(szCLRTemp2,
            "SELECT DavisBaconWages FROM Job WHERE JobKey = "
            , sizeof szCLRTemp2);
      ItiStrCpy(szCLRTemp2, pszJobKey, sizeof szCLRTemp2);

      pszDavisBacon = ItiDbGetRow1Col1(szCLRTemp2, hheap, 0, 0, 0);

      /* if the job uses NonDavisBacon wages, the prefix, Non, is attached */

      if (pszDavisBacon != NULL)
         {
         pszPrefix = ((pszDavisBacon[0] == '1') ? "" : "Non");
         ItiMemFree (hheap, pszDavisBacon);
         }

      sprintf(szCLRTemp3, szCLRTemp, pszPrefix, pszPrefix);

      usRet = ItiWndReplaceAllParams(szCLRTemp4, szCLRTemp3, hwnd, hwnd,
                                    NULL, NULL, 0, FALSE, sizeof szCLRTemp4);
      if (usRet)
         return 1;
	   }


   ItiDbBeginTransaction (hheap);

   if (ItiDbExecSQLStatement (hheap, szCLRTemp4))
      {
      ErrCalc ("Bad Exec for CalcLaborerRates", szCLRTemp4);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   /*--- do buffer updates after all calcs ---*/
   ItiDbUpdateBuffers ("JobCrewLaborUsed");
   
   
   return 0;
   } /* End of Function CalcLaborRate */




/* Inserts the Rate and OvertimeRate in the JobCrewEquipmentUsed table
 * 
 *
 * Needs: JobCostSheetKey
 *        JobKey
 *        JobCrewKey
 *        EquipmentKey
 */

int EXPORT CalcEquipmentRate (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
//   char   szCERTemp [1024];
	PSZ    pszRes = NULL;
   DOUBLE dRes;
   USHORT usRet;

	/* -- Test if have a value for the Rate, otherwise the subqurey   */
	/* -- would cause the update to fail by trying to insert a null value. */
      usRet = ItiStrReplaceParams (szCERTemp,
                               " SELECT Rate"
                               " FROM EquipmentRate, Job, Equipment, JobZone"
                               " WHERE EquipmentRate.EquipmentKey = %EquipmentKey"
                               " AND EquipmentRate.ZoneKey = JobZone.ZoneKey"
                               " AND JobZone.ZoneTypeKey = Equipment.ZoneTypeKey"
                               " AND JobZone.JobKey = %JobKey"  
                               " AND EquipmentRate.EquipmentKey = Equipment.EquipmentKey"
                               " AND EquipmentRate.BaseDate = Job.BaseDate"
                               " AND Job.JobKey = %JobKey ",
                               sizeof szCERTemp, 
                               ppszPNames, 
                               ppszParams, 
                               100);
   if (usRet)
      {
      ErrCalc ("Not all params available for CalcEquipmentRates", szCERTemp);
      return 1;
      }

   pszRes = ItiDbGetRow1Col1(szCERTemp, hheap, 0, 0, 0);
	if (pszRes != NULL)
      {
      dRes = ItiStrToDouble (pszRes);
      ItiMemFree (hheap, pszRes);
      }
   else
      dRes = 0.0;

   if (dRes == 0.0)
		{
      usRet = ItiStrReplaceParams (szCERTemp,
                               " UPDATE JobCrewEquipmentUsed"
                               " SET Rate = 0.00"
                               ", OvertimeRate = 0.00"
                               " WHERE JobKey = %JobKey"
                               " AND JobCostSheetKey = %JobCostSheetKey"
                               " AND JobCrewKey = %JobCrewKey"
                               " AND EquipmentKey = %EquipmentKey",
                               sizeof szCERTemp, 
                               ppszPNames, 
                               ppszParams, 
                               100);
//      ItiErrWriteDebugMessage
//			("The following query was used in place of a NULL subquery value: ");
//      ItiErrWriteDebugMessage(szCERTemp);
		}
	else
      usRet = ItiStrReplaceParams (szCERTemp,
                               " UPDATE JobCrewEquipmentUsed"
                               " SET Rate ="
                               " (SELECT Rate"
                               " FROM EquipmentRate, Job, Equipment, JobZone"
                               " WHERE EquipmentRate.EquipmentKey = %EquipmentKey"
                               " AND EquipmentRate.ZoneKey = JobZone.ZoneKey"
                               " AND JobZone.ZoneTypeKey = Equipment.ZoneTypeKey"
                               " AND JobZone.JobKey = %JobKey"  
                               " AND EquipmentRate.EquipmentKey = Equipment.EquipmentKey"
                               " AND EquipmentRate.BaseDate = Job.BaseDate"
                               " AND Job.JobKey = %JobKey),"
                               " OvertimeRate ="
                               " (SELECT OvertimeRate"
                               " FROM EquipmentRate, Job, Equipment, JobZone"
                               " WHERE EquipmentRate.EquipmentKey = %EquipmentKey"
                               " AND EquipmentRate.ZoneKey = JobZone.ZoneKey"
                               " AND JobZone.ZoneTypeKey = Equipment.ZoneTypeKey"
                               " AND JobZone.JobKey = %JobKey"  
                               " AND EquipmentRate.EquipmentKey = Equipment.EquipmentKey"
                               " AND EquipmentRate.BaseDate = Job.BaseDate"
                               " AND Job.JobKey = %JobKey)"
                               " WHERE JobKey = %JobKey"
                               " AND JobCostSheetKey = %JobCostSheetKey"
                               " AND JobCrewKey = %JobCrewKey"
                               " AND EquipmentKey = %EquipmentKey ",
                               sizeof szCERTemp, 
                               ppszPNames, 
                               ppszParams, 
                               100);

   if (usRet)
      {
      ErrCalc ("Not all params available for CalcEquipmentRates", szCERTemp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szCERTemp))
      {
      ErrCalc ("Bad Exec for CalcEquipmentRates", szCERTemp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   /*--- do buffer updates after all calcs ---*/
   ItiDbUpdateBuffers ("JobCrewEquipmentUsed");
   
   return 0;
   } /* End of Function CalcEquipmentRates */






/* Calculates the MaterialsUnitCost and the Unit Price in the
 * JobCostSheet table
 *
 * Needs: JobCostSheetKey
 *        JobKey
 *
 */

int EXPORT CalcMaterialsUnitCost (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
//   char   szTemp [250] =" ";
//   char   szTemp2 [250] =" ";
   USHORT usRet, usLen;
   DOUBLE  dRes;
	PSZ    pszRes = NULL;
   CHAR   szResFmt[32] = "";

	/* -- Test if have a value for the CalcMaterialsUnitCost, otherwise the subqurey   */
	/* -- would cause the update to fail by trying to insert a null value. */
   usRet = ItiStrReplaceParams (szTemp,
                               " /* cbest.calcfn.CalcMaterialsUnitCost */ "
                               "SELECT SUM(Quantity * UnitPrice)"
                               " FROM JobMaterialUsed"
                               " WHERE JobKey = %JobKey"
                               " AND JobCostSheetKey = %JobCostSheetKey",
                               sizeof szTemp, 
                               ppszPNames, 
                               ppszParams, 
                               100);
   if (usRet)
      {
      ErrCalc ("Not all params available for CalcMaterialUnitCost", szTemp);
      return 1;
      }

   pszRes = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);
	if (pszRes != NULL)
      {
      dRes = ItiStrToDouble (pszRes);
      ItiFmtFormatString (pszRes, szResFmt,
                          sizeof szResFmt, "Number,......", &usLen);
      }
   else
      dRes = 0.0;

   if (dRes == 0.0)
		{
      usRet = ItiStrReplaceParams (szTemp,
                               " /* cbest.calcfn.CalcMaterialsUnitCost */ "
                               "UPDATE JobCostSheet"
                               " SET MaterialsUnitCost = 0.00"
                               " WHERE JobKey = %JobKey"
                               " AND JobCostSheetKey = %JobCostSheetKey",
                               sizeof szTemp, 
                               ppszPNames, 
                               ppszParams, 
                               100);
		}
	else
      {
      ItiStrCpy (szTemp2," /* cbest.calcfn.CalcMaterialsUnitCost */ "
                      " UPDATE JobCostSheet"
                      " SET MaterialsUnitCost = ", sizeof szTemp2);
      ItiStrCat (szTemp2, szResFmt, sizeof szTemp2);
     
      ItiStrCat (szTemp2,
         " WHERE JobKey = %JobKey AND JobCostSheetKey = %JobCostSheetKey "
         , sizeof szTemp2);

      usRet = ItiStrReplaceParams
                 (szTemp, szTemp2, sizeof szTemp, ppszPNames, ppszParams, 100);
      }

   if (pszRes != NULL)
      ItiMemFree (hheap, pszRes);

   if (usRet)
      {
      ErrCalc ("Not all params available for CalcMaterialsCost", szTemp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      {
      ErrCalc ("Bad Exec for CalcMaterialsUnitCost", szTemp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   /*--- do buffer updates after all calcs ---*/
   ItiDbUpdateBuffers ("JobCostSheet");
   
   return 0;
   }






/* Calculates the LaborDailyCost in the
 * JobCostSheet table
 *
 * Needs: JobCostSheetKey
 *        JobKey
 *        JobCrewKey
 *
 */

int EXPORT CalcLaborDailyCost (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
//   char   szTemp [450] = "";
//   char   szTemp2 [450] = "";
   USHORT usRet, usLen;
   DOUBLE dProdRate, dSum;
   PSZ    pszProdRate = NULL;
   CHAR   szProdRateFmt[32] = "";
   PSZ    pszSum = NULL;
   CHAR   szSumFmt[32] = "";


   usRet = ItiStrReplaceParams (szTemp,
          " /* cbest.calcfn.CalcLaborDailyCost */ "
          " SELECT SUM((Quantity * Rate * HoursPerDay) +"
          " (Quantity * OvertimeRate * OvertimeHoursPerDay))"
          " FROM JobCrewLaborUsed, JobCostSheet"
          " WHERE JobCrewLaborUsed.JobCostSheetKey = JobCostSheet.JobCostSheetKey"
          " AND JobCrewLaborUsed.JobKey = JobCostSheet.JobKey"
          " AND JobCrewLaborUsed.JobKey = %JobKey"
          " AND JobCrewLaborUsed.JobCostSheetKey = %JobCostSheetKey",
          sizeof szTemp, 
          ppszPNames, 
          ppszParams, 
          100);

   pszSum = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);
	if (pszSum != NULL)
      {
      dSum = ItiStrToDouble (pszSum);
      ItiFmtFormatString (pszSum, szSumFmt,
                          sizeof szSumFmt, "Number,......", &usLen);
      }
   else
      dSum = 0.0;

   if (dSum == 0.0)
		{
      usRet = ItiStrReplaceParams (szTemp,
               " /* cbest.calcfn.CalcLaborDailyCost */ "
               "UPDATE JobCostSheet "
               " SET LaborDailyCost = 0.0 "
               " WHERE JobCostSheetKey = %JobCostSheetKey"
               " AND JobKey = %JobKey",
               sizeof szTemp, 
               ppszPNames, 
               ppszParams, 
               100);
      }
   else
      {
      /* -- Test if Production Rate is NON zero. */
      usRet = ItiStrReplaceParams (szTemp,
                " SELECT DisplayedProductionRate"
                " FROM JobCostSheet"
                " WHERE JobKey = %JobKey"
                " AND JobCostSheetKey = %JobCostSheetKey",
                sizeof szTemp, 
                ppszPNames, 
                ppszParams, 
                100);

      pszProdRate = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);
		if (pszProdRate != NULL)
         {
         dProdRate = ItiStrToDouble (pszProdRate);
         ItiFmtFormatString (pszProdRate, szProdRateFmt,
                             sizeof szProdRateFmt, "Number,......", &usLen);
         }
      else
         dProdRate = 0.0;

      if (dProdRate != 0.0)
			{       /* we can divide by the production rate */
         sprintf (szTemp2,
                  " /* cbest.calcfn.CalcLaborDailyCost, non-zero ProdRate */ "
                  "UPDATE JobCostSheet"
                  " SET LaborDailyCost = ( %s / %s )"
                  " WHERE JobCostSheetKey = %%JobCostSheetKey"
                  " AND JobKey = %%JobKey",
                  szSumFmt, szProdRateFmt);

         usRet = ItiStrReplaceParams
                    (szTemp, szTemp2, sizeof szTemp, ppszPNames, ppszParams, 100);
         }
      else
         { /* -- Production rate was zero, so do not divide by it. */
         sprintf (szTemp2,
                " /* cbest.calcfn.CalcLaborDailyCost, zero ProductionRate */ "
                "UPDATE JobCostSheet"
                " SET LaborDailyCost = %s"
                  " WHERE JobCostSheetKey = %%JobCostSheetKey"
                  " AND JobKey = %%JobKey ",
                  szSumFmt);

         usRet = ItiStrReplaceParams
                    (szTemp, szTemp2, sizeof szTemp, ppszPNames, ppszParams, 100);
         }
      }/* end if dSum... else clause */

   if (pszSum)
      ItiMemFree (hheap, pszSum);

   if (pszProdRate)
      ItiMemFree (hheap, pszProdRate);

   if (usRet)
      {
      ErrCalc ("Not all params available for CalcLaborDailyCost", szTemp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      {
      ErrCalc ("Bad Exec for CalcLaborDailyCost", szTemp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   /*--- do buffer updates after all calcs ---*/
   ItiDbUpdateBuffers ("JobCostSheet");
   return 0;
   } /* End of Function CalcLaborDailyCost */





int EXPORT CalcEquipmentDailyCost (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
//   char   szTemp [400] = "";
//   char   szTemp2 [400] = "";
   USHORT usRet, usLen;
   DOUBLE dSum;
   DOUBLE dProdRate;
   PSZ    pszProdRate = NULL;
   PSZ    pszSum = NULL;
   CHAR   szSumFmt[32] = "";
   CHAR   szProdRateFmt[32] = "";


   usRet = ItiStrReplaceParams (szTemp,
         " /* cbest.calcfn.CalcEquipmentDailyCost */ "
         "SELECT SUM (Quantity * Rate * HoursPerDay +"  
         " Quantity * OvertimeRate * OvertimeHoursPerDay)"
         " FROM JobCostSheet JCS, JobCrewEquipmentUsed JCEU"
         " WHERE JCEU.JobCostSheetKey = JCS.JobCostSheetKey"
         " AND JCEU.JobKey = JCS.JobKey"
         " AND JCEU.JobKey = %JobKey"
         " AND JCEU.JobCostSheetKey = %JobCostSheetKey",
         sizeof szTemp, 
         ppszPNames, 
         ppszParams, 
         100);

   pszSum = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);
	if (pszSum != NULL)
      {
      dSum = ItiStrToDouble (pszSum);
      ItiFmtFormatString (pszSum, szSumFmt,
                          sizeof szSumFmt, "Number,......", &usLen);
      }
   else
      dSum = 0.0;

   if (dSum == 0.0)
		{
      usRet = ItiStrReplaceParams (szTemp,
               " /* cbest.calcfn.CalcEquipmentDailyCost */ "
               "UPDATE JobCostSheet "
               " SET EquipmentDailyCost = 0 "
               " WHERE JobCostSheetKey = %JobCostSheetKey"
               " AND JobKey = %JobKey",
               sizeof szTemp, 
               ppszPNames, 
               ppszParams, 
               100);
      }
   else
      {
      /* -- Test if Production Rate is NON zero. */
      usRet = ItiStrReplaceParams (szTemp,
                " /* cbest.calcfn.CalcEquipmentDailyCost */ "
                " SELECT DisplayedProductionRate"
                " FROM JobCostSheet"
                " WHERE JobKey = %JobKey"
                " AND JobCostSheetKey = %JobCostSheetKey",
                sizeof szTemp, 
                ppszPNames, 
                ppszParams, 
                100);
      pszProdRate = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);
		if (pszProdRate != NULL)
         {
         dProdRate = ItiStrToDouble (pszProdRate);
         ItiFmtFormatString (pszProdRate, szProdRateFmt,
                             sizeof szProdRateFmt, "Number,......", &usLen);
         }
      else
         dProdRate = 0.0;

      if (dProdRate != 0.0)
			{
         sprintf (szTemp2,
                  " /* cbest.calcfn.CalcEquipmentDailyCost, non-zero ProdRate */ "
                  "UPDATE JobCostSheet"
                  " SET EquipmentDailyCost = ( %s / %s ) "
                  " WHERE JobCostSheetKey = %%JobCostSheetKey"
                  " AND JobKey = %%JobKey",
                  szSumFmt, szProdRateFmt);

         usRet = ItiStrReplaceParams
                    (szTemp, szTemp2, sizeof szTemp, ppszPNames, ppszParams, 100);
         }
      else
         { /* -- Production rate was zero, so do not divide by it. */
         sprintf (szTemp2,
                  " /* cbest.calcfn.CalcEquipmentDailyCost, zero ProdRate */ "
                  "UPDATE JobCostSheet"
                  " SET EquipmentDailyCost = %s "
                  " WHERE JobCostSheetKey = %%JobCostSheetKey"
                  " AND JobKey = %%JobKey ",
                  szSumFmt);

         usRet = ItiStrReplaceParams
                    (szTemp, szTemp2, sizeof szTemp, ppszPNames, ppszParams, 100);
         }/* End if (dProdRate != 0) */

      }/* End if (dSum == 0) */


   if (pszSum)
      ItiMemFree (hheap, pszSum);

   if (pszProdRate)
      ItiMemFree (hheap, pszProdRate);



   if (usRet)
      {
      ErrCalc ("Not all params available for CalcEquipDailyCost", szTemp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      {
      ErrCalc ("Bad Exec for CalcEquipDailyCost", szTemp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   /*--- do buffer updates after all calcs ---*/
   ItiDbUpdateBuffers ("JobCostSheet");
   return 0;
   }













/* Calculates the LaborDailyCost, EquipmentDaily Cost in the
 * JobCostSheet table
 *
 * Needs: JobCostSheetKey
 *        JobKey      
 *        JobCrewKey
 *
 */

int EXPORT CalcLbEqDailyCost (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
   USHORT i;

   i = CalcLaborDailyCost (hheap, ppszPNames, ppszParams);
   if (i)
      return i;
   i = CalcEquipmentDailyCost (hheap, ppszPNames, ppszParams);
   return i;
   }








/* Calculates the Unit Price in the
 * JobCostSheet table
 *
 * Needs: JobCostSheetKey
 *        JobKey
 *        
 *
 */

int EXPORT CalcJobCostShtUnitPrice (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
   HHEAP  hhp;
//   char   szTemp [512] = " ";
//   char   szTemp2 [512] = " ";
   USHORT usRet, usLen;
   DOUBLE  dRes = 0.0;
   PSZ    pszRes = NULL;
   PSZ    pszUnitPrice = NULL;
   CHAR   szUnitPriceFmt[32] = "";
   char   szZero[4] = " 0 ";
   BOOL   bGot = FALSE;

   hhp = ItiMemCreateHeap (MAX_HEAP_SIZE);
   if (hhp == NULL)
      {
      ItiErrWriteDebugMessage
         ("*** Failed to create a heap for use in cbest.CalcJobCostShtUnitPrice. ");
      return 13;
      }

//   /* -- First recalc the Job Cost Sheet's ProductionRate. */
//   CalcJobCostShtProductionRate (hhp, ppszPNames, ppszParams);



// In this function DisplayedProductionRate was 
// switched with the old ProductionRate to get the printed report
// to match the displayed screen. (3 occurrences changed)

   /* -- Test if Production Rate is NON zero. */
   usRet = ItiStrReplaceParams (szTemp,
             "/* cbest.calcfn.CalcJobCostShtUnitPrice, non-zero test */ "
             "SELECT DisplayedProductionRate"
             " FROM JobCostSheet"
             " WHERE JobKey = %JobKey"
             " AND JobCostSheetKey = %JobCostSheetKey",
             sizeof szTemp, 
             ppszPNames, 
             ppszParams, 
             100);

   pszRes = ItiDbGetRow1Col1(szTemp, hhp, 0, 0, 0);
	if (pszRes != NULL)
      {
      dRes = ItiStrToDouble (pszRes);
      ItiMemFree (hhp, pszRes);
      }
   else
      dRes = 0.0;

   if (dRes != 0.0)
		{
      usRet = ItiStrReplaceParams (szTemp,
                   " /* CalcJobCostShtUnitPrice, div DPR */"
                   " SELECT (1 + MarkUpPct) *"
                   " (((EquipmentDailyCost / convert (money,DisplayedProductionRate)) * ($1 + convert (money, EquipmentOverheadPct))) +"
                         " ((LaborDailyCost / convert (money,DisplayedProductionRate)) * ($1 + convert (money, LaborOverheadPct))) +"
                      " (MaterialsUnitCost * ($1 + convert (money, MaterialsOverheadPct))))"
                   " FROM JobCostSheet"
                   " WHERE JobKey = %JobKey "
                   " AND JobCostSheetKey = %JobCostSheetKey ",
                   sizeof szTemp, 
                   ppszPNames, 
                   ppszParams, 
                   100);
      }
   else
      { /* -- production rate was zero, so do NOT divide by it. */
      usRet = ItiStrReplaceParams (szTemp,
                   " /* cbest.calcfn.CalcJobCostShtUnitPrice */ "
                   " SELECT (1 + MarkUpPct) *"
                    " ((EquipmentDailyCost * ($1 + convert (money, EquipmentOverheadPct))) +"
                     " (LaborDailyCost * ($1 + convert (money, LaborOverheadPct))) +"
                     " (MaterialsUnitCost * ($1 + convert (money, MaterialsOverheadPct))))"
                    " FROM JobCostSheet"
                    " WHERE JobKey = %JobKey"
                    " AND JobCostSheetKey = %JobCostSheetKey ",
                   sizeof szTemp, 
                   ppszPNames, 
                   ppszParams, 
                   100);
      }


   if (usRet)
      {
      ErrCalc ("Not all params available for CalcJobCostShtUnitPrice", szTemp);
      ItiMemDestroyHeap (hhp);
      return 1;
      }

   pszUnitPrice = ItiDbGetRow1Col1(szTemp, hhp, 0, 0, 0);
   if (pszUnitPrice == NULL)
      {
      pszUnitPrice = &szZero[0];
      bGot = FALSE;
      }
   else
      {
      /* -- Must reformat the number without commas. */
      ItiFmtFormatString (pszUnitPrice, szUnitPriceFmt,
                          sizeof szUnitPriceFmt, "Number,......", &usLen);
      bGot = TRUE;
      }

   sprintf(szTemp2,
            " /* cbest.calcfn.CalcJobCostShtUnitPrice */ "
            "UPDATE JobCostSheet"
            " SET UnitPrice = %s "
            " WHERE JobKey = %%JobKey"
            " AND JobCostSheetKey = %%JobCostSheetKey",
            szUnitPriceFmt);

	if ( (bGot) && (pszUnitPrice != NULL) )
      {
      ItiMemFree (hhp, pszUnitPrice);
      }

   usRet = ItiStrReplaceParams
              (szTemp, szTemp2, sizeof szTemp, ppszPNames, ppszParams, 100);

   ItiDbBeginTransaction (hhp);
   if (ItiDbExecSQLStatement (hhp, szTemp))
      {
      ErrCalc ("Bad Exec for CalcJobCostShtUnitPrice", szTemp);
      ItiDbRollbackTransaction (hhp);
      ItiMemDestroyHeap (hhp);
      return 1;
      }
   ItiDbCommitTransaction (hhp);


   /*--- do buffer updates after all calcs ---*/
   ItiDbUpdateBuffers ("JobCostSheet");
   ItiDbUpdateBuffers ("Task");

   ItiMemDestroyHeap (hhp);
   return 0;
   }





/* Calculates the Unit Price and ExtendedAmount
 * of the JobItem
 *
 * This function is called when a job item is added
 * to the job, and after all the associated CostBasedEstimates,
 * CostSheets, Crews, Materials, Equipment, Laborers, etc. have been copied
 *
 * Needs: JobKey
 *        JobItemKey
 *        
 *
 * 11-17-1992: Added code to round the unit price.  BSR # 920923-4101.
 * mdh.
 */

int EXPORT CalcJobItem (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
   USHORT usRet;

   usRet = CalcJobItemUnitPrice (hheap, ppszPNames, ppszParams);
   ItiDbUpdateBuffers ("JobItem");
   ItiDbUpdateBuffers ("JobBreakdown");
   ItiDbUpdateBuffers ("Job");

   return usRet;
   } /* End of Function */






 int EXPORT SetJobItemUnitPriceToZero (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
//   char   szTemp [256];
   USHORT usRet;

   usRet = ItiStrReplaceParams (szTemp, 
                           "/* cbest.calcfn...PriceToZero */ "
                           "UPDATE JobItem "
                            "SET UnitPrice = 0.0 "
                            "WHERE JobKey = %JobKey "
                            "AND JobItemKey = %JobItemKey "
                           "UPDATE JobItem "
                            "SET ExtendedAmount = 0.0 "
                            "WHERE JobKey = %JobKey "
                            "AND JobItemKey = %JobItemKey ",
                           sizeof szTemp,
                           ppszPNames,
                           ppszParams,
                           100);


   if (usRet)
      {
      ErrCalc ("Not all params available for SetJobItemUnitPriceToZero", szTemp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      {
      ErrCalc ("Bad Exec for SetJobItemUnitPriceToZero", szTemp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   /*--- do buffer updates after all calcs ---*/
   ItiDbUpdateBuffers ("JobItem");

  }













 int EXPORT UpDateJobItemEstMethod (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
//   char   szTemp [300];
   USHORT usRet;

   usRet = ItiStrReplaceParams (szTemp, 
                           " /* cbest.calcfn.UpDateJobItemEstMethod */ "
                           "UPDATE JobItem "
                           " SET ItemEstimationMethodKey = "
                           " (SELECT ItemEstimationMethodKey "
                           " FROM ItemEstimationMethod "
                           " WHERE ItemEstimationMethodID = 'Ad-Hoc') "
                           " WHERE JobKey = %JobKey "
                           " AND JobItemKey = %JobItemKey ",
                           sizeof szTemp,
                           ppszPNames,
                           ppszParams,
                           100);


   if (usRet)
      {
      ErrCalc ("Not all params available for UpDateJobItemEstMethod", szTemp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      {
      ErrCalc ("Bad Exec for UpDateJobItemEstMethod", szTemp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   /*--- do buffer updates after all calcs ---*/
   ItiDbUpdateBuffers ("JobItem");
//   ItiDbUpdateBuffers ("JobBreakdown");
//   ItiDbUpdateBuffers ("Job");

  }






 int EXPORT UpDateJobItemCostBasedEstimate (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
//   char   szTemp [210];
   USHORT usRet;

   usRet = ItiStrReplaceParams (szTemp, 
                           " UPDATE JobCostBasedEstimate "
                           " SET Active =  0 "
                           " WHERE JobKey = %JobKey "
                           " AND JobItemKey = %JobItemKey "
                           " AND JobItemCostBasedEstimateKey = %JobItemCostBasedEstimateKey",
                           sizeof szTemp,
                           ppszPNames,
                           ppszParams,
                           100);
   if (usRet)
      {
      ErrCalc ("Not all params available for UpDateJobItemCostBasedEstimate", szTemp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      {
      ErrCalc ("Bad Exec for UpDateJobItemCostBasedEstimate", szTemp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   /*--- do buffer updates after all calcs ---*/
   ItiDbUpdateBuffers ("JobItemCostBasedEstimate");
//   ItiDbUpdateBuffers ("JobBreakdown");
//   ItiDbUpdateBuffers ("Job");

  }





int EXPORT DoJobCrewsExist (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey)
   {
//   char  szTemp [124];
   PSZ   pszJobCrewExists;


   sprintf(szTemp, "SELECT JobCrewKey "
                   "FROM JobCrewUsed "
                   "WHERE JobKey = %s "
                   "AND JobCostSheetKey = %s",
                    pszJobKey, pszJobCostSheetKey);

   pszJobCrewExists = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);

   /* If the JobCostSheet has JobCrews associated with it, return 0,       */
   /* to indicate that they exist.  If there are no JobCrews               */     
   /* associated with the JobCostSheet, return 1, so that the              */
   /* CalcLaborDailyCost and CalcEquipmentDailyCost functions will not be  */
   /* called and the JobCostSheet will not be updated with a NULL insert   */

   if (pszJobCrewExists != NULL)
      {
      ItiMemFree (hheap, pszJobCrewExists);
      return 0;
      }
   else return 1;

   }





int EXPORT DoJobMaterialsExist (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey)
   {
//   char  szTemp [124];
   PSZ   pszJobMaterialExists;


   sprintf(szTemp, "SELECT MaterialKey "
                   "FROM JobMaterialUsed "
                   "WHERE JobKey = %s "
                   "AND JobCostSheetKey = %s",
                    pszJobKey, pszJobCostSheetKey);

   pszJobMaterialExists = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);

   /* If the JobCostSheet has JobMaterials associated with                 */
   /* it, return 0, to indicate that they exist.                           */     
   /* If there are no JobMaterials associated with the JobCostSheet,       */
   /* return 1, so that CalcMaterialsCost will not be called and the       */
   /* JobCostSheet will be not updated with a Null insert.                 */

   if (pszJobMaterialExists != NULL)
      {
      ItiMemFree (hheap, pszJobMaterialExists);
      return 0;
      }
   else return 1;

   }




int EXPORT DoesEquipExist (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey)
   {
//   char  szTemp [124];
   PSZ   pszJobEquipExists;

   sprintf(szTemp, "SELECT EquipmentKey "
                   "FROM JobCrewEquipmentUsed "
                   "WHERE JobKey = %s "
                   "AND JobCostSheetKey = %s",
                    pszJobKey, pszJobCostSheetKey);

   pszJobEquipExists = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);

   /* If the JobCrewEquipmentUsed table has at least one piece of equipment */
   /* associated with a particular JobCostSheet, return 0.                  */
   /* If there are no pieces of equipment associated with this JobCostSht,  */
   /* return 1, so that CalcEquipmentDailyCost will not be called and the   */
   /* JobCostSheet will be not updated with a Null insert.                  */

   if (pszJobEquipExists != NULL)
      {
      ItiMemFree (hheap, pszJobEquipExists);
      return 0;
      }
   else return 1;

   }





int EXPORT DoLaborersExist (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey)
   {
//   char  szTemp [124];
   PSZ   pszJobLaborersExist;


   sprintf(szTemp, "SELECT LaborerKey "
                   "FROM JobCrewLaborUsed "
                   "WHERE JobKey = %s "
                   "AND JobCostSheetKey = %s",
                    pszJobKey, pszJobCostSheetKey);

   pszJobLaborersExist = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);

   /* If the JobCrewLaborUsed table has at least one laborer           */
   /* associated with a particular JobCostSheet, return 0.             */
   /* If there are no laborers  associated with this CostSheet,        */
   /* return 1, so that CalcLaborDailyCost will not be called and the  */
   /* JobCostSheet will be not updated with a Null insert.             */

   if (pszJobLaborersExist != NULL)
      {
      ItiMemFree (hheap, pszJobLaborersExist);
      return 0;
      }
   else return 1;

   }





 int EXPORT SetMaterialsUnitCostToZero (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
//   char   szTemp [124];
   USHORT usRet;


   usRet = ItiStrReplaceParams (szTemp, 
                              "UPDATE JobCostSheet "
                              "SET MaterialsUnitCost = 0.00 "
                              "WHERE JobKey = %JobKey "
                              "AND JobCostSheetKey = %JobCostSheetKey",
                              sizeof szTemp,
                              ppszPNames,
                              ppszParams,
                              100);
   if (usRet)
      {
      ErrCalc ("Not all params available for SetMaterialsUnitCostToZero", szTemp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      {
      ErrCalc ("Bad Exec for SetMaterialsUnitCostToZero", szTemp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   /*--- do buffer updates after all calcs ---*/
   ItiDbUpdateBuffers ("JobCostSheet");

  }






 int EXPORT SetEquipmentDailyCostToZero (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
//   char   szTemp [140];
   USHORT usRet;


   usRet = ItiStrReplaceParams (szTemp, 
                              "UPDATE JobCostSheet "
                              " SET EquipmentDailyCost = 0 "
                              " WHERE JobKey = %JobKey "
                              " AND JobCostSheetKey = %JobCostSheetKey " ,
                              sizeof szTemp,
                              ppszPNames,
                              ppszParams,
                              100);


   if (usRet)
      {
      ErrCalc ("Not all params available for SetEquipmentDailyCostToZero", szTemp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      {
      ErrCalc ("Bad Exec for SetEquipmentDailyCostToZero", szTemp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   /*--- do buffer updates after all calcs ---*/
   ItiDbUpdateBuffers ("JobCostSheet");

  }




 int EXPORT SetLaborDailyCostToZero (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
//   char   szTemp [140];
   USHORT usRet;


   usRet = ItiStrReplaceParams (szTemp, 
                              "UPDATE JobCostSheet "
                              "SET LaborDailyCost = 0 "
                              "WHERE JobKey = %JobKey "
                              "AND JobCostSheetKey = %JobCostSheetKey",
                              sizeof szTemp,
                              ppszPNames,
                              ppszParams,
                              100);


   if (usRet)
      {
      ErrCalc ("Not all params available for SetLaborDailyCostToZero", szTemp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      {
      ErrCalc ("Bad Exec for SetLaborDailyCostToZero", szTemp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   /*--- do buffer updates after all calcs ---*/
   ItiDbUpdateBuffers ("JobCostSheet");

  }

// =========================================================================
//keep

DOUBLE EXPORT Round (DOUBLE dVal)
   {
   DOUBLE dInt, dTmp2, dRet, dFrac, dnu, dItrm;
   
   // dInt  = floor(dVal);
   // dFrac = dVal - dInt;
   dFrac = modf(dVal, &dInt);

   /* The following IF clause provides rounding to the 1st, 2nd, or 3rd
    * decimal place of the given number (dVal) rather than to its
    * integer portion. The cbest initialization function, ItiDllInitDll, 
    * checks the environment for the variable "ROUNDPLACE" which should 
    * be equal to 1, 2, or 3 for 0.x, 0.xx, or 0.xxx place rounding.
    * This was done at the request of MNDot to handle small quantities.
    */
   if ((usRoundToDecimalPlace > 0) && (usRoundToDecimalPlace < 4))
      {
      switch (usRoundToDecimalPlace)
         {
         case 1:
            dnu = 10.0;
            break;

         case 2:
            dnu = 100.0;
            break;

         case 3:
         default:
           dnu = 1000.0;
           break;
         }

      dTmp2 = dFrac * dnu;
      dRet = modf(dTmp2, &dItrm);
      if (dRet >= 0.5)
         dItrm += 1.0;

      dInt += (dItrm / dnu);

      if (dInt == 0.0)
         return 1.0;
      else
         return dInt;
      } /* end of special decimal place rounding rounding */

   dTmp2 = dVal + 0.5;
   dTmp2 = floor(dTmp2);

   if (dInt == dTmp2)
      dRet = dInt;
   else
      dRet = (dInt+1.0);

   if (dRet == 0.0)
      return 1.0;
   else
      return dRet;
   } /* End of Round function */


// BSR 930037
 /*
 -- FUNCTIONS: CalcJobCostShtProductionRate, CalcJobCostShtDuration 
 --
 -- TotalTaskQty
 --  = JobItemQty * QtyPerItemUnit
 --
 -- AdjustedDuration
 --  = ( (TotalTaskQty / ProductionRate) / GranularityConversionFactor )
 --
 -- Duration (In units of Days)
 --  = ROUND(AdjustedDuration) * GranularityConversionFactor
 --
 --   NOTE: The CEILING function could be used rather than ROUND because 
 --         rounding in some cases would require a crew to have a
 --         production rate greater than the what was entered.
 --         For example, say a crew production rate is given as 10 units
 --         per day and the total task quantity is 94 units. 
 --         The time to complete the task is then 9.4 days; however,
 --         ROUNDING yields 9 days to complete the task which then
 --         requires the crew's production rate to be 10.44 units (94/9).
 --         Using the CEILING function yields 10 days to complete the
 --         task which then requires the crew's production rate to be
 --         9.4 units (94/10) which is less than given production rate
 --         of 10 units per day.  The assumption is that the given
 --         production rate for a crew is what can be done under the
 --         best of working conditions (good weather, materials delivered
 --         on time, nobody is out sick, etc.).
 --
 --         The GranularityConversionFactor factor is the number of Days 
 --         per granularity unit.  These factors are based upon the initially
 --         given production rate being in units of the estimated JobItem 
 --         per day (ref. page 88 of the CES System Manager's Guide).
 --         The following are the values:
 --
 --         Enum  Unit    GranularityConversionFactor
 --         ----  ----    ---------------------------
 --          30   Hour    (1/(HoursPerDay + OvertimeHoursPerDay))
 --          35   HalfDay 0.5 
 --          40   Day     1
 --
 --    To be omited:
 --          50   Week    5
 --          60   Month   20.83 = (50 week/year * 5 day/week) / 12 month/year
 --          70   Quarter 62.5
 --          80   Year    250
 --
 --         The Enum values for granularity are from the itifmt/enum.c file.
 --
 --  
 -- DisplayedProductionRate
 --  = TotalTaskQty / Duration
 --
 -- --------------------------------------------------------------------
 -- The following is an example using CEILING and a production granularity
 -- of WEEK:
 --
 -- TotalTaskQty 
 --         = JobItemQty * QtyPerItemUnit
 --  10,000 = 5,000      * 2
 --
 -- AdjustedDuration
 --       = ((TotalTaskQty / ProductionRate)   / GranularityConversionFactor)
 --  16.7 = ((10,000 / 120{in units per day} ) / 5 {A week contains 5 days.}
 --     Using a production granularity of week, a quantity of 10,000 units
 --     at a given production rate of 120 units per day will take 16.7 weeks.
 --
 -- Duration (In units of Days)
 --       = CEILING(AdjustedDuration) * GranularityConversionFactor
 --       = CEILING( 16.67 ) * 5
 --  85.0 = 17.0 * 5
 --
 -- DisplayedProductionRate
 --        = TotalTaskQty / Duration
 -- 117.65 = 10,000 / 85.0
 --    
 */



DOUBLE EXPORT CalcJobCostShtDuration (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
//   char   szTemp [400] = "";
//   char   szTemp2 [400] = "";
   USHORT usRet, usCol, usGran, usDurationDBvalue;
   DOUBLE dProdRate, dHours, dTotalTaskQty, dConversionFactor;
   DOUBLE dDuration, dAdjustedDuration, dInterm;
//   DOUBLE dGranularizedProdRate;
   PSZ    *ppszRes = NULL;
   PSZ    psz = NULL;
   BOOL bProdRateAdjUp;


   usRet = ItiStrReplaceParams (szTemp,
             "/* cbest.calcfn.CalcJobCostShtDuration */ "
             "SELECT JCS.ProductionRate, "
             " (JCS.HoursPerDay + JCS.OvertimeHoursPerDay), "
             " (JCS.QuantityPerItemUnit * JI.Quantity), "
             " JCS.ProductionGranularity "
             " FROM JobCostSheet JCS, JobItem JI"
             " WHERE JCS.JobKey = %JobKey"
             " AND JCS.JobCostSheetKey = %JobCostSheetKey"
             " AND JCS.JobItemKey = JI.JobItemKey"
             " AND JCS.JobKey = JI.JobKey ",
             sizeof szTemp, 
             ppszPNames, 
             ppszParams, 
             100);

   ppszRes = ItiDbGetRow1(szTemp, hheap, 0, 0, 0, &usCol);
	if (ppszRes != NULL)
      {
      dProdRate = ItiStrToDouble (ppszRes[0]);
      dHours = ItiStrToDouble (ppszRes[1]);
      dTotalTaskQty = ItiStrToDouble (ppszRes[2]);
      ItiStrToUSHORT (ppszRes[3], &usGran);
      ItiMemFree (hheap, ppszRes);
      }

   switch (usGran)
      {
      case 30: if (dHours != 0.0)                  /* hours   */
                  dConversionFactor = (1.0 / dHours);
               else
                  /* default to an eight hour day. */
                  dConversionFactor = (1.0 / 8.0);
               break;

      case 35: dConversionFactor = HALF_DAY;       /* half days */
               break;

      case 50: dConversionFactor = WEEK;           /* week    */
               break;

      case 60: dConversionFactor = MONTH;          /* month   */
               break;

      case 70: dConversionFactor = QTR;           /* quarter */
               break;

      case 80: dConversionFactor = YEAR;          /* year    */
               break;

      case 40:                                     /* day     */
      default:                                    /* all else */
               dConversionFactor = 1.0;           
               break;
      }

   if (dProdRate == 0.0)
       dProdRate = 1.0;

   /*--- Allow a change to be made to the ProductionRate calc adjustment. ---*/
   DosScanEnv ("JOBPRODRATEUP", &psz);
   if (NULL != psz)
      {
      if ((psz[0] == 'Y') || (psz[0] == 'y'))
         bProdRateAdjUp = TRUE;   /* Use ceil function.  */
      else
         bProdRateAdjUp = FALSE;  /* Use round function. */
      }
   else
      bProdRateAdjUp = FALSE;  /* Use round function. */



   dAdjustedDuration = (dTotalTaskQty / dProdRate) / dConversionFactor;
   if (bProdRateAdjUp)
      {
      dDuration = (ceil( dAdjustedDuration )) * dConversionFactor;
      /* The above value is returned for use in calculations.    */
      /* The value below (rounded to the INT value) is stored    */
      /* in the database, that is the way the column is defined. */ 
      usDurationDBvalue = (USHORT) ceil(dDuration);
//      sprintf (szTemp2, "--- Ceiling used: %f ", dDuration);
//      ItiErrWriteDebugMessage (szTemp2);
      }
   else
      {
      dDuration = (Round( dAdjustedDuration )) * dConversionFactor;
      if (modf(dDuration, &dInterm) >= 0.50)
         usDurationDBvalue = (USHORT) ceil(dDuration);
      else
         {
         usDurationDBvalue = (USHORT) floor(dDuration);
         }
//      sprintf (szTemp2, "--- Rounding used: %f ", dDuration);
//      ItiErrWriteDebugMessage (szTemp2);
      }

   /* -- Test if the computed value exceeds database smallint size. */
   /* -- The design specifies a limit of 9,999.                     */
   if (dDuration > 32765.0)
      {
      sprintf (szTemp2,
         "-- Job cost sheet's Duration computed to be too large: %f  "
         " The value was reset to 1."
         " Values used: ProdRate %f, Hours %f, TotalTaskQty %f, ConversionFactor %f "
         , dDuration, dProdRate, dHours, dTotalTaskQty, dConversionFactor);
      ItiErrWriteDebugMessage (szTemp2);
      dDuration = 1.0;
      usDurationDBvalue = 1;
      }

   if (dDuration < 1.0) 
      {                 /* Added this check for Minnesota, JUNE 94 */
      sprintf (szTemp2,
         "-- Job cost sheet's Duration computed to be less than one: %f  "
         " The database whole value was reset to 1."
         " Values used: ProdRate %f, Hours %f, TotalTaskQty %f, ConversionFactor %f "
         , dDuration, dProdRate, dHours, dTotalTaskQty, dConversionFactor);
      ItiErrWriteDebugMessage (szTemp2);
      if (dDuration == 0.0)
         dDuration = 1.0;
      usDurationDBvalue = 1;
      }


   sprintf (szTemp2,
            "/* cbest.calcfn.CalcJobCostShtDuration */ "
            "UPDATE JobCostSheet"
            " SET Duration = %i "
            " WHERE JobKey = %%JobKey "
            " AND JobCostSheetKey = %%JobCostSheetKey "
     " UPDATE JobItem"
     " SET Duration = %i "
     " WHERE JobKey = %%JobKey "
      " AND JobItemKey = (SELECT JobItemKey "
                        " FROM JobCostSheet "
                        " WHERE JobKey = %%JobKey "
                        " AND JobCostSheetKey = %%JobCostSheetKey )"
       , usDurationDBvalue, usDurationDBvalue);

   usRet = ItiStrReplaceParams
              (szTemp, szTemp2, sizeof szTemp, ppszPNames, ppszParams, 100);

   if (ItiDbExecSQLStatement (hheap, szTemp))
      return 1.0;

   return dDuration;
   } /* End of Function CalcJobCostShtDuration */





/* -- Needs the following keys:
   -- JobKey
   -- JobCostSheetKey
 */
int EXPORT CalcJobCostShtProductionRate (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
//   char   szTemp [300] = "";
//   char   szTemp2 [300] = "";
   USHORT usRet;
   DOUBLE dTotalTaskQty, dDuration, dDisplayedProdRate, dUserProdRate;
   PSZ    pszRes = NULL;
   HHEAP  hp;

   hp = ItiMemCreateHeap (MAX_HEAP_SIZE);

   usRet = ItiStrReplaceParams (szTemp,
             "/* cbest.calcfn.CalcJobCostShtProductionRate "
             "(user entered value) */ "
             "SELECT ProductionRate "
             " FROM JobCostSheet "
             " WHERE JobKey = %JobKey"
             " AND JobCostSheetKey = %JobCostSheetKey ",
             sizeof szTemp, 
             ppszPNames, 
             ppszParams, 
             100);
   pszRes = ItiDbGetRow1Col1 (szTemp, hp, 0, 0, 0);
	if (pszRes != NULL)
      {
      dUserProdRate = ItiStrToDouble (pszRes);
      ItiMemFree (hp, pszRes);
      }
   else
      dUserProdRate = 1.0;


   usRet = ItiStrReplaceParams (szTemp,
             "/* cbest.calcfn.CalcJobCostShtProductionRate */ "
             "SELECT (JCS.QuantityPerItemUnit * JI.Quantity) "
             " FROM JobCostSheet JCS, JobItem JI"
             " WHERE JCS.JobKey = %JobKey"
             " AND JCS.JobCostSheetKey = %JobCostSheetKey"
             " AND JCS.JobItemKey = JI.JobItemKey"
             " AND JCS.JobKey = JI.JobKey ",
             sizeof szTemp, 
             ppszPNames, 
             ppszParams, 
             100);

   pszRes = ItiDbGetRow1Col1 (szTemp, hp, 0, 0, 0);
	if (pszRes != NULL)
      {
      dTotalTaskQty = ItiStrToDouble (pszRes);
      ItiMemFree (hp, pszRes);
      }
   else
      dTotalTaskQty = 0.0;

   dDuration = CalcJobCostShtDuration (hp, ppszPNames, ppszParams);

   if ((dTotalTaskQty != 0.0) && (dDuration != 0.0))
		{
      /* -- If TotalTaskQty less than the user entered production */
      /* -- rate then use that user entered value.                */
      if (dTotalTaskQty < dUserProdRate)
         dDisplayedProdRate = (dUserProdRate / dDuration);
      else
         dDisplayedProdRate = (dTotalTaskQty / dDuration);

      sprintf (szTemp2,
                 " /* cbest.calcfn.CalcJobCostShtProductionRate */ "
                 " UPDATE JobCostSheet"
                 " SET DisplayedProductionRate= %f "
                 " WHERE JobKey = %%JobKey "
                 " AND JobCostSheetKey = %%JobCostSheetKey ",
                 dDisplayedProdRate);

      usRet = ItiStrReplaceParams (szTemp, szTemp2,
                 sizeof szTemp, 
                 ppszPNames, 
                 ppszParams, 
                 100);
      }
   else
      { /* -- We had zeros so just set the ProdRate to 1. */
        /* -- Does a default of 1 here make sense?              */
      usRet = ItiStrReplaceParams (szTemp,
                 " /* cbest.calcfn.CalcJobCostShtProductionRate */ "
                 " UPDATE JobCostSheet"
                 " SET DisplayedProductionRate = 1.0 "
                 " WHERE JobKey = %JobKey "
                 " AND JobCostSheetKey = %JobCostSheetKey ",
                 sizeof szTemp, 
                 ppszPNames, 
                 ppszParams, 
                 100);
      }


   if (usRet)
      {
      ErrCalc ("Not all params available for CalcJobCostShtProductionRate", szTemp);
      return 1;
      }

   ItiDbBeginTransaction (hp);
   if (ItiDbExecSQLStatement (hp, szTemp))
      {
      ErrCalc ("Bad Exec for CalcJobCostShtProductionRate", szTemp);
      ItiDbRollbackTransaction (hp);
      ItiMemDestroyHeap(hp);
      return 1;
      }
   ItiDbCommitTransaction (hp);

   /*--- do buffer updates after all calcs ---*/
   ItiDbUpdateBuffers ("JobCostSheet");

   ItiMemDestroyHeap(hp);
   return 0;
   }





int EXPORT ResetProdRate (HWND hwnd, BOOL bForAJob, PSZ pszInitialGranularity)
   {
   HHEAP hheap, hp;
   HWND  hwndStatic;
   PSZ   pszJobKey = NULL;
   PSZ   pszJobCostSheetKey = NULL;
   PSZ   pszCostSheetKey = NULL;
   PSZ   pszProdGran = NULL;
   PSZ   psz = NULL;
   CHAR  szTemp [512] = "";
   USHORT usGran = 0;
   USHORT usInitGran = 0;
   DOUBLE dConversionFactor = 1.0;
   HQRY  hqry;
   USHORT uNumCols, uState, uError;
   PSZ    *ppszRow, pszRes = NULL;
   PSZ    *ppszLocPNames = NULL;
   PSZ    *ppszLocParams = NULL;
   DOUBLE adConvFactor[7][7] =  {
              /*                  HRS,                   HALF_DAY,              DAY,                     WEEK,                     MONTH,                     QTR,                     YEAR    */
/* HRS */  {                      1.0,         DEF_HRS * HALF_DAY,    DEF_HRS * DAY,           DEF_HRS * WEEK,           DEF_HRS * MONTH,           DEF_HRS * QTR,           DEF_HRS * YEAR },
/* HLF */  { 1.0/(HALF_DAY * DEF_HRS),                        1.0,     1.0/HALF_DAY,    (1.0/HALF_DAY) * WEEK,    (1.0/HALF_DAY) * MONTH,    (1.0/HALF_DAY) * QTR,    (1.0/HALF_DAY) * YEAR },
/* DAY */  {      1.0/(DAY * DEF_HRS),                 (HALF_DAY),              1.0,                     WEEK,                     MONTH,                     QTR,                     YEAR },
/* WK  */  {     1.0/(WEEK * DEF_HRS),  1.0/(WEEK*(1.0/HALF_DAY)),         1.0/WEEK,                      1.0,        (1.0/WEEK) * MONTH,        (1.0/WEEK) * QTR,        (1.0/WEEK) * YEAR },
/* MTH */  {    1.0/(MONTH * DEF_HRS), 1.0/(MONTH*(1.0/HALF_DAY)),        1.0/MONTH,       (1.0/MONTH) * WEEK,                       1.0,       (1.0/MONTH) * QTR,       (1.0/MONTH) * YEAR },
/* QTR */  {      1.0/(QTR * DEF_HRS),   1.0/(QTR*(1.0/HALF_DAY)),          1.0/QTR,        (1.0/QTR)  * WEEK,        (1.0/QTR)  * MONTH,                     1.0,         (1.0/QTR) * YEAR },
/* YR  */  {     1.0/(YEAR * DEF_HRS),  1.0/(YEAR*(1.0/HALF_DAY)),         1.0/YEAR,        (1.0/YEAR) * WEEK,        (1.0/YEAR) * MONTH,        (1.0/YEAR) * QTR,                      1.0 }
      };


   hwndStatic = QW(hwnd, ITIWND_OWNERWND, 1, 0, 0);

   hheap            = (HHEAP) WinSendMsg (hwndStatic, WM_ITIWNDQUERY,
                      MPFROMSHORT (ITIWND_HEAP), 0);

   pszProdGran = (PSZ) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_DATA),
                                   MPFROM2SHORT (0, cProductionGranularity));
   ItiStrToUSHORT (pszProdGran, &usGran);
   ItiStrToUSHORT (pszInitialGranularity, &usInitGran);
   if (usGran == usInitGran)
      return 0;

   switch (usInitGran)
      {
      case 30: usInitGran = 0;  /* hours   */
               break;
      case 35: usInitGran = 1;  /* half day */
               break;
      case 50: usInitGran = 3;  /* week    */
               break;
      case 60: usInitGran = 4;  /* month   */
               break;
      case 70: usInitGran = 5;  /* quarter */
               break;
      case 80: usInitGran = 6;  /* year    */
               break;
      case 40:                  /* day     */
      default: usInitGran = 2;
               break;
      } /* end of switch usInitGran */


   switch (usGran)
      {
      case 30: usGran = 0;  /* hours   */
               break;
      case 35: usGran = 1;  /* half day */
               break;
      case 50: usGran = 3;  /* week    */
               break;
      case 60: usGran = 4;  /* month   */
               break;
      case 70: usGran = 5;  /* quarter */
               break;
      case 80: usGran = 6;  /* year    */
               break;
      case 40:              /* day     */
      default: usGran = 2;
               break;
      } /* end of switch usGran */

   /* -- Lookup and assign the multiplier to reset */
   /* -- the value of the daily production rate.   */
   dConversionFactor = adConvFactor[usInitGran][usGran];

   if (bForAJob)
      {
      pszJobKey = (PSZ) WinSendMsg (hwndStatic, WM_ITIWNDQUERY,
                                    MPFROMSHORT (ITIWND_DATA),
                                    MPFROM2SHORT (0, cJobKey));
      pszJobCostSheetKey = (PSZ) WinSendMsg (hwndStatic, WM_ITIWNDQUERY,
                                      MPFROMSHORT (ITIWND_DATA),
                                      MPFROM2SHORT (0, cJobCostSheetKey));

      if (pszJobCostSheetKey && pszJobKey)
         {
         sprintf (szTemp, "/* ResetProdRate */ "
                          " UPDATE JobCostSheet "
                          " SET ProductionRate = ProductionRate * %f "
                          " WHERE JobKey = %s "
                          " AND JobCostSheetKey = %s",
                          dConversionFactor, pszJobKey, pszJobCostSheetKey);
         }
      else
         sprintf (szTemp, " SELECT 'Error: Missing Job or JobCstSht keys in CBEst ResetProdRate.' ");
      }
   else
      {
      pszCostSheetKey = (PSZ) WinSendMsg (hwndStatic, WM_ITIWNDQUERY,
                                      MPFROMSHORT (ITIWND_DATA),
                                      MPFROM2SHORT (0, cCostSheetKey));
      if (pszCostSheetKey)
         {
         sprintf (szTemp, "/* ResetProdRate */ "
                          "UPDATE CostSheet "
                          " SET ProductionRate = ProductionRate * %f "
                          " WHERE CostSheetKey = %s ",
                          dConversionFactor, pszCostSheetKey);
         ItiMemFree(hheap, pszCostSheetKey);
         }
      else
         sprintf (szTemp, " SELECT 'Error: Missing CostSheet key in CBEst ResetProdRate.' ");
      }

   ItiDbBeginTransaction (hheap);
//   if (ItiDbExecSQLStatement (hheap, szTemp))
//      {
//      ErrCalc ("Failed Exec for ResetProdRate ", szTemp);
//      ItiDbRollbackTransaction (hheap);
//      return 13;
//      }
   /*--- Exec the query ---*/
   if (!(hqry = ItiDbExecQuery (szTemp, hheap, 0, 0, 0, &uNumCols, &uState)))
      return 13;
   /*--- Force the completion of the query. ---*/
   while (ItiDbGetQueryRow (hqry, &ppszRow, &uError))
      {
      ItiFreeStrArray (hheap, ppszRow, uNumCols);
      }

   if (bForAJob)
      {
      hp = ItiMemCreateHeap (1024);

      ppszLocPNames = ItiStrMakePPSZ (hp, 3, "JobKey", "JobCostSheetKey",  NULL);
      ppszLocParams = ItiStrMakePPSZ (hp, 3, pszJobKey, pszJobCostSheetKey, NULL);
      CalcJobCostShtProductionRate (hp, ppszLocPNames, ppszLocParams);

     //  sprintf (szTemp, "/* ResetProdRate */ "
     //                   " SELECT ProductionRate "
     //                   " FROM JobCostSheet "
     //                   " WHERE JobKey = %s "
     //                   " AND JobCostSheetKey = %s",
     //                   pszJobKey, pszJobCostSheetKey);
     // 
     //  pszRes = ItiDbGetRow1Col1 (szTemp, hp, 0, 0, 0);
     //  WinSetDlgItemText (hwnd, DID_PRODRATE, pszRes);

      ItiMemFree(hheap, pszJobKey);
      ItiMemFree(hheap, pszJobCostSheetKey);

      ItiFreeStrArray (hp, ppszLocPNames, 3);
      ItiFreeStrArray (hp, ppszLocParams, 3);
      ItiMemDestroyHeap(hp);
      }

   ItiDbCommitTransaction (hheap);

   return 0;
   }/* End Function ResetProdRate */



/* ------------------------------------------------------------- */
/* --   FUNCTION: ItiCesQueryJobCBEstActive                      */
/* -- PARAMETERS: hheap                                          */
/* --             ppszNames      (JobKey, JobItemKey)            */
/* --             ppszValues                                     */
/* --                                                            */
/* -- DESCRIPTION:                                               */
/* --    Determines if a CBEst is active for a given job item.   */
/* --                                                            */
/* -- RETURN: TURE or FALSE                                      */
/* ------------------------------------------------------------- */
BOOL EXPORT ItiCesQueryJobCBEstActive (HHEAP hheap, 
                                       PPSZ  ppszNames, 
                                       PPSZ  ppszValues)
   {
//   char     szTemp [640] = "";
   PSZ      psz;
   BOOL     bRet;
   USHORT   i;

   i = ItiStrReplaceParams (szTemp, 
              " SELECT JobItemCostBasedEstimateKey"
              " FROM JobCostBasedEstimate"
              " WHERE JobKey = %JobKey"
              " AND JobItemKey = %JobItemKey"
              " AND 0 < "
              " (SELECT COUNT (JCS.JobItemCostBasedEstimateKey)"
              " FROM JobCostSheet JCS"
              " WHERE JCS.JobItemCostBasedEstimateKey ="
                     " JobCostBasedEstimate.JobItemCostBasedEstimateKey)"
              " AND Active = 1"
              , sizeof szTemp,
              ppszNames,
              ppszValues,
              100);
   if (i)
      {
      ItiErrWriteDebugMessage ("ERROR: ItiCesQueryJobCBEstActive is missing keys.");
      return FALSE;
      }

   psz = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);
   bRet = !(psz == NULL || *psz == '\0');
   if (psz != NULL)
      ItiMemFree (hheap, psz);

   return bRet;
   }/* End of Function ItiCesQueryJobCBEstActive */




/* ------------------------------------------------------------- */
/* --   FUNCTION: GetJobCostSheetsForJobItem                     */
/* -- PARAMETERS: hheap                                          */
/* --             pszJobKey                                      */
/* --             pszJobItemKey                                  */
/* --             pszCBEstKey                                    */
/* --             **pppszJCstShtKeys                             */
/* --                                                            */
/* -- DESCRIPTION:                                               */
/* --    Obtains all of the JobCostSheets (tasks) associated     */
/* --    with an Active JobCostBasedEstimate for a particular    */
/* --    JobItem and stores them in an array.                    */ 
/* --                                                            */
/* -- RETURN: TURE  Array was built.                             */
/* --         FALSE Failed to build array.                       */
/* ------------------------------------------------------------- */
BOOL EXPORT GetJobCostSheetsForJobItem (HHEAP hheap,
                                        PSZ   pszJobKey,
                                        PSZ   pszJobItemKey, 
                                        PSZ   pszCBEstKey,
                                        PSZ   **pppszJCstShtKeys)
   {
//   char    szTemp [512] = "";
   USHORT  usErr;
   USHORT  usCols [2] = {1,0};

   sprintf (szTemp, "/* CBEst calc.c GetJobCostSheetsForJobItem */ "
                    "SELECT JobCostSheetKey " 
                    "FROM JobCostSheet "
                    "WHERE JobKey = %s "
                    "AND JobItemKey = %s "
                    "AND JobItemCostBasedEstimateKey = %s",
                     pszJobKey, pszJobItemKey, pszCBEstKey);

   usErr = ItiDbBuildSelectedKeyArray2
                              (hheap, 0, szTemp, pppszJCstShtKeys, usCols);
   if (usErr != 0)
      return FALSE;

   return TRUE;
   }/* End of Function GetJobCostSheetsForJobItem */







USHORT EXPORT ReCalcJobCstShtCrewEquipment
          (PSZ pszJobKey, PSZ pszJobCostSheetKey, PSZ pszJobCrewKey)
   {
   HQRY   hqry;
   HHEAP  hhpLocal;
   USHORT uCols, uErr, usCnt, usLen, usRet = 0;      // auKeys[] = {1, 0};
   PSZ    *ppszJobCrewEquipmentKey = NULL;
   PSZ    pszTestSubQuery = NULL;
   DOUBLE dTestSubQuery = 0.0;
   CHAR   szResFmt[64];
//   CHAR   szTempRecal[712], szTempRecal2[712];
//   CHAR   aszJobCrewEquipmentKeys[MAXLABORERSPERJCS][KEYLENGTH];


   if ((pszJobKey == NULL) || (pszJobCostSheetKey == NULL)
       || (pszJobCrewKey == NULL) )
      {
      ItiErrWriteDebugMessage
         ("CBEst calcfn ReCalcJobCstShtCrewEquipment received a NULL key value.");
      return RETURN_ERROR;
      }

   hhpLocal = ItiMemCreateHeap (MAX_HEAP_SIZE);
   if (hhpLocal == NULL)
      {
      ItiErrWriteDebugMessage
         ("CBEst calcfn ReCalcJobCstShtCrewEquipment failed to create hhpLocal.");
      return RETURN_ERROR;
      }

// // -- Start zero out. 
//    sprintf (szTempRecal,
//             " /* CBEst calcfn ReCalcJobCstShtCrewEquipment */ "
//             "UPDATE JobCostSheet "
//             "SET EquipmentDailyCost = 0.0 "
//             "WHERE JobKey = %s "
//             "AND JobCostSheetKey = %s ",
//             pszJobKey, pszJobCostSheetKey);
// 
//    if (ItiDbExecSQLStatement(hhpLocal, szTempRecal) != 0)
//       {
//       sprintf (szTempRecal2,
//          "CBEst calcfn ReCalcJobCstShtCrewEquipment failed for query: %s", szTempRecal);
//       ItiErrWriteDebugMessage (szTempRecal2);
//       }
// -- End zero out.

   /* -- Build array of EquipmentKeys for current JCSheet and JCUsed. */
   ItiStrCpy (szTempRecal,
            " SELECT EquipmentKey FROM JobCrewEquipmentUsed"
            " WHERE JobKey = ", sizeof szTempRecal);
      ItiStrCat (szTempRecal, pszJobKey, sizeof szTempRecal);
      ItiStrCat (szTempRecal, " AND JobCostSheetKey = ", sizeof szTempRecal);
      ItiStrCat (szTempRecal, pszJobCostSheetKey, sizeof szTempRecal);
      ItiStrCat (szTempRecal, " AND JobCrewKey = ", sizeof szTempRecal);
      ItiStrCat (szTempRecal, pszJobCrewKey, sizeof szTempRecal);

//   ItiDbBuildSelectedKeyArray2 (hhpLocal, 0, szTempRecal, &ppszJobCrewEquipmentKey, auKeys);
//     The above function call generates a pointer problem.

   if (!(hqry = ItiDbExecQuery(szTempRecal, hhpLocal, 0, 0, 0, &uCols, &uErr)))
      {
      return 13;
      }

   aszJobCrewEquipmentKeys[0][0] = '\0';
   usCnt = 0;
   while (ItiDbGetQueryRow(hqry, &ppszJobCrewEquipmentKey, &uErr)) 
      {
      ItiStrCpy (aszJobCrewEquipmentKeys[usCnt], ppszJobCrewEquipmentKey[0], KEYLENGTH);
      usCnt++;
      aszJobCrewEquipmentKeys[usCnt][0] = '\0';
      }

   /* -- For each Equipment key... */
   for (usCnt=0; aszJobCrewEquipmentKeys[usCnt][0] ; usCnt++)
      {
      /* -- Update the current Equipment key's rate entries. -- */
      /* -- Test the subquery. -- */
      ItiStrCpy (szTempRecal,
                " /* CBEst calcfn ReCalcJobCstShtCrewEquipment */ "
                "SELECT EQR.Rate "
                "FROM  Equipment EQ, EquipmentRate EQR, "
                "JobZone JZ, Job J "
                "WHERE EQ.EquipmentKey = EQR.EquipmentKey "
                "AND EQ.ZoneTypeKey = JZ.ZoneTypeKey "
                "AND JZ.ZoneKey = EQR.ZoneKey "
                "AND J.BaseDate = EQR.BaseDate "
                "AND JZ.JobKey = J.JobKey "
                "AND J.JobKey = ", sizeof szTempRecal);
      ItiStrCat (szTempRecal, pszJobKey, sizeof szTempRecal);
      ItiStrCat (szTempRecal," AND EQ.EquipmentKey = ", sizeof szTempRecal);
      ItiStrCat (szTempRecal, aszJobCrewEquipmentKeys[usCnt], sizeof szTempRecal);


      pszTestSubQuery = ItiDbGetRow1Col1 (szTempRecal, hhpLocal, 0, 0, 0);
      if (pszTestSubQuery != NULL)
         {
         dTestSubQuery = ItiStrToDouble (pszTestSubQuery);
         ItiFmtFormatString (pszTestSubQuery, szResFmt,
                             sizeof szResFmt, "Number,....", &usLen);
         }
      else
         dTestSubQuery = 0.0;

      if (dTestSubQuery == 0.0)
         {
         /* -- Since the zone did not have a value, test if the */
         /* -- default zone key will give us a value to use.    */
         ItiStrCpy (szTempRecal,
            "/* CBEst calcfn ReCalcJobCstShtCrewEquipment, query default zone rate. */ "
            "SELECT EQR.Rate "
            "FROM  Equipment EQ, EquipmentRate EQR, "
                  "JobZone JZ, Job J "
            "WHERE EQ.EquipmentKey = EQR.EquipmentKey "
            "AND EQ.ZoneTypeKey = JZ.ZoneTypeKey "
            "AND EQR.ZoneKey = 1 "
            "AND J.BaseDate = EQR.BaseDate "
            "AND JZ.JobKey = J.JobKey "
            "AND J.JobKey = ", sizeof szTempRecal);
        ItiStrCat (szTempRecal, pszJobKey, sizeof szTempRecal);
        ItiStrCat (szTempRecal," AND EQ.EquipmentKey = ", sizeof szTempRecal);
        ItiStrCat (szTempRecal, aszJobCrewEquipmentKeys[usCnt], sizeof szTempRecal);
         
         if (pszTestSubQuery)      /* Recover old test query's memory. */
            ItiMemFree (hhpLocal, pszTestSubQuery);

         pszTestSubQuery = ItiDbGetRow1Col1 (szTempRecal, hhpLocal, 0, 0, 0);
         if (pszTestSubQuery != NULL)
            {
            ItiFmtFormatString (pszTestSubQuery, szResFmt,
                                sizeof szResFmt, "Number,....", &usLen);
            ItiStrCpy (szTempRecal,
               "/* CBEst calcfn ReCalcJobCstShtCrewEquipment, using default zone rate. */ "
               "UPDATE JobCrewEquipmentUsed SET Rate = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, szResFmt, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " WHERE JobKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal,      pszJobKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND JobCostSheetKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal,    pszJobCostSheetKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND JobCrewKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal,    pszJobCrewKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND EquipmentKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, aszJobCrewEquipmentKeys[usCnt], sizeof szTempRecal);
            }
         else
            {
            /* -- If the default zone key did not have a value, set rate to $0. */
            ItiStrCpy (szTempRecal,
                 "/* CBEst calcfn ReCalcJobCstShtCrewEquipment, NO default rate available */ "
                 "UPDATE JobCrewEquipmentUsed SET Rate = 0.0 "
                 "WHERE JobKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal,      pszJobKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND JobCostSheetKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal,    pszJobCostSheetKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND JobCrewKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal,    pszJobCrewKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND EquipmentKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, aszJobCrewEquipmentKeys[usCnt], sizeof szTempRecal);
            }
         }
      else
         {
         ItiStrCpy (szTempRecal,
              "/* CBEst calcfn ReCalcJobCstShtCrewEquipment, use job zone rate */ "
              "UPDATE JobCrewEquipmentUsed SET Rate = ",sizeof szTempRecal);
         ItiStrCat (szTempRecal, szResFmt, sizeof szTempRecal);
         ItiStrCat (szTempRecal, " WHERE JobKey = ", sizeof szTempRecal);
         ItiStrCat (szTempRecal,      pszJobKey, sizeof szTempRecal);
         ItiStrCat (szTempRecal, " AND JobCostSheetKey = ", sizeof szTempRecal);
         ItiStrCat (szTempRecal,    pszJobCostSheetKey, sizeof szTempRecal);
         ItiStrCat (szTempRecal, " AND JobCrewKey = ", sizeof szTempRecal);
         ItiStrCat (szTempRecal,    pszJobCrewKey, sizeof szTempRecal);
         ItiStrCat (szTempRecal, " AND EquipmentKey = ", sizeof szTempRecal);
         ItiStrCat (szTempRecal, aszJobCrewEquipmentKeys[usCnt], sizeof szTempRecal);
         }

      if (pszTestSubQuery)
         ItiMemFree (hhpLocal, pszTestSubQuery);

      if (ItiDbExecSQLStatement(hhpLocal, szTempRecal) != 0)
         {
         ItiErrWriteDebugMessage ("\n *** JobCostSheet Update failed for query: ");
         ItiErrWriteDebugMessage (szTempRecal);
         }


      /* -- Test the subquery for overtime rate. -- */
      ItiStrCpy (szTempRecal,
         "/* CBEst calcfn ReCalcJobCstShtCrewEquipment, query job zone OTRate*/ "
         "SELECT EQR.OvertimeRate "
         "FROM  Equipment EQ, EquipmentRate EQR, "
         "JobZone JZ, Job J "
         "WHERE EQ.EquipmentKey = EQR.EquipmentKey "
         "AND EQ.ZoneTypeKey = JZ.ZoneTypeKey "
         "AND JZ.ZoneKey = EQR.ZoneKey "
         "AND J.BaseDate = EQR.BaseDate "
         "AND JZ.JobKey = J.JobKey "
         "AND J.JobKey = ", sizeof szTempRecal);
      ItiStrCat (szTempRecal, pszJobKey, sizeof szTempRecal);
      ItiStrCat (szTempRecal, " AND EQ.EquipmentKey = ", sizeof szTempRecal);
      ItiStrCat (szTempRecal, aszJobCrewEquipmentKeys[usCnt], sizeof szTempRecal);

      pszTestSubQuery = ItiDbGetRow1Col1 (szTempRecal, hhpLocal, 0, 0, 0);
      if (pszTestSubQuery != NULL)
         {
         dTestSubQuery = ItiStrToDouble (pszTestSubQuery);
         ItiFmtFormatString (pszTestSubQuery, szResFmt,
                             sizeof szResFmt, "Number,....", &usLen);
         }
      else
         dTestSubQuery = 0.0;

      if (dTestSubQuery == 0.0)
         {
         /* -- Now if test if the default zone key has a value. */
         ItiStrCpy (szTempRecal,
            "/* CBEst calcfn ReCalcJobCstShtCrewEquipment, query default zone OTRate*/ "
            "SELECT EQR.OvertimeRate "
            "FROM  Equipment EQ, EquipmentRate EQR, "
            "JobZone JZ, Job J "
            "WHERE EQ.EquipmentKey = EQR.EquipmentKey "
            "AND EQ.ZoneTypeKey = JZ.ZoneTypeKey "
            "AND EQR.ZoneKey = 1 "
            "AND J.BaseDate = EQR.BaseDate "
            "AND JZ.JobKey = J.JobKey "
            "AND J.JobKey = ", sizeof szTempRecal);
         ItiStrCat (szTempRecal, pszJobKey, sizeof szTempRecal);
         ItiStrCat (szTempRecal, " AND EQ.EquipmentKey = ", sizeof szTempRecal);
         ItiStrCat (szTempRecal, aszJobCrewEquipmentKeys[usCnt], sizeof szTempRecal);
         
         if (pszTestSubQuery)      /* Recover old test query's memory. */
            ItiMemFree (hhpLocal, pszTestSubQuery);

         pszTestSubQuery = ItiDbGetRow1Col1 (szTempRecal, hhpLocal, 0, 0, 0);
         if (pszTestSubQuery != NULL)
            {
            ItiFmtFormatString (pszTestSubQuery, szResFmt,
                                sizeof szResFmt, "Number,....", &usLen);
            ItiStrCpy (szTempRecal,
               "/* CBEst calcfn ReCalcJobCstShtCrewEquipment, use default zone OTRate*/ "
               "UPDATE JobCrewEquipmentUsed SET OvertimeRate = ",sizeof szTempRecal);
            ItiStrCat (szTempRecal, szResFmt, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " WHERE JobKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal,      pszJobKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND JobCostSheetKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal,    pszJobCostSheetKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND JobCrewKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal,    pszJobCrewKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND EquipmentKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, aszJobCrewEquipmentKeys[usCnt], sizeof szTempRecal);
            }
         else
            {
            /* -- If the default zone key did not have a value, */
            /* -- set the overtime rate equal to the rate.      */
            ItiStrCpy (szTempRecal,
                 " /* CBEst calcfn ReCalcJobCstShtCrewEquipment */ "
                 "UPDATE JobCrewEquipmentUsed SET OvertimeRate = Rate ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, " WHERE JobKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal,      pszJobKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND JobCostSheetKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal,    pszJobCostSheetKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND JobCrewKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal,    pszJobCrewKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND EquipmentKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, aszJobCrewEquipmentKeys[usCnt], sizeof szTempRecal);
            }
         }
      else
         {
         ItiStrCpy (szTempRecal,
              " /* CBEst calcfn ReCalcJobCstShtCrewEquipment */ "
              "UPDATE JobCrewEquipmentUsed SET OvertimeRate = ", sizeof szTempRecal);
         ItiStrCat (szTempRecal, szResFmt, sizeof szTempRecal);
         ItiStrCat (szTempRecal, " WHERE JobKey = ", sizeof szTempRecal);
         ItiStrCat (szTempRecal,      pszJobKey, sizeof szTempRecal);
         ItiStrCat (szTempRecal, " AND JobCostSheetKey = ", sizeof szTempRecal);
         ItiStrCat (szTempRecal,    pszJobCostSheetKey, sizeof szTempRecal);
         ItiStrCat (szTempRecal, " AND JobCrewKey = ", sizeof szTempRecal);
         ItiStrCat (szTempRecal,    pszJobCrewKey, sizeof szTempRecal);
         ItiStrCat (szTempRecal, " AND EquipmentKey = ", sizeof szTempRecal);
         ItiStrCat (szTempRecal, aszJobCrewEquipmentKeys[usCnt], sizeof szTempRecal);
         }/* End of if ((pszTestSubQuery... else */

      if (pszTestSubQuery)
         ItiMemFree (hhpLocal, pszTestSubQuery);
 
      if (ItiDbExecSQLStatement(hhpLocal, szTempRecal) != 0)
         {
         ItiErrWriteDebugMessage
            ( "\n *** JobCostSheet Update failed for query: ");
         ItiErrWriteDebugMessage (szTempRecal);
         }

      } /* end of for (k=0; ppszJobCrewEquipmentKey... loop */
   ItiFreeStrArray2 (hhpLocal, ppszJobCrewEquipmentKey);
   

//   if (pszTestSubQuery)
//      ItiMemFree (hhpLocal, pszTestSubQuery);

   if (NULL != hhpLocal)
      ItiMemDestroyHeap (hhpLocal);

   return usRet;
   } /* End of function ReCalcJobCstShtCrewEquipment */



USHORT EXPORT ReCalcJobCstShtCrewLabor
          (PSZ pszJobKey, PSZ pszJobCostSheetKey, PSZ pszJobCrewKey)
   {
   HQRY   hqry;
   HHEAP  hhpRCJCSCL;
   USHORT uCols, uErr, usCnt, usLen, usRet = 0, auKeys[] = {1, 0};
   PSZ    *ppszJobCrewLaborKey = NULL;
   PSZ    pszDavis = NULL;
   PSZ    pszTestSubQuery = NULL;
   DOUBLE dTestSubQuery = 0.0;
   CHAR   szResFmt[30] = "";
//   CHAR   szTempRecal[712], szTempRecal2[712];
//   CHAR   aszJobCrewLaborKeys [MAXLABORERSPERJCS][KEYLENGTH];

   if ((pszJobKey == NULL)
       || (pszJobCostSheetKey == NULL) || (pszJobCrewKey == NULL))
      {
      ItiErrWriteDebugMessage
         ("CBEst calcfn ReCalcJobCstShtCrewLabor received a NULL key value.");
      return RETURN_ERROR;
      }                            

   hhpRCJCSCL = ItiMemCreateHeap (MAX_HEAP_SIZE);
   if (hhpRCJCSCL == NULL)
      {
      ItiErrWriteDebugMessage
         ("CBEst calcfn ReCalcJobCstShtCrewLabor failed to create hhpRCJCSCL.");
      return RETURN_ERROR;
      }

   /* -- Test Job's DavisBacon flag. */
   ItiStrCpy (szTempRecal,
      "SELECT DavisBaconWages FROM Job WHERE JobKey = ", sizeof szTempRecal);
   ItiStrCat (szTempRecal, pszJobKey, sizeof szTempRecal);
   pszDavis = ItiDbGetRow1Col1 (szTempRecal, hhpRCJCSCL, 0, 0, 0);


   /* -- Build array of LaborerKeys for current JobCostSheet and JobCrewUsed. */
   ItiStrCpy (szTempRecal,
            " /* CBEst calcfn ReCalcJobCstShtCrewLabor, laborer key list */ "
            " SELECT LaborerKey FROM JobCrewLaborUsed"
            " WHERE JobKey = ", sizeof szTempRecal);
   ItiStrCat (szTempRecal, pszJobKey, sizeof szTempRecal);
   ItiStrCat (szTempRecal, " AND JobCostSheetKey = ", sizeof szTempRecal);
   ItiStrCat (szTempRecal, pszJobCostSheetKey, sizeof szTempRecal);
   ItiStrCat (szTempRecal, " AND JobCrewKey = ", sizeof szTempRecal);
   ItiStrCat (szTempRecal, pszJobCrewKey, sizeof szTempRecal);
             
   if (!(hqry = ItiDbExecQuery(szTempRecal, hhpRCJCSCL, 0, 0, 0, &uCols, &uErr)))
      {
      return 13;
      }

   aszJobCrewLaborKeys[0][0] = '\0';
   usCnt = 0;
   while (ItiDbGetQueryRow(hqry, &ppszJobCrewLaborKey, &uErr)) 
      {
      ItiStrCpy (aszJobCrewLaborKeys[usCnt], ppszJobCrewLaborKey[0], KEYLENGTH);
      usCnt++;
      aszJobCrewLaborKeys[usCnt][0] = '\0';
      }

   /* -- For each laborer key... */
   for (usCnt=0; aszJobCrewLaborKeys[usCnt][0] && (usRet == 0); usCnt++)
      {
      /* -- Update the current laborer key's rate entries. -- */
      if (pszDavis[0] == '1')
         {
         /* -- DavisBaconRate ------------------------------------- */
         /* -- Test the subquery. -- */
         ItiStrCpy (szTempRecal,
                   " /* CBEst calcfn ReCalcJobCstShtCrewLabor DBR */ "
                   "SELECT LWB.DavisBaconRate "
                   "FROM  Labor L, LaborerWageAndBenefits LWB, "
                   "JobZone JZ, Job J "
                   "WHERE L.LaborerKey = LWB.LaborerKey "
                   "AND L.ZoneTypeKey = JZ.ZoneTypeKey "
                   "AND JZ.ZoneKey = LWB.ZoneKey "
                   "AND J.BaseDate = LWB.BaseDate "
                   "AND JZ.JobKey = J.JobKey "
                   "AND J.JobKey = ", sizeof szTempRecal);
         ItiStrCat (szTempRecal, pszJobKey, sizeof szTempRecal);
         ItiStrCat (szTempRecal, " AND L.LaborerKey = ", sizeof szTempRecal);
         ItiStrCat (szTempRecal, aszJobCrewLaborKeys[usCnt], sizeof szTempRecal);

         pszTestSubQuery = ItiDbGetRow1Col1 (szTempRecal, hhpRCJCSCL, 0, 0, 0);
         if (pszTestSubQuery != NULL)
            {
            dTestSubQuery = ItiStrToDouble (pszTestSubQuery);
            ItiFmtFormatString (pszTestSubQuery, szResFmt,
                                sizeof szResFmt, "Number,....", &usLen);
            }
         else
            dTestSubQuery = 0.0;

         if (dTestSubQuery == 0.0)
            {
            ItiStrCpy (szTempRecal,
                 " /* CBEst calcfn ReCalcJobCstShtCrewLabor DBR was 0 */ "
                 "UPDATE JobCrewLaborUsed SET Rate = 0.0 "
                 "WHERE JobKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, pszJobKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND JobCostSheetKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, pszJobCostSheetKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND JobCrewKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, pszJobCrewKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND LaborerKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, aszJobCrewLaborKeys[usCnt], sizeof szTempRecal);
            }
         else
            {
            ItiStrCpy (szTempRecal,
                 " /* CBEst calcfn ReCalcJobCstShtCrewLabor DBR not 0 */ "
                 "UPDATE JobCrewLaborUsed SET Rate = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, szResFmt, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " WHERE JobKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, pszJobKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND JobCostSheetKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, pszJobCostSheetKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND JobCrewKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, pszJobCrewKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND LaborerKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, aszJobCrewLaborKeys[usCnt], sizeof szTempRecal);
            }

         if (pszTestSubQuery)
            ItiMemFree (hhpRCJCSCL, pszTestSubQuery);

         if (ItiDbExecSQLStatement(hhpRCJCSCL, szTempRecal) != 0)
            {
            ItiErrWriteDebugMessage 
               ("\n *** CBEst calcfn ReCalcJobCstShtCrewLabor Update failed for query: ");
            ItiErrWriteDebugMessage (szTempRecal);
            }


         /* -- DavisBaconOvertimeRate */
         /* -- Test the subquery. -- */
         ItiStrCpy (szTempRecal,
                   " /* CBEst calcfn ReCalcJobCstShtCrewLabor DBOTR */ "
                   "SELECT LWB.DavisBaconOvertimeRate "
                   "FROM  Labor L, LaborerWageAndBenefits LWB, "
                   "JobZone JZ, Job J "
                   "WHERE L.LaborerKey = LWB.LaborerKey "
                   "AND L.ZoneTypeKey = JZ.ZoneTypeKey "
                   "AND JZ.ZoneKey = LWB.ZoneKey "
                   "AND J.BaseDate = LWB.BaseDate "
                   "AND JZ.JobKey = J.JobKey "
                   "AND J.JobKey = ", sizeof szTempRecal);
         ItiStrCat (szTempRecal, pszJobKey, sizeof szTempRecal);
         ItiStrCat (szTempRecal, " AND L.LaborerKey = ", sizeof szTempRecal);
         ItiStrCat (szTempRecal, aszJobCrewLaborKeys[usCnt], sizeof szTempRecal);

         pszTestSubQuery = ItiDbGetRow1Col1 (szTempRecal, hhpRCJCSCL, 0, 0, 0);
         if (pszTestSubQuery != NULL)
            {
            dTestSubQuery = ItiStrToDouble (pszTestSubQuery);
            ItiFmtFormatString (pszTestSubQuery, szResFmt,
                                sizeof szResFmt, "Number,....", &usLen);
            }
         else
            dTestSubQuery = 0.0;

         if (dTestSubQuery == 0.0)
            {
            ItiStrCpy (szTempRecal,
                 " /* CBEst calcfn ReCalcJobCstShtCrewLabor DBOTR was 0 */ "
                 "UPDATE JobCrewLaborUsed SET OvertimeRate = 0.0 "
                 "WHERE JobKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, pszJobKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND JobCostSheetKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, pszJobCostSheetKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND JobCrewKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, pszJobCrewKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND LaborerKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, aszJobCrewLaborKeys[usCnt], sizeof szTempRecal);
            }
         else
            {
            ItiStrCpy (szTempRecal,
                 " /* CBEst calcfn ReCalcJobCstShtCrewLabor DBOTR not 0 */ "
                 "UPDATE JobCrewLaborUsed SET OvertimeRate = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, szResFmt, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " WHERE JobKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal,      pszJobKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND JobCostSheetKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal,    pszJobCostSheetKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND JobCrewKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal,    pszJobCrewKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND LaborerKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, aszJobCrewLaborKeys[usCnt], sizeof szTempRecal);
            }

         if (pszTestSubQuery)
            ItiMemFree (hhpRCJCSCL, pszTestSubQuery);

         if (ItiDbExecSQLStatement(hhpRCJCSCL, szTempRecal) != 0)
            {
            sprintf (szTempRecal2,
               "CBEst calcfn ReCalcJobCstShtCrewLabor Update failed for query: %s", szTempRecal);
            ItiErrWriteDebugMessage (szTempRecal2);
            }

         } /* End of if DavisBaconWage flag test then clause. */
      else
         {
         /* -- NON-DavisBaconRate --------------------------------- */
         /* -- Test the subquery. -- */
         ItiStrCpy (szTempRecal,
                      " /* CBEst calcfn ReCalcJobCstShtCrewLabor NonDB */ "
                      "SELECT LWB.NonDavisBaconRate "
                      "FROM  Labor L, LaborerWageAndBenefits LWB, "
                      "JobZone JZ, Job J "
                      "WHERE L.LaborerKey = LWB.LaborerKey "
                      "AND L.ZoneTypeKey = JZ.ZoneTypeKey "
                      "AND JZ.ZoneKey = LWB.ZoneKey "
                      "AND J.BaseDate = LWB.BaseDate "
                      "AND JZ.JobKey = J.JobKey "
                      "AND J.JobKey = ", sizeof szTempRecal);
         ItiStrCat (szTempRecal, pszJobKey, sizeof szTempRecal);
         ItiStrCat (szTempRecal, " AND L.LaborerKey = ", sizeof szTempRecal);
         ItiStrCat (szTempRecal, aszJobCrewLaborKeys[usCnt], sizeof szTempRecal);

         pszTestSubQuery = ItiDbGetRow1Col1 (szTempRecal, hhpRCJCSCL, 0, 0, 0);
         if (pszTestSubQuery != NULL)
            {
            dTestSubQuery = ItiStrToDouble (pszTestSubQuery);
            ItiFmtFormatString (pszTestSubQuery, szResFmt,
                                sizeof szResFmt, "Number,....", &usLen);
            }
         else
            dTestSubQuery = 0.0;

         if (dTestSubQuery == 0.0)
            {
            ItiStrCpy (szTempRecal,
                    " /* CBEst calcfn ReCalcJobCstShtCrewLabor NonDBR was 0 */ "
                    "UPDATE JobCrewLaborUsed SET Rate = 0.0 "
                    "WHERE JobKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, pszJobKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND JobCostSheetKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, pszJobCostSheetKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND JobCrewKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, pszJobCrewKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND LaborerKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, aszJobCrewLaborKeys[usCnt], sizeof szTempRecal);
            }
         else
            {
            ItiStrCpy (szTempRecal,
                    " /* CBEst calcfn ReCalcJobCstShtCrewLabor NonDBR not 0 */ "
                    "UPDATE JobCrewLaborUsed SET Rate = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, szResFmt, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " WHERE JobKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal,      pszJobKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND JobCostSheetKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal,    pszJobCostSheetKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND JobCrewKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal,    pszJobCrewKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND LaborerKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, aszJobCrewLaborKeys[usCnt], sizeof szTempRecal);
            }

         if (pszTestSubQuery)
            ItiMemFree (hhpRCJCSCL, pszTestSubQuery);


         if (ItiDbExecSQLStatement(hhpRCJCSCL, szTempRecal) != 0)
            {
            sprintf (szTempRecal2,
               "CBEst calcfn ReCalcJobCstShtCrewLabor Update failed for query: %s", szTempRecal);
            ItiErrWriteDebugMessage (szTempRecal2);
            }


         /* -- NON-DavisBaconOvertimeRate */
         /* -- Test the subquery. -- */
         ItiStrCpy (szTempRecal,
                   " /* CBEst calcfn ReCalcJobCstShtCrewLabor NonDBOT */ "
                   "SELECT LWB.NonDavisBaconOvertimeRate "
                   "FROM  Labor L, LaborerWageAndBenefits LWB, "
                   "JobZone JZ, Job J "
                   "WHERE L.LaborerKey = LWB.LaborerKey "
                   "AND L.ZoneTypeKey = JZ.ZoneTypeKey "
                   "AND JZ.ZoneKey = LWB.ZoneKey "
                   "AND J.BaseDate = LWB.BaseDate "
                   "AND JZ.JobKey = J.JobKey "
                   "AND J.JobKey = ", sizeof szTempRecal);
         ItiStrCat (szTempRecal, pszJobKey, sizeof szTempRecal);
         ItiStrCat (szTempRecal, " AND L.LaborerKey = ", sizeof szTempRecal);
         ItiStrCat (szTempRecal, aszJobCrewLaborKeys[usCnt], sizeof szTempRecal);

         pszTestSubQuery = ItiDbGetRow1Col1 (szTempRecal, hhpRCJCSCL, 0, 0, 0);
         if (pszTestSubQuery != NULL)
            {
            dTestSubQuery = ItiStrToDouble (pszTestSubQuery);
            ItiFmtFormatString (pszTestSubQuery, szResFmt,
                                sizeof szResFmt, "Number,....", &usLen);
            }
         else
            dTestSubQuery = 0.0;

         if (dTestSubQuery == 0.0)
            {
            ItiStrCpy (szTempRecal,
                 " /* CBEst calcfn ReCalcJobCstShtCrewLabor NonDBOT 0 */ "
                 "UPDATE JobCrewLaborUsed SET OvertimeRate = 0.0 "
                 "WHERE JobKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, pszJobKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND JobCostSheetKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, pszJobCostSheetKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND JobCrewKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, pszJobCrewKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND LaborerKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, aszJobCrewLaborKeys[usCnt], sizeof szTempRecal);
            }
         else
            {
            ItiStrCpy (szTempRecal,
                 " /* CBEst calcfn ReCalcJobCstShtCrewLabor NonDBOT not 0 */ "
                 "UPDATE JobCrewLaborUsed SET OvertimeRate = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, szResFmt, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " WHERE JobKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, pszJobKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND JobCostSheetKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, pszJobCostSheetKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND JobCrewKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, pszJobCrewKey, sizeof szTempRecal);
            ItiStrCat (szTempRecal, " AND LaborerKey = ", sizeof szTempRecal);
            ItiStrCat (szTempRecal, aszJobCrewLaborKeys[usCnt], sizeof szTempRecal);
            }

         if (ItiDbExecSQLStatement(hhpRCJCSCL, szTempRecal) != 0)
            {
            ItiErrWriteDebugMessage
               ("\n *** CBEst calcfn ReCalcJobCstShtCrewLabor Update failed for query: ");
            ItiErrWriteDebugMessage (szTempRecal);
            }

         if (NULL != pszTestSubQuery)
            ItiMemFree (hhpRCJCSCL, pszTestSubQuery);

         } /* End of else DavisBaconWage flag test. */

      } /* End of for (k=0; ppszJobCrewLaborKey... loop */


   if (NULL != pszDavis)
      ItiMemFree (hhpRCJCSCL, pszDavis);

   if (ppszJobCrewLaborKey)
      ItiFreeStrArray2 (hhpRCJCSCL, ppszJobCrewLaborKey);

   if (NULL != hhpRCJCSCL)
      ItiMemDestroyHeap (hhpRCJCSCL);

   return usRet;
   } /* End of function ReCalcJobCstShtCrewLabor */




USHORT EXPORT RecalcTotalForJCSCrews (PSZ pszJobKey, PSZ pszJobCostSheetKey)
   {
//   HQRY   hqry;
//   USHORT uCols, uErr, usCnt;
   HHEAP  hhpLocalRecalc;
   USHORT usLen, usRet = 0;
   CHAR   szTemp[512] = "";
   CHAR  szSumLaborRateXQtyFmt[30] = "";
   CHAR  szSumLaborOTRateXQtyFmt[30] = "";
   CHAR  szSumEquipmentRateXQtyFmt[30] = "";
   CHAR  szSumEquipmentOTRateXQtyFmt[30] = "";
   PSZ  pszSumLaborRateXQty = NULL;
   PSZ  pszSumLaborOTRateXQty = NULL;
   PSZ  pszSumEquipmentRateXQty = NULL;
   PSZ  pszSumEquipmentOTRateXQty = NULL;
   DOUBLE dSumLaborRateXQty = 0.0;
   DOUBLE dSumLaborOTRateXQty = 0.0;
   DOUBLE dSumEquipmentRateXQty = 0.0;
   DOUBLE dSumEquipmentOTRateXQty = 0.0;
   PSZ    *ppszJobCrewKey = NULL;

   if ((pszJobKey == NULL) || (pszJobCostSheetKey == NULL))
      {
      ItiErrWriteDebugMessage
         ("CBEst calcfn RecalcTotalForJCSCrews received a NULL key value.");
      return RETURN_ERROR;
      }

   hhpLocalRecalc = ItiMemCreateHeap (MAX_HEAP_SIZE);
   if (hhpLocalRecalc == NULL)
      {
      ItiErrWriteDebugMessage
         ("CBEst calcfn RecalcTotalForJCSCrews failed to create hhpLocalRecalc.");
      return RETURN_ERROR;
      }


   /* ===================================================== */
   /* -- Now update the JobCostSheet's LaborDailyCost total *
    * -- for the current cost sheet key.                    */

   ItiStrCpy (szTemp,
            " /* CBEst calcfn RecalcTotalForJCSCrews */ "
            " SELECT  SUM(Rate * Quantity) "
            " FROM JobCrewLaborUsed  "
            " WHERE JobKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
   ItiStrCat (szTemp, " AND JobCostSheetKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobCostSheetKey, sizeof szTemp);

   pszSumLaborRateXQty = ItiDbGetRow1Col1 (szTemp, hhpLocalRecalc, 0, 0, 0);
   if (pszSumLaborRateXQty != NULL)
      {
      dSumLaborRateXQty = ItiStrToDouble (pszSumLaborRateXQty);
      ItiFmtFormatString (pszSumLaborRateXQty, szSumLaborRateXQtyFmt,
                          sizeof szSumLaborRateXQtyFmt, "Number,....", &usLen);
      }
   else
      dSumLaborRateXQty = 0.0;


   ItiStrCpy (szTemp,
            " /* CBEst calcfn RecalcTotalForJCSCrews */ "
            " SELECT  SUM(OvertimeRate * Quantity) "
            " FROM JobCrewLaborUsed  "
            " WHERE JobKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
   ItiStrCat (szTemp, " AND JobCostSheetKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobCostSheetKey, sizeof szTemp);

   pszSumLaborOTRateXQty = ItiDbGetRow1Col1 (szTemp, hhpLocalRecalc, 0, 0, 0);
   if (pszSumLaborOTRateXQty != NULL)
      {
      dSumLaborOTRateXQty = ItiStrToDouble (pszSumLaborOTRateXQty);
      ItiFmtFormatString (pszSumLaborOTRateXQty, szSumLaborOTRateXQtyFmt,
                          sizeof szSumLaborOTRateXQtyFmt, "Number,....", &usLen);
      }
   else
      dSumLaborOTRateXQty = 0.0;


   if (dSumLaborRateXQty == 0.0)
      {
      szTemp[0] = '\0';
      }
   else
      {
      ItiStrCpy (szTemp,
               " /* CBEst calcfn RecalcTotalForJCSCrews */ "
               "UPDATE JobCostSheet SET LaborDailyCost = ((", 
               sizeof szTemp);
      ItiStrCat (szTemp, szSumLaborRateXQtyFmt, sizeof szTemp);
      ItiStrCat (szTemp, " * JobCostSheet.HoursPerDay) + (", sizeof szTemp);
      ItiStrCat (szTemp, szSumLaborOTRateXQtyFmt, sizeof szTemp);
      ItiStrCat (szTemp,
               " * JobCostSheet.OvertimeHoursPerDay)) WHERE JobKey = ", 
               sizeof szTemp);
      ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
      ItiStrCat (szTemp, " AND JobCostSheetKey = ", sizeof szTemp);
      ItiStrCat (szTemp, pszJobCostSheetKey, sizeof szTemp);
      }


   if (szTemp[0] != '\0')
      {
      if (ItiDbExecSQLStatement(hhpLocalRecalc, szTemp) != 0)
         {
         ItiErrWriteDebugMessage (" Update failed for query: ");
         ItiErrWriteDebugMessage (szTemp);
         }
      }
 


   /* ========================================================= */
   /* -- Now update the JobCostSheet's EquipmentDailyCost total */
   /* -- for the current cost sheet key.                        */
   ItiStrCpy (szTemp,
            " /* CBEst calcfn RecalcTotalForJCSCrews */ "
            " SELECT  SUM(Rate * Quantity) "
            " FROM JobCrewEquipmentUsed  "
            " WHERE JobKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
   ItiStrCat (szTemp, " AND JobCostSheetKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobCostSheetKey, sizeof szTemp);

   pszSumEquipmentRateXQty = ItiDbGetRow1Col1 (szTemp, hhpLocalRecalc, 0, 0, 0);
   if (pszSumEquipmentRateXQty != NULL)
      {
      dSumEquipmentRateXQty = ItiStrToDouble (pszSumEquipmentRateXQty);
      ItiFmtFormatString (pszSumEquipmentRateXQty, szSumEquipmentRateXQtyFmt,
                          sizeof szSumEquipmentRateXQtyFmt, "Number,....", &usLen);
      }
   else
      dSumEquipmentRateXQty = 0.0;


   ItiStrCpy (szTemp,
             " /* CBEst calcfn RecalcTotalForJCSCrews */ "
             " SELECT  SUM(OvertimeRate * Quantity) "
             " FROM JobCrewEquipmentUsed  "
             " WHERE JobKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
   ItiStrCat (szTemp, " AND JobCostSheetKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobCostSheetKey, sizeof szTemp);

   pszSumEquipmentOTRateXQty = ItiDbGetRow1Col1 (szTemp, hhpLocalRecalc, 0, 0, 0);
   if (pszSumEquipmentOTRateXQty != NULL)
      {
      dSumEquipmentOTRateXQty = ItiStrToDouble (pszSumEquipmentOTRateXQty);
      ItiFmtFormatString (pszSumEquipmentOTRateXQty, szSumEquipmentOTRateXQtyFmt,
                          sizeof szSumEquipmentOTRateXQtyFmt, "Number,....", &usLen);
      }
   else
      dSumEquipmentOTRateXQty = 0.0;

   if (dSumEquipmentRateXQty == 0.0)
      {
      szTemp[0] = '\0';
      }
   else
      {
      ItiStrCpy (szTemp,
                 " /* CBEst calcfn RecalcTotalForJCSCrews */ "
                 "UPDATE JobCostSheet "
                 "SET EquipmentDailyCost = ((", sizeof szTemp);
      ItiStrCat (szTemp, szSumEquipmentRateXQtyFmt, sizeof szTemp);
      ItiStrCat (szTemp, " * JobCostSheet.HoursPerDay) + (", sizeof szTemp);
      ItiStrCat (szTemp, szSumEquipmentOTRateXQtyFmt, sizeof szTemp);
      ItiStrCat (szTemp,
               " * JobCostSheet.OvertimeHoursPerDay)) WHERE JobKey = ", 
               sizeof szTemp);
      ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
      ItiStrCat (szTemp, " AND JobCostSheetKey = ", sizeof szTemp);
      ItiStrCat (szTemp, pszJobCostSheetKey, sizeof szTemp);
      }

   if (szTemp[0] != '\0')
      {
      if (ItiDbExecSQLStatement(hhpLocalRecalc, szTemp) != 0)
         {
         ItiErrWriteDebugMessage ("\n *** Update failed for query: ");
         ItiErrWriteDebugMessage (szTemp);
         }
      }

   if (hhpLocalRecalc)
      ItiMemDestroyHeap (hhpLocalRecalc);

   return 0;
   } /* End of Function RecalcTotalForJCSCrews */



/* -- This function will recalc ALL crews attached to a Job Cost Sheet. */
USHORT EXPORT ReCalcJobCstShtCrew (PSZ pszJobKey, PSZ pszJobCostSheetKey)
   {
   HQRY   hqry;
   HHEAP  hhpLocalRecalc;
   USHORT uCols, uErr, usCnt, usRet = 0;
//   USHORT usLen;
   CHAR   szTemp[510] = "";
//   CHAR   aszCrewKeys [MAXCREWSPERJCS][KEYLENGTH];
   CHAR  szSumLaborRateXQtyFmt[25] = "";
   CHAR  szSumLaborOTRateXQtyFmt[25] = "";
   CHAR  szSumEquipmentRateXQtyFmt[25] = "";
   CHAR  szSumEquipmentOTRateXQtyFmt[25] = "";
   PSZ  pszSumLaborRateXQty = NULL;
   PSZ  pszSumLaborOTRateXQty = NULL;
   PSZ  pszSumEquipmentRateXQty = NULL;
   PSZ  pszSumEquipmentOTRateXQty = NULL;
   DOUBLE dSumLaborRateXQty = 0.0;
   DOUBLE dSumLaborOTRateXQty = 0.0;
   DOUBLE dSumEquipmentRateXQty = 0.0;
   DOUBLE dSumEquipmentOTRateXQty = 0.0;
   PSZ    *ppszJobCrewKey = NULL;

   if ((pszJobKey == NULL) || (pszJobCostSheetKey == NULL))
      {
      ItiErrWriteDebugMessage
         ("CBEst calcfn ReCalcJobCstShtCrew received a NULL key value.");
      return RETURN_ERROR;
      }

   hhpLocalRecalc = ItiMemCreateHeap (MAX_HEAP_SIZE);
   if (hhpLocalRecalc == NULL)
      {
      ItiErrWriteDebugMessage
         ("CBEst calcfn ReCalcJobCstShtCrew failed to create hhpLocalRecalc.");
      return RETURN_ERROR;
      }

                         
   /* ------------------------------------------------------------- */
   /* -- Now do the job crews used.                                 */
   /* ------------------------------------------------------------- */
   ItiStrCpy (szTemp,
            " /* CBEst calcfn ReCalcJobCstShtCrew */ "
            " SELECT JobCrewKey FROM JobCrewUsed"
            " WHERE JobKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
   ItiStrCat (szTemp, " AND JobCostSheetKey = ",sizeof szTemp);
   ItiStrCat (szTemp, pszJobCostSheetKey, sizeof szTemp);

//   ItiDbBuildSelectedKeyArray2 (hhpLocalRecalc, 0, szTemp, &ppszJobCrewKey, auKeys);

   if (!(hqry = ItiDbExecQuery(szTemp, hhpLocalRecalc, 0, 0, 0, &uCols, &uErr)))
      {
      return 13;
      }

   /* -- Now fill the array with the job crews used.   */
   aszCrewKeys[0][0] = '\0';
   usCnt = 0;
   while (ItiDbGetQueryRow(hqry, &ppszJobCrewKey, &uErr)) 
      {
      ItiStrCpy (aszCrewKeys[usCnt], ppszJobCrewKey[0], KEYLENGTH);
      usCnt++;
      aszCrewKeys[usCnt][0] = '\0';
      }


/* -- Start of zero out. */ 
   ItiStrCpy (szTemp,
            " /* CBEst calcfn ReCalcJobCstShtCrew */ "
            "UPDATE JobCostSheet "
            "SET EquipmentDailyCost = 0.0 "
            ", LaborDailyCost = 0.0 "
            "WHERE JobKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
   ItiStrCat (szTemp, " AND JobCostSheetKey = ",sizeof szTemp);
   ItiStrCat (szTemp, pszJobCostSheetKey, sizeof szTemp);

   if (ItiDbExecSQLStatement(hhpLocalRecalc, szTemp) != 0)
      {
      ItiErrWriteDebugMessage ("\n *** Query failed: ");
      ItiErrWriteDebugMessage (szTemp);
      }
/* -- End of zero out. */


   /* -- For each job crew key in the current job cost sheet... */
   for (usCnt=0; aszCrewKeys[usCnt][0] && (usRet == 0); usCnt++)
      {
      /* -- Do the job cost sheet's crew laborers. */
      usRet = ReCalcJobCstShtCrewLabor (pszJobKey, pszJobCostSheetKey, aszCrewKeys[usCnt]);

      /* -- Do the job cost sheet's crew equipment. */
      usRet = ReCalcJobCstShtCrewEquipment (pszJobKey, pszJobCostSheetKey, aszCrewKeys[usCnt]);

      } /* End of for (usCnt=0; aszCrewKeys[usCnt]... loop */


   if (usRet == RETURN_ERROR)
      ItiErrWriteDebugMessage
         ("We have a key problem in CBEst calcfn ReCalcJobCstShtCrew. ");



   /* ===================================================== */
   /* -- Now update the JobCostSheet's LaborDailyCost total *
    * -- and the EquipmentDailyCost total for the current   *
    * -- cost sheet key.                                    */
   RecalcTotalForJCSCrews (pszJobKey, pszJobCostSheetKey);


   if (hhpLocalRecalc)
      ItiMemDestroyHeap (hhpLocalRecalc);

   return usRet;
   } /* End of function ReCalcJobCstShtCrew */



USHORT EXPORT RecalcTotalForJCSMaterials (PSZ pszJobKey, PSZ pszJobCostSheetKey)
   {
//   HQRY   hqry;
//   USHORT uCols, uErr, usCnt;
   USHORT usLen, usRet = 0;
   HHEAP  hhpLocal;
   PSZ    pszTestSubQuery = NULL;
   DOUBLE  dTestSubQuery = 0.0;
   CHAR   szResFmt[20] = "";
   CHAR   szTemp[250] = "";

   if ((pszJobKey == NULL) || (pszJobCostSheetKey == NULL))
      {
      ItiErrWriteDebugMessage
         ("CBEst calcfn RecalcTotalForJCSMaterials received a NULL key value.");
      return RETURN_ERROR;
      }

   hhpLocal = ItiMemCreateHeap (3000);
   if (hhpLocal == NULL)
      {
      ItiErrWriteDebugMessage
         ("CBEst calcfn RecalcTotalForJCSMaterials failed to create hhpLocal.");
      return RETURN_ERROR;
      }



   /* == Update the JobCostSheet's MaterialUnitCost total *
    * -- for the given cost sheet key.                    */
   ItiStrCpy (szTemp, 
             "/* CBEst calcfn RecalcTotalForJCSMaterials */ "
             "SELECT SUM(UnitPrice * Quantity) "
              "FROM JobMaterialUsed "
              "WHERE JobKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
   ItiStrCat (szTemp, " AND JobCostSheetKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobCostSheetKey, sizeof szTemp);


   pszTestSubQuery = ItiDbGetRow1Col1 (szTemp, hhpLocal, 0, 0, 0);
   if (pszTestSubQuery != NULL)
      {     
      dTestSubQuery = ItiStrToDouble (pszTestSubQuery);
      ItiFmtFormatString (pszTestSubQuery, szResFmt,
                          sizeof szResFmt, "Number,....", &usLen);
      ItiMemFree (hhpLocal, pszTestSubQuery);
      }
   else
      dTestSubQuery = 0.0;

   if (dTestSubQuery != 0.0)
      {
      ItiStrCpy (szTemp,
               "/* CBEst calcfn RecalcTotalForJCSMaterials */ "
               "UPDATE JobCostSheet SET MaterialsUnitCost =  "
               , sizeof szTemp);
      ItiStrCat (szTemp, szResFmt, sizeof szTemp);
      ItiStrCat (szTemp, " WHERE JobKey = ", sizeof szTemp);
      ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
      ItiStrCat (szTemp, " AND JobCostSheetKey = ", sizeof szTemp);
      ItiStrCat (szTemp, pszJobCostSheetKey, sizeof szTemp);
      }
   else
      {
      ItiStrCpy (szTemp,
               " /* CBEst calcfn RecalcTotalForJCSMaterials */ "
               "UPDATE JobCostSheet SET MaterialsUnitCost = 0.00 "
               "WHERE JobKey = ", sizeof szTemp);
      ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
      ItiStrCat (szTemp, " AND JobCostSheetKey = ", sizeof szTemp);
      ItiStrCat (szTemp, pszJobCostSheetKey, sizeof szTemp);
      }


   if (ItiDbExecSQLStatement(hhpLocal, szTemp) != 0)
      {
      ItiErrWriteDebugMessage ( " *** JobCostSheet Update failed for query: ");
      ItiErrWriteDebugMessage (szTemp);
      }

   if (NULL != hhpLocal)
      ItiMemDestroyHeap (hhpLocal);

   }/* End of Function RecalcTotalForJCSMaterials */





USHORT EXPORT ReCalcJobCstShtMaterials (PSZ pszJobKey, PSZ pszJobCostSheetKey)
   {
   HQRY   hqry;
   USHORT uCols, uErr, usCnt, usLen, usRet = 0;
   HHEAP  hhpLocal;
   PSZ    *ppszJobMaterialKey;
   PSZ    pszTestSubQuery = NULL;
   DOUBLE  dTestSubQuery = 0.0;
   CHAR   szResFmt[20] = "";
   CHAR   szTemp[550] = "";
   CHAR   aszJobMaterialKeys[MAXMATERIALSPERJCS][KEYLENGTH];


   if ((pszJobKey == NULL) || (pszJobCostSheetKey == NULL))
      {
      ItiErrWriteDebugMessage
         ("CBEst calcfn ReCalcJobCstShtMaterials received a NULL key value.");
      return RETURN_ERROR;
      }

   hhpLocal = ItiMemCreateHeap (MAX_HEAP_SIZE);
   if (hhpLocal == NULL)
      {
      ItiErrWriteDebugMessage
         ("CBEst calcfn ReCalcJobCstShtMaterials failed to create hhpLocal.");
      return RETURN_ERROR;
      }


   /* ------------------------------------------------------------- */
   /* -- Build array of JobMaterialKeys for current job cost sheet. */
   /* ------------------------------------------------------------- */
   ItiStrCpy (szTemp,
            " /* CBEst calcfn ReCalcJobCstShtMaterials */ "
            "SELECT MaterialKey FROM JobMaterialUsed "
            " WHERE JobKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
   ItiStrCat (szTemp," AND JobCostSheetKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobCostSheetKey, sizeof szTemp);


   if (!(hqry = ItiDbExecQuery(szTemp, hhpLocal, 0, 0, 0, &uCols, &uErr)))
      {
      return 13;
      }

   aszJobMaterialKeys[0][0] = '\0';
   usCnt = 0;
   while (ItiDbGetQueryRow(hqry, &ppszJobMaterialKey, &uErr)) 
      {
      ItiStrCpy (aszJobMaterialKeys[usCnt], ppszJobMaterialKey[0], KEYLENGTH);
      usCnt++;
      aszJobMaterialKeys[usCnt][0] = '\0';
      }

   /* -- For each job material key in the current job cost sheet... */
   for (usCnt=0; aszJobMaterialKeys[usCnt][0] ; usCnt++)
      {
      /* -- First test the subquery. -- */
      ItiStrCpy (szTemp,
             "/* CBEst calcfn ReCalcJobCstShtMaterials */ "
             "SELECT  MP.UnitPrice "
             "FROM JobMaterialUsed JMU, Material M, MaterialPrice MP,"
             " JobZone JZ, Job J "
             "WHERE JMU.MaterialKey = M.MaterialKey  "
             "AND M.MaterialKey = MP.MaterialKey "
             "AND JZ.ZoneKey = MP.ZoneKey "
             "AND J.BaseDate = MP.BaseDate "
             "AND J.JobKey = JMU.JobKey "
             "AND JZ.JobKey = JMU.JobKey "
             "AND (MP.UnitType = J.UnitType OR MP.UnitType = NULL) " 
             "AND JMU.JobKey = ", sizeof szTemp);
      ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
      ItiStrCat (szTemp, " AND JMU.JobCostSheetKey = ", sizeof szTemp);
      ItiStrCat (szTemp, pszJobCostSheetKey, sizeof szTemp);
      ItiStrCat (szTemp, " AND JMU.MaterialKey = ", sizeof szTemp);
      ItiStrCat (szTemp, aszJobMaterialKeys[usCnt], sizeof szTemp);

      pszTestSubQuery = ItiDbGetRow1Col1 (szTemp, hhpLocal, 0, 0, 0);
      if (pszTestSubQuery != NULL)
         {
         dTestSubQuery = ItiStrToDouble (pszTestSubQuery);
         ItiFmtFormatString (pszTestSubQuery, szResFmt,
                              sizeof szResFmt, "Number,....", &usLen);
         }
      else
         dTestSubQuery = 0.0;

      if (dTestSubQuery == 0.0)
         {
         /* -- Since the zone did not have a value, test if the */
         /* -- default zone key will give us a value to use.    */
         ItiStrCpy (szTemp,
             " /* CBEst ReCalcJobCstShtMaterials, query default prices. */ "
             "SELECT  MP.UnitPrice "
             "FROM JobMaterialUsed JMU, Material M, MaterialPrice MP,"
             " JobZone JZ, Job J "
             "WHERE JMU.MaterialKey = M.MaterialKey  "
             "AND M.MaterialKey = MP.MaterialKey "
             "AND MP.ZoneKey = 1 "
             "AND J.JobKey = JMU.JobKey "
             "AND JZ.JobKey = JMU.JobKey "
             "AND (MP.UnitType = J.UnitType OR MP.UnitType = NULL) "
             "AND JMU.JobKey = ", sizeof szTemp);
         ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
         ItiStrCat (szTemp, " AND JMU.JobCostSheetKey = ", sizeof szTemp);
         ItiStrCat (szTemp, pszJobCostSheetKey, sizeof szTemp);
         ItiStrCat (szTemp, " AND JMU.MaterialKey = ", sizeof szTemp);
         ItiStrCat (szTemp, aszJobMaterialKeys[usCnt], sizeof szTemp);

         
         if (pszTestSubQuery)      /* Recover old test query's memory. */
            ItiMemFree (hhpLocal, pszTestSubQuery);

         pszTestSubQuery = ItiDbGetRow1Col1 (szTemp, hhpLocal, 0, 0, 0);
         if (pszTestSubQuery != NULL)
            {
            ItiFmtFormatString (pszTestSubQuery, szResFmt,
                             sizeof szResFmt, "Number,....", &usLen);
            ItiStrCpy (szTemp,
               "/* CBEst calcfn ReCalcJobCstShtMaterials, using default zone price. */ "
               "UPDATE JobMaterialUsed SET UnitPrice = ", sizeof szTemp);
            ItiStrCat (szTemp, szResFmt, sizeof szTemp);
            ItiStrCat (szTemp, " WHERE JobKey = ", sizeof szTemp);
            ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
            ItiStrCat (szTemp, " AND JobCostSheetKey = ", sizeof szTemp);
            ItiStrCat (szTemp,  pszJobCostSheetKey, sizeof szTemp);
            ItiStrCat (szTemp, " AND MaterialKey = ", sizeof szTemp);
            ItiStrCat (szTemp,  aszJobMaterialKeys[usCnt], sizeof szTemp);

            }
         else
            {
            ItiStrCpy (szTemp,
               " /* CBEst calcfn ReCalcJobCstShtMaterials (Zero) */ "
               "UPDATE JobMaterialUsed SET UnitPrice = 0.0 "
               "WHERE JobKey = ", sizeof szTemp);
            ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
            ItiStrCat (szTemp, " AND JobCostSheetKey = ", sizeof szTemp);
            ItiStrCat (szTemp,  pszJobCostSheetKey, sizeof szTemp);
            ItiStrCat (szTemp, " AND MaterialKey = ", sizeof szTemp);
            ItiStrCat (szTemp,  aszJobMaterialKeys[usCnt], sizeof szTemp);
            }
         }
      else
         {
         /* -- Update the current material key's UnitPrice entry. -- */
         ItiStrCpy (szTemp,
            " /* ReCalcJobCstShtMaterials, using job zone material price. */ "
            "UPDATE JobMaterialUsed SET UnitPrice = ", sizeof szTemp);
         ItiStrCat (szTemp, szResFmt, sizeof szTemp);
         ItiStrCat (szTemp, " WHERE JobKey = ", sizeof szTemp);
         ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
         ItiStrCat (szTemp, " AND JobCostSheetKey = ", sizeof szTemp);
         ItiStrCat (szTemp,  pszJobCostSheetKey, sizeof szTemp);
         ItiStrCat (szTemp, " AND MaterialKey = ", sizeof szTemp);
         ItiStrCat (szTemp,  aszJobMaterialKeys[usCnt], sizeof szTemp);
         }

      if (pszTestSubQuery)
         ItiMemFree (hhpLocal, pszTestSubQuery);


      if (ItiDbExecSQLStatement(hhpLocal, szTemp) != 0)
         {
         ItiErrWriteDebugMessage ("*** JobCostSheet Update failed for query: ");
         ItiErrWriteDebugMessage (szTemp);
         }

      } /* end of for (k=0; aszJobMaterialKey... loop. */

   RecalcTotalForJCSMaterials (pszJobKey, pszJobCostSheetKey);
 
   if (NULL != hhpLocal)
      ItiMemDestroyHeap (hhpLocal);
   return usRet;

   } /* End of function ReCalcJobCstShtMaterials */






/* ------------------------------------------------------------- */
/* --   FUNCTION: RecalcJobCostSheet                             */
/* -- PARAMETERS: hheap                                          */
/* --             pszJobKey                                      */
/* --             pszJobCostSheetKey                             */
/* --                                                            */
/* -- DESCRIPTION:                                               */
/* --    Recalculates the given job cost sheet materials         */
/* --    and crews.                                              */
/* --                                                            */
/* -- RETURN: void                                               */
/* ------------------------------------------------------------- */
void EXPORT RecalcJobCostSheet (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey)
   {
   PSZ  *ppszPNames, *ppszParams;
   PSZ  pszJobItemKey = NULL; 
   PSZ  pszJobBrkdwnKey = NULL; 
   CHAR szRJCSTemp[480] = ""; 

   if ((pszJobCostSheetKey == NULL) || (pszJobKey == NULL))
      return;

   ItiDbExecSQLStatement (hheap,"select ' ** BEGIN RecalcJobCostSheet ' ");

   /* -- since we are REcalculating, zero out the JobCostSheet totals. */
   ItiStrCpy (szRJCSTemp,
            "/* CBEst calcfn ReCalcJobCostSheet, INIT values */ "
            "UPDATE JobCostSheet "
            " SET EquipmentDailyCost = 0.0"
                 ", LaborDailyCost = 0.0"
                 ", MaterialsUnitCost = 0.0"
            " WHERE JobKey = ", sizeof szRJCSTemp);
   ItiStrCat (szRJCSTemp, pszJobKey, sizeof szRJCSTemp);
   ItiStrCat (szRJCSTemp, " AND JobCostSheetKey = ", sizeof szRJCSTemp);
   ItiStrCat (szRJCSTemp, pszJobCostSheetKey, sizeof szRJCSTemp);

   if (ItiDbExecSQLStatement(hheap, szRJCSTemp) != 0)
      {
      ItiErrWriteDebugMessage ("*** JobCostSheet zeroing failed for query: ");
      ItiErrWriteDebugMessage (szRJCSTemp);
      }

   ReCalcJobCstShtCrew (pszJobKey, pszJobCostSheetKey);
   ReCalcJobCstShtMaterials (pszJobKey, pszJobCostSheetKey);

   ItiStrCpy (szRJCSTemp,
            " /* CBEst calcfn ReCalcJobCostSheet, get JIK */ "
            "SELECT JobItemKey FROM JobCostSheet "
            " WHERE JobKey = ", sizeof szRJCSTemp);
   ItiStrCat (szRJCSTemp, pszJobKey, sizeof szRJCSTemp);
   ItiStrCat (szRJCSTemp, " AND JobCostSheetKey = ", sizeof szRJCSTemp);
   ItiStrCat (szRJCSTemp, pszJobCostSheetKey, sizeof szRJCSTemp);

   pszJobItemKey = ItiDbGetRow1Col1(szRJCSTemp, hheap, 0, 0, 0);


   ItiStrCpy (szRJCSTemp,
            " /* CBEst calcfn ReCalcJobCostSheet, get JBK */ "
            "SELECT JobBreakdownKey FROM JobItem "
            " WHERE JobKey = ", sizeof szRJCSTemp);
   ItiStrCat (szRJCSTemp, pszJobKey, sizeof szRJCSTemp);
   ItiStrCat (szRJCSTemp, " AND JobItemKey = ", sizeof szRJCSTemp);
   ItiStrCat (szRJCSTemp, pszJobItemKey, sizeof szRJCSTemp);

   pszJobBrkdwnKey = ItiDbGetRow1Col1(szRJCSTemp, hheap, 0, 0, 0);

   /* -- Build a parameter list. */
   ppszPNames = ItiStrMakePPSZ (hheap, 5, "JobKey", "JobCostSheetKey", "JobBreakdownKey", "JobItemKey", NULL);
   ppszParams = ItiStrMakePPSZ (hheap, 5, pszJobKey, pszJobCostSheetKey,pszJobBrkdwnKey, pszJobItemKey, NULL);

   /* -- Now recalc the Job Cost Sheet's ProductionRate. */
   CalcJobCostShtProductionRate (hheap, ppszPNames, ppszParams);

   /* -- Now recalc the Job Cost Sheet's UnitPrice. */
   CalcJobCostShtUnitPrice (hheap, ppszPNames, ppszParams);

   ItiFreeStrArray (hheap, ppszPNames, 5);
   ItiFreeStrArray (hheap, ppszParams, 5);
   if (NULL != pszJobItemKey)
      ItiMemFree(hheap, pszJobItemKey);
   if (NULL != pszJobBrkdwnKey)
      ItiMemFree(hheap, pszJobBrkdwnKey);

   ItiDbExecSQLStatement (hheap,"select ' ** END RecalcJobCostSheet ' ");

   }/* End of Function RecalcJobCostSheet */





/* ---------------------------------------------------------------------*
 * Calculates the JobItem UnitPrice and ExtendedAmount                  *
 *                                                                      *
 * Needs: JobKey                                                        *
 *        JobItemKey                                                    *
 *        JobCostSheetKey                                               *
 ---------------------------------------------------------------------- */
int  EXPORT RecalcCBEJobItemUnitPrice (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
   HHEAP  hhpLocal;
   USHORT usRet, usLen;
   PSZ    pszXPrice, pszStdItemKey;
   DOUBLE dXPrice = 0.0;
   CHAR   szJobKey   [10] = "";
   CHAR   szJobCSKey [10] = "";
   CHAR   szZero[8] = " 0.0 ";
   CHAR   szXPriceFmt[20] = "";
//   CHAR   szTemp  [1024] = "";  // szCLRTemp3
//   CHAR   szTemp2 [1024] = "";  // szCLRTemp4

   hhpLocal = ItiMemCreateHeap (6144);

   usRet = ItiStrGet1Param (szJobKey, "JobKey",
              sizeof szJobKey, ppszPNames, ppszParams);

   usRet = ItiStrGet1Param (szJobCSKey, "JobCostSheetKey",
              sizeof szJobCSKey, ppszPNames, ppszParams);

   /* update the JobCostSheet Mats, Crews totals here */
   RecalcTotalForJCSMaterials (szJobKey, szJobCSKey);
   RecalcTotalForJCSCrews (szJobKey, szJobCSKey);


   usRet = ItiStrReplaceParams (szCLRTemp3,
                    "/* CESUtil.calc.RecalcCBEJobItemUnitPrice */ "
                    " SELECT SUM (QuantityPerItemUnit * UnitPrice) "
                    " FROM JobCostSheet JCS, JobCostBasedEstimate JCBE "
                    " WHERE JCS.JobKey= %JobKey "
                    " AND JCS.JobItemKey = %JobItemKey "
                    " AND JCS.JobItemCostBasedEstimateKey = "
                        " JCBE.JobItemCostBasedEstimateKey "
                    " AND JCBE.JobKey = %JobKey "
                    " AND JCBE.JobItemKey = %JobItemKey "
                    " AND JCBE.Active = 1 ",
              sizeof szCLRTemp3,
              ppszPNames,
              ppszParams,
              100);

   pszXPrice = ItiDbGetRow1Col1(szCLRTemp3, hhpLocal, 0, 0, 0);

   if (pszXPrice == NULL)
      {
      ItiErrWriteDebugMessage
         ("WARNING: cesutil calc RecalcCBEJobItemUnitPrice pszXPrice was NULL, from: ");
      ItiErrWriteDebugMessage (szCLRTemp3);
      pszXPrice = szZero;
      }
   else
      {
      dXPrice = ItiStrToDouble (pszXPrice);
// ItiErrWriteDebugMessage (" *** cesutil.calc *** CALCed JOB ITEM UNIT PRICE: ***** ");
// ItiErrWriteDebugMessage (pszXPrice);
// ItiErrWriteDebugMessage (szCLRTemp3);
      if (NULL != pszXPrice)
         ItiMemFree (hhpLocal, pszXPrice);
      ItiStrReplaceParams (szCLRTemp3, 
                           " SELECT StandardItemKey"
                           " FROM JobItem"
                           " WHERE JobItemKey = %JobItemKey"
                           " AND JobKey = %JobKey",
                           sizeof szCLRTemp3, ppszPNames, ppszParams, 100);
      pszStdItemKey = ItiDbGetRow1Col1 (szCLRTemp3, hhpLocal, 0, 0, 0);
      pszXPrice = ItiEstRoundUnitPrice (hhpLocal, dXPrice, pszStdItemKey);
      if (pszStdItemKey != NULL)
         {
         ItiMemFree (hhpLocal, pszStdItemKey);
         pszStdItemKey = NULL;
         }
      }

   ItiFmtFormatString (pszXPrice, szXPriceFmt, sizeof szXPriceFmt, "Number,......", &usLen);
//   ItiErrWriteDebugMessage (szXPriceFmt);

   ItiStrCpy (szCLRTemp4,
              " /* CESUtil.calc.RecalcCBEJobItemUnitPrice */ "
              "UPDATE JobItem "
              " SET UnitPrice = ", sizeof szCLRTemp4);
   ItiStrCat (szCLRTemp4, szXPriceFmt, sizeof szCLRTemp4);

   ItiStrCat (szCLRTemp4,
             " WHERE JobKey = %JobKey "
             " AND JobItemKey = %JobItemKey   "
             "UPDATE JobItem"
             " SET ExtendedAmount = ", sizeof szCLRTemp4);
   ItiStrCat (szCLRTemp4, szXPriceFmt, sizeof szCLRTemp4);

   ItiStrCat (szCLRTemp4,
                   " * (SELECT Quantity  "
                   " FROM JobItem "
                   " WHERE JobKey= %JobKey"
                   " AND JobItemKey = %JobItemKey)"
             " WHERE JobKey = %JobKey "
             " AND JobItemKey = %JobItemKey ", sizeof szCLRTemp4);

   usRet = ItiStrReplaceParams (szCLRTemp3, szCLRTemp4,
              sizeof szCLRTemp3,
              ppszPNames,
              ppszParams,
              100);

   if (usRet)
      {
      if (NULL != hhpLocal)
         ItiMemDestroyHeap (hhpLocal);
      return 1;
      }

   ItiDbBeginTransaction (hhpLocal);
   if (ItiDbExecSQLStatement (hhpLocal, szCLRTemp3))
      {
      ItiDbRollbackTransaction (hhpLocal);
      if (NULL != hhpLocal)
         ItiMemDestroyHeap (hhpLocal);
      return 1;
      }
   ItiDbCommitTransaction (hhpLocal);

   if (NULL != hhpLocal)
      ItiMemDestroyHeap (hhpLocal);
   return 0;
   } /* End Of Function RecalcCBEJobItemUnitPrice */






