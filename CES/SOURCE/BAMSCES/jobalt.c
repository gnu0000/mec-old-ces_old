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
 * JobAlt.c
 * Mark Hampton
 * (C) 1991 AASHTO
 *
 * This module handles the JobAlternateGroup and JobAlternate windows.
 */


#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\winid.h"
#include "..\include\itiglob.h"
#include "..\include\dialog.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\colid.h"
#include "..\include\itiutil.h"
#include "bamsces.h"
#include "jobalt.h"
#include "menu.h"
#include "dialog.h"
#include <stdio.h>

/*
 * Window procedure for JobAlt window.
 */





MRESULT EXPORT JobAlternateGroupListSProc (HWND     hwnd, 
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

               bSelected = ItiWndQueryActive (hwnd, JobAlternateGroupL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               return 0;
               }

            case IDM_VIEW_MENU:
               {
               BOOL     bSelected;

               bSelected = ItiWndQueryActive (hwnd, JobAlternateGroupL, NULL);
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2), IDM_VIEW_USER_FIRST);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "Job ~Alternate Group", 
                                      IDM_VIEW_USER_FIRST + 1);
               ItiMenuGrayMenuItem   (HWNDFROMMP (mp2),
                                      IDM_VIEW_USER_FIRST + 1, !bSelected);
               return 0;
               }
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, JobAlternateGroupL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, JobAlternateGroupL, TRUE);
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               ItiWndBuildWindow (WinWindowFromID (hwnd, JobAlternateGroupL), 
                                  JobAlternateGroupS);
               break;
            }   
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }



static BOOL CreateChildRelations (HHEAP hheap, 
                                  PSZ   pszJobAlternateGroupKey, 
                                  PSZ   pszJobKey)
   {
   PSZ pszJobAlternateKey;
   char szTemp [400] = "";

   pszJobAlternateKey = ItiDbGetKey (hheap, "JobAlternate", 1);
   if (pszJobAlternateKey == NULL)
      return FALSE;

   sprintf (szTemp, "/* ..\bamsces\jobalt.c CreateChildRelations */ "
                    "UPDATE JobAlternateGroup "
                    "SET ChosenAlternateKey = %s "
                    "WHERE JobKey = %s "
                    "AND JobAlternateGroupKey = %s ",
                    pszJobAlternateKey, pszJobKey, pszJobAlternateGroupKey);

   ItiDbExecSQLStatement (hheap, szTemp);

   sprintf (szTemp, "/* ..\bamsces\jobalt.c CreateChildRelations */ "
                    "INSERT INTO JobAlternate "
                    "(JobKey, JobAlternateGroupKey, JobAlternateKey, "
                    "JobAlternateID, Description, Estimate) "
                    "VALUES "
                    "(%s,%s,%s,'Non-Alt',"
                    "'Non-Alternate Items.',0)",
                    pszJobKey, pszJobAlternateGroupKey, pszJobAlternateKey);

   if (ItiDbExecSQLStatement (hheap, szTemp))
      return FALSE;

   ItiMemFree (hheap, pszJobAlternateKey);
   return TRUE;
   }



MRESULT EXPORT JobAltGroupDProc (HWND     hwnd,
                                    USHORT   usMessage,
                                    MPARAM   mp1,
                                    MPARAM   mp2)
   {
   static PSZ pszJobAlternateGroupKey;

   switch (usMessage)
      {
      case WM_NEWKEY:
         {
         HHEAP hheap;

         if (ItiStrICmp ((PSZ) mp1, "JobAlternateGroup") == 0)
            {
            hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
            pszJobAlternateGroupKey = ItiStrDup (hheap, (PSZ) mp2);
            }

         return 0;
         }
         break;


      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               {
               MRESULT  mr;
               HHEAP    hheap;
               HBUF     hbuf;
               HWND     hwndOwner;
               char     szJobKey [60] = "";

               pszJobAlternateGroupKey = NULL;
               if (mr = ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2))
                  return mr;

               if (pszJobAlternateGroupKey != NULL)
                  {
                  hwndOwner = (HWND) QW (hwnd, ITIWND_OWNERWND, 0, 0, 0);
                  hbuf = (HBUF) QW (hwndOwner, ITIWND_BUFFER, 0, 0, 0);
                  ItiDbGetBufferRowCol (hbuf, 0, cJobKey, szJobKey);

                  hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);

                  CreateChildRelations (hheap, pszJobAlternateGroupKey,
                                        szJobKey);

                  ItiMemFree (hheap, pszJobAlternateGroupKey);
                  }
               return mr;
               }
               break;
            }
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }



MRESULT EXPORT JobAlternateGroupSProc (HWND     hwnd, 
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

               bSelected = ItiWndQueryActive (hwnd, JobAlternateL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               return 0;
               }
            }

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, JobAlternateL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, JobAlternateL, TRUE);
               break;
            }   
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }






MRESULT EXPORT JobAltDProc (HWND    hwnd,
                               USHORT   usMessage,
                               MPARAM   mp1,
                               MPARAM   mp2)
   {
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }
