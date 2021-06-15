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
 * ProgJob.c
 * Mark Hampton
 * (C) 1991 AASHTO
 *
 * This module handles the Program Jobs window.
 */


#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\winid.h"
#include "..\include\itiglob.h"
#include "..\include\winid.h"
#include "..\include\colid.h"
#include "..\include\itidbase.h"
#include "..\include\itiperm.h"
#include "..\include\itibase.h"
#include "..\include\itiutil.h"
#include "..\include\itifmt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bamsces.h"
#include "progjob.h"
#include "menu.h"
#include "dialog.h"
#include "calcfn.h"


USHORT PJErr (HWND hwnd, PSZ psz, USHORT uRet)
   {
   WinMessageBox (HWND_DESKTOP, hwnd, psz, pglobals->pszAppName, 0,
                  MB_OK | MB_MOVEABLE | MB_ICONHAND);
   return uRet;
   }

/*
 * This function checks to make sure the user, 
 * who is looking at a program,
 * has permission to open the job window.
 * ret: TRUE= has permission.
 * 4/8/93 CLF
 */
BOOL HasPerms (HWND hwnd)
   {
   USHORT uSel;
   HWND   hwndList;
   HHEAP  hheap;
   PSZ    pszJobKey;

   if (!(hwndList = WinWindowFromID (hwnd, ProgramJobL)))
      return PJErr (hwnd, "Could not obtain List Window ID for ProgramJobL", FALSE);

   if (0xFFFF == (uSel = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0)))
      return PJErr (hwnd, "Could not obtain Selection Index from ProgramJobL", FALSE);

   if (!(hheap = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0)))
      return PJErr (hwnd, "Could not obtain Window Heap from ProgramJobL", FALSE);

   if (!(pszJobKey = (PSZ) QW (hwndList, ITIWND_DATA, 0, uSel, cJobKey)))
      return PJErr (hwnd, "Could not obtain Job Key from ProgramJobL", FALSE);

   if (!ItiPermJobOpened (pszJobKey, TRUE))
      {
      ItiMemFree (hheap, pszJobKey);
      return PJErr (hwnd, "You do not have permission to open that Job.", FALSE);
      }
   ItiMemFree (hheap, pszJobKey);
   return TRUE;
   }



/*
 * Window procedure for Program Job window.
 */

MRESULT EXPORT ProgramJobSProc (HWND   hwnd,
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

               bSelected = ItiWndQueryActive (hwnd, ProgramJobL, NULL);

               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2), 
                                           IDM_EDIT_MENU_FIRST + 6);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), "C~hange Pred/Succ", 
                                      IDM_EDIT_MENU_FIRST + 7);

               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD,    FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CUT,    FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_COPY,   FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_PASTE,  FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_EDIT_MENU_FIRST + 7, !bSelected);
               return 0;
               break;
               }
            

            case IDM_VIEW_MENU:
               {
               BOOL bSelected;

               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_PARENT, FALSE);
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2), 
                                           IDM_VIEW_USER_FIRST);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), "~Job", 
                                      IDM_VIEW_USER_FIRST + 1);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), "~Inflation Prediction", 
                                      IDM_VIEW_USER_FIRST + 2);

               bSelected = ItiWndQueryActive (hwnd, ProgramJobL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2),
                                    IDM_VIEW_USER_FIRST + 1, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2),
                                    IDM_VIEW_USER_FIRST + 2, !bSelected);
               return 0;
               break;
               }

            case IDM_UTILITIES_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_FIND, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_SORT, FALSE);
               return 0;
               break;
            }
            break;



      case WM_COMMAND:
         {
         HWND hwndList;

         hwndList = WinWindowFromID (hwnd, ProgramJobL);
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, ProgramJobL, FALSE);
               return 0;
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, ProgramJobL, TRUE);
               return 0;
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, ProgramJobL), ITI_DELETE);
               DoDelProgJobReCalc(hwnd);
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               if (HasPerms (hwnd))
                  ItiWndBuildWindow (hwndList, JobS);
               return 0;
               break;

            case (IDM_VIEW_USER_FIRST + 2):
               ItiWndBuildWindow (hwndList, InflationPredictionS);
               return 0;
               break;
            case (IDM_EDIT_MENU_FIRST + 7):
               ItiWndDoNextDialog (hwnd, ProgramJobL, ProgramJobSchedD, ITI_CHANGE);
               return 0;
               break;
            }
         }
         break;


      /* ---  initializing ProgramJobPredL and ProgramJobSucL  --- */
      /* ---  is a special case. This window's keys come from the      --- */
      /* ---  window's list sibling rather than the frame window's     --- */
      /* ---  parent list window                                       --- */

      /* --- mp1 = parent hbuf               --- */
      /* --- mp2l=parent row                 --- */
      /* --- mp2h=no autostart children querys? --- */
      case WM_INITQUERY: 
         {
         HWND     hwndL1;

         /* --- manually init normal list window --- */
         hwndL1 = WinWindowFromID (hwnd, ProgramJobL);
         WinSendMsg (hwndL1, WM_INITQUERY, mp1, mp2);

         /* --- other list window isn't yet ready--- */

         /* --- turn off auto child init --- */
         mp2 = MPFROM2SHORT (SHORT1FROMMP (mp2), TRUE);

         /* --- allow msg to continue slightly modified --- */
         return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
         }

//                        -------------------------------------
//                        |DATA    |DATA    |LABEL   |LABEL   |
//                        |ELEMENT |AREA    |ELEMENT |AREA    |
//      -------------------------------------------------------
//      |SingleClickLeft  |   *    |   *    |   *    |   *    |
//      |                 |        |        |        |        |
//      |SingleClickRight |  ------ Adjust Window pos ------  |
//      |                 |        |        |        |        |
//      |DoubleClickLeft  | assoc. |   *    | assoc. |   *    | 
//      |                 |        |        |        |        |
//      |DoubleClickRight | change | change |  add   |  add   |
//      |                 |        |        |        |        |
//      -------------------------------------------------------
      case WM_ITILWMOUSE:
         {
         MPARAM   MP1, MP2;
         HBUF     hbufL1;
         HWND     hwndL1, hwndL2, hwndL3;
         char     szJobID [64];
         HHEAP    hheap;

         // if the used dbl-clicks on a job in the ProgJob List
         // then we intercept the mouse msg to make sure he/she
         // has permission to view that job
         // 4/8/93 clf
         //
         if ((SHORT1FROMMP (mp2) == ProgramJobL) && // correct win ?
             !ItiWndIsSingleClick (mp2) &&          // dbl click   ?
             ItiWndIsLeftClick (mp2) &&             // left button ?
             !HasPerms (hwnd))                      // permission  ?
            return 0;


         if (!(SHORT1FROMMP (mp2) == ProgramJobL) ||
             !ItiWndIsSingleClick (mp2) ||
             !ItiWndIsLeftClick (mp2))
            return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);

         /* --- O.K. the ProgramJobL window was clicked in.    --- */
         /* --- so now we must update the query in the             --- */
         /* --- ProgramJobPredL and ProgramJobSucL window  --- */

         hheap  = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
         hwndL1 = WinWindowFromID (hwnd, ProgramJobL);
         hwndL2 = WinWindowFromID (hwnd, ProgramJobPredL);
         hwndL3 = WinWindowFromID (hwnd, ProgramJobSucL);
         hbufL1 = (HBUF) QW (hwndL1, ITIWND_BUFFER, 0, 0, 0);

         ItiDbGetBufferRowCol (hbufL1, SHORT1FROMMP (mp1), cJobID, szJobID);
         SWL (hwnd, ITIWND_LABEL, 2, szJobID);
         SWL (hwnd, ITIWND_LABEL, 3, szJobID);

         MP1 = MPFROMP (hbufL1);  /* --- buffer to use  --- */
         MP2 = mp1;               /* --- row clicked on in low word--- */
         WinSendMsg (hwndL2, WM_CHANGEQUERY, MP1, MP2);
         WinSendMsg (hwndL3, WM_CHANGEQUERY, MP1, MP2);
         return 0;
         }
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }






/*
 * This function inserts a row in the ProgramFund table if the Program Job
 * being inserted is funded by a fund not currently in the ProgramFund table. 
 */


static BOOL InsertProgFunds (HHEAP hheap, PSZ pszJobKey, PSZ pszProgramKey);





MRESULT EXPORT ChProgJobDProc (HWND    hwnd,
                                  USHORT  uMsg,
                                  MPARAM  mp1,
                                  MPARAM  mp2)
   {
   switch (uMsg)
      {
      //Select params         hwnd, uCtl,           uCtlCol,           uQueryId, uQueryCol
      //==========================================================================================

      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, DID_INFLPRED,    cInflationPredictionKey, hwnd, 0);
         ItiWndActivateRow (hwnd, ChProgJobL,   cJobKey,                 hwnd, 0);
         WinEnableWindow (WinWindowFromID (hwnd, DID_WORKENDDATE+DID_STOFFSET), FALSE);
         WinSendDlgItemMsg (hwnd, DID_WORKENDDATE, EM_SETREADONLY, 
                            MPFROMSHORT (1), 0);
         break;

      case WM_COMMAND:

      switch (SHORT1FROMMP (mp1))
         {
         case DID_OK:
         case DID_ADDMORE:
            {
            MRESULT mr;
            HWND    hwndList, hwndParent, hwndTemp;
            HHEAP   hheap, hheapList;
            USHORT  usRow, usRet;
            PSZ     pszJobKey, pszProgramKey, pszDuration;
            CHAR    szWorkDate[50], szTmp[50];
            CHAR    szPriority[9];
            LONG    lJulDay;


            WinQueryDlgItemText (hwnd, DID_WORKBEGINDATE,
                                 sizeof szWorkDate, szWorkDate);

            WinQueryDlgItemText (hwnd, DID_PRIORITY,
                                 sizeof szPriority, szPriority);

            usRet = ItiFmtFormatString (szWorkDate,
                                        szTmp,
                                        sizeof szTmp,
                                        "DateTime,mm/dd/yyyy", NULL);
            if (usRet)
               {
               PJErr (hwnd, "Invalid Work Begin Date.", FALSE);
               return 0;
               }


            hwndList = WinWindowFromID(hwnd, ChProgJobL);

            if (hwndList == NULL)
               break;


            /* getting the JobKey from list in dialog box */

            usRow = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);

            if (!(hheapList = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0)))
               break;

            pszJobKey    = (PSZ)  QW (hwndList,  ITIWND_DATA, 0, usRow, cJobKey);
            hwndTemp     = (HWND) QW (hwnd,      ITIWND_OWNERWND, 0, 0, 0);
            hwndParent   = (HWND) QW (hwndTemp,  ITIWND_OWNERWND, 0, 0, 0);
            pszProgramKey= (PSZ)  QW (hwndParent,ITIWND_DATA, 0, 0, cProgramKey);

            pszDuration  = (PSZ)  QW (hwndList,  ITIWND_DATA, 0, usRow, cDuration);

            lJulDay = ItiFmtCalendarToJulian (szTmp);
            lJulDay += (atoi (pszDuration) ? atoi (pszDuration) : 1);
            ItiStrCpy (szTmp, ItiFmtJulianToCalendar (lJulDay), sizeof szTmp);

            ItiFmtFormatString (szTmp,
                                szWorkDate,
                                sizeof szWorkDate,
                                "DateTime,mmm d,yyyy", NULL);

            WinSetDlgItemText (hwnd, DID_WORKENDDATE, szWorkDate);

            InsertProgFunds(hheap, pszJobKey, pszProgramKey);

            if (mr = ItiWndDefDlgProc(hwnd, uMsg, mp1, mp2))
               return mr;

            DoProgramJobReCalc(hwnd, hwndList, pszJobKey, pszProgramKey);

            return mr;

            }
            break;

         }

      }
   return ItiWndDefDlgProc (hwnd, uMsg, mp1, mp2);
   }





MRESULT EXPORT AddProgJobDProc (HWND    hwnd,
                                   USHORT  uMsg,
                                   MPARAM  mp1,
                                   MPARAM  mp2)
   {
   switch (uMsg)
      {
      //Select params         hwnd, uCtl,           uCtlCol,           uQueryId, uQueryCol
      //==========================================================================================

      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, DID_INFLPRED,    cInflationPredictionKey, hwnd, 0);
         ItiWndActivateRow (hwnd, AddProgJobL,  cJobKey,                 hwnd, 0);
         WinEnableWindow (WinWindowFromID (hwnd, DID_WORKENDDATE+DID_STOFFSET), FALSE);
         WinSendDlgItemMsg (hwnd, DID_WORKENDDATE, EM_SETREADONLY, 
                            MPFROMSHORT (1), 0);
         break;

      case WM_COMMAND:

      switch (SHORT1FROMMP (mp1))
         {
         case DID_OK:
         case DID_ADDMORE:
            {
            MRESULT mr;
            HWND    hwndList, hwndParent, hwndTemp;
            HHEAP   hheap, hheapList;
            USHORT  usRow, usRet;
            PSZ     pszJobKey, pszProgramKey, pszDuration;
            CHAR    szWorkDate[50], szTmp[50];
            CHAR    szPriority[9];
            LONG    lJulDay;


            WinQueryDlgItemText (hwnd, DID_WORKBEGINDATE,
                                 sizeof szWorkDate, szWorkDate);

            WinQueryDlgItemText (hwnd, DID_PRIORITY,
                                 sizeof szPriority, szPriority);

            usRet = ItiFmtFormatString (szWorkDate,
                                        szTmp,
                                        sizeof szTmp,
                                        "DateTime,mm/dd/yyyy", NULL);
            if (usRet)
               {
               PJErr (hwnd, "Invalid Work Begin Date.", FALSE);
               return 0;
               }


            hwndList = WinWindowFromID(hwnd, AddProgJobL);

            if (hwndList == NULL)
               break;


            /* getting the JobKey from list in dialog box */

            usRow = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);

            hheapList = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0);

            if (hheapList == NULL)
               break;

            pszJobKey    = (PSZ)  QW (hwndList,  ITIWND_DATA, 0, usRow, cJobKey);
            hwndTemp     = (HWND) QW (hwnd,      ITIWND_OWNERWND, 0, 0, 0);
            hwndParent   = (HWND) QW (hwndTemp,  ITIWND_OWNERWND, 0, 0, 0);
            pszProgramKey= (PSZ)  QW (hwndParent,ITIWND_DATA, 0, 0, cProgramKey);

            pszDuration  = (PSZ)  QW (hwndList,  ITIWND_DATA, 0, usRow, cDuration);

            lJulDay = ItiFmtCalendarToJulian (szTmp);
            lJulDay += atoi (pszDuration);
            lJulDay += (atoi (pszDuration) ? atoi (pszDuration) : 1);
            ItiStrCpy (szTmp, ItiFmtJulianToCalendar (lJulDay), sizeof szTmp);

            ItiFmtFormatString (szTmp,
                                szWorkDate,
                                sizeof szWorkDate,
                                "DateTime,mmm d,yyyy", NULL);

            WinSetDlgItemText (hwnd, DID_WORKENDDATE, szWorkDate);

            InsertProgFunds(hheap, pszJobKey, pszProgramKey);

            if (mr = ItiWndDefDlgProc(hwnd, uMsg, mp1, mp2))
               return mr;

            DoProgramJobReCalc(hwnd, hwndList, pszJobKey, pszProgramKey);

            return mr;

            }
            break;

         }

      }

   return ItiWndDefDlgProc (hwnd, uMsg, mp1, mp2);

   }     /* end of AddProgJobDProc function */



static void SetEditButtons (HWND hwnd)
   {
   BOOL bEnable;
//   DosSleep (1000);
   bEnable = !!(UM)QW (WinWindowFromID (hwnd, ProgramJobSchPredL), ITIWND_NUMROWS, 0, 0, 0);
   WinEnableWindow (WinWindowFromID (hwnd, DID_CHANGE1), bEnable);
   WinEnableWindow (WinWindowFromID (hwnd, DID_DELETE1), bEnable);
   bEnable = !!(UM)QW (WinWindowFromID (hwnd, ProgramJobSchSuccL), ITIWND_NUMROWS, 0, 0, 0);
   WinEnableWindow (WinWindowFromID (hwnd, DID_CHANGE2), bEnable);
   WinEnableWindow (WinWindowFromID (hwnd, DID_DELETE2), bEnable);
   }

MRESULT EXPORT ProgramJobSchedDProc (HWND    hwnd,
                                     USHORT   uMsg,
                                     MPARAM   mp1,
                                     MPARAM   mp2)
   {
   switch (uMsg)
      {
      case WM_QUERYDONE:                                
         ItiWndActivateRow (hwnd, ProgramJobSchPredL, 0, NULL, 0);
         ItiWndActivateRow (hwnd, ProgramJobSchSuccL,  0, NULL, 0);
         break;

      case WM_CHILDQUERYDONE:                                
         SetEditButtons (hwnd);
         break;

      case WM_COMMAND:
         {
         switch (SHORT1FROMMP (mp1))
            {
            case DID_ADD1:
               ItiWndDoNextDialog (hwnd, ProgramJobSchPredL, ProgramJobPredAddD, ITI_ADD);
            //   SetEditButtons (hwnd);
               break;

            case DID_CHANGE1:
               ItiWndDoNextDialog (hwnd, ProgramJobSchPredL, ProgramJobPredChangeD, ITI_CHANGE);
               break;

            case DID_ADD2:
               ItiWndDoNextDialog (hwnd, ProgramJobSchSuccL, ProgramJobSuccAddD, ITI_ADD);
             //  SetEditButtons (hwnd);
               break;

            case DID_CHANGE2:
               ItiWndDoNextDialog (hwnd, ProgramJobSchSuccL, ProgramJobSuccChangeD, ITI_CHANGE);
               break;

            case DID_DELETE1:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, ProgramJobSchPredL), ITI_DELETE);
             //  SetEditButtons (hwnd);
               break;

            case DID_DELETE2:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, ProgramJobSchSuccL), ITI_DELETE);
             //  SetEditButtons (hwnd);
               break;
            }
         }

      }
   return ItiWndDefDlgProc (hwnd, uMsg, mp1, mp2);
   }






MRESULT EXPORT ProgramJobPredAddDProc (HWND   hwnd,
                                       USHORT uMsg,
                                       MPARAM mp1,
                                       MPARAM mp2)
   {
   switch (uMsg)
      {
      case WM_INITQUERY:
         {
         HWND   hwnd1, hwnd2;
         PSZ    pszJobKey;

         hwnd1 = QW(hwnd, ITIWND_OWNERWND, 3, 0, 0);
         hwnd2 = WinWindowFromID (hwnd1, ProgramJobL);
         pszJobKey = (PSZ) QW (hwnd2, ITIWND_DATA, 0, 0, cJobKey);
         break;
         }
      case WM_QUERYDONE:
         {
         /*--- delete default choice ---*/
         WinSendDlgItemMsg (hwnd, DID_LINK, LM_DELETEITEM, 0, 0);
         WinSendDlgItemMsg (hwnd, DID_LINK, LM_SELECTITEM, (MPARAM) 2, (MPARAM) TRUE);
//         ItiWndActivateRow (hwnd, ProgramJobPredSchAddL, cJobKey, NULL, cJobKey);
//         ItiWndActivateRow (hwnd, ProgramJobPredSchAddL, cWorkBeginDate, NULL, cWorkBeginDate);
         break;
         }
      }
   return ItiWndDefDlgProc (hwnd, uMsg, mp1, mp2);
   }


MRESULT EXPORT ProgramJobPredChangeDProc (HWND   hwnd,
                                          USHORT uMsg,
                                          MPARAM mp1,
                                          MPARAM mp2)
   {
   switch (uMsg)
      {
      case WM_QUERYDONE:
         {
         ItiWndActivateRow (hwnd, ProgramJobPredSchChgL, cJobKey, NULL, cJobKey);
         WinSendDlgItemMsg (hwnd, DID_LINK, LM_DELETEITEM, 0, 0);
         ItiWndActivateRow (hwnd, DID_LINK, cLink, hwnd, cLink);
//         ItiWndActivateRow (hwnd, ProgramJobPredSchAddL, cWorkBeginDate, NULL, cWorkBeginDate);
         break;
         }
      }
   return ItiWndDefDlgProc (hwnd, uMsg, mp1, mp2);
   }





MRESULT EXPORT ProgramJobSuccChangeDProc (HWND   hwnd,
                                          USHORT uMsg,
                                          MPARAM mp1,
                                          MPARAM mp2)
   {
   switch (uMsg)
      {
      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, ProgramJobSuccSchChgL, cJobKey, NULL, cJobKey);
         /*--- delete default choice ---*/
         WinSendDlgItemMsg (hwnd, DID_LINK, LM_DELETEITEM, 0, 0);
         ItiWndActivateRow (hwnd, DID_LINK, cLink, hwnd, cLink);
         break;
      }
   return ItiWndDefDlgProc (hwnd, uMsg, mp1, mp2);
   }





MRESULT EXPORT ProgramJobSuccAddDProc (HWND   hwnd,
                                       USHORT uMsg,
                                       MPARAM mp1,
                                       MPARAM mp2)
   {
   switch (uMsg)
      {
      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, ProgramJobSuccSchAddL, cJobKey, NULL, cJobKey);
         /*--- delete default choice ---*/
         WinSendDlgItemMsg (hwnd, DID_LINK, LM_DELETEITEM, 0, 0);
         WinSendDlgItemMsg (hwnd, DID_LINK, LM_SELECTITEM, (MPARAM) 2, (MPARAM) TRUE);
         break;
      }
   return ItiWndDefDlgProc (hwnd, uMsg, mp1, mp2);
   }


static BOOL InsertProgFunds (HHEAP hheap, PSZ pszJobKey, PSZ pszProgramKey)

   {
   char szTemp [300] = "";

   sprintf(szTemp, "INSERT INTO ProgramFund "
                   "(ProgramKey, FundKey, PercentFunded, AmountFunded) "
                   "SELECT %s, FundKey, 0, .00 "
                   "FROM JobFund "
                   "WHERE JobKey = %s "
                   "AND FundKey NOT IN "
                   "(SELECT FundKey "
                   "FROM ProgramFund "
                   "WHERE ProgramKey = %s)", pszProgramKey, pszJobKey, pszProgramKey); 

   if (ItiDbExecSQLStatement (hheap, szTemp))
      return FALSE;


   return TRUE;

   }
