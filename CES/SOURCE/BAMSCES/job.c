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
 * Job.c
 * Mark Hampton
 * (C) 1991 AASHTO
 *
 * This module handles the Job window.
 */


#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itierror.h"
#include "..\include\itimbase.h"
#include "..\include\itiutil.h"
#include "..\include\itimap.h"
#include "..\include\winid.h"
#include "..\include\colid.h"
#include "..\include\itiglob.h"
#include "..\include\dialog.h"
#include "..\include\cesutil.h"
#include "..\include\itiest.h"
#include "calcfn.h"
#include "bamsces.h"
#include "job.h"
#include "menu.h"
#include "dialog.h"

#include <stdio.h>
#include <string.h>



/*
 * Window procedure for Job window.
 */

MRESULT EXPORT JobSProc (HWND   hwnd,
                         USHORT usMessage,
                         MPARAM mp1,
                         MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
//             case IDM_EDIT_MENU:
//                {
// // /// Metrication code
//                BOOL bMetric = FALSE;
//                PSZ  psz;
//                psz = (PSZ) QW (hwnd, ITIWND_DATA, 0, 0, cUnitType);
//                if (psz != NULL && (*psz) == '1')
//                   bMetric = TRUE;
//                if (pglobals->bUseMetric == bMetric)
//                   ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, FALSE);
//                else
//                   ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, TRUE);
//                ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, TRUE);
//                return 0;
//                }

            case IDM_EDIT_MENU:
               {
// /// Metrication code
               PGLOBALS pglob;
               BOOL bMetric;
               PSZ  pszSys = NULL;

               pszSys = (PSZ) QW (hwnd, ITIWND_DATA, 0, 0, cUnitType);
               if (pszSys != NULL
                  && ((*pszSys) == 'M' || (*pszSys) == 'm' || (*pszSys) == '1'))
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

                  // Tell user why they can not edit job now.
                  WinMessageBox (pglobals->hwndDesktop, hwnd,
                     " The active Unit System is different from this Job's;"
                     " so the Job can not be altered at this time.  From the"
                     " Utilities menu you can switch the active Unit System. ",
                     " Message Box",  0,
                     MB_OK | MB_SYSTEMMODAL | MB_ICONEXCLAMATION );
                  }

               return 0;
               }
               break;


            case IDM_VIEW_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_PARENT, TRUE);
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2), 
                                           IDM_VIEW_USER_FIRST);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "~Default Users", 
                                      IDM_VIEW_USER_FIRST + 1);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "Job ~Alternate Groups", 
                                      IDM_VIEW_USER_FIRST + 2);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "Job ~Breakdowns", 
                                      IDM_VIEW_USER_FIRST + 3);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "Job ~Comments", 
                                      IDM_VIEW_USER_FIRST + 4);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "Job ~Fund List", 
                                      IDM_VIEW_USER_FIRST + 5);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "Job ~Item Schedule", 
                                      IDM_VIEW_USER_FIRST + 6);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "Job ~User List", 
                                      IDM_VIEW_USER_FIRST + 7);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "~Job Zone List", 
                                      IDM_VIEW_USER_FIRST + 8);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2),      
                                      "~Working Day Calendar", 
                                      IDM_VIEW_USER_FIRST + 9);

               return 0;

            case IDM_UTILITIES_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_UPDATE_BASE_DATE, 
                                    FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_UPDATE_SPEC_YEAR, 
                                    FALSE);
               return 0;
            }
            break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
//            PSZ    pszJobKey;
//            USHORT uRow;
//            HHEAP  hheapStatic;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, 0, FALSE);

//             IS THIS NEEDED?!?!?  Shouldn't the dialog box do these calcs?
//             Anyway, I commented them out, 1/25/93, mdh.
//
//               DoRecalcJobTotal(hwnd);
//               hheapStatic       = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
//               pszJobKey = (PSZ) QW (hwnd, ITIWND_DATA, 0, uRow, cJobKey);
//               RecalcJFForJob(hheapStatic, pszJobKey);
//               ItiMemFree(hheapStatic, pszJobKey);

////               Note: these were already commented out. mdh 1/25/93
////               pszJobKey  = (PSZ)   QW (hwnd, ITIWND_DATA, 0, uRow, cJobKey);
////               NewRecalcJob(hwnd, pszJobKey);               /* ****** */

               return 0;
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               ItiWndBuildWindow (hwnd, DefaultUserS);
               return 0;
            
            case (IDM_VIEW_USER_FIRST + 2):
               ItiWndBuildWindow (hwnd, JobAlternateGroupListS);
               return 0;
            
            case (IDM_VIEW_USER_FIRST + 3):
               ItiWndBuildWindow (hwnd, JobBreakdownListS);
               return 0;
            
            case (IDM_VIEW_USER_FIRST + 4):
               ItiWndBuildWindow (hwnd, JobCommentS);
               return 0;
            
            case (IDM_VIEW_USER_FIRST + 5):
               ItiWndBuildWindow (hwnd, JobFundS);
               return 0;

            case (IDM_VIEW_USER_FIRST + 6):
               ItiWndBuildWindow (hwnd, JobItemScheduleS);
               return 0;
            
            case (IDM_VIEW_USER_FIRST + 7):
               ItiWndBuildWindow (hwnd, JobUserS);
               return 0;

            case (IDM_VIEW_USER_FIRST + 8):
               ItiWndBuildWindow (hwnd, JobZoneS);
               return 0;
            
            case (IDM_VIEW_USER_FIRST + 9):
               ItiWndBuildWindow (hwnd, WorkingDayCalendarS);
               return 0;
            }
         break;
      }

   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }




MRESULT EXPORT JobDProc (HWND    hwnd,
                         USHORT   uMsg,
                         MPARAM   mp1,
                         MPARAM   mp2)
   {
   static BOOL bDavisBacon;

   switch (uMsg)
      {
      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_CANCEL:   /* CANCEL button pushed */
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;

            case DID_OK:
               {
               BOOL     bHoseJob,
                        bRecalcZones = FALSE,
                        bRecalcWages = FALSE,
                        bRecalcPcts = FALSE;
               PSZ      pszJobKey;
               HHEAP    hheap, hheapStatic;
               MRESULT  mr;
               HWND     hwndStatic;

               if (ItiWndGetDlgMode () != ITI_CHANGE)
                  break;

//               ItiWndSetHourGlass (TRUE);

               hwndStatic = QW(hwnd, ITIWND_OWNERWND, 0, 0, 0);             //BSR#921109-4101
               hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
               hheapStatic = (HHEAP) QW (hwndStatic, ITIWND_HEAP, 0, 0, 0);
               pszJobKey = (PSZ) QW(hwndStatic, ITIWND_DATA, 0, 0, cJobKey);//     "
                                                                            //     "
               if (pszJobKey == NULL)                                       //     "
                  {                                                         //     "
                  ItiErrWriteDebugMessage                                   //     "
                     (" ==> In the JobDProc, failed to get the JobKey!");   //     "
                  break;     /* oh no, we got problems... */                //     "
                  }                                                         //     "

               bHoseJob  = ItiWndActiveRowChanged (hwnd, DID_WORKTYPE,     cCodeValueKey, hwnd, cWorkType);
               bHoseJob |= ItiWndActiveRowChanged (hwnd, DID_COUNTYCAT,    cCountyKey,    hwnd, cPrimaryCountyKey);
               bHoseJob |= ItiWndActiveRowChanged (hwnd, DID_LETTINGMONTH, 0,             hwnd, 0);
               /* -- bsr 930133; line added below */
               bHoseJob |= ItiWndActiveRowChanged (hwnd, DID_DISTRICT,     cCodeValueKey, hwnd, cDistrict);

               bRecalcWages = bDavisBacon != (UM) WinSendDlgItemMsg (hwnd,
                                                      DID_DAVISBACON,
                                                      BM_QUERYCHECK, 0, 0);
               if (ItiWndEntryChanged (hwnd, DID_LOCATION))
                  {
                  /* recalculate the JobZone stuff */
                  bRecalcZones = TRUE;
                  bRecalcWages = TRUE;
                  bHoseJob = TRUE;
                  }

               if ((ItiWndEntryChanged (hwnd, DID_CONSTRPCT))
                   || (ItiWndEntryChanged (hwnd, DID_CONTINGPCT)) )
                  {
                  bRecalcPcts = TRUE;
                  }

               if (mr = ItiWndDefDlgProc (hwnd, uMsg, mp1, mp2))
                  {
                  ItiMemFree (hheap, pszJobKey);
                  return mr;
                  }

               ItiWndSetHourGlass (TRUE);


               if ((bRecalcZones) || (bRecalcWages))
						{
                  /* -- Recalculate the JobZone locations. */
                  PopulateJobZoneTable (hheap, pszJobKey);

					   /* -- Reprice cost sheets - update labor, equip,
						 * -- and the material prices. */
                  DoJobCostSheetReCalc (pszJobKey);
						}

               if (bHoseJob)
                  /* recalculate the job */
                  NewRecalcJob (pszJobKey);

               if (bRecalcPcts)
                  /* Recalc the job total if the Con%s changed. */
                  DoRecalcJobTotal(hwndStatic);

               ItiMemFree (hheapStatic, pszJobKey);

               ItiWndSetHourGlass (FALSE);

               return mr;
               }/* end of DID_OK */
               break;
            }/* end of switch (SHORT1FROMMP... */
         break;

      case WM_QUERYDONE:
         {
         ItiWndActivateRow (hwnd, DID_CHECKEDBY,    cUserKey,               hwnd, ITIMB_RELCOL +2);
         ItiWndActivateRow (hwnd, DID_APPROVEDBY,   cUserKey,               hwnd, ITIMB_RELCOL +3);
         ItiWndActivateRow (hwnd, DID_ESTIMATOR,    cUserKey,               hwnd, ITIMB_RELCOL +4);
         ItiWndActivateRow (hwnd, DID_WORKTYPE,     cCodeValueKey,          hwnd, cWorkType);
         ItiWndActivateRow (hwnd, DID_WORKDAYCAL,   cWorkingDayCalendarKey, hwnd, 0);
         ItiWndActivateRow (hwnd, DID_DISTRICT,     cCodeValueKey,          hwnd, cDistrict);
         ItiWndActivateRow (hwnd, DID_LETTINGMONTH, 0,                      hwnd, 0);
         ItiWndActivateRow (hwnd, DID_COUNTYCAT,    cCountyKey,             hwnd, cPrimaryCountyKey);

         bDavisBacon = (UM) WinSendDlgItemMsg (hwnd, DID_DAVISBACON,
                                               BM_QUERYCHECK, 0, 0);
         break;
         }
      }
   return ItiWndDefDlgProc (hwnd, uMsg, mp1, mp2);
   }



static BOOL CreateChildRelations (HHEAP hheap, PSZ pszJobKey);

MRESULT EXPORT JobNewDProc (HWND    hwnd,
                            USHORT   uMsg,
                            MPARAM   mp1,
                            MPARAM   mp2)
   {
   static PSZ pszKey;

   switch (uMsg)
      {
      case WM_QUERYDONE:
      case WM_ITIDBBUFFERDONE:
         WinSetDlgItemText (hwnd, DID_LOCATION, "Long 0D 0M 0S, Lat 0D 0M 0S");
         WinSetDlgItemText (hwnd, DID_CONSTRPCT, "0");
         WinSetDlgItemText (hwnd, DID_CONTINGPCT, "0");
         break;

      case WM_NEWKEY:
         {
         HHEAP hheap;

         if (ItiStrICmp ((PSZ) mp1, "Job") == 0)
            {
            hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
            pszKey = ItiStrDup (hheap, (PSZ) mp2);
            }

         return 0;
         }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               {
               MRESULT mr;
               HHEAP   hheap;
               HWND    hwndList;

               pszKey = NULL;
               if (mr = ItiWndDefDlgProc (hwnd, uMsg, mp1, mp2))
                  return mr;

               if (pszKey != NULL)
                  {
                  hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);

                  CreateChildRelations (hheap, pszKey);
                  PopulateJobZoneTable (hheap, pszKey);

                  /* -- */

                  ItiMemFree (hheap, pszKey);
                  }

               hwndList = WinWindowFromID (hwnd, JobNewDL);
               ItiWndBuildWindow (hwndList, JobS);

               return mr;
               }
               break;
            }
         break;
      }

   return ItiWndDefDlgProc (hwnd, uMsg, mp1, mp2);
   }






static BOOL CreateChildRelations (HHEAP hheap, PSZ pszJobKey)
   {
   PSZ pszJobBreakdownKey, pszJobAlternateGroupKey, pszJobAlternateKey;
   char szTemp [600] = "";

   pszJobBreakdownKey = ItiDbGetKey (hheap, "JobBreakdown", 1);
   if (pszJobBreakdownKey == NULL)
      return FALSE;

   pszJobAlternateGroupKey = ItiDbGetKey (hheap, "JobAlternateGroup", 1);
   if (pszJobAlternateGroupKey == NULL)
      return FALSE;

   pszJobAlternateKey = ItiDbGetKey (hheap, "JobAlternate", 1);
   if (pszJobAlternateKey == NULL)
      return FALSE;

   sprintf (szTemp, "INSERT INTO JobBreakdown "
                    "(JobKey, JobBreakdownKey, JobBreakdownID, "
                    "Description, PreliminaryEstimateCost, DetailedEstimate, "
                    "DetailedEstimateCost, BreakdownEstimationMethod, "
                    "Length, PavementWidth, PavementDepth, "
                    "RoadType, "
                    "StructureType, StructureID, StructureLength, "
                    "StructureWidth, WorkType) "
                    "VALUES "
                    "(%s,%s,'Main',"
                    "'Main Job Breakdown.',0, 1,"
                    "0,0,0,0,0,3,2,'N/A',0,0,1)",
                    pszJobKey, pszJobBreakdownKey);

   ItiDbExecSQLStatement (hheap, szTemp);

   // Added on 1 MAR 94
   sprintf (szTemp, "UPDATE JobBreakdown "
                    " SET WorkType = "
                    "(SELECT WorkType FROM Job "
                    " WHERE JobKey = %s ) "
                    " WHERE JobKey = %s "
                    " AND JobBreakdownKey = %s "
                    , pszJobKey, pszJobKey, pszJobBreakdownKey);

   ItiDbExecSQLStatement (hheap, szTemp);
   // 



   sprintf (szTemp, "INSERT INTO JobBreakdownProfile "
                    "(JobKey, JobBreakdownKey, ProfileKey) "
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

   sprintf (szTemp, "INSERT INTO JobUser "
                    "(JobKey, UserID, AccessPrivilege) "
                    "VALUES "
                    "(%s,'%s',30)",
                    pszJobKey, pglobals->pszUserID);

   ItiDbExecSQLStatement (hheap, szTemp);

   sprintf (szTemp, "INSERT INTO JobUser "
                    "(JobKey, UserID, AccessPrivilege) "
                    "SELECT %s, UserGrantedPrivilege, AccessPrivilege "
                    "FROM DefaultJobUser "
                    "WHERE UserGrantedPrivilege != '%s' "
                    "AND UserKey = %s ",
                    pszJobKey, pglobals->pszUserID, pglobals->pszUserKey);

   ItiDbExecSQLStatement (hheap, szTemp);

   sprintf (szTemp, "INSERT INTO JobAlternateGroup "
                    "(JobKey, JobAlternateGroupKey, JobAlternateGroupID, Description, "
                    "ChosenAlternateKey, Estimate) "
                    "VALUES "
                    "(%s,%s,'Non-Alt','Non-Alternate Items.',"
                    "%s,0) ",
                    pszJobKey, pszJobAlternateGroupKey, pszJobAlternateKey);

   ItiDbExecSQLStatement (hheap, szTemp);

   sprintf (szTemp, "INSERT INTO JobAlternate "
                    "(JobKey, JobAlternateGroupKey, JobAlternateKey, "
                    "JobAlternateID, Description, Estimate) "
                    "VALUES "
                    "(%s,%s,%s,'Non-Alt',"
                    "'Non-Alternate Items.',0)",
                    pszJobKey, pszJobAlternateGroupKey, pszJobAlternateKey);

   ItiDbExecSQLStatement (hheap, szTemp);

   sprintf (szTemp, "INSERT INTO JobFund "
                    "(JobKey, FundKey, PercentFunded, AmountFunded) "
                    "VALUES "
                    "(%s,1,1,0)",
                    pszJobKey);

   ItiDbExecSQLStatement (hheap, szTemp);

   sprintf (szTemp, "INSERT INTO JobBreakdownFund "
                    "(JobKey,JobBreakdownKey,FundKey,PercentFunded,"
                    "AmountFunded) "
                    "VALUES "
                    "(%s,%s,1,1,0)",
                    pszJobKey, pszJobBreakdownKey);

   ItiDbExecSQLStatement (hheap, szTemp);

   ItiMemFree (hheap, pszJobBreakdownKey);
   ItiMemFree (hheap, pszJobAlternateGroupKey);
   ItiMemFree (hheap, pszJobAlternateKey);
   return TRUE;
   }



/* This fn calculates the zone for each zone type that
 * contains the job location.
 *
 * 1> Delete any zones with the same jobid
 * 2> add the zones
 *
 * returns _____ if successful.
 *
 * (return value depends on the mood setting of
 *  the METAPHYSICS environment variable)
 */
   
//BOOL PopulateJobZoneTable (HHEAP hheap, PSZ pszJobKey)
//   {
//   char  szTmp [1024] = "";
//   char   szTemp [1024] = "";
//   USHORT i, j, k, auKeys[] = {1, 0};
//   PSZ    pszLocation, pszBaseDate;
//   PSZ    *ppszZoneType, *ppszZone, *ppszMap;
//   BOOL   bDoneZoneType, bDoneZoneInsert;
//
//   /*--- get needed info about the job ---*/
//   sprintf (szTemp, " SELECT Location FROM Job WHERE JobKey = %s", pszJobKey);
//   pszLocation = (PSZ) ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);
//
//   sprintf (szTemp, " SELECT BaseDate FROM Job WHERE JobKey = %s", pszJobKey);
//   pszBaseDate = (PSZ) ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);
//
//   /*--- delete old entrys ---*/
//   sprintf (szTemp, " DELETE FROM JobZone WHERE JobKey=%s", pszJobKey);
//   ItiDbExecSQLStatement (hheap, szTemp);
//
//   /*--- get array of zone type keys ---*/
//   ItiDbBuildSelectedKeyArray2 (hheap, 0, " SELECT ZoneTypeKey FROM ZoneType",
//                                &ppszZoneType, auKeys);
//
//   for (i=0; ppszZoneType && ppszZoneType[i]; i++)
//      {
//      /*--- get array of zone keys for a zone type ---*/
//      sprintf (szTemp, " SELECT ZoneKey FROM Zone"
//                      " WHERE ZoneTypeKey = %s"
//                      " ORDER BY ZoneKey DESC", ppszZoneType[i]);
//
//      ItiDbBuildSelectedKeyArray2 (hheap, 0, szTemp, &ppszZone, auKeys);
//
//      bDoneZoneType = FALSE;
//      bDoneZoneInsert = FALSE;
//      for (j=0; ppszZone && ppszZone[j] && !bDoneZoneType; j++)
//         {
//         /*--- get array of maps for zone ---*/
//         sprintf (szTemp, " SELECT MapKey FROM ZoneMap"
//                         " WHERE ZoneKey = %s"
//                         " AND BaseDate = '%s'"
//                         " ORDER BY MapKey DESC", ppszZone[j], pszBaseDate);
//
//         ItiDbBuildSelectedKeyArray2 (hheap, 0, szTemp, &ppszMap, auKeys);
//
//         for (k=0; ppszMap && ppszMap[k] && !bDoneZoneType; k++)
//            {
//            if (ItiMapIsPointInMap (hheap, ppszMap[k], pszLocation))
//               {
//               /*--- add new entry ---*/
//               sprintf (szTemp, " INSERT INTO JobZone"
//                                " (JobKey, ZoneTypeKey, ZoneKey)"
//                                " VALUES (%s, %s, %s)",
//                                pszJobKey, ppszZoneType[i], ppszZone[j]);
//               ItiDbExecSQLStatement (hheap, szTemp);
//               bDoneZoneType = TRUE;
//               bDoneZoneInsert = TRUE;
//               }
//            }
//
//         /* -- If no ZoneTypes were inserted, we need to set the default
//            -- ZoneKey into the JobZone table for the ZoneTypeKeys.     */
//         if (bDoneZoneInsert == FALSE)
//            {
//            /*--- add default entry ---*/
//            sprintf (szTemp, " INSERT INTO JobZone"
//                             " (JobKey, ZoneTypeKey, ZoneKey)"
//                             " VALUES (%s, %s, %s)",
//                             pszJobKey, ppszZoneType[i], DEFAULT_ZONEKEY_STR);
//            if (ItiDbExecSQLStatement(hheap, szTemp) == 0)
//               {
//               bDoneZoneInsert = TRUE;
////               sprintf (szTmp, "--> Set Default JobZone insert query: %s", szTemp);
////               ItiErrWriteDebugMessage (szTmp);
//               }
//            else
//               {
//               sprintf (szTmp, "FAILED to exec default JobZone insert query: %s", szTemp);
//               ItiErrWriteDebugMessage (szTmp);
//               }
//            }
//
//         for (k=0; ppszMap && ppszMap[k]; k++)
//            ;
//         ItiFreeStrArray (hheap, ppszMap, k);
//         }
//                                                                           
//      for (j=0; ppszZone && ppszZone[j]; j++)
//         ;
//      ItiFreeStrArray (hheap, ppszZone, j);
//      }
//
//   for (i=0; ppszZoneType && ppszZoneType[i]; i++)
//      ;
//   ItiFreeStrArray (hheap, ppszZoneType, i);
//
//   return TRUE;
//   }
//


/* 1/15/93 mdh: I "fixed" this function */

BOOL PopulateJobZoneTable (HHEAP hheap, PSZ pszJobKey)
   {
   char   szTemp2 [400] = "";
   char   szTemp [400] = "";
   USHORT i, j, k,  auKeys[] = {1, 0};
   PSZ    pszLocation, pszBaseDate;
   PSZ    *ppszZoneType, *ppszZone, *ppszMap;
   BOOL   bDoneZoneType, bDoneZoneInsert;

   /*--- get needed info about the job ---*/
   sprintf (szTemp, " SELECT Location FROM Job WHERE JobKey = %s", pszJobKey);
   pszLocation = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);
   pszBaseDate = ItiEstGetJobBaseDate (hheap, pszJobKey);

   /*--- delete old entrys ---*/
   sprintf (szTemp, " DELETE FROM JobZone WHERE JobKey=%s", pszJobKey);
   ItiDbExecSQLStatement (hheap, szTemp);

   /*--- get array of zone type keys ---*/
   ItiDbBuildSelectedKeyArray2 (hheap, 0,
                                "SELECT ZoneTypeKey FROM ZoneType",
                                &ppszZoneType, auKeys);

   for (i=0; ppszZoneType && ppszZoneType[i]; i++)
      {
      /*--- get array of zone keys for a zone type ---*/
      sprintf (szTemp,
               " SELECT ZoneKey FROM Zone"
               " WHERE ZoneTypeKey = %s"
               " ORDER BY ZoneKey DESC",
               ppszZoneType[i]);

      ItiDbBuildSelectedKeyArray2 (hheap, 0, szTemp, &ppszZone, auKeys);

      bDoneZoneType = FALSE;
      bDoneZoneInsert = FALSE;
      for (j=0; ppszZone && ppszZone[j] && !bDoneZoneType; j++)
         {
         /*--- get array of maps for zone ---*/
         sprintf (szTemp,
                  " SELECT MapKey FROM ZoneMap"
                  " WHERE ZoneKey = %s"
                  " AND ZoneTypeKey = %s"
                  " AND BaseDate = '%s'"
                  " ORDER BY MapKey DESC",
                  ppszZone[j], ppszZoneType [i], pszBaseDate);

         ItiDbBuildSelectedKeyArray2 (hheap, 0, szTemp, &ppszMap, auKeys);

         for (k=0; ppszMap && ppszMap[k] && !bDoneZoneType; k++)
            {
            if (ItiMapIsPointInMap (hheap, ppszMap[k], pszLocation))
               {
               /*--- add new entry ---*/
               sprintf (szTemp,
                        " INSERT INTO JobZone"
                        " (JobKey, ZoneTypeKey, ZoneKey)"
                        " VALUES (%s, %s, %s)",
                        pszJobKey, ppszZoneType[i], ppszZone[j]);
               ItiDbExecSQLStatement (hheap, szTemp);
               bDoneZoneType = TRUE;
               bDoneZoneInsert = TRUE;
               }
            }
         ItiFreeStrArray2 (hheap, ppszMap);
         }
                                                                           
      /* -- If no ZoneTypes were inserted, we need to set the default
         -- ZoneKey into the JobZone table for the ZoneTypeKeys.     */
      if (bDoneZoneInsert == FALSE)
         {
         /*--- add default entry ---*/
         sprintf (szTemp,
                  " INSERT INTO JobZone"
                  " (JobKey, ZoneTypeKey, ZoneKey)"
                  " VALUES (%s, %s, %s)",
                  pszJobKey, ppszZoneType[i], DEFAULT_ZONEKEY_STR);
         if (ItiDbExecSQLStatement(hheap, szTemp) == 0)
            {
            bDoneZoneInsert = TRUE;
//            sprintf (szTemp2, "--> Set Default JobZone insert query: %s", szTemp);
//            ItiErrWriteDebugMessage (szTemp2);
            }
         else
            {
            sprintf (szTemp2, "FAILED to exec default JobZone insert query: %s", szTemp);
            ItiErrWriteDebugMessage (szTemp2);
            }
         }
      ItiFreeStrArray2 (hheap, ppszZone);
      }

   ItiFreeStrArray2 (hheap, ppszZoneType);
   ItiMemFree (hheap, pszBaseDate);
   ItiMemFree (hheap, pszLocation);

   return TRUE;
   } /* End Function PopulateJobZoneTable */




