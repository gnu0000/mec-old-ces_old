/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1992 by Info Tech, Inc.  All Rights Reserved.         |
|                                                                           |
|       This program module is part of DS/Shell (PC), Info Tech's           |
|       database interfaces for OS/2, a proprietary product of              |
|       Info Tech, Inc., no part of which may be reproduced or              |
|       transmitted in any form or by any means, electronic,                |
|       mechanical, or otherwise, including photocopying and recording      |
|       or in connection with any information storage or retrieval          |
|       system, without permission in writing from Info Tech, Inc.          |
|                                                                           |
+--------------------------------------------------------------------------*/


/*
 * CodeTab.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This module provides the Code Table catalog window.
 */


#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itidbase.h"
#include "..\include\itibase.h"
#include "..\include\itimbase.h"
#include "..\include\colid.h"
#include "..\include\winid.h"
#include "..\include\itiutil.h"
#include "..\include\itimenu.h"
#include "..\include\itierror.h"
#include "codetab.h"
#include "initcat.h"
#include "dialog.h"
#include <stdio.h>

static CHAR   szDllVersion[] = "1.1a0 CodeTab.dll";
static CHAR   szNewQuery [510] = "";
static HHEAP  hheap;

static PSZ    pszCTK;


BOOL IsButtonChecked (HWND    hwnd,
                      USHORT  usControl)
   {
   return 1 == SHORT1FROMMR (WinSendDlgItemMsg (hwnd, usControl, 
                                                BM_QUERYCHECK, 0, 0));
   }



void EXPORT BuildAddQuery (HWND hwndGiven, HHEAP hheapGiven)
   {
   HWND hwndParent;
   CHAR szCodeVal[64] = "";
   CHAR szXlat[512] = "";
   CHAR szCodeTableKey[32] = "";
   CHAR szCodeValueKey[32] = "";
   PSZ  pszCodeTableKey;
   PSZ  pszCodeValueKey;

   WinQueryDlgItemText (hwndGiven, DID_CODEVAL, sizeof szCodeVal, szCodeVal);
   WinQueryDlgItemText (hwndGiven, DID_XLATE, sizeof szXlat, szXlat);

   hwndParent  = (HWND) QW (hwndGiven, ITIWND_OWNERWND,0, 0, 0);
   pszCodeTableKey = (PSZ) QW (hwndParent, ITIWND_DATA, 0, 0, cCodeTableKey);
   if (pszCodeTableKey == NULL)
      pszCodeTableKey = (PSZ) QW (hwndGiven, ITIWND_DATA, 0, 0, cCodeTableKey);
   ItiStrCpy(szCodeTableKey, pszCodeTableKey, sizeof szCodeTableKey);

   pszCodeValueKey = ItiDbGetKey (hheapGiven, "CodeValue", 1);
   ItiStrCpy(szCodeValueKey, pszCodeValueKey, sizeof szCodeValueKey);
   ItiMemFree(hheapGiven, pszCodeValueKey);


   if (IsButtonChecked (hwndGiven, DID_METRIC_UNIT))
      {
      sprintf (szNewQuery,
          "/* CodeTab buildADDquery M */"
          " INSERT INTO CodeValue"
          " (CodeValueID, Translation, CodeTableKey, CodeValueKey, UnitType) "
          " VALUES ('%s', '%s', %s, %s, 1 ) "
          , szCodeVal, szXlat, szCodeTableKey, szCodeValueKey);
      }
   else if (IsButtonChecked (hwndGiven, DID_ENGLISH_UNIT))
      {
      sprintf (szNewQuery,
          "/* CodeTab buildADDquery E */"
          " INSERT INTO CodeValue"
          " (CodeValueID, Translation, CodeTableKey, CodeValueKey, UnitType) "
          " VALUES ('%s', '%s', %s, %s, 0 ) "
          , szCodeVal, szXlat, szCodeTableKey, szCodeValueKey);
      }
   else 
      { /* -- Unit to be put in both. */
      sprintf (szNewQuery,
          "/* CodeTab buildADDquery both, Metric copy */"
          " INSERT INTO CodeValue"
          " (CodeValueID, Translation, CodeTableKey, CodeValueKey, UnitType) "
          " VALUES ('%s', '%s', %s, %s, 1 ) "
          , szCodeVal, szXlat, szCodeTableKey, szCodeValueKey);

      ItiDbExecSQLStatement (hheapGiven, szNewQuery);

      pszCodeValueKey = ItiDbGetKey (hheapGiven, "CodeValue", 1);
      ItiStrCpy(szCodeValueKey, pszCodeValueKey, sizeof szCodeValueKey);
      ItiMemFree(hheapGiven, pszCodeValueKey);

      sprintf (szNewQuery,
          "/* CodeTab buildADDquery both, English copy */"
          " INSERT INTO CodeValue"
          " (CodeValueID, Translation, CodeTableKey, CodeValueKey, UnitType) "
          " VALUES ('%s', '%s', %s, %s, 0 ) "
          , szCodeVal, szXlat, szCodeTableKey, szCodeValueKey);
      }

   return;
   } /*  End of Function BuildAddQuery */



void EXPORT BuildChgQuery (HWND hwnd)
   {
   CHAR szCodeVal[64] = "";
   CHAR szXlat[512] = "";
   CHAR szCodeTableKey[32] = "";
   CHAR szCodeValueKey[32] = "";
   PSZ  pszCodeTableKey;
   PSZ  pszCodeValueKey;

   WinQueryDlgItemText (hwnd, DID_CODEVAL, sizeof szCodeVal, szCodeVal);
   WinQueryDlgItemText (hwnd, DID_XLATE, sizeof szXlat, szXlat);

   pszCodeTableKey = (PSZ) QW (hwnd, ITIWND_DATA, 0, 0, cCodeTableKey);
   ItiStrCpy(szCodeTableKey, pszCodeTableKey, sizeof szCodeTableKey);

   pszCodeValueKey = (PSZ) QW (hwnd, ITIWND_DATA, 0, 0, cCodeValueKey);
   ItiStrCpy(szCodeValueKey, pszCodeValueKey, sizeof szCodeValueKey);

   if (IsButtonChecked (hwnd, DID_METRIC_UNIT))
      {
      sprintf (szNewQuery,
                 "/* CodeTab buildCHGquery M */"
                 " UPDATE CodeValue"
                 " SET CodeValueID = '%s',"
                 " Translation = '%s',"
                 " UnitType = 1 "
                 " WHERE CodeTableKey = %s "
                 " AND CodeValueKey = %s "
                 " AND CodeValueID != '----' "
                 , szCodeVal, szXlat, szCodeTableKey, szCodeValueKey);
      }
   else if (IsButtonChecked (hwnd, DID_ENGLISH_UNIT))
      {
      sprintf (szNewQuery,
                 "/* CodeTab buildCHGquery E */"
                 " UPDATE CodeValue"
                 " SET CodeValueID = '%s',"
                 " Translation = '%s',"
                 " UnitType = 0 "
                 " WHERE CodeTableKey = %s "
                 " AND CodeValueKey = %s "
                 " AND CodeValueID != '----' "
                 , szCodeVal, szXlat, szCodeTableKey, szCodeValueKey);
      }
   else 
      {
      sprintf (szNewQuery,
                 "/* CodeTab buildCHGquery N */"
                 " UPDATE CodeValue"
                 " SET CodeValueID = '%s',"
                 " Translation = '%s',"
                 " UnitType = NULL "
                 " WHERE CodeTableKey = %s "
                 " AND CodeValueKey = %s "
                 " AND CodeValueID != '----' "
                 , szCodeVal, szXlat, szCodeTableKey, szCodeValueKey);
      }

   return;
   } /*  End of Function BuildChgQuery */



MRESULT EXPORT CodeTableDProc (HWND    hwnd,
                                        USHORT   usMessage,
                                        MPARAM   mp1,
                                        MPARAM   mp2)
   {
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }




MRESULT EXPORT CodeValueDProc (HWND    hwnd,
                               USHORT   usMessage,
                               MPARAM   mp1,
                               MPARAM   mp2)
   {
   USHORT usDlgMode, uRowSel;
   PSZ    pszUnitType;

   switch (usMessage)
      {
      case WM_QUERYDONE:
         usDlgMode =  ItiWndGetDlgMode();
         pszCTK = (PSZ) QW ((WinWindowFromID (hwnd, CodeValueS)), ITIWND_DATA, 0, 0, cCodeTableKey);
        // uRowSel = (UM) QW ((WinWindowFromID (hwnd, CodeTableL)), ITIWND_ACTIVE, 0, 0, 0);
        // pszCTK = (PSZ) QW (hwnd, ITIWND_DATA, 0, uRowSel, cCodeTableKey);
      //ItiErrWriteDebugMessage("This is the key set as the QUERYDONE CodeTableS pszCTK:");
      //ItiErrWriteDebugMessage(pszCTK);

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
            case DID_ADDMORE:
            case DID_OK:
               hheap = (HHEAP)QW (hwnd, ITIWND_HEAP, 0, 0, 0);
               pszCTK = (PSZ) QW (hwnd, ITIWND_DATA, 0, 0, cCodeTableKey);
        // ItiErrWriteDebugMessage("This is the key set as the OK pszCTK:");
        // ItiErrWriteDebugMessage(pszCTK);

               usDlgMode =  ItiWndGetDlgMode();
               if (usDlgMode == ITI_CHANGE)
                  BuildChgQuery (hwnd);
               else if (usDlgMode == ITI_ADD)
                  BuildAddQuery (hwnd, hheap);
               else
                  {
                  WinDismissDlg (hwnd, FALSE);
                  return 0;
                  }

               ItiDbExecSQLStatement (hheap, szNewQuery);
               if (SHORT1FROMMP (mp1) == DID_OK)
                  {
                  WinDismissDlg (hwnd, FALSE);
                  return 0;
                  }

               ItiDbUpdateBuffers ("CodeValue");
               // ItiDbUpdateBuffers ("CodeTable");
               break;

            case DID_CANCEL:   /* CANCEL button pushed */
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;

            default:
               break;
            }/* end switch (SHORT1FROMMP... */
      }/* end switch (usMessage... */

   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }



MRESULT EXPORT CodeTableSProc (HWND     hwnd,
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

               bSelected = ItiWndQueryActive (hwnd, CodeTableL, NULL);


               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               return 0;
               }
               break;

            case IDM_VIEW_MENU:
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2),
                                           IDM_VIEW_USER_FIRST);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),
                                      "~Code Value",
                                      IDM_VIEW_USER_FIRST + 1);
               break;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, CodeTableL, FALSE);
               ItiDbUpdateBuffers ("CodeTable");
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, CodeTableL, TRUE);

               ItiDbExecSQLStatement (
                           (HHEAP)QW (hwnd, ITIWND_HEAP, 0, 0, 0), 
                           "INSERT INTO CodeValue "
                           "(CodeValueKey,CodeTableKey,CodeValueID,Translation) "
                           "select 333, MAX(CodeTableKey), '    ', '  ' FROM CodeTable ");

               ItiDbUpdateBuffers ("CodeTable");
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, CodeTableL), ITI_DELETE);
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, CodeTableL);
               ItiWndBuildWindow (hwndChild, CodeValueS);
               return 0;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }






MRESULT EXPORT CodeValueSProc (HWND     hwnd,
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

               bSelected = ItiWndQueryActive (hwnd, CodeValueL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               return 0;
               break;
               }
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, CodeValueL, FALSE);
               ItiDbUpdateBuffers ("CodeValue");
               ItiDbUpdateBuffers ("CodeTable");
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, CodeValueL, TRUE);
               ItiDbUpdateBuffers ("CodeValue");
               ItiDbUpdateBuffers ("CodeTable");
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, CodeValueL), ITI_DELETE);
               break;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }









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
   ItiStrCpy (pszBuffer, "C~ode Table Catalog", usMaxSize);
   *pusWindowID = CodeTableS;
   }


VOID EXPORT InitCodeTable (HHEAP hhp, PSZ pszKeyVal)
   {
   PSZ  pszRes;
   CHAR szQryStr[512] = "";

   if ((NULL == pszKeyVal) || (*pszKeyVal) == '\0')
      return;

   sprintf (szQryStr,"/* codetab.c InitCodeTable 0 */ "
                  "select count(CodeTableKey) "
                  " from CodeValue "
                  " where CodeTableKey = %s ", pszKeyVal);
   pszRes = ItiDbGetRow1Col1(szQryStr, hhp, 0, 0, 0);

   if ( (NULL == pszRes) || ((*pszRes) == '\0') || ((*pszRes) == '0') )
      {
      sprintf (szQryStr,"/* codetab.c InitCodeTable 1 */ "
                         "INSERT INTO CodeValue "
                         "(CodeValueKey, CodeTableKey,"
                         " CodeValueID, Translation, UnitType) "
                         "VALUES "
                         "(4, %s, 'Def', 'Default', 0)"
                       , pszKeyVal);
      ItiDbExecSQLStatement (hhp, szQryStr);

      sprintf (szQryStr,"/* codetab.c InitCodeTable 1 */ "
                         "INSERT INTO CodeValue "
                         "(CodeValueKey, CodeTableKey,"
                         " CodeValueID, Translation, UnitType) "
                         "VALUES "
                         "(4, %s, 'Def', 'Default', 1)"
                       , pszKeyVal);
      ItiDbExecSQLStatement (hhp, szQryStr);
      }
   else if (((*pszRes) == '1') && ((*(pszRes+1)) == '\0') ) 
      {
      sprintf (szQryStr,"/* codetab.c InitCodeTable 2 */ "
                     "IF EXISTS (select CodeValueID from CodeValue"
                                  " where CodeValueKey = 5"
                                  " AND  UnitType = NULL"
                                  " AND CodeTableKey = %s ) "
                         "UPDATE CodeValue "
                         " SET Deleted = NULL, UnitType = 0 "
                         " WHERE CodeValueKey = 5 "
                         " AND CodeTableKey = %s "
                     , pszKeyVal, pszKeyVal);
      ItiDbExecSQLStatement (hhp, szQryStr);

      sprintf (szQryStr,"/* codetab.c InitCodeTable 1 */ "
                     "IF NOT EXISTS (select CodeValueID from CodeValue"
                                  " where CodeValueKey = 5"
                                  " AND  UnitType = 1"
                                  " AND CodeTableKey = %s ) "
                         "INSERT INTO CodeValue "
                         "(CodeValueKey, CodeTableKey,"
                         " CodeValueID, Translation, UnitType) "
                         "VALUES "
                         "(5, %s, 'Def', 'Default', 1)"
                       , pszKeyVal, pszKeyVal);
      ItiDbExecSQLStatement (hhp, szQryStr);
      }
   else  if (((*pszRes) > '2') && ((*(pszRes+1)) == '\0') ) 
      {
      sprintf (szQryStr,"/* codetab.c InitCodeTable 3 */ "
                  "DELETE FROM CodeValue "
                  "WHERE CodeTableKey = %s"
                  " AND CodeValueKey = 5 ", pszKeyVal);
      ItiDbExecSQLStatement (hhp, szQryStr);
      }

   return;
   }/* End of Function InitCodeTable */


#define NUMBER_OF_KEYS  64
#define SZLEN           10

BOOL EXPORT ItiDllInitDll (void)
   {
   USHORT uCols, uErr, usCnt;
   HQRY   hqry;
   HHEAP  hhp;
   PSZ    pszRes, *ppsz;
   CHAR  aszKeys [NUMBER_OF_KEYS + 1][SZLEN];
   CHAR szCodeValueKey[16] = "";
   PSZ  pszCodeValueKey;

   CHAR   szChk[1000] =
      "IF EXISTS (select Deleted from CodeValue "
                 "where CodeValueKey = 1 and CodeTableKey = 1000022)"
      " BEGIN "
        "UPDATE CodeValue SET Deleted = NULL, UnitType = 0 "
         " where CodeValueKey = 1 and CodeTableKey = 1000022 "
            "and UnitType = NULL "
        "IF NOT EXISTS (select Deleted from CodeValue "
                       "where CodeValueKey = 1 and CodeTableKey = 1000022 "
                       "and UnitType = 1)"
           " insert into "
           "CodeValue (CodeValueKey,CodeTableKey,CodeValueID,Translation,UnitType) "
           "VALUES (1,1000022,'*','All', 1) " 
      " END "
      "ELSE "
      " BEGIN "
        "IF NOT EXISTS (select CodeValueKey from CodeValue "
                       "where CodeValueKey = 1 and CodeTableKey = 1000022 "
                       "and UnitType = 0)"
      " insert into CodeValue (CodeValueKey,CodeTableKey,CodeValueID,Translation,UnitType) "
      " VALUES (1,1000022,'*','All', 0) "  
        "IF NOT EXISTS (select CodeValueKey from CodeValue "
                       "where CodeValueKey = 1 and CodeTableKey = 1000022 "
                       "and UnitType = 1)"
      " insert into CodeValue (CodeValueKey,CodeTableKey,CodeValueID,Translation,UnitType) "
      " VALUES (1,1000022,'*','All', 1) "  
      " END ";

   hhp = ItiMemCreateHeap (1000);
   ItiDbExecSQLStatement (hhp, szChk);  /* Set default WorkType */

   /* -- Set a non-structure default value. */
   sprintf (szChk,"/* codetab.c ItiDllInitDll */ "
                  "IF NOT EXISTS (select CodeValueID from CodeValue "   
                                " where CodeValueKey = 2 and UnitType = 0 ) "
                  "INSERT INTO CodeValue "
                  "(CodeValueKey,CodeTableKey,CodeValueID,Translation,UnitType) "
                  "VALUES "
                  "(2, 1000026, 'NAS', 'Not A Structure', 0)"); //CODE_DEF_STRU = 2
   ItiDbExecSQLStatement (hhp, szChk);  /* Set default StructureType */

   sprintf (szChk,"/* codetab.c ItiDllInitDll */ "
                  "IF NOT EXISTS (select CodeValueID from CodeValue "   
                                " where CodeValueKey = 2 and UnitType = 1 ) "
                  "INSERT INTO CodeValue "
                  "(CodeValueKey,CodeTableKey,CodeValueID,Translation,UnitType) "
                  "VALUES "
                  "(2, 1000026, 'NAS', 'Not A Structure', 1)"); //CODE_DEF_STRU = 2
   ItiDbExecSQLStatement (hhp, szChk);  /* Set default StructureType */


   /* -- Set a HwyType default value. */
   sprintf (szChk,"/* codetab.c ItiDllInitDll */ "
                  "IF NOT EXISTS (select CodeValueID from CodeValue "   
                                " where CodeValueKey = 3 and UnitType = 0 ) "
                  "INSERT INTO CodeValue "
                  "(CodeValueKey,CodeTableKey,CodeValueID,Translation,UnitType) "
                  "VALUES "
                  "(3, 1000017, 'GEN', 'General default hwy type.', 0)" ); //CODE_DEF_ROAD =3
   ItiDbExecSQLStatement (hhp, szChk);  /* Set default HwyType */

   sprintf (szChk,"/* codetab.c ItiDllInitDll */ "
                  "IF NOT EXISTS (select CodeValueID from CodeValue "   
                                " where CodeValueKey = 3 and UnitType = 1 ) "
                  "INSERT INTO CodeValue "
                  "(CodeValueKey,CodeTableKey,CodeValueID,Translation,UnitType) "
                  "VALUES "
                  "(3, 1000017, 'GEN', 'General default hwy type.', 1)" ); //CODE_DEF_ROAD =3
   ItiDbExecSQLStatement (hhp, szChk);  /* Set default HwyType */


   /* -- Set a default unit value. */
   sprintf (szChk,"/* codetab.c ItiDllInitDll */ "
                  "IF NOT EXISTS (select CodeValueID from CodeValue "   
                                " where CodeValueKey = 4 and UnitType = 0 ) "
                  "INSERT INTO CodeValue "
                  "(CodeValueKey,CodeTableKey,CodeValueID,Translation,UnitType) "
                  "VALUES "
                  "(4, 1000016, 'UnSp', 'Unspecifed units.', 0)" ); // CODE_DEF_UNIT = 4
   ItiDbExecSQLStatement (hhp, szChk);  /* Set default StructureType */

   sprintf (szChk,"/* codetab.c ItiDllInitDll */ "
                  "IF NOT EXISTS (select CodeValueID from CodeValue "   
                                " where CodeValueKey = 4 and UnitType = 1 ) "
                  "INSERT INTO CodeValue "
                  "(CodeValueKey,CodeTableKey,CodeValueID,Translation,UnitType) "
                  "VALUES "
                  "(4, 1000016, 'UnSp', 'Unspecifed units.', 1)" ); // CODE_DEF_UNIT = 4
   ItiDbExecSQLStatement (hhp, szChk);  /* Set default StructureType */



   /* = Insure that each orginal code table has at least 1 default value. = */
   sprintf (szChk,"/* codetab.c ItiDllInitDll */ "
                  "select DISTINCT CodeTableKey  from CodeTable T "
                  "where Deleted is null OR Deleted = 0 ");
   if (!(hqry = ItiDbExecQuery(szChk, hhp, 0, 0, 0, &uCols, &uErr)))
      {
		ItiErrWriteDebugMessage
			("codetab.c ItiDllInitDll, failed ExecQuery.");
      }

   aszKeys[0][0] = '\0';
   usCnt = 0;
   while (ItiDbGetQueryRow(hqry, &ppsz, &uErr)) 
      {
      if (usCnt < NUMBER_OF_KEYS)
         {
         ItiStrCpy(aszKeys[usCnt], ppsz[0], SZLEN); 
         aszKeys[usCnt+1][0] = '\0';
         }
      ItiFreeStrArray(hhp, ppsz, uCols); 
      usCnt++;
      }

   usCnt = 0;
   while (aszKeys[usCnt][0] != '\0')
      {
      InitCodeTable (hhp, aszKeys[usCnt]);
      usCnt++;
      }

///////
   sprintf (szChk,"/* codetab.c ItiDllInitDll, last */ "
             "select DISTINCT CodeValueID, CodeValueKey, "
             "Translation, CodeTableKey "
             "from CodeValue CVF "
             "where "
          //     "CodeTableKey = 1000016 and "
             " UnitType = NULL"
             " and CodeValueID NOT IN "
                   "(select DISTINCT CodeValueID from CodeValue "
                    "where "
                 //    "CodeTableKey = 1000016 and "
                     "UnitType != NULL)"
             " and (Deleted is NULL OR Deleted = 0) ");
   if (!(hqry = ItiDbExecQuery(szChk, hhp, 0, 0, 0, &uCols, &uErr)))
      {
		ItiErrWriteDebugMessage
			("codetab.c ItiDllInitDll, failed ExecQuery.");
      }

   while (ItiDbGetQueryRow(hqry, &ppsz, &uErr)) 
      {
      sprintf (szChk,
         " UPDATE CodeValue SET UnitType = 0 " 
         " where CodeValueKey = %s and CodeTableKey = %s "
         " and UnitType = NULL "
         , ppsz[1], ppsz[3]);
      ItiDbExecSQLStatement (hhp, szChk);

      pszCodeValueKey = ItiDbGetKey (hhp, "CodeValue", 1);
      ItiStrCpy(szCodeValueKey, pszCodeValueKey, sizeof szCodeValueKey);
      ItiMemFree(hhp, pszCodeValueKey);

      sprintf (szChk,
          " INSERT INTO CodeValue"
          " (CodeValueID, Translation, CodeTableKey, CodeValueKey, UnitType) "
          " VALUES ('%s', '%s', %s, %s, 1 ) "
          , ppsz[0], ppsz[2], ppsz[3], szCodeValueKey);
      ItiDbExecSQLStatement (hhp, szChk);

      ItiFreeStrArray(hhp, ppsz, uCols); 
      }


////////

   ItiMemDestroyHeap (hhp);

   if (ItiMbRegisterStaticWnd (CodeTableS,  CodeTableSProc,  hmodMe))
      return FALSE;
   if (ItiMbRegisterStaticWnd (CodeValueS,  CodeValueSProc,  hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (CodeTableL,  ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterListWnd (CodeValueL,  ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (CodeTableD, CodeTableD, CodeTableDProc, hmodMe))
      return FALSE;
   if (ItiMbRegisterDialogWnd (CodeValueD, CodeValueD, CodeValueDProc, hmodMe))
      return FALSE;

   return TRUE;
   }



