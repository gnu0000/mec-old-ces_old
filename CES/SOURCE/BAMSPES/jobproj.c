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
#include "bamspes.h"
#include "jobproj.h"
#include "menu.h"
#include "dialog.h"
//#include "joe.h"

/*---- This is not a hack. -----*/
#include "..\itiwin\swmain.h"


#include <stdio.h>

/*
 * Window procedure for JobProject window.
 */








MRESULT EXPORT ProjectsForJobStaticProc (HWND   hwnd,
                                         USHORT uMsg,
                                         MPARAM mp1,
                                         MPARAM mp2)
   {
   switch (uMsg)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               {
               BOOL bSelected;

               bSelected = ItiWndQueryActive (hwnd, ProjectsForJobL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               return 0;
               }
               break;

            case IDM_VIEW_MENU:
               {
               BOOL bSelected;

               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_PARENT, TRUE);
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2), 
                                           IDM_VIEW_USER_FIRST);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "Project ~Category", 
                                      IDM_VIEW_USER_FIRST + 1);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "Project ~Items", 
                                      IDM_VIEW_USER_FIRST + 2);

               /*--- Dont allow access to this win if no projects exist ---*/
               bSelected = ItiWndQueryActive (hwnd, ProjectsForJobL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_VIEW_USER_FIRST + 1, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_VIEW_USER_FIRST + 2, !bSelected);
               return 0;
               }
               break;
            }
            break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, ProjectsForJobL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, ProjectsForJobL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, ProjectsForJobL), ITI_DELETE);
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, ProjectsForJobL);
               ItiWndBuildWindow (hwndChild, ProjectCategoryS);
               return 0;

            case (IDM_VIEW_USER_FIRST + 2):
               ItiWndBuildWindow (hwnd, ProjectItemS);
               return 0;
            }
            break;
      }

   return ItiWndDefStaticWndProc (hwnd, uMsg, mp1, mp2);
   }





MRESULT EXPORT ProjectCategoryStaticProc (HWND   hwnd,
                                          USHORT uMsg,
                                          MPARAM mp1,
                                          MPARAM mp2)
   {
   switch (uMsg)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               {
               BOOL bSelected;

               bSelected = ItiWndQueryActive (hwnd, ProjectCategoryL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, TRUE);
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
            case IDM_ADD:
              ItiWndDoDialog (hwnd, ProjectCategoryL, TRUE);
              break;

            case IDM_DELETE:
              ItiWndAutoModifyDb (WinWindowFromID (hwnd, ProjectCategoryL), ITI_DELETE);
              break;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, uMsg, mp1, mp2);
   }














//
//
//MRESULT EXPORT ProjectItemsSProc (HWND   hwnd,
//                                  USHORT uMsg,
//                                  MPARAM mp1,
//                                  MPARAM mp2)
//   {
//   return ItiWndDefStaticWndProc (hwnd, uMsg, mp1, mp2);
//   }
//
//
//


MRESULT EXPORT EditProjectsProc (HWND   hwnd,
                                 USHORT uMsg,
                                 MPARAM mp1,
                                 MPARAM mp2)

   {
   switch (uMsg)
      {
//      case WM_COMMAND:
//         switch (SHORT1FROMMP (mp1))
//            {
//            case DID_OK:
//            case DID_ADDMORE:
//            }
//         break;


//  initial code
//
//      case WM_QUERYDONE:
//         {
//         ItiWndActivateRow (hwnd, DID_WORKTYPE, cCodeValueKey, hwnd, cWorkType);
//         ItiWndActivateRow (hwnd, DID_DISTRICT, cCodeValueKey, hwnd, cDistrict);
//
//         if (ITI_CHANGE == ItiWndGetDlgMode ())
//            {
//            WinSendDlgItemMsg (hwnd, DID_PCN, EM_SETREADONLY, MPFROMSHORT(1), 0);
//            WinEnableWindow (WinWindowFromID (hwnd, DID_PCN), FALSE);
//            WinSendDlgItemMsg (hwnd, DID_PCN+DID_STOFFSET, EM_SETREADONLY, MPFROMSHORT(1), 0);
//            WinEnableWindow (WinWindowFromID (hwnd, DID_PCN+DID_STOFFSET), FALSE);
//            }
//         break;
//         }
//


      case WM_QUERYDONE:
         {
         if (ITI_CHANGE == ItiWndGetDlgMode ())
            {
            ItiWndActivateRow (hwnd, DID_WORKTYPE, cCodeValueKey, hwnd, cWorkType);
            ItiWndActivateRow (hwnd, DID_DISTRICT, cCodeValueKey, hwnd, cDistrict);

            WinSendDlgItemMsg (hwnd, DID_PCN, EM_SETREADONLY, MPFROMSHORT(1), 0);
            WinEnableWindow (WinWindowFromID (hwnd, DID_PCN), FALSE);
            WinSendDlgItemMsg (hwnd, DID_PCN+DID_STOFFSET, EM_SETREADONLY, MPFROMSHORT(1), 0);
            WinEnableWindow (WinWindowFromID (hwnd, DID_PCN+DID_STOFFSET), FALSE);
            }
         else /* ITI_ADD */
            {
            HWND     hwndStatic;
			   PDWDAT   pdwdat;
            PSZ      psz;

            hwndStatic = WinSendMsg (hwnd, WM_ITIWNDQUERY, 
                                    MPFROM2SHORT (ITIWND_OWNERWND, 1), 0);

            /*--- try a cheat here ---*/
			   pdwdat = (PDWDAT)WinSendMsg (hwnd, WM_GETDLGDAT, 0L, 0L);
			   pdwdat->uDlgMode = ITI_CHANGE;
            ItiWndActivateRow (hwnd, DID_WORKTYPE, cCodeValueKey, hwndStatic, cWorkType);
            ItiWndActivateRow (hwnd, DID_DISTRICT, cCodeValueKey, hwndStatic, cDistrict);
			   pdwdat->uDlgMode = ITI_ADD;

            psz= (PSZ) QW (hwndStatic, ITIWND_DATA, 0, 0, cLocation);
            WinSetDlgItemText (hwnd, DID_LOCATION, psz);
            }
         break;
         }
      }
   return ItiWndDefDlgProc (hwnd, uMsg, mp1, mp2);
   }






MRESULT EXPORT EditProjectCategoryProc (HWND   hwnd,
                                        USHORT uMsg,
                                        MPARAM mp1,
                                        MPARAM mp2)
   {
   switch (uMsg)
      {
      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
            case DID_ADDMORE:
               {
               MRESULT  mr;
               HHEAP    hheap;
               HWND     hwndStatic;
               PSZ      pszJobKey, pszPCN;
               char     szCategory [50];

               hwndStatic = WinSendMsg (hwnd, WM_ITIWNDQUERY, 
                                        MPFROM2SHORT (ITIWND_OWNERWND, 1), 0);

               pszJobKey  = WinSendMsg (hwndStatic, WM_ITIWNDQUERY, 
                                        MPFROMSHORT (ITIWND_DATA), 
                                        MPFROM2SHORT (0, cJobKey));

               pszPCN     = WinSendMsg (hwndStatic, WM_ITIWNDQUERY, 
                                        MPFROMSHORT (ITIWND_DATA), 
                                        MPFROM2SHORT (0, cProjectControlNumber));

               hheap = WinSendMsg (hwndStatic, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_HEAP), 0);

               WinQueryDlgItemText (hwnd, DID_CATEGORY, sizeof szCategory,
                                        szCategory);

               mr = ItiWndDefDlgProc (hwnd, uMsg, mp1, mp2);

//               AddProjectItems (hheap, pszJobKey, pszPCN, szCategory);
               ItiMemFree (hheap, pszJobKey);
               ItiMemFree (hheap, pszPCN);

               return mr;
               }
            }
         break;
      }
   return ItiWndDefDlgProc (hwnd, uMsg, mp1, mp2);
   }
