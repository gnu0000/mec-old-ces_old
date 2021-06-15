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
 * Labor.c
 * Mark Hampton
 *
 * This module provides the Labor catalog.
 */



#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimbase.h"
#include "..\include\winid.h"
#include "..\include\colid.h"
#include "..\include\itiutil.h"
#include "..\include\itimenu.h"
#include "..\include\dialog.h"
#include "Labor.h"
#include "initcat.h"
#include "dialog.h"
#include <stdio.h>
#include "..\include\itidbase.h"
#include "..\include\itibase.h"

static CHAR szDllVersion[] = "1.1a0 Labor.dll";

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
   ItiStrCpy (pszBuffer, "~Labor Catalog", usMaxSize);
   *pusWindowID = LaborCatS;
   }



MRESULT EXPORT LaborCatSProc (HWND     hwnd,
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

               bSelected = ItiWndQueryActive (hwnd, LaborCatL, NULL);
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
                                      "~Laborer",
                                      IDM_VIEW_USER_FIRST + 1);
               break;
            }
            break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, LaborCatL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, LaborCatL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, LaborCatL), ITI_DELETE);
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, LaborCatL);
               ItiWndBuildWindow (hwndChild, LaborerS);
               return 0;
            }
         break;
      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, DID_LABZONEID, cZoneKey, NULL, cZoneKey);
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }









MRESULT EXPORT LaborerSProc (HWND     hwnd,
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

               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, FALSE);
               return 0;
               break;
               }
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, LaborerL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, LaborerL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, LaborerL), ITI_DELETE);
               break;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }






static BOOL CreateChildRelations (HHEAP hheap,
                                  PSZ   pszLaborerKey)
   {
   char szTemp [1024];

   sprintf (szTemp, "INSERT INTO LaborerWageAndBenefits "
                    "(LaborerKey, ZoneKey, BaseDate, "
                    "DavisBaconRate, DavisBaconOvertimeRate, "
                    "NonDavisBaconRate, NonDavisBaconOvertimeRate) "
                    "VALUES "
                    "(%s,1,'%s',0,0,0,0)",
                    pszLaborerKey, UNDECLARED_BASE_DATE);

   return 0 == ItiDbExecSQLStatement (hheap, szTemp);
   }




MRESULT EXPORT LaborDProc (HWND    hwnd,
                              USHORT   usMessage,
                              MPARAM   mp1,
                              MPARAM   mp2)
   {
   static PSZ pszLaborerKey;

   switch (usMessage)
      {
      case WM_NEWKEY:
         {
         HHEAP hheap;

         if (ItiStrICmp ((PSZ) mp1, "Labor") == 0)
            {
            hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
            pszLaborerKey = ItiStrDup (hheap, (PSZ) mp2);
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

               pszLaborerKey = NULL;
               mr = ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
               if (pszLaborerKey != NULL)
                  {
                  hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
                  CreateChildRelations (hheap, pszLaborerKey);
                  ItiMemFree (hheap, pszLaborerKey);
                  }
               return mr;
               }
               break;
            }
         break;

      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, DID_LABORZONETYPE, cZoneTypeKey, hwnd, 0);
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }




MRESULT EXPORT LabRateDProc (HWND   hwnd,
                                USHORT usMessage,
                                MPARAM mp1,
                                MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, DID_LABZONEID, cZoneKey, NULL, cZoneKey);
         break;
      }

   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }







BOOL EXPORT ItiDllInitDll (void)
   {
   if (ItiMbRegisterStaticWnd (LaborCatS, LaborCatSProc, hmodMe))
      return FALSE;


   if (ItiMbRegisterListWnd (LaborCatL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterStaticWnd (LaborerS, LaborerSProc, hmodMe))
      return FALSE;


   if (ItiMbRegisterListWnd (LaborerL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (LaborD, LaborD, LaborDProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (LabRateD, LabRateD, LabRateDProc, hmodMe))
      return FALSE;

   return TRUE;
   }




