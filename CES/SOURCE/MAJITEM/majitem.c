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
 * MajItem.c 
 * Mark Hampton
 *
 * This module provides the Major Item catalog.
 */

#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itibase.h"
#include "..\include\itimbase.h"
#include "..\include\itidbase.h"
#include "..\include\winid.h"
#include "..\include\colid.h"
#include "..\include\itiutil.h"
#include "..\include\itimenu.h"
#include "..\include\itifmt.h"
#include "..\include\dialog.h"
// #include "..\include\itiglob.h"
#include "MajItem.h"
#include "initcat.h"
#include "dialog.h"
#include <stdio.h>

static CHAR szDllVersion[] = "1.1a0 MajItem.dll";

static DOUBLE dPctTotal = 0.0, dMaxPctAllowed = 0.0;


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

BOOL EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion)

   {
   if ( ItiVerCmp(szDllVersion) )
      return TRUE;
   return FALSE;
   }




VOID EXPORT ItiDllQueryMenuName (PSZ      pszBuffer, 
                                 USHORT   usMaxSize,
                                 PUSHORT  pusWindowID)
   {
   ItiStrCpy (pszBuffer, "Ma~jor Item Catalog", usMaxSize);
   *pusWindowID = MajorItemCatS;
   }




static void GetPercentageTotal (HWND hwnd)
   {
   HHEAP    hheap;
   PSZ      *ppsz;
   PSZ      pszMajorItemKey = NULL;
   PSZ      pszPctOfCost;
   CHAR     szQuery [1024] = "", szPctStr [64];
   USHORT   usNumCols = 0, usReturn, usRow;
   DOUBLE   dPctOfCost;
   HWND     hwndList;

   hheap = WinSendMsg (hwnd, WM_ITIWNDQUERY, MPFROMSHORT (ITIWND_HEAP), 0);
   pszMajorItemKey = WinSendMsg (hwnd, WM_ITIWNDQUERY,
                           MPFROMSHORT (ITIWND_DATA),
                           MPFROM2SHORT (0, cMajorItemKey));

   if (   (pszMajorItemKey == NULL) || ((*pszMajorItemKey)== '0') )
      {
      sprintf (szQuery, "/* majitem.c.GetPercentageTotal (has null key) */  SELECT 0");
      }
   else      
      {
      sprintf (szQuery, "/* majitem.c GetPercentageTotal */ "
               "SELECT SUM(PercentofCost) "
               "FROM MajorItem MI, StandardItemMajorItem SIMI, StandardItem "
               "WHERE MI.MajorItemKey = SIMI.MajorItemKey "
               "AND SIMI.StandardItemKey = StandardItem.StandardItemKey "
               "AND MI.MajorItemKey = %s "
               "AND MI.MajorItemKey > 5 "
               , pszMajorItemKey);
      }

   ppsz = ItiDbGetRow1 (szQuery, hheap, hmodMe, 0, MajorItemCatS,
                           &usNumCols);
   if (ppsz != NULL && ItiCharIsDigit (*ppsz [0]))
      {
      usReturn = ItiFmtCheckString (ppsz [0], szPctStr, sizeof szPctStr,
                                    "Number,....", NULL);
         
      if (!usReturn)
         dPctTotal = ItiStrToDouble (ppsz [0]);
      else
         dPctTotal = 1.0;

      dMaxPctAllowed = dPctOfCost = 0.0;
   
      if (ItiWndGetDlgMode () == ITI_ADD)
         dMaxPctAllowed = (1.0 - dPctTotal) * 100.0;
      else
         {
         hwndList = WinWindowFromID (hwnd, MajorItemL);
         usRow = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);

         pszPctOfCost = (PSZ) QW (hwndList, ITIWND_DATA,
                                  0, usRow, cPercentofCost);

         if (pszPctOfCost != NULL)
            {
            usReturn = ItiFmtCheckString (pszPctOfCost,
                                          szPctStr, sizeof szPctStr,
                                          "Number,....", NULL);
         
            if (!usReturn)
               dPctOfCost = (ItiStrToDouble (pszPctOfCost)) / 100.0;

            dMaxPctAllowed = (1.0 - dPctTotal + dPctOfCost) * 100.0;
            }
         else
            dMaxPctAllowed = 0.0;
         }

      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }
   else
      {
      dMaxPctAllowed = 100.0;
      }

   if (pszMajorItemKey)
      ItiMemFree (hheap, pszMajorItemKey);

   } /*  End of Function GetPercentageTotal */




MRESULT EXPORT MajorItemCatDProc (HWND    hwnd,
                                  USHORT  uMsg,
                                  MPARAM  mp1,
                                  MPARAM  mp2)
   {
   switch (uMsg)
      {
      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, DID_UNIT, cCodeValueKey, hwnd, cCommonUnit);
         ItiWndActivateRow (hwnd, DID_SPECYEAR, cSpecYear, hwnd, cSpecYear);
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
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
   return ItiWndDefDlgProc (hwnd, uMsg, mp1, mp2);
   }



MRESULT EXPORT MajorItemDProc (HWND    hwnd,
                               USHORT  uMsg,
                               MPARAM  mp1,
                               MPARAM  mp2)
   {
   HWND  hwndList;

   switch (uMsg)
      {
      case WM_ITIDBBUFFERDONE:
         hwndList = (HWND) QW (hwnd, ITIWND_OWNERWND, 1, 0, 0);
        // GetPercentageTotal (hwndList);
         break;

      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, StandardItemCatL, cStandardItemKey,  hwnd, 0);

         ItiWndActivateRow (hwnd, DID_CONV, cQuantityConversionFactor, hwnd, 0);
         ItiWndActivateRow (hwnd, DID_PCT,  cPercentofCost,            hwnd, 0);
      //   ItiWndActivateRow (hwnd, DID_SPECYEAR,  cSpecYear,          hwnd, cSpecYear);

         hwndList = (HWND) QW (hwnd, ITIWND_OWNERWND, 1, 0, 0);
        // GetPercentageTotal (hwndList);

         if (ItiWndGetDlgMode () == ITI_CHANGE)
            {
            WinEnableWindow (WinWindowFromID (hwnd, StandardItemCatL), FALSE);
            WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, DID_CONV));
            return 0;
            }

         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            char   szPctStr [64], szPctOfCost [64];
            DOUBLE dPctEntered = 0.0;
            USHORT usReturn;

            case DID_OK:
            case DID_ADDMORE:
               {
            //   hwndList = (HWND) QW (hwnd, ITIWND_OWNERWND, 1, 0, 0);
            //   WinQueryDlgItemText (hwnd, DID_PCT,
            //                              sizeof szPctOfCost, szPctOfCost);
            //
            //   usReturn = ItiFmtCheckString (szPctOfCost,
            //                                 szPctStr, sizeof szPctStr,
            //                                 "Number,....", NULL);
            //
            //   if (!usReturn)
            //      dPctEntered = ItiStrToDouble (szPctOfCost);
            //
            //   if (dPctEntered > dMaxPctAllowed)
            //      {
            //      sprintf (szPctStr, "The percentage entered will exceed "
            //                         " 100 percent for this Major Item.");
            //
            //      WinMessageBox (HWND_DESKTOP, hwnd, szPctStr,  
            //                     "Major Item", 666,
            //                     MB_MOVEABLE | MB_OK | 
            //                     MB_APPLMODAL | MB_ICONHAND);
            //      WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, DID_PCT));
            //      return 0;
            //      }
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
   return ItiWndDefDlgProc (hwnd, uMsg, mp1, mp2);
   }




MRESULT EXPORT MajorItemCatSProc (HWND     hwnd, 
                                  USHORT   uMsg,
                                  MPARAM   mp1,
                                  MPARAM   mp2)
   {
   switch (uMsg)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               {
               BOOL bSelected;

               bSelected = ItiWndQueryActive (hwnd, MajorItemCatL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               return 0;
               }

            case IDM_VIEW_MENU:
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2), IDM_VIEW_USER_FIRST);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), "~Major Item", IDM_VIEW_USER_FIRST + 1);

               // 97 FEB // ItiMenuInsertMenuItem (HWNDFROMMP (mp2), "Major Item ~Price Regression", IDM_VIEW_USER_FIRST + 2);
               // 97 FEB // ItiMenuInsertMenuItem (HWNDFROMMP (mp2), "Major Item ~Quantity Regression", IDM_VIEW_USER_FIRST + 3);

//  ItiMenuGrayMenuItem   (HWNDFROMMP (mp2), IDM_VIEW_USER_FIRST + 3, TRUE);
// JUN 96
               break;
            }


      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, MajorItemCatL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, MajorItemCatL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, MajorItemCatL), ITI_DELETE);
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, MajorItemCatL);
               ItiWndBuildWindow (hwndChild, MajorItemS);
               return 0;

            case (IDM_VIEW_USER_FIRST + 2):
               hwndChild = WinWindowFromID (hwnd, MajorItemCatL);
               ItiWndBuildWindow (hwndChild, MajItemPriceRegrS);
               return 0;

            case (IDM_VIEW_USER_FIRST + 3):
               hwndChild = WinWindowFromID (hwnd, MajorItemCatL);
               ItiWndBuildWindow (hwndChild, MajItemQuantityRegrS);
               return 0;
            }   
      }
   return ItiWndDefStaticWndProc (hwnd, uMsg, mp1, mp2);
   }





MRESULT EXPORT MajorItemSProc (HWND     hwnd, 
                               USHORT   uMsg,
                               MPARAM   mp1,
                               MPARAM   mp2)
   {
   switch (uMsg)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               {
               BOOL bSelected;

               bSelected = ItiWndQueryActive (hwnd, MajorItemL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               return 0;
               }
            }


      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, MajorItemL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, MajorItemL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, MajorItemL), ITI_DELETE);
               break;
            }


      }
   return ItiWndDefStaticWndProc (hwnd, uMsg, mp1, mp2);
   }







BOOL EXPORT ItiDllInitDll (void)
   {
//   HHEAP  hhp;
//   PGLOBALS pglobs;
//   CHAR   szChk[775] = "";
//   CHAR   szChkPat[750] =
//            " /* majitem.c.ItiDllInitDll */ "
//            " IF not exists (select MajorItemKey "
//                           " from MajorItem "
//                           " WHERE MajorItemKey = 1 ) "
//  " INSERT INTO MajorItem"
//  " (MajorItemKey,MajorItemID,Description,CommonUnit"
//    ", UnitType, SpecYear )"
//  " select 1, 'Def', 'Default', 4, NULL , %s ";
// //   " select 1,'Def','Default',4,NULL,SpecYear from CurrentSpecYear ";
 

//   hhp = ItiMemCreateHeap (1024);
//   pglobs = ItiGlobQueryGlobalsPointer ();
//   sprintf (szChk, szChkPat, pglobs->pszCurrentSpecYear );
//   ItiDbExecSQLStatement (hhp, szChk);  /* check defaults */
//
//   ItiMemDestroyHeap (hhp);


   if (ItiMbRegisterStaticWnd (MajorItemCatS, MajorItemCatSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterStaticWnd (MajorItemS,    MajorItemSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (MajorItemCatL,   ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (MajorItemL,      ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (MajorItemCatD, MajorItemCatD,  MajorItemCatDProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (MajorItemD,    MajorItemD,     MajorItemDProc,    hmodMe))
      return FALSE;

   return TRUE;
   }




