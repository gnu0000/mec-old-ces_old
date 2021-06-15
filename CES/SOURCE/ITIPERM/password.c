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
 * password.c
 * Mark Hampton
 */

#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itierror.h"
#include "..\include\itibase.h"
#include "..\include\itiperm.h"
#include "..\include\itiglob.h"
#include "..\include\itiwin.h"
#include "..\include\winid.h"
#include "..\include\itiutil.h"
#include "..\include\itimbase.h"
#include "..\include\itidbase.h"
#include "..\include\itifmt.h"
#include "init.h"
#include <stdio.h>
#include "..\include\dialog.h"
#include "dialog.h"
#include "util.h"
#include "query.h"



static BOOL CheckOldPassword (HWND hwnd)
   {
   char szPassword [256];

   WinQueryDlgItemText (hwnd, DID_OLD, sizeof szPassword, szPassword);
   return VerifyPassword (pglobals->pszUserID, szPassword);
   }


static BOOL ChangePassword (PSZ pszUserKey, PSZ pszNewPassword)
   {
   PSZ      apszTokens [2], apszValues [2];
   char     szTemp [256];
   char     szQuery [1024];

   apszTokens [0] = "UserKey";
   apszTokens [1] = "Password";
   apszValues [0] = pszUserKey;
   apszValues [1] = pszNewPassword;

   ItiMbQueryQueryText (hmod, ITIRID_CALC, UPDATE_PASSWORD, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                        apszTokens, apszValues, 2);

   return 0 == ItiDbExecSQLStatement (hheap, szQuery);
   }



MRESULT EXPORT PasswordDProc (HWND     hwnd,
                              USHORT   usMessage,
                              MPARAM   mp1,
                              MPARAM   mp2)
   {
   switch (usMessage)
      {
      case WM_INITDLG:
         {
         SWP   swpMe;
         SWP   swpApplication;

         /* position the dialog box */
         WinQueryWindowPos (hwnd, &swpMe);
         WinQueryWindowPos (pglobals->hwndAppFrame, &swpApplication);

         swpMe.x = swpApplication.x + swpApplication.cx / 2 - swpMe.cx / 2;
         swpMe.y = swpApplication.y + swpApplication.cy -
                   (swpMe.cy + (SHORT) pglobals->alSysValues [SV_CYTITLEBAR] +
                    (SHORT) pglobals->alSysValues [SV_CYMENU] * 2);

         if (swpMe.x < 0)
            swpMe.x = 0;

         if (swpMe.y < 0)
            swpMe.y = 0;
         
         if (swpMe.x + swpMe.cx > (SHORT) pglobals->alSysValues [SV_CXSCREEN])
            swpMe.x -= (swpMe.x + swpMe.cx) -
                       (SHORT) pglobals->alSysValues [SV_CXSCREEN];

         if (swpMe.y + swpMe.cy > (SHORT) pglobals->alSysValues [SV_CYSCREEN])
            swpMe.y -= (swpMe.y + swpMe.cy) -
                       (SHORT) pglobals->alSysValues [SV_CYSCREEN];

         swpMe.fs = SWP_MOVE;
         WinSetMultWindowPos (hwnd, &swpMe, 1);
         }
         return 0;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               {
               char szNew [256], szVerify [256];

               if (!CheckOldPassword (hwnd))
                  {
                  WinMessageBox (HWND_DESKTOP, hwnd, 
                                 "Incorrect old password", 
                                 "Change Password", 0, 
                                 MB_OK | MB_MOVEABLE | MB_ICONEXCLAMATION);
                  return 0;
                  }

               WinQueryDlgItemText (hwnd, DID_NEW, sizeof szNew, szNew);
               WinQueryDlgItemText (hwnd, DID_VERIFY, sizeof szVerify, szVerify);
               if (ItiStrCmp (szNew, szVerify) != 0)
                  {
                  WinMessageBox (HWND_DESKTOP, hwnd, 
                                 "Your new passwords don't match each other.", 
                                 "Change Password", 0, 
                                 MB_OK | MB_MOVEABLE | MB_ICONEXCLAMATION);
                  return 0;
                  }

               if (0 != ItiFmtCheckString (szVerify, szNew, sizeof szNew, 
                                           "Password", NULL))
                  {
                  WinMessageBox (HWND_DESKTOP, hwnd, 
                                 "Your new password is not valid.", 
                                 "Change Password", 0, 
                                 MB_OK | MB_MOVEABLE | MB_ICONEXCLAMATION);
                  return 0;
                  }

               /* update the database */
               if (!ChangePassword (pglobals->pszUserKey, szNew))
                  {
                  WinMessageBox (HWND_DESKTOP, hwnd, 
                                 "The database could not be updated.  Your password has not been changed.", 
                                 "Change Password", 0, 
                                 MB_OK | MB_MOVEABLE | MB_ICONEXCLAMATION);
                  return 0;
                  }
               WinDismissDlg (hwnd, DID_OK);
               }
               break;

            case DID_CANCEL:
               WinDismissDlg (hwnd, DID_CANCEL);
               break;
            }
         break;
      }
   return WinDefDlgProc (hwnd, usMessage, mp1, mp2);
   }



void EXPORT ItiPermChangePassword (HWND hwnd)
   {
   WinDlgBox (pglobals->hwndDesktop, hwnd, PasswordDProc, hmod, PasswordD, NULL);
   }

