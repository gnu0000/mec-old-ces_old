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
 * Crew.c
 * Mark Hampton
 *
 * This module provides the Crew catalog.
 */



#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimbase.h"
#include "..\include\colid.h"
#include "..\include\winid.h"
#include "..\include\itiutil.h"
#include "..\include\itimenu.h"
#include "..\include\dialog.h"
#include "Crew.h"
#include "initcat.h"
#include "dialog.h"


static CHAR szDllVersion[] = "1.1a0 crew.dll";


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
   ItiStrCpy (pszBuffer, "~Crew Catalog", usMaxSize);
   *pusWindowID = CrewCatS;
   }



MRESULT EXPORT CrewDProc (HWND   hwnd,
                             USHORT usMessage,
                             MPARAM mp1,
                             MPARAM mp2)
   {
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }



MRESULT EXPORT CrewCatSProc (HWND     hwnd,
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

               bSelected = ItiWndQueryActive (hwnd, CrewCatL, NULL);
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
                                      "~Crew",
                                      IDM_VIEW_USER_FIRST + 1);
               break;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, CrewCatL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, CrewCatL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, CrewCatL), ITI_DELETE);
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, CrewCatL);
               ItiWndBuildWindow (hwndChild, CrewS);
               return 0;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }




MRESULT EXPORT CrewSProc (HWND     hwnd,
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
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, TRUE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, TRUE);
               return 0;
               break;
               }
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





MRESULT EXPORT CrewLbEqDProc (HWND   hwnd,
                                 USHORT usMessage,
                                 MPARAM mp1,
                                 MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_COMMAND:
         {
         switch (SHORT1FROMMP (mp1))
            {
            case DID_ADD1:
               ItiWndDoNextDialog (hwnd, CrewLaborL, CrewLaborD, ITI_ADD);
               break;

            case DID_CHANGE1:
               ItiWndDoNextDialog (hwnd, CrewLaborL, CrewLaborD, ITI_CHANGE);
               break;

            case DID_ADD2:
               ItiWndDoNextDialog (hwnd, CrewEquipL, CrewEquipD, ITI_ADD);
               break;

            case DID_CHANGE2:
               ItiWndDoNextDialog (hwnd, CrewEquipL, CrewEquipD, ITI_CHANGE);
               break;

            case DID_DELETE1:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, CrewLaborL), ITI_DELETE);
               break;

            case DID_DELETE2:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, CrewEquipL), ITI_DELETE);
               break;

            }
         }
         break;
      }

   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }







MRESULT EXPORT CrewLaborDProc (HWND   hwnd,
                               USHORT usMessage,
                               MPARAM mp1,
                               MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_QUERYDONE:
         {
         //Select params    hwnd, uCtl,         uCtlCol,     matchwnd, umatchCol
         //=====================================================================
         ItiWndActivateRow (hwnd, LaborCat2L, cLaborerKey, hwnd,     0);
         break;
         }
      }

   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }



MRESULT EXPORT CrewEquipDProc (HWND   hwnd,
                                        USHORT usMessage,
                                        MPARAM mp1,
                                        MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_QUERYDONE:
         {
         //Select params    hwnd, uCtl,         uCtlCol,       matchwnd, umatchCol
         //=======================================================================
         ItiWndActivateRow (hwnd, EquipCatL, cEquipmentKey, hwnd,     0);
         break;
         }
      }

   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }




BOOL EXPORT ItiDllInitDll (void)
   {
   if (ItiMbRegisterStaticWnd (CrewCatS, CrewCatSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterStaticWnd (CrewS, CrewSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (CrewCatL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (CrewL1, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (CrewL2, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (CrewLaborL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (CrewEquipL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (EquipCatL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (LaborCat2L, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (CrewD, CrewD, CrewDProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (CrewLbEqD, CrewLbEqD, CrewLbEqDProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (CrewLaborD, CrewLaborD, CrewLaborDProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (CrewEquipD, CrewEquipD, CrewEquipDProc, hmodMe))
      return FALSE;

   return TRUE;
   }



