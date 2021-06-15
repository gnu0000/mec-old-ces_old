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
 * InflPred.c 
 * Mark Hampton
 *
 * This module provides the Inflation Prediction catalog.
 */



#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itibase.h"
#include "..\include\itimbase.h"
#include "..\include\itimenu.h"
#include "..\include\colid.h"
#include "..\include\winid.h"
#include "..\include\itiutil.h"
#include "InflPred.h"
#include "initcat.h"
#include "dialog.h"

static CHAR szDllVersion[] = "1.1a0 inflpred.dll";


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
   ItiStrCpy (pszBuffer, "~Inflation Prediction Catalog", usMaxSize);
   *pusWindowID = InflationPredictionCatS;
   }



MRESULT EXPORT InflationPredictionDProc (HWND    hwnd,
                                 USHORT   usMessage,
                                 MPARAM   mp1,
                                 MPARAM   mp2)
   {
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }


MRESULT EXPORT InflationPredictionCatSProc (HWND    hwnd,
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
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, FALSE);
               return 0;
               break;
            }

            case IDM_VIEW_MENU:
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2),
                                           IDM_VIEW_USER_FIRST);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Change Dates",
                                      IDM_VIEW_USER_FIRST + 1);
               break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, InflationPredictionCatL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, InflationPredictionCatL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, InflationPredictionCatL), ITI_DELETE);
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, InflationPredictionCatL);
               ItiWndBuildWindow (hwndChild, InflationPredChangeS);
               return 0;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }

MRESULT EXPORT InflationPredChangeSProc (HWND     hwnd,
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

               bSelected = ItiWndQueryActive (hwnd, InflationPredChangeL, NULL);

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
               ItiWndDoDialog (hwnd, InflationPredChangeL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, InflationPredChangeL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, InflationPredChangeL), ITI_DELETE);
               break;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }



MRESULT EXPORT InflationPredChangeDProc (HWND   hwnd,
                                         USHORT usMessage,
                                         MPARAM mp1,
                                         MPARAM mp2)
   {
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }

MRESULT EXPORT InflationPredChangeAProc (HWND   hwnd,
                                         USHORT usMessage,
                                         MPARAM mp1,
                                         MPARAM mp2)
   {
   HWND    hwndStatic;
   HHEAP   hheap;
   PSZ     pszInflPredKey;

   switch (usMessage)
      {
      case WM_QUERYDONE:
         hwndStatic = QW(hwnd, ITIWND_OWNERWND, 1, 0, 0);
         hheap      = (HHEAP) WinSendMsg (hwndStatic, WM_ITIWNDQUERY,
                                          MPFROMSHORT (ITIWND_HEAP), 0);
         pszInflPredKey = (PSZ) WinSendMsg (hwndStatic, WM_ITIWNDQUERY,
                                            MPFROMSHORT (ITIWND_DATA),
                                            MPFROM2SHORT (0, cInflationPredictionKey));
         WinSetDlgItemText (hwnd, DID_INFLATIONPREDKEY, pszInflPredKey);
         ItiMemFree(hheap, pszInflPredKey);
         break;
      }
         

   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }


BOOL EXPORT ItiDllInitDll (void)
   {
   if (ItiMbRegisterStaticWnd (InflationPredictionCatS, InflationPredictionCatSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterStaticWnd (InflationPredictionS, ItiWndDefStaticWndProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (InflationPredictionL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (InflationPredictionCatL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (InflationPredictionD, InflationPredictionD, InflationPredictionDProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterStaticWnd (InflationPredChangeS, InflationPredChangeSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (InflationPredChangeL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (InflationPredChangeD, InflationPredChangeD, InflationPredChangeDProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (InflationPredChangeA, InflationPredChangeA, InflationPredChangeAProc, hmodMe))
      return FALSE;

   return TRUE;
   }





