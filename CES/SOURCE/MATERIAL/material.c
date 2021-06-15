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
 * Material.c
 * Mark Hampton
 *
 * This module provides the Material catalog.
 */


#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimbase.h"
#include "..\include\winid.h"
#include "..\include\colid.h"
#include "..\include\itiutil.h"
#include "..\include\itiglob.h"
#include "..\include\itimenu.h"
#include "..\include\dialog.h"
#include "Material.h"
#include "initcat.h"
#include "dialog.h"
#include <stdio.h>
#include "..\include\itidbase.h"
#include "..\include\itibase.h"

static CHAR szDllVersion[] = "1.1a0 Material.dll";

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
   ItiStrCpy (pszBuffer, "~Material Catalog", usMaxSize);
   *pusWindowID = MaterialCatS;
   }



MRESULT EXPORT MaterialCatSProc (HWND     hwnd,
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

               bSelected = ItiWndQueryActive (hwnd, MaterialCatL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               }
               return 0;
               break;

            case IDM_VIEW_MENU:
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2),
                                           IDM_VIEW_USER_FIRST);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Material",
                                      IDM_VIEW_USER_FIRST + 1);
               break;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, MaterialCatL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, MaterialCatL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, MaterialCatL), ITI_DELETE);
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, MaterialCatL);
               ItiWndBuildWindow (hwndChild, MaterialPriceS);
               return 0;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }



MRESULT EXPORT MaterialPriceSProc (HWND     hwnd,
                                   USHORT   usMessage,
                                   MPARAM   mp1,
                                   MPARAM   mp2)
   {
   static PSZ   pszUnitType;
   static HWND  hwndList;
   static SHORT uRow;
   PGLOBALS pglob;

   switch (usMessage)
      {
      case WM_INITDLG:
         pszUnitType = NULL;
         break;

      case WM_QUERYDONE:
         break;

      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               {
               /* -- The editing of MaterialPrices is contingent  */
               /* -- upon matching material unit system to the    */
               /* -- active global unit system.                   */
               // PSZ pszUnitType;
               CHAR szUS[4] = "";

               pglob = ItiGlobQueryGlobalsPointer();

               if (pglob->bUseMetric)
                  szUS[0] = '1';
               else
                  szUS[0] = '0';

         /* -- Get the UnitType value. */
               hwndList = WinWindowFromID (hwnd, MaterialPriceL);
               uRow = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);
               pszUnitType = (PSZ) QW (hwndList, ITIWND_DATA, 0, uRow, cUnitType);

               if ( (pszUnitType == NULL)
                   || ((pszUnitType != NULL)
                        && ((CHAR)(*pszUnitType) == (CHAR)szUS[0])) )
                  {
                  ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, FALSE);
                  ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
                  ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, FALSE);
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

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, MaterialPriceL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, MaterialPriceL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, MaterialPriceL), ITI_DELETE);
               break;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }



MRESULT EXPORT MatPricesDProc (HWND   hwnd,
                                      USHORT usMessage,
                                      MPARAM mp1,
                                      MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, DID_MATZONEID, cZoneKey, NULL, cZoneKey);
         break;

/* csp 20oct94 BSR 940040 */
      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               {
               MRESULT mr;

               mr = ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
               ItiDbUpdateBuffers ("CostSheet");
               ItiDbUpdateBuffers ("JobCostSheet");
               return mr;
               }
               break;
            }
         break;
/* csp 20oct94 BSR 940040 */
      }

   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }








static BOOL CreateChildRelations (HHEAP hheap,
                                  PSZ   pszMaterialKey)
   {
   char szTemp [1024];
   CHAR szUS[4] = "";
   CHAR szNotUS[4] = "";
   PGLOBALS pglob;


   pglob = ItiGlobQueryGlobalsPointer();

   if (pglob->bUseMetric)
      {
      szUS[0] = '1';
      szNotUS[0] = '0';
      }
   else
      {
      szUS[0] = '0';
      szNotUS[0] = '1';
      }

   sprintf (szTemp, "/* add the active measurement system default zone. */ "
                    "IF NOT EXISTS (SELECT MaterialKey "
                         " FROM MaterialPrice "
                         " WHERE MaterialKey = %s "
                         " AND ZoneKey = 1 "
                         " AND UnitType = %s ) "
                    " INSERT INTO MaterialPrice "
                    " (MaterialKey, ZoneKey, BaseDate, UnitPrice, Deleted, UnitType) "
                    " VALUES "
                    " (%s,1,'%s',0, NULL, %s)",
                    pszMaterialKey, szUS, pszMaterialKey, UNDECLARED_BASE_DATE, szUS);

   ItiDbExecSQLStatement (hheap, szTemp);

   sprintf (szTemp, "/* add other the system default zone. */ "
                    "IF NOT EXISTS (SELECT MaterialKey "
                         " FROM MaterialPrice "
                         " WHERE MaterialKey = %s "
                         " AND ZoneKey = 1 "
                         " AND UnitType = %s ) "
                    " INSERT INTO MaterialPrice "
                    " (MaterialKey, ZoneKey, BaseDate, UnitPrice, Deleted, UnitType) "
                    " VALUES "
                    " (%s,1,'%s',0, NULL, %s)",
                    pszMaterialKey, szNotUS, pszMaterialKey, UNDECLARED_BASE_DATE, szNotUS);

   return 0 == ItiDbExecSQLStatement (hheap, szTemp);
   }


MRESULT EXPORT MaterialDProc (HWND    hwnd,
                                 USHORT   usMessage,
                                 MPARAM   mp1,
                                 MPARAM   mp2)
   {
   static PSZ pszMaterialKey;

   switch (usMessage)
      {
      case WM_NEWKEY:
         {
         HHEAP hheap;

         if (ItiStrICmp ((PSZ) mp1, "Material") == 0)
            {
            hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
            pszMaterialKey = ItiStrDup (hheap, (PSZ) mp2);
            }

         return 0;
         }
         break;


      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_ADDMORE:
            case DID_OK:
               {
               MRESULT mr;
               HHEAP   hheap;

               pszMaterialKey = NULL;
               mr = ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
               if (pszMaterialKey != NULL)
                  {
                  hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
                  CreateChildRelations (hheap, pszMaterialKey);
                  ItiMemFree (hheap, pszMaterialKey);
                  }
               return mr;
               }
               break;
            }
         break;

      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, DID_MATERIALUNIT ,    cCodeValueKey, hwnd,   cUnit);
         ItiWndActivateRow (hwnd, DID_MATERIALZONETYPE, cZoneTypeKey,  hwnd,   0);
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }





BOOL EXPORT ItiDllInitDll (void)
   {

   if (ItiMbRegisterStaticWnd (MaterialCatS, MaterialCatSProc, hmodMe))
      return FALSE;


   if (ItiMbRegisterListWnd (MaterialCatL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterStaticWnd (MaterialPriceS, MaterialPriceSProc, hmodMe))
      return FALSE;


   if (ItiMbRegisterListWnd (MaterialPriceL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (MaterialD, MaterialD, MaterialDProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (MatPricesD, MatPricesD, MatPricesDProc, hmodMe))
      return FALSE;


   return TRUE;
   }
