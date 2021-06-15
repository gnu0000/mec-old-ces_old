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
 * JobFund.h
 * Mark Hampton
 * (C) 1991 AASHTO
 *
 * This module handles the Job Fund window.
 */

#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\winid.h"
#include "..\include\itiglob.h"
#include "..\include\winid.h"
#include "bamsces.h"
#include "jobfund.h"
#include "menu.h"


/*
 * Window procedure for Job Fund window.
 */

MRESULT EXPORT JobFundSProc (HWND   hwnd,
                            USHORT usMessage,
                            MPARAM mp1,
                            MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, TRUE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, TRUE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, TRUE);
               return 0;
               break;

//            case IDM_VIEW_MENU:
//               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2), 
//                                           IDM_VIEW_USER_FIRST);
//               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
//                                      "Job ~Breakdown Funds?", 
//                                      IDM_VIEW_USER_FIRST + 1);

            case IDM_UTILITIES_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_FIND, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_SORT, FALSE);
               return 0;
               break;
            }
            break;

      case WM_COMMAND:
         {
         HWND hwndList;

         hwndList = WinWindowFromID (hwnd, JobFundL);
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
//               ItiWndBuildWindow (hwndList, CodeTableD);
               return 0;
               break;

//            case (IDM_VIEW_USER_FIRST + 1):
//               ItiWndBuildWindow (hwndList, JobBreakdownFundS);
//               return 0;
//               break;
            }
         }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }


