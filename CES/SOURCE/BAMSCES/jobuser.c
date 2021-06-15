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
 * JobUser.h
 * Mark Hampton
 * (C) 1991 AASHTO
 *
 * This module handles the Job User window.
 */

#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\winid.h"
#include "..\include\itiglob.h"
#include "..\include\winid.h"
#include "..\include\colid.h"
#include "..\include\dialog.h"
#include "..\include\itibase.h"
#include "bamsces.h"
#include "jobuser.h"
#include "menu.h"
#include "dialog.h"


/*
 * Window procedure for Job User window.
 */

MRESULT EXPORT JobUserSProc (HWND   hwnd,
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
               {
               BOOL bSelected;

               bSelected = ItiWndQueryActive (hwnd, JobUserL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               return 0;
               }

            case IDM_UTILITIES_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_FIND, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_SORT, FALSE);
               return 0;
               break;
            }
            break;

      case WM_COMMAND:
         {
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, JobUserL, FALSE);
               return 0;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, JobUserL, TRUE);
               return 0;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, JobUserL), ITI_DELETE);
               return 0;
               break;

            }
         }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }






MRESULT EXPORT ChJobUserDProc (HWND    hwnd,
                                  USHORT   usMessage,
                                  MPARAM   mp1,
                                  MPARAM   mp2)
   {
   switch (usMessage)
      {
      case WM_QUERYDONE:
         {
         //Select params         hwnd, uCtl,           uCtlCol,           uQueryId, uQueryCol
         //==========================================================================================
         ItiWndActivateRow (hwnd, DID_ACCESSPRIV, 0, hwnd, 0);

         break;
         }
       }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }





MRESULT EXPORT AddJobUserDProc (HWND    hwnd,
                                  USHORT   usMessage,
                                  MPARAM   mp1,
                                  MPARAM   mp2)
   {
   switch (usMessage)
      {
      case WM_ITILWMOUSE:
         if (SHORT1FROMMP (mp2) == DefJobUserCatL &&
             ItiWndIsSingleClick (mp2) && ItiWndIsLeftClick (mp2))
            {
            PSZ  pszTemp;
            HWND hwndList;
            HHEAP hheap;
            USHORT usRow;

            hwndList = WinWindowFromID (hwnd, DefJobUserCatL);
            if (hwndList == NULL)
               break;

            usRow = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);

            hheap = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0);
            if (hheap == NULL)
               break;

            pszTemp = (PSZ) QW (hwndList, ITIWND_DATA, 0, usRow, cUserID);
            if (pszTemp != NULL)
               {
               WinSetDlgItemText (hwnd, DID_USERID, pszTemp);
               ItiMemFree (hheap, pszTemp);
               }
             }
         break;

      case WM_QUERYDONE:
         {
         ItiWndActivateRow (hwnd, DefJobUserCatL,  0, hwnd, 0);
         break;
         }
       }

   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }
