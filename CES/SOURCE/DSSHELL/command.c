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
 * Command.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 */

#define INCL_WIN


#include "..\include\iti.h"
#include "..\include\winid.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\message.h"
#include "..\include\itibase.h"
#include "..\include\itiperm.h"
#include "DSShell.h"
#include "menu.h"
#include "command.h"
#include "about.h"
#include "..\include\dialog.h"
#include <stdio.h>
#include <process.h>


/*
 * returns TRUE if message processed, FALSE if not.
 */


BOOL GreyMenuItems (HWND hwndMenu, USHORT usMenuID)
   {
   HWND  hwndActive;
   BOOL  bOK;
   HWND  hwndSubmenu;

   hwndActive = WinSendMsg (WinQueryWindow (pglobals->hwndAppClient,
                                            QW_PARENT, FALSE),
                            WM_QUERYACTIVEDOC, 0, 0);
   if (hwndActive)
      hwndActive = WinWindowFromID (hwndActive, FID_CLIENT);
   
   switch (usMenuID)
      {
      case IDM_FILE_MENU:
         hwndSubmenu = ItiMenuQuerySubmenu (hwndMenu, IDM_OPEN_CATALOG_MENU);
         ItiMenuInitOpenCatalogMenuPopup (hwndSubmenu);
         hwndSubmenu = ItiMenuQuerySubmenu (hwndMenu, IDM_PRINT_MENU);
         ItiMenuInitPrintMenuPopup (hwndSubmenu);
         ItiMenuGrayMenuItem (hwndMenu, IDM_CLOSE, hwndActive == NULL);

//         ItiMenuGrayMenuItem (hwndMenu, IDM_PRINTER_SETUP, TRUE);
         hwndSubmenu = ItiMenuQuerySubmenu (hwndMenu, IDM_PRINTER_SETUP);

         ItiMenuGrayMenuItem (hwndMenu, IDM_EXPORT,        FALSE);
         ItiMenuGrayMenuItem (hwndMenu, IDM_IMPORT,        FALSE);
         return 0 == WinSendMsg (hwndActive, WM_INITMENU, 
                                 MPFROMSHORT (usMenuID),
                                 MPFROMHWND (hwndMenu));
         break;

      case IDM_EDIT_MENU:
         ItiMenuGrayMenuItem (hwndMenu, IDM_ADD,    TRUE);
         ItiMenuGrayMenuItem (hwndMenu, IDM_CHANGE, TRUE);
         ItiMenuGrayMenuItem (hwndMenu, IDM_DELETE, TRUE);
         ItiMenuGrayMenuItem (hwndMenu, IDM_CUT,    FALSE);
         ItiMenuGrayMenuItem (hwndMenu, IDM_COPY,   FALSE);
         ItiMenuGrayMenuItem (hwndMenu, IDM_PASTE,  FALSE);
         bOK = 0 == WinSendMsg (hwndActive, WM_INITMENU, 
                                MPFROMSHORT (usMenuID),
                                MPFROMHWND (hwndMenu));
         if (!ItiPermQueryEdit (hwndActive))
            {
            ItiMenuGrayMenuItem (hwndMenu, IDM_ADD,    TRUE);
            ItiMenuGrayMenuItem (hwndMenu, IDM_CHANGE, TRUE);
            ItiMenuGrayMenuItem (hwndMenu, IDM_DELETE, TRUE);
            }
         return bOK;
         break;

      case IDM_WINDOW_MENU:
         return FALSE;

      case IDM_VIEW_MENU:
         {
         USHORT i;

         if (VIEW_MENU_COUNT < 
             ItiMenuDeleteMenuItem (hwndMenu, IDM_VIEW_USER_FIRST))
            {
            for (i=IDM_VIEW_USER_FIRST+1; i <= IDM_VIEW_USER_LAST; i++)
               {
               if (VIEW_MENU_COUNT == ItiMenuDeleteMenuItem (hwndMenu, i))
                  {
                  break;
                  }
               }
            }
         ItiMenuGrayMenuItem (hwndMenu, IDM_PARENT, FALSE);

         return 0 == WinSendMsg (hwndActive, WM_INITMENU, 
                                 MPFROMSHORT (usMenuID),
                                 MPFROMHWND (hwndMenu));
         }
         break;

      case IDM_UTILITIES_MENU:
         {
         ULONG    ulKind;
         USHORT   uKids;

         ItiMenuGrayMenuItem (hwndMenu, IDM_SET_PREFERENCES, TRUE);

         /*--- determine if Find/Sort is available ---*/
         ItiMenuGrayMenuItem (hwndMenu, IDM_FIND, TRUE);
         ItiMenuGrayMenuItem (hwndMenu, IDM_SORT, TRUE);
         uKids = (UM) QW (hwndActive, ITIWND_NUMKIDS, 0, 0, 0);
         for (; uKids; uKids--)
            {
            ulKind = (ULONG) QW (hwndActive, ITIWND_KIDTYPE, uKids-1, 0, 0);
            if ((ulKind & ITIWC_LIST) == ITIWC_LIST)
               {
               ItiMenuGrayMenuItem (hwndMenu, IDM_FIND, FALSE);
               ItiMenuGrayMenuItem (hwndMenu, IDM_SORT, FALSE);
               }
            }

         return 0 == WinSendMsg (hwndActive, WM_INITMENU, 
                             MPFROMSHORT (usMenuID), MPFROMHWND (hwndMenu));
         break;
         }

      default:
         return 0 == WinSendMsg (hwndActive, WM_INITMENU, MPFROMSHORT (usMenuID),
                                 MPFROMHWND (hwndMenu));
      }
   return TRUE;
   }




/*
 * returns TRUE if message processed, FALSE if not.
 */

BOOL ProcessCommand (HWND hwnd, USHORT usCommand, MPARAM mp2)
   {
   HWND           hwndActive;

   hwndActive = WinSendMsg (WinQueryWindow (pglobals->hwndAppClient,
                                            QW_PARENT, FALSE),
                            WM_QUERYACTIVEDOC, 0, 0);

   /* pass through to the default window procedure */
   if ((usCommand >= IDM_WINDOW_MENU_FIRST &&
        usCommand <= IDM_WINDOW_MENU_LAST) ||
#ifdef DEBUG
       (usCommand >= IDM_DEBUG_MENU_FIRST &&
        usCommand <= IDM_DEBUG_MENU_LAST) ||
#endif
       (usCommand >= IDM_HELP_MENU_FIRST &&
        usCommand <= IDM_HELP_MENU_LAST &&
        usCommand != IDM_ABOUT)         ||
        usCommand == IDM_EXIT)
      {
      return FALSE;
      }

   /* move this to the ITIWIN module, ItiWndAppDefWindowProc */
   if (usCommand >= IDM_FILE_OPEN_CATALOG_FIRST &&
       usCommand <= IDM_FILE_OPEN_CATALOG_LAST)
      {
      ItiMenuOpenCatalog (pglobals->hwndAppClient, usCommand);
      return TRUE;
      }

   /* move this to the ITIWIN module, ItiWndAppDefWindowProc */
   if (usCommand >= IDM_FILE_PRINT_FIRST &&
       usCommand <= IDM_FILE_PRINT_LAST)
      {
      ItiMenuPrint (pglobals->hwndAppClient, usCommand);
      return TRUE;
      }

   switch (usCommand)
      {
      case IDM_CLOSE:
         if (hwndActive != NULL)
            WinPostMsg (hwndActive, WM_CLOSE, 0, 0);
         break;

      case IDM_ABOUT:
         WinDlgBox (pglobals->hwndDesktop, hwnd, AboutDlgProc, 
                    0, IDD_ABOUT, NULL);
         break;


      case IDM_IMPORT:
         ItiMenuDoImport (hwnd);
         break;

      case IDM_RUN_OTHER:
         ItiMenuDaDoRunRunRun (hwnd);
         break;

      case IDM_EXPORT:
         ItiMenuDoExport (hwnd);
         break;

      case IDM_SORT:
      case IDM_FIND:
         {
         USHORT uKids, uListKids = 0;
         HWND   hwndList;
         ULONG  ulKind;

         /*--- In order to process this command in an automatic    ---*/
         /*--- fashion, The active window must contain exactly one ---*/
         /*--- list window. If there are 0, this message will not  ---*/
         /*--- be generated. If >1, this message is passed on to   ---*/
         /*--- the individual message processor for the active win ---*/

         uKids = (UM) QW (hwndActive, ITIWND_NUMKIDS, 0, 0, 0);
         for (; uKids; uKids--)
            {
            ulKind = (ULONG) QW (hwndActive, ITIWND_KIDTYPE, uKids-1, 0, 0);
            if (ulKind == ITIWC_LIST)
               {
               uListKids ++;
               hwndList = (HWND) QW (hwndActive, ITIWND_KID, uKids-1, 0, 0);
               }
            }
         if (uListKids == 1 && usCommand == IDM_FIND)
            ItiMenuDoFind (hwndList);
         else if (uListKids == 1 && usCommand == IDM_SORT)
            ItiMenuDoSort (hwndList);
         else
            return 0;
         break;
         }

      case IDM_RUN_ITEM_BROWSER:
         spawnl (P_NOWAIT, "dssitem.exe", "dssitem.exe", NULL);
         break;

      case IDM_RUN_SQL_BROWSER:
         spawnl (P_NOWAIT, "sqlbrow.exe", "sqlbrow.exe", NULL);
         break;

      case IDM_RUN_FACILITY_BROWSER:
         spawnl (P_NOWAIT, "dssfacil.exe", "dssfacil.exe", NULL);
         break;

      case IDM_RUN_PROPOSAL_BROWSER:
         spawnl (P_NOWAIT, "dssprop.exe", "dssprop.exe", NULL);
         break;

      case IDM_PRINTER_SETUP:
         ItiMenuDoPrinterSetup (hwnd);
         break;

      case IDM_GENERATE_SUPPORT_REQUEST:
         ItiMenuDoSupport (hwnd);
         break;

      case IDM_CHANGE_PASSWORD:
         ItiMenuChangePassword (hwnd);
         break;

      case IDM_SHOW_USERS:
         ItiMenuShowUsers (hwnd);
         break;

      case IDM_DENY_LOGON:
         ItiMenuDenyLogon (hwnd);
         break;

      case IDM_INSTALL:
         ItiMenuInstall (hwnd);
         break;

      case IDM_SET_PREFERENCES:
         ItiMenuSetPreferences (hwnd);
         break;

      case IDM_SEND_MESSAGE:
         ItiMenuSendMessage (hwnd);
         break;

      case IDM_COPY:
         WinSendMsg (hwndActive, WM_ITICOPY, 0, 0);
         return 0;

      case IDM_CUT:
         WinSendMsg (hwndActive, WM_ITICUT, 0, 0);
         return 0;

      case IDM_PASTE:
         WinSendMsg (hwndActive, WM_ITIPASTE, 0, 0);
         return 0;

      default:
         if (hwndActive != NULL)
            return 0 == WinSendMsg (hwndActive, WM_COMMAND, MPFROMSHORT (usCommand), 
                                    mp2);
         else
            return 0;
      }
   return TRUE;
   }
