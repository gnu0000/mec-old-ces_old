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
 * Proposal.c
 * Mark Hampton
 * (C) 1991 AASHTO
 *
 * This module handles the Proposal window.
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
#include "bamsdss.h"
#include "proposal.h"
#include "menu.h"
#include "dialog.h"

#include <stdio.h>

/*
 * Window procedure for Proposal window.
 */

MRESULT EXPORT ProposalSProc (HWND   hwnd,
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
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, FALSE);
               return 0;
               }
               break;


            case IDM_VIEW_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_PARENT, TRUE);
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2), 
                                           IDM_VIEW_USER_FIRST);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "~Bidder", 
                                      IDM_VIEW_USER_FIRST + 1);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "~Project", 
                                      IDM_VIEW_USER_FIRST + 2);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                    "Proposal ~Items", 
                                  IDM_VIEW_USER_FIRST + 3);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "Proposal ~Major Items", 
                                      IDM_VIEW_USER_FIRST + 4);

               return 0;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, 0, FALSE);
               return 0;
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, ProposalList1);
               ItiWndBuildWindow (hwndChild, BidderS);
               return 0;
            
            case (IDM_VIEW_USER_FIRST + 2):
               hwndChild = WinWindowFromID (hwnd, ProposalList2);
               ItiWndBuildWindow (hwndChild, ProjectS);
               return 0;
            
            case (IDM_VIEW_USER_FIRST + 3):
               ItiWndBuildWindow (hwnd, ProposalItemS);
               return 0;
            
            case (IDM_VIEW_USER_FIRST + 4):
               ItiWndBuildWindow (hwnd, ProposalMajItemS);
               return 0;
            }
         break;
      }

   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }





MRESULT EXPORT ProposalMajItemSProc (HWND   hwnd,
                                    USHORT usMessage,
                                    MPARAM mp1,
                                    MPARAM mp2)
   {

   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }



static CHAR *ppszUnit[] = {"Unit Price",
                           "Extention"};

static CHAR *ppszViews[] = {"Awarded",
                            "Low Bid",
                            "Estimate"};


MRESULT EXPORT ProposalItemSProc (HWND  hwnd,
                                 USHORT uMsg,
                                 MPARAM mp1,
                                 MPARAM mp2)
   {
   switch (uMsg)
      {
      case WM_INITQUERY:
         {
         USHORT   i;

         /*--- Fill / Display Combobox ---*/
         for (i=0; i < sizeof ppszUnit / sizeof (PSZ); i++)
            WinSendDlgItemMsg (hwnd, ProposalItemC1, LM_INSERTITEM, (MPARAM)LIT_END, MPFROMP (ppszUnit[i]));

         for (i=0; i < sizeof ppszViews / sizeof (PSZ); i++)
            {
            WinSendDlgItemMsg (hwnd, ProposalItemC2, LM_INSERTITEM, (MPARAM)LIT_END, MPFROMP (ppszViews[i]));
            WinSendDlgItemMsg (hwnd, ProposalItemC3, LM_INSERTITEM, (MPARAM)LIT_END, MPFROMP (ppszViews[i]));
            WinSendDlgItemMsg (hwnd, ProposalItemC4, LM_INSERTITEM, (MPARAM)LIT_END, MPFROMP (ppszViews[i]));
            }

         WinSendDlgItemMsg (hwnd, ProposalItemC1, LM_SELECTITEM, (MPARAM)0, MPFROMP (TRUE));
         WinSendDlgItemMsg (hwnd, ProposalItemC2, LM_SELECTITEM, (MPARAM)0, MPFROMP (TRUE));
         WinSendDlgItemMsg (hwnd, ProposalItemC3, LM_SELECTITEM, (MPARAM)1, MPFROMP (TRUE));
         WinSendDlgItemMsg (hwnd, ProposalItemC4, LM_SELECTITEM, (MPARAM)2, MPFROMP (TRUE));

         break;
         }


      case WM_CONTROL:
         {
         USHORT  i, j, x, uCmd, uCombo;
         HWND    hwndL;
         PEDT    pedt;

         uCmd   = SHORT2FROMMP (mp1);
         uCombo = SHORT1FROMMP (mp1);
         x = 0;

         switch (uCombo)
            {
            case ProposalItemC4:
               x += 1;
            case ProposalItemC3:
               x += 1;
            case ProposalItemC2:
               if (uCmd != CBN_LBSELECT)
                  break;

               i = (UM) WinSendDlgItemMsg (hwnd, uCombo, LM_QUERYSELECTION, 0, 0);
               j = (UM) WinSendDlgItemMsg (hwnd, ProposalItemC1, LM_QUERYSELECTION, 0, 0);
               if (i == LIT_NONE)
                  return 0;

               hwndL = WinWindowFromID (hwnd, ProposalItemL);

               pedt = (PEDT) QW (hwndL, ITIWND_DATAEDT, x, 0, 0);
               pedt->uIndex = 0x8001 + i + (j * 3);

               WinInvalidateRect (WinWindowFromID (hwnd, ProposalItemL), NULL, TRUE);
               break;

            case ProposalItemC1:
               {
               if (uCmd != CBN_LBSELECT)
                  break;

               j = (UM) WinSendDlgItemMsg (hwnd, ProposalItemC1, LM_QUERYSELECTION, 0, 0);
               hwndL = WinWindowFromID (hwnd, ProposalItemL);

               i = (UM) WinSendDlgItemMsg (hwnd, ProposalItemC2, LM_QUERYSELECTION, 0, 0);
               pedt = (PEDT) QW (hwndL, ITIWND_DATAEDT, 0, 0, 0);
               pedt->uIndex = 0x8001 + i + (j * 3);
               i = (UM) WinSendDlgItemMsg (hwnd, ProposalItemC3, LM_QUERYSELECTION, 0, 0);
               pedt = (PEDT) QW (hwndL, ITIWND_DATAEDT, 1, 0, 0);
               pedt->uIndex = 0x8001 + i + (j * 3);
               i = (UM) WinSendDlgItemMsg (hwnd, ProposalItemC4, LM_QUERYSELECTION, 0, 0);
               pedt = (PEDT) QW (hwndL, ITIWND_DATAEDT, 2, 0, 0);
               pedt->uIndex = 0x8001 + i + (j * 3);

               WinInvalidateRect (WinWindowFromID (hwnd, ProposalItemL), NULL, TRUE);
               break;

               }
            break;
            }
         break;
         }
      }

   return ItiWndDefStaticWndProc (hwnd, uMsg, mp1, mp2);
   }


void DoProposalDialogBox ()
   {
   USHORT usReturnValue;

   usReturnValue = (USHORT) (ULONG) ItiWndBuildWindow (pglobals->hwndAppFrame, 
                                                       ProposalD);
   }




MRESULT EXPORT ProposalDProc (HWND   hwnd,
                                 USHORT usMessage,
                                 MPARAM mp1,
                                 MPARAM mp2)
   {
   MRESULT  mr;

   switch (usMessage)
      {
      case WM_INITDLG:
         break;

      case WM_QUERYDONE:
         {
         ItiWndActivateRow (hwnd, DID_WORKTYPE, cCodeValueKey, hwnd, cWorkType);
         ItiWndActivateRow (hwnd, DID_ROADTYPE, cCodeValueKey, hwnd, cRoadType);
         ItiWndActivateRow (hwnd, DID_DISTRICT, cCodeValueKey, hwnd, cDistrict);
         break;
         }
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
               ItiWndSetHourGlass (TRUE);
               mr = ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
               ItiWndSetHourGlass (FALSE);
               return mr;
               }

            case DID_CANCEL:
               WinDismissDlg (hwnd, FALSE);
               return 0;
            }
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }
