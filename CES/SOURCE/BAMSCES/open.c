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
 * Open.c
 * Mark Hampton
 * (C) 1991 AASHTO
 *
 * This module handles the open program and open job dialog boxes.
 */


#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\winid.h"
#include "..\include\itiglob.h"
#include "..\include\itiperm.h"
#include "..\include\colid.h"
#include "bamsces.h"
#include "open.h"
#include "..\include\dialog.h"
#include "dialog.h"
#include "copy.h"

#include <stdio.h>

static  CHAR szQry[126] = "";
static  CHAR szPat[100] = " SELECT JobID, Description, JobKey"
                          " FROM Job"
                          " WHERE UnitType = %s "
                                " OR UnitType = NULL"
                          " ORDER BY JobID";


void DoOpenJobDialogBox ()
   {
   USHORT usReturnValue;

   usReturnValue = (USHORT) (ULONG) ItiWndBuildWindow (pglobals->hwndAppFrame, 
                                                       OpenJobD);
   }


void DoOpenProgramDialogBox ()
   {
   USHORT usReturnValue;

   usReturnValue = (USHORT) (ULONG) ItiWndBuildWindow (pglobals->hwndAppFrame, 
                                                       OpenProgramD);
   }


BOOL CopyJob (HWND hwnd, PSZ pszOldJobKey)
   {
   BOOL bOK;

   if (!ItiPermJobOpened (pszOldJobKey, TRUE))
      {
      WinMessageBox (HWND_DESKTOP, hwnd, 
                     "You do not have permission to copy that job.",
                     pglobals->pszAppName, 0, 
                     MB_MOVEABLE | MB_OK | MB_ICONHAND);
      return FALSE;
      }
   ItiPermJobClosed (pszOldJobKey);

   bOK = DoCopyJob (hwnd);
   return bOK;
   }



BOOL DeleteJob (HWND hwnd, HWND hwndList, PSZ pszKey)
   {
   char     szQuery [256];
   USHORT   usNumCols, usError;

   if (!ItiPermQueryDelete (pszKey, TRUE))
      {
      WinMessageBox (HWND_DESKTOP, hwnd, 
                     "You do not have permission to delete that job.",
                     pglobals->pszAppName, 0, 
                     MB_MOVEABLE | MB_OK | MB_ICONHAND);
      return FALSE;
      }
   else 
      {
      sprintf (szQuery, 
               "SELECT COUNT (*) FROM ProgramJob WHERE JobKey = %s",
               pszKey);
      if (ItiDbGetQueryCount (szQuery, &usNumCols, &usError))
         {
         WinMessageBox (HWND_DESKTOP, hwnd, 
                        "The job is still in a program.  You cannot delete "
                        "a job that is in a program.",
                        pglobals->pszAppName, 0, 
                        MB_MOVEABLE | MB_OK | MB_ICONHAND);
         return FALSE;
         }
      ItiWndAutoModifyDb (hwndList, ITI_DELETE);
      }
   return TRUE;
   }




BOOL DeleteProgram (HWND hwnd, HWND hwndList, PSZ pszKey)
   {
   if (!ItiPermQueryDelete (pszKey, TRUE))
      {
      WinMessageBox (HWND_DESKTOP, hwnd, 
                     "You do not have permission to delete that program.",
                     pglobals->pszAppName, 0, 
                     MB_MOVEABLE | MB_OK | MB_ICONHAND);
      return FALSE;
      }
   else 
      {
      ItiWndAutoModifyDb (hwndList, ITI_DELETE);
      }
   return TRUE;
   }

/*
 * Dialog box procedure for Open Job dialog box.
 */


MRESULT EXPORT OpenJobDProc (HWND   hwnd,
                            USHORT usMessage,
                            MPARAM mp1,
                            MPARAM mp2)
   {
   switch (usMessage)
      {
    //  case WM_INITQUERY:
    //     break;
    //
    //  case WM_ITIDBBUFFERDONE:
    //    if (pglobals->bUseMetric)
    //       {
    //       DosBeep (1400, 25);
    //       DosBeep (1500, 45);
    //       DosBeep (1400, 25);
    //       sprintf(szQry, szPat, "1 ");
    //       }
    //    else
    //       {
    //       DosBeep (900, 70);
    //       sprintf(szQry, szPat, "0 ");
    //       }
    //
    //    WinSendMsg (hwnd, WM_CHANGEQUERY, MPFROMLONG (-1L), MPFROMP (szQry));
    //    break;


      case WM_ITILWMOUSE:
         if (ItiWndIsLeftClick (mp2) && !ItiWndIsSingleClick (mp2))
            {
            WinPostMsg (hwnd, WM_COMMAND, MPFROMSHORT (DID_OK),
                        MPFROM2SHORT (CMDSRC_OTHER, FALSE));
            return 0;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               {
               /* check for a selected Job */
               USHORT usSelected;
               HWND   hwndList;
               HHEAP  hheap;
               PSZ    pszJobKey;

               hwndList = WinWindowFromID (hwnd, OpenJobDL);
               if (hwndList == NULL)
                  break;

               usSelected = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);
               hheap = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0);
               if (usSelected == 0xffff)
                  {
                  WinAlarm (HWND_DESKTOP, WA_ERROR);
                  }
               else
                  {
                  /* create child window */
                  pszJobKey = (PSZ) QW (hwndList, ITIWND_DATA, 0, usSelected, 
                                        cJobKey);
                  if (!ItiPermJobOpened (pszJobKey, TRUE))
                     {
                     WinMessageBox (HWND_DESKTOP, hwnd, 
                        "You do not have permission to open that Job.",
                        pglobals->pszAppName, 0, 
                        MB_OK | MB_MOVEABLE | MB_ICONHAND);
                     ItiMemFree (hheap, pszJobKey);
                     return 0;
                     }
                  ItiMemFree (hheap, pszJobKey);
                  ItiWndBuildWindow (hwndList, JobS);
                  WinDismissDlg (hwnd, TRUE);
                  }
               }
               return 0;


            case DID_CANCEL:
               WinDismissDlg (hwnd, FALSE);
               return 0;

            case DID_COPY:
            case DID_DELETE:
               {
               PSZ      pszKey;
               HHEAP    hheap;
               USHORT   usRow;
               HWND     hwndList;

               hwndList = WinWindowFromID (hwnd, OpenJobDL);

               hheap = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0);
               usRow = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);
               pszKey = (PSZ) QW (hwndList, ITIWND_DATA, 0, usRow, cJobKey);

               if (hheap == NULL || pszKey == NULL)
                  return 0;

               if (SHORT1FROMMP (mp1) == DID_DELETE)
                  DeleteJob (hwnd, hwndList, pszKey);
               else if (SHORT1FROMMP (mp1) == DID_COPY)
                  if (CopyJob (hwnd, pszKey))
                     ItiDbUpdateBuffers ("Job");

               ItiMemFree (hheap, pszKey);
               }
               return 0;
               break;
            }
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }



/*
 * Dialog box procedure for Open Program dialog box.
 */

MRESULT EXPORT OpenProgramDProc (HWND   hwnd,
                                USHORT usMessage,
                                MPARAM mp1,
                                MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_ITILWMOUSE:
         if (ItiWndIsLeftClick (mp2) && !ItiWndIsSingleClick (mp2))
            {
            WinPostMsg (hwnd, WM_COMMAND, MPFROMSHORT (DID_OK),
                        MPFROM2SHORT (CMDSRC_OTHER, FALSE));
            return 0;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               {
               /* check for a selected Job */
               USHORT usSelected;
               HWND   hwndList;
               HHEAP  hheap;
               PSZ    pszProgramKey;

               hwndList = WinWindowFromID (hwnd, OpenProgramDL);
               if (hwndList == NULL)
                  break;

               usSelected = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);
               hheap = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0);

               if (usSelected == 0xffff)
                  {
                  WinAlarm (HWND_DESKTOP, WA_ERROR);
                  }
               else
                  {
                  /* create child window */
                  pszProgramKey = (PSZ) QW (hwndList, ITIWND_DATA, 0, usSelected, 
                                        cProgramKey);
//
//  04/06/93 CLF 
//  The 2nd param must be false for a program
//
//                  if (!ItiPermJobOpened (pszProgramKey, TRUE))
//

                  if (!ItiPermJobOpened (pszProgramKey, FALSE))
                     {
                     WinMessageBox (HWND_DESKTOP, hwnd, 
                        "You do not have permission to open that Program.",
                        pglobals->pszAppName, 0, 
                        MB_OK | MB_MOVEABLE | MB_ICONHAND);
                     ItiMemFree (hheap, pszProgramKey);
                     return 0;
                     }
                  ItiMemFree (hheap, pszProgramKey);
                  ItiWndBuildWindow (hwndList, ProgramS);
                  WinDismissDlg (hwnd, TRUE);
                  }
               return 0;

            case DID_DELETE:
               {
               PSZ      pszKey;
               HHEAP    hheap;
               USHORT   usRow;
               HWND     hwndList;

               hwndList = WinWindowFromID (hwnd, OpenProgramDL);

               hheap = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0);
               usRow = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);
               pszKey = (PSZ) QW (hwndList, ITIWND_DATA, 0, usRow, cProgramKey);

               if (hheap == NULL || pszKey == NULL)
                  return 0;

               DeleteProgram (hwnd, hwndList, pszKey);

               ItiMemFree (hheap, pszKey);
               }
               return 0;
               }
               return 0;

            case DID_CANCEL:
               WinDismissDlg (hwnd, FALSE);
               return 0;
            }
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }
