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
 * WorkDay.c
 * Mark Hampton
 *
 * This module provides the Working Day catalog.
 */



#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimbase.h"
#include "..\include\winid.h"
#include "..\include\itiutil.h"
#include "WorkDay.h"
#include "initcat.h"
#include "..\include\itidbase.h"
#include "..\include\colid.h"
#include "..\include\itibase.h"
#include "..\include\itimenu.h"
#include "..\include\dialog.h"
#include "dialog.h"


static CHAR szDllVersion[] = "1.1a0 Workday.dll";


/* This is the window proc for the WorkingDay_Static window
 * All window procs must call ItiWndDefStaticWndProc as the
 * default window message handler.
 * This proc handles the child-window special-case as well as the
 * usual menu functions normally handled by the window proc
 */




WNDPROC EXPORT WorkingDayCalendarCatSProc (HWND hwnd, USHORT umsg, MPARAM mp1, MPARAM mp2)
   {
   switch (umsg)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               {
               BOOL bSelected;

               bSelected = ItiWndQueryActive (hwnd, WorkingDayCalendarCat, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               }
               return 0;
               break;

            case IDM_VIEW_MENU:
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2),
                                           IDM_VIEW_USER_FIRST);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Working Day Calendar",
                                      IDM_VIEW_USER_FIRST + 1);
               break;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, WorkingDayCalendarCat, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, WorkingDayCalendarCat, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, WorkingDayCalendarCat), ITI_DELETE);
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, WorkingDayCalendarCat);
               ItiWndBuildWindow (hwndChild, WorkingDayCalendarS);
               return 0;
            }
         break;

      /* need to have special paint for column 4 */
      }

   return ItiWndDefStaticWndProc (hwnd, umsg, mp1, mp2);
   }







MRESULT EXPORT WorkDayDProc (HWND    hwnd,
                                        USHORT   usMessage,
                                        MPARAM   mp1,
                                        MPARAM   mp2)
   {
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }




WNDPROC EXPORT WorkingDayCalendarSProc (HWND hwnd, USHORT umsg, MPARAM mp1, MPARAM mp2)
   {
   switch (umsg)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               {
               BOOL bSelected;

               bSelected = ItiWndQueryActive (hwnd, WorkingDayCalendarL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               return 0;
               break;
               }
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, WorkingDayCalendarL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, WorkingDayCalendarL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, WorkingDayCalendarL), ITI_DELETE);
               break;

            }
         break;

      /* need to have special paint for column 4 */
      }

   return ItiWndDefStaticWndProc (hwnd, umsg, mp1, mp2);
   }



MRESULT EXPORT WorkDayInfoDProc (HWND    hwnd,
                                          USHORT   usMessage,
                                          MPARAM   mp1,
                                          MPARAM   mp2)
   {
   switch (usMessage)
      {
      case WM_QUERYDONE:
         {
         //Select params    hwnd, uCtl,      uCtlCol, uQueryId, uQueryCol
         //==========================================================================================
         ItiWndActivateRow (hwnd, DID_MONTH, 0,       hwnd,     0);
         break;
         }
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
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



VOID EXPORT ItiDllQueryMenuName (PSZ      pszBuffer,
                                 USHORT   usMaxSize,
                                 PUSHORT  pusWindowID)
   {
   ItiStrCpy (pszBuffer, "~Working Day Catalog", usMaxSize);
   *pusWindowID = WorkingDayCalendarCatS;
   }




BOOL EXPORT ItiDllInitDll (void)
   {
   if (ItiMbRegisterStaticWnd (WorkingDayCalendarCatS, WorkingDayCalendarCatSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterStaticWnd (WorkingDayCalendarS, WorkingDayCalendarSProc, hmodMe))
      return FALSE;


   if (ItiMbRegisterListWnd (WorkingDayCalendarL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (WorkingDayCalendarCat, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (WorkDayD, WorkDayD, WorkDayDProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (WorkDayInfoD, WorkDayInfoD, WorkDayInfoDProc, hmodMe))
      return FALSE;



   return TRUE;
   }




