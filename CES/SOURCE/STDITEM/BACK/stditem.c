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
 * StdItem.c
 * Mark Hampton
 *
 * This module provides the Standard Item catalog window.
 */



#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimbase.h"
#include "..\include\itibase.h"
#include "..\include\winid.h"
#include "..\include\colid.h"
#include "..\include\itiutil.h"
#include "..\include\itimenu.h"
#include "..\include\itiimp.h"
#include "..\include\itiest.h"
#include "..\include\itidbase.h"
#include "stditem.h"
#include "initcat.h"
#include "dialog.h"

#include <stdio.h>

/*
 * ItiDllQueryVersion returns the version number of this DLL.
 */

USHORT EXPORT ItiDllQueryVersion (void)
   {
   return VERSION;
   }





/*
 * ItiDllQueryCompatibility returns TRUE if this DLL is compatable
 * with the caller's version numer, otherwise FALSE is returned.
 */

BOOL EXPORT ItiDllQueryCompatibility (USHORT usCallerVersion)

   {
   if (usCallerVersion <= VERSION)
      return TRUE;
   return FALSE;
   }



VOID EXPORT ItiDllQueryMenuName (PSZ      pszBuffer,
                                 USHORT   usMaxSize,
                                 PUSHORT  pusWindowID)
   {
   ItiStrCpy (pszBuffer, "~Standard Item Catalog", usMaxSize);
   *pusWindowID = StandardItemCatS;
   }


MRESULT EXPORT StanItemDProc (HWND    hwnd,
                                          USHORT   usMessage,
                                          MPARAM   mp1,
                                          MPARAM   mp2)
   {
   switch (usMessage)
      {
      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, DID_UNIT,      cCodeValueKey,      hwnd, cUnit);
         ItiWndActivateRow (hwnd, DID_PRECISION, cRoundingPrecision, hwnd, 0);
         break;

      case WM_COMMAND:
         {
         switch (SHORT1FROMMP (mp1))
            {
            case DID_ADDEP:
               ItiWndDoNextDialog (hwnd, 0, StdItemAddPredD, ITI_ADD);
               break;

            case DID_DEDEP:
               ItiWndDoNextDialog (hwnd, 0, StdItemDelPredD, ITI_DELETE);
               break;
            }
         }
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }




MRESULT EXPORT StdItemAddPredDProc (HWND     hwnd,
                                         USHORT   usMessage,
                                         MPARAM   mp1,
                                         MPARAM   mp2)
   {
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }



MRESULT EXPORT StdItemDelPredDProc (HWND     hwnd,
                                         USHORT   usMessage,
                                         MPARAM   mp1,
                                         MPARAM   mp2)
   {
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }





MRESULT EXPORT StandardItemCatSProc (HWND     hwnd,
                                          USHORT   usMessage,
                                          MPARAM   mp1,
                                          MPARAM   mp2)
   {
   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               {
               BOOL bSelected;

               bSelected = ItiWndQueryActive (hwnd, StandardItemCatL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               return 0;
               }
               break;

            case IDM_VIEW_MENU:
               {
               HHEAP hheap;
               HWND  hwndL;
               PSZ   pszSIK;
               USHORT usActive;

               hwndL = WinWindowFromID (hwnd, StandardItemCatL);
               hheap = QW (hwndL, ITIWND_HEAP, 0, 0, 0);
               usActive = (UM) QW (hwndL, ITIWND_ACTIVE, 0, 0, 0);
               pszSIK = QW (hwndL, ITIWND_DATA, 0, usActive, cStandardItemKey);

               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2),
                                           IDM_VIEW_USER_FIRST);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Standard Item",
                                      IDM_VIEW_USER_FIRST + 1);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Cost Based Estimates",
                                      IDM_VIEW_USER_FIRST + 2);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Default Unit Price",
                                      IDM_VIEW_USER_FIRST + 3);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_VIEW_USER_FIRST + 3,
                  !ItiEstQueryMethod (hheap, pszSIK, NULL, "Default"));

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "PEMETH ~Average",
                                      IDM_VIEW_USER_FIRST + 4);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_VIEW_USER_FIRST + 4,
                  !ItiEstQueryMethod (hheap, pszSIK, NULL, "Average"));

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "PEMETH ~Regression",
                                      IDM_VIEW_USER_FIRST + 5);

               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_VIEW_USER_FIRST + 5,
                  !ItiEstQueryMethod (hheap, pszSIK, NULL, "Regress"));
               ItiMemFree (hheap, pszSIK);
               }
               break;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, StandardItemCatL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, StandardItemCatL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, StandardItemCatL), ITI_DELETE);
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, StandardItemCatL);
               ItiWndBuildWindow (hwndChild, StandardItemS);
               return 0;

            case (IDM_VIEW_USER_FIRST + 2):
               hwndChild = WinWindowFromID (hwnd, StandardItemCatL);
               ItiWndBuildWindow (hwndChild, CBEstForStdItemS);
               return 0;

            case (IDM_VIEW_USER_FIRST + 3):
               hwndChild = WinWindowFromID (hwnd, StandardItemCatL);
               ItiWndBuildWindow (hwndChild, DefaultUnitPriceS);
               return 0;

            case (IDM_VIEW_USER_FIRST + 4):
               hwndChild = WinWindowFromID (hwnd, StandardItemCatL);
               ItiWndBuildWindow (hwndChild, PemethAvgS);
               return 0;

            case (IDM_VIEW_USER_FIRST + 5):
               hwndChild = WinWindowFromID (hwnd, StandardItemCatL);
               ItiWndBuildWindow (hwndChild, PemethRegS);
               return 0;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }




BOOL EXPORT ItiDllInitDll (void)
   {
   if (ItiMbRegisterStaticWnd (StandardItemCatS, StandardItemCatSProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterStaticWnd (StandardItemS, ItiWndDefStaticWndProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (StandardItemL1, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (StandardItemL2, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (StandardItemL3, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (StandardItemCatL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
//   if (ItiMbRegisterListWnd (PredLinkL, ItiWndDefListWndTxtProc, hmodMe))
//      return FALSE;
//   if (ItiMbRegisterListWnd (SuccLinkL, ItiWndDefListWndTxtProc, hmodMe))
//      return FALSE;

   if (ItiMbRegisterDialogWnd (StanItemD, StanItemD, StanItemDProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (StdItemAddPredD, StdItemAddPredD, StdItemAddPredDProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (StdItemDelPredD, StdItemDelPredD, StdItemDelPredDProc, hmodMe))
      return FALSE;

   return TRUE;
   }








USHORT EXPORT ItiImpStdItemUnit (HHEAP       hheap,
                                 PIMPORTINFO pii,
                                 PCOLINFO    pci,
                                 PSZ         pszProdDatabase)
   {
   char  szQuery [1024];
   char  szTemp [1024];
   PSZ   apszTokens [6], apszValues [6];
   HQRY  hqry;
   PSZ   *ppsz;
   USHORT usNumCols, usState;
   BOOL  bDone;

   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszProdDatabase;
   apszTokens [1] = "TableName";
   apszValues [1] = pii->pti->pszTableName;
   apszTokens [2] = "ColumnName";
   apszValues [2] = pci->pszColumnName;
   apszTokens [3] = "KeyValue";
   apszValues [3] = NULL;
   apszTokens [4] = "CodeTableName";
   apszValues [4] = "UNITS";
   apszTokens [5] = "TargetColumn";
   apszValues [5] = "Unit";

   ItiMbQueryQueryText (0, ITIRID_IMPORT, ITIIMP_GETDISTINCTFKS,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery,
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usNumCols, &usState);
   if (hqry == NULL)
      return pii->usError = -2;

   ItiMbQueryQueryText (0, ITIRID_IMPORT, ITIIMP_UPDATECODEVALUE,
                        szTemp, sizeof szTemp);
   while (!(bDone = !ItiDbGetQueryRow (hqry, &ppsz, &usState)) &&
          pii->usError == 0)
      {
      apszValues [3] = *ppsz;
      ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery,
                           apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
      pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }

   if (!bDone)
      ItiDbTerminateQuery (hqry);

   return pii->usError;
   }





USHORT EXPORT ItiImpCopy (HHEAP       hheap,
                          PIMPORTINFO pii,
                          PSZ         pszProdDatabase)
   {
   char  szQuery [1024];
   char  szTemp [1024];
   PSZ   apszTokens [1], apszValues [1];

   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszProdDatabase;

   ItiMbQueryQueryText (hmodMe, ITIRID_IMPORT, COPYSTANDARDITEM,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery,
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);

   return pii->usError;
   }


