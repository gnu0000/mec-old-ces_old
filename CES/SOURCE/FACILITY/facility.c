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
 * Facility.c
 * Mark Hampton
 *
 * This module provides the Facility catalog.
 */



#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimbase.h"
#include "..\include\winid.h"
#include "..\include\itiutil.h"
#include "..\include\itimenu.h"
#include "Facility.h"
#include "initcat.h"
#include "dialog.h"

static CHAR szDllVersion[] = "1.1a0 Facility.dll";


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
   ItiStrCpy (pszBuffer, "~Facility Catalog", usMaxSize);
   *pusWindowID = FacilityCatS;
   }




MRESULT EXPORT FacilityDProc (HWND     hwnd,
                                 USHORT   usMessage,
                                 MPARAM   mp1,
                                 MPARAM   mp2)
   {
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }



MRESULT EXPORT FacilityCatSProc (HWND     hwnd,
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

               bSelected = ItiWndQueryActive (hwnd, FacilityCatL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               return 0;
               }

            case IDM_VIEW_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_PARENT, TRUE);
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2),
                                           IDM_VIEW_USER_FIRST);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "Vendor ~Facility List",
                                      IDM_VIEW_USER_FIRST + 1);


               return 0;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, FacilityCatL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, FacilityCatL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, FacilityCatL), ITI_DELETE);
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, FacilityCatL);
               ItiWndBuildWindow (hwndChild, FacilityForVendorS);
               return 0;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }





MRESULT EXPORT FacilitySProc (HWND     hwnd,
                                      USHORT   usMessage,
                                      MPARAM   mp1,
                                      MPARAM   mp2)

   {
   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_VIEW_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_PARENT, TRUE);
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2),
                                           IDM_VIEW_USER_FIRST);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Facilities for Material",
                                      IDM_VIEW_USER_FIRST + 1);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Material Catalog",
                                      IDM_VIEW_USER_FIRST + 2);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Vendor",
                                      IDM_VIEW_USER_FIRST + 3);

               return 0;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, FacilityL);
               ItiWndBuildWindow (hwndChild, FacilityForMaterialS);
               return 0;

            case (IDM_VIEW_USER_FIRST + 2):
               ItiWndBuildWindow (hwnd, MaterialCatS);
               return 0;

            case (IDM_VIEW_USER_FIRST + 3):
               ItiWndBuildWindow (hwnd, VendorS);
               return 0;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }











MRESULT EXPORT FacilityForVendorSProc (HWND     hwnd,
                                            USHORT   usMessage,
                                            MPARAM   mp1,
                                            MPARAM   mp2)
   {
   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_VIEW_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_PARENT, TRUE);
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2),
                                           IDM_VIEW_USER_FIRST);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Facility",
                                      IDM_VIEW_USER_FIRST + 1);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Vendor",
                                      IDM_VIEW_USER_FIRST + 2);

               return 0;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, FacilityForVendorL);

               ItiWndBuildWindow (hwndChild, FacilityS);
               return 0;

            case (IDM_VIEW_USER_FIRST + 2):
               ItiWndBuildWindow (hwnd, VendorS);
               return 0;
            }
         break;
     }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }




MRESULT EXPORT FacilityForMaterialSProc (HWND     hwnd,
                                            USHORT   usMessage,
                                            MPARAM   mp1,
                                            MPARAM   mp2)
   {
   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_VIEW_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_PARENT, TRUE);
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2),
                                           IDM_VIEW_USER_FIRST);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Facility",
                                      IDM_VIEW_USER_FIRST + 1);
               return 0;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, FacilityForMaterialL);

               ItiWndBuildWindow (hwndChild, FacilityS);
               return 0;
            }
         break;

     }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }




BOOL EXPORT ItiDllInitDll (void)
   {
   if (ItiMbRegisterStaticWnd (FacilityCatS, FacilityCatSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (FacilityCatL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterStaticWnd (FacilityForVendorS, FacilityForVendorSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (FacilityForVendorL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterStaticWnd (FacilityForMaterialS, FacilityForMaterialSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (FacilityForMaterialL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterStaticWnd (FacilityS, FacilitySProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (FacilityL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (FacilityD, FacilityD, FacilityDProc, hmodMe))
      return FALSE;


   return TRUE;
   }


