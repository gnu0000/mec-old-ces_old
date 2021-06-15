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
 * Equip.c
 * Mark Hampton
 *
 * This module provides the Equip catalog.
 */



#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimbase.h"
#include "..\include\winid.h"
#include "..\include\colid.h"
#include "..\include\itiutil.h"
#include "..\include\itimenu.h"
#include "..\include\dialog.h"
#include "Equip.h"
#include "initcat.h"
#include "dialog.h"
#include <stdio.h>
#include "..\include\itidbase.h"
#include "..\include\itibase.h"

static CHAR szDllVersion[] = "1.1a0 equip.dll";

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



VOID EXPORT ItiDllQueryMenuName (PSZ     pszBuffer,
                                 USHORT  usMaxSize,
                                 PUSHORT pusWindowID)
   {
   ItiStrCpy (pszBuffer, "~Equipment Catalog", usMaxSize);
   *pusWindowID = EquipmentCatS;
   }

MRESULT EXPORT EquipmentCatSProc (HWND     hwnd,
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

               bSelected = ItiWndQueryActive (hwnd, EquipmentCatL, NULL);
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
                                      "~Equipment",
                                      IDM_VIEW_USER_FIRST + 1);
               break;
            }
            break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, EquipmentCatL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, EquipmentCatL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, EquipmentCatL), ITI_DELETE);
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, EquipmentCatL);
               ItiWndBuildWindow (hwndChild, EquipmentS);
               return 0;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }






MRESULT EXPORT EquipmentSProc (HWND     hwnd,
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

               bSelected = ItiWndQueryActive (hwnd, EquipmentL, NULL);
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
            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, EquipmentL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, EquipmentL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, EquipmentL), ITI_DELETE);
               break;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }






static BOOL CreateChildRelations (HHEAP hheap,
                                  PSZ   pszEquipmentKey)
   {
   char szTemp [1024];

   sprintf (szTemp, "INSERT INTO EquipmentRate "
                    "(EquipmentKey, ZoneKey, BaseDate, Rate, OvertimeRate) "
                    "VALUES "
                    "(%s,1,'%s',0,0)",
                    pszEquipmentKey, UNDECLARED_BASE_DATE);

   return 0 == ItiDbExecSQLStatement (hheap, szTemp);
   }




MRESULT EXPORT EquipmentDProc   (HWND   hwnd,
                                    USHORT usMessage,
                                    MPARAM mp1,
                                    MPARAM mp2)
   {
   static PSZ pszEquipmentKey;

   switch (usMessage)
      {
      case WM_NEWKEY:
         {
         HHEAP hheap;

         if (ItiStrICmp ((PSZ) mp1, "Equipment") == 0)
            {
            hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
            pszEquipmentKey = ItiStrDup (hheap, (PSZ) mp2);
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

               pszEquipmentKey = NULL;
               mr = ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
               if (pszEquipmentKey != NULL)
                  {
                  hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
                  CreateChildRelations (hheap, pszEquipmentKey);
                  ItiMemFree (hheap, pszEquipmentKey);
                  }
               return mr;
               }
               break;
            }
         break;

      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, DID_EQUIPZONETYPE, cZoneTypeKey, hwnd, 0);
         return 0;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }





MRESULT EXPORT EquipRateDProc (HWND   hwnd,
                                  USHORT usMessage,
                                  MPARAM mp1,
                                  MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, DID_EQUIPZONEID, cZoneKey, NULL, cZoneKey);
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }






BOOL EXPORT ItiDllInitDll (void)
   {
   if (ItiMbRegisterStaticWnd (EquipmentCatS, EquipmentCatSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (EquipmentCatL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterStaticWnd (EquipmentS, EquipmentSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (EquipmentL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (EquipmentD, EquipmentD, EquipmentDProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (EquipRateD, EquipRateD, EquipRateDProc, hmodMe))
      return FALSE;


   return TRUE;
   }



