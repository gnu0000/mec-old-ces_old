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
 * CostSht.c
 * Mark Hampton
 *
 * This module provides the Cost Sheet catalog.
 */


#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itidbase.h"
#include "..\include\itibase.h"
#include "..\include\itimbase.h"
#include "..\include\colid.h"
#include "..\include\winid.h"
#include "..\include\itiglob.h"
#include "..\include\itiutil.h"
#include "..\include\itimenu.h"
#include "..\include\itifmt.h"
#include "..\include\itierror.h"
#include "CostSht.h"
#include "initcat.h"
#include "dialog.h"
#include <stdio.h>

static CHAR   szDllVersion[] = "1.1b1 CostSht.dll";
static CHAR   szDlgQuery [1023];
static CHAR   szBuff [1023];
static HHEAP  hheap;


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
   ItiStrCpy (pszBuffer, "Cos~t Sheet Catalog", usMaxSize);
   *pusWindowID = CostSheetCatS;
   }





BOOL IsButtonChecked (HWND    hwnd,
                      USHORT  usControl)
   {
   return 1 == SHORT1FROMMR (WinSendDlgItemMsg (hwnd, usControl, 
                                                BM_QUERYCHECK, 0, 0));
   }



void BuildQuery (HWND hwnd, HHEAP hheapGiven, BOOL bIsADD, HWND hwndList)
   {
//   HHEAP  hheap;
//   HWND   hwndParent;
   USHORT usi, usj, usLen = 0;
   SHORT  sErr = 0;
   CHAR szUnitType           [9] = "";
   CHAR szCostSheetKey      [15] = "";
   CHAR szCostSheetID       [32] = "";
   CHAR szProductionRate    [32] = "";
   CHAR szLabOverheadPct    [8] = "";
   CHAR szEquipOverheadPct  [8] = "";
   CHAR szProdutionGranularity    [8] = "";
   CHAR szMatOverheadPct    [8] = "";
   CHAR szMarkUpPct         [8] = "";
   CHAR szDefHoursPerDay    [8] = "";
#define TWOFIFTYEIGHT 258
   CHAR szDescription       [TWOFIFTYEIGHT + 1] = "";
   CHAR szDescBuffer        [TWOFIFTYEIGHT + 1] = "";
   CHAR szProdutionGranularityFmt   [8] = "";
   CHAR szLabOverheadPctFmt   [8] = "";
   CHAR szEquipOverheadPctFmt [8] = "";
   CHAR szMatOverheadPctFmt [8] = "";
   CHAR szMarkUpPctFmt [8] = "";
   CHAR szExtractList[] = "\'\"";

   PSZ   pszCostSheetKey;
//   PSZ   pszSelected;
//   SHORT sSelected;


   WinQueryDlgItemText (hwnd, DID_COSTSHEETID     , sizeof szCostSheetID     , szCostSheetID      );
   WinQueryDlgItemText (hwnd, DID_PRODUCTIONRATE  , sizeof szProductionRate  , szProductionRate   );
   WinQueryDlgItemText (hwnd, DID_LABOVERHEADPCT  , sizeof szLabOverheadPct  , szLabOverheadPct   );
   WinQueryDlgItemText (hwnd, DID_EQUIPOVERHEADPCT, sizeof szEquipOverheadPct, szEquipOverheadPct );
   WinQueryDlgItemText (hwnd, DID_PRODUCTIONGRAN  , sizeof szProdutionGranularity  , szProdutionGranularity   );
   WinQueryDlgItemText (hwnd, DID_MATOVERHEADPCT  , sizeof szMatOverheadPct  , szMatOverheadPct   );
   WinQueryDlgItemText (hwnd, DID_MARKUPPCT       , sizeof szMarkUpPct       , szMarkUpPct        );
   WinQueryDlgItemText (hwnd, DID_DEFHOURSPERDAY  , sizeof szDefHoursPerDay  , szDefHoursPerDay   );
   WinQueryDlgItemText (hwnd, DID_DESCRIPTION     , sizeof szDescBuffer     , szDescBuffer       );

   /* -- Alter any feet or inch quote marks in the description field. */
   usi = 0;
   usj = 0;
   while ( (usi < TWOFIFTYEIGHT) && (usj < TWOFIFTYEIGHT) && (szDescBuffer[usj] != '\0') )
      {
      if (szDescBuffer[usj] == '\r')
         szDescription [usi] = ' '; /* Change carriage return into a space. */
      else if (szDescBuffer[usj] == '\n')
         szDescription [usi] = ' '; /* Change newline into a space. */
      else if (szDescBuffer[usj] == '\t')
         szDescription [usi] = ' '; /* Change tab into a space. */
      else if (szDescBuffer[usj] == '\'') 
         {
         szDescription [usi] = 'f'; usi++;
         szDescription [usi] = 't'; 
         }
      else if (szDescBuffer[usj] == '\"')
         {
         szDescription [usi] = 'i'; usi++;
         szDescription [usi] = 'n'; 
         }
      else
         szDescription [usi] = szDescBuffer[usj];

      usi++;
      usj++;   
      szDescription [usi] = '\0';
      }


   if (bIsADD)
      {
      pszCostSheetKey = ItiDbGetKey (hheapGiven, "CostSheet", 1);
      ItiStrCpy(szCostSheetKey, pszCostSheetKey, sizeof szCostSheetKey);
      ItiMemFree(hheapGiven, pszCostSheetKey);

      sErr = ItiMbQueryQueryText(hmodMe, ITIRID_INSERT, CostSheetD2,
                                 szBuff, sizeof szBuff );
      }
   else /* is a change. */
      {
      pszCostSheetKey = (PSZ) QW (hwnd, ITIWND_DATA, 0, 0, cCostSheetKey);
      if (pszCostSheetKey != NULL)
         {
         ItiStrCpy(szCostSheetKey, pszCostSheetKey, sizeof szCostSheetKey);
 ItiErrWriteDebugMessage(szCostSheetKey);
    //     ItiMemFree (hheap, pszCostSheetKey);
         }
      else
         ItiErrWriteDebugMessage("*** ERROR: NULL key in CostSheet dialog box.");


      sErr = ItiMbQueryQueryText(hmodMe, ITIRID_UPDATE, CostSheetD2,
                                 szBuff, sizeof szBuff );
      }

   /* -- Now determine and convert the production granularity to a number. */
   /*      0, "NA",           "Not Available",      */
   /*     10, "Sec",          "Second",             */
   /*     20, "Min",          "Minute",             */
   /*     30, "Hour",         "Hour",               */
   /*     35, "HfDy",         "HalfDay",            */
   /*     40, "Day",          "Day",                */
   /*     50, "Week",         "Week",               */
   /*     60, "Mth",          "Month",              */
   /*     70, "Qtr",          "Quarter",            */
   /*     80, "Year",         "Year",               */
   /* -- the above codes are from the itifmt\enum.c file. */

   switch (szProdutionGranularity[0])
      {
      case 'S':
         szProdutionGranularity[0] = '1';
         break;

      case 'M':
         if (szProdutionGranularity[1] == 'i')
            szProdutionGranularity[0] = '2';
         else
            szProdutionGranularity[0] = '6';
         break;

      case 'H':
         szProdutionGranularity[0] = '3';
         break;

      case 'D':
         szProdutionGranularity[0] = '4';
         break;

      case 'W':
         szProdutionGranularity[0] = '5';
         break;

      case 'Q':
         szProdutionGranularity[0] = '7';
         break;

      case 'Y':
         szProdutionGranularity[0] = '8';
         break;

      default:
         szProdutionGranularity[0] = '0';
      }/* end of switch */

   szProdutionGranularity[1] = '0';
   szProdutionGranularity[2] = '\0';

   ItiFmtFormatString (szLabOverheadPct,
                       szLabOverheadPctFmt, sizeof szLabOverheadPctFmt,
                       "Number,....", &usLen);

   ItiFmtFormatString (szEquipOverheadPct,
                       szEquipOverheadPctFmt, sizeof szEquipOverheadPctFmt,
                       "Number,....", &usLen);

   ItiFmtFormatString (szMatOverheadPct,
                       szMatOverheadPctFmt, sizeof szMatOverheadPctFmt,
                       "Number,....", &usLen);

   ItiFmtFormatString (szMarkUpPct,
                       szMarkUpPctFmt, sizeof szMarkUpPctFmt,
                       "Number,....", &usLen);

   /* -- Now set the unit system. */
   if (IsButtonChecked (hwnd, DID_METRIC_UNIT))
      {
      ItiStrCpy(szUnitType," 1 ", sizeof szUnitType);
      }
   else if (IsButtonChecked (hwnd, DID_ENGLISH_UNIT))
      {
      ItiStrCpy(szUnitType," 0 ", sizeof szUnitType);
      }
   else 
      {
      ItiStrCpy(szUnitType," NULL ", sizeof szUnitType);
      }


 //  sprintf (szDlgQuery
 //             , szBuff
 //             , szCostSheetID      
 //             , szDescription      
 //             , szProductionRate
 //             , szProdutionGranularity
 //             , szDefHoursPerDay   
 //             , szLabOverheadPctFmt   
 //             , szEquipOverheadPctFmt 
 //             , szMatOverheadPctFmt   
 //             , szMarkUpPctFmt
 //             , szUnitType
 //             , szCostSheetKey );

ItiStrCpy(szDlgQuery, " UPDATE CostSheet SET CostSheetID= '", sizeof szDlgQuery);
ItiStrCat(szDlgQuery, szCostSheetID, sizeof szDlgQuery);
ItiStrCat(szDlgQuery, "', Description= '", sizeof szDlgQuery);
ItiStrCat(szDlgQuery, szDescription, sizeof szDlgQuery);
ItiStrCat(szDlgQuery, "', ProductionRate= ", sizeof szDlgQuery);
ItiStrCat(szDlgQuery, szProductionRate, sizeof szDlgQuery);
ItiStrCat(szDlgQuery, ", ProductionGranularity= ", sizeof szDlgQuery);
ItiStrCat(szDlgQuery, szProdutionGranularity, sizeof szDlgQuery);
ItiStrCat(szDlgQuery, ", DefaultHoursPerDay= ", sizeof szDlgQuery);
ItiStrCat(szDlgQuery, szDefHoursPerDay   , sizeof szDlgQuery);
ItiStrCat(szDlgQuery, ", LaborOverheadPct= ", sizeof szDlgQuery);
ItiStrCat(szDlgQuery, szLabOverheadPctFmt   , sizeof szDlgQuery);
ItiStrCat(szDlgQuery, ", EquipmentOverheadPct= ", sizeof szDlgQuery);
ItiStrCat(szDlgQuery, szEquipOverheadPctFmt , sizeof szDlgQuery);
ItiStrCat(szDlgQuery, ", MaterialsOverheadPct= ", sizeof szDlgQuery);
ItiStrCat(szDlgQuery, szMatOverheadPctFmt   , sizeof szDlgQuery);
ItiStrCat(szDlgQuery, ", MarkUpPct= ", sizeof szDlgQuery);
ItiStrCat(szDlgQuery, szMarkUpPctFmt, sizeof szDlgQuery);
ItiStrCat(szDlgQuery, ", UnitType= ", sizeof szDlgQuery);
ItiStrCat(szDlgQuery, szUnitType, sizeof szDlgQuery);
ItiStrCat(szDlgQuery, " WHERE CostSheetKey= ", sizeof szDlgQuery);
ItiStrCat(szDlgQuery, szCostSheetKey , sizeof szDlgQuery);

   szCostSheetKey[0] = '\0';
   return;
   } /*  End of Function BuildQuery */



BOOL CopyCostSheet (HHEAP hheap,
                    HWND  hwnd,
                    PSZ   pszOldCSKey,
                    PSZ   pszNewCSKey,
                    PSZ   pszNewID)
   {
   BOOL bOK = FALSE;
 //  USHORT i;
   CHAR szTemp[1524] = "";

   ItiErrWriteDebugMessage("*********** Entered CopyCostSheet");

   /* -- 1) Copy the CostSheet table entry. */
   sprintf (szTemp, "/* CopyCostSheet 1) Copy the CostSheet table entry. */ " 
                    " INSERT INTO CostSheet ("
                    " CostSheetKey, CostSheetID, Description, ProductionRate,"
                    " DefaultHoursPerDay, ProductionGranularity, MarkUpPct,"
                    " LaborOverheadPct, EquipmentOverheadPct,"
                    " MaterialsOverheadPct,"
                    " UnitType "
                    ") SELECT "
                    " %s, '%s', Description, ProductionRate,"
                    " DefaultHoursPerDay, ProductionGranularity, MarkUpPct,"
                    " LaborOverheadPct, EquipmentOverheadPct,"
                    " MaterialsOverheadPct,"
                    " UnitType "
                    " FROM CostSheet "
                    " WHERE CostSheetKey = %s "
                    ,pszNewCSKey, pszNewID, pszOldCSKey);
   ItiDbExecSQLStatement (hheap, szTemp);


   /* -- 2) Copy the CrewUsed table entry. */
   sprintf (szTemp, "/* CopyCostSheet 2) Copy the CrewUsed table entry. */"
                    " INSERT INTO CrewUsed ("
                    " CostSheetKey, CostSheetCrewKey, CostSheetCrewID,"
                    " Description"
                    ") SELECT "
                    " %s, CostSheetCrewKey, CostSheetCrewID,"
                    " Description"
                    " FROM CrewUsed "
                    " WHERE CostSheetKey = %s "
                    ,pszNewCSKey, pszOldCSKey);
   ItiDbExecSQLStatement (hheap, szTemp);


   /* -- 3) Copy the CrewLaborUsed table entry. */
   sprintf (szTemp, "/* CopyCostSheet 3) Copy the CrewLaborUsed table entry. */"
                    " INSERT INTO CrewLaborUsed ("
                    " CostSheetKey, CostSheetCrewKey, LaborerKey,"
                    " Quantity"
                    ") SELECT "
                    " %s, CostSheetCrewKey, LaborerKey,"
                    " Quantity"
                    " FROM CrewLaborUsed "
                    " WHERE CostSheetKey = %s "
                    ,pszNewCSKey, pszOldCSKey);
   ItiDbExecSQLStatement (hheap, szTemp);


   /* -- 4) Copy the CrewEquipmentUsed table entry. */
   sprintf (szTemp, "/* CopyCostSheet 4) Copy the CrewEquipmentUsed table entry. */"
                    " INSERT INTO CrewEquipmentUsed ("
                    " CostSheetKey, CostSheetCrewKey, EquipmentKey,"
                    " Quantity"
                    ") SELECT "
                    " %s, CostSheetCrewKey, EquipmentKey,"
                    " Quantity"
                    " FROM CrewEquipmentUsed "
                    " WHERE CostSheetKey = %s "
                    ,pszNewCSKey, pszOldCSKey);
   ItiDbExecSQLStatement (hheap, szTemp);


   /* -- 5) Copy the MaterialUsed table entry. */
   sprintf (szTemp, "/* CopyCostSheet 5) Copy the MaterialUsed table entry. */"
                    " INSERT INTO MaterialUsed ("
                    " CostSheetKey, MaterialKey," 
                    " Quantity"
                    ") SELECT "
                    " %s, MaterialKey,"
                    " Quantity"
                    " FROM MaterialUsed "
                    " WHERE CostSheetKey = %s "
                    ,pszNewCSKey, pszOldCSKey);
   ItiDbExecSQLStatement (hheap, szTemp);


   ItiDbUpdateBuffers ("CostSheet");
   ItiDbUpdateBuffers ("CrewUsed");
   ItiDbUpdateBuffers ("CrewLaborUsed");
   ItiDbUpdateBuffers ("CrewEquipmentUsed");
   ItiDbUpdateBuffers ("MaterialUsed");
   ItiErrWriteDebugMessage("*********** Exited CopyCostSheet");

   return TRUE;
   }/* End of Function CopyCostSheet */



MRESULT EXPORT CopyCostSheetDProc (HWND   hwnd,
                                   USHORT usMessage,
                                   MPARAM mp1,
                                   MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               {
               HHEAP hheap;
               PSZ   pszOldCostSheetKey;
               PSZ   pszNewCostSheetKey;
               CHAR  szNewID [9] = "";
               CHAR  szTmp [120] = "The new cost sheet name is too long: %s ";
               CHAR  szMsg [120] = "";
               CHAR  szQry[120] = "";
               PSZ   pszCnt = NULL;
               PGLOBALS pglobals;


               pglobals = ItiGlobQueryGlobalsPointer();

               hheap = QW (hwnd, ITIWND_HEAP, 0, 0, 0);
               pszOldCostSheetKey = QW (hwnd, ITIWND_DATA, 0, 0, cCostSheetKey);
               pszNewCostSheetKey = ItiDbGetKey (hheap, "CostSheet", 1);
               if (pszNewCostSheetKey == NULL)
                  {
                  WinMessageBox (HWND_DESKTOP, hwnd, 
                                 "Unable to get a new key for the new CostSheet.",
                                 pglobals->pszAppName, 0, 
                                 MB_MOVEABLE | MB_OK | MB_ICONHAND);
                  return FALSE;
                  }

               WinQueryDlgItemText (hwnd, DID_COSTSHEETID, sizeof szNewID, szNewID);

               /* -- Check new ID for length. */
               sprintf(szMsg, szTmp, szNewID);
               if (ItiStrLen (szNewID) > 8)
                  szNewID[8] = '\0';

               /* -- Check new ID for duplicates. */
               sprintf(szQry," /* CopyCostSheetDProc */" 
                             " SELECT count(*) FROM CostSheet "
                             " WHERE CostSheetID = '%s' ", szNewID);
               pszCnt = ItiDbGetRow1Col1(szQry, hheap, 0, 0, 0);
               if (pszCnt != NULL  &&  (*pszCnt) != '0')
                  {
                  szMsg[27] = 'a'; szMsg[28] = ' '; szMsg[29] = 'r';
                  szMsg[30] = 'e'; szMsg[31] = 'p'; szMsg[32] = 'e';
                  szMsg[33] = 'a'; szMsg[34] = 't'; szMsg[45] = '\0';
                  }
               else
                  sprintf(szMsg,"The new cost sheet name will be: %s ", szNewID);

               WinMessageBox (HWND_DESKTOP, hwnd, 
                                 szMsg,
                                 pglobals->pszAppName, 0, 
                                 MB_MOVEABLE | MB_OK );

               ItiWndSetHourGlass (TRUE);
               CopyCostSheet (hheap, hwnd, pszOldCostSheetKey, pszNewCostSheetKey, szNewID);
               ItiMemFree (hheap, pszOldCostSheetKey);
               ItiMemFree (hheap, pszNewCostSheetKey);
               ItiWndSetHourGlass (FALSE);
               }
               break;

            case DID_CANCEL:   /* CANCEL button pushed */
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;
            }
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }





MRESULT EXPORT CostSheetDProc (HWND    hwnd,
                                  USHORT  usMessage,
                                  MPARAM  mp1,
                                  MPARAM  mp2)
   {
   static CHAR szCostSheetKey [15];
   static PSZ  pszUnitType;
   USHORT usDlgMode;
   HWND   hwndList;

   switch (usMessage)
      {
      case WM_INITDLG:
         szCostSheetKey [0] = '\0';
         pszUnitType = NULL;
         break;

      case WM_ITIDBBUFFERDONE:
         hwndList = (HWND) QW (hwnd, ITIWND_OWNERWND, 1, 0, 0);
         break;

      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, DID_PRODUCTIONGRAN, 0, hwnd, 0);

         usDlgMode =  ItiWndGetDlgMode();
         if (usDlgMode == ITI_CHANGE)
            {
            /* -- Get the UnitType value. */
            pszUnitType = (PSZ) QW (hwnd, ITIWND_DATA, 0, 0, cUnitType);

            /* -- Set the appropriate button. */
            switch (*pszUnitType)
               {
               case '1':
                  WinSendDlgItemMsg (hwnd, DID_METRIC_UNIT,
                     BM_SETCHECK, MPFROMSHORT(1), (MPARAM) NULL );
                  break;

               case '0':
                  WinSendDlgItemMsg (hwnd, DID_ENGLISH_UNIT,
                     BM_SETCHECK, MPFROMSHORT(1), (MPARAM) NULL );
                  break;

               default:
                  WinSendDlgItemMsg (hwnd, DID_NEITHER_UNIT,
                     BM_SETCHECK, MPFROMSHORT(1), (MPARAM) NULL );
                  break;
               } /* end switch */
            } /* end if (usDlgMode... */
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               hheap = (HHEAP)QW (hwnd, ITIWND_HEAP, 0, 0, 0);

               usDlgMode =  ItiWndGetDlgMode();
               if (usDlgMode == ITI_CHANGE)
                  BuildQuery (hwnd, hheap, FALSE, hwndList);
               else if (usDlgMode == ITI_ADD)
                  BuildQuery (hwnd, hheap, TRUE, hwndList);
               else
                  {
                  WinDismissDlg (hwnd, FALSE);
                  return 0;
                  }
   
               ItiDbExecSQLStatement (hheap, szDlgQuery);
               WinDismissDlg (hwnd, FALSE);
               ItiDbUpdateBuffers ("CostSheet");
               return 0;
               break;

            case DID_CANCEL:   /* CANCEL button pushed */
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;

            case DID_COPY:
               {
               static PSZ  pszKey;

               ItiWndSetHourGlass (TRUE);

               pszKey = (PSZ) QW (hwnd, ITIWND_DATA, 0, 0, cCostSheetKey);
               if (pszKey != NULL)
                  {
                  ItiStrCpy(szCostSheetKey, pszKey, sizeof szCostSheetKey);
                  ItiErrWriteDebugMessage(szCostSheetKey);
                  }
               else
                  {
                  ItiErrWriteDebugMessage("*** ERROR: NULL key in CostSheet dialog box.");
                  return 0;
                  }
                                                       
               ItiErrWriteDebugMessage(pszUnitType);

              // if (CopyCostSheet (hwnd, szCostSheetKey))
              //    ItiDbUpdateBuffers ("CostSheet");

               ItiWndSetHourGlass (FALSE);
               }
               return 0;
               break;


            default:
               break;
            }/* end switch (SHORT1FROMMP... */
      }/* switch (usMessage... */
   

   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }



MRESULT EXPORT CostSheetCatSProc (HWND     hwnd,
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

               bSelected = ItiWndQueryActive (hwnd, CostSheetCatL, NULL);

               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2), 
                                           IDM_EDIT_MENU_FIRST + 6);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), "Copy Cost Sheet", 
                                      IDM_EDIT_MENU_FIRST + 7);


               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);

               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CUT,    TRUE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_COPY,   TRUE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_PASTE,  TRUE);

               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_EDIT_MENU_FIRST + 7, !bSelected);
               return 0;
               }
               break;

            case IDM_VIEW_MENU:
               {
               BOOL bSelected;

               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2),
                                           IDM_VIEW_USER_FIRST);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Cost Sheet",
                                      IDM_VIEW_USER_FIRST + 1);
               bSelected = ItiWndQueryActive (hwnd, CostSheetCatL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_VIEW_USER_FIRST + 1, !bSelected);
               break;
               }
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, CostSheetCatL, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, CostSheetCatL, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, CostSheetCatL), ITI_DELETE);
               break;

            case (IDM_EDIT_MENU_FIRST + 7):
               ItiWndDoNextDialog (hwnd, CostSheetCatL, CopyCostSheetD, ITI_CHANGE);

            //   ItiWndBuildWindow (WinWindowFromID (hwnd,CostSheetD), CopyCostSheetD);
                 return 0;
               break;


            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, CostSheetCatL);
               ItiWndBuildWindow (hwndChild, CostSheetS);
               return 0;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }



MRESULT EXPORT CostSheetSProc (HWND     hwnd,
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
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD,    TRUE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, TRUE);
               return 0;
               break;

            case IDM_VIEW_MENU:
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2),
                                           IDM_VIEW_USER_FIRST);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Crew for Cost Sheet",
                                      IDM_VIEW_USER_FIRST + 1);


               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Material List",
                                      IDM_VIEW_USER_FIRST + 2);
               break;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, 0, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, 0, TRUE);
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (hwnd, ITI_DELETE);
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               ItiWndBuildWindow (hwnd, CostShtCrewS);
               return 0;


            case (IDM_VIEW_USER_FIRST + 2):
               ItiWndBuildWindow (hwnd, MaterialS);
               return 0;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }



BOOL EXPORT ItiDllInitDll (void)
   {
   if (ItiMbRegisterStaticWnd (CostSheetCatS, CostSheetCatSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterStaticWnd (CostSheetS, CostSheetSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (CostSheetCatL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (CostSheetD, CostSheetD, CostSheetDProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (CopyCostSheetD, CopyCostSheetD, CopyCostSheetDProc, hmodMe))
      return FALSE;

   return TRUE;
   }







