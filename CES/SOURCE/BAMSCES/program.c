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
 * Program.c
 * Mark Hampton
 * (C) 1991 AASHTO
 *
 * This module handles the Program window.
 */


#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\winid.h"
#include "..\include\colid.h"
#include "..\include\itiglob.h"
#include "..\include\winid.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itiutil.h"
#include <stdio.h>
#include <string.h>
#include "bamsces.h"
#include "job.h"
#include "menu.h"


/*
 * Window procedure for Program window.
 */

MRESULT EXPORT ProgramSProc (HWND   hwnd,
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
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, FALSE);
               return 0;
               break;

            case IDM_VIEW_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_PARENT, TRUE);
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2), 
                                           IDM_VIEW_USER_FIRST);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "~Default Users", 
                                      IDM_VIEW_USER_FIRST + 1);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "Program ~Fund List", 
                                      IDM_VIEW_USER_FIRST + 2);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "Program ~Job List", 
                                      IDM_VIEW_USER_FIRST + 3);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "Program ~User List", 
                                      IDM_VIEW_USER_FIRST + 4);
               return 0;
               break;
            }
            break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, 0, FALSE);
               return 0;
               break;
            
            case (IDM_VIEW_USER_FIRST + 1):
               ItiWndBuildWindow (hwnd, DefaultUserS);
               return 0;
            
            case (IDM_VIEW_USER_FIRST + 2):
               ItiWndBuildWindow (hwnd, ProgramFundS);
               return 0;
               break;
            
            case (IDM_VIEW_USER_FIRST + 3):
               ItiWndBuildWindow (hwnd, ProgramJobS);
               return 0;
               break;
            
            case (IDM_VIEW_USER_FIRST + 4):
               ItiWndBuildWindow (hwnd, ProgramUserS);
               return 0;
               break;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }




static BOOL InsertInitialProgramFund (HHEAP hheap, PSZ ProgramKey);




MRESULT EXPORT ProgramDProc (HWND   hwnd,
                                USHORT uMsg,
                                MPARAM mp1,
                                MPARAM mp2)
   {
   static PSZ pszKey;


   switch (uMsg)
      {

      case WM_NEWKEY:
         {
         HHEAP hheap;

         if (ItiStrICmp ((PSZ) mp1, "Program") == 0)
            {
            hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);

            pszKey = ItiStrDup (hheap, (PSZ) mp2);
            }
         return 0;
         break;
         }


      case WM_COMMAND:

      switch (SHORT1FROMMP (mp1))
         {
         case DID_OK:
            {
            MRESULT mr;
            HHEAP hheap;

            pszKey = NULL;

            if (mr = ItiWndDefDlgProc(hwnd, uMsg, mp1, mp2))
               return mr;
               
            if (pszKey != NULL)
               {
               hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);

               if (hheap == NULL)
                   break;

               InsertInitialProgramFund(hheap, pszKey);

               ItiMemFree (hheap, pszKey);
               }
            return mr;
            }
            break;

         }
      }
   return ItiWndDefDlgProc (hwnd, uMsg, mp1, mp2);
   }









/*                                                                  
 * This function inserts the initial ProgramFund (UNF) when  
 * a new Program is added by the user
 *
 */                                                                

static BOOL InsertInitialProgramFund (HHEAP hheap, PSZ pszProgramKey)

   {
   char szTemp [300] = "";

   sprintf (szTemp, "INSERT INTO ProgramFund "
                    "(ProgramKey, FundKey, PercentFunded, AmountFunded) "
                    "VALUES "
                    "(%s, 1, 1.00, 0) ", pszProgramKey);



   ItiDbExecSQLStatement (hheap, szTemp);
      
   sprintf (szTemp, "INSERT INTO ProgramUser "
                    "(ProgramKey, UserID, AccessPrivilege) "
                    "VALUES "
                    "(%s,'%s',30)",
                    pszProgramKey, pglobals->pszUserID);

   ItiDbExecSQLStatement (hheap, szTemp);

   sprintf (szTemp, "INSERT INTO ProgramUser "
                    "(ProgramKey, UserID, AccessPrivilege) "
                    "SELECT %s, UserGrantedPrivilege, AccessPrivilege "
                    "FROM DefaultProgramUser "
                    "WHERE UserGrantedPrivilege != '%s' "
                    "AND UserKey = %s ",
                    pszProgramKey, pglobals->pszUserID, pglobals->pszUserKey);

   ItiDbExecSQLStatement (hheap, szTemp);

   return TRUE;
   }
