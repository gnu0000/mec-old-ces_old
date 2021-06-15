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
 * DefUser.c
 * Mark Hampton
 * (C) 1991 AASHTO
 *
 * This module handles the Default User window.
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
#include "defuser.h"
#include "menu.h"
#include "dialog.h"


/*
 * Window procedure for Default User window.
 */



MRESULT EXPORT DefaultUserSProc (HWND     hwnd, 
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

               bSelected = ItiWndQueryActive (hwnd, DefaultUserL1, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, TRUE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, TRUE);
               return 0;
               }
            }

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, DefaultUserL1, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, DefaultUserL1, TRUE);
               break;
            }   
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }






MRESULT EXPORT DefltJobProgUsersDProc (HWND    hwnd,
                                          USHORT   uMsg,
                                          MPARAM   mp1,
                                          MPARAM   mp2)
   {
   switch (uMsg)
      {
      case WM_COMMAND:
         {
         switch (SHORT1FROMMP (mp1))
            {
            case DID_ADD1:
               ItiWndDoNextDialog (hwnd, DefltJobUsersL, DefJobUsersD, ITI_ADD);
               break;

            case DID_CHANGE1:
               ItiWndDoNextDialog (hwnd, DefltJobUsersL, DefJobUsersD, ITI_CHANGE);
               break;

            case DID_ADD2:
               ItiWndDoNextDialog (hwnd, DefltProgUsersL, DefProgUsersD, ITI_ADD);
               break;

            case DID_CHANGE2:
               ItiWndDoNextDialog (hwnd, DefltProgUsersL, DefProgUsersD, ITI_CHANGE);
               break;

            case DID_DELETE1:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, DefltJobUsersL), ITI_DELETE);
               break;

            case DID_DELETE2:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, DefltProgUsersL), ITI_DELETE);
               break;
             }
          }

      case WM_QUERYDONE:
         {
         ItiWndActivateRow (hwnd, DefltJobUsersL,  cUserKey, hwnd, 0);
         ItiWndActivateRow (hwnd, DefltProgUsersL, cUserKey, hwnd, 0);

         break;
         }
       }
   return ItiWndDefDlgProc (hwnd, uMsg, mp1, mp2);
   }














MRESULT EXPORT DefProgUsersDProc (HWND    hwnd,
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
            PSZ      pszTemp;
            HWND     hwndList;
            HHEAP    hheap;
            USHORT   usRow;

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
               WinSetDlgItemText (hwnd, DID_USRGRPRIV, pszTemp);
               ItiMemFree (hheap, pszTemp);
               }

             }
         break;

      case WM_QUERYDONE:
         {
         ItiWndActivateRow (hwnd, DID_ACCESSPRIV, 0, hwnd, 0);
         break;
         }
       }
   
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }











//  MRESULT EXPORT ChDefJobUsersEditDlgProc (HWND    hwnd,
//                                          USHORT   usMessage,
//                                          MPARAM   mp1,
//                                          MPARAM   mp2)
//   {
//   switch (usMessage)
//      {
//      case WM_QUERYDONE:
//         {
//         break;
//         }
//       }
//   
//   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
//   }





MRESULT EXPORT DefJobUsersDProc (HWND    hwnd,
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
            PSZ    pszTemp;
            HWND   hwndList;
            HHEAP  hheap;
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
               WinSetDlgItemText (hwnd, DID_USRGRPRIV, pszTemp);
               ItiMemFree (hheap, pszTemp);
               }

             }
         break;

      case WM_QUERYDONE:
         {
         ItiWndActivateRow (hwnd, DID_ACCESSPRIV, 0, hwnd, 0);
         break;
         }
       }
   
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }
