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
 * This module handles the open proposal and open project dialog boxes.
 */


#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\winid.h"
#include "..\include\itiglob.h"
#include "..\include\winid.h"
#include "bamsdss.h"
#include "open.h"


void DoOpenProposalDialogBox ()
   {
   USHORT usReturnValue;

   usReturnValue = (USHORT) (ULONG) ItiWndBuildWindow (pglobals->hwndAppFrame, 
                                                       OpenProposalD);
   }



void DoOpenProjectDialogBox ()
   {
   USHORT usReturnValue;

   usReturnValue = (USHORT) (ULONG) ItiWndBuildWindow (pglobals->hwndAppFrame, 
                                                       OpenProjectD);
   }



/*
 * Dialog box procedure for Open Proposal dialog box.
 */

MRESULT EXPORT OpenProposalDProc (HWND   hwnd,
                                 USHORT usMessage,
                                 MPARAM mp1,
                                 MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_INITDLG:
         break;

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
               /* check for a selected Proposal */
               USHORT usSelected;
               HWND   hwndList;

               hwndList = WinWindowFromID (hwnd, OpenProposalDL);
               if (hwndList == NULL)
                  break;

               usSelected = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);

               if (usSelected == 0xffff)
                  {
                  WinAlarm (HWND_DESKTOP, WA_ERROR);
                  }
               else
                  {
                  /* create child window */
                  ItiWndBuildWindow (hwndList, ProposalS);
                  WinDismissDlg (hwnd, TRUE);
                  }
               }
               return 0;

            case DID_CANCEL:
               WinDismissDlg (hwnd, FALSE);
               return 0;
            }
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }





/*
 * Dialog box procedure for Open Project dialog box.
 */

MRESULT EXPORT OpenProjectDProc (HWND   hwnd,
                                USHORT usMessage,
                                MPARAM mp1,
                                MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_INITDLG:
         break;

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
               /* check for a selected Project */
               USHORT usSelected;
               HWND   hwndList;

               hwndList = WinWindowFromID (hwnd, OpenProjectDL);
               if (hwndList == NULL)
                  break;

               usSelected = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);

               if (usSelected == 0xffff)
                  {
                  WinAlarm (HWND_DESKTOP, WA_ERROR);
                  }
               else
                  {
                  /* create child window */
                  ItiWndBuildWindow (hwndList, ProjectS);
                  WinDismissDlg (hwnd, TRUE);
                  }
               }
               return 0;

            case DID_CANCEL:
               WinDismissDlg (hwnd, FALSE);
               return 0;
            }
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }








