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
 * Project.c
 * Mark Hampton
 * (C) 1991 AASHTO
 *
 * This module handles the Project window.
 */


#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itiutil.h"
#include "..\include\winid.h"
#include "..\include\colid.h"
#include "..\include\itiglob.h"
#include "..\include\dialog.h"
#include "bamsdss.h"
#include "project.h"
#include "menu.h"
#include "dialog.h"

#include <stdio.h>

/*
 * Window procedure for Project window.
 */

MRESULT EXPORT ProjectSProc (HWND   hwnd,
                            USHORT usMessage,
                            MPARAM mp1,
                            MPARAM mp2)
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
                                      "~Category", 
                                      IDM_VIEW_USER_FIRST + 1);

               return 0;
             }
             break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;
            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, ProjectL);
               ItiWndBuildWindow (hwndChild, CategoryS);
               return 0;

            }                                                   break;
      }

   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }





MRESULT EXPORT CategorySProc (HWND   hwnd,
                             USHORT usMessage,
                             MPARAM mp1,
                             MPARAM mp2)
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
                                      "~Project Item List", 
                                      IDM_VIEW_USER_FIRST + 1);
               return 0;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case (IDM_VIEW_USER_FIRST + 1):
               ItiWndBuildWindow (hwnd, ProjectItemS);
               return 0;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }












CHAR *ppszViews[] =     {"Awarded Bidder",
                         "Low Bidder",
                         "Engineer's Estimate"};


MRESULT EXPORT ProjectItemSProc (HWND   hwnd,
                                USHORT uMsg,
                                MPARAM mp1,
                                MPARAM mp2)
   {
   switch (uMsg)
      {
      case WM_INITQUERY:
         {
         USHORT   i;

         /*--- Display Combobox ---*/
         for (i=0; i < sizeof ppszViews / sizeof (PSZ); i++)
            WinSendDlgItemMsg (hwnd, ProjectItemC, LM_INSERTITEM,
                                  (MPARAM)LIT_END, MPFROMP (ppszViews[i]));

         WinSendDlgItemMsg (hwnd, ProjectItemC, LM_SELECTITEM, (MPARAM)0, MPFROMP (TRUE));
         break;
         }

      case WM_QUERYDONE:
         WinShowWindow (WinWindowFromID (hwnd, ProjectItemC), FALSE);
         WinUpdateWindow (hwnd);
         WinShowWindow (WinWindowFromID (hwnd, ProjectItemC), TRUE);
         break;

      case WM_CONTROL:
         {
         USHORT  i, uCmd;
         HWND    hwndL;
         PEDT    pedt;
         PSZ     pszLabel;

         WinShowWindow (WinWindowFromID (hwnd, ProjectItemC), FALSE);
         WinUpdateWindow (hwnd);
         WinShowWindow (WinWindowFromID (hwnd, ProjectItemC), TRUE);

         uCmd = SHORT2FROMMP (mp1);

         switch (SHORT1FROMMP (mp1))
            {
            case ProjectItemC:
               if (uCmd != CBN_LBSELECT)
                  break;

               i = (UM) WinSendDlgItemMsg (hwnd, ProjectItemC, LM_QUERYSELECTION, 0, 0);
               if (i == LIT_NONE)
                  return 0;

               hwndL = WinWindowFromID (hwnd, ProjectItemL);

               pedt = (PEDT) QW (hwndL, ITIWND_DATAEDT, 0, 0, 0);
               pedt->uIndex = 0x8001 + (i * 2);

               pedt = (PEDT) QW (hwndL, ITIWND_DATAEDT, 1, 0, 0);
               pedt->uIndex = 0x8002 + (i * 2);

               pszLabel = (PSZ) QW (hwnd, ITIWND_DATA, 0, 0, 0x8003 + i);
               SWL (hwnd, ITIWND_LABEL, 0, pszLabel);

               WinInvalidateRect (WinWindowFromID (hwnd, ProjectItemL), NULL, TRUE);
               break;
            }
         break;
         }

      }
   return ItiWndDefStaticWndProc (hwnd, uMsg, mp1, mp2);
   }
