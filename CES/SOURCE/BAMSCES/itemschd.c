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
 * ItemSchd.c
 * Mark Hampton
 * (C) 1991 AASHTO
 *
 * This module handles the Job Item Schedule window.
 */

#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\winid.h"
#include "..\include\itiglob.h"
#include "..\include\winid.h"
#include "..\include\dialog.h"
#include "..\include\colid.h"
#include "bamsces.h"
#include "itemschd.h"
#include "dialog.h"




/*
 * Window procedure procedure for Job Item Schedule window.
 */

MRESULT EXPORT JobItemScheduleSProc (HWND   hwnd,
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

               bSelected = ItiWndQueryActive (hwnd, JobItemScheduleL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, TRUE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, TRUE);
               return 0;
               break;
               }
            }
            break;

      case WM_COMMAND:
         {
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, JobItemScheduleL, FALSE);
               return 0;
               break;
            }
         }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }




static void SetEditButtons (HWND hwnd)
   {
   BOOL bEnable;

   DosSleep (1000);
   bEnable = !!(UM)QW (WinWindowFromID (hwnd, ItemSchPredL), ITIWND_NUMROWS, 0, 0, 0);
   WinEnableWindow (WinWindowFromID (hwnd, DID_CHANGE1), bEnable);
   WinEnableWindow (WinWindowFromID (hwnd, DID_DELETE1), bEnable);
   bEnable = !!(UM)QW (WinWindowFromID (hwnd, ItemSchSucL), ITIWND_NUMROWS, 0, 0, 0);
   WinEnableWindow (WinWindowFromID (hwnd, DID_CHANGE2), bEnable);
   WinEnableWindow (WinWindowFromID (hwnd, DID_DELETE2), bEnable);
   }

 


MRESULT EXPORT ItemSchedDProc (HWND    hwnd,
                                       USHORT   uMsg,
                                       MPARAM   mp1,
                                       MPARAM   mp2)
   {
   switch (uMsg)
      {
      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, ItemSchPredL, 0, NULL, 0);
         ItiWndActivateRow (hwnd, ItemSchSucL,  0, NULL, 0);
         break;

      case WM_CHILDQUERYDONE:
         SetEditButtons (hwnd);
         break;

      case WM_COMMAND:
         {
         switch (SHORT1FROMMP (mp1))
            {
            case DID_ADD1:
               ItiWndDoNextDialog (hwnd, ItemSchPredL, PredecessorsAddD, ITI_ADD);
               break;

            case DID_CHANGE1:
               ItiWndDoNextDialog (hwnd, ItemSchPredL, PredecessorsChangeD, ITI_CHANGE);
               break;

            case DID_ADD2:
               ItiWndDoNextDialog (hwnd, ItemSchSucL, SuccessorsAddD, ITI_ADD);
               break;

            case DID_CHANGE2:
               ItiWndDoNextDialog (hwnd, ItemSchSucL, SuccessorsChangeD, ITI_CHANGE);
               break;

            case DID_DELETE1:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, ItemSchPredL), ITI_DELETE);
               break;

            case DID_DELETE2:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, ItemSchSucL), ITI_DELETE);
               break;
            }
         }

      }
   return ItiWndDefDlgProc (hwnd, uMsg, mp1, mp2);
   }






MRESULT EXPORT PredecessorsAddDProc (HWND   hwnd,
                                     USHORT uMsg,
                                     MPARAM mp1,
                                     MPARAM mp2)
   {
   switch (uMsg)
      {
      case WM_QUERYDONE:
         {
         ItiWndActivateRow (hwnd, PredSchedAddL, cStandardItemKey, NULL, cStandardItemKey);
         /*--- delete default choice ---*/
         WinSendDlgItemMsg (hwnd, DID_LINK, LM_DELETEITEM, 0, 0);
         WinSendDlgItemMsg (hwnd, DID_LINK, LM_SELECTITEM, (MPARAM) 2, (MPARAM) TRUE);
         break;
         }
      }
   return ItiWndDefDlgProc (hwnd, uMsg, mp1, mp2);
   }


MRESULT EXPORT PredecessorsChangeDProc (HWND   hwnd,
                                        USHORT uMsg,
                                        MPARAM mp1,
                                        MPARAM mp2)
   {
   switch (uMsg)
      {
      case WM_QUERYDONE:
         {
         ItiWndActivateRow (hwnd, PredSchedChangeL, cStandardItemKey, NULL, cStandardItemKey);
         WinSendDlgItemMsg (hwnd, DID_LINK, LM_DELETEITEM, 0, 0);
         ItiWndActivateRow (hwnd, DID_LINK, cLink, hwnd, cLink);
         break;
         }
      }
   return ItiWndDefDlgProc (hwnd, uMsg, mp1, mp2);
   }





MRESULT EXPORT SuccessorsChangeDProc (HWND   hwnd,
                                      USHORT uMsg,
                                      MPARAM mp1,
                                      MPARAM mp2)
   {
   switch (uMsg)
      {
      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, SuccSchedChangeL, cStandardItemKey, NULL, cStandardItemKey);
         /*--- delete default choice ---*/
         WinSendDlgItemMsg (hwnd, DID_LINK, LM_DELETEITEM, 0, 0);
         ItiWndActivateRow (hwnd, DID_LINK, cLink, hwnd, cLink);
         break;
      }
   return ItiWndDefDlgProc (hwnd, uMsg, mp1, mp2);
   }





MRESULT EXPORT SuccessorsAddDProc (HWND   hwnd,
                                   USHORT uMsg,
                                   MPARAM mp1,
                                   MPARAM mp2)
   {
   switch (uMsg)
      {
      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, SuccSchedAddL, cStandardItemKey, NULL, cStandardItemKey);
         /*--- delete default choice ---*/
         WinSendDlgItemMsg (hwnd, DID_LINK, LM_DELETEITEM, 0, 0);
         WinSendDlgItemMsg (hwnd, DID_LINK, LM_SELECTITEM, (MPARAM) 2, (MPARAM) TRUE);
         break;
      }
   return ItiWndDefDlgProc (hwnd, uMsg, mp1, mp2);
   }


