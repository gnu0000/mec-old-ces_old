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
 * County.c
 * Mark Hampton
 *
 * This module provides the County catalog.
 */



#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimbase.h"
#include "..\include\winid.h"
#include "..\include\itiutil.h"
#include "..\include\dialog.h"
#include "..\include\itimenu.h"
// #include "..\include\itiimp.h"
#include "County.h"
#include "initcat.h"
#include "dialog.h"

static CHAR szDllVersion[] = "1.1a0 county.dll";


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
   ItiStrCpy (pszBuffer, "Cou~nty Catalog", usMaxSize);
   *pusWindowID = CountyCatS;
   }






MRESULT EXPORT CountyDProc (HWND    hwnd,
                               USHORT  usMessage,
                               MPARAM  mp1,
                               MPARAM  mp2)
   {
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }





MRESULT EXPORT CountyCatSProc (HWND     hwnd,
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

               bSelected = ItiWndQueryActive (hwnd, CountyCatL, NULL);
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
               ItiWndDoDialog (hwnd, CountyCatL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, CountyCatL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, CountyCatL), ITI_DELETE);
               break;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }






BOOL EXPORT ItiDllInitDll (void)
   {
   if (ItiMbRegisterStaticWnd (CountyCatS, CountyCatSProc, hmodMe))
      return FALSE;


   if (ItiMbRegisterListWnd (CountyCatL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (CountyD, CountyD, CountyDProc, hmodMe))
      return FALSE;


   return TRUE;
   }




// 
// USHORT EXPORT ItiImpCopyCounty (HHEAP       hheap,
//                           PIMPORTINFO pii,
//                           PSZ         pszProdDatabase)
//    {
//    char  szQuery [1024];
//    char  szTemp [1024];
//    PSZ   apszTokens [1], apszValues [1];
// 
//    apszTokens [0] = "ProductionDatabase";
//    apszValues [0] = pszProdDatabase;
// 
//    ItiMbQueryQueryText (hmodMe, ITIRID_IMPORT, COPYCOUNTY,
//                         szTemp, sizeof szTemp);
//    ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery,
//                         apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
//    pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
// 
//    return pii->usError;
//    }
// 
