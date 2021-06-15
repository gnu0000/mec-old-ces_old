/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1992 by Info Tech, Inc.  All Rights Reserved.         |
|                                                                           |
|       This program module is part of DS/Shell (PC), Info Tech's           |
|       database interfaces for OS/2, a proprietary product of              |
|       Info Tech, Inc., no part of which may be reproduced or              |
|       transmitted in any form or by any means, electronic,                |
|       mechanical, or otherwise, including photocopying and recording      |
|       or in connection with any information storage or retrieval          |
|       system, without permission in writing from Info Tech, Inc.          |
|                                                                           |
+--------------------------------------------------------------------------*/



/*
 * UserCat.c 
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This module provides the User catalog window.
 */



#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\winid.h"
#include "..\include\colid.h"
#include "..\include\itiutil.h"
#include "..\include\itimenu.h"
#include "..\include\dialog.h"
#include "Usercat.h"
#include "initcat.h"
#include "dialog.h"

#include <stdio.h>

static CHAR szDllVersion[] = "1.1a0 UserCat.dll";


VOID EXPORT ItiDllQueryMenuName (PSZ      pszBuffer, 
                                 USHORT   usMaxSize,
                                 PUSHORT  pusWindowID)
   {
   ItiStrCpy (pszBuffer, "~User Catalog", usMaxSize);
   *pusWindowID = UserCatS;
   }


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




MRESULT EXPORT CatalogUserDProc (HWND     hwnd,
                                 USHORT   usMessage,
                                 MPARAM   mp1,
                                 MPARAM   mp2)
   {
   switch (usMessage)
      {
      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, DID_PERMISSION, 0, hwnd, 0);
         break;

      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }



MRESULT EXPORT ApplicationUserDProc (HWND    hwnd,
                                     USHORT  usMessage,
                                     MPARAM  mp1,
                                     MPARAM  mp2)
   {
   switch (usMessage)
      {
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }


static void CreateChildRelations (HHEAP hheap, PSZ pszKey)
   {
   char szQuery [250] = "";

   sprintf (szQuery,
            "INSERT INTO UserCatalog "
            "(UserKey,CatalogKey,Permission) "
            "SELECT %s,CatalogKey,10 "
            "FROM Catalog ",
            pszKey);
   ItiDbExecSQLStatement (hheap, szQuery);

   sprintf (szQuery, 
            "INSERT INTO UserApplication "
            "(UserKey,ApplicationKey) "
            "SELECT %s,ApplicationKey "
            "FROM Application ",
            pszKey);
   ItiDbExecSQLStatement (hheap, szQuery);
   }



MRESULT EXPORT UserDProc (HWND   hwnd,
                          USHORT usMessage,
                          MPARAM mp1,
                          MPARAM mp2)
   {
   static PSZ pszKey;

   switch (usMessage)
      {
      case WM_NEWKEY:
         {
         HHEAP hheap;

         if (ItiStrICmp ((PSZ) mp1, "User") == 0)
            {
            hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
            pszKey = ItiStrDup (hheap, (PSZ) mp2);
            }

         return 0;
         }
         break;

      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, DID_USERTYPE, 0, hwnd, 0);
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_ADDMORE:
            case DID_OK:
               {
               MRESULT mr;
               HHEAP   hheap;

               pszKey = NULL;
               if (mr = ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2))
                  return mr;

               if (pszKey != NULL)
                  {
                  hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
                  CreateChildRelations (hheap, pszKey);
                  ItiMemFree (hheap, pszKey);
                  }
               return mr;
               }
               break;
            }
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }





MRESULT EXPORT UserCatSProc (HWND     hwnd, 
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

               bSelected = ItiWndQueryActive (hwnd, UserCatL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               return 0;
               break;
               }

            case IDM_VIEW_MENU:
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2), 
                                           IDM_VIEW_USER_FIRST);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "~Catalog Permissions", 
                                      IDM_VIEW_USER_FIRST + 1);
            }

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, UserCatL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, UserCatL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, UserCatL), ITI_DELETE);
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               ItiWndBuildWindow (WinWindowFromID (hwnd, UserCatL), CatalogUserS);
               return 0;
            }   
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }


MRESULT EXPORT CatalogUserSProc (HWND     hwnd, 
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

               bSelected = ItiWndQueryActive (hwnd, CatalogUserL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, TRUE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, TRUE);
               return 0;
               break;
               }
            }

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, CatalogUserL, FALSE);
               break;
            }   
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }


MRESULT EXPORT ApplicationUserSProc (HWND     hwnd, 
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

               bSelected = ItiWndQueryActive (hwnd, ApplicationUserL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               return 0;
               break;
               }
            }

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, ApplicationUserL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, ApplicationUserL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, ApplicationUserL), ITI_DELETE);
               break;

            }   
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }






BOOL EXPORT ItiDllInitDll (void)
   {
   if (ItiMbRegisterStaticWnd (UserCatS, UserCatSProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterStaticWnd (CatalogUserS, CatalogUserSProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterStaticWnd (ApplicationUserS, ApplicationUserSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (UserCatL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (CatalogUserL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (ApplicationUserL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (ApplicationUserDL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (UserD, UserD, UserDProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (CatalogUserD, CatalogUserD, CatalogUserDProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (ApplicationUserD, ApplicationUserD, ApplicationUserDProc, hmodMe))
      return FALSE;


   return TRUE;
   }



