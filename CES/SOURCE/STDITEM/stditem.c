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
 * StdItem.c
 * Mark Hampton
 *
 * This module provides the Standard Item catalog window.
 */


#define     INCL_WIN
#define     INCL_DOSMISC 

#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimbase.h"
#include "..\include\itibase.h"
#include "..\include\winid.h"
#include "..\include\colid.h"
#include "..\include\itiutil.h"
#include "..\include\itimenu.h"
#include "..\include\itiimp.h"
#include "..\include\itiest.h"
#include "..\include\itidbase.h"
#include "..\include\itierror.h"
#include "..\include\itiglob.h"
#include "stditem.h"
#include "initcat.h"
#include "dialog.h"

#include <stdio.h>
#include <stdlib.h>

static CHAR szStar[10] = "-\\|/+   ";
static CHAR szDllVersion[] = "1.1b4 StdItem.DLL";
                          //   1.1b_ The underscore means final release version.


static CHAR szItemNumber      [16] = "";
static CHAR szParentStdItmKey [16] = "";
static CHAR szLinkStdItmKey   [16] = "";
static PSZ  pszBeep = NULL;
static BOOL bBeep = FALSE;
//static BOOL bLogDups = FALSE;
static BOOL bPurgeDone = FALSE;
//static PSZ  pszTestImpEnv = NULL;
//static BOOL bImpTstFlag = FALSE;

#define ONE_THOU    2000

static CHAR szQueryAPDP     [ONE_THOU + 1] = "";
static CHAR szQuerySIU      [ONE_THOU + 1] = "";
static CHAR szTempSIU       [ONE_THOU + 1] = "";
static CHAR szICQuery       [ONE_THOU + 1] = "";
static CHAR szICTemp        [ONE_THOU + 1] = "";
static CHAR szICUpdateDups  [ONE_THOU + 1] = "";

// static CHAR  szSetMarker[] =
//         "INSERT INTO StandardItem "
//         "(StandardItemKey, StandardItemNumber, SpecYear, "
//         "ShortDescription, LongDescription, Unit, LumpSum, "
//         "DefaultProductionRate, DefaultStartTime, RoundingPrecision "
//         ", UnitType , Deleted) VALUES "
//         "(1000, '0',1900,'Marker','X',1,0,0,0,0,NULL,1) ";



/*
 * ItiDllQueryVersion returns the version number of this DLL.
 */

USHORT EXPORT ItiDllQueryVersion (void)
   {
   return VERSION;
   }





/*
 * ItiDllQueryCompatibility returns TRUE if this DLL is compatable
 * with the caller's version numer, otherwise FALSE is returned.
 */

BOOL EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion)

   {
   if ( ItiVerCmp(szDllVersion) )
      return TRUE;

   return FALSE;
   }



VOID EXPORT ItiDllQueryMenuName (PSZ      pszBuffer,
                                 USHORT   usMaxSize,
                                 PUSHORT  pusWindowID)
   {
   ItiStrCpy (pszBuffer, "~Standard Item Catalog", usMaxSize);
   *pusWindowID = StandardItemCatS;
   }


BOOL EXPORT DoStdItemAddPredD (HWND hwnd)
   {
   USHORT i;

   i = (UM) ItiWndBuildWindow (WinWindowFromID (hwnd,StanItemD), StdItemAddPredD);

   return i == 1;
   }


MRESULT EXPORT StanItemDProc (HWND    hwnd,
                              USHORT   usMessage,
                              MPARAM   mp1,
                              MPARAM   mp2)
   {
   static CHAR szStdItmDKey [16] = "";
   static MRESULT  mr;
   BOOL bUseMR = FALSE;
   HWND  hwndList;

   switch (usMessage)
      {
      case WM_ITIDBBUFFERDONE:
         hwndList = (HWND) QW (hwnd, ITIWND_OWNERWND, 1, 0, 0);
         break;

      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, DID_UNIT,      cCodeValueKey,      hwnd, cUnit);
         ItiWndActivateRow (hwnd, DID_PRECISION, cRoundingPrecision, hwnd, 0);
         ItiWndActivateRow (hwnd, DID_SPECYEAR,  cSpecYear,          hwnd, cSpecYear);
         break;

      case WM_COMMAND:
         {
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
            case DID_ADDMORE:
               {
               //static MRESULT  mr;
               static CHAR szMaxKeyQry[60] =
                           " select MAX(StandardItemKey) from StandardItem ";
               static CHAR szSetQry[250] = "";
               static CHAR szOKQry[] =
                            " /* StanItemDProc */ "
                            " select UnitType from CodeValue "
                               " where CodeValueKey = "
                               " (SELECT Unit from StandardItem "
                                " WHERE StandardItemKey = "
                                 " (select MAX(StandardItemKey) "
                                   "from StandardItem ) )";
               HHEAP hhpOK;
               PSZ   pszRes, pszKey;

               ItiStrCpy (szSetQry,
                          " /* StanItemDProc 0197 */ "
                          " UPDATE StandardItem SET UnitType = "
                          ,sizeof szSetQry);

               if (ItiWndGetDlgMode() == ITI_ADD)
                  {
                  mr = ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
                  bUseMR =TRUE;

                  /* -- This section is done only for a new insert into
                     -- the catalog.  The new entry's UnitType is
                     -- set to match the UnitType of the unit code
                     -- that was chosen.
                  */
                  hhpOK = ItiMemCreateHeap (500);
                  pszKey = ItiDbGetRow1Col1(szMaxKeyQry, hhpOK, 0, 0, 0);
                  pszRes = ItiDbGetRow1Col1(szOKQry, hhpOK, 0, 0, 0);
                  if ((pszRes != NULL)
                      && ( (*pszRes) == '0' || (*pszRes) == '1' )  )
                     ItiStrCat (szSetQry, pszRes, sizeof szSetQry);
                  else
                     ItiStrCat (szSetQry, " NULL ", sizeof szSetQry);

                  ItiStrCat (szSetQry, " where StandardItemKey = ", sizeof szSetQry);
                  ItiStrCat (szSetQry, pszKey, sizeof szSetQry);
                  ItiDbExecSQLStatement (hhpOK, szSetQry);
                  ItiMemDestroyHeap (hhpOK);
                  //return mr;
                  }/* end if */
               }
               break;

            case DID_CANCEL:   /* CANCEL button pushed */
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;

            case DID_ADDEP:
               {
               USHORT uRowSel;
               PSZ pszKey;

               uRowSel = (UM) QW (hwnd, ITIWND_ACTIVE, 0, 0, 0);
               pszKey = (PSZ) QW (hwnd, ITIWND_DATA, 0, uRowSel, cStandardItemKey);
               if (pszKey != NULL)
                  {
                  ItiStrCpy (szParentStdItmKey, pszKey, sizeof szParentStdItmKey);
         ItiErrWriteDebugMessage("This is the key set as the parent:");
         ItiErrWriteDebugMessage(szParentStdItmKey);
                  }
               else
         ItiErrWriteDebugMessage("NULL key set as the parent.");


               DoStdItemAddPredD (hwnd);
             //  ItiWndDoNextDialog (hwnd, 0, StdItemAddPredD, ITI_ADD);
               }
               break;

            case DID_DEDEP:
               ItiWndDoNextDialog (hwnd, 0, StdItemDelPredD, ITI_DELETE);
               break;

            }
         }
         break;
      }

   if (bUseMR)
      {
      return mr;
      bUseMR = FALSE;
      }
   else
      return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }




MRESULT EXPORT StdItemAddPredDProc (HWND     hwnd,
                                    USHORT   usMessage,
                                    MPARAM   mp1,
                                    MPARAM   mp2)
   {
   static bIsPred = FALSE;
   static bIsSucc = FALSE;

   switch (usMessage)
      {
      case WM_QUERYDONE:
        // ItiWndActivateRow (hwnd, StandardItemCatL, cStandardItemKey,  hwnd, 0);
         break;

      case WM_ITILWMOUSE:
         if (SHORT1FROMMP (mp2) == StandardItemCatL &&
             ItiWndIsSingleClick (mp2) && ItiWndIsLeftClick (mp2))
            {
            PSZ  pszTemp;
            HWND hwndList;
            HHEAP hheap;
            USHORT uRow;

            hwndList = WinWindowFromID (hwnd, StandardItemCatL);
            if (hwndList == NULL)
               break;
            
            uRow = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);

            hheap = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0);
            if (hheap == NULL)
               break;

            pszTemp = (PSZ) QW (hwndList, ITIWND_DATA, 0, uRow, cStandardItemNumber);
            if (pszTemp != NULL)
               {
               WinSetDlgItemText (hwnd, DID_ITEMNUMBER, pszTemp);
               ItiStrCpy (szItemNumber, pszTemp, sizeof szItemNumber);
               ItiMemFree (hheap, pszTemp);
               }

            pszTemp = (PSZ) QW (hwndList, ITIWND_DATA, 0, uRow, cStandardItemKey);
            if (pszTemp != NULL)
               {
               ItiStrCpy (szLinkStdItmKey, pszTemp, sizeof szLinkStdItmKey);
               ItiMemFree (hheap, pszTemp);
               }
            else
               szLinkStdItmKey[0] = '\0';

            }
         break;



      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               {
               HHEAP hhpOK;
               //HBUF  hbuf;
               //HWND  hwndParent;
               USHORT usRow;
               CHAR  szLag[8] = "0";
               CHAR  szLink[32] = "X";
               // CHAR  szQueryAPDP[1024] = "";
               PSZ   pszLinkStdItmKey = NULL;
               PSZ   pszParentStdItmKey = NULL;

         ItiErrWriteDebugMessage("Inside StdItemAddPredDProc");

               bIsPred = (UM) WinSendDlgItemMsg
                                 (hwnd, DID_PRED, BM_QUERYCHECK, 0, 0);
               bIsSucc = (UM) WinSendDlgItemMsg
                                 (hwnd, DID_SUCC, BM_QUERYCHECK, 0, 0);

               WinQueryDlgItemText (hwnd, DID_LAG, sizeof szLag, szLag);
               /* check for number only. */
               if (FALSE == ItiStrToUSHORT (szLag, &usRow))
                  {
                  szLag[0] = '1';
                  szLag[1] = '\0';
                  break;
                  }

               WinQueryDlgItemText (hwnd, DID_LINK, sizeof szLink, szLink);

         ItiErrWriteDebugMessage("Is this the link StdItemKey used:");
         ItiErrWriteDebugMessage(szLinkStdItmKey);
         ItiErrWriteDebugMessage("Now this is the given Parent StdItemKey to use:");
         ItiErrWriteDebugMessage(szParentStdItmKey);
         ItiErrWriteDebugMessage("Lag given is:");
         ItiErrWriteDebugMessage(szLag);
         ItiErrWriteDebugMessage("Link selected is:");
         ItiErrWriteDebugMessage(szLink);

               if ((szLink[0] == 'S') && (szLink[9] == 'S'))
                  {
                  szLink[0] = '1'; /* -- Start-to-Start  */
                  }
               else if ((szLink[0] == 'S') && (szLink[9] == 'F'))
                  {
                  szLink[0] = '2'; /* -- Start-to-Finish  */
                  }
               else if ((szLink[0] == 'F') && (szLink[10] == 'S'))
                  {
                  szLink[0] = '3'; /* -- Finish-to-Start  */
                  }
               else if ((szLink[0] == 'F') && (szLink[9] == 'F'))
                  {
                  szLink[0] = '4'; /* -- Finish-to-Finish */
                  }
               else
                  szLink[0] = '0';

               szLink[1] = '\0';

         ItiErrWriteDebugMessage("Link code is:");
         ItiErrWriteDebugMessage(szLink);

               /* -- Prep StdItmDependency insert statements here! */
               if (bIsPred)
                  {
                  sprintf (szQueryAPDP,
                           "INSERT INTO StandardItemDependency"
                           " (PredecessorItemKey,SuccessorItemKey,Link,Lag)"
                           " VALUES (%s,%s,%s,%s) ",
                           szLinkStdItmKey, szParentStdItmKey, szLink, szLag);
                  }
               else /* is a successor */
                  {
                  sprintf (szQueryAPDP,
                           "INSERT INTO StandardItemDependency"
                           " (PredecessorItemKey,SuccessorItemKey,Link,Lag)"
                           " VALUES (%s,%s,%s,%s) ",
                           szParentStdItmKey, szLinkStdItmKey, szLink, szLag);
                  }

               hhpOK = ItiMemCreateHeap (1024);
               ItiDbExecSQLStatement (hhpOK, szQueryAPDP);
               ItiMemDestroyHeap (hhpOK);
               }
               break;

            case DID_CANCEL:   /* CANCEL button pushed */
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;

            default:
               break;
            }
         break;
      } /* end of switch (usMessage... */

   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }/* End of Function StdItemAddPredDProc */



MRESULT EXPORT StdItemDelPredDProc (HWND     hwnd,
                                         USHORT   usMessage,
                                         MPARAM   mp1,
                                         MPARAM   mp2)
   {
   MRESULT  mrD;

   mrD = ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);

   return mrD;
   }/* End of Function StdItemDelPredDProc */





MRESULT EXPORT StandardItemCatSProc (HWND     hwnd,
                                     USHORT   usMessage,
                                     MPARAM   mp1,
                                     MPARAM   mp2)
   {
   static BOOL bMenuSet = FALSE;

   switch (usMessage)
      {
      case WM_INITQUERY:
         break;
      /* --- mp1 = parent hbuf               --- */
      /* --- mp2l=parent row                 --- */
      /* --- mp2h=no autostart children querys? --- */
//      case WM_INITQUERY: 
//         {
//         HWND     hwndL1;
//
//         /* --- manually init normal list window --- */
//         hwndL1 = WinWindowFromID (hwnd, StdCatL??);
//         WinSendMsg (hwndL1, WM_INITQUERY, mp1, mp2);
//
//         /* --- other list window isn't yet ready--- */
//
//         /* --- turn off auto child init --- */
//         mp2 = MPFROM2SHORT (SHORT1FROMMP (mp2), TRUE);
//
//         /* --- allow msg to continue slightly modified --- */
//         return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
//         }  /* end case wm_initquery */


      case WM_SHOW:
         usActiveWindow = StandardItemCatL;

         WinSetWindowPos (WinWindowFromID (hwnd, StdItemDefUPL),
                          NULL, 0, 0, 0, 0, SWP_HIDE);
         WinSetWindowPos (WinWindowFromID (hwnd, StdItemCBEstL),
                          NULL, 0, 0, 0, 0, SWP_HIDE);
         WinSetWindowPos (WinWindowFromID (hwnd, StdItemPemethAvgL),
                          NULL, 0, 0, 0, 0, SWP_HIDE);
         WinSetWindowPos (WinWindowFromID (hwnd, StdItemPemethRegL),
                          NULL, 0, 0, 0, 0, SWP_HIDE);
         break;

      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               {
               BOOL bSelected = FALSE;

               bSelected = ItiWndQueryActive (hwnd, usActiveWindow, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);

               if (bMenuSet == FALSE)
                  {
                  ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2), IDM_EDIT_USER_FIRST);
                  ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "Purge Deleted Items",
                                      IDM_EDIT_USER_FIRST + 1);
                  bMenuSet = TRUE;
                  }
               //return 0;
               }
               break;

            case IDM_VIEW_MENU:
               {
               HHEAP hheap;
               HWND  hwndL;
               PSZ   pszSIK;
               USHORT usActive;
               BOOL  bMakeGrey = FALSE;
               HWND  hwndParent, hwndTitleBar;
               char  szTtl[80] = "";

               hwndL = WinWindowFromID (hwnd, usActiveWindow);
               hheap = QW (hwndL, ITIWND_HEAP, 0, 0, 0);
               usActive = (UM) QW (hwndL, ITIWND_ACTIVE, 0, 0, 0);
               pszSIK = QW (hwndL, ITIWND_DATA, 0, usActive, cStandardItemKey);

               hwndParent = WinQueryWindow (hwnd, QW_PARENT, 0),
               hwndTitleBar = WinWindowFromID(hwndParent, FID_TITLEBAR);

               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2),
                                           IDM_VIEW_USER_FIRST);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Standard Item",
                                      IDM_VIEW_USER_FIRST + 1);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Cost Based Estimates",
                                      IDM_VIEW_USER_FIRST + 2);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Default Unit Price",
                                      IDM_VIEW_USER_FIRST + 3);
               bMakeGrey = !ItiEstQueryMethod (hheap, pszSIK, NULL, "Default");
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2),
                                    IDM_VIEW_USER_FIRST + 3, bMakeGrey);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "PEMETH ~Average",
                                      IDM_VIEW_USER_FIRST + 4);
               bMakeGrey =  !ItiEstQueryMethod (hheap, pszSIK, NULL, "Average");
               WinQueryWindowText(hwndTitleBar, sizeof szTtl, szTtl);
               if  (szTtl[31] == 'R')
                 bMakeGrey = TRUE;
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2),
                                    IDM_VIEW_USER_FIRST + 4, bMakeGrey);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "PEMETH ~Regression",
                                      IDM_VIEW_USER_FIRST + 5);
               bMakeGrey =  !ItiEstQueryMethod (hheap, pszSIK, NULL, "Regress");
               WinQueryWindowText(hwndTitleBar, sizeof szTtl, szTtl);
               if  (szTtl[31] == 'A')
                 bMakeGrey = TRUE;
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2),
                                    IDM_VIEW_USER_FIRST + 5, bMakeGrey);

               ItiMemFree (hheap, pszSIK);

               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2),
                                           IDM_VIEW_USER_FIRST + 6);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "Cost Based ~Only",
                                      IDM_VIEW_USER_FIRST + 7);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "Default Unit Price O~nly",
                                      IDM_VIEW_USER_FIRST + 8);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "PEMETH Average On~ly",
                                      IDM_VIEW_USER_FIRST + 9);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "PEMETH Regression Onl~y",
                                      IDM_VIEW_USER_FIRST + 10);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~View All Items",
                                      IDM_VIEW_USER_FIRST + 11);

               }
               break;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild, hwndParent, hwndTitleBar;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, usActiveWindow, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, usActiveWindow, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, usActiveWindow), ITI_DELETE);
               break;

            case (IDM_EDIT_USER_FIRST + 1):
               {  /* This will really delete ALL items flagged as deleted. */
               HHEAP hhpZ;
               INT xi;
               PGLOBALS pglobSI = NULL;

               pglobSI = ItiGlobQueryGlobalsPointer();
               hhpZ = ItiMemCreateHeap (1020);

               if (!bPurgeDone  &&
                   MBID_YES != WinMessageBox (pglobSI->hwndDesktop, hwnd,
                         "About to remove all items flagged as deleted; "
                         " this will take several minutes, continue?"
                         ,pglobSI->pszAppName, 0, 
                         MB_YESNO | MB_MOVEABLE | MB_ICONEXCLAMATION))
                  {
                  break;
                  }

               if (bPurgeDone  &&
                   MBID_NO == WinMessageBox (pglobSI->hwndDesktop, hwnd,
                         "All items flagged as deleted have already been purged "
                         "in this sesion, do you want to do it again???"
                         ,pglobSI->pszAppName, 0, 
                         MB_YESNO | MB_MOVEABLE | MB_ICONEXCLAMATION))
                  {
                  break;
                  }

              ItiWndSetHourGlass (TRUE);
              DosBeep(10000, 29);
              ItiErrWriteDebugMessage("\n     *** PURGE of all items flagged as deleted.\n");

   ItiDbExecSQLStatement (hhpZ, 
 " DELETE from WorkTypeAdjustment"
 " WHERE StandardItemKey IN (select distinct StandardItemKey from StandardItem "
 " where Deleted != NULL AND StandardItemKey > 1000000"
 " AND StandardItemKey NOT IN (select distinct StandardItemKey from JobItem)"
 " AND StandardItemKey NOT IN (select distinct StandardItemKey from ProjectItem)"
   " )" );
              ItiWndSetHourGlass (FALSE);
              if (bBeep)
                 for (xi = 1; xi < 12; xi++)
                    DosBeep(10000, 3);
              ItiWndSetHourGlass (TRUE);

   ItiDbExecSQLStatement (hhpZ, 
 " DELETE from WeightedAverageUnitPrice"
 " WHERE StandardItemKey IN (select distinct StandardItemKey from StandardItem "
 " where Deleted != NULL AND StandardItemKey > 1000000"
 " AND StandardItemKey NOT IN (select distinct StandardItemKey from JobItem)"
 " AND StandardItemKey NOT IN (select distinct StandardItemKey from ProjectItem)"
   " )" );
              ItiWndSetHourGlass (FALSE);
              if (bBeep)
                 for (xi = 1; xi < 11; xi++)
                    DosBeep(10000, 3);
              ItiWndSetHourGlass (TRUE);

   ItiDbExecSQLStatement (hhpZ, 
 " DELETE from StdItemDefaultPrices"
 " WHERE StandardItemKey IN (select distinct StandardItemKey from StandardItem "
 " where Deleted != NULL AND StandardItemKey > 1000000"
 " AND StandardItemKey NOT IN (select distinct StandardItemKey from JobItem)"
 " AND StandardItemKey NOT IN (select distinct StandardItemKey from ProjectItem)"
   " )" );
              ItiWndSetHourGlass (FALSE);
              if (bBeep)
                 for (xi = 1; xi < 10; xi++)
                    DosBeep(10000, 3);
              ItiWndSetHourGlass (TRUE);

   ItiDbExecSQLStatement (hhpZ, 
 " DELETE from StandardItemPEMETHRegression"
 " WHERE StandardItemKey IN (select distinct StandardItemKey from StandardItem "
 " where Deleted != NULL AND StandardItemKey > 1000000"
 " AND StandardItemKey NOT IN (select distinct StandardItemKey from JobItem)"
 " AND StandardItemKey NOT IN (select distinct StandardItemKey from ProjectItem)"
   " )" );
              ItiWndSetHourGlass (FALSE);
              if (bBeep)
                 for (xi = 1; xi < 9; xi++)
                    DosBeep(10000, 3);
              ItiWndSetHourGlass (TRUE);

   ItiDbExecSQLStatement (hhpZ, 
 " DELETE from StandardItemPEMETHAverage"
 " WHERE StandardItemKey IN (select distinct StandardItemKey from StandardItem "
 " where Deleted != NULL AND StandardItemKey > 1000000"
 " AND StandardItemKey NOT IN (select distinct StandardItemKey from JobItem)"
 " AND StandardItemKey NOT IN (select distinct StandardItemKey from ProjectItem)"
   " )" );
              ItiWndSetHourGlass (FALSE);
              if (bBeep)
                 for (xi = 1; xi < 8; xi++)
                    DosBeep(10000, 3);
              ItiWndSetHourGlass (TRUE);

   ItiDbExecSQLStatement (hhpZ, 
 " DELETE from StandardItemMajorItem"
 " WHERE StandardItemKey IN (select distinct StandardItemKey from StandardItem "
 " where Deleted != NULL AND StandardItemKey > 1000000"
 " AND StandardItemKey NOT IN (select distinct StandardItemKey from JobItem)"
 " AND StandardItemKey NOT IN (select distinct StandardItemKey from ProjectItem)"
   " )" );
              ItiWndSetHourGlass (FALSE);
              if (bBeep)
                 for (xi = 1; xi < 7; xi++)
                    DosBeep(10000, 5);
              ItiWndSetHourGlass (TRUE);

   ItiDbExecSQLStatement (hhpZ, 
 " DELETE from SeasonAdjustment"
 " WHERE StandardItemKey IN (select distinct StandardItemKey from StandardItem "
 " where Deleted != NULL AND StandardItemKey > 1000000"
 " AND StandardItemKey NOT IN (select distinct StandardItemKey from JobItem)"
 " AND StandardItemKey NOT IN (select distinct StandardItemKey from ProjectItem)"
   " )" );
              ItiWndSetHourGlass (FALSE);
              if (bBeep)
                 for (xi = 1; xi < 6; xi++)
                    DosBeep(10000, 5);
              ItiWndSetHourGlass (TRUE);

   ItiDbExecSQLStatement (hhpZ, 
 " DELETE from AreaAdjustment"
 " WHERE StandardItemKey IN (select distinct StandardItemKey from StandardItem "
 " where Deleted != NULL AND StandardItemKey > 1000000"
 " AND StandardItemKey NOT IN (select distinct StandardItemKey from JobItem)"
 " AND StandardItemKey NOT IN (select distinct StandardItemKey from ProjectItem)"
   " )" );
              ItiWndSetHourGlass (FALSE);
              if (bBeep)
                 for (xi = 1; xi < 5; xi++)
                    DosBeep(10000, 5);
              ItiWndSetHourGlass (TRUE);

   ItiDbExecSQLStatement (hhpZ, 
 " DELETE from PEMETHRegression"
 " WHERE StandardItemKey IN (select distinct StandardItemKey from StandardItem "
 " where Deleted != NULL AND StandardItemKey > 1000000"
 " AND StandardItemKey NOT IN (select distinct StandardItemKey from JobItem)"
 " AND StandardItemKey NOT IN (select distinct StandardItemKey from ProjectItem)"
   " )" );
              ItiWndSetHourGlass (FALSE);
              if (bBeep)
                 for (xi = 1; xi < 4; xi++)
                    DosBeep(10000, 5);
              ItiWndSetHourGlass (TRUE);

   ItiDbExecSQLStatement (hhpZ, 
 " DELETE from PEMETHAverage"
 " WHERE StandardItemKey IN (select distinct StandardItemKey from StandardItem "
 " where Deleted != NULL AND StandardItemKey > 1000000"
 " AND StandardItemKey NOT IN (select distinct StandardItemKey from JobItem)"
 " AND StandardItemKey NOT IN (select distinct StandardItemKey from ProjectItem)"
   " )" );
              ItiWndSetHourGlass (FALSE);
              if (bBeep)
                 for (xi = 1; xi < 3; xi++)
                    DosBeep(10000, 5);
              ItiWndSetHourGlass (TRUE);

   ItiDbExecSQLStatement (hhpZ, 
 " DELETE from CostBasedEstimate"
 " WHERE StandardItemKey IN (select distinct StandardItemKey from StandardItem "
 " where Deleted != NULL AND StandardItemKey > 1000000"
 " AND StandardItemKey NOT IN (select distinct StandardItemKey from JobItem)"
 " AND StandardItemKey NOT IN (select distinct StandardItemKey from ProjectItem)"
   " )" );
              ItiWndSetHourGlass (FALSE);
              if (bBeep)
                 for (xi = 1; xi < 2; xi++)
                    DosBeep(10000, 5);
              ItiWndSetHourGlass (TRUE);

   ItiDbExecSQLStatement (hhpZ, 
 " DELETE from StandardItem"
 " WHERE StandardItemKey IN (select distinct StandardItemKey from StandardItem "
 " where Deleted != NULL AND StandardItemKey > 1000000 "
 " AND StandardItemKey NOT IN (select distinct StandardItemKey from JobItem)"
 " AND StandardItemKey NOT IN (select distinct StandardItemKey from ProjectItem)"
   " )" );
               bPurgeDone = TRUE;
               DosBeep(1200, 20); DosBeep(1300, 20); DosBeep(1400, 20);
               ItiMemDestroyHeap (hhpZ);
               ItiWndSetHourGlass (FALSE);
               }
               break;  /* end of pruge deleted section */

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, usActiveWindow);
               ItiWndBuildWindow (hwndChild, StandardItemS);
               return 0;

            case (IDM_VIEW_USER_FIRST + 2):
               hwndChild = WinWindowFromID (hwnd, usActiveWindow);
               ItiWndBuildWindow (hwndChild, CBEstForStdItemS);
               return 0;

            case (IDM_VIEW_USER_FIRST + 3):
               hwndChild = WinWindowFromID (hwnd, usActiveWindow);
               ItiWndBuildWindow (hwndChild, DefaultUnitPriceS);
               return 0;

            case (IDM_VIEW_USER_FIRST + 4):
               hwndChild = WinWindowFromID (hwnd, usActiveWindow);
               ItiWndBuildWindow (hwndChild, PemethAvgS);
               return 0;

            case (IDM_VIEW_USER_FIRST + 5):
               hwndChild = WinWindowFromID (hwnd, usActiveWindow);
               ItiWndBuildWindow (hwndChild, PemethRegS);
               return 0;

            case (IDM_VIEW_USER_FIRST + 7):
               WinSetWindowPos (WinWindowFromID (hwnd, usActiveWindow),
                                NULL, 0, 0, 0, 0, SWP_HIDE);
               WinSetWindowPos (WinWindowFromID (hwnd, StdItemCBEstL),
                                NULL, 0, 0, 0, 0, SWP_SHOW);
               usActiveWindow = StdItemCBEstL;
               hwndParent = WinQueryWindow (hwnd, QW_PARENT, 0),
               hwndTitleBar = WinWindowFromID(hwndParent, FID_TITLEBAR);

           //    pglobStdItm = ItiGlobQueryGlobalsPointer();
           //    if (pglobStdItm->bUseMetric)
           //       WinSetWindowText(hwndTitleBar,
           //          "Standard Item Catalog - Cost Based Only   (Metric)");
           //    else
               WinSetWindowText(hwndTitleBar,
                  "Standard Item Catalog - Cost Based Only");

               return 0;

            case (IDM_VIEW_USER_FIRST + 8):
               WinSetWindowPos (WinWindowFromID (hwnd, usActiveWindow),
                                NULL, 0, 0, 0, 0, SWP_HIDE);
               WinSetWindowPos (WinWindowFromID (hwnd, StdItemDefUPL),
                                NULL, 0, 0, 0, 0, SWP_SHOW);
               usActiveWindow = StdItemDefUPL;
               hwndParent = WinQueryWindow (hwnd, QW_PARENT, 0),
               hwndTitleBar = WinWindowFromID(hwndParent, FID_TITLEBAR);

         //      pglobStdItm = ItiGlobQueryGlobalsPointer();
         //      if (pglobStdItm->bUseMetric)
         //         WinSetWindowText(hwndTitleBar,
         //            "Standard Item Catalog - Default Unit Price Only   (Metric)");
         //      else
                  WinSetWindowText(hwndTitleBar,
                     "Standard Item Catalog - Default Unit Price Only");

               return 0;

            case (IDM_VIEW_USER_FIRST + 9):
               WinSetWindowPos (WinWindowFromID (hwnd, usActiveWindow),
                                NULL, 0, 0, 0, 0, SWP_HIDE);
               WinSetWindowPos (WinWindowFromID (hwnd, StdItemPemethAvgL),
                                NULL, 0, 0, 0, 0, SWP_SHOW);
               usActiveWindow = StdItemPemethAvgL;
               hwndParent = WinQueryWindow (hwnd, QW_PARENT, 0),
               hwndTitleBar = WinWindowFromID(hwndParent, FID_TITLEBAR);

         //      pglobStdItm = ItiGlobQueryGlobalsPointer();
         //      if (pglobStdItm->bUseMetric)
         //         WinSetWindowText(hwndTitleBar,
         //            "Standard Item Catalog - PEMETH Average Only   (Metric)");
         //      else
                  WinSetWindowText(hwndTitleBar,
                     "Standard Item Catalog - PEMETH Average Only");
                                                //   ^31
               return 0;

            case (IDM_VIEW_USER_FIRST + 10):
               WinSetWindowPos (WinWindowFromID (hwnd, usActiveWindow),
                                NULL, 0, 0, 0, 0, SWP_HIDE);
               WinSetWindowPos (WinWindowFromID (hwnd, StdItemPemethRegL),
                                NULL, 0, 0, 0, 0, SWP_SHOW);
               usActiveWindow = StdItemPemethRegL;
               hwndParent = WinQueryWindow (hwnd, QW_PARENT, 0),
               hwndTitleBar = WinWindowFromID(hwndParent, FID_TITLEBAR);

         //      pglobStdItm = ItiGlobQueryGlobalsPointer();
         //      if (pglobStdItm->bUseMetric)
         //         WinSetWindowText(hwndTitleBar,
         //            "Standard Item Catalog - PEMETH Regression Only   (Metric)");
         //      else
                  WinSetWindowText(hwndTitleBar,
                     "Standard Item Catalog - PEMETH Regression Only");
                                                 //  ^31
               return 0;

            case (IDM_VIEW_USER_FIRST + 11):
               WinSetWindowPos (WinWindowFromID (hwnd, usActiveWindow),
                                NULL, 0, 0, 0, 0, SWP_HIDE);
               WinSetWindowPos (WinWindowFromID (hwnd, StandardItemCatL),
                                NULL, 0, 0, 0, 0, SWP_SHOW);
               usActiveWindow = StandardItemCatL;
               hwndParent = WinQueryWindow (hwnd, QW_PARENT, 0),
               hwndTitleBar = WinWindowFromID(hwndParent, FID_TITLEBAR);

         //      pglobStdItm = ItiGlobQueryGlobalsPointer();
         //      if (pglobStdItm->bUseMetric)
         //         WinSetWindowText(hwndTitleBar,
         //                          "Standard Item Catalog   (Metric)");
         //      else
                  WinSetWindowText(hwndTitleBar,
                                   "Standard Item Catalog");

               return 0;

            case IDM_FIND:
               ItiMenuDoFind (WinWindowFromID (hwnd, usActiveWindow));
               break;

            case IDM_SORT:
               ItiMenuDoSort (WinWindowFromID (hwnd, usActiveWindow));
               break;

            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }




BOOL EXPORT ItiDllInitDll (void)
   {
   HHEAP hhp0;
   HQRY  hqry;
   USHORT usNumCols, usState;
   PSZ   pszRes = NULL;
  // PSZ   pszDups = NULL;
   PPSZ  ppsz;
   CHAR  szInitQ[128] = "select count(*) from StandardItem ";
   CHAR  szChkMarker[] = "select count(*) from StandardItem"
                         " where StandardItemKey = 1000 ";
   CHAR  szSetMarker[] =
      "INSERT INTO StandardItem "
      "(StandardItemKey, StandardItemNumber, SpecYear, "
      "ShortDescription, LongDescription, Unit, LumpSum, "
      "DefaultProductionRate, DefaultStartTime, RoundingPrecision "
      ", UnitType , Deleted) VALUES "
      "(1000, '0',1900,'Marker','X',1,0,0,0,0,NULL,1) ";

   CHAR  szDef[] =
      "INSERT INTO StandardItem "
      "(StandardItemKey, StandardItemNumber, SpecYear, "
      "ShortDescription, LongDescription, Unit, LumpSum, "
      "DefaultProductionRate, DefaultStartTime, RoundingPrecision "
         ", UnitType , Deleted "
      ") VALUES "
      "(1, '0', 1900, " 
      " 'X', 'X', 1, 0, "
      " 0, 0, 0 "
      ", NULL, NULL )";

   CHAR szQry[] =
      " select UnitType, CodeValueKey "
      " from CodeValue where CodeTableKey = 1000016 "
      " and CodeValueKey >= 1000000 "
      " order by UnitType ";
   CHAR szPat[100] =
      "UPDATE StandardItem SET UnitType = %s WHERE Unit = %s ";
   CHAR szReset[100] = "    DLL Version loaded: ";

   /* --------------------------------------------------------- */
   ItiStrCat (szReset, szDllVersion, sizeof szReset);
   ItiErrWriteDebugMessage (szReset);
   for (usNumCols = 0; usNumCols < sizeof szReset; usNumCols++)
      szReset[usNumCols] = '\0';
   ItiErrWriteDebugMessage (" *** Note: Set ITILOGDUPS to YES to write imported duplicates to the log file.");
   /* --------------------------------------------------------- */

 //  pglobStdItm = NULL;
   hhp0 = ItiMemCreateHeap (1020);

   pszRes = ItiDbGetRow1Col1(szInitQ, hhp0, 0, 0, 0);
   if ((pszRes != NULL) && ( (*pszRes) == '0' ))
      ItiDbExecSQLStatement (hhp0, szDef);


   /* == Update for new functions 97 APR */
   pszRes = ItiDbGetRow1Col1(
      "/* Dll Init */ select MergeFunction from DSShellImport..Tables where TableKey = 95 "
      ,hhp0, 0, 0, 0);

   if (pszRes != NULL)
      {
      if ((pszRes[0] != 'I') && (pszRes[6] != 'M') && (pszRes[9] != 'G'))
         {
         ItiDbExecSQLStatement (hhp0,
         " UPDATE DSShellImport..Tables SET MergeFunction='ITIIMPMERGESTDITMS' WHERE TableKey = 95");
         //ItiErrWriteDebugMessage (" Note: StdItm import db update for ItiImpMergeStdItms done.");
         }
      }

  // DosScanEnv("ITILOGDUPS", &pszDups); /* Writes duplicated item data to log file. */
  // if (pszDups == NULL)
  //    bLogDups = FALSE;
  // else
  //    bLogDups = TRUE;

   /* == End of Update for new functions 97 APR */


///////
   pszRes = ItiDbGetRow1Col1(szChkMarker, hhp0, 0, 0, 0);
   if ((pszRes != NULL) && ( (*pszRes) == '0' ))
      {
      hqry = ItiDbExecQuery (szQry, hhp0, 0, 0, 0, &usNumCols, &usState);
      if (hqry != NULL)
         {
         while (ItiDbGetQueryRow (hqry, &ppsz, &usState))
            {
            ItiStrCpy (szReset, szPat, sizeof szReset);
            if ((*ppsz[0] == '0') || (*ppsz[0] == '1') )
               sprintf (szReset, szPat, ppsz[0], ppsz[1]);
            else
               sprintf (szReset, szPat, " NULL ", ppsz[1]);

            ItiDbExecSQLStatement (hhp0, szReset);
            ItiFreeStrArray (hhp0, ppsz, usNumCols);
            }
         }/* end if (hqry... */

      ItiDbExecSQLStatement (hhp0, szSetMarker);
      }/* end if ((pszRes... */
//////

   DosScanEnv("ITIBEEP", &pszBeep); /* Controls beep during StdItem purge. */
   if (pszBeep != NULL)
      bBeep = FALSE;
   else
      bBeep = TRUE;

   // /* -- Init import test flag; False means DO production database changes. */
   //bImpTstFlag = ItiGlobGetImportTestFlag ();

   if (ItiMbRegisterStaticWnd (StandardItemCatS, StandardItemCatSProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterStaticWnd (StandardItemS, ItiWndDefStaticWndProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (StandardItemL1, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (StandardItemL2, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (StandardItemL3, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (StandardItemCatL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (StdItemCBEstL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (StdItemDefUPL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (StdItemPemethAvgL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (StdItemPemethRegL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
//   if (ItiMbRegisterListWnd (PredLinkL, ItiWndDefListWndTxtProc, hmodMe))
//      return FALSE;
//   if (ItiMbRegisterListWnd (SuccLinkL, ItiWndDefListWndTxtProc, hmodMe))
//      return FALSE;

   if (ItiMbRegisterDialogWnd (StanItemD, StanItemD, StanItemDProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (StanItemDAdd, StanItemDAdd, StanItemDProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (StdItemAddPredD, StdItemAddPredD, StdItemAddPredDProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (StdItemDelPredD, StdItemDelPredD, StdItemDelPredDProc, hmodMe))
      return FALSE;

   return TRUE;
   }








USHORT EXPORT ItiImpStdItemUnit (HHEAP       hheap,
                                 PIMPORTINFO pii,
                                 PCOLINFO    pci,
                                 PSZ         pszProdDatabase)
   {
   PSZ   apszTokens [8], apszValues [8];
   HQRY  hqry;
   PSZ   *ppsz;
   USHORT usNumCols, usState, inx;
   BOOL  bDone;

   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszProdDatabase;
   apszTokens [1] = "TableName";
   apszValues [1] = pii->pti->pszTableName;
   apszTokens [2] = "ColumnName";
   apszValues [2] = pci->pszColumnName;
   apszTokens [3] = "KeyValue";
   apszValues [3] = NULL;
   apszTokens [4] = "CodeTableName";
   apszValues [4] = "UNITS";
   apszTokens [5] = "TargetColumn";
   apszValues [5] = "Unit";
   apszTokens [6] = NULL;
   apszValues [6] = NULL;

   inx = 0;   /* -- Init query buffers. */
   while (inx < ONE_THOU )
      {
      szTempSIU[inx]  = '\0';
      szQuerySIU[inx] = '\0';
      inx++;
      }

   ItiMbQueryQueryText (0, ITIRID_IMPORT, ITIIMP_GETDISTINCTFKS,
                        szTempSIU, sizeof szTempSIU);
   ItiStrReplaceParams (szQuerySIU, szTempSIU, sizeof szQuerySIU,
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   hqry = ItiDbExecQuery (szQuerySIU, hheap, 0, 0, 0, &usNumCols, &usState);
   if (hqry == NULL)
      return pii->usError = -2;

   inx = 0;   /* -- Reinit query buffers. */
   while (inx < ONE_THOU )
      {
      szTempSIU[inx]  = '\0';
      szQuerySIU[inx] = '\0';
      szICTemp[inx]   = '\0'; 
      inx++;
      }

   ItiMbQueryQueryText (0, ITIRID_IMPORT, ITIIMP_UPDATECODEVALUE,
                        szTempSIU, sizeof szTempSIU);
   ItiStrCpy(szICTemp, szTempSIU, sizeof szICTemp);

   while (!(bDone = !ItiDbGetQueryRow (hqry, &ppsz, &usState)) &&
          pii->usError == 0)
      {
      apszValues [3] = *ppsz;

      /* reinit template from the copy. */
      ItiStrCpy(szTempSIU, szICTemp, sizeof szTempSIU);

      ItiStrReplaceParams (szQuerySIU, szTempSIU, sizeof szQuerySIU,
                 apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
ItiErrWriteDebugMessage (szTempSIU);
ItiErrWriteDebugMessage (szQuerySIU);
      pii->usError = ItiDbExecSQLStatement (hheap, szQuerySIU);

      inx = 0;
      while (inx < ONE_THOU)
         {
         szQuerySIU[inx] = '\0';
         inx++;
         }
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      } /* end while (!(bDone... */

   if (!bDone)
      ItiDbTerminateQuery (hqry);

   return pii->usError;
   }


USHORT EXPORT UpdateDuplicates (PSZ pszProdDatabase);
USHORT EXPORT FixDescriptions (PSZ pszProdDatabase);



USHORT EXPORT ItiImpCopy (HHEAP       hheap,
                          PIMPORTINFO pii,
                          PSZ         pszProdDatabase)
   {
   HQRY   hqry;
   USHORT usNumCols, usState;
   USHORT uNumCols, uError;
   ULONG  ulLine = 0L;
   PSZ    apszTokens [2], apszValues [2];
   PSZ    *ppsz;
   BOOL   bDupsFound = FALSE;
   CHAR   szKMQry[180] = "";

   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszProdDatabase;
   apszTokens [1] = NULL;
   apszValues [1] = NULL;

   
   if (ItiGlobGetImportTestFlag())
      ItiErrWriteDebugMessage (" MSG: StdItm.DLL detects ACTIVE import test flag.");
   else
      ItiErrWriteDebugMessage (" MSG: StdItm.DLL detects no import test flag.");


   /* ==== Check for StandardItem/SpecYear Duplicates. ==== tlb NOV 94 === */
   ItiErrWriteDebugMessage
      ("*** Checking for StdItem duplicates in stditem.c.ItiImpCopy ");

   ItiMbQueryQueryText (hmodMe, ITIRID_IMPORT, FINDDUPLICATES_METRIC,
                        szICTemp, sizeof szICTemp);

   ItiStrReplaceParams (szICQuery, szICTemp, sizeof szICQuery,
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));

   usNumCols = ItiDbGetQueryCount (szICQuery, &uNumCols, &uError);
   if (usNumCols > 0)
       bDupsFound = TRUE;
   else
       bDupsFound = FALSE;

   if (FALSE == bDupsFound)
      ItiErrWriteDebugMessage ("*** No StdItem duplicates found. ");
   /* ==================================================================== */

   /* = If needed, import (ie. update) the duplicated StdItems. = */
   if ((bDupsFound) && (FALSE == ItiGlobGetImportTestFlag() ) )
      {
      UpdateDuplicates (pszProdDatabase);
      }/* end of if (bDups... */


   /* === Now import the non-duplicated StdItems. ======================== */
   ItiMbQueryQueryText (hmodMe, ITIRID_IMPORT, COPYSTANDARDITEM_METRIC,
                        szICTemp, sizeof szICTemp);

   ItiStrReplaceParams (szICQuery, szICTemp, sizeof szICQuery,
                        apszTokens, apszValues,
                        sizeof apszTokens / sizeof (PSZ));

   if (FALSE == ItiGlobGetImportTestFlag() )
      pii->usError = ItiDbExecSQLStatement (hheap, szICQuery);


   if (FALSE == ItiGlobGetImportTestFlag() )
      FixDescriptions (pszProdDatabase);


   /* -- Now reset the production database KeyMaster. */
   ItiStrCpy (szKMQry, "/* reset prodDB KM */ UPDATE ", sizeof szKMQry);
   ItiStrCat (szKMQry, pszProdDatabase, sizeof szKMQry);
   ItiStrCat (szKMQry, "..KeyMaster", sizeof szKMQry);
   ItiStrCat (szKMQry,
      " SET KeyValue = (select MAX(StandardItemKey) + 1 from "
      , sizeof szKMQry);
   ItiStrCat (szKMQry, pszProdDatabase, sizeof szKMQry);
   ItiStrCat (szKMQry, "..StandardItem) WHERE TableName = 'StandardItem' "
       , sizeof szKMQry);

   if (FALSE == ItiGlobGetImportTestFlag() )
      ItiDbExecSQLStatement (hheap, szKMQry);


   return pii->usError;
   }/* End of Function ItiImpCopy */




USHORT EXPORT ItiImpMergeStdItms (HHEAP       hheap,
                                  PIMPORTINFO pii,
                                  PSZ         pszProdDatabase)
   {
   HQRY   hqry;
   HHEAP  hhp;
   USHORT uCols, uErr, usCnt;
   PPSZ   ppszTmp = NULL;
   CHAR   szQry[250] = "/* StdItmMerge */"
                     " select P.StandardItemKey, I.StandardItemNumber"
                     ", I.SpecYear, I.UnitType"
                     " from StandardItem I , ";

   CHAR   szTail[] = "..StandardItem P"
                     " where I.StandardItemNumber = P.StandardItemNumber "
                     " AND I.SpecYear = P.SpecYear"
                     " AND I.UnitType = P.UnitType ";
   CHAR     szMark[3] = "";


   hhp = ItiMemCreateHeap (MAX_HEAP_SIZE);
   if (hhp == NULL)
      {
      ItiErrWriteDebugMessage
         ("IMPORT ERROR: NO heap could be created in stditm.c.ItiImpMergeStdItms");

      fprintf(stderr,"ERROR: NO heap could be created in stditm.c.ItiImpMergeStdItms");
      return 13;
      }

   fprintf(stderr, "Merging primary keys with existing StdItmNums. \n");

   /* -- Setup the query to find which existing StdItmNums overlap. */
   if (pszProdDatabase != NULL)
      ItiStrCat(szQry, pszProdDatabase, sizeof szQry);
   else
      ItiStrCat(szQry, "DSShell", sizeof szQry);

   ItiStrCat(szQry, szTail, sizeof szQry);

   /* -- Setup outer loop query to get overlapping StdItmNums. */
   if (!(hqry = ItiDbExecQuery(szQry, hhp, 0, 0, 0, &uCols, &uErr)))
      {
      ItiErrWriteDebugMessage ("stditm.c.ItiImpMergeStdItms, failed ExecQuery.");
      ItiMemDestroyHeap (hhp);
      return 13;
      }

   usCnt = 0;
   while (ItiDbGetQueryRow(hqry, &ppszTmp, &uErr)) 
      {
      ItiStrCpy (szQry,"/* merge */ UPDATE DSShellImport..StandardItem "
                       " SET StandardItemKey = ", sizeof szQry);
      ItiStrCat (szQry, ppszTmp[0], sizeof szQry);
      ItiStrCat (szQry, " WHERE StandardItemNumber = '", sizeof szQry);
      ItiStrCat (szQry, ppszTmp[1], sizeof szQry);
      ItiStrCat (szQry, "' AND SpecYear = ", sizeof szQry);
      ItiStrCat (szQry, ppszTmp[2], sizeof szQry);
      ItiStrCat (szQry, " AND UnitType = ", sizeof szQry);
      ItiStrCat (szQry, ppszTmp[3], sizeof szQry);

      pii->usError = ItiDbExecSQLStatement (hhp, szQry);

      ItiFreeStrArray(hhp, ppszTmp, uCols); 
      usCnt++;

      /* -- Show the user that something is happening. */
      szMark[0] = szStar[ (usCnt % 5) ];
      szMark[1] = '\0';
      fprintf(stderr, " %s \r", szMark);

      }/* end of while */

   ItiErrWriteDebugMessage( " Merged existing StdItm primary keys."); 
   fprintf(stderr, "Merged primary keys for %d existing StdItmNums. \n", usCnt);

   ItiMemDestroyHeap (hhp);

   return pii->usError;
   }/* End of Function ItiImpMergeStdItms */



#define ARRAY_SIZE  50

USHORT EXPORT UpdateDuplicates (PSZ pszProdDatabase)
   {
   PSZ    pszCount, pszMin, pszMax, *ppsz;
   USHORT us;
   SHORT sCount;
   // USHORT usDestMax;
   ULONG  ulMin,  ulMax;
   ULONG  ulStart = 0L;
   ULONG  ulFinish = 0L;
   ULONG  ulArySize = 49L; /* This must be same as the (ARRAY_SIZE - 1) value! */
   USHORT usNumCols, usState;
   USHORT i, j, usX ;
   // USHORT usLenUD;
   HHEAP hhpUD;
   HQRY   hqry;
   PSZ    apszTokens [3], apszValues [3];
   CHAR   aszKeys[ARRAY_SIZE+1]        [17];
   CHAR   aszSpYrs[ARRAY_SIZE+1]       [ 8];

#define SHRTLEN 42
#define LONGLEN 258

   CHAR   aszShortDesc[ARRAY_SIZE+1]   [SHRTLEN];
   CHAR   aszLongDesc[ARRAY_SIZE+1]    [LONGLEN];
   CHAR   aszUnit[ARRAY_SIZE+1]        [16];
   CHAR   aszLumpSum[ARRAY_SIZE+1]     [ 8];
   CHAR   aszDefProdRate[ARRAY_SIZE+1] [16];
   CHAR   aszDefStart[ARRAY_SIZE+1]    [16];
   CHAR   aszRndPrec[ARRAY_SIZE+1]     [ 8];
   CHAR   aszStdItmNum[ARRAY_SIZE+1]   [16];
   CHAR   aszUnitType[ARRAY_SIZE+1]    [10];

   CHAR  szRange[120] = "";
   CHAR  szUpdateDups [100] = "";
   CHAR  szFindDups [600] = "";
   CHAR  szTemp[800] = 
 " /* COUNT of FINDDUPLICATES_METRIC */ "
 "SELECT count( P.StandardItemKey) "
 "FROM DSShell..StandardItem P "
     ", DSShellImport..StandardItem I "
 "WHERE I.StandardItemNumber = P.StandardItemNumber "
  " AND P.StandardItemKey > 999999 "
  " AND P.SpecYear = I.SpecYear "
  " AND P.Deleted = NULL "
  " AND (P.UnitType = I.UnitType "
       " OR (P.UnitType = NULL "
           " AND I.UnitType = NULL )  "
       " OR P.UnitType = NULL  "
      " ) "
  //" AND I.StandardItemKey != NULL AND I.StandardItemKey > 0 AND I.StandardItemNumber != '0' "
  ;


   CHAR  szQ[800] = "select count(*) from DSShellImport..StandardItem "
                    " where StandardItemNumber != '0' ";

   CHAR   szShort[42] = "";
   CHAR   szLong[258] = "";
   CHAR   szS[42] = "";
   CHAR   szL[258] = "";
   CHAR  szULStart[20] = "";
   CHAR  szULFinish[20] = "";
   CHAR     szMark[3] = "";



   hhpUD = ItiMemCreateHeap (1000);
   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszProdDatabase;
   apszTokens [1] = "ImportDatabase";
   apszValues [1] = "DSShellImport";
   apszTokens [2] = NULL;
   apszValues [2] = NULL;

   for(us=0; us <= ARRAY_SIZE; us++)
      {
      aszKeys[us][0] = '\0';
      aszSpYrs[us][0] = '\0';

      aszShortDesc[us][0] = '\0';
      aszLongDesc[us][0] = '\0';
      aszUnit[us][0] = '\0';
      aszLumpSum[us][0] = '\0';
      aszDefProdRate[us][0] = '\0';
      aszDefStart[us][0] = '\0';
      aszRndPrec[us][0] = '\0';
      aszStdItmNum[us][0] = '\0';
      aszUnitType[us][0] = '\0';

      /* -- Show the user that something is happening. */
      szMark[0] = szStar[ (us % 5) ];
      szMark[1] = '\0';
      fprintf(stderr, " %s \r", szMark);
      }

   // pszCount = ItiDbGetRow1Col1(szQ, hhpUD, 0, 0, 0);
   // if ((pszCount != NULL) && ( (*pszCount) != '0' ))
   //    {
   //    ItiStrToSHORT(pszCount, &sCount);
   //    ItiErrWriteDebugMessage (pszCount);
   //    }
   // else
   //    {
   //    ItiMemDestroyHeap (hhpUD);
   //    return 0;
   //    }

   pszCount = ItiDbGetRow1Col1(szTemp, hhpUD, 0, 0, 0);
   if ((pszCount != NULL) && ( (*pszCount) != '0' ))
      {
      ItiStrToSHORT(pszCount, &sCount);
      ItiErrWriteDebugMessage (pszCount);
      fprintf(stderr, "\n   UpdateDuplicates of imported StdItmNums, %s were found to already exist: \n", pszCount);
      }

   /* -- this count will be the correct one (one less than above). */
   pszCount = ItiDbGetRow1Col1(szQ, hhpUD, 0, 0, 0);
   if ((pszCount != NULL) && ( (*pszCount) != '0' ))
      {
      ItiStrToSHORT(pszCount, &sCount);
      }
   else
      {
      if ((*pszCount) == '0')
         fprintf(stderr,"\nUpdateDuplicates has a query count of ZERO, done. \n");
      else
         fprintf(stderr,"\nUpdateDuplicates has a query count ERROR, halt called. \n");

      ItiMemDestroyHeap (hhpUD);
      return 0;
      }



   pszMin = ItiDbGetRow1Col1
      ("select MIN(StandardItemKey) from DSShellImport..StandardItem where StandardItemKey > 0 ",
       hhpUD, 0, 0, 0);
   if (pszMin != NULL) 
      ItiStrToULONG(pszMin, &ulMin);
   else
      ulMin = 0L;

   pszMax = ItiDbGetRow1Col1
      ("select MAX(StandardItemKey) from DSShellImport..StandardItem ",
       hhpUD, 0, 0, 0);
   if (pszMax != NULL) 
      ItiStrToULONG(pszMax, &ulMax);
   else
      ulMax = 0L;

   ItiMbQueryQueryText (hmodMe, ITIRID_IMPORT, FINDDUPLICATES_METRIC,
                        szFindDups, sizeof szFindDups);


   /* Prepare to reuse heap inside loop. */
   ItiMemDestroyHeap (hhpUD);
    

   /* === Now loop */
   ulStart = ulMin;
   while ( (sCount > 0) && (ulMax != 0L) && (ulStart <= ulMax) )
      {
      ulFinish = ulStart + ( ((ulMax - ulFinish) > ulArySize ) ? ulArySize : (ulMax - ulStart));

      /* -- Show the user that something is happening. */
      szMark[0] = szStar[ (rand() % 5) ];
      szMark[1] = '\0';
      //fprintf(stderr, " %s \r", szMark);
      fprintf(stderr, " %s     (%6d rows remaining)       \r", szMark, sCount);

      /* -- Lets use a new heap for each loop to try and avoid
         -- running out of memory near the end of the inner loop. */
      hhpUD = ItiMemCreateHeap (8000);

      /* === Get duplicated data to fill the arrays. ===================== */
      ItiStrCpy(szTemp, szFindDups, sizeof szTemp);

      //sprintf (szRange,
      //   " AND I.StandardItemKey >= %lu AND I.StandardItemKey <= %lu "
      //   " ORDER BY 1 "
      //   , ulStart, ulFinish);
      // The infamous sprintf is removed with the following lines:

      ItiStrULONGToString (ulStart, szULStart, sizeof szULStart);
      ItiStrULONGToString (ulFinish, szULFinish, sizeof szULFinish);

      ItiStrCpy (szRange, " AND I.StandardItemKey >= ", sizeof szRange);
      ItiStrCat (szRange, szULStart, sizeof szRange);
      ItiStrCat (szRange, " AND I.StandardItemKey <= ", sizeof szRange);
      ItiStrCat (szRange, szULFinish, sizeof szRange);
      ItiStrCat (szRange, " ORDER BY 1 ", sizeof szRange);

      ItiStrCat (szTemp, szRange, sizeof szTemp);

      ItiStrReplaceParams (szQ, szTemp, sizeof szQ,
                           apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
      hqry = ItiDbExecQuery (szQ, hhpUD, 0, 0, 0, &usNumCols, &usState);
      if (hqry == NULL)
         return -1;

      us = 0;
      while (ItiDbGetQueryRow (hqry, &ppsz, &usState))
         {
         if (us < ARRAY_SIZE)
            {
            ItiStrCpy(aszKeys[us], ppsz[0], sizeof aszKeys[us]);
            ItiStrCpy(aszSpYrs[us], ppsz[9], sizeof aszSpYrs[us]);

            /* ========================================================== */
            /* -- Adjust the " quotes. */
            ItiStrCpy (szS, ppsz[1], sizeof szS);
            ItiStrCpy (szL, ppsz[2], sizeof szL);
                                  
            /* -- Adjust the ' quotes Short. */
            i = 0;
            j = 0;
            while ( szS[j] != '\0')
               {
               if ( (szS[j] == '\'') && (j>1)
                     && (szS[j-1] >= '0' && szS[j-1] <= '9') )
                  {
                  szShort[i++] = 'f';
                  szShort[i++] = 't';
                  j++;
                  }
               else if (szS[j] == '\'')
                  {
                  szShort[i++] = ' ';
                  j++;
                  }
      
               szShort[i] = szS[j];
               i++;
               j++;

               /* -- Show the user that something is happening. */
               szMark[0] = szStar[ (j % 5) ];
               szMark[1] = '\0';
               fprintf(stderr, " %s \r", szMark);
               }/* end of while */
      
      
            /* -- Adjust the ' quotes in Long. */
            i = 0;
            j = 0;
            while ( szL[j] != '\0')
               {
               if ( (szL[j] == '\'') && (j>1)
                    && (szL[j-1] >= '0' && szL[j-1] <= '9') )
                  {
                  szLong[i++] = 'f';
                  szLong[i++] = 't';
                  j++;
                  }
               else if (szL[j] == '\'')
                  {
                  szLong[i++] = ' ';
                  j++;
                  }
      
               szLong[i] = szL[j];
               i++;
               j++;

               /* -- Show the user that something is happening. */
               szMark[0] = szStar[ (j % 5) ];
               szMark[1] = '\0';
               fprintf(stderr, " %s \r", szMark);

               }/* end of while */

            /* -- Reinit. description buffers. */
            for (usX = 0; usX < LONGLEN; usX++)
               {
               aszLongDesc[us][usX] = '\0';
               if (usX < SHRTLEN)
                  aszShortDesc[us][usX] = '\0';

               /* -- Show the user that something is happening. */
               szMark[0] = szStar[ (usX % 5) ];
               szMark[1] = '\0';
               fprintf(stderr, " %s \r", szMark);
               }

            ItiStrCpy(aszShortDesc[us], szShort, sizeof aszShortDesc[us]);   

            /* -- Show the user that something is happening. */
            szMark[0] = szStar[ (rand() % 5) ];
            szMark[1] = '\0';
            fprintf(stderr, " %s \r", szMark);

            ItiStrCpy(aszLongDesc[us],  szLong , sizeof aszLongDesc[us]);
            /* ========================================================== */

            ItiStrCpy(aszUnit[us],        ppsz[3], sizeof aszUnit[us]);        
            ItiStrCpy(aszLumpSum[us],     ppsz[4], sizeof aszLumpSum[us]);     
            ItiStrCpy(aszDefProdRate[us], ppsz[5], sizeof aszDefProdRate[us]); 
            ItiStrCpy(aszDefStart[us],    ppsz[6], sizeof aszDefStart[us]);    
            ItiStrCpy(aszRndPrec[us],     ppsz[7], sizeof aszRndPrec[us]);     
            ItiStrCpy(aszStdItmNum[us],   ppsz[8], sizeof aszStdItmNum[us]);

            if ( (ppsz[10] != NULL) && (ppsz[10][0] != '\0') )
               ItiStrCpy(aszUnitType[us], ppsz[10], sizeof aszUnitType[us]);
            else
               ItiStrCpy(aszUnitType[us], " NULL ", sizeof aszUnitType[us]);

            }
         else if (us == ARRAY_SIZE)
            { /* must have multiple specyears, more rows than array space. */
            ulFinish = us - 1;

            aszKeys[us][0]        = '\0';   aszKeys[us][1] = '\0';
            aszSpYrs[us][0]       = '\0';
            aszShortDesc[us][0]   = '\0';
            aszLongDesc[us][0]    = '\0';
            aszUnit[us][0]        = '\0';
            aszLumpSum[us][0]     = '\0';
            aszDefProdRate[us][0] = '\0';
            aszDefStart[us][0]    = '\0';
            aszRndPrec[us][0]     = '\0';
            aszStdItmNum[us][0]   = '\0';
            aszUnitType[us][0]    = '\0';
            }

         us++;

         /* -- Show the user that something is happening. */
         szMark[0] = szStar[ (rand() % 3) ];
         szMark[1] = '\0';
         fprintf(stderr, " %s \r", szMark);

         } /* end of while get row */


      //printf (".");

      /* === Do the updates with keys. =========================== */
      us = 0;
      while ( (us < ARRAY_SIZE) && (aszKeys[us][0] != '\0') )
         {
         ItiStrCpy(szTemp, "/* StdItem.c.UpdateDuplicates */ UPDATE ", sizeof szTemp);
         ItiStrCat (szTemp, pszProdDatabase, sizeof szTemp);
         ItiStrCat (szTemp, "..StandardItem SET Unit = ", sizeof szTemp);
         ItiStrCat (szTemp, aszUnit[us], sizeof szTemp);

         ItiStrCat (szTemp, ", DefaultProductionRate = ", sizeof szTemp);
         ItiStrCat (szTemp, aszDefProdRate[us], sizeof szTemp);
         ItiStrCat (szTemp, ", LumpSum = ", sizeof szTemp);
         ItiStrCat (szTemp, aszLumpSum[us], sizeof szTemp);
         ItiStrCat (szTemp, ", DefaultStartTime = ", sizeof szTemp);
         ItiStrCat (szTemp, aszDefStart[us], sizeof szTemp);
         ItiStrCat (szTemp, ", RoundingPrecision = ", sizeof szTemp);
         ItiStrCat (szTemp, aszRndPrec[us], sizeof szTemp);

     /* -- The production database SIKey already exists, don't change it! */
     //  ItiStrCat (szTemp, ", StandardItemKey = ", sizeof szTemp);
     //  ItiStrCat (szTemp, aszKeys[us], sizeof szTemp);

      /* -- Show the user that something is happening. */
      szMark[0] = szStar[ ((us+2) % 5) ];
      szMark[1] = '\0';
      fprintf(stderr, " %s \r", szMark);

         ItiStrCat (szTemp, ", ShortDescription = '", sizeof szTemp);
         ItiStrCat (szTemp, aszShortDesc[us], sizeof szTemp);

      /* -- Show the user that something is happening. */
      szMark[0] = szStar[ ((us+3) % 5) ];
      szMark[1] = '\0';
      fprintf(stderr, " %s \r", szMark);

         ItiStrCat (szTemp, "', LongDescription = '", sizeof szTemp);
         ItiStrCat (szTemp, aszLongDesc[us], sizeof szTemp);
         ItiStrCat (szTemp, "' WHERE StandardItemNumber = '", sizeof szTemp);
         ItiStrCat (szTemp, aszStdItmNum[us], sizeof szTemp);
         ItiStrCat (szTemp, "' AND SpecYear = ", sizeof szTemp);
         ItiStrCat (szTemp, aszSpYrs[us], sizeof szTemp);
         ItiStrCat (szTemp, " AND UnitType = ", sizeof szTemp);
         ItiStrCat (szTemp, aszUnitType[us], sizeof szTemp);

         ItiDbExecSQLStatement (hhpUD, szTemp);
         
         /* -- Now that we are finished with this key. */
         aszKeys[us][0] = '\0';

         sCount--; /* Decrement the actual key count. */
         us++;     /* Set the next array index.       */

         /* -- Show the user that something is happening. */
         szMark[0] = szStar[ (rand() % 5) ];
         szMark[1] = '\0';
         //fprintf(stderr, " %s \r", szMark);
         fprintf(stderr, " %s     (%6d rows remaining)       \r", szMark, sCount);

         }/* end of while (aszKeys... */


      /* === Prepare for the next loop thru. ===================== */
      ulStart = ulFinish + 1;

      /* -- Show the user that something is happening. */
      szMark[0] = szStar[ (rand() % 5) ];
      szMark[1] = '\0';
      fprintf(stderr, " %s \r", szMark);


      ItiMemDestroyHeap (hhpUD);
      }/* end of while (ulStart... */

   // ItiMemDestroyHeap (hhpUD);
   //sCount--;
   fprintf(stderr, " %s     (%6d StdItemNumbers were new entries) \n", " ", sCount);
   return 0;
   }/* End of Function UpdateDuplicates */



/////////////////////////

USHORT EXPORT FixDescriptions (PSZ pszProdDatabase)
   {
   PSZ    pszCount, pszMin, pszMax, pszCnt, *ppsz;
   USHORT us;
   SHORT sCount;
   // USHORT usDestMax;
   LONG   lCnt = 0L;
   ULONG  ulMin,  ulMax;
   ULONG  ulStart = 0L;
   ULONG  ulFinish = 0L;
   ULONG  ulArySize = 49L; /* This must be same as the (ARRAY_SIZE - 1) value! */
   USHORT usNumCols, usState;
   USHORT i, j, usX ;
   // USHORT usLenUD;
   HHEAP hhpUD;
   HQRY   hqry;
   PSZ    apszTokens [3], apszValues [3];
   CHAR   aszKeys[ARRAY_SIZE+1]        [17];
//   CHAR   aszSpYrs[ARRAY_SIZE+1]       [ 8];

#define SHRTLEN 42
#define LONGLEN 258

   CHAR   aszShortDesc[ARRAY_SIZE+1]   [SHRTLEN+1];
   CHAR   aszLongDesc[ARRAY_SIZE+1]    [LONGLEN+1];
   CHAR   szShort[SHRTLEN+1] = "";
   CHAR   szLong [LONGLEN+1] = "";
   CHAR   szS[SHRTLEN+1] = "";
   CHAR   szL[LONGLEN+1] = "";

   CHAR  szRange[120] = "";
   CHAR  szUpdateDups [100] = "";
   CHAR  szFindDups [600] = "";
   CHAR  szTemp[800] = "select count(*) from DSShellImport..StandardItem "
     " where StandardItemKey != NULL AND StandardItemKey > 0 AND StandardItemNumber != '0' ";
   CHAR  szQ[800] = "select count(*) from DSShellImport..StandardItem "
                   " where StandardItemKey != NULL AND StandardItemKey > 0 ";

   CHAR  szULStart[20] = "";
   CHAR  szULFinish[20] = "";
   CHAR     szMark[3] = "";



   hhpUD = ItiMemCreateHeap (1000);
   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszProdDatabase;
   apszTokens [1] = "ImportDatabase";
   apszValues [1] = "DSShellImport";
   apszTokens [2] = NULL;
   apszValues [2] = NULL;



   for(us=0; us <= ARRAY_SIZE; us++)
      {
      aszKeys[us][0] = '\0';

      /* -- Reinit. description buffers. */
      for (usX = 0; usX < LONGLEN; usX++)
         {
         aszLongDesc[us][usX] = ' ';
         if (usX < SHRTLEN)
            aszShortDesc[us][usX] = ' ';
         }
      aszLongDesc[us][LONGLEN] = '\0';
      aszShortDesc[us][SHRTLEN] = '\0';
      } /* end of for (us... */


   pszCount = ItiDbGetRow1Col1(szTemp, hhpUD, 0, 0, 0);
   if ((pszCount != NULL) && ( (*pszCount) != '0' ))
      {
      ItiStrToSHORT(pszCount, &sCount);
      ItiErrWriteDebugMessage (pszCount);
      fprintf(stderr, "\n   Fixing StdItm descriptions for %s imported StdItmNums. \n", pszCount);
      }

   /* -- this count will be the correct one (one less than above). */
   pszCount = ItiDbGetRow1Col1(szQ, hhpUD, 0, 0, 0);
   if ((pszCount != NULL) && ( (*pszCount) != '0' ))
      {
      ItiStrToSHORT(pszCount, &sCount);
      }
   else
      {
      if ((*pszCount) == '0')
         fprintf(stderr,"\nFixing StdItm descriptions has a query count of ZERO, done. \n");
      else
         fprintf(stderr,"\nFixing StdItm descriptions has a query count ERROR, halt called. \n");

      ItiMemDestroyHeap (hhpUD);
      return 0;
      }





   pszMin = ItiDbGetRow1Col1
      ("select MIN(StandardItemKey) from DSShellImport..StandardItem where StandardItemKey > 0 ",
       hhpUD, 0, 0, 0);
   if (pszMin != NULL) 
      ItiStrToULONG(pszMin, &ulMin);
   else
      ulMin = 0L;

   pszMax = ItiDbGetRow1Col1
      ("select MAX(StandardItemKey) from DSShellImport..StandardItem ",
       hhpUD, 0, 0, 0);
   if (pszMax != NULL) 
      ItiStrToULONG(pszMax, &ulMax);
   else
      ulMax = 0L;

//   ItiMbQueryQueryText (hmodMe, ITIRID_IMPORT, FINDDUPLICATES_METRIC,
//                        szFindDups, sizeof szFindDups);
     ItiStrCpy (szFindDups,
         "/* FixDesc. */ SELECT "
                " P.StandardItemKey"
               ", I.ShortDescription"
               ", I.LongDescription"
         " FROM %ProductionDatabase..StandardItem P "
             ", DSShellImport..StandardItem I "
         " WHERE I.StandardItemNumber = P.StandardItemNumber "
          " AND P.StandardItemKey > 999999 "
          " AND P.SpecYear = I.SpecYear "
          " AND P.Deleted = NULL "
          " AND (P.UnitType = I.UnitType "
               " OR (P.UnitType = NULL "
                   " AND I.UnitType = NULL )  "
               " OR P.UnitType = NULL  "
              " ) "
          , sizeof szFindDups);

   /* Prepare to reuse heap inside loop. */
   ItiMemDestroyHeap (hhpUD);
    

   /* === Now loop */
   //sCount++;
   ulStart = ulMin;
   while ( (sCount > 0) && (ulMax != 0L) && (ulStart <= ulMax) )
      {
      ulFinish = ulStart + ( ((ulMax - ulFinish) > ulArySize ) ? ulArySize : (ulMax - ulStart));

      /* -- Show the user that something is happening. */
      szMark[0] = szStar[ (rand() % 5) ];
      szMark[1] = '\0';
      fprintf(stderr, " %s     (%6d rows remaining)       \r", szMark, sCount);

      /* -- Lets use a new heap for each loop to try and avoid
         -- running out of memory near the end of the inner loop. */
      hhpUD = ItiMemCreateHeap (8000);

      /* === Get duplicated data to fill the arrays. ===================== */
      ItiStrCpy(szTemp, szFindDups, sizeof szTemp);

      ItiStrULONGToString (ulStart, szULStart, sizeof szULStart);
      ItiStrULONGToString (ulFinish, szULFinish, sizeof szULFinish);

      ItiStrCpy (szRange, " AND I.StandardItemKey >= ", sizeof szRange);
      ItiStrCat (szRange, szULStart, sizeof szRange);
      ItiStrCat (szRange, " AND I.StandardItemKey <= ", sizeof szRange);
      ItiStrCat (szRange, szULFinish, sizeof szRange);
      ItiStrCat (szRange, " ORDER BY 1 ", sizeof szRange);

      ItiStrCat (szTemp, szRange, sizeof szTemp);

      ItiStrReplaceParams (szQ, szTemp, sizeof szQ,
                           apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
      hqry = ItiDbExecQuery (szQ, hhpUD, 0, 0, 0, &usNumCols, &usState);
      if (hqry == NULL)
         {
         fprintf(stderr, " %s     (unexpected stop with %d rows remaining)\r", szMark, sCount);
         return -1;
         }

      us = 0;
      while (ItiDbGetQueryRow (hqry, &ppsz, &usState))
         {
         if (us < ARRAY_SIZE)
            {
            ItiStrCpy(aszKeys[us], ppsz[0], sizeof aszKeys[us]);

            /* === Reinit the buffers here. */
            for (usX = 0; usX < LONGLEN; usX++)
               {
               szLong[usX] = '\0';
               szL[usX] = '\0';
               if (usX < SHRTLEN)
                  {
                  szShort[usX] = '\0';
                  szS[usX] = '\0';
                  }
               /* -- Show the user that something is happening. */
               szMark[0] = szStar[ (usX % 5) ];
               szMark[1] = '\0';
               fprintf(stderr, " %s \r", szMark);
               }
            szLong[LONGLEN] = '\0';
            szShort[SHRTLEN] = '\0';
            szL[LONGLEN] = '\0';
            szS[SHRTLEN] = '\0';

            /* ========================================================== */
            ItiStrCpy (szS, ppsz[1], sizeof szS);
            ItiStrCpy (szL, ppsz[2], sizeof szL);
                                  
            /* -- Adjust the ' quotes Short. */
            i = 0;
            j = 0;
            while ( szS[j] != '\0')
               {
               if ( (szS[j] == '\'') && (j>1)
                     && (szS[j-1] >= '0' && szS[j-1] <= '9') )
                  {
                  szShort[i++] = 'f';
                  szShort[i++] = 't';
                  j++;
                  }
               else if (szS[j] == '\'')
                  {
                  szShort[i++] = ' ';
                  j++;
                  }
      
               szShort[i] = szS[j];
               i++;
               j++;

               /* -- Show the user that something is happening. */
               szMark[0] = szStar[ (j % 5) ];
               szMark[1] = '\0';
               fprintf(stderr, " %s \r", szMark);
               }/* end of while */
      
      
            /* -- Adjust the ' quotes in Long. */
            i = 0;
            j = 0;
            while ( szL[j] != '\0')
               {
               if ( (szL[j] == '\'') && (j>1)
                    && (szL[j-1] >= '0' && szL[j-1] <= '9') )
                  {
                  szLong[i++] = 'f';
                  szLong[i++] = 't';
                  j++;
                  }
               else if (szL[j] == '\'')
                  {
                  szLong[i++] = ' ';
                  j++;
                  }
      
               szLong[i] = szL[j];
               i++;
               j++;

               /* -- Show the user that something is happening. */
               szMark[0] = szStar[ (j % 5) ];
               szMark[1] = '\0';
               fprintf(stderr, " %s \r", szMark);
               }/* end of while */

            /* -- Reinit. description buffers. */
            for (usX = 0; usX < LONGLEN; usX++)
               {
               aszLongDesc[us][usX] = ' ';
               if (usX < SHRTLEN)
                  aszShortDesc[us][usX] = ' ';
               /* -- Show the user that something is happening. */
               szMark[0] = szStar[ (usX % 5) ];
               szMark[1] = '\0';
               fprintf(stderr, " %s \r", szMark);
               }
            aszLongDesc[us][LONGLEN] = '\0';
            aszShortDesc[us][SHRTLEN] = '\0';


            ItiStrCpy(aszShortDesc[us], szShort, sizeof aszShortDesc[us]);   

      /* -- Show the user that something is happening. */
      szMark[0] = szStar[ (rand() % 5) ];
      szMark[1] = '\0';
      fprintf(stderr, " %s \r", szMark);

            ItiStrCpy(aszLongDesc[us],  szLong , sizeof aszLongDesc[us]);
            /* ========================================================== */

            }
         else if (us == ARRAY_SIZE)
            { /* must have multiple specyears, more rows than array space. */
            ulFinish = us - 1;

            aszKeys[us][0] = '\0';
            aszKeys[us][1] = '\0';

            /* -- Reinit. description buffers. */
            for (usX = 0; usX < LONGLEN; usX++)
               {
               aszLongDesc[us][usX] = ' ';
               if (usX < SHRTLEN)
                  aszShortDesc[us][usX] = ' ';

               /* -- Show the user that something is happening. */
               szMark[0] = szStar[ (usX % 5) ];
               szMark[1] = '\0';
               fprintf(stderr, " %s \r", szMark);
               }
            aszLongDesc[us][LONGLEN] = '\0';
            aszShortDesc[us][SHRTLEN] = '\0';
            }                            

         us++;
         /* -- Show the user that something is happening. */
         szMark[0] = szStar[ (rand() % 5) ];
         szMark[1] = '\0';
         fprintf(stderr, " %s \r", szMark);

         } /* end of while get row */


      //printf (".");

      /* === Do the updates with keys. =========================== */
      us = 0;
      while ( (us < ARRAY_SIZE) && (aszKeys[us][0] != '\0') )
         {
         ItiStrCpy(szTemp, "/* StdItem.c.FixDesc. */ UPDATE ", sizeof szTemp);
         ItiStrCat (szTemp, pszProdDatabase, sizeof szTemp);
         ItiStrCat (szTemp, "..StandardItem SET ", sizeof szTemp);
         ItiStrCat (szTemp, " ShortDescription = '", sizeof szTemp);
      /* -- Show the user that something is happening. */
      szMark[0] = szStar[ ((us+1) % 5) ];
      szMark[1] = '\0';
      fprintf(stderr, " %s \r", szMark);

         ItiStrCat (szTemp, aszShortDesc[us], sizeof szTemp);
      /* -- Show the user that something is happening. */
      szMark[0] = szStar[ ((us+2) % 5) ];
      szMark[1] = '\0';
      fprintf(stderr, " %s \r", szMark);

         ItiStrCat (szTemp, "', LongDescription = '", sizeof szTemp);
      /* -- Show the user that something is happening. */
      szMark[0] = szStar[ ((us+3) % 5) ];
      szMark[1] = '\0';
      fprintf(stderr, " %s \r", szMark);

         ItiStrCat (szTemp, aszLongDesc[us], sizeof szTemp);
      /* -- Show the user that something is happening. */
      szMark[0] = szStar[ ((us+4) % 5) ];
      szMark[1] = '\0';
      fprintf(stderr, " %s \r", szMark);

       /* -- The production database SIKey already exists, don't change it! */
         ItiStrCat (szTemp, "' WHERE  StandardItemKey = ", sizeof szTemp);
         ItiStrCat (szTemp, aszKeys[us], sizeof szTemp);

         ItiDbExecSQLStatement (hhpUD, szTemp);
         
         /* -- Now that we are finished with this key. */
         aszKeys[us][0] = '\0';

         /* -- Reinit. description buffers, here? */
         for (usX = 0; usX < LONGLEN; usX++)
            {
            aszLongDesc[us][usX] = ' ';
            if (usX < SHRTLEN)
               aszShortDesc[us][usX] = ' ';

            /* -- Show the user that something is happening. */
            szMark[0] = szStar[ ((usX) % 5) ];
            szMark[1] = '\0';
            fprintf(stderr, " %s \r", szMark);
            }
         aszLongDesc[us][LONGLEN] = '\0';
         aszShortDesc[us][SHRTLEN] = '\0';


         sCount--; /* Decrement the actual key count. */
         us++;     /* Set the next array index.       */

         /* -- Show the user that something is happening. */
         szMark[0] = szStar[ (rand() % 5) ];
         szMark[1] = '\0';
         //fprintf(stderr, " %s \r", szMark);
         fprintf(stderr, " %s     (%6d rows remaining)       \r", szMark, sCount);

         }/* end of while (aszKeys... */ 


      /* === Prepare for the next loop thru. ===================== */
      ulStart = ulFinish + 1;

      /* -- Show the user that something is happening. */
      //printf (".");
      szMark[0] = szStar[ (rand() % 5) ];
      szMark[1] = '\0';
      fprintf(stderr, " %s \r", szMark);

      ItiMemDestroyHeap (hhpUD);
      }/* end of while (ulStart... */

   // ItiMemDestroyHeap (hhpUD);
   sCount--;
   fprintf(stderr, " %s     (%6d rows remaining)       \r", " ", sCount);
   return 0;
   }/* End of Function FixDescriptions */


