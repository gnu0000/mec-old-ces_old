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
 * JobBreak.c
 * Mark Hampton
 * (C) 1991 AASHTO
 *
 * This module handles the Job Breakdown windows.
 */


#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\winid.h"
#include "..\include\colid.h"
#include "..\include\itiglob.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\dialog.h"
#include "..\include\itiutil.h"
#include "..\include\cesutil.h"
#include <stdio.h>
#include <string.h>
#include "bamsces.h"
#include "job.h"
#include "jobbreak.h"
#include "menu.h"
#include "dialog.h"
#include "calcfn.h"


/*
 * Window procedure procedure for Job Breakdown List window.
 */

MRESULT EXPORT JobBreakdownListSProc (HWND   hwnd,
                                      USHORT usMessage,
                                      MPARAM mp1,
                                      MPARAM mp2)
   {
   static BOOL bIsZero;

   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               {
               BOOL bSelected;

               bSelected = ItiWndQueryActive (hwnd, JobBreakdownL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               return 0;
               }

            case IDM_VIEW_MENU:
               {
            HBUF hbuf;
            USHORT uRows;
            HWND  hwndList;

            hwndList = WinWindowFromID (hwnd, JobBreakdownL);

            hbuf = (HBUF) QW (hwndList, ITIWND_BUFFER, 0, JobBreakdownL, 0);
            uRows = (UM) ItiDbQueryBuffer (hbuf, ITIDB_NUMROWS, 0);
            bIsZero = (uRows <= 0);

               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2), 
                                           IDM_VIEW_USER_FIRST);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "View Job ~Breakdown", 
                                      IDM_VIEW_USER_FIRST + 1);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "Job Breakdown ~Funds", 
                                      IDM_VIEW_USER_FIRST + 2);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "Job ~Items", 
                                      IDM_VIEW_USER_FIRST + 3);
               ItiMenuGrayMenuItem
                  (HWNDFROMMP (mp2), IDM_VIEW_USER_FIRST + 1, bIsZero);
               ItiMenuGrayMenuItem
                  (HWNDFROMMP (mp2), IDM_VIEW_USER_FIRST + 2, bIsZero);
               ItiMenuGrayMenuItem
                  (HWNDFROMMP (mp2), IDM_VIEW_USER_FIRST + 3, bIsZero);


        //       ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
        //                              "~Job Breakdown Profile", 
        //                              IDM_VIEW_USER_FIRST + 4);
               return 0;
               }
               break;

            case IDM_UTILITIES_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_FIND, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_SORT, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_UPDATE_BASE_DATE, 
                                    FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_UPDATE_SPEC_YEAR, 
                                    FALSE);
               return 0;
               break;
            }
            break;

      case WM_COMMAND:
         {
         HWND  hwndList;

         hwndList = WinWindowFromID (hwnd, JobBreakdownL);

         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, JobBreakdownL, FALSE);
               return 0;
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, JobBreakdownL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, JobBreakdownL), ITI_DELETE);
               DoJobEstimateRecalc (hwnd);
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               ItiWndBuildWindow (hwndList, JobBreakdownS);
               return 0;
               break;

            case (IDM_VIEW_USER_FIRST + 2):
               ItiWndBuildWindow (hwndList, JobBreakdownFundS);
               return 0;
               break;
            
            case (IDM_VIEW_USER_FIRST + 3):
               ItiWndBuildWindow (hwndList, JobItemsS);
               return 0;
               break;
      //
      //      case (IDM_VIEW_USER_FIRST + 4):
      //                                          /* In JobBreakdownListSProc */
      //         ItiWndBuildWindow (hwndList, JobBreakdownProfileS);
      //         return 0;
      //         break;
            }
         }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }





/*
 * Window procedure for the Job Breakdown window.
 */

MRESULT EXPORT JobBreakdownSProc (HWND   hwnd,
                                  USHORT usMessage,
                                  MPARAM mp1,
                                  MPARAM mp2)
   {
   static BOOL bIsNotParametric;

   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               {
// /// Metrication code
               PGLOBALS pglob;
               BOOL bMetric;
               PSZ  pszSys = NULL;

               pszSys = (PSZ) QW (hwnd, ITIWND_DATA, 0, 0, cUnitType);
               if (pszSys != NULL && (*pszSys) == '1')
                  bMetric = TRUE;
               else
                  bMetric = FALSE;

               pglob = ItiGlobQueryGlobalsPointer();
               if ( ((pglob->bUseMetric == TRUE) && (bMetric == TRUE))
                   || ((pglob->bUseMetric == FALSE) && (bMetric == FALSE)) )
                  {
                  ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, FALSE);
                  }
               else
                  {
                  ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, TRUE);

                  // Tell user why they can not edit job breakdown now.
                  WinMessageBox (pglobals->hwndDesktop, hwnd,
                     " The active Unit Type System is different from this job;"
                     " so the Job Breakdown can not be altered at this time.  From the"
                     " Utilities menu you can switch the active Unit Type System. ",
                     " Message Box",  0,
                     MB_OK | MB_SYSTEMMODAL | MB_ICONEXCLAMATION );
                  }

               return 0;
               }
               break;

            case IDM_VIEW_MENU:
               {
              // bIsNotParametric = (0 == ItiStrCmp("1",((PSZ) QW (hwnd, ITIWND_DATA, 0, 1, cDetailedEstimate)) ));

               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2), 
                                           IDM_VIEW_USER_FIRST);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "Job Breakdown ~Funds", 
                                      IDM_VIEW_USER_FIRST + 1);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "Job ~Items", 
                                      IDM_VIEW_USER_FIRST + 2);
               ItiMenuGrayMenuItem
                  (HWNDFROMMP (mp2), IDM_VIEW_USER_FIRST + 2, !bIsNotParametric);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "~Job Breakdown Profile", 
                                      IDM_VIEW_USER_FIRST + 3);
               ItiMenuGrayMenuItem
                  (HWNDFROMMP (mp2), IDM_VIEW_USER_FIRST + 3, bIsNotParametric);
               return 0;
               }
               break;

            case IDM_UTILITIES_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_UPDATE_BASE_DATE, 
                                    FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_UPDATE_SPEC_YEAR, 
                                    FALSE);
               return 0;
               break;
            }
            break;

      case WM_ITIDBBUFFERDONE:
         {
         HHEAP hheap;
         PSZ   pszStructure;
         PSZ   pszUnitType;
         BOOL  bHidden, bMetric;

         hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
         if (hheap == NULL)
            break;

         /* -- tlb 95MAR metric change labels. */
         pszUnitType = (PSZ) QW (hwnd, ITIWND_DATA, 0, 0, cUnitType);
         if (pszUnitType != NULL && (*pszUnitType) == '1')
            bMetric = TRUE;
         else
            bMetric = FALSE;

         if (bMetric == TRUE)
            {
            SW (hwnd, ITIWND_LABELHIDDEN, 2, TRUE, 0); // miles
            SW (hwnd, ITIWND_LABELHIDDEN, 5, TRUE, 0); // feet
            SW (hwnd, ITIWND_LABELHIDDEN, 7, TRUE, 0); // inches
            SW (hwnd, ITIWND_LABELHIDDEN, 19, FALSE, 0); // kilometers
            SW (hwnd, ITIWND_LABELHIDDEN, 20, FALSE, 0); // meters
            SW (hwnd, ITIWND_LABELHIDDEN, 21, FALSE, 0); // millimeters
            }
         else 
            {
            SW (hwnd, ITIWND_LABELHIDDEN, 2, FALSE, 0); // miles
            SW (hwnd, ITIWND_LABELHIDDEN, 5, FALSE, 0); // feet
            SW (hwnd, ITIWND_LABELHIDDEN, 7, FALSE, 0); // inches
            SW (hwnd, ITIWND_LABELHIDDEN, 19, TRUE, 0); // kilometers
            SW (hwnd, ITIWND_LABELHIDDEN, 20, TRUE, 0); // meters
            SW (hwnd, ITIWND_LABELHIDDEN, 21, TRUE, 0); // millimeters
            }
         ItiMemFree (hheap, pszUnitType);


         bIsNotParametric = (0 == ItiStrCmp("1",((PSZ) QW (hwnd, ITIWND_DATA, 0, 1, cDetailedEstimate)) ));

         pszStructure = (PSZ) QW (hwnd, ITIWND_DATA, 0, 0, cStructureID);

         bHidden = ((pszStructure != NULL) && 
             (*pszStructure == '\0' || (*pszStructure == '0' && *(pszStructure + 1) == '\0')) 
             || (*pszStructure == 'N' && *(pszStructure+1) == 'A' && *(pszStructure+2) == 'S' )
             || (*pszStructure == 'N' && *(pszStructure+1) == '/' && *(pszStructure+2) == 'A' )
            );

         ItiMemFree (hheap, pszStructure);

         /* hide/unhide  the structure information */
         SW (hwnd, ITIWND_LABELHIDDEN, 9,                bHidden, 0);// Structure:
         SW (hwnd, ITIWND_DATAHIDDEN,  cStructureID,     bHidden, 0);
         SW (hwnd, ITIWND_LABELHIDDEN, 10,               bHidden, 0);// Structure Length:
         SW (hwnd, ITIWND_LABELHIDDEN, 11,               bHidden, 0);// feet
         SW (hwnd, ITIWND_DATAHIDDEN,  cStructureLength, bHidden, 0);
         SW (hwnd, ITIWND_LABELHIDDEN, 12,               bHidden, 0);// Structure Width:
         SW (hwnd, ITIWND_LABELHIDDEN, 13,               bHidden, 0);// feet
         SW (hwnd, ITIWND_DATAHIDDEN,  cStructureWidth,  bHidden, 0);

         SW (hwnd, ITIWND_LABELHIDDEN, 17,               !bIsNotParametric, 0);
         SW (hwnd, ITIWND_LABELHIDDEN, 18,               bIsNotParametric, 0);

//         pszStructure = (PSZ) QW (hwnd, ITIWND_DATA, 0, 0, cDetailedEstimate);
//         if ((pszStructure) && (*pszStructure == '1'))
//            {
//            SW (hwnd, ITIWND_LABELHIDDEN, 15, TRUE, 0);
//            }
//         ItiMemFree (hheap, pszStructure);

//         SWL(hwnd, ITIWND_DATA, 17, "  X  ");
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
               ItiWndBuildWindow (hwnd, JobBreakdownFundS);
               return 0;
               break;
            
            case (IDM_VIEW_USER_FIRST + 2):
               ItiWndBuildWindow (hwnd, JobItemsS);
               return 0;
               break;

            case (IDM_VIEW_USER_FIRST + 3):
               {
                                            /* In JobBreakdownSProc */
               ItiWndBuildWindow (hwnd, JobBreakdownProfileS);
               return 0;
               }
               break;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }





static BOOL InsertInitialJobBreakdownStuff (HHEAP hheap, PSZ pszJobKey, PSZ pszJobBreakdownKey);



MRESULT EXPORT JobBreakdownDProc (HWND    hwnd,
                                  USHORT   usMessage,
                                  MPARAM   mp1,
                                  MPARAM   mp2)
   {
   static PSZ pszKey;

   switch (usMessage)
      {
      case WM_QUERYDONE:
         {
         if (ItiWndGetDlgMode () == ITI_ADD)
            {
            WinSendDlgItemMsg (hwnd, DID_ROADTYPE, LM_SELECTITEM, 
                               MPFROMSHORT (0), 
                               MPFROMSHORT (TRUE));
            WinSendDlgItemMsg (hwnd, DID_STRUCTTYPE, LM_SELECTITEM,
                               MPFROMSHORT (0), 
                               MPFROMSHORT (TRUE));
            WinSendDlgItemMsg (hwnd, DID_WORKTYPE, LM_SELECTITEM,
                               MPFROMSHORT (0), 
                               MPFROMSHORT (TRUE));
            WinSetDlgItemText (hwnd, DID_STRUCTID    , "0");
            WinSetDlgItemText (hwnd, DID_STRUCTLENGTH, "0");
            WinSetDlgItemText (hwnd, DID_STRUCTWIDTH , "0");
            }
         else
            {
            ItiWndActivateRow (hwnd, DID_ROADTYPE,   cCodeValueKey, hwnd, cRoadType);
            ItiWndActivateRow (hwnd, DID_STRUCTTYPE, cCodeValueKey, hwnd, cStructureType);
            ItiWndActivateRow (hwnd, DID_WORKTYPE,   cCodeValueKey, hwnd, cWorkType);
            }
         break;
         }

      case WM_NEWKEY:
         {
         HHEAP hheap;

         if (ItiStrICmp ((PSZ) mp1, "JobBreakdown") == 0)
            {
            hheap= (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);

            pszKey = ItiStrDup (hheap, (PSZ) mp2);
            }
         return 0;
         break;
         }

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
            case DID_ADDMORE:
               {
               MRESULT  mr;
               PSZ      pszJobKey;
               HWND     hwndParent;
               HHEAP    hheap, hheapParent;

               pszKey = NULL;

               if (mr = ItiWndDefDlgProc(hwnd, usMessage, mp1, mp2))
                  return mr;

               if (pszKey != NULL)
                  {
                  hwndParent  = (HWND)  QW (hwnd, ITIWND_OWNERWND, 0, 0, 0);
                  hheap       = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
                  hheapParent = (HHEAP) QW (hwndParent, ITIWND_HEAP, 0, 0, 0);
                  pszJobKey   = (PSZ)   QW (hwndParent, ITIWND_DATA, 0, 0, cJobKey);

                  InsertInitialJobBreakdownStuff (hheap, pszJobKey, pszKey);

                  ItiMemFree (hheapParent, pszJobKey);
                  ItiMemFree (hheap, pszKey);
                  }
               else
                  ItiDbUpdateBuffers ("Job");

               return mr;
               }
               break;
            }
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }




MRESULT EXPORT JobBreakdownFundSProc (HWND     hwnd, 
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

               bSelected = ItiWndQueryActive (hwnd, JobBreakdownFundL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               return 0;
               }
            }

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
               {
               HWND hwndList;
               PSZ    pszFundKey;
               USHORT uRow;

               hwndList = WinWindowFromID (hwnd, JobBreakdownFundL);

               /* the user is not allowed to change the Fund with FundKey = 1 */

               uRow = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);               
               pszFundKey = (PSZ) QW (hwndList, ITIWND_DATA, 0, uRow, cFundKey);

               if ((pszFundKey == NULL) || ((*pszFundKey == '1') && (*(pszFundKey+1) == '\0')))                                           /* ****** */
                  return 0;                                                   


               ItiWndDoDialog (hwnd, JobBreakdownFundL, FALSE);

               DoJobBreakdownFundReCalc(hwndList, hwndList);
               break;
               }

            case IDM_ADD:
               {

               ItiWndDoDialog (hwnd, JobBreakdownFundL, TRUE);
               break;
               }

            case IDM_DELETE:
               {
               HWND     hwndList;
               HHEAP    hheapKeys;
               PSZ      *ppszSelKeys, pszFundKey;
               USHORT   uKeys [] = {cJobKey, cJobBreakdownKey, cFundKey, 0};
               USHORT   uRet, uRow;



               hwndList = WinWindowFromID(hwnd, JobBreakdownFundL);

               /* the user is not allowed to delete the Fund with FundKey = 1 */

               uRow = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);                 
               pszFundKey = (PSZ) QW (hwndList, ITIWND_DATA, 0, uRow, cFundKey);  

               if ((pszFundKey == NULL) || ((*pszFundKey == '1') && (*(pszFundKey+1) == '\0')))
                  return 0;                                                   


               hheapKeys = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0);

               ItiDbBuildSelectedKeyArray(hwndList, hheapKeys, &ppszSelKeys, uKeys);
               uRet = ItiWndAutoModifyDb (WinWindowFromID (hwnd, JobBreakdownFundL), ITI_DELETE);
               if (uRet != 666)                               /* User canceled the delete */
                  {

                  DoDelFundReCalc (hwndList, hheapKeys, ppszSelKeys);
                  }
               return 0;
               break;
               }

            }   
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }




MRESULT EXPORT ChJobBreakFundDProc (HWND hwnd,
                                    USHORT  usMessage,
                                    MPARAM  mp1,
                                    MPARAM  mp2)
   {
  return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }


/*
 * This function inserts a row in the JobFund table 
 * When the user chooses to add a new JobBreakdownFund, (i.e. a new fund from
 * the Fund Catalog list) and this fund is not currently in the JobFund 
 * table, this insertion is made.
 */

static BOOL UpDateJobFund (HHEAP hheap, PSZ pszJobKey, PSZ pszJobBreakdownKey,
                           PSZ pszFundKey);

/*
 * This function inserts a row in the ProgramFund table if the Job is associated with a program. 
 * When the user chooses to add a new JobBreakdownFund, (i.e. a new fund from
 * the Fund Catalog list) and this fund is not currently in the ProgramFund 
 * table, this insertion is made.
 */


static BOOL UpDateProgramFund (HHEAP hheap, PSZ pszJobKey, PSZ pszJobBreakdownKey,
                               PSZ pszFundKey, PSZ pszProgramFund);



MRESULT EXPORT AddJobBreakFundDProc (HWND    hwnd,
                                     USHORT  usMessage,
                                     MPARAM  mp1,
                                     MPARAM  mp2)
   
   {
   switch (usMessage)
      {
      case WM_ITILWMOUSE:
         if (SHORT1FROMMP (mp2) == JobBreakFundL &&
             ItiWndIsSingleClick (mp2) && ItiWndIsLeftClick (mp2))
            {
            PSZ  pszTemp;
            HWND hwndList;
            HHEAP hheap;
            USHORT usRow;

            hwndList = WinWindowFromID (hwnd, JobBreakFundL);
            if (hwndList == NULL)
               break;

            usRow = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);

            hheap = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0);
            if (hheap == NULL)
               break;

            pszTemp = (PSZ) QW (hwndList, ITIWND_DATA, 0, usRow, cFundID);
            if (pszTemp != NULL)
               {
               WinSetDlgItemText (hwnd, DID_FUNDID, pszTemp);
               ItiMemFree (hheap, pszTemp);
               }
            }
         break;
         
     
      case WM_COMMAND:

      switch (SHORT1FROMMP (mp1))
         {
         case DID_OK:
         case DID_ADDMORE:
            {
            MRESULT mr;
            HHEAP   hheap, hheapStatic;
            HWND    hwndList, hwndStatic;
            PSZ     pszJobKey, pszJobBreakdownKey, pszFundKey, pszProgramKey;
            char    szTmp2[150] = "";
            USHORT  usRow;


            if (mr = ItiWndDefDlgProc(hwnd, usMessage, mp1, mp2))
               return mr;

            hwndList = WinWindowFromID(hwnd, JobBreakFundL);

            if (hwndList == NULL)
               break;

            usRow = (UM)    QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);
            if (!(hheap = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0)))
               break;

            /* getting the FundKey from list in dialog box */
            pszFundKey  = (PSZ)   QW (hwndList,   ITIWND_DATA, 0, usRow, cFundKey);
            hwndStatic  = (HWND)  QW (hwnd,       ITIWND_OWNERWND, 1, 0, 0);
            hheapStatic = (HHEAP) QW (hwndStatic, ITIWND_HEAP, 0, 0, 0);

            if (hheapStatic == NULL)
               break;

            pszJobKey = (PSZ) QW (hwndStatic, ITIWND_DATA, 0, 0, cJobKey);
            pszJobBreakdownKey = (PSZ) QW (hwndStatic, ITIWND_DATA, 0, 0, cJobBreakdownKey);

            UpDateJobFund(hheap, pszJobKey, pszJobBreakdownKey, pszFundKey );

            /* Determines whether the Job is currently in a Program */

            sprintf(szTmp2,"SELECT ProgramKey "
                           "FROM ProgramJob "
                           "WHERE JobKey = %s", pszJobKey);


            pszProgramKey = ItiDbGetRow1Col1(szTmp2, hheap, 0, 0, 0);

            if (pszProgramKey != NULL)
               {
                UpDateProgramFund(hheap, pszJobKey, pszJobBreakdownKey, pszFundKey, pszProgramKey);
               }

            DoJobBreakdownFundReCalc(hwnd, hwndList);

            ItiMemFree (hheap, pszFundKey);
            ItiMemFree (hheap, pszProgramKey);
            ItiMemFree (hheapStatic, pszJobKey);
            ItiMemFree (hheapStatic, pszJobBreakdownKey);

            return mr;
         }
         break;

       }
    }

   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }




static BOOL UpDateJobFund (HHEAP hheap, 
                           PSZ   pszJobKey, 
                           PSZ   pszJobBreakdownKey,
                           PSZ   pszFundKey)

   {
   char szTemp [400] = "";

   sprintf (szTemp, "INSERT INTO JobFund "
                    "(JobKey, FundKey, PercentFunded, AmountFunded) "
                    "SELECT JobKey, FundKey, 0, 0 "
                    "FROM JobBreakdownFund "
                    "WHERE JobKey = %s "
                    "AND JobBreakdownKey = %s "
                    "AND FundKey = %s "
                    "AND FundKey NOT IN "
                    "(SELECT FundKey "
                    "FROM JobFund "
                    "WHERE FundKey = %s "
                    "AND JobKey = %s)", pszJobKey, pszJobBreakdownKey, pszFundKey, pszFundKey, pszJobKey);

   if (ItiDbExecSQLStatement (hheap, szTemp))
      return FALSE;

   return TRUE;

   }
      
             

static BOOL UpDateProgramFund (HHEAP   hheap, 
                               PSZ     pszJobKey, 
                               PSZ     pszJobBreakdownKey,
                               PSZ     pszFundKey, 
                               PSZ     pszProgramKey)
   {
   char szTemp [400] = "";


   sprintf (szTemp, "INSERT INTO ProgramFund "
                    "(ProgramKey, FundKey, PercentFunded, AmountFunded) "
                    "SELECT %s, FundKey, 0, 0 "
                    "FROM JobFund "
                    "WHERE JobKey = %s "
                    "AND FundKey = %s "
                    "AND FundKey NOT IN "
                    "(SELECT FundKey "
                    "FROM ProgramFund "
                    "WHERE FundKey = %s "
                    "AND ProgramKey = %s)", pszProgramKey, pszJobKey, pszFundKey, pszFundKey, pszProgramKey); 

   if (ItiDbExecSQLStatement (hheap, szTemp))
      return FALSE;


   return TRUE;

   }





/*                                                                  
 * This function inserts the initial JobBreakdownFund (UNF) when  
 * a new JobBreakdown is added by the user
 *
 */                                                                

static BOOL InsertInitialJobBreakdownStuff (HHEAP   hheap, 
                                           PSZ     pszJobKey, 
                                           PSZ     pszJobBreakdownKey)

   {
   char szTemp [600] = "";

   sprintf (szTemp, "INSERT INTO JobBreakdownFund "
                    "(JobKey,JobBreakdownKey,FundKey,PercentFunded,AmountFunded) "
                    "VALUES "
                    "(%s,%s,1,1.00,0) ", pszJobKey, pszJobBreakdownKey);

   ItiDbExecSQLStatement (hheap, szTemp);
      
   sprintf (szTemp, "INSERT INTO JobBreakdownProfile "
                    "(JobKey,JobBreakdownKey,ProfileKey) "
                    "VALUES "
                    "(%s,%s,%s)",
                    pszJobKey, pszJobBreakdownKey, DEFAULT_PROFILEKEY_STR);

   ItiDbExecSQLStatement (hheap, szTemp);

   sprintf (szTemp, "INSERT INTO JobBreakdownMajorItem "
                    "(JobKey, JobBreakdownKey, MajorItemKey, Quantity, "
                    "QuantityLock,UnitPrice,UnitPriceLock,ExtendedAmount) "
                    "VALUES "
                    "(%s,%s,1,0,0,0,0,0)",
                    pszJobKey, pszJobBreakdownKey);

   ItiDbExecSQLStatement (hheap, szTemp);

   return TRUE;
   }



/*
 *
 * This function obtains the selected keys from the window, and is
 * used for multi-selected deletes
 *
 */




