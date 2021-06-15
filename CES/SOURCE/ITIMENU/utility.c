
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
 * utility.c
 * Copyright (C) 1992 Info Tech, Inc.
 *
 * This module provides support for the "generic" commands on the 
 * utilities menu.
 */

#define INCL_WIN
#define INCL_DOSMISC
#include <stdlib.h> 
#include <string.h>
#include <stdio.h>
#include "..\include\iti.h"
#include "..\include\itiglob.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\itibase.h"
#include "..\include\itimbase.h"
#include "..\include\itidbase.h"
#include "..\include\itiutil.h"
#include "..\include\colid.h"
#include "..\include\winid.h"
#include "..\include\itierror.h"
#include "..\include\dialog.h"
#include "..\include\itiperm.h"
#include "window.h"
#include "dialog.h"
#include "init.h"
#include "utility.h"


void EXPORT ItiMenuChangePassword (HWND hwnd)
   {
   ItiPermChangePassword (hwnd);
   }



void EXPORT ItiMenuDenyLogon (HWND hwnd)
   {
   static BOOL bDenied = FALSE;
   HWND hwndMenu;
   char szQuery [510] = "";

   hwndMenu = WinWindowFromID (pglobals->hwndAppFrame, FID_MENU);
   if (hwndMenu == NULL)
      return;

   if (bDenied)
      {
      if (MBID_YES != WinMessageBox (HWND_DESKTOP, hwnd, 
                                    "Are you sure you want to enable "
                                    "user logons?", "Enable Logon", 0, 
                                    MB_YESNO | MB_MOVEABLE | MB_ICONQUESTION))

         return;

      bDenied = FALSE;
      hwndMenu = WinSendMsg (hwndMenu, 
                             MM_SETITEMTEXT, 
                             MPFROM2SHORT (IDM_DENY_LOGON, TRUE),
                             "Deny Logon...");
      sprintf (szQuery,
               "DELETE FROM ApplicationDisabled "
               "WHERE ApplicationKey = "
               "(SELECT ApplicationKey "
               "FROM Application "
               "WHERE ApplicationID = '%s') ",
               pglobals->pszApplicationID);
      ItiDbExecSQLStatement (hheapGlobal, szQuery);
      }
   else
      {
      if (MBID_YES != WinMessageBox (HWND_DESKTOP, hwnd, 
                                    "Are you sure you want to lock out "
                                    "all users?", "Deny Logon",  0, 
                                    MB_YESNO | MB_MOVEABLE | MB_ICONQUESTION))

         return;

      bDenied = TRUE;
      hwndMenu = WinSendMsg (hwndMenu, 
                             MM_SETITEMTEXT, 
                             MPFROM2SHORT (IDM_DENY_LOGON, TRUE),
                             "Enable Logon...");

      sprintf (szQuery,
               "INSERT INTO ApplicationDisabled "
               "(UserKey, ApplicationKey, Time) "
               "(SELECT DISTINCT UserKey, ApplicationKey,"
               " '%.2d/%.2d/%.4d %.2d:%.2d:%.2d' "
               "FROM Application, User "
               "WHERE ApplicationID = '%s' "
               " AND UserKey >= 1000000 "
               " AND UserKey != %s )"
               ,(USHORT) pglobals->pgis->month,
               (USHORT) pglobals->pgis->day, 
               (USHORT) pglobals->pgis->year,
               (USHORT) pglobals->pgis->hour,
               (USHORT) pglobals->pgis->minutes, 
               (USHORT) pglobals->pgis->seconds,
               pglobals->pszApplicationID,
               pglobals->pszUserKey);

      ItiDbExecSQLStatement (hheapGlobal, szQuery);
      }
   }



void EXPORT ItiMenuInstall (HWND hwnd)
   {
   HMODULE     hmodDll;
   PDLLWNDFN   pfn;

   hmodDll = ItiDllLoadDll ("A:\\itiinst.dll", "1");
   if (hmodDll == 0)
      {
      WinMessageBox (HWND_DESKTOP, hwnd, 
                     "The disk in drive A: is not an Info Tech Install disk.",
                     "Install", 0, 
                     MB_OK | MB_MOVEABLE | MB_ICONHAND);
      return;
      }
   
   pfn = ItiDllQueryProcAddress (hmodDll, "ITIINSTALL");
   if (pfn != NULL)
      pfn (hwnd);
   else
      {
      WinMessageBox (HWND_DESKTOP, hwnd, 
                     "The disk in drive A: is not an Info Tech Install disk.",
                     "Install", 0, 
                     MB_OK | MB_MOVEABLE | MB_ICONHAND);
      }

   ItiDllFreeDll (hmodDll);
   }



void EXPORT ItiMenuSetPreferences (HWND hwnd)
   {
   }




MRESULT EXPORT SendMessageDProc (HWND    hwnd, 
                                 USHORT  usMessage,
                                 MPARAM  mp1,
                                 MPARAM  mp2)
   {
   switch (usMessage)
      {
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }



void EXPORT ItiMenuSendMessage (HWND hwnd)
   {
   ItiWndSetDlgMode (ITI_ADD);
   ItiWndBuildWindow (hwnd, SendMessageD);
   }


MRESULT EXPORT ShowUsersDProc (HWND    hwnd, 
                               USHORT  usMessage,
                               MPARAM  mp1,
                               MPARAM  mp2)
   {

   switch (usMessage)
      {
      case WM_TIMER:
         ItiDbUpdateBuffers("sysprocesses");
         break;
      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HHEAP hheap;
            case DID_OK:
               hheap = QW (hwnd, ITIWND_HEAP, 0, 0, 0);
               WinDestroyWindow (hwnd);
               ItiMemDestroyHeap (hheap);
               return 0;
            }
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }


void EXPORT ItiMenuShowUsers (HWND hwnd)
   {
   HWND hwndUsers;

   ItiWndSetDlgMode (ITI_OTHER);
   hwndUsers = ItiWndBuildWindow (hwnd, ShowUsersD);
   WinStartTimer (WinQueryAnchorBlock (hwnd), hwndUsers, 0, 5000);
   }


