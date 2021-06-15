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
 * Command.c
 * Mark Hampton
 * Copyright (C) 1991 AASHTO
 */

#define INCL_WIN


#include "..\include\iti.h"
#include "..\include\winid.h"
#include "..\include\dialog.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\message.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itierror.h"
#include "..\include\itiperm.h"
#include "..\include\cesutil.h"
#include "..\include\dssimp.h"
#include "BAMSCES.h"
#include "menu.h"
#include "command.h"
#include "about.h"
#include "open.h"
#include "utility.h"
#include "basedate.h"
#include "jobpath.h"
#include "specyear.h"
#include <process.h>
#include <stdio.h>



/*
 * returns TRUE if message processed, FALSE if not.
 */



BOOL     bBuffersLoaded = FALSE;
HBUF     hbufSIC ; /* StandardItem Catalog */
HBUF     hbufSIAC; /* StandardItem Avg. Catalog */
HBUF     hbufSIRC; /* StandardItem Reg. Catalog */
HBUF     hbufSIDC; /* StandardItem Def. Catalog */
HBUF     hbufSICC; /* StandardItem CBE  Catalog */

HBUF EXPORT CmnPreLoadBuffer (USHORT uWinId)
   {
   USHORT   uJunk;
   HMODULE  hmod;
   CHAR     szQuery [1000];

   ItiMbQueryHMOD (uWinId, &hmod);
   ItiMbQueryQueryText (hmod, ITIRID_WND, uWinId, szQuery, sizeof szQuery);
   return ItiDbCreateBuffer (NULL, uWinId, ITI_LIST, szQuery, &uJunk);
   }



void CmnPREloadBuffers(void)
   {
   /* --- PRELOAD  SOME BUFFERS --- */
   bBuffersLoaded = TRUE;
   hbufSIC  = CmnPreLoadBuffer (StandardItemCatL);
   hbufSIAC = CmnPreLoadBuffer (StdItemPemethAvgL);
   hbufSIRC = CmnPreLoadBuffer (StdItemPemethRegL);
   hbufSIDC = CmnPreLoadBuffer (StdItemDefUPL);
   hbufSICC = CmnPreLoadBuffer (StdItemCBEstL);
   }

void CmnUNloadBuffers(void)
   {
   ItiDbDeleteBuffer (hbufSIC, NULL);
   ItiDbDeleteBuffer (hbufSIAC, NULL);
   ItiDbDeleteBuffer (hbufSIRC, NULL);
   ItiDbDeleteBuffer (hbufSIDC, NULL);
   ItiDbDeleteBuffer (hbufSICC, NULL);
   bBuffersLoaded = FALSE;
   }





/*
 * returns TRUE if message processed, FALSE if not.
 */

BOOL GreyMenuItems (HWND hwndMenu, USHORT usMenuID)
   {
   HWND  hwndActive;
   HWND  hwndSubmenu;
   BOOL  bOK;

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
         ItiMenuDeleteMenuItem (hwndMenu, (IDM_EDIT_MENU_FIRST + 6));
         ItiMenuDeleteMenuItem (hwndMenu, (IDM_EDIT_MENU_FIRST + 7));
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

         if (hwndActive == NULL || !ItiCesIsJobWindow (hwndActive, 0))
            {
            ItiMenuGrayMenuItem (hwndMenu, IDM_UPDATE_BASE_DATE, TRUE);
            ItiMenuGrayMenuItem (hwndMenu, IDM_UPDATE_SPEC_YEAR, TRUE);
            ItiMenuGrayMenuItem (hwndMenu, IDM_CALCULATE_JOB_SCHEDULE, TRUE);
            }
         else
            {
            ItiMenuGrayMenuItem (hwndMenu, IDM_UPDATE_BASE_DATE, FALSE);
            ItiMenuGrayMenuItem (hwndMenu, IDM_UPDATE_SPEC_YEAR, FALSE);
            ItiMenuGrayMenuItem (hwndMenu, IDM_CALCULATE_JOB_SCHEDULE, FALSE);
            }

         if (hwndActive == NULL || !ItiCesIsProgramWindow (hwndActive, 0))
            ItiMenuGrayMenuItem (hwndMenu, IDM_CALCULATE_PROGRAM_SCHEDULE, TRUE);
         else
            ItiMenuGrayMenuItem (hwndMenu, IDM_CALCULATE_PROGRAM_SCHEDULE, FALSE);

         ItiMenuGrayMenuItem (hwndMenu, IDM_SET_PREFERENCES, TRUE);

         // tlb -- 94NOV
         ItiMenuGrayMenuItem (hwndMenu, IDM_PURGE_BASE_DATES, !ItiPermIsSuperUser() );

         ItiMenuGrayMenuItem (hwndMenu, IDM_DECLARE_BASE_DATE, !ItiPermIsSuperUser ());
         // Following added for BSR 940063:
         ItiMenuGrayMenuItem (hwndMenu, IDM_DENY_LOGON, !ItiPermIsSuperUser ());

         ItiMenuGrayMenuItem (hwndMenu, IDM_SYS_MGR, !ItiPermIsSuperUser() );

         if (bBuffersLoaded)
            {
            ItiMenuGrayMenuItem (hwndMenu, IDM_PRELOAD, TRUE);
            ItiMenuGrayMenuItem (hwndMenu, IDM_UNLOAD, FALSE);
            }
         else
            {
            ItiMenuGrayMenuItem (hwndMenu, IDM_PRELOAD, FALSE);
            ItiMenuGrayMenuItem (hwndMenu, IDM_UNLOAD, TRUE);
            }
   
            
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

         hwndSubmenu = ItiMenuQuerySubmenu (hwndMenu, IDM_OPEN_SPECYEAR_MENU);
         ItiMenuInitOpenSpecYrMenuPopup (hwndSubmenu);

         return 0 == WinSendMsg (hwndActive, WM_INITMENU, 
                             MPFROMSHORT (usMenuID), MPFROMHWND (hwndMenu));
         break;
         }

      default:
         return 0 == WinSendMsg (hwndActive, WM_INITMENU, 
                                 MPFROMSHORT (usMenuID),
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
        usCommand <= IDM_HELP_MENU_LAST  &&
        usCommand != IDM_ABOUT)          || 
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

   if (usCommand >= IDM_SPECYR_FIRST &&
       usCommand <= IDM_SPECYR_LAST)
      {
      ItiWndSetHourGlass (TRUE);
      ItiMenuSetSpecYr (usCommand);
      SetSpecYrTitle();
      ItiWndSetHourGlass (FALSE);
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
      case IDM_EXPORT:
         ItiMenuDoExport (hwnd);
         break;

      case IDM_IMPORT:
         ItiMenuDoImport (hwnd);
         break;

      case IDM_NEW_JOB:
         ItiWndSetDlgMode (ITI_ADD);
         ItiWndBuildWindow (pglobals->hwndAppFrame, JobNewD);
         break;

      case IDM_NEW_PROGRAM:
         ItiWndSetDlgMode (ITI_ADD);
         ItiWndBuildWindow (pglobals->hwndAppFrame, ProgramD);
         break;

      case IDM_CLOSE:
         if (hwndActive != NULL)
            WinPostMsg (hwndActive, WM_CLOSE, 0, 0);
         break;

      case IDM_ABOUT:
         WinDlgBox (pglobals->hwndDesktop, hwnd, AboutDlgProc, 
                    0, IDD_ABOUT, NULL);
         break;

      case IDM_RUN_OTHER:
         ItiMenuDaDoRunRunRun (hwnd);
         break;

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

      case IDM_OPEN_PROGRAM:
         DoOpenProgramDialogBox ();
         break;

      case IDM_OPEN_JOB:
         DoOpenJobDialogBox ();
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
            {
            if (uListKids > 1)
               WinSendMsg (hwndActive, WM_COMMAND, MPFROMSHORT (usCommand), 
                           mp2);
            return 0;
            }
         break;
         }


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

      case IDM_DECLARE_BASE_DATE:
         DoDeclareBaseDate (hwnd);
         break;

      case IDM_CALCULATE_JOB_SCHEDULE:
         DoCalculateJobSchedule (hwndActive);
         break;

      case IDM_CALCULATE_PROGRAM_SCHEDULE:
         DoCalculateProgramSchedule (hwndActive);
         break;

      case IDM_UPDATE_BASE_DATE:
         DoUpdateBaseDate (hwndActive);
         break;

      // tlb -- 94NOV
      case IDM_PURGE_BASE_DATES:
         DoPurgeUnRefBaseDates (hwndActive);
         break;

      // tlb -- 95MAR
      case IDM_PRELOAD:
         CmnPREloadBuffers();
         break;

      // tlb -- 95MAR
      case IDM_UNLOAD:
         CmnUNloadBuffers();
         break;

      // tlb -- 95JAN
      case IDM_TOGGLE_SPECBOOK:
         ItiWndSetHourGlass (TRUE);
         ToggleUnitSystem();
         ItiWndSetHourGlass (FALSE);
         break;

      // tlb feb 96
      case IDM_SYS_MGR:
         ItiWndSetHourGlass (TRUE);
         ResetCalcHPMIItem ("DSShell");
         ItiWndSetHourGlass (FALSE);
         break;

      case IDM_OPEN_SPECYEAR_MENU:
         break;

      case IDM_UPDATE_SPEC_YEAR:
         DoUpdateSpecYear (hwndActive);
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

//      case IDM_STOPCALC:
//        ItiSemRequestMutex (hStopCalcQ, FOREVER);
//         return 0;

//      case IDM_STARTCALC:
//        ItiSemReleaseMutex (hStopCalcQ, FOREVER);
//         return 0;

      default:
         if (hwndActive != NULL)
            return 0 == WinSendMsg (hwndActive, WM_COMMAND, MPFROMSHORT (usCommand), 
                                    mp2);
         else
            return 0;
      }
   return TRUE;
   }
