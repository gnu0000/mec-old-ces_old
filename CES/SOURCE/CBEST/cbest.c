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
 * CBEst.c
 */

#define  INCL_DOSMISC

#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimbase.h"
#include "..\include\itidbase.h"
#include "..\include\winid.h"
#include "..\include\itibase.h"
#include "..\include\itierror.h"
#include "..\include\itiutil.h"
#include "..\include\itimenu.h"
#include "..\include\itiFmt.h"
#include "..\include\colid.h"
#include "..\include\itiglob.h"
#include "..\include\dialog.h"
#include "..\include\cbest.h"
#include "..\include\cesutil.h"
#include <stdio.h>
#include <string.h>
#include "cbest.h"
#include "CopyCBE.h"
#include "initcat.h"
#include "dialog.h"
#include "calcfn.h"

static CHAR szDllVersion[] = "1.1a0 CBEst.dll";

static BOOL bDoCrewEqLbRecalc;
//static CHAR szTempQry  [1000];

// static char szQuery     [400];
static CHAR szTemp     [1000];

/*
 * ItiDllQueryVersion returns the version number of this DLL.
 */

USHORT EXPORT ItiDllQueryVersion (void)
   {
   return VERSION;
   }





/*
 * ItiDllQueryCompatibility returns TRUE if this DLL is compatible
 * with the caller's version numer, otherwise FALSE is returned.
 */

BOOL EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion)
   {
   if ( ItiVerCmp(szDllVersion) )
      return TRUE;
   return FALSE;
   }





MRESULT EXPORT JobZoneStaticProc (HWND   hwnd,
                                  USHORT usMessage,
                                  MPARAM mp1,
                                  MPARAM mp2)
   {
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }





MRESULT EXPORT CostShtCrewProc (HWND   hwnd,
                                USHORT usMessage,
                                MPARAM mp1,
                                MPARAM mp2)
   {
   static HHEAP hhp;
   char   szTemp[120] = "";
   PSZ    pszTtl = NULL;
   PSZ    pszCostSheetKey = NULL;

//   hhp = WinSendMsg(hwnd, WM_ITIWNDQUERY, MPFROMSHORT(ITIWND_HEAP),0);   

   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               {
               BOOL bSelected;

               bSelected = ItiWndQueryActive (hwnd, CostShtCrewL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               }
               return 0;

            case IDM_VIEW_MENU:
               {
               hhp = ItiMemCreateHeap (MAX_HEAP_SIZE);
               if (hhp == NULL)
                  {
                  ItiErrWriteDebugMessage ("*** Failed to create heap in cbest.c ");
                  return 0;
                  }

               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_PARENT, TRUE);
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2),
                                           IDM_VIEW_USER_FIRST);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "C~rew For Cost Sheet",
                                      IDM_VIEW_USER_FIRST + 1);

               /* -- If no crews are available then gray view option. */
               pszCostSheetKey = (PSZ) WinSendMsg (hwnd, WM_ITIWNDQUERY,             
                                             MPFROMSHORT (ITIWND_DATA),        
                                             MPFROM2SHORT (0, cCostSheetKey)); 
               ItiStrCpy (szTemp,
                        " SELECT count(*) "
                        " FROM CrewUsed "
                        " WHERE CostSheetKey = ", sizeof szTemp);
               ItiStrCat (szTemp, pszCostSheetKey, sizeof szTemp); 
                                                                                     
               pszTtl = ItiDbGetRow1Col1 (szTemp, hhp, 0, 0, 0);                     
               if ((pszTtl != NULL) && (*pszTtl == '0'))
                  {
                  ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_VIEW_USER_FIRST + 1, TRUE);
                  }

               if ((hhp != NULL) && (pszTtl != NULL))                                
                  ItiMemFree (hhp, pszTtl);
               if (NULL != hhp)
                  ItiMemDestroyHeap (hhp);
               }
               return 0;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, CostShtCrewL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, CostShtCrewL, TRUE);
               break;


            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, CostShtCrewL), ITI_DELETE);
               break;


            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, CostShtCrewL);
               ItiWndBuildWindow (hwndChild, CrewForCostShtS);
               return 0;
            }
         break;
      }

   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }





MRESULT EXPORT MaterialProc (HWND   hwnd,
                             USHORT usMessage,
                             MPARAM mp1,
                             MPARAM mp2)
   {
   static HHEAP hhp;
   PGLOBALS pglob;
   CHAR     szUS[4] = "";
//   CHAR     szTemp[1020];
   PSZ      pszTtl = NULL;
   PSZ      pszCostSheetKey;

   pglob = ItiGlobQueryGlobalsPointer();
   if (pglob->bUseMetric)
      szUS[0] = '1';
   else
      szUS[0] = '0';


#define WINDOW_TTL                                                             \
         hhp = WinSendMsg(hwnd, WM_ITIWNDQUERY, MPFROMSHORT(ITIWND_HEAP),0);   \
                                                                               \
         pszCostSheetKey = (PSZ) WinSendMsg (hwnd, WM_ITIWNDQUERY,             \
                                             MPFROMSHORT (ITIWND_DATA),        \
                                             MPFROM2SHORT (0, cCostSheetKey)); \
         sprintf (szTemp,                                                      \
            " SELECT SUM(Quantity * UnitPrice) "                               \
            " FROM CodeValue CV, Material M, MaterialUsed MU, "                \
                 " MaterialPrice MP, CostSheet CS "                            \
            " WHERE MU.MaterialKey = M.MaterialKey"                            \
            " AND MP.MaterialKey = M.MaterialKey"                              \
            " AND M.Unit = CodeValueKey "                                      \
            " AND MP.ZoneKey = 1"                                              \
            " AND MP.BaseDate =  '" UNDECLARED_BASE_DATE "' "                  \
            " AND MU.CostSheetKey = %s "                                       \
            " AND CS.CostSheetKey = MU.CostSheetKey "                          \
            " AND ((MP.UnitType = CS.UnitType OR MP.UnitType = NULL) "         \
              " OR ( CS.UnitType = NULL "                                      \
                  " AND MP.UnitType = %s "                                     \
                  " AND (CV.UnitType = MP.UnitType OR CV.UnitType = NULL) ))"  \
            , pszCostSheetKey, szUS);                                          \
                                                                               \
                                                                               \
         pszTtl = ItiDbGetRow1Col1 (szTemp, hhp, 0, 0, 0);                     \
                                                                               \
         if (pszTtl != NULL)                                                   \
            {                                                                  \
            ItiFmtFormatString (pszTtl, szTemp, 32, "Number,$,..", NULL);      \
            }                                                                  \
         else                                                                  \
            {                                                                  \
            ItiStrCpy("$--,---.--", szTemp, sizeof szTemp);                    \
            }                                                                  \
                                                                               \
         WinSendMsg (hwnd, WM_ITIWNDSET,                                       \
                     MPFROM2SHORT (ITIWND_LABEL, 0),                           \
                     MPFROMP (szTemp));                                        \
                                                                               \
         if ((hhp != NULL) && (pszTtl != NULL))                                \
            ItiMemFree (hhp, pszTtl)
         





   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               {
               BOOL bSelected;
               /* -- The editing of Materials is contingent  */
               /* -- upon matching material unit system to the    */
               /* -- active global unit system.                   */
               PSZ pszUnitType;

//               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
//               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
//               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
//               return 0;
//               break;

               bSelected = ItiWndQueryActive (hwnd, MaterialL, NULL);
              // pglob = ItiGlobQueryGlobalsPointer();
              // if (pglob->bUseMetric)
              //    szUS[0] = '1';
              // else
              //    szUS[0] = '0';

               pszUnitType = (PSZ) QW (hwnd, ITIWND_DATA, 0, 0, cUnitType);
               if ( (pszUnitType == NULL)
                   || ((pszUnitType != NULL)
                        && ((CHAR)(*pszUnitType) == (CHAR)szUS[0])) )
                  {
                  ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, (FALSE & !bSelected) );
                  ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
                  ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, (FALSE & !bSelected) );
                  }
               else
                  { /* different unit system active, so prohibit editing. */
                  ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, TRUE);
                  ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, TRUE);
                  ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, TRUE);
                  }
               return 0;
               break;

               }
            }
         break;

      case WM_QUERYDONE:
      case WM_ITIDBBUFFERDONE:
         WINDOW_TTL;
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, MaterialL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, MaterialL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, MaterialL), ITI_DELETE);
               break;
            }

         WINDOW_TTL;
         break;

      }

   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }



static void SetTotals (HWND hwnd)
   {
   HHEAP    hheap;
   PSZ      *ppsz;
   PSZ pszJobKey = NULL;
   PSZ pszJobItemKey = NULL;
   PSZ pszJCBEKey = NULL;
   char     szQuery [400] = "";
   USHORT   usNumCols = 0;

   hheap = WinSendMsg (hwnd, WM_ITIWNDQUERY, MPFROMSHORT (ITIWND_HEAP), 0);
   pszJobKey = WinSendMsg (hwnd, WM_ITIWNDQUERY,
                           MPFROMSHORT (ITIWND_DATA),
                           MPFROM2SHORT (0, cJobKey));
   pszJobItemKey = WinSendMsg (hwnd, WM_ITIWNDQUERY,
                               MPFROMSHORT (ITIWND_DATA),
                               MPFROM2SHORT (0, cJobItemKey));
   pszJCBEKey = WinSendMsg (hwnd, WM_ITIWNDQUERY,
                            MPFROMSHORT (ITIWND_DATA),
                            MPFROM2SHORT (0, cJobItemCostBasedEstimateKey));

   if (     (pszJobKey == NULL) ||   (pszJCBEKey == NULL)  || (pszJobItemKey == NULL)
       || ((*pszJobKey)== '0')  || ((*pszJCBEKey)== '0') || ((*pszJobItemKey)== '0') )
      {
      sprintf (szQuery, "/* CBESt.cbest.c.SetTotals (has null key) */  SELECT 0, 0, 0");
      }
   else      
      {
      //           "SELECT sum(UnitPrice), sum(QuantityPerItemUnit), "
      ItiStrCpy (szQuery,
               "/* CBESt.cbest.c.SetTotals */ "
               "SELECT sum(QuantityPerItemUnit * UnitPrice) "
               "FROM JobCostSheet "
               "WHERE JobKey = ", sizeof szQuery);
      ItiStrCat (szQuery, pszJobKey, sizeof szQuery);
      ItiStrCat (szQuery, " AND JobItemCostBasedEstimateKey = ", sizeof szQuery);
      ItiStrCat (szQuery, pszJCBEKey, sizeof szQuery);
      ItiStrCat (szQuery, " AND JobItemKey = ", sizeof szQuery);
      ItiStrCat (szQuery, pszJobItemKey, sizeof szQuery);
      }

   ppsz = ItiDbGetRow1 (szQuery, hheap, hmodMe, ITIRID_CALC, JobTaskS,
                           &usNumCols);
   if (ppsz != NULL)
      {
      WinSendMsg (hwnd, WM_ITIWNDSET, MPFROM2SHORT (ITIWND_LABEL, 0),
                  MPFROMP (ppsz [0]));
//      WinSendMsg (hwnd, WM_ITIWNDSET, MPFROM2SHORT (ITIWND_LABEL, 1),
//                  MPFROMP (ppsz [1]));
//      WinSendMsg (hwnd, WM_ITIWNDSET, MPFROM2SHORT (ITIWND_LABEL, 2),
//                  MPFROMP (ppsz [2]));
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }
   else
      {
      WinSendMsg (hwnd, WM_ITIWNDSET, MPFROM2SHORT (ITIWND_LABEL, 0),
                  MPFROMP ("This can't happen"));
      }

   if (pszJobKey)
      ItiMemFree (hheap, pszJobKey);
   if (pszJobItemKey)
      ItiMemFree (hheap, pszJobItemKey);
   if (pszJCBEKey)
      ItiMemFree (hheap, pszJCBEKey);
   }




MRESULT EXPORT JobTaskProc (HWND   hwnd,
                            USHORT usMessage,
                            MPARAM mp1,
                            MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               {
               BOOL bSelected;

               bSelected = ItiWndQueryActive (hwnd, JobTaskL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               return 0;
               }
               break;

            case IDM_VIEW_MENU:
               {
               BOOL bSelected;

               bSelected = ItiWndQueryActive (hwnd, JobTaskL, NULL);

               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2),
                                           IDM_VIEW_USER_FIRST);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Job Cost Sheet",
                                      IDM_VIEW_USER_FIRST + 1);

               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_VIEW_USER_FIRST + 1, !bSelected);

               return 0;
               }
               break;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND   hwndChild;
            USHORT usActive, usCBEst;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, JobTaskL, FALSE);
               ItiWndSetHourGlass (TRUE);

               /* If quantity of a task is changed, and the JobItem's   */
               /* UnitPrice is calculated using CostBasedEstimate, then */
               /* the JobItem must be recalculated                      */
               usCBEst = DetermineJobItemEstMethod(hwnd);
               if (usCBEst == 0)
                  {
                  usActive = DetermineIfSelectedCBEActive(hwnd);
                  if (usActive == 0)
                     {
                     /* UPDATE TASK CALCS HERE! */
                     }
                  }
               SetTotals (hwnd);
               ItiWndSetHourGlass (FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, JobTaskL, TRUE);
               SetTotals (hwnd);
               break;

            case IDM_DELETE:
               {
               HHEAP hheap;
               USHORT us;
               PSZ   pszJobKey, pszJobItemKey, pszJobItemCostBasedEstimateKey;
               PSZ   pszQuantity, pszStandardItemKey, pszJobBrkdwnKey;
               PSZ    *ppszPNames, *ppszParams;
               CHAR  szTemp[256] = "";
               CHAR  szQuantity[64] = "";

               ItiWndSetHourGlass (TRUE);

               hheap            = (HHEAP) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_HEAP), 0);

               pszJobKey        = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_DATA),
                                   MPFROM2SHORT (0, cJobKey));

               pszJobItemKey     = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_DATA),
                                   MPFROM2SHORT (0, cJobItemKey));

               pszJobItemCostBasedEstimateKey = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                                MPFROMSHORT (ITIWND_DATA),
                                                MPFROM2SHORT (0, cJobItemCostBasedEstimateKey));

               ItiWndAutoModifyDb (WinWindowFromID (hwnd, JobTaskL), ITI_DELETE);

               /* If a task(s) is deleted, and the JobItem's UnitPrice */
               /* is calculated using CostBasedEstimate, then the      */
               /* JobItem must be recalculated                         */

               usCBEst = DetermineJobItemEstMethod (hwnd);
               if (usCBEst == 0)
                  {
//                  usActive = IsCBEForJobItemActive (hheap, pszJobKey, pszJobItemKey, pszJobItemCostBasedEstimateKey);
//                  usRet = DetermineIfJCShtsExist (hheap, pszJobKey, pszJobItemKey);

                  /* If JobCostSheets exist for the active JCBEst, and if the JobItem's    */
                  /* UnitPrice is calculated using CostBasedEstimation, then the JobItem's */
                  /* UnitPrice must be recalculated.                                       */
                  sprintf (szTemp,  " SELECT Quantity"
                                    " FROM JobItem"
                                    " WHERE JobItemKey = %s "
                                    " AND JobKey = %s ",
                                    pszJobItemKey, pszJobKey);
                  pszQuantity = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);
                             /* We need to remove any possible commas */
                             /* in the number formats.                */
                  us = 1 + ItiStrLen (pszQuantity);
                  ItiStrCpy(szQuantity,
                            (ItiExtract (pszQuantity, ",$")), us);

                  sprintf (szTemp,  " SELECT StandardItemKey"
                                    " FROM JobItem"
                                    " WHERE JobItemKey = %s "
                                    " AND JobKey = %s ",
                                    pszJobItemKey, pszJobKey);
                  pszStandardItemKey = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);

                  sprintf (szTemp,  " SELECT JobBreakdownKey"
                                    " FROM JobItem"
                                    " WHERE JobItemKey = %s "
                                    " AND JobKey = %s ",
                                    pszJobItemKey, pszJobKey);
                  pszJobBrkdwnKey = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);

                  RecalcCBEST(hheap,
                              pszJobKey,
                              pszJobBrkdwnKey,
                              pszJobItemKey,
                              szQuantity,
                              pszStandardItemKey);

                  /* Now we need to update the Job and JobBreakdown  */
                  /* to reflect the change.                          */
                  ppszPNames = ItiStrMakePPSZ (hheap, 4, "JobKey", "JobItemKey", "JobBreakdownKey", NULL);
                  ppszParams = ItiStrMakePPSZ (hheap, 4, pszJobKey, pszJobItemKey, pszJobBrkdwnKey, NULL);

                  CalcJobItemChgJobBrkdwn (hheap, ppszPNames, ppszParams);
                  CalcJobItemChgJob       (hheap, ppszPNames, ppszParams);

                  ItiFreeStrArray (hheap, ppszPNames, 4);
                  ItiFreeStrArray (hheap, ppszParams, 4);

                  if (pszQuantity)
                     ItiMemFree(hheap, pszQuantity);

                  if (pszJobBrkdwnKey)
                     ItiMemFree(hheap, pszJobBrkdwnKey);

                  if (pszStandardItemKey)
                     ItiMemFree(hheap, pszStandardItemKey);

                  /* If this CostBasedEstimation is active, but all of the */
                  /* JobCostSheets for this CBEst have been deleted        */
                  /* then JobItem's UnitPrice is set to .00                */
                  }

               SetTotals (hwnd);


               if (pszJobKey)
                  ItiMemFree(hheap, pszJobKey);
               if (pszJobItemKey)
                  ItiMemFree(hheap, pszJobItemKey);
               if (pszJobItemCostBasedEstimateKey)
                  ItiMemFree(hheap, pszJobItemCostBasedEstimateKey);

               ItiDbUpdateBuffers ("JobItem");
               ItiDbUpdateBuffers ("JobBreakdown");
               ItiDbUpdateBuffers ("Job");

               ItiWndSetHourGlass (FALSE);
               break;
               }

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, JobTaskL);
               ItiWndBuildWindow (hwndChild, JobCostShtS);
               return 0;
            }
         break;

      case WM_ITIDBBUFFERDONE:
      case WM_QUERYDONE:
         SetTotals (hwnd);
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }




MRESULT EXPORT JobCrewStaticProc (HWND   hwnd,
                                  USHORT usMessage,
                                  MPARAM mp1,
                                  MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               {
               BOOL bSelected;
               bSelected = ItiWndQueryActive(hwnd, JobCrewL, NULL);

               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               return 0;
               }
               break;

            case IDM_VIEW_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_PARENT, FALSE);
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2),
                                           IDM_VIEW_USER_FIRST);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "C~rew For Job",
                                      IDM_VIEW_USER_FIRST + 1);
               return 0;
               break;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND   hwndChild;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, JobCrewL, FALSE);
               break;

            case IDM_ADD:
               {
               static HHEAP  hhpLocAdd;
               static PSZ    pszJobKey, pszJobItemKey, pszJobBrkdwnKey, pszJobCostSheetKey;
               static PSZ    pszQuantity;
               static PSZ    pszStandardItemKey;
               PSZ    *ppszPNames, *ppszParams;
               USHORT us;
               CHAR   szTemp[128] = "";
               CHAR   szQuantity[16] = "";
               CHAR   szStandardItemKey[10] = "";
               CHAR   szJobKey[10] = "";
               CHAR   szJobCostSheetKey[10] = "";

               ItiWndDoDialog (hwnd, JobCrewL, TRUE);

               /* If a crew is added, and the JobItem's UnitPrice */
               /* is calculated using CostBasedEstimate, then the   */
               /* JobItem must be recalculated                      */
               /* JobCostSheet & JobBrkdwn & Job recalc/updates     */
               hhpLocAdd = ItiMemCreateHeap (10240);
               if (hhpLocAdd == NULL)
                  {
                  ItiErrWriteDebugMessage ("*** Failed to create heap in cbest.c JobCrewStaticProc ");
                  return 0;
                  }

               ItiWndSetHourGlass (TRUE);

               pszJobKey = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY,
                            MPFROMSHORT (ITIWND_DATA),
                            MPFROM2SHORT (0, cJobKey));
               ItiStrCpy(szJobKey, pszJobKey, sizeof szJobKey);

               pszJobCostSheetKey = (PSZ) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                               MPFROMSHORT (ITIWND_DATA),
                                               MPFROM2SHORT (0, cJobCostSheetKey));
               ItiStrCpy(szJobCostSheetKey,
                         pszJobCostSheetKey, sizeof szJobCostSheetKey);

               sprintf (szTemp, "SELECT JobItemKey "
                                "FROM JobCostSheet "
                                "WHERE JobKey = %s "
                                "AND JobCostSheetKey = %s", szJobKey, szJobCostSheetKey);

               pszJobItemKey = ItiDbGetRow1Col1(szTemp, hhpLocAdd, 0, 0, 0);


               sprintf (szTemp,  " SELECT JobBreakdownKey"
                                 " FROM JobItem"
                                 " WHERE JobItemKey = %s "
                                 " AND JobKey = %s ",
                                 pszJobItemKey, pszJobKey);
               pszJobBrkdwnKey = ItiDbGetRow1Col1 (szTemp, hhpLocAdd, 0, 0, 0);


               sprintf (szTemp,  " SELECT Quantity"
                                 " FROM JobItem"
                                 " WHERE JobItemKey = %s "
                                 " AND JobKey = %s ",
                                 pszJobItemKey, pszJobKey);
               pszQuantity = ItiDbGetRow1Col1 (szTemp, hhpLocAdd, 0, 0, 0);
                          /* We need to remove any possible commas */
                          /* in the number formats.                */
               us = 1 + ItiStrLen (pszQuantity);
               ItiStrCpy(szQuantity,
                         (ItiExtract (pszQuantity, ",$")), us);

               sprintf (szTemp,  " SELECT StandardItemKey"
                                 " FROM JobItem"
                                 " WHERE JobItemKey = %s "
                                 " AND JobKey = %s ",
                                 pszJobItemKey, szJobKey);
               pszStandardItemKey = ItiDbGetRow1Col1 (szTemp, hhpLocAdd, 0, 0, 0);

               RecalcCBEST(hhpLocAdd,
                           szJobKey,
                           pszJobBrkdwnKey,
                           pszJobItemKey,
                           szQuantity,
                           pszStandardItemKey);

               /* Now we need to update the Job and JobBreakdown  */
               /* to reflect the change.                          */
               ppszPNames = ItiStrMakePPSZ (hhpLocAdd, 4, "JobKey", "JobItemKey", "JobBreakdownKey", NULL);
               ppszParams = ItiStrMakePPSZ (hhpLocAdd, 4, pszJobKey, pszJobItemKey, pszJobBrkdwnKey, NULL);

               CalcJobItemChgJobBrkdwn (hhpLocAdd, ppszPNames, ppszParams);
               CalcJobItemChgJob       (hhpLocAdd, ppszPNames, ppszParams);

               ItiDbUpdateBuffers ("JobCostSheet");
               ItiDbUpdateBuffers ("JobItem");
               ItiDbUpdateBuffers ("JobBreakdown");
               ItiDbUpdateBuffers ("Job");

               ItiFreeStrArray (hhpLocAdd, ppszPNames, 4);
               ItiFreeStrArray (hhpLocAdd, ppszParams, 4);

               if (NULL != pszQuantity)
                  ItiMemFree(hhpLocAdd, pszQuantity);

               if (NULL != pszJobBrkdwnKey)
                  ItiMemFree(hhpLocAdd, pszJobBrkdwnKey);

               if (NULL != pszStandardItemKey)
                  ItiMemFree(hhpLocAdd, pszStandardItemKey);

               if (NULL != pszQuantity)
                  ItiMemFree(hhpLocAdd, pszQuantity);

               if (NULL != pszStandardItemKey)
                  ItiMemFree(hhpLocAdd, pszStandardItemKey);

               if (NULL != hhpLocAdd)
                  ItiMemDestroyHeap (hhpLocAdd);

               ItiWndSetHourGlass (FALSE);

               } /* end _ADD */
               break;

            case IDM_DELETE:
               {
               static HHEAP  hhpLocDel;
               PSZ    pszJobKey, pszJobItemKey, pszJobBrkdwnKey, pszJobCostSheetKey;
               PSZ    pszQuantity;
               PSZ    pszStandardItemKey;
               PSZ    *ppszPNames, *ppszParams;
               USHORT us;
               CHAR   szTemp[128] = "";
               CHAR   szQuantity[16] = "";
               CHAR   szStandardItemKey[10] = "";
               CHAR   szJobKey[10] = "";
               CHAR   szJobCostSheetKey[10] = "";

               ItiWndAutoModifyDb (WinWindowFromID (hwnd, JobCrewL), ITI_DELETE);
               hhpLocDel = ItiMemCreateHeap (10240);
               if (hhpLocDel == NULL)
                  {
                  ItiErrWriteDebugMessage ("*** Failed to create heap in cbest.c JobCrewStaticProc ");
                  return 0;
                  }

               ItiWndSetHourGlass (TRUE);

               pszJobKey = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY,
                            MPFROMSHORT (ITIWND_DATA),
                            MPFROM2SHORT (0, cJobKey));
               ItiStrCpy(szJobKey, pszJobKey, sizeof szJobKey);

               pszJobCostSheetKey = (PSZ) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                               MPFROMSHORT (ITIWND_DATA),
                                               MPFROM2SHORT (0, cJobCostSheetKey));
               ItiStrCpy(szJobCostSheetKey,
                         pszJobCostSheetKey, sizeof szJobCostSheetKey);

               sprintf (szTemp, "SELECT JobItemKey "
                                "FROM JobCostSheet "
                                "WHERE JobKey = %s "
                                "AND JobCostSheetKey = %s", szJobKey, szJobCostSheetKey);

               pszJobItemKey = ItiDbGetRow1Col1(szTemp, hhpLocDel, 0, 0, 0);


               sprintf (szTemp,  " SELECT JobBreakdownKey"
                                 " FROM JobItem"
                                 " WHERE JobItemKey = %s "
                                 " AND JobKey = %s ",
                                 pszJobItemKey, pszJobKey);
               pszJobBrkdwnKey = ItiDbGetRow1Col1 (szTemp, hhpLocDel, 0, 0, 0);


               sprintf (szTemp,  " SELECT Quantity"
                                 " FROM JobItem"
                                 " WHERE JobItemKey = %s "
                                 " AND JobKey = %s ",
                                 pszJobItemKey, pszJobKey);
               pszQuantity = ItiDbGetRow1Col1 (szTemp, hhpLocDel, 0, 0, 0);
                          /* We need to remove any possible commas */
                          /* in the number formats.                */
               us = 1 + ItiStrLen (pszQuantity);
               ItiStrCpy(szQuantity,
                         (ItiExtract (pszQuantity, ",$")), us);

               sprintf (szTemp,  " SELECT StandardItemKey"
                                 " FROM JobItem"
                                 " WHERE JobItemKey = %s "
                                 " AND JobKey = %s ",
                                 pszJobItemKey, szJobKey);
               pszStandardItemKey = ItiDbGetRow1Col1 (szTemp, hhpLocDel, 0, 0, 0);

               RecalcCBEST(hhpLocDel,
                           szJobKey,
                           pszJobBrkdwnKey,
                           pszJobItemKey,
                           szQuantity,
                           pszStandardItemKey);

               /* Now we need to update the Job and JobBreakdown  */
               /* to reflect the change.                          */
               ppszPNames = ItiStrMakePPSZ (hhpLocDel, 4, "JobKey", "JobItemKey", "JobBreakdownKey", NULL);
               ppszParams = ItiStrMakePPSZ (hhpLocDel, 4, pszJobKey, pszJobItemKey, pszJobBrkdwnKey, NULL);

               CalcJobItemChgJobBrkdwn (hhpLocDel, ppszPNames, ppszParams);
               CalcJobItemChgJob       (hhpLocDel, ppszPNames, ppszParams);

               ItiDbUpdateBuffers ("JobCostSheet");
               ItiDbUpdateBuffers ("JobItem");
               ItiDbUpdateBuffers ("JobBreakdown");
               ItiDbUpdateBuffers ("Job");

               ItiFreeStrArray (hhpLocDel, ppszPNames, 4);
               ItiFreeStrArray (hhpLocDel, ppszParams, 4);

               if (NULL != pszQuantity)
                  ItiMemFree(hhpLocDel, pszQuantity);

               if (NULL != pszJobBrkdwnKey)
                  ItiMemFree(hhpLocDel, pszJobBrkdwnKey);

               if (NULL != pszStandardItemKey)
                  ItiMemFree(hhpLocDel, pszStandardItemKey);

               if (NULL != pszQuantity)
                  ItiMemFree(hhpLocDel, pszQuantity);

               if (NULL != pszStandardItemKey)
                  ItiMemFree(hhpLocDel, pszStandardItemKey);

               if (NULL != hhpLocDel)
                  ItiMemDestroyHeap (hhpLocDel);

               ItiWndSetHourGlass (FALSE);

               }/* end _DELETE */
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, JobCrewL);
               ItiWndBuildWindow (hwndChild, CrewForJobS);
               return 0;
            }
         break;
      }

   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }






MRESULT EXPORT JobMaterialsStaticProc (HWND   hwnd,
                                       USHORT usMessage,
                                       MPARAM mp1,
                                       MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               {
               BOOL bSelected;

               bSelected = ItiWndQueryActive (hwnd, JobMaterialsL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               return 0;
               break;
               }
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
//            USHORT usCBEst, usActive;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, JobMaterialsL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, JobMaterialsL, TRUE);
               break;

            case IDM_DELETE:
               {
               static HHEAP  hhpLocDel;
               PSZ    pszJobKey, pszJobItemKey, pszJobBrkdwnKey, pszJobCostSheetKey;
               PSZ    pszQuantity;
               PSZ    pszStandardItemKey;
               PSZ    *ppszPNames, *ppszParams;
               USHORT us;
               CHAR   szTemp[128] = "";
               CHAR   szQuantity[16] = "";
               CHAR   szStandardItemKey[10] = "";
               CHAR   szJobKey[10] = "";
               CHAR   szJobCostSheetKey[10] = "";

               ItiWndAutoModifyDb (WinWindowFromID (hwnd, JobMaterialsL), ITI_DELETE);
               
               hhpLocDel = ItiMemCreateHeap (10240);
               if (hhpLocDel == NULL)
                  {
                  ItiErrWriteDebugMessage ("*** Failed to create heap in cbest.c JobCrewStaticProc ");
                  return 0;
                  }

               ItiWndSetHourGlass (TRUE);

               pszJobKey = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY,
                            MPFROMSHORT (ITIWND_DATA),
                            MPFROM2SHORT (0, cJobKey));
               ItiStrCpy(szJobKey, pszJobKey, sizeof szJobKey);

               pszJobCostSheetKey = (PSZ) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                               MPFROMSHORT (ITIWND_DATA),
                                               MPFROM2SHORT (0, cJobCostSheetKey));
               ItiStrCpy(szJobCostSheetKey,
                         pszJobCostSheetKey, sizeof szJobCostSheetKey);

               sprintf (szTemp, "SELECT JobItemKey "
                                "FROM JobCostSheet "
                                "WHERE JobKey = %s "
                                "AND JobCostSheetKey = %s", szJobKey, szJobCostSheetKey);

               pszJobItemKey = ItiDbGetRow1Col1(szTemp, hhpLocDel, 0, 0, 0);


               sprintf (szTemp,  " SELECT JobBreakdownKey"
                                 " FROM JobItem"
                                 " WHERE JobItemKey = %s "
                                 " AND JobKey = %s ",
                                 pszJobItemKey, pszJobKey);
               pszJobBrkdwnKey = ItiDbGetRow1Col1 (szTemp, hhpLocDel, 0, 0, 0);


               sprintf (szTemp,  " SELECT Quantity"
                                 " FROM JobItem"
                                 " WHERE JobItemKey = %s "
                                 " AND JobKey = %s ",
                                 pszJobItemKey, pszJobKey);
               pszQuantity = ItiDbGetRow1Col1 (szTemp, hhpLocDel, 0, 0, 0);
                          /* We need to remove any possible commas */
                          /* in the number formats.                */
               us = 1 + ItiStrLen (pszQuantity);
               ItiStrCpy(szQuantity,
                         (ItiExtract (pszQuantity, ",$")), us);

               sprintf (szTemp,  " SELECT StandardItemKey"
                                 " FROM JobItem"
                                 " WHERE JobItemKey = %s "
                                 " AND JobKey = %s ",
                                 pszJobItemKey, szJobKey);
               pszStandardItemKey = ItiDbGetRow1Col1 (szTemp, hhpLocDel, 0, 0, 0);

               RecalcCBEST(hhpLocDel,
                           szJobKey,
                           pszJobBrkdwnKey,
                           pszJobItemKey,
                           szQuantity,
                           pszStandardItemKey);

               /* Now we need to update the Job and JobBreakdown  */
               /* to reflect the change.                          */
               ppszPNames = ItiStrMakePPSZ (hhpLocDel, 4, "JobKey", "JobItemKey", "JobBreakdownKey", NULL);
               ppszParams = ItiStrMakePPSZ (hhpLocDel, 4, pszJobKey, pszJobItemKey, pszJobBrkdwnKey, NULL);

               CalcJobItemChgJobBrkdwn (hhpLocDel, ppszPNames, ppszParams);
               CalcJobItemChgJob       (hhpLocDel, ppszPNames, ppszParams);

               ItiDbUpdateBuffers ("JobCostSheet");
               ItiDbUpdateBuffers ("JobItem");
               ItiDbUpdateBuffers ("JobBreakdown");
               ItiDbUpdateBuffers ("Job");

               ItiFreeStrArray (hhpLocDel, ppszPNames, 4);
               ItiFreeStrArray (hhpLocDel, ppszParams, 4);

               if (NULL != pszQuantity)
                  ItiMemFree(hhpLocDel, pszQuantity);

               if (NULL != pszJobBrkdwnKey)
                  ItiMemFree(hhpLocDel, pszJobBrkdwnKey);

               if (NULL != pszStandardItemKey)
                  ItiMemFree(hhpLocDel, pszStandardItemKey);

               if (NULL != pszQuantity)
                  ItiMemFree(hhpLocDel, pszQuantity);

               if (NULL != pszStandardItemKey)
                  ItiMemFree(hhpLocDel, pszStandardItemKey);

               if (NULL != hhpLocDel)
                  ItiMemDestroyHeap (hhpLocDel);

               ItiWndSetHourGlass (FALSE);
               }
               break;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }






MRESULT EXPORT TaskProc (HWND   hwnd,
                         USHORT usMessage,
                         MPARAM mp1,
                         MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               {
               BOOL bSelected;

               bSelected = ItiWndQueryActive (hwnd, TaskL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               return 0;
               }
               break;

            case IDM_VIEW_MENU:
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2),
                                           IDM_VIEW_USER_FIRST);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Task Cost Sheet",
                                      IDM_VIEW_USER_FIRST + 1);
               return 0;
               break;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, TaskL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, TaskL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, TaskL), ITI_DELETE);
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, TaskL);
               ItiWndBuildWindow (hwndChild, TaskCostShtS);
               return 0;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }




MRESULT EXPORT TaskCostShtStaticProc (HWND   hwnd,
                                      USHORT usMessage,
                                      MPARAM mp1,
                                      MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, FALSE);
               return 0;
               break;

            case IDM_VIEW_MENU:
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2),
                                           IDM_VIEW_USER_FIRST);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Crew List",
                                      IDM_VIEW_USER_FIRST + 1);

					// BSR#921117-4102 addition
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Material List",
                                      IDM_VIEW_USER_FIRST + 2);
               return 0;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, 0, FALSE);
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               ItiWndBuildWindow (hwnd, CostShtCrewS);
               return 0;

            // BSR#921117-4102 addition
            case (IDM_VIEW_USER_FIRST + 2):
               ItiWndBuildWindow (hwnd, MaterialS);
               return 0;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }







MRESULT EXPORT CBEstForStdItemStaticProc (HWND   hwnd,
                                          USHORT usMessage,
                                          MPARAM mp1,
                                          MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               {
               BOOL bSelected;

               bSelected = ItiWndQueryActive (hwnd, CBEstForStdItemL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               return 0;
               }
               break;

            case IDM_VIEW_MENU:
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2),
                                           IDM_VIEW_USER_FIRST);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Task List",
                                      IDM_VIEW_USER_FIRST + 1);
               return 0;
               break;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, CBEstForStdItemL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, CBEstForStdItemL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, CBEstForStdItemL), ITI_DELETE);
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, CBEstForStdItemL);
               ItiWndBuildWindow (hwndChild, TaskS);
               return 0;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }






MRESULT EXPORT CBEstForJobItemStaticProc (HWND   hwnd,
                                          USHORT usMessage,
                                          MPARAM mp1,
                                          MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               {
               BOOL bSelected;

               bSelected = ItiWndQueryActive (hwnd, CBEstForJobItemL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               return 0;
               }
               break;

            case IDM_VIEW_MENU:
               {
               HWND   hwndLst;
               USHORT uRow;
               PSZ    pszActive;

               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2),
                                           IDM_VIEW_USER_FIRST);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Job Task List",
                                      IDM_VIEW_USER_FIRST + 1);

               hwndLst = WinWindowFromID(hwnd, CBEstForJobItemL);
               if (hwndLst == NULL)
                  break;
               uRow = (UM) QW (hwndLst, ITIWND_ACTIVE, 0, 0, 0);
               pszActive = (PSZ) QW (hwndLst, ITIWND_DATA, 0, uRow, cActive);
               if ((pszActive != NULL) && (*pszActive != 'Y'))
                  {
                  ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_VIEW_USER_FIRST+1, TRUE);
                  ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                         " Only Active CBEs have viewable tasks.",
                                         IDM_VIEW_USER_FIRST + 2);
                  }

               return 0;
               }
               break;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, CBEstForJobItemL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, CBEstForJobItemL, TRUE);
               break;

            case IDM_DELETE:
               {
               HWND     hwndList;
               HHEAP    hheapKeys;
               PSZ      *ppszSelKeys;
               USHORT   uKeys [] = {cJobKey, cJobItemKey, cJobItemCostBasedEstimateKey, cActive, 0};
               USHORT   uRet, usCBEst;

               hwndList = WinWindowFromID(hwnd, CBEstForJobItemL);

               hheapKeys = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0);

               /* store CBEs selected by user to be deleted */

               ItiDbBuildSelectedKeyArray(hwndList, hheapKeys, &ppszSelKeys, uKeys);

               uRet = ItiWndAutoModifyDb (WinWindowFromID (hwnd, CBEstForJobItemL), ITI_DELETE);
               if (uRet != 666)            /* if User did not cancel the delete */
                  {

                   /* Call a function to delete all of associated JCShts, Crews, etc.      */
                   /* If the JobCostBasedEstimate is Active, change the JobItem estimation */
                   /* method to ADHOC.  UnitPrice will remain the same.                    */

                  usCBEst = DetermineJobItemEstMethod(hwnd);

                  DoDelJobCostSheetsForCBE(hheapKeys, ppszSelKeys, usCBEst);
                  }
               }
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, CBEstForJobItemL);
               ItiWndBuildWindow (hwndChild, JobTaskS);
               return 0;

            case (IDM_VIEW_USER_FIRST + 2):
               break;

            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }




MRESULT EXPORT CrewForCostShtStaticProc (HWND   hwnd,
                                         USHORT usMessage,
                                         MPARAM mp1,
                                         MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, TRUE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, TRUE);
               return 0;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, 0, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, 0, TRUE);
               break;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }





MRESULT EXPORT EditCostBasedEstimateProc (HWND    hwnd,
                                          USHORT  usMessage,
                                          MPARAM  mp1,
                                          MPARAM  mp2)
   {
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }





MRESULT EXPORT EditJobCostBasedEstProc (HWND    hwnd,
                                        USHORT  usMessage,
                                        MPARAM  mp1,
                                        MPARAM  mp2)
   {
   USHORT  usActiveChecked;
   MRESULT mr;

   switch (usMessage)
      {
      case WM_QUERYDONE:
         usActiveChecked = (UM) WinSendDlgItemMsg(hwnd, DID_ACTIVE, BM_QUERYCHECK, 0, 0);

         /* If the CBE to be changed is the active one        */
         /* do not allow the user to change it to inactive    */
         if (usActiveChecked == 1) 
            WinEnableWindow (WinWindowFromID (hwnd, DID_ACTIVE), FALSE);
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
            case DID_ADDMORE:
               {
               USHORT  us, usChangeActive, usRet, usCBEst;
               HWND    hwndStatic;
               HHEAP   hheap;
               PSZ     pszJobKey, pszJobItemKey, pszJobBrkdwnKey;
               PSZ    *ppszPNames, *ppszParams;
               CHAR    szTemp[128] = "";
               PSZ     pszQuantity;
               CHAR    szQuantity[16] = "";
               PSZ     pszStandardItemKey;
               CHAR    szStandardItemKey[10] = "";

               hwndStatic = QW(hwnd, ITIWND_OWNERWND, 1, 0, 0);

               hheap            = (HHEAP) WinSendMsg (hwndStatic, WM_ITIWNDQUERY,
                                  MPFROMSHORT (ITIWND_HEAP), 0);

               pszJobKey        = (PSZ)   WinSendMsg (hwndStatic, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_DATA),
                                   MPFROM2SHORT (0, cJobKey));

               pszJobItemKey = (PSZ)   WinSendMsg (hwndStatic, WM_ITIWNDQUERY,
                                MPFROMSHORT (ITIWND_DATA),
                                MPFROM2SHORT (0, cJobItemKey));

               sprintf (szTemp,  " SELECT JobBreakdownKey"
                                 " FROM JobItem"
                                 " WHERE JobItemKey = %s "
                                 " AND JobKey = %s ",
                                 pszJobItemKey, pszJobKey);
               pszJobBrkdwnKey = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);

               /* If the user is changing a non active JobCostBasedEstimate    */
               /* to be active, then we must change the previously active      */
               /* JobCostBasedEstimate to be inactive.  The same applies       */
               /* if the user is adding a new JobCostBasedEstimate and wishes  */
               /* it to be the active one.                                     */

               usChangeActive = (UM) WinSendDlgItemMsg(hwnd, DID_ACTIVE, BM_QUERYCHECK, 0, 0);

               /* if the user set DID_ACTIVE to be active */
               if (usChangeActive == 1)       
                  {
                  usRet = DeactivatePreviouslyActiveCBE(hwnd);
                  mr = ItiWndDefDlgProc(hwnd, usMessage, mp1, mp2);
                  usRet = DetermineIfJCShtsExist(hheap, pszJobKey, pszJobItemKey);
                  usCBEst = DetermineJobItemEstMethod(hwndStatic);

                  /* If JobCostSheets exist for the active JCBEst, and if the JobItem's    */
                  /* UnitPrice is calculated using CostBasedEstimation, then the JobItem's */
                  /* UnitPrice must be recalculated.                                       */

                  if ((usRet == 0) && (usCBEst == 0))
                     {
                     sprintf (szTemp,  " SELECT Quantity"
                                       " FROM JobItem"
                                       " WHERE JobItemKey = %s "
                                       " AND JobKey = %s ",
                                       pszJobItemKey, pszJobKey);
                     pszQuantity = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);
                                /* We need to remove any possible commas */
                                /* in the number formats.                */
                     us = 1 + ItiStrLen (pszQuantity);
                     ItiStrCpy(szQuantity,
                               (ItiExtract (pszQuantity, ",$")), us);

                     sprintf (szTemp,  " SELECT StandardItemKey"
                                       " FROM JobItem"
                                       " WHERE JobItemKey = %s "
                                       " AND JobKey = %s ",
                                       pszJobItemKey, pszJobKey);
                     pszStandardItemKey = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);

                     RecalcCBEST(hheap,
                                 pszJobKey,
                                 pszJobBrkdwnKey,
                                 pszJobItemKey,
                                 szQuantity,
                                 pszStandardItemKey);

                     if (NULL != pszQuantity)
                        ItiMemFree(hheap, pszQuantity);
                     if (NULL != pszStandardItemKey)
                        ItiMemFree(hheap, pszStandardItemKey);
                     }

                  /* If CostBasedEstimation is used to calculate the JobItem   */
                  /* but there are no JobCostSheets for this CBEst             */
                  /* then JobItem's UnitPrice is set to 00                     */

                  if ((usRet == 1) &&  (usCBEst == 0))
                     DoSetJobItemUnitPriceToZero(hheap, pszJobKey, pszJobItemKey);

                  /* Now we need to update the Job and JobBreakdown  */
                  /* to reflect the change.                          */
                  ppszPNames = ItiStrMakePPSZ (hheap, 4, "JobKey", "JobItemKey", "JobBreakdownKey", NULL);
                  ppszParams = ItiStrMakePPSZ (hheap, 4, pszJobKey, pszJobItemKey, pszJobBrkdwnKey, NULL);

                  CalcJobItemChgJobBrkdwn (hheap, ppszPNames, ppszParams);
                  CalcJobItemChgJob       (hheap, ppszPNames, ppszParams);

                  ItiDbUpdateBuffers ("JobItem");
                  ItiDbUpdateBuffers ("JobBreakdown");
                  ItiDbUpdateBuffers ("Job");
                  }
               else
                  /* the user is adding a non-active CBE */
                  mr=ItiWndDefDlgProc(hwnd, usMessage, mp1, mp2);

               if (NULL != pszJobKey)
                  ItiMemFree(hheap, pszJobKey);
               if (NULL != pszJobItemKey)
                  ItiMemFree(hheap, pszJobItemKey);
               }
            return mr;
            break;

            case DID_CANCEL:   /* CANCEL button pushed */
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;

            default:
               break;
            }
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }





MRESULT EXPORT EditTasksForStdItemProc (HWND    hwnd,
                                        USHORT  usMessage,
                                        MPARAM  mp1,
                                        MPARAM  mp2)
   {
   switch (usMessage)
      {
      case WM_INITDLG:
         /* ADDED 17 Aug 93 */
         /* Disable push buttons until we fill the list box. */
         WinEnableWindow(WinWindowFromID(hwnd, 1), FALSE); /* OK button       */
         break;

      case WM_QUERYDONE:
      //   ItiWndActivateRow (hwnd, DID_UNIT, cCodeValueID, NULL, 0);
         ItiWndActivateRow (hwnd, DID_UNIT_NJ, cCodeValueID, NULL, 0);
         ItiWndActivateRow (hwnd, TaskCostShtEditDL, cCostSheetKey, NULL, 0);
         /* ADDED 17 Aug 93 */
         /* Now enable the push buttons. */
         WinEnableWindow(WinWindowFromID(hwnd, 1), TRUE);
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }






MRESULT EXPORT EditCostShtForTaskProc (HWND    hwnd,
                                       USHORT  usMessage,
                                       MPARAM  mp1,
                                       MPARAM  mp2)
   {
   BOOL bChg;
   PSZ  pszInitialGran;
   static CHAR szInitGran[8];

   switch (usMessage)
      {
      case WM_INITDLG:
         break;

      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, DID_PRODGRAN, 0, hwnd, 0);
         pszInitialGran = (PSZ) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_DATA),
                                   MPFROM2SHORT (0, cProductionGranularity));
         ItiStrCpy(szInitGran, pszInitialGran, sizeof szInitGran);
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_CANCEL:   /* CANCEL button pushed */
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;

            case DID_OK:
               /* -- If the ProductionGranularity changed we need to  */
               /* -- reset the ProductionRate to reflect that change. */
               bChg = ItiWndActiveRowChanged (hwnd, DID_PRODGRAN, 0, hwnd, 0);
               if (bChg)
                  {
//                  ResetProdRate (hwnd, FALSE, szInitGran);
                                    /* ^^^^^ This is NOT a Job Cost Sheet. */
                  }
               break;

            default:
               break;
            }
      }

   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }




MRESULT EXPORT EditChJobTaskListProc (HWND    hwnd,
                                      USHORT  usMessage,
                                      MPARAM  mp1,
                                      MPARAM  mp2)
   {
   switch (usMessage)
      {
      case WM_INITDLG:
         /* ADDED 17 Aug 93 */
         /* Disable push buttons until we fill the list box. */
         WinEnableWindow(WinWindowFromID(hwnd, 1), FALSE); /* OK button       */
         break;

      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, DID_UNIT, cCodeValueID, NULL, 0);

         /* ADDED 17 Aug 93 */
         /* Now enable the push buttons. */
         WinEnableWindow(WinWindowFromID(hwnd, 1), TRUE);
         break;
// /
      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               {
               MRESULT mr;
               HHEAP   hheap, hheapStatic;
               HWND    hwndStatic;
               PSZ     pszJobKey, pszJobItemKey;
               PSZ     pszStdItemKey;
               PSZ     pszJobBrkdwnKey, pszQuantity;
               PSZ    *ppszPNames, *ppszParams;

               hheap = WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_HEAP), 0);

               mr = ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);

               ItiWndSetHourGlass (TRUE);

               hwndStatic = QW(hwnd, ITIWND_OWNERWND, 1, 0, 0);
               hheapStatic = QW (hwndStatic, ITIWND_HEAP, 0, 0, 0);
               pszJobKey = QW (hwndStatic, ITIWND_DATA, 0, 0, cJobKey);
               pszJobItemKey = QW (hwndStatic, ITIWND_DATA, 0, 0, cJobItemKey);
               pszStdItemKey = QW (hwndStatic, ITIWND_DATA,
                                   0, 0, cStandardItemKey);
               pszJobBrkdwnKey = QW (hwndStatic, ITIWND_DATA,
                                   0, 0, cJobBreakdownKey);
               pszQuantity = QW (hwndStatic, ITIWND_DATA,
                                   0, 0, cQuantity);

               /* === Now do a recalcs here. === */
               RecalcCBEST(hheap,
                           pszJobKey,
                           pszJobBrkdwnKey,
                           pszJobItemKey,
                           pszQuantity,
                           pszStdItemKey);

               /* Now we need to update the Job and JobBreakdown  */
               /* to reflect the change.                          */
               ppszPNames = ItiStrMakePPSZ (hheap, 4, "JobKey", "JobItemKey", "JobBreakdownKey", NULL);
               ppszParams = ItiStrMakePPSZ (hheap, 4, pszJobKey, pszJobItemKey, pszJobBrkdwnKey, NULL);

               CalcJobItemChgJobBrkdwn (hheap, ppszPNames, ppszParams);
               CalcJobItemChgJob       (hheap, ppszPNames, ppszParams);

               ItiDbUpdateBuffers ("JobCostSheet");
               ItiDbUpdateBuffers ("JobItem");
               ItiDbUpdateBuffers ("JobBreakdown");
               ItiDbUpdateBuffers ("Job");

               ItiFreeStrArray (hheap, ppszPNames, 4);
               ItiFreeStrArray (hheap, ppszParams, 4);


               // if (pszJobKey)
               //    ItiMemFree (hheapStatic, pszJobKey);
               // if (pszJobItemKey)
               //    ItiMemFree (hheapStatic, pszJobItemKey);
                
               ItiWndSetHourGlass (FALSE);
               
               return mr;
               }
               break;

            case DID_CANCEL:   /* CANCEL button pushed */
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;

            default:
               break;
            }
// /
      }/* end of switch */
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }





/*
 * sets the cost sheet ID and cost sheet description controls
 * from the selected row in the list
 */

static void SetControlText (HWND hwnd)
   {
   PSZ      pszTemp;
   HWND     hwndList;
   HHEAP    hheap;
   USHORT   usRow;

   hwndList = WinWindowFromID (hwnd, TaskCostShtEditDL);
   if (hwndList == NULL)
      return;

   usRow = SHORT1FROMMR (WinSendMsg (hwndList, WM_ITIWNDQUERY,
                            MPFROMSHORT (ITIWND_ACTIVE), 0));

   hheap = WinSendMsg (hwndList, WM_ITIWNDQUERY,
                       MPFROMSHORT (ITIWND_HEAP), 0);
   if (hheap == NULL)
      return;

   pszTemp = WinSendMsg (hwndList, WM_ITIWNDQUERY,
                         MPFROMSHORT (ITIWND_DATA),
                         MPFROM2SHORT (usRow, cCostSheetID));
   if (pszTemp != NULL)
      {
      WinSetDlgItemText (hwnd, DID_COSTSHTID, pszTemp);
      ItiMemFree (hheap, pszTemp);
      }

   pszTemp = WinSendMsg (hwndList, WM_ITIWNDQUERY,
                         MPFROMSHORT (ITIWND_DATA),
                         MPFROM2SHORT (usRow, cDescription));
   if (pszTemp != NULL)
      {
      WinSetDlgItemText (hwnd, DID_DESCRIP, pszTemp);
      ItiMemFree (hheap, pszTemp);
      }
   }









MRESULT EXPORT EditAddJobTaskListProc (HWND    hwnd,
                                       USHORT  usMessage,
                                       MPARAM  mp1,
                                       MPARAM  mp2)
   {
   static PSZ pszKey;

   switch (usMessage)
      {
      case WM_ITILWMOUSE:
         if (SHORT1FROMMP (mp2) == TaskCostShtEditDL &&
             ItiWndIsSingleClick (mp2) && ItiWndIsLeftClick (mp2))
            {
            SetControlText (hwnd);
            }
         break;

      case WM_INITDLG:
         /* ADDED 17 Aug 93 */
         /* Disable push buttons until we fill the list box. */
         WinEnableWindow(WinWindowFromID(hwnd, 1), FALSE); /* OK button       */
         WinEnableWindow(WinWindowFromID(hwnd, 4), FALSE); /* Add More button */
         break;

      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, DID_UNIT, cCodeValueID, NULL, 0);
         /* ADDED 17 Aug 93 */
         /* Now enable the push buttons. */
         WinEnableWindow(WinWindowFromID(hwnd, 1), TRUE);
         WinEnableWindow(WinWindowFromID(hwnd, 4), TRUE);
         break;

      case WM_NEWKEY:
         {
         HHEAP hheap;

         if (ItiStrICmp ((PSZ) mp1, "JobCostSheet") == 0)
            {
            hheap = WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                MPFROMSHORT (ITIWND_HEAP), 0);
            pszKey = ItiStrDup (hheap, (PSZ) mp2);
            }
         return 0;
         break;
         }

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
            case DID_ADDMORE:
               {
               MRESULT mr;
               HHEAP   hheap, hheapList, hheapStatic;
               HWND    hwndList, hwndStatic;
               USHORT  usRow;
               PSZ     pszJobKey, pszJobItemKey, pszCostSheetKey;
               PSZ     pszJobItemCostBasedEstimateKey, pszStdItemKey;
               PSZ     pszJobBrkdwnKey, pszQuantity;
               PSZ    *ppszPNames, *ppszParams;

               hwndList = WinWindowFromID (hwnd, TaskCostShtEditDL);
               pszKey = NULL;

               hheap = WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_HEAP), 0);

               mr = ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);

               if (pszKey != NULL)
                  {
                  ItiWndSetHourGlass (TRUE);

                  usRow = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);
                  hheapList = QW (hwndList, ITIWND_HEAP, 0, 0, 0);
                  pszCostSheetKey = QW (hwndList, ITIWND_DATA, 0,
                                        usRow, cCostSheetKey);
                  hwndStatic = QW(hwnd, ITIWND_OWNERWND, 1, 0, 0);
                  hheapStatic = QW (hwndStatic, ITIWND_HEAP, 0, 0, 0);
                  pszJobKey = QW (hwndStatic, ITIWND_DATA, 0, 0, cJobKey);
                  pszJobItemKey = QW (hwndStatic, ITIWND_DATA, 0, 0, cJobItemKey);
                  pszJobItemCostBasedEstimateKey = QW (hwndStatic, ITIWND_DATA,
                                                       0, 0, cJobItemCostBasedEstimateKey);
                  pszStdItemKey = QW (hwndStatic, ITIWND_DATA,
                                      0, 0, cStandardItemKey);
                  pszJobBrkdwnKey = QW (hwndStatic, ITIWND_DATA,
                                      0, 0, cJobBreakdownKey);
                  pszQuantity = QW (hwndStatic, ITIWND_DATA,
                                      0, 0, cQuantity);

                  CopyCrewsForNewTask (hheap, 
                                       pszJobKey,
                                       pszKey,       /* JobCostSheetKey */
                                       pszCostSheetKey);
                  CreateJobMaterialsUsed (hheap, 
                                       pszJobKey, 
                                       pszKey,       /* JobCostSheetKey */
                                       pszCostSheetKey);
                  /* --- patch, could modify AddJobTaskListD in met file. --- */
                  ItiStrCpy (szTemp,
                           "/* EditAddJobTaskListProc */ "
                           "UPDATE JobCostSheet "
                           "SET ProductionRate = "
                               "(SELECT ProductionRate "
                                "FROM CostSheet "
                                "WHERE CostSheetKey = "
                              , sizeof szTemp);
                  ItiStrCat (szTemp, pszCostSheetKey, sizeof szTemp);
                  ItiStrCat (szTemp,
                           "), HoursPerDay = "
                               "(SELECT DefaultHoursPerDay "
                                "FROM CostSheet "
                                "WHERE CostSheetKey = "
                              , sizeof szTemp);
                  ItiStrCat (szTemp, pszCostSheetKey, sizeof szTemp);
                  ItiStrCat (szTemp,
                           "), ProductionGranularity = "
                               "(SELECT ProductionGranularity "
                                "FROM CostSheet "
                                "WHERE CostSheetKey = "
                              , sizeof szTemp);
                  ItiStrCat (szTemp, pszCostSheetKey, sizeof szTemp);
                  ItiStrCat (szTemp,
                           "), MarkUpPct = "
                               "(SELECT MarkUpPct "
                                "FROM CostSheet "
                                "WHERE CostSheetKey = "
                              , sizeof szTemp);
                  ItiStrCat (szTemp, pszCostSheetKey, sizeof szTemp);
                  ItiStrCat (szTemp,
                           "), LaborOverheadPct = "
                               "(SELECT LaborOverheadPct "
                                "FROM CostSheet "
                                "WHERE CostSheetKey = "
                              , sizeof szTemp);
                  ItiStrCat (szTemp, pszCostSheetKey, sizeof szTemp);
                  ItiStrCat (szTemp,
                           "), MaterialsOverheadPct = "
                               "(SELECT MaterialsOverheadPct "
                                "FROM CostSheet "
                                "WHERE CostSheetKey = "
                              , sizeof szTemp);
                  ItiStrCat (szTemp, pszCostSheetKey, sizeof szTemp);
                  ItiStrCat (szTemp,
                           "), EquipmentOverheadPct = "
                               "(SELECT EquipmentOverheadPct "
                                "FROM CostSheet "
                                "WHERE CostSheetKey = "
                              , sizeof szTemp);
                  ItiStrCat (szTemp, pszCostSheetKey, sizeof szTemp);
                  ItiStrCat (szTemp,
                              ") WHERE JobCostSheetKey =  "
                              , sizeof szTemp);
                  ItiStrCat (szTemp, pszKey, sizeof szTemp);

                  if (ItiDbExecSQLStatement (hheap, szTemp))
                     {
                     ItiErrWriteDebugMessage
                        ("Update of added JobCostSheet "
                         "FAILED in EditAddJobTaskListProc. ");
                     return 0;
                     }

                  /* === Now do a recalcs here. === */
                  RecalcCBEST(hheap,
                              pszJobKey,
                              pszJobBrkdwnKey,
                              pszJobItemKey,
                              pszQuantity,
                              pszStdItemKey);

                  /* Now we need to update the Job and JobBreakdown  */
                  /* to reflect the change.                          */
                  ppszPNames = ItiStrMakePPSZ (hheap, 4, "JobKey", "JobItemKey", "JobBreakdownKey", NULL);
                  ppszParams = ItiStrMakePPSZ (hheap, 4, pszJobKey, pszJobItemKey, pszJobBrkdwnKey, NULL);

                  CalcJobItemChgJobBrkdwn (hheap, ppszPNames, ppszParams);
                  CalcJobItemChgJob       (hheap, ppszPNames, ppszParams);

                  ItiDbUpdateBuffers ("JobCostSheet");
                  ItiDbUpdateBuffers ("JobItem");
                  ItiDbUpdateBuffers ("JobBreakdown");
                  ItiDbUpdateBuffers ("Job");

                  ItiFreeStrArray (hheap, ppszPNames, 4);
                  ItiFreeStrArray (hheap, ppszParams, 4);


                  if (pszKey)
                     ItiMemFree (hheap, pszKey);
                  // if (pszCostSheetKey)
                  //    ItiMemFree (hheapList, pszCostSheetKey);
                  // if (pszJobKey)
                  //    ItiMemFree (hheapStatic, pszJobKey);
                  // if (pszJobItemKey)
                  //    ItiMemFree (hheapStatic, pszJobItemKey);

                  ItiWndSetHourGlass (FALSE);
                  }
               return mr;
               }
               break;

            case DID_CANCEL:   /* CANCEL button pushed */
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;

            default:
               break;
            }
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }






MRESULT EXPORT EditMatForCostShtProc (HWND    hwnd,
                                      USHORT  usMessage,
                                      MPARAM  mp1,
                                      MPARAM  mp2)
   {
   switch (usMessage)
      {
      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, MatForCostShtDL, cMaterialKey, NULL, 0);
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }





MRESULT EXPORT EditJobMaterialsProc (HWND    hwnd,
                                     USHORT  usMessage,
                                     MPARAM  mp1,
                                     MPARAM  mp2)
   {
   static  MRESULT mr;

   switch (usMessage)
      {
      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, MatForCostShtDL, cMaterialKey, NULL, 0);
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP(mp1))
            {
            case DID_OK:
            case DID_ADDMORE:
               {
               HWND hwndList, hwndStatic, hwndParent;
               USHORT  usRow;
               PSZ    pszJobKey, pszJobCostSheetKey, pszMaterialKey;
               PSZ    pszJobBrkdwnKey, pszJobItemKey;
               PSZ    *ppszPNames, *ppszParams;
               HHEAP  hheap, hheap2;
               HHEAP  hheapList;

               hwndList = WinWindowFromID(hwnd, MatForCostShtDL);
               if (hwndList == NULL)
                  break;

               hwndParent = QW(hwnd, ITIWND_OWNERWND, 1, 0, 0);

               mr = ItiWndDefDlgProc(hwnd, usMessage, mp1, mp2);

               ItiWndSetHourGlass (TRUE);
               hwndStatic = QW(hwndParent, ITIWND_OWNERWND, 1, 0, 0);  

               hheap = (HHEAP) WinSendMsg (hwndParent, WM_ITIWNDQUERY,
                        MPFROMSHORT (ITIWND_HEAP), 0);

               hheap2 = (HHEAP) WinSendMsg (hwndStatic, WM_ITIWNDQUERY,
                        MPFROMSHORT (ITIWND_HEAP), 0);

//             usRow = (UM) WinSendMsg (hwndParent, WM_ITIWNDQUERY, 
//                       MPFROMSHORT (ITIWND_ACTIVE), 0);

               pszJobKey = (PSZ)WinSendMsg (hwndParent, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_DATA), 
                                   MPFROM2SHORT (0, cJobKey));

               pszJobItemKey = (PSZ)WinSendMsg (hwndStatic, WM_ITIWNDQUERY, 
                                       MPFROMSHORT (ITIWND_DATA), 
                                       MPFROM2SHORT (0, cJobItemKey));

               pszJobBrkdwnKey = (PSZ) WinSendMsg (hwndParent, WM_ITIWNDQUERY, 
                                    MPFROMSHORT (ITIWND_DATA), 
                                    MPFROM2SHORT (0, cJobBreakdownKey));

               pszJobCostSheetKey = (PSZ)WinSendMsg (hwndParent, WM_ITIWNDQUERY, 
                                            MPFROMSHORT (ITIWND_DATA), 
                                            MPFROM2SHORT (0, cJobCostSheetKey));

               usRow = SHORT1FROMMR (WinSendMsg (hwndList, WM_ITIWNDQUERY,
                                    MPFROMSHORT (ITIWND_ACTIVE),0));
            
               hheapList =(HHEAP) WinSendMsg (hwndList, WM_ITIWNDQUERY,
                                       MPFROMSHORT (ITIWND_HEAP), 0);
            

               pszMaterialKey = (PSZ)WinSendMsg (hwndList, WM_ITIWNDQUERY, 
                                            MPFROMSHORT (ITIWND_DATA), 
                                            MPFROM2SHORT (usRow, cMaterialKey));
   
               /* -- Now that we have any needed keys, do the work. */ 
               ReCalcJobCstShtMaterials (pszJobKey, pszJobCostSheetKey);

               //   DoReCalcJobBreakJobFunds(hwnd);       /* ***** */

               /* -- Build a parameter list. */
               ppszPNames = ItiStrMakePPSZ (hheap, 5, "JobKey", "JobCostSheetKey", "JobBreakdownKey", "JobItemKey", NULL);
               ppszParams = ItiStrMakePPSZ (hheap, 5, pszJobKey, pszJobCostSheetKey,pszJobBrkdwnKey, pszJobItemKey, NULL);
            
            //   /* -- Now recalc the Job Cost Sheet's ProductionRate. */
            //   CalcJobCostShtProductionRate (hheap, ppszPNames, ppszParams);
            
               /* -- Now recalc the Job Cost Sheet's UnitPrice. */
               CalcJobCostShtUnitPrice (hheap, ppszPNames, ppszParams);

               RecalcCBEJobItemUnitPrice (hheap, ppszPNames, ppszParams);

               CalcJobItemChgJobBrkdwn (hheap, ppszPNames, ppszParams);
               CalcJobItemChgJob (hheap, ppszPNames, ppszParams);
               ItiDbUpdateBuffers ("JobCostSheet");
               ItiDbUpdateBuffers ("Task");
               ItiDbUpdateBuffers ("JobItem");
               ItiDbUpdateBuffers ("Job");
               ItiDbUpdateBuffers ("JobBreakdown");


               if (pszJobKey)
                  ItiMemFree(hheap, pszJobKey);
               if (pszJobBrkdwnKey)
                  ItiMemFree(hheap, pszJobBrkdwnKey);
               if (pszJobItemKey)
                  ItiMemFree(hheap2, pszJobItemKey);
               if (pszJobCostSheetKey)
                  ItiMemFree(hheap, pszJobCostSheetKey);
               if (pszMaterialKey)
                  ItiMemFree(hheapList, pszMaterialKey);

               ItiWndSetHourGlass (FALSE);
               return mr;
               }
               break;

            case DID_CANCEL:   /* CANCEL button pushed */
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;

            default:
               break;
            }
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }







static BOOL CreateChildRelations (HHEAP hheap, PSZ CostSheetKey, PSZ CostSheetCrewKey, PSZ CrewKey);

static BOOL DelChildRelations (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey, PSZ pszJobCrewKey);

static BOOL DelCostShtChildRelations(HHEAP hheap, PSZ pszCostSheetKey, PSZ pszCostSheetCrewKey);





MRESULT EXPORT EditCrewForCostShtProc (HWND    hwnd,
                                       USHORT  usMessage,
                                       MPARAM  mp1,
                                       MPARAM  mp2)
   {
   static PSZ pszKey;               /* to obtain a NewKey */

   switch (usMessage)
      {
      case WM_ITILWMOUSE:
         if (SHORT1FROMMP (mp2) == CrewCatForCostShtD &&
             ItiWndIsSingleClick (mp2) && ItiWndIsLeftClick (mp2))
            {
            PSZ  pszTemp;
            HWND hwndList;
            HHEAP hheap;
            USHORT usRow;

            hwndList = WinWindowFromID (hwnd, CrewCatForCostShtD);
            if (hwndList == NULL)
               break;

            usRow = SHORT1FROMMR (WinSendMsg (hwndList, WM_ITIWNDQUERY,
                                     MPFROMSHORT (ITIWND_ACTIVE), 0));

            hheap = WinSendMsg (hwndList, WM_ITIWNDQUERY,
                                MPFROMSHORT (ITIWND_HEAP), 0);
            if (hheap == NULL)
               break;

            pszTemp = WinSendMsg (hwndList, WM_ITIWNDQUERY,
                                  MPFROMSHORT (ITIWND_DATA),
                                  MPFROM2SHORT (usRow, cCrewID));
            if (pszTemp != NULL)
               {
               WinSetDlgItemText (hwnd, DID_COSTSHTCREWID, pszTemp);
               ItiMemFree (hheap, pszTemp);
               }


            pszTemp = WinSendMsg (hwndList, WM_ITIWNDQUERY,
                                  MPFROMSHORT (ITIWND_DATA),
                                  MPFROM2SHORT (usRow, cDescription));
            if (pszTemp != NULL)
               {
               WinSetDlgItemText (hwnd, DID_DESCRIP, pszTemp);
               ItiMemFree (hheap, pszTemp);
               }
            }
         break;

      case WM_NEWKEY:
         {
         HHEAP hheap;

         if (ItiStrICmp ((PSZ) mp1, "CrewUsed") == 0)
            {
             hheap = WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                MPFROMSHORT (ITIWND_HEAP), 0);
             pszKey = ItiStrDup (hheap, (PSZ) mp2);
            }
         return 0;
         break;
         }

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
            case DID_ADDMORE:
               {
               MRESULT mr;
               HHEAP   hheap, hheapList, hheapStatic;
               HWND hwndList, hwndStatic;
               USHORT usRow, usUpDateID;
               PSZ pszCrewKey;
               PSZ pszCostSheetKey;
               PSZ pszCostSheetCrewKey;

               hwndList = WinWindowFromID(hwnd, CrewCatForCostShtD);
               pszKey = NULL;

               hheap  = WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                          MPFROMSHORT (ITIWND_HEAP), 0);


               /* if this is a change in Crews we need CostSheetCrewKey in order to delete the laborers */
               /* and equipment associated with the CostSheetCrew being changed                         */

               usUpDateID =  ItiWndGetDlgMode();

               if (usUpDateID == ITI_CHANGE)
                  {
                  pszCostSheetCrewKey = WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                                    MPFROMSHORT (ITIWND_DATA),
                                                    MPFROM2SHORT (0, cCostSheetCrewKey));
                  }

               mr = ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);

               if (pszKey != NULL)
                  {
                  usRow = SHORT1FROMMR (WinSendMsg (hwndList, WM_ITIWNDQUERY,
                                        MPFROMSHORT (ITIWND_ACTIVE), 0));

                  hheapList = WinSendMsg (hwndList, WM_ITIWNDQUERY,
                                      MPFROMSHORT (ITIWND_HEAP), 0);

                  pszCrewKey = WinSendMsg (hwndList, WM_ITIWNDQUERY,
                                      MPFROMSHORT (ITIWND_DATA),
                                      MPFROM2SHORT (usRow, cCrewKey));

                  hwndStatic = QW(hwnd, ITIWND_OWNERWND, 1, 0, 0);
                  hheapStatic  = (HHEAP) QW (hwndStatic, ITIWND_HEAP, 0, 0, 0);

                  hheap = WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                      MPFROMSHORT (ITIWND_HEAP), 0);


                  pszCostSheetKey = WinSendMsg (hwndStatic, WM_ITIWNDQUERY,
                                                MPFROMSHORT (ITIWND_DATA),
                                                MPFROM2SHORT (0, cCostSheetKey));


                  CreateChildRelations (hheap, pszCostSheetKey, pszKey, pszCrewKey);


                  /* if this is a change in Crews we must delete the old CostSheetCrewUsed */

                  usUpDateID =  ItiWndGetDlgMode();

                  if (usUpDateID == ITI_CHANGE)
                     {
                     DelCostShtChildRelations(hheap, pszCostSheetKey, pszCostSheetCrewKey);
                     ItiMemFree(hheap, pszCostSheetCrewKey);
                     }

                  if (pszKey)
                     ItiMemFree (hheap, pszKey);
                  if (pszCrewKey)
                     ItiMemFree (hheapList, pszCrewKey);
                  if (pszCostSheetKey)
                     ItiMemFree (hheapStatic, pszCostSheetKey);
                  }
               return mr;
               }
               break;

            case DID_CANCEL:   /* CANCEL button pushed */
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;

            default:
               break;
            }
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }










static BOOL CreateChildRelations (HHEAP hheap, 
                                  PSZ pszCostSheetKey, 
                                  PSZ pszCostSheetCrewKey, 
                                  PSZ pszCrewKey)
   {
   char szTemp [400];

   sprintf (szTemp,
            "INSERT INTO CrewLaborUsed "
            "(CostSheetKey, CostSheetCrewKey, LaborerKey, Quantity) "
            "SELECT %s, %s, LaborerKey, Quantity "
            "FROM CrewLabor "
            "WHERE CrewKey = %s",
            pszCostSheetKey, pszCostSheetCrewKey, pszCrewKey);

   if (ItiDbExecSQLStatement (hheap, szTemp))
      return FALSE;

   sprintf (szTemp,
            "INSERT INTO CrewEquipmentUsed "
            "(CostSheetKey, CostSheetCrewKey, EquipmentKey, Quantity) "
            "SELECT %s, %s, EquipmentKey, Quantity "
            "FROM CrewEquipment "
            "WHERE CrewKey = %s",
            pszCostSheetKey, pszCostSheetCrewKey, pszCrewKey);

   if (ItiDbExecSQLStatement (hheap, szTemp))
      return FALSE;

   return TRUE;
   }




static BOOL DelCostShtChildRelations (HHEAP hheap, 
                                      PSZ pszCostSheetKey, 
                                      PSZ pszCostSheetCrewKey)
   {
   char szTemp [300];

   sprintf (szTemp,
            " DELETE FROM CrewEquipmentUsed "
            " WHERE CostSheetKey = %s "
            " AND CostSheetCrewKey = %s "
            " DELETE FROM CrewLaborUsed "
            " WHERE CostSheetKey = %s "
            " AND CostSheetCrewKey = %s",
            pszCostSheetKey, pszCostSheetCrewKey,
            pszCostSheetKey, pszCostSheetCrewKey);

   if (ItiDbExecSQLStatement (hheap, szTemp))
      return FALSE;

   return TRUE;
   }







MRESULT EXPORT EditCrewCShtLbEqProc (HWND   hwnd,
                                     USHORT usMessage,
                                     MPARAM mp1,
                                     MPARAM mp2)
   {
   switch (usMessage)
     {
      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_ADD1:
               ItiWndDoNextDialog (hwnd, CrewCShtLaborL, CrewCShtLaborD, ITI_ADD);
               break;

            case DID_CHANGE1:
               ItiWndDoNextDialog (hwnd, CrewCShtLaborL, CrewCShtLaborD, ITI_CHANGE);
               break;

            case DID_ADD2:
               ItiWndDoNextDialog (hwnd, CrewCShtEquipL, CrewCShtEquipD, ITI_ADD);
               break;

            case DID_CHANGE2:
               ItiWndDoNextDialog (hwnd, CrewCShtEquipL, CrewCShtEquipD, ITI_CHANGE);
               break;

            case DID_DELETE1:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, CrewCShtLaborL), ITI_DELETE);
               break;

            case DID_DELETE2:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, CrewCShtEquipL), ITI_DELETE);
               break;
            }
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }






MRESULT EXPORT EditCrewCShtLaborProc (HWND   hwnd,
                                      USHORT usMessage,
                                      MPARAM mp1,
                                      MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, LaborCatL, cLaborerKey, hwnd,     0);
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }



MRESULT EXPORT EditCrewCShtEquipProc (HWND   hwnd,
                                      USHORT usMessage,
                                      MPARAM mp1,
                                      MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, EquipCatL, cEquipmentKey, hwnd,     0);
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }







static BOOL CreateSecondChildRelations (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey, PSZ pszJobCrewKey, PSZ pszCrewKey);



MRESULT EXPORT EditCrewForJobProc (HWND    hwnd,
                                   USHORT  usMessage,
                                   MPARAM  mp1,
                                   MPARAM  mp2)
   {
   static PSZ pszKey;

   switch (usMessage)
      {
      case WM_ITILWMOUSE:
         if (SHORT1FROMMP (mp2) == CrewCatForCostShtD &&
             ItiWndIsSingleClick (mp2) && ItiWndIsLeftClick (mp2))
            {
            PSZ  pszTemp;
            HWND hwndList;
            HHEAP hheap;
            USHORT usRow;

            hwndList = WinWindowFromID (hwnd, CrewCatForCostShtD);
            if (hwndList == NULL)
               break;

            usRow = SHORT1FROMMR (WinSendMsg (hwndList, WM_ITIWNDQUERY,
                                     MPFROMSHORT (ITIWND_ACTIVE), 0));

            hheap = WinSendMsg (hwndList, WM_ITIWNDQUERY,
                                MPFROMSHORT (ITIWND_HEAP), 0);
            if (hheap == NULL)
               break;

            pszTemp = WinSendMsg (hwndList, WM_ITIWNDQUERY,
                                  MPFROMSHORT (ITIWND_DATA),
                                  MPFROM2SHORT (usRow, cCrewID));
            if (pszTemp != NULL)
               {
               WinSetDlgItemText (hwnd, DID_JOBCREWID, pszTemp);
               ItiMemFree (hheap, pszTemp);
               }


            pszTemp = WinSendMsg (hwndList, WM_ITIWNDQUERY,
                                  MPFROMSHORT (ITIWND_DATA),
                                  MPFROM2SHORT (usRow, cDescription));
            if (pszTemp != NULL)
               {
               WinSetDlgItemText (hwnd, DID_DESCRIP, pszTemp);
               ItiMemFree (hheap, pszTemp);
               }
            }
         break;

      case WM_NEWKEY:
         {
         HHEAP hheap;

         if (ItiStrICmp ((PSZ) mp1, "JobCrewUsed") == 0)
            {
             hheap = WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                MPFROMSHORT (ITIWND_HEAP), 0);
             pszKey = ItiStrDup (hheap, (PSZ) mp2);
            }
         return 0;
         break;
         }

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_CANCEL:   /* CANCEL button pushed */
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;

            case DID_OK:
            case DID_ADDMORE:
               {

               MRESULT mr;
               HHEAP   hheap, hheapList, hheapStatic;
               HWND hwndList, hwndStatic;
               USHORT usRow;
               USHORT usUpDateID;
               PSZ pszJobKey;
               PSZ pszJobCostSheetKey;
               PSZ pszJobCrewKey;
               PSZ pszCrewKey;


               hwndList = WinWindowFromID(hwnd, CrewCatForCostShtD);
               pszKey = NULL;

               hheap  = WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                          MPFROMSHORT (ITIWND_HEAP), 0);


              /* if this is a change in Crews we need JobCrewKey in order to delete the laborers */
              /* and equipment associated with the JobCrew being changed                         */

               usUpDateID =  ItiWndGetDlgMode();

               if (usUpDateID == ITI_CHANGE)
                 {
                 pszJobCrewKey = WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                             MPFROMSHORT (ITIWND_DATA),
                                             MPFROM2SHORT (0, cJobCrewKey));
                 }

               mr = ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);

               if (pszKey != NULL)
                  {
                  usRow = SHORT1FROMMR (WinSendMsg (hwndList, WM_ITIWNDQUERY,
                                        MPFROMSHORT (ITIWND_ACTIVE), 0));

                  hheapList = WinSendMsg (hwndList, WM_ITIWNDQUERY,
                                      MPFROMSHORT (ITIWND_HEAP), 0);

                  pszCrewKey = WinSendMsg (hwndList, WM_ITIWNDQUERY,
                                      MPFROMSHORT (ITIWND_DATA),
                                      MPFROM2SHORT (usRow, cCrewKey));


                  hwndStatic = QW(hwnd, ITIWND_OWNERWND, 1, 0, 0);
                  hheapStatic = WinSendMsg (hwndStatic, WM_ITIWNDQUERY,
                                            MPFROMSHORT (ITIWND_HEAP), 0);

                  pszJobCostSheetKey = WinSendMsg (hwndStatic, WM_ITIWNDQUERY,
                                                   MPFROMSHORT (ITIWND_DATA),
                                                   MPFROM2SHORT (0, cJobCostSheetKey));

                  pszJobKey = WinSendMsg (hwndStatic, WM_ITIWNDQUERY,
                                          MPFROMSHORT (ITIWND_DATA),
                                          MPFROM2SHORT (0, cJobKey));

                  /* Call a function to insert the laborers and equipment, associated */
                  /* with the new JobCrew,  into the appropriate db tables            */

                  CreateSecondChildRelations (hheapStatic, pszJobKey, pszJobCostSheetKey, pszKey, pszCrewKey);

                  /* if this is a change in Crews we must delete the old JobCrewUsed */

                  usUpDateID =  ItiWndGetDlgMode();

                  if (usUpDateID == ITI_CHANGE)
                     {
                     DelChildRelations(hheap, pszJobKey, pszJobCostSheetKey, pszJobCrewKey);
                     ItiMemFree(hheap, pszJobCrewKey);
                     }

                  if (pszKey)
                     ItiMemFree (hheap, pszKey);
                  if (pszCrewKey)
                     ItiMemFree (hheapList, pszCrewKey);
                  if (pszJobCostSheetKey)
                     ItiMemFree (hheapStatic, pszJobCostSheetKey);
                  if (pszJobKey)
                     ItiMemFree (hheapStatic, pszJobKey);
                  }
               return mr;
               }
               break;

            default:
               break;
            }
         break;

      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }





static BOOL DelChildRelations (HHEAP hheap, 
                               PSZ pszJobKey, 
                               PSZ pszJobCostSheetKey, 
                               PSZ pszJobCrewKey)
   {
   char szTemp [400] = "";


   sprintf (szTemp,
            "DELETE FROM JobCrewEquipmentUsed "
            "WHERE JobKey = %s "
            "AND JobCostSheetKey = %s "
            "AND JobCrewKey = %s "
            "DELETE FROM JobCrewLaborUsed "
            "WHERE JobKey = %s "
            "AND JobCostSheetKey = %s "
            "AND JobCrewKey = %s",                    
            pszJobKey, pszJobCostSheetKey, pszJobCrewKey,
            pszJobKey, pszJobCostSheetKey, pszJobCrewKey);

   if (ItiDbExecSQLStatement (hheap, szTemp))
      return FALSE;

   return TRUE;
   }





static BOOL CreateSecondChildRelations (HHEAP hheap, 
                                        PSZ pszJobKey, 
                                        PSZ pszJobCostSheetKey, 
                                        PSZ pszJobCrewKey, 
                                        PSZ pszCrewKey)
   {
   char szTemp  [700];
   char szTemp2 [700];
   PSZ  pszDavisBacon;

   /* Determines whether the Job uses DavisBacon or NonDavisBacon wages  */

   sprintf(szTemp2, "SELECT DavisBaconWages "
                    "FROM Job "
                    "WHERE JobKey = %s", pszJobKey);

   pszDavisBacon = ItiDbGetRow1Col1(szTemp2, hheap, 0, 0, 0);

   if ((pszDavisBacon != NULL) && (pszDavisBacon[0] == '0'))
      {
      sprintf (szTemp,
               "INSERT INTO JobCrewLaborUsed "
               "(JobKey, JobCostSheetKey, JobCrewKey, LaborerKey, Quantity, Rate, OvertimeRate) "
               "SELECT %s, %s, %s, CrewLabor.LaborerKey, CrewLabor.Quantity, "
               "NonDavisBaconRate, NonDavisBaconOvertimeRate "
               "FROM CrewLabor, Labor, JobZone, Job, LaborerWageAndBenefits "
               "WHERE CrewLabor.LaborerKey = Labor.LaborerKey "
               "AND Labor.ZoneTypeKey = JobZone.ZoneTypeKey "
               "AND JobZone.JobKey = Job.JobKey "
               "AND Job.BaseDate = LaborerWageAndBenefits.BaseDate "
               "AND LaborerWageAndBenefits.ZoneKey = JobZone.ZoneKey "
               "AND LaborerWageAndBenefits.LaborerKey = CrewLabor.LaborerKey "
               "AND CrewLabor.CrewKey = %s "
               "AND Job.JobKey = %s",
               pszJobKey, pszJobCostSheetKey, pszJobCrewKey,
               pszCrewKey, pszJobKey);
      }
   else
      {
      sprintf (szTemp,
               "INSERT INTO JobCrewLaborUsed "
               "(JobKey, JobCostSheetKey, JobCrewKey, LaborerKey, Quantity, Rate, OvertimeRate) "
               "SELECT %s, %s, %s, CrewLabor.LaborerKey, CrewLabor.Quantity, "
               "DavisBaconRate, DavisBaconOvertimeRate "
               "FROM CrewLabor, Labor, JobZone, Job, LaborerWageAndBenefits "
               "WHERE CrewLabor.LaborerKey = Labor.LaborerKey "
               "AND Labor.ZoneTypeKey = JobZone.ZoneTypeKey "
               "AND JobZone.JobKey = Job.JobKey "
               "AND Job.BaseDate = LaborerWageAndBenefits.BaseDate "
               "AND LaborerWageAndBenefits.ZoneKey = JobZone.ZoneKey "
               "AND LaborerWageAndBenefits.LaborerKey = CrewLabor.LaborerKey "
               "AND CrewLabor.CrewKey = %s "
               "AND Job.JobKey = %s",
               pszJobKey, pszJobCostSheetKey, pszJobCrewKey,
               pszCrewKey, pszJobKey);
      }
   if (pszDavisBacon)
      ItiMemFree (hheap, pszDavisBacon);

   if (ItiDbExecSQLStatement (hheap, szTemp))
      return FALSE;

   sprintf (szTemp,
            "INSERT INTO JobCrewEquipmentUsed "
            "(JobKey, JobCostSheetKey, JobCrewKey, EquipmentKey, Quantity, Rate, OvertimeRate) "
            "SELECT %s, %s, %s, CrewEquipment.EquipmentKey, CrewEquipment.Quantity, "
            "Rate, OvertimeRate "
            "FROM CrewEquipment, Equipment, JobZone, Job, EquipmentRate "
            "WHERE CrewEquipment.EquipmentKey = Equipment.EquipmentKey "
            "AND Equipment.ZoneTypeKey = JobZone.ZoneTypeKey "
            "AND JobZone.JobKey = Job.JobKey "
            "AND Job.BaseDate = EquipmentRate.BaseDate "
            "AND EquipmentRate.ZoneKey = JobZone.ZoneKey "
            "AND EquipmentRate.EquipmentKey = CrewEquipment.EquipmentKey "
            "AND CrewEquipment.CrewKey = %s "
            "AND Job.JobKey = %s",
            pszJobKey, pszJobCostSheetKey, pszJobCrewKey,
            pszCrewKey, pszJobKey);


   if (ItiDbExecSQLStatement (hheap, szTemp))
      return FALSE;


   return TRUE;
   }







MRESULT EXPORT CrewForJobStaticProc (HWND   hwnd,
                                     USHORT usMessage,
                                     MPARAM mp1,
                                     MPARAM mp2)
   {
//   USHORT usCBEst, usActive;

   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, TRUE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, TRUE);
               return 0;
               break;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
               {
               ItiWndDoDialog (hwnd, 0, FALSE);
               if (bDoCrewEqLbRecalc)
                  {
                  HHEAP  hhpLocChg;
                  USHORT us;
                  PSZ    pszJobKey, pszJobItemKey, pszJobBrkdwnKey;
                  PSZ    pszJobCostSheetKey;
                  PSZ    pszJobCrewKey;
                  PSZ    *ppszPNames, *ppszParams;
                  CHAR   szTemp[128] = "";
                  PSZ    pszQuantity;
                  CHAR   szQuantity[16] = "";
                  PSZ    pszStandardItemKey;
                  CHAR   szStandardItemKey[10] = "";


                  ItiWndSetHourGlass (TRUE);

                  hhpLocChg = (HHEAP) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                              MPFROMSHORT (ITIWND_HEAP), 0);

                  pszJobKey = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY,
                               MPFROMSHORT (ITIWND_DATA),
                               MPFROM2SHORT (0, cJobKey));

                  pszJobCostSheetKey = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                       MPFROMSHORT (ITIWND_DATA),
                                       MPFROM2SHORT (0, cJobCostSheetKey));

                  pszJobCrewKey = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                  MPFROMSHORT (ITIWND_DATA),
                                  MPFROM2SHORT (0, cJobCrewKey));

                  pszJobItemKey = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_DATA),
                                   MPFROM2SHORT (0, cJobItemKey));

                  sprintf (szTemp,  " SELECT JobBreakdownKey"
                                    " FROM JobItem"
                                    " WHERE JobItemKey = %s "
                                    " AND JobKey = %s ",
                                    pszJobItemKey, pszJobKey);
                  pszJobBrkdwnKey = ItiDbGetRow1Col1 (szTemp, hhpLocChg, 0, 0, 0);


                  sprintf (szTemp,  " SELECT Quantity"
                                    " FROM JobItem"
                                    " WHERE JobItemKey = %s "
                                    " AND JobKey = %s ",
                                    pszJobItemKey, pszJobKey);
                  pszQuantity = ItiDbGetRow1Col1 (szTemp, hhpLocChg, 0, 0, 0);
                             /* We need to remove any possible commas */
                             /* in the number formats.                */
                  us = 1 + ItiStrLen (pszQuantity);
                  ItiStrCpy(szQuantity,
                            (ItiExtract (pszQuantity, ",$")), us);

                  sprintf (szTemp,  " SELECT StandardItemKey"
                                    " FROM JobItem"
                                    " WHERE JobItemKey = %s "
                                    " AND JobKey = %s ",
                                    pszJobItemKey, pszJobKey);
                  pszStandardItemKey = ItiDbGetRow1Col1 (szTemp, hhpLocChg, 0, 0, 0);

                  RecalcCBEST(hhpLocChg,
                              pszJobKey,
                              pszJobBrkdwnKey,
                              pszJobItemKey,
                              szQuantity,
                              pszStandardItemKey);

                  /* Now we need to update the Job and JobBreakdown  */
                  /* to reflect the change.                          */
                  ppszPNames = ItiStrMakePPSZ (hhpLocChg, 4, "JobKey", "JobItemKey", "JobBreakdownKey", NULL);
                  ppszParams = ItiStrMakePPSZ (hhpLocChg, 4, pszJobKey, pszJobItemKey, pszJobBrkdwnKey, NULL);

                  CalcJobItemChgJobBrkdwn (hhpLocChg, ppszPNames, ppszParams);
                  CalcJobItemChgJob       (hhpLocChg, ppszPNames, ppszParams);

                  ItiDbUpdateBuffers ("JobCostSheet");
                  ItiDbUpdateBuffers ("JobItem");
                  ItiDbUpdateBuffers ("JobBreakdown");
                  ItiDbUpdateBuffers ("Job");

                  ItiFreeStrArray (hhpLocChg, ppszPNames, 4);
                  ItiFreeStrArray (hhpLocChg, ppszParams, 4);

                  if (NULL != pszQuantity)
                     ItiMemFree(hhpLocChg, pszQuantity);

                  if (NULL != pszJobBrkdwnKey)
                     ItiMemFree(hhpLocChg, pszJobBrkdwnKey);

                  if (NULL != pszStandardItemKey)
                     ItiMemFree(hhpLocChg, pszStandardItemKey);

                  if (NULL != pszQuantity)
                     ItiMemFree(hhpLocChg, pszQuantity);

                  if (NULL != pszStandardItemKey)
                     ItiMemFree(hhpLocChg, pszStandardItemKey);

                  bDoCrewEqLbRecalc = FALSE;
                  DosBeep (1200, 10);
                  ItiWndSetHourGlass (FALSE);
                  }
               }
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, 0, TRUE);
               if (bDoCrewEqLbRecalc)
                  {
                  DosBeep (1502, 22);
                  DosBeep (1400, 42);
                  DosBeep (1300, 22);
                  }
               break;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }






MRESULT EXPORT EditCrewJobLbEqProc (HWND   hwnd,
                                    USHORT usMessage,
                                    MPARAM mp1,
                                    MPARAM mp2)
   {

   switch (usMessage)
      {
      case WM_INITDLG:
         bDoCrewEqLbRecalc = FALSE;
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_ADD1:
               ItiWndDoNextDialog (hwnd, CrewJobLaborL, CrewJobLaborD, ITI_ADD);
               bDoCrewEqLbRecalc = TRUE;
               break;

            case DID_CHANGE1:
               ItiWndDoNextDialog (hwnd, CrewJobLaborL, CrewJobLaborD, ITI_CHANGE);
               bDoCrewEqLbRecalc = TRUE;
               break;

            case DID_ADD2:
               ItiWndDoNextDialog (hwnd, CrewJobEquipL, CrewJobEquipD, ITI_ADD);
               bDoCrewEqLbRecalc = TRUE;
               break;

            case DID_CHANGE2:
               ItiWndDoNextDialog (hwnd, CrewJobEquipL, CrewJobEquipD, ITI_CHANGE);
               bDoCrewEqLbRecalc = TRUE;
               break;

            case DID_DELETE1:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, CrewJobLaborL), ITI_DELETE);
               bDoCrewEqLbRecalc = TRUE;
               break;

            case DID_DELETE2:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, CrewJobEquipL), ITI_DELETE);
               bDoCrewEqLbRecalc = TRUE;
               break;

            case DID_OK:
               break;

            case DID_CANCEL:   /* CANCEL button pushed */
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;

            default:
               break;
            }
         break;
      }/* end of switch */


   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }




MRESULT EXPORT EditCrewJobLaborProc (HWND   hwnd,
                                     USHORT usMessage,
                                     MPARAM mp1,
                                     MPARAM mp2)
   {
   PSZ    pszJobKey;
   PSZ    pszJobCostSheetKey;
   PSZ    pszJobCrewKey;
   static HHEAP  hheap;

   switch (usMessage)
      {
      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, LaborCatL, cLaborerKey, hwnd,     0);
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP(mp1))
            {
            case DID_OK:
            case DID_ADDMORE:
               {
               HWND hwndList, hwndStatic;
               MRESULT mr;

               hwndList = WinWindowFromID(hwnd, LaborCatL);
               if (hwndList == NULL)
                  break;

               hwndStatic = QW(hwnd, ITIWND_OWNERWND, 2, 0, 0);

               hheap            = (HHEAP) WinSendMsg (hwndStatic, WM_ITIWNDQUERY,
                                  MPFROMSHORT (ITIWND_HEAP), 0);

               pszJobKey        = (PSZ)   WinSendMsg (hwndStatic, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_DATA),
                                   MPFROM2SHORT (0, cJobKey));

               pszJobCostSheetKey = (PSZ)   WinSendMsg (hwndStatic, WM_ITIWNDQUERY,
                                    MPFROMSHORT (ITIWND_DATA),
                                    MPFROM2SHORT (0, cJobCostSheetKey));

               pszJobCrewKey = (PSZ)   WinSendMsg (hwndStatic, WM_ITIWNDQUERY,
                               MPFROMSHORT (ITIWND_DATA),
                               MPFROM2SHORT (0, cJobCrewKey));

               mr = ItiWndDefDlgProc(hwnd, usMessage, mp1, mp2);
//               DoCalcJobCrewLaborRate (hheap, hwndList, pszJobKey, pszJobCostSheetKey, pszJobCrewKey);
               if (hheap != NULL)
                  {
                  if (pszJobKey)
                     ItiMemFree(hheap, pszJobKey);
                  if (pszJobCostSheetKey)
                     ItiMemFree(hheap, pszJobCostSheetKey);
                  if (pszJobCrewKey)
                     ItiMemFree(hheap, pszJobCrewKey);
                  }
               return mr;
               break;
               }


            case DID_CANCEL:   /* CANCEL button pushed */
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;

            default:
               break;
            }
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }




MRESULT EXPORT EditCrewJobEquipProc (HWND   hwnd,
                                     USHORT usMessage,
                                     MPARAM mp1,
                                     MPARAM mp2)
   {
   PSZ   pszJobKey;
   PSZ   pszJobCostSheetKey;
   PSZ   pszJobCrewKey;
   HHEAP hheap;

   switch (usMessage)
      {
      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, EquipCatL, cEquipmentKey, hwnd,     0);
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP(mp1))
            {
            case DID_OK:
            case DID_ADDMORE:
               {
               HWND hwndList, hwndStatic;
               MRESULT mr;

               hwndList = WinWindowFromID(hwnd, EquipCatL);
               if (hwndList == NULL)
                  break;

               hwndStatic = QW(hwnd, ITIWND_OWNERWND, 2, 0, 0);

               hheap            = (HHEAP) WinSendMsg (hwndStatic, WM_ITIWNDQUERY,
                                  MPFROMSHORT (ITIWND_HEAP), 0);              

               pszJobKey        = (PSZ)   WinSendMsg (hwndStatic, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_DATA),
                                   MPFROM2SHORT (0, cJobKey));

               pszJobCostSheetKey = (PSZ)   WinSendMsg (hwndStatic, WM_ITIWNDQUERY,
                                    MPFROMSHORT (ITIWND_DATA),
                                    MPFROM2SHORT (0, cJobCostSheetKey));

               pszJobCrewKey = (PSZ)   WinSendMsg (hwndStatic, WM_ITIWNDQUERY,
                               MPFROMSHORT (ITIWND_DATA),
                               MPFROM2SHORT (0, cJobCrewKey));

               mr = ItiWndDefDlgProc(hwnd, usMessage, mp1, mp2);
//               DoCalcJobCrewEquipRate (hheap, hwndList, pszJobKey, pszJobCostSheetKey, pszJobCrewKey);
               if (pszJobKey)
                  ItiMemFree(hheap, pszJobKey);
               if (pszJobCostSheetKey)
                  ItiMemFree(hheap, pszJobCostSheetKey);
               if (pszJobCrewKey)
                  ItiMemFree(hheap, pszJobCrewKey);
               return mr;
               }

            case DID_CANCEL:   /* CANCEL button pushed */
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;

            default:
               break;
            }
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }







BOOL EXPORT DelCBEForJobItem (HHEAP hheap, PSZ pszJobKey, PSZ pszJobItemKey)
   {
   char szTemp [500] = "";

   sprintf (szTemp,
            "DELETE FROM JobMaterialUsed "
            "WHERE JobKey = %s "
            "AND JobCostSheetKey IN "
             "(SELECT JobCostSheetKey FROM JobCostSheet "
             "WHERE JobKey = %s AND JobItemKey = %s "
             "AND JobItemCostBasedEstimateKey IN "
               "(SELECT JobItemCostBasedEstimateKey"
               " FROM JobCostBasedEstimate"
               " WHERE JobKey = %s"
               " AND JobItemKey = %s ) ) "
            ,pszJobKey,
            pszJobKey,
            pszJobItemKey,
            pszJobKey,
            pszJobItemKey);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      return FALSE;
                  

   sprintf (szTemp,
            "/* cbest.c DelCBEForJobItem */ "
            "DELETE FROM JobCrewLaborUsed "
            "WHERE JobKey = %s "
            "AND JobCostSheetKey IN "
             "(SELECT JobCostSheetKey FROM JobCostSheet "
             "WHERE JobKey = %s AND JobItemKey = %s "
             "AND JobItemCostBasedEstimateKey IN "
               "(SELECT JobItemCostBasedEstimateKey"
               " FROM JobCostBasedEstimate"
               " WHERE JobKey = %s"
               " AND JobItemKey = %s ) ) "
            ,pszJobKey,
            pszJobKey,
            pszJobItemKey,
            pszJobKey,
            pszJobItemKey);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      return FALSE;
                  
   sprintf (szTemp,
            "/* cbest.c DelCBEForJobItem */ "
            "DELETE FROM JobCrewEquipmentUsed "
            "WHERE JobKey = %s "
            "AND JobCostSheetKey IN "
             "(SELECT JobCostSheetKey FROM JobCostSheet "
             "WHERE JobKey = %s AND JobItemKey = %s "
             "AND JobItemCostBasedEstimateKey IN "
               "(SELECT JobItemCostBasedEstimateKey"
               " FROM JobCostBasedEstimate"
               " WHERE JobKey = %s"
               " AND JobItemKey = %s ) ) "
            ,pszJobKey,
            pszJobKey,
            pszJobItemKey,
            pszJobKey,
            pszJobItemKey);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      return FALSE;

                  
   sprintf (szTemp,
            "/* cbest.c DelCBEForJobItem */ "
            "DELETE JobCrewUsed "
            "WHERE JobKey = %s "
            "AND JobCostSheetKey IN "
             "(SELECT JobCostSheetKey FROM JobCostSheet "
             "WHERE JobKey = %s AND JobItemKey = %s "
             "AND JobItemCostBasedEstimateKey IN "
               "(SELECT JobItemCostBasedEstimateKey"
               " FROM JobCostBasedEstimate"
               " WHERE JobKey = %s"
               " AND JobItemKey = %s ) ) "
            ,pszJobKey,
            pszJobKey,
            pszJobItemKey,
            pszJobKey,
            pszJobItemKey);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      return FALSE;

   sprintf (szTemp,
            "/* cbest.c DelCBEForJobItem */ "
            " DELETE FROM JobCostSheet "
            " WHERE JobKey = %s "
            " AND JobItemKey = %s"
            " DELETE FROM JobCostBasedEstimate "
            " WHERE JobKey = %s "
            " AND JobItemKey = %s "
            ,pszJobKey,
            pszJobItemKey,
            pszJobKey,
            pszJobItemKey);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      return FALSE;

   return TRUE;
   }/* End of Function DelCBEForJobItem */




BOOL EXPORT DoDelCostBasedEstForJobItem (HHEAP hheap1,
                                         PSZ pszJobKey,
                                         PSZ pszJobItemKey)
   {
   BOOL  bRet;
//   char   szTemp   [1024];
//    USHORT usCols   [] = {1, 2, 3, 0};
//    USHORT usErr, usNumRows, usNumRows2;
//    PSZ *ppszCBEKeys = NULL;
//    PSZ *ppszJCShtKeys = NULL;

   if ((pszJobKey == NULL) || (pszJobItemKey == NULL))
      return FALSE;

   bRet = DelCBEForJobItem (hheap1, pszJobKey, pszJobItemKey);
   return bRet;

//    /* -- This query obtains all of the CostBasedEstimates */
//    /* -- association with a Job Item.                     */
//    sprintf (szTemp, "/* cbest.DoDelCostBasedEstForJobItem */ "
//                     "SELECT JobKey, JobItemKey, JobItemCostBasedEstimateKey "
//                     "FROM JobCostBasedEstimate "
//                     "WHERE JobKey = %s "
//                     "AND JobItemKey = %s ",
//                     pszJobKey, pszJobItemKey);
// 
//    usErr = ItiDbBuildSelectedKeyArray2
//               (hheap1, 0, szTemp, &ppszCBEKeys, usCols);
//    if (usErr != 0)
//       return FALSE;
// 
//    /* This query obtains all of the CostSheets associated with a JobItem */
//    sprintf (szTemp, "/* cbest.DoDelCostBasedEstForJobItem */ "
//                     "SELECT JobKey, JobCostSheetKey,"
//                           " JobItemCostBasedEstimateKey "
//                     "FROM JobCostSheet "
//                     "WHERE JobKey = %s "
//                     "AND JobItemKey = %s ",
//                     pszJobKey, pszJobItemKey);
// 
//    usErr = ItiDbBuildSelectedKeyArray2
//               (hheap1, 0, szTemp, &ppszJCShtKeys, usCols);
//    if (usErr != 0)
//       return FALSE;
// 
//    for (usNumRows = 0; ppszCBEKeys[usNumRows] != NULL; usNumRows++)
//       ;
//    for (usNumRows2 = 0; ppszJCShtKeys[usNumRows2] != NULL; usNumRows2++)
//       ;
// 
//  //  DelCrewsAndMaterialsForJobItem (hheap1, ppszJCShtKeys);
//    ItiFreeStrArray (hheap1, ppszCBEKeys, usNumRows);
//    ItiFreeStrArray (hheap1, ppszJCShtKeys, usNumRows2);
   } /* End of Function DoDelCostBasedEstForJobItem */






/*
 * This function loops through a stored array of JobCostBasedEstimates that have
 * been selected by the user to be deleted. All of the JobCostSheets associated with a
 * particular CBE are then retrieved and stored in another array.  This array is then
 * passed to another function which deletes all of the Crews, Laborers, Equipment
 * and Materials associated with these JobCostSheets
 */



BOOL EXPORT DoDelJobCostSheetsForCBE (HHEAP hheapKeys, 
                                      PSZ *ppszSelKeys, 
                                      USHORT usCBEst)
   {
   USHORT i;
   PSZ    *ppszJCShtKeys, *ppszPNames, *ppszParams;
   PSZ    pszJobKey, pszJobItemKey, pszActive, pszJICBEKey;
   USHORT usCols [3] = {1, 2, 0};
   USHORT uRet, usErr, usNumRows2;

   ppszPNames = ItiStrMakePPSZ (hheapKeys, 3, "JobKey", "JobItemKey", NULL);

   pszJobKey = ItiDbGetZStr(ppszSelKeys[0],0);
   pszJobItemKey = ItiDbGetZStr(ppszSelKeys[0],1);

   ppszParams = ItiStrMakePPSZ (hheapKeys, 3, pszJobKey, pszJobItemKey, NULL);

   for (i = 0; ppszSelKeys[i] != NULL; i++)
      {
      ppszJCShtKeys = NULL;

      /* if the Job Item's unit price is based on CBE, and the active one    */
      /* is being deleted, then the EstimationMethodKey is the JobItem table */
      /* is changed to ADD HOC.  The unit price is unchanged.                */

      pszActive = ItiDbGetZStr(ppszSelKeys[i],3);

      uRet = ItiStrCmp(pszActive, "Yes");

      if ((usCBEst == 0) && (uRet == 0))
         {
         /* change JobItems's estimation method to ADD HOC */
         UpDateJobItemEstMethod(hheapKeys, ppszPNames, ppszParams);
         }

      pszJobKey     = ItiDbGetZStr(ppszSelKeys[i],0);
      pszJobItemKey = ItiDbGetZStr(ppszSelKeys[i],1);
      pszJICBEKey   = ItiDbGetZStr(ppszSelKeys[i],2);

      /* This query obtains all of the JobCostSheets associated with a CBE */
      ItiStrCpy (szTemp,
                       "SELECT JobKey, JobCostSheetKey "
                       "FROM JobCostSheet "
                       "WHERE JobKey = ", sizeof szTemp);
      ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
      ItiStrCat (szTemp, " AND JobItemKey =  ", sizeof szTemp);
      ItiStrCat (szTemp, pszJobItemKey, sizeof szTemp);
      ItiStrCat (szTemp, " AND JobItemCostBasedEstimateKey = ", sizeof szTemp);
      ItiStrCat (szTemp, pszJICBEKey, sizeof szTemp);

      usErr = ItiDbBuildSelectedKeyArray2(hheapKeys, 0, szTemp, &ppszJCShtKeys, usCols);
      if (usErr != 0)
         return FALSE;

      for (usNumRows2 = 0; ppszJCShtKeys[usNumRows2] != NULL; usNumRows2++)
         ;

      if (*ppszJCShtKeys != NULL)    /* if there are JobCostSheets */
         {
         DelCrewsAndMaterialsForJobItem (hheapKeys, ppszJCShtKeys);
         DelJobCostSheetsForCBE(hheapKeys, ppszJCShtKeys);
         }
      ItiFreeStrArray (hheapKeys, ppszJCShtKeys, (usNumRows2 + 1));
      }

   ItiFreeStrArray (hheapKeys, ppszSelKeys, (i+1));
   ItiFreeStrArray (hheapKeys, ppszPNames, 3);
   ItiFreeStrArray (hheapKeys, ppszParams, 3);

   return TRUE;
   }          /* end of function */



BOOL EXPORT DelCrewsAndMaterialsForJobItem (HHEAP hheap, PSZ *ppszJCShtKeys)
   {
   USHORT   i;
   char     szTemp [400] = "";
   PSZ      pszJobKey, pszJCSKey;

   for (i = 0; ppszJCShtKeys[i] != NULL; i++)
      {
      pszJobKey = ItiDbGetZStr(ppszJCShtKeys[i],0);
      pszJCSKey = ItiDbGetZStr(ppszJCShtKeys[i],1);

      ItiStrCpy (szTemp,
               " DELETE FROM JobMaterialUsed WHERE JobKey = ", sizeof szTemp);
      ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
      ItiStrCat (szTemp,   " AND JobCostSheetKey = ", sizeof szTemp);
      ItiStrCat (szTemp, pszJCSKey, sizeof szTemp);
      ItiStrCat (szTemp,   " DELETE JobCrewUsed WHERE JobKey = ", sizeof szTemp);
      ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
      ItiStrCat (szTemp,   " AND JobCostSheetKey = ", sizeof szTemp);
      ItiStrCat (szTemp, pszJCSKey, sizeof szTemp);
      ItiStrCat (szTemp,   " DELETE FROM JobCrewLaborUsed WHERE JobKey = ", sizeof szTemp);
      ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
      ItiStrCat (szTemp,   " AND JobCostSheetKey = ", sizeof szTemp);
      ItiStrCat (szTemp, pszJCSKey, sizeof szTemp);
      ItiStrCat (szTemp,   " DELETE FROM JobCrewEquipmentUsed WHERE JobKey = ", sizeof szTemp);
      ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
      ItiStrCat (szTemp,   " AND JobCostSheetKey = ", sizeof szTemp);
      ItiStrCat (szTemp, pszJCSKey, sizeof szTemp);

      if (ItiDbExecSQLStatement (hheap, szTemp))
         return FALSE;
      }

   return TRUE;
   }





BOOL EXPORT DelJobCostSheetsForCBE (HHEAP hheap, PSZ *ppszJCShtKeys)
   {
   USHORT   i;
   char     szTemp [100] = "";
   PSZ      pszJCSKey = NULL;

   for (i = 0; ppszJCShtKeys[i] != NULL; i++)
      {
      pszJCSKey = ItiDbGetZStr(ppszJCShtKeys[i],1);

      ItiStrCpy (szTemp,
          " DELETE FROM JobCostSheet WHERE JobCostSheetKey = ", sizeof szTemp);
      ItiStrCat (szTemp, pszJCSKey, sizeof szTemp);

      if (ItiDbExecSQLStatement (hheap, szTemp))
         return FALSE;
      }

   return TRUE;
   }





// static int GetEstimationMethod (HHEAP hheap, 
//                                 PSZ pszJobKey, 
//                                 PSZ pszJobCostSheetKey)
//    {
//    char szTemp [1024];
//    USHORT      uRet;
//    PSZ         pszItemEstMethod;
// 
//    /* Determines what estimation method the JobItem is using */
// 
//    sprintf(szTemp,  "/* cbest.c.GetEstimationMethod */ "
//                     "SELECT ItemEstimationMethodID "
//                     "FROM ItemEstimationMethod IEM, JobItem JI "
//                     "WHERE JCS.JobItemKey = JI.JobItemKey "
//                     "AND JI.ItemEstimationMethodKey "
//                      "= IEM.ItemEstimationMethodKey "
//                     "AND JI.JobKey = %s "
//                     pszJobKey, pszJobKey, pszJobCostSheetKey);
// 
//    pszItemEstMethod = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);
// 
// 
//    uRet = ItiStrCmp(pszItemEstMethod, "CBEst");
//    if (pszItemEstMethod)
//       ItiMemFree(hheap, pszItemEstMethod);
// 
//    if (uRet == 0)
//       return 0;
//    else
//       return 1;
//    }





int EXPORT IsCBEActive (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey)
   {
   char     szTemp [320] = "";
   PSZ      pszActive;
   USHORT   i;

   /* Determines if the CBE is the active one */
   ItiStrCpy (szTemp,
            "SELECT Active "
            "FROM JobCostSheet JCS, JobCostBasedEstimate JCBE "
            "WHERE JCS.JobKey = JCBE.JobKey "
            "AND JCS.JobItemKey = JCBE.JobItemKey "
            "AND JCS.JobItemCostBasedEstimateKey = JCBE.JobItemCostBasedEstimateKey "
            "AND JCS.JobKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
   ItiStrCat (szTemp, " AND JCS.JobCostSheetKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobCostSheetKey, sizeof szTemp);

   pszActive = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);

   i = *pszActive != '1';

   if (pszActive)
      ItiMemFree(hheap, pszActive);

   return i;
   }






int EXPORT IsCBEForJobItemActive (HHEAP hheap, 
                                  PSZ pszJobKey, 
                                  PSZ pszJobItemKey, 
                                  PSZ pszJobItemCostBasedEstimateKey)
   {
   char     szTemp [240] = "";
   PSZ      pszActive;
   USHORT   i = 0;

   /* Determines if the CBE is the active one */

   ItiStrCpy (szTemp,
    " SELECT Active FROM  JobCostBasedEstimate WHERE JobKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
   ItiStrCat (szTemp, " AND JobItemKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobItemKey, sizeof szTemp);
   ItiStrCat (szTemp, " AND JobItemCostBasedEstimateKey = ",  sizeof szTemp);
   ItiStrCat (szTemp, pszJobItemCostBasedEstimateKey, sizeof szTemp);

   pszActive = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);
   i = *pszActive != '1';

   if (pszActive)
      ItiMemFree(hheap, pszActive);

   return i;
   }




int EXPORT DetermineIfSelectedCBEActive (HWND hwnd)
   {
   HHEAP    hheap;
   PSZ      pszJobKey, pszJobItemKey, pszJobItemCostBasedEstimateKey;
   PSZ      pszActive;
   char     szTemp [250] = "";
   USHORT   i;

   hheap            = (HHEAP) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_HEAP), 0);

   pszJobKey        = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_DATA),
                                   MPFROM2SHORT (0, cJobKey));

   pszJobItemKey = (PSZ) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                     MPFROMSHORT (ITIWND_DATA),
                                     MPFROM2SHORT (0, cJobItemKey));

   pszJobItemCostBasedEstimateKey = (PSZ) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                     MPFROMSHORT (ITIWND_DATA),
                                     MPFROM2SHORT (0, cJobItemCostBasedEstimateKey));


   ItiStrCpy (szTemp,
            " SELECT Active "
            "FROM  JobCostBasedEstimate "
            "WHERE JobKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
   ItiStrCat (szTemp, " AND JobItemKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobItemKey, sizeof szTemp);
   ItiStrCat (szTemp, " AND JobItemCostBasedEstimateKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobItemCostBasedEstimateKey, sizeof szTemp);

   pszActive = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);

   i = (*pszActive) != '1';

   if (pszJobKey)
      ItiMemFree(hheap, pszJobKey);
   if (pszJobItemKey)
      ItiMemFree(hheap, pszJobItemKey);
   if (pszJobItemCostBasedEstimateKey)
      ItiMemFree(hheap, pszJobItemCostBasedEstimateKey);
   if (pszActive)
      ItiMemFree(hheap, pszActive);

   return i;
   }


int EXPORT DetermineEstMethod (HWND hwnd)
   {
   HHEAP  hheap;
   PSZ    pszJobKey, pszJobCostSheetKey, pszItemEstMethod;
   char   szTemp [500] = "";
   USHORT uRet;

   hheap            = (HHEAP) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_HEAP), 0);

   pszJobKey        = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_DATA),
                                   MPFROM2SHORT (0, cJobKey));

   pszJobCostSheetKey = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_DATA),
                                   MPFROM2SHORT (0, cJobCostSheetKey));
   ItiStrCpy (szTemp,
            " SELECT ItemEstimationMethodID "
            "FROM ItemEstimationMethod, JobItem, JobCostSheet "
            "WHERE JobCostSheet.JobItemKey = JobItem.JobItemKey "
            "AND JobItem.ItemEstimationMethodKey = ItemEstimationMethod.ItemEstimationMethodKey "
            "AND JobItem.JobKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
   ItiStrCat (szTemp, " AND JobCostSheet.JobKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
   ItiStrCat (szTemp, " AND JobCostSheet.JobCostSheetKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobCostSheetKey, sizeof szTemp);

   pszItemEstMethod = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);

   uRet = ItiStrNCmp (pszItemEstMethod, "CBEst", 4);

   if (pszJobKey)
      ItiMemFree(hheap, pszJobKey);
   if (pszJobCostSheetKey)
      ItiMemFree(hheap, pszJobCostSheetKey);
   if (pszItemEstMethod)
      ItiMemFree(hheap, pszItemEstMethod);

   if (uRet == 0)
      return 0;
   else
      return 1;
   }






int EXPORT DetermineIfCBEActive(HWND hwnd)
   {
   HHEAP    hheap;
   PSZ      pszJobKey, pszJobCostSheetKey, pszActive;
   char     szTemp [400] = "";
   USHORT   i;

   hheap            = (HHEAP) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_HEAP), 0);

   pszJobKey        = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_DATA),
                                   MPFROM2SHORT (0, cJobKey));

   pszJobCostSheetKey = (PSZ) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_DATA),
                                   MPFROM2SHORT (0, cJobCostSheetKey));

   ItiStrCpy (szTemp,
            "SELECT Active "
            "FROM JobCostSheet, JobCostBasedEstimate "
            "WHERE JobCostSheet.JobKey = JobCostBasedEstimate.JobKey "
            "AND JobCostSheet.JobItemKey = JobCostBasedEstimate.JobItemKey "
            "AND JobCostSheet.JobItemCostBasedEstimateKey = JobCostBasedEstimate.JobItemCostBasedEstimateKey "
            "AND JobCostSheet.JobKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
   ItiStrCat (szTemp, " AND JobCostSheet.JobCostSheetKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobCostSheetKey, sizeof szTemp);

   pszActive = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);
   i = *pszActive != '1';

   if (pszJobKey)
      ItiMemFree (hheap, pszJobKey);
   if (pszJobCostSheetKey)
      ItiMemFree (hheap, pszJobCostSheetKey);
   if (pszActive)
      ItiMemFree (hheap, pszActive);

   return i;
   }









int EXPORT GetJobItemKeyAndRecalc (HWND hwnd)
   {
   HHEAP hheap;
   PSZ   pszJobKey, pszJobCostSheetKey, pszJobItemKey;
   char  szTemp [126] = "";

   hheap            = (HHEAP) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_HEAP), 0);

   pszJobKey        = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_DATA),
                                   MPFROM2SHORT (0, cJobKey));

   pszJobCostSheetKey = (PSZ) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_DATA),
                                   MPFROM2SHORT (0, cJobCostSheetKey));


   ItiStrCpy (szTemp,
        "SELECT JobItemKey FROM JobCostSheet WHERE JobKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
   ItiStrCat (szTemp, " AND JobCostSheetKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobCostSheetKey, sizeof szTemp);


   pszJobItemKey = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);

   DoJobItemReCalcCBEST(hheap, pszJobKey, pszJobItemKey);

   if (pszJobKey)
      ItiMemFree(hheap, pszJobKey);
   if (pszJobItemKey)
      ItiMemFree(hheap, pszJobItemKey);
   if (pszJobCostSheetKey)
      ItiMemFree(hheap, pszJobCostSheetKey);

   return 0;
   }









int EXPORT GetJobBreakKeyAndRecalc (HWND hwnd)
   {
   static PSZ   pszJobKey, pszJobCostSheetKey, pszJobItemKey, pszJobBreakdownKey;
   HHEAP hheap;
   char  szTemp [126] = "";

   hheap            = (HHEAP) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_HEAP), 0);


   pszJobKey        = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_DATA),
                                   MPFROM2SHORT (0, cJobKey));

   pszJobCostSheetKey = (PSZ) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_DATA),
                                   MPFROM2SHORT (0, cJobCostSheetKey));


   ItiStrCpy (szTemp,
        "SELECT JobItemKey FROM JobCostSheet WHERE JobKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
   ItiStrCat (szTemp, " AND JobCostSheetKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobCostSheetKey, sizeof szTemp);

   pszJobItemKey = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);


   ItiStrCpy (szTemp,
      " SELECT JobBreakdownKey FROM JobItem WHERE JobKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
   ItiStrCat (szTemp, " AND JobItemKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobItemKey, sizeof szTemp);

   pszJobBreakdownKey = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);

   if (pszJobBreakdownKey != NULL)
      DoJobAndJobBreakReCalcCBEST(hheap, pszJobKey, pszJobBreakdownKey);

   if (pszJobKey)
      ItiMemFree(hheap, pszJobKey);
   if (pszJobItemKey)
      ItiMemFree(hheap, pszJobItemKey);
   if (pszJobBreakdownKey)
      ItiMemFree(hheap, pszJobBreakdownKey);
   if (pszJobCostSheetKey)
      ItiMemFree(hheap, pszJobCostSheetKey);


   return 0;

   }







int EXPORT GetKeysAndRecalc (HWND hwnd)
   {
   HHEAP hheap;
   PSZ   pszJobKey, pszJobItemKey;

   hheap            = (HHEAP) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_HEAP), 0);


   pszJobKey        = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_DATA),
                                   MPFROM2SHORT (0, cJobKey));

   pszJobItemKey = (PSZ) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_DATA),
                                   MPFROM2SHORT (0, cJobItemKey));


   DoJobItemReCalcCBEST(hheap, pszJobKey, pszJobItemKey);

   if (pszJobKey)
      ItiMemFree(hheap, pszJobKey);
   if (pszJobItemKey)
      ItiMemFree(hheap, pszJobItemKey);

   return 0;
   }











int EXPORT DetermineJobItemEstMethod (HWND hwnd)
   {
   HHEAP  hheap;
   PSZ    pszJobKey, pszJobItemKey, pszItemEstMethod;
   char   szTemp [400] = "";
   USHORT uRet;

   hheap            = (HHEAP) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_HEAP), 0);


   pszJobKey        = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_DATA),
                                   MPFROM2SHORT (0, cJobKey));

   pszJobItemKey = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_DATA),
                                   MPFROM2SHORT (0, cJobItemKey));


   ItiStrCpy (szTemp, " SELECT ItemEstimationMethodID "
                    "FROM ItemEstimationMethod, JobItem  "
                    " WHERE JobItem.ItemEstimationMethodKey = ItemEstimationMethod.ItemEstimationMethodKey"
                    " AND JobItem.JobKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
   ItiStrCat (szTemp, " AND JobItem.JobItemKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobItemKey, sizeof szTemp);


   pszItemEstMethod = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);

   ItiMemFree(hheap, pszJobKey);
   ItiMemFree(hheap, pszJobItemKey);

   uRet = ItiStrCmp(pszItemEstMethod, "CBEst");
   ItiMemFree(hheap, pszItemEstMethod);

   if (uRet == 0)
      return 0;
   else
      return 1;
   }





int EXPORT DeactivatePreviouslyActiveCBE (HWND hwnd)
   {
   HHEAP hheapStatic;
   HWND  hwndStatic;
   char  szTemp [300] = "";
   PSZ   pszJobKey, pszJobItemKey;
   PSZ   pszJobItemCostBasedEstimateKey;
   PSZ   *ppszParams, *ppszPNames;

   hwndStatic = QW(hwnd, ITIWND_OWNERWND, 1, 0, 0);

   hheapStatic = WinSendMsg (hwndStatic, WM_ITIWNDQUERY,
                       MPFROMSHORT (ITIWND_HEAP), 0);


   pszJobKey = WinSendMsg (hwndStatic, WM_ITIWNDQUERY,
                                 MPFROMSHORT (ITIWND_DATA),
                                 MPFROM2SHORT (0, cJobKey));

   pszJobItemKey = WinSendMsg (hwndStatic, WM_ITIWNDQUERY,
                                 MPFROMSHORT (ITIWND_DATA),
                                 MPFROM2SHORT (0, cJobItemKey));


   ItiStrCpy (szTemp, " SELECT JobItemCostBasedEstimateKey "
                    " FROM JobCostBasedEstimate "
                    " WHERE  Active = 1 AND JobKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
   ItiStrCat (szTemp, " AND JobItemKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobItemKey, sizeof szTemp);


   pszJobItemCostBasedEstimateKey = ItiDbGetRow1Col1(szTemp, hheapStatic, 0, 0, 0);

   /* if there is an active JCBEst in the database, then Update the table */
   /* to make it inactive.                                                */

   if (pszJobItemCostBasedEstimateKey != NULL)
      {
      ppszParams = ItiStrMakePPSZ(hheapStatic, 4, pszJobKey, pszJobItemKey,
                                  pszJobItemCostBasedEstimateKey, NULL);

      ppszPNames = ItiStrMakePPSZ(hheapStatic, 4, "JobKey", "JobItemKey", "JobItemCostBasedEstimateKey", NULL);

      UpDateJobItemCostBasedEstimate(hheapStatic, ppszPNames, ppszParams);
      ItiFreeStrArray (hheapStatic, ppszPNames, 4);
      ItiFreeStrArray (hheapStatic, ppszParams, 4);
      ItiMemFree(hheapStatic, pszJobItemCostBasedEstimateKey);
      }

   ItiMemFree(hheapStatic, pszJobKey);
   ItiMemFree(hheapStatic, pszJobItemKey);

   return 0;
   }






int EXPORT DetermineIfJCShtsExist (HHEAP hheap, 
                                   PSZ pszJobKey, 
                                   PSZ pszJobItemKey)
   {
   char     szTemp [400] = "";
   PSZ      pszJobCostSheetExists;

   ItiStrCpy (szTemp, "SELECT JobCostSheetKey "
                   "FROM JobCostSheet JCS, JobCostBasedEstimate  JCBE "
                   "WHERE JCBE.JobKey = JCS.JobKey "
                   "AND JCBE.JobItemKey = JCS.JobItemKey "
                   "AND JCBE.JobItemCostBasedEstimateKey = JCS.JobItemCostBasedEstimateKey "
                   "AND JCBE.JobKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
   ItiStrCat (szTemp, " AND JCBE.JobItemKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobItemKey, sizeof szTemp);


   pszJobCostSheetExists = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);

   /* If the active JobCostBasedEstimate has JobCostSheets associated      */
   /* with it, return 0, to indicate that the JobItem can be recalculated  */
   /* by summing up the JobCostSheets.                                     */
   /* If there are no JobCostSheets associated with the active JCBEst      */
   /* return 1, so $.00 can be placed in the JobItem's unit price          */

   if (pszJobCostSheetExists != NULL)
      {
      ItiMemFree (hheap, pszJobCostSheetExists);
      return 0;
      }
   return 1;
   }

// *** NEW CALCS ******

int EXPORT DoChgJobBreakKeyAndRecalc (HWND hwnd)
   {
   static PSZ   pszJobKey, pszJobCostSheetKey, pszJobItemKey, pszJobBreakdownKey;
   HHEAP hheap;
   char  szTemp [200];

   hheap            = (HHEAP) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_HEAP), 0);


   pszJobKey        = (PSZ)   WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_DATA),
                                   MPFROM2SHORT (0, cJobKey));

   pszJobCostSheetKey = (PSZ) WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                   MPFROMSHORT (ITIWND_DATA),
                                   MPFROM2SHORT (0, cJobCostSheetKey));


   ItiStrCpy (szTemp,
          " SELECT JobItemKey FROM JobCostSheet WHERE JobKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
   ItiStrCat (szTemp, " AND JobCostSheetKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobCostSheetKey, sizeof szTemp);
 
   pszJobItemKey = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);


   ItiStrCpy (szTemp,
       " SELECT JobBreakdownKey FROM JobItem WHERE JobKey =  ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
   ItiStrCat (szTemp, " AND JobItemKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobItemKey, sizeof szTemp);

   pszJobBreakdownKey = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);
   if (pszJobBreakdownKey != NULL)
      DoJobAndJobBreakReCalcCBEST(hheap, pszJobKey, pszJobBreakdownKey);

   if (pszJobKey)
      ItiMemFree(hheap, pszJobKey);
   if (pszJobItemKey)
      ItiMemFree(hheap, pszJobItemKey);
   if (pszJobBreakdownKey)
      ItiMemFree(hheap, pszJobBreakdownKey);
   if (pszJobCostSheetKey)
      ItiMemFree(hheap, pszJobCostSheetKey);


   return 0;

   }







BOOL EXPORT ItiDllInitDll (void)
   {
   static BOOL bInited = FALSE;
   PSZ    psz = NULL;

   if (bInited)
      return TRUE;

   bInited = TRUE;
   bDoCrewEqLbRecalc = FALSE;

   usRoundToDecimalPlace = 0;
   DosScanEnv ("ROUNDPLACE", &psz);
   if (NULL != psz)
      {
      if ItiCharIsDigit(psz[0])
         ItiStrToUSHORT(psz, &usRoundToDecimalPlace);
      }


   if (ItiMbRegisterStaticWnd (CostShtCrewS, CostShtCrewProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterStaticWnd (MaterialS, MaterialProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterStaticWnd (CBEstForStdItemS, CBEstForStdItemStaticProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterStaticWnd (CBEstForJobItemS, CBEstForJobItemStaticProc, hmodMe))
      return FALSE; 
   if (ItiMbRegisterStaticWnd (TaskS, TaskProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterStaticWnd (JobTaskS, JobTaskProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterStaticWnd (TaskCostShtS, TaskCostShtStaticProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterStaticWnd (JobCostShtS, JobCostShtStaticProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterStaticWnd (JobCrewS, JobCrewStaticProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterStaticWnd (JobMaterialsS, JobMaterialsStaticProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterStaticWnd (CrewForCostShtS, CrewForCostShtStaticProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterStaticWnd (CrewForJobS, CrewForJobStaticProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterStaticWnd (JobZoneS, JobZoneStaticProc, hmodMe))
      return FALSE;



   if (ItiMbRegisterListWnd (CostShtCrewL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (MaterialL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (CBEstForStdItemL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (CBEstForJobItemL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (TaskL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (TaskCostShtEditDL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (JobTaskL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (JobCrewL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (JobMaterialsL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (MatForCostShtDL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (CrewForCostShtL1, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (CrewForCostShtL2, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (CrewCatForCostShtD, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (CrewCShtLaborL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (CrewCShtEquipL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (CrewForJobL1, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (CrewForJobL2, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (CrewJobLaborL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (CrewJobEquipL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (JobZoneL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;


   if (ItiMbRegisterDialogWnd (CostBasedEstimateD, CostBasedEstimateD, EditCostBasedEstimateProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (JobCostBasedEstD, JobCostBasedEstD, EditJobCostBasedEstProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (TasksForStdItemD, TasksForStdItemD, EditTasksForStdItemProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (ChJobTaskListD, ChJobTaskListD, EditChJobTaskListProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (AddJobTaskListD, AddJobTaskListD, EditAddJobTaskListProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (CostShtForTaskD, CostShtForTaskD, EditCostShtForTaskProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (JobCostSheetD, JobCostSheetD, EditJobCostSheetProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (MatForCostShtD, MatForCostShtD, EditMatForCostShtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (CrewForCostShtD, CrewForCostShtD, EditCrewForCostShtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (CrewCShtLbEqD, CrewCShtLbEqD, EditCrewCShtLbEqProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (CrewCShtLaborD, CrewCShtLaborD, EditCrewCShtLaborProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (CrewCShtEquipD, CrewCShtEquipD, EditCrewCShtEquipProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (JobMaterialsD, JobMaterialsD, EditJobMaterialsProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (CrewForJobD, CrewForJobD, EditCrewForJobProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (CrewJobLbEqD, CrewJobLbEqD, EditCrewJobLbEqProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (CrewJobLaborD, CrewJobLaborD, EditCrewJobLaborProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (CrewJobEquipD, CrewJobEquipD, EditCrewJobEquipProc, hmodMe))
      return FALSE;

   return TRUE;
   }





