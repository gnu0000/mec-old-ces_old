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
 * Applicat.c 
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This module provides the Application catalog window.
 */



#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimbase.h"
#include "..\include\itimenu.h"
#include "..\include\winid.h"
#include "..\include\itiutil.h"
#include "applicat.h"
#include "initcat.h"


static CHAR szDllVersion[] = "1.1a0 Applicat.DLL";

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
   ItiStrCpy (pszBuffer, "~Application Catalog", usMaxSize);
   *pusWindowID = ApplicationS;
   }



MRESULT EXPORT ApplicationSProc (HWND     hwnd, 
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
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, TRUE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, TRUE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, TRUE);
               return 0;
               break;

            case IDM_VIEW_MENU:
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2), 
                                           IDM_VIEW_USER_FIRST);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "~Application Permissions", 
                                      IDM_VIEW_USER_FIRST + 1);
               break;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case (IDM_VIEW_USER_FIRST + 1):
               ItiWndBuildWindow (WinWindowFromID (hwnd, ApplicationL), ApplicationUserS);
               return 0;
            }   
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, uMsg, mp1, mp2);
   }


BOOL EXPORT ItiDllInitDll (void)
   {
   if (ItiMbRegisterStaticWnd (ApplicationS, ApplicationSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (ApplicationL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   return TRUE;
   }

