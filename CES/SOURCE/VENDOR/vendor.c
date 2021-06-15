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
 * Vendor.c
 * Mark Hampton
 *
 * This module provides the Vendor catalog window.
 */



#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimbase.h"
#include "..\include\winid.h"
#include "..\include\colid.h"
#include "..\include\itibase.h"
#include "..\include\itiutil.h"
#include "..\include\itimenu.h"
#include "..\include\dialog.h"
#include "vendor.h"
#include "initcat.h"
#include "dialog.h"


static CHAR szDllVersion[] = "1.1a0 Vendor.dll";

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
   ItiStrCpy (pszBuffer, "~Vendor Catalog", usMaxSize);
   *pusWindowID = VendorCatS;
   }




MRESULT EXPORT VendorCatSProc (HWND     hwnd,
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

               bSelected = ItiWndQueryActive (hwnd, VendorCatL, NULL);
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
                                      "~Vendor",
                                      IDM_VIEW_USER_FIRST + 1);
               break;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, VendorCatL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, VendorCatL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, VendorCatL), ITI_DELETE);
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, VendorCatL);
               ItiWndBuildWindow (hwndChild, VendorS);
               return 0;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }




MRESULT EXPORT VendorSProc (HWND     hwnd,
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
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, TRUE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, TRUE);
               return 0;
               break;

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
            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, 0, FALSE);
               break;

              /* No add for the vendor window */
//            case IDM_ADD:
//               ItiWndDoDialog (hwnd, 0, TRUE);
//               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (hwnd, ITI_DELETE);
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               ItiWndBuildWindow (hwnd, FacilityForVendorS);
               return 0;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }









MRESULT EXPORT VenEditDProc (HWND    hwnd,
                                     USHORT  usMessage,
                                     MPARAM  mp1,
                                     MPARAM  mp2)
   {
   switch (usMessage)
      {
      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, DID_TYPE,          cCodeValueID, NULL, ITIMB_RELCOL +5);
         ItiWndActivateRow (hwnd, DID_CERTIFICATION, cCodeValueID, NULL, ITIMB_RELCOL +7);
         ItiWndActivateRow (hwnd, DID_DBECLASSIF,    cCodeValueID, NULL, ITIMB_RELCOL +9);
         break;

      case WM_COMMAND:
         {
         switch (SHORT1FROMMP (mp1))
            {
            case DID_ADD1:
               ItiWndDoNextDialog (hwnd, VenEditL1, VendorAddressD, ITI_ADD);
               break;

            case DID_CHANGE1:
               ItiWndDoNextDialog (hwnd, VenEditL1, VendorAddressD, ITI_CHANGE);
               break;

            case DID_ADD2:
               ItiWndDoNextDialog (hwnd, VenEditL2, WorkClassD, ITI_ADD);
               break;

            case DID_CHANGE2:
               ItiWndDoNextDialog (hwnd, VenEditL2, WorkClassD, ITI_CHANGE);
               break;

            case DID_DELETE1:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, VenEditL1), ITI_DELETE);
               break;

            case DID_DELETE2:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, VenEditL2), ITI_DELETE);
               break;
            }
         break;
         }

      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }






MRESULT EXPORT VendorDProc (HWND    hwnd,
                                     USHORT  usMessage,
                                     MPARAM  mp1,
                                     MPARAM  mp2)
   {
   switch (usMessage)
      {
      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, DID_TYPE,          cCodeValueID, NULL, ITIMB_RELCOL +3);
         ItiWndActivateRow (hwnd, DID_CERTIFICATION, cCodeValueID, NULL, ITIMB_RELCOL +4);
         ItiWndActivateRow (hwnd, DID_DBECLASSIF,    cCodeValueID, NULL, ITIMB_RELCOL +5);
         break;

      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }





MRESULT EXPORT VendorAddressDProc (HWND   hwnd,
                                            USHORT usMessage,
                                            MPARAM mp1,
                                            MPARAM mp2)
   {

   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }




MRESULT EXPORT WorkClassDProc (HWND   hwnd,
                                        USHORT usMessage,
                                        MPARAM mp1,
                                        MPARAM mp2)
   {
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }






BOOL EXPORT ItiDllInitDll (void)
   {
   if (ItiMbRegisterStaticWnd (VendorCatS, VendorCatSProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterStaticWnd (VendorS, VendorSProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (VendorL1, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (VendorL2, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (VendorCatL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (VenEditL1, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (VenEditL2, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (WorkClassL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (VendorD, VendorD, VendorDProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (VenEditD, VenEditD, VenEditDProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (VendorAddressD, VendorAddressD, VendorAddressDProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (WorkClassD, WorkClassD, WorkClassDProc, hmodMe))
      return FALSE;
   return TRUE;
   }


