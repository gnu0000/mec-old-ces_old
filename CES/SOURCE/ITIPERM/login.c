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
 * login.c
 * Mark Hampton
 */

#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itierror.h"
#include "..\include\itibase.h"
#include "..\include\itiperm.h"
#include "..\include\itiglob.h"
#include "..\include\itiwin.h"
#include "..\include\itiutil.h"
#include "..\include\itimbase.h"
#include "..\include\itidbase.h"
#include "..\include\itifmt.h"
#include "init.h"
#include <stdio.h>
#include "..\include\dialog.h"
#include "dialog.h"
#include "query.h"
#include "catalog.h"
#include "message.h"


#define LOGGED_IN       1
#define CANCEL          2
#define NOT_AUTHORIZED  3



BOOL LogonEnabled (void)
   {
   USHORT usNumCols, usError;
   char szQuery [512];

   sprintf (szQuery, 
            "SELECT AD.UserKey "
            "FROM ApplicationDisabled AD, Application A "
            "WHERE A.ApplicationID = '%s' "
            "AND A.ApplicationKey = AD.ApplicationKey ",
            pglobals->pszApplicationID);

   return !ItiDbGetQueryCount (szQuery, &usNumCols, &usError);
   }




static BOOL ValidateUser (HWND hwnd)
   {
   char     szID [17], szPassword [256];
   char     szTemp [512];
   char     szQuery [1024];
   PSZ      apszTokens [3], apszValues [3], *ppsz;
   USHORT   usNumCols;

   WinQueryDlgItemText (hwnd, DID_USERID, sizeof szID, szID);
   WinQueryDlgItemText (hwnd, DID_PASSWORD, sizeof szTemp, szTemp);
   if (0 != ItiFmtCheckString (szTemp, szPassword, sizeof szPassword, "Password", NULL))
      return FALSE;

   apszTokens [0] = "UserID";
   apszTokens [1] = "Password";
   apszTokens [2] = "ApplicationID";
   apszValues [0] = szID;
   apszValues [1] = szPassword;
   apszValues [2] = pglobals->pszApplicationID;

   ItiMbQueryQueryText (hmod, ITIRID_CALC, GET_USER_INFO, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, 
                        apszTokens, apszValues, 3);

   ppsz = ItiDbGetRow1 (szQuery, hheap, 0, 0, 0, &usNumCols);
   if (ppsz == NULL || ppsz [0] == NULL)
      return FALSE;

   /* free up the old user info */
   ItiMemFree (hheap, pglobals->pszUserID);
   ItiMemFree (hheap, pglobals->pszUserName);
   ItiMemFree (hheap, pglobals->pszUserKey);
   ItiMemFree (hheap, pglobals->pszPhoneNumber);

   pglobals->pszUserKey     = ppsz [0];
   pglobals->pszUserID      = ppsz [1];
   pglobals->pszUserName    = ppsz [2];
   pglobals->pszPhoneNumber = ppsz [4];
   ItiStrToUSHORT (ppsz [5], &(pglobals->usUserType));

   /* free up the junk */
   ItiMemFree (hheap, ppsz [3]);
   ItiMemFree (hheap, ppsz);

   return TRUE;
   }



MRESULT EXPENTRY LoginUserProc (HWND     hwnd,
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
         SWP   swpLoading;
         HWND  hwndLoading;

         hwndLoading = HWNDFROMMP (mp2);

         /* position the dialog box */
         WinQueryWindowPos (hwnd, &swpMe);
         WinQueryWindowPos (pglobals->hwndAppFrame, &swpApplication);
         WinQueryWindowPos (hwndLoading, &swpLoading);

         swpMe.x = swpApplication.x + swpApplication.cx / 2 - swpMe.cx / 2;
         swpMe.y = swpLoading.y -
                   (swpMe.cy + (SHORT) pglobals->alSysValues [SV_CYMENU]);

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
         switch (COMMANDMSG (&usMessage)->cmd)
            {
            case DID_OK:
               if (ValidateUser (hwnd))
                  WinDismissDlg (hwnd, LOGGED_IN);
               else
                  WinDismissDlg (hwnd, NOT_AUTHORIZED);
               return 0;

            case DID_CANCEL:
               WinDismissDlg (hwnd, CANCEL);
               return 0;
            }
         break;
      }
   return WinDefDlgProc (hwnd, usMessage, mp1, mp2);
   }




BOOL EXPORT ItiPermLogin (HWND hwndLoading)
   {
   USHORT i, usLoggedIn = CANCEL;

   if (!bInited && !ItiPermInit ())
      {
      WinMessageBox (pglobals->hwndDesktop, pglobals->hwndAppFrame,
                     "Could not initailize the login module!",
                     "Logon", 0, MB_OK | MB_MOVEABLE | MB_ICONEXCLAMATION);
      return FALSE;
      }

   if (!LogonEnabled ())
      {
      WinMessageBox (pglobals->hwndDesktop, pglobals->hwndAppFrame,
                     "The system administrator has disabled logons.",
                     "Logon", 0, MB_OK | MB_MOVEABLE | MB_ICONHAND);
      return FALSE;
      }

   for (i = 0; i < 3; i++)
      {
      usLoggedIn = WinDlgBox (pglobals->hwndDesktop, pglobals->hwndAppFrame,
                              LoginUserProc, hmod, IDD_LOGIN, hwndLoading);
      if (usLoggedIn == CANCEL ||
          usLoggedIn == LOGGED_IN)
         break;
      }

   switch (usLoggedIn)
      {
      case NOT_AUTHORIZED:
         WinMessageBox (pglobals->hwndDesktop, 
                        pglobals->hwndAppFrame,
                        "You are not authorized to run this application.",
                        pglobals->pszAppName, 
                        0, MB_OK | MB_MOVEABLE | MB_ICONHAND);
         break;

      case LOGGED_IN:
         InitCatalogForUser (pglobals->pszUserKey);
         ShowMessages ();
         break;

      case CANCEL:
         break;

      default:
         WinMessageBox (pglobals->hwndDesktop, 
                        pglobals->hwndAppFrame,
                        "An unknown error occured in ItiPerm.  Because of "
                        "this, the application cannot run.  Please contact "
                        "Info Tech Technical Support.",
                        pglobals->pszAppName, 
                        0, MB_OK | MB_MOVEABLE | MB_ICONHAND);
         break;
      }

   return usLoggedIn == LOGGED_IN;
   }


