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
 * SpecYear.c
 * Mark Hampton
 * (C) 1991 AASHTO
 * Timothy Blake
 * (C) 1995 AASHTO
 *
 * This module handles the specyear commands.
 */


#define INCL_WIN
#define INCL_DOSPROCESS
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\winid.h"
#include "..\include\itiglob.h"
#include "..\include\winid.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"
#include "..\include\colid.h"
#include "..\include\cesutil.h"
#include "bamsces.h"
#include "SpecYear.h"
#include "basedate.h"
#include "dialog.h"
#include <stdio.h>
#include <process.h>
#include <errno.h>
#include <string.h>

CHAR  szMetricYear[YEARLEN] = "" ;
CHAR  szEnglishYear[YEARLEN] = "" ;
BOOL bEnglishYearGiven, bMetricYearGiven;


static BOOL UpdateSpecYear (HHEAP hheap, 
                            PSZ   pszSpecYear,
                            PSZ   pszJobKey, 
                            HWND  hwndStatus);


static BOOL OkToUpdateSpecYear (HHEAP  hheap, 
                                PSZ    pszSpecYear,
                                PSZ    pszJobKey)
   {
   char     szQuery [350] = "";
   USHORT   usCurrent, usNew;
   PSZ      pszCurrent, pszTemp;

   ItiStrToUSHORT (pszSpecYear, &usNew);
   sprintf (szQuery, 
            "SELECT J.SpecYear "
            "FROM Job J "
            "WHERE J.JobKey=%s ",
            pszJobKey);
   pszCurrent = ItiDbGetRow1Col1 (szQuery, hheap, 0, 0, 0);

   if (pszCurrent == NULL || *pszCurrent == '\0')
      {
      WinMessageBox (HWND_DESKTOP, pglobals->hwndAppFrame, 
                     "An internal error has occured.  Contact Info Tech"
                     " Support.  Error: Could not find given JobKey in"
                     " Update Spec Year. ",
                     pglobals->pszAppName, 0, MB_OK | MB_ICONHAND | MB_MOVEABLE);
      return FALSE;
      }

   ItiStrToUSHORT (pszCurrent, &usCurrent);
   if (usCurrent == usNew)
      {
      WinMessageBox (HWND_DESKTOP, pglobals->hwndAppFrame, 
                     "This job is already using the current spec year.",
                     pglobals->pszAppName, 0, MB_OK | MB_ICONHAND | MB_MOVEABLE);
      ItiMemFree (hheap, pszCurrent);
      return FALSE;
      }

   /* check to see if we have all the needed standard items */
   sprintf (szQuery, 
            "SELECT COUNT (*) "
            "FROM JobItem JI, StandardItem SI "
            "WHERE JI.JobKey=%s "
            "AND JI.StandardItemKey = SI.StandardItemKey "
            "AND SI.StandardItemNumber NOT IN "
            " (SELECT StandardItem.StandardItemNumber "
            "  FROM StandardItem"
            "  WHERE StandardItem.StandardItemNumber = SI.StandardItemNumber "
            "  AND StandardItem.SpecYear = %s)",
            pszJobKey, pszSpecYear);

   pszTemp = ItiDbGetRow1Col1 (szQuery, hheap, 0, 0, 0);
   if (pszTemp != NULL || *pszTemp != '\0')
      {
      sprintf (szQuery, 
               "There are %s Job Items that are not in the new spec year %s. "
               "These items must be removed before the spec year for this job "
               "can be changed.", 
               pszTemp, pszSpecYear);
      WinMessageBox (HWND_DESKTOP, pglobals->hwndAppFrame, szQuery, 
                     pglobals->pszAppName, 0, MB_OK | MB_ICONHAND | MB_MOVEABLE);
      ItiMemFree (hheap, pszCurrent);
      ItiMemFree (hheap, pszTemp);
      return FALSE;
      }

   ItiMemFree (hheap, pszCurrent);
   ItiMemFree (hheap, pszTemp);
   return TRUE;
   }




void DoUpdateSpecYear (HWND hwnd)
   {
   PSZ   pszJobKey;
   HHEAP hheap;
   HWND  hwndModeless;
   QMSG  qmsg;
   PSZ   pszSpecYear;
   char  szTemp [512];

   // BSR 921026-4105: Tell user that this command does not
   // work, and a resolution is pending the next release of SQL Server.
   WinMessageBox (HWND_DESKTOP, pglobals->hwndAppFrame, 
                  "The Update Spec Year Command Does not work with this "
                  "release of the SQL Server.  If you need to perform "
                  "this operation, contact Info Tech BAMS/CES Technical "
                  "Support.  They will provide you with instructions on "
                  "how to do this command with this release of SQL Server."
                  ,pglobals->pszAppName, 0, MB_OK | MB_ICONHAND | MB_MOVEABLE);
   return;


   WinUpdateWindow (hwnd);
   if (hwnd == NULL || !ItiCesIsJobWindow (hwnd, 0))
      return;

   hheap = WinSendMsg (hwnd, WM_ITIWNDQUERY, MPFROMSHORT (ITIWND_HEAP), 0);
   pszJobKey = WinSendMsg (hwnd, WM_ITIWNDQUERY, MPFROMSHORT (ITIWND_DATA), 
                           MPFROM2SHORT (0, cJobKey));

   if (hheap == NULL || pszJobKey == NULL)
      {
      WinMessageBox (HWND_DESKTOP, hwnd, 
                     "An internal error has occured.  Contact Info Tech"
                     " Support.  Error: Could not get heap/JobKey for"
                     " Update Spec Year. ",
                     pglobals->pszAppName, 0, MB_OK | MB_ICONHAND | MB_MOVEABLE);
      return;
      }

   ItiWndSetHourGlass (TRUE);
   pszSpecYear = ItiDbGetDbValue (hheap, ITIDB_MAX_SPEC_YEAR);
   ItiWndSetHourGlass (FALSE);
   sprintf (szTemp, "The active job's spec year will be set to %s.  "
                    "This will cause repricing of the job.  Are you sure you "
                    "want to continue?", pszSpecYear);
   if (DID_OK != WinMessageBox (pglobals->hwndDesktop, hwnd, szTemp, 
                                pglobals->pszAppName, 0, 
                                MB_OKCANCEL | MB_MOVEABLE | MB_ICONEXCLAMATION))
      {
      ItiMemFree (hheap, pszJobKey);
      ItiMemFree (hheap, pszSpecYear);
      return;
      }

   ItiWndSetHourGlass (TRUE);

   if (!OkToUpdateSpecYear (hheap, pszSpecYear, pszJobKey))
      {
      ItiMemFree (hheap, pszJobKey);
      ItiMemFree (hheap, pszSpecYear);
      ItiWndSetHourGlass (FALSE);
      return;
      }

//   hwndModeless = WinLoadDlg (HWND_DESKTOP, hwnd, DoDeclareSpecYearStatusProc, 
//                              0, DeclareSpecYearStatus, szDate);
//   
//   if (hwndModeless == NULL)
//      {
//      WinMessageBox (HWND_DESKTOP, hwnd, 
//                     "Not enough memory to update the spec year."
//                     "  Try the command again after closing some "
//                     "applications.  (could not load modeless dialog).",
//                     pglobals->pszAppName, 0, MB_OK | MB_ICONHAND | MB_MOVEABLE);
//      ItiMemFree (hheap, pszSpecYear);
//      return;
//      }
//                                 
//   WinUpdateWindow (hwndModeless);

   if (!UpdateSpecYear (hheap, pszSpecYear, pszJobKey, hwndModeless))
      {
      ItiWndSetHourGlass (FALSE);
      WinMessageBox (HWND_DESKTOP, hwnd, 
                     "Not enough memory to update the spec year."
                     "  Try the command again after closing some "
                     "applications.  (no memory for thread).",
                     pglobals->pszAppName, 0, MB_OK | MB_ICONHAND | MB_MOVEABLE);
//      WinDestroyWindow (hwndModeless);
      ItiMemFree (hheap, pszJobKey);
      ItiMemFree (hheap, pszSpecYear);
      return;
      }

   /* Get and dispatch messages. */
   while (WinIsWindow (pglobals->habMainThread, hwndModeless) && 
          WinGetMsg (pglobals->habMainThread, &qmsg, NULL, 0, 0))
      {
      if (qmsg.hwnd != NULL)
         WinDispatchMsg (pglobals->habMainThread, &qmsg);
      else
         {
         /* process WM_SEMx messages */
         }
      }
   NewRecalcJob (pszJobKey);

   ItiWndSetHourGlass (FALSE);
   ItiMemFree (hheap, pszJobKey);
   ItiMemFree (hheap, pszSpecYear);
   }




typedef struct
   {
   HHEAP hheap;
   PSZ   pszSpecYear;
   PSZ   pszJobKey;
   HWND  hwndStatus;
   } SPECYEARINFO; /* bdi */

typedef SPECYEARINFO FAR *PSPECYEARINFO; /* pbdi */
   

void FAR _cdecl _loadds UpdateSpecYearThread (PSPECYEARINFO pbdi);

#define SPECYEAR_STACKSIZE 16380



static BOOL UpdateSpecYear (HHEAP hheap, 
                            PSZ   pszSpecYear, 
                            PSZ   pszJobKey,
                            HWND  hwndStatus)
   {
   PSPECYEARINFO  pbdi;

   pbdi = ItiMemAlloc (hheap, sizeof (SPECYEARINFO), 0);
   if (pbdi == NULL)
      return FALSE;

   pbdi->hheap = hheap;
   pbdi->pszSpecYear = ItiStrDup (hheap, pszSpecYear);
   pbdi->pszJobKey = ItiStrDup (hheap, pszJobKey);
   pbdi->hwndStatus = hwndStatus;

   if (-1 == _beginthread (UpdateSpecYearThread, NULL, SPECYEAR_STACKSIZE, pbdi))
      return FALSE;
//   UpdateSpecYearThread (pbdi);
   return TRUE;
   }


static USHORT DoIt (PSPECYEARINFO pbdi)
   {
   char     szTemp [340];
   USHORT   usErrorCount = 0;

   /* update the standard item keys */
   sprintf (szTemp, "UPDATE JobItem "
                    "SET StandardItemKey = "
                    " (SELECT SI1.StandardItemKey "
                    " FROM StandardItem SI1 "
                    " WHERE SI1.SpecYear = %s "
                    " AND SI1.StandardItemNumber = "
                    " (SELECT SI2.StandardItemNumber "
                    " FROM StandardItem SI2 "
                    " WHERE SI2.StandardItemKey = JobItem.StandardItemKey)) "
                    "WHERE JobKey = %s ",
            pbdi->pszSpecYear, pbdi->pszJobKey);
   if (ItiDbExecSQLStatement (pbdi->hheap, szTemp))
      {
      WinMessageBox (HWND_DESKTOP, pglobals->hwndAppFrame,
                     "Could not update the Standard Item Keys.",
                     "Update Spec Year", 0, MB_OK | MB_MOVEABLE);
      return 1;
      }

   sprintf (szTemp, "UPDATE Job "
                    "SET SpecYear = %s "
                    "WHERE JobKey = %s ",
            pbdi->pszSpecYear, pbdi->pszJobKey);
   if (ItiDbExecSQLStatement (pbdi->hheap, szTemp))
      {
      WinMessageBox (HWND_DESKTOP, pglobals->hwndAppFrame,
                     "Could not update the Spec Year.",
                     "Update Spec Year", 0, MB_OK | MB_MOVEABLE);
      return 1;
      }

   /* send the update messages */
   ItiDbUpdateBuffers ("Job");
   ItiDbUpdateBuffers ("JobItem");
   ItiDbUpdateBuffers ("JobBreakdown");
   ItiDbUpdateBuffers ("JobAlternate");
   return 0;
   }


void FAR _cdecl _loadds UpdateSpecYearThread (PSPECYEARINFO pbdi)
   {
   HMQ   hmq;
   HAB   hab;
   HWND  hwnd;

   hab = WinInitialize (0);
   hmq = WinCreateMsgQueue (hab, DEFAULT_QUEUE_SIZE);

   DoIt (pbdi);

   /* finish up */
   hwnd = pbdi->hwndStatus;
   ItiMemFree (pbdi->hheap, pbdi->pszSpecYear);
   ItiMemFree (pbdi->hheap, pbdi->pszJobKey);
   ItiMemFree (pbdi->hheap, pbdi);

   WinDestroyMsgQueue (hmq); 
   WinTerminate (hab);
//   WinPostMsg (hwnd, WM_CLOSE, 0, 0);
   }


BOOL EXPORT SetTitleBarString (void)
   {
   CHAR  szMsg[254] = "";
   PSZ   pszTemp;
   HHEAP hhpLoc;
   PGLOBALS pglobSpecYr = NULL;

   hhpLoc = ItiMemCreateHeap (1024);
   pglobSpecYr = ItiGlobQueryGlobalsPointer();
   if ((NULL == pglobSpecYr) || (hhpLoc == NULL))
      {
      DosBeep(1200, 20); DosBeep(1300, 20); DosBeep(1400, 20);
      ItiErrWriteDebugMessage
         ("MSG: Could not access the GLOBALS pointer in SpecYear.c.SetTitleBarString ");
      WinMessageBox (HWND_DESKTOP, pglobSpecYr->hwndAppFrame,
                     "*** Unable to comply at this time! ***",
                     "-- Status Reset --", 0, MB_OK | MB_ICONHAND);
      if (hhpLoc != NULL)
         ItiMemDestroyHeap (hhpLoc);
      return FALSE;
      }


   /* -- Unit Type System */
  // sprintf (szMsg,
  //    "SELECT UnitType FROM UnitTypeSystem WHERE UserKey = %s ",
  //    pglobSpecYr->pszUserKey);
  // pszTemp = ItiDbGetRow1Col1 (szMsg, hhpLoc, 0, 0, 0);
   /* -- Here use the pglob data ONLY! */
   if (pglobSpecYr->bUseMetric)
      pszTemp = "1";
   else
      pszTemp = "0";

   if ((pszTemp != NULL) && (pszTemp[0] == '1'))
      ItiStrCpy (szTitleBarString, APP_TITLE_METRIC, sizeof szTitleBarString);
   else
      ItiStrCpy (szTitleBarString, APP_TITLE_ENGLISH, sizeof szTitleBarString);

   // ItiMemFree (hhpLoc, pszTemp); -- psztemp no longer from getrow1col1

   /* -- SpecYear */
 //  sprintf (szMsg,
//      "SELECT SpecYear FROM CurrentSpecYear WHERE UserKey = %s ",
//      pglobSpecYr->pszUserKey);
 //     " SELECT %s "
 //     pglobSpecYr->pszCurrentSpecYear);
 //  pszTemp = ItiDbGetRow1Col1 (szMsg, hhpLoc, 0, 0, 0);

   pszTemp = pglobSpecYr->pszCurrentSpecYear;
   if (pszTemp != NULL) 
      ItiStrCat (szTitleBarString, pszTemp, sizeof szTitleBarString);
   else
      ItiStrCat (szTitleBarString, "1776", sizeof szTitleBarString);

   sprintf (szMsg, "MESSAGE: The SpecYear is now set to %s. ", pszTemp);
   ItiErrWriteDebugMessage (szMsg);

  // ItiMemFree (hhpLoc, pszTemp); -- psztemp no longer from getrow1col1

   if (hhpLoc != NULL)
      ItiMemDestroyHeap (hhpLoc);

   return TRUE;
   }/* End of Function SetTitleBarString */


BOOL EXPORT ToggleUnitSystem (void)
   {
   HWND hwndTitleBar;
   BOOL bMetricFlag;
   HHEAP hhpLoc;
   PGLOBALS pglobSpecYr = NULL;
   CHAR szQry[254] = "";
   //CHAR szMsg[254] = "";
   extern CHAR  szMetricYear[];
   extern CHAR  szEnglishYear[];

   hhpLoc = ItiMemCreateHeap (1024);

   pglobSpecYr = ItiGlobQueryGlobalsPointer();
   if ((NULL == pglobSpecYr) || (hhpLoc == NULL))
      {
      DosBeep(1200, 20); DosBeep(1300, 20); DosBeep(1400, 20);
      ItiErrWriteDebugMessage
         ("MSG: Could not access the GLOBALS pointer in SpecYear.ToggleUnitSystem ");
      WinMessageBox (HWND_DESKTOP, pglobSpecYr->hwndAppFrame,
                     "*** Unable to change the SpecBook at this time! ***",
                     "-- Spec Book Status --", 0, MB_OK | MB_ICONHAND);
      if (hhpLoc != NULL)
         ItiMemDestroyHeap (hhpLoc);
      return FALSE;
      }



   /* -- Toggle the metric flag: */
   bMetricFlag = !(pglobSpecYr->bUseMetric);  
   pglobSpecYr->bUseMetric = bMetricFlag;

   if (bMetricFlag)
      {
      DosBeep (1600, 10);
      sprintf (szQry,
          " /* specyear.c.ToggleUnitSystem */ "
          "IF EXISTS (select UserKey from UnitTypeSystem where UserKey = %s ) "
              " UPDATE UnitTypeSystem SET UnitType = 1 where UserKey = %s "
          " ELSE INSERT INTO UnitTypeSystem (UnitType, UserKey) VALUES (1, %s) ",
          pglobSpecYr->pszUserKey,
          pglobSpecYr->pszUserKey,
          pglobSpecYr->pszUserKey);
      ItiDbExecSQLStatement (hhpLoc, szQry);

      if (bMetricYearGiven)
         {
         /* -- Set the current SpecYear selection here. */
         ItiStrCpy (szQry, 
            "IF EXISTS (select UserKey from CurrentSpecYear where UserKey = ", sizeof szQry);
         ItiStrCat (szQry, pglobSpecYr->pszUserKey, sizeof szQry);
         ItiStrCat (szQry, " ) UPDATE CurrentSpecYear SET SpecYear = ", sizeof szQry);
         ItiStrCat (szQry, szMetricYear, sizeof szQry);
         ItiStrCat (szQry, " where UserKey = ", sizeof szQry);
         ItiStrCat (szQry, pglobSpecYr->pszUserKey, sizeof szQry);
         ItiStrCat (szQry, " ELSE INSERT INTO CurrentSpecYear (UserKey, SpecYear) VALUES ( ", sizeof szQry);
         ItiStrCat (szQry, pglobSpecYr->pszUserKey , sizeof szQry);
         ItiStrCat (szQry, " , ", sizeof szQry);
         ItiStrCat (szQry, szMetricYear, sizeof szQry);
         ItiStrCat (szQry, " ) ", sizeof szQry);
      
         if (ItiDbExecSQLStatement (hhpLoc, szQry))
            {
            ItiErrWriteDebugMessage ("ERROR: SQL execution problem. ");
            }
      
         pglobSpecYr->pszCurrentSpecYear = szMetricYear;
      
         ItiStrCpy (szQry, "MESSAGE: The SpecYear is now set to ", sizeof szQry);
         ItiStrCat (szQry, szMetricYear, sizeof szQry);
         ItiErrWriteDebugMessage (szQry);
      
         ItiDbUpdateBuffers("MajorItem");
         ItiDbUpdateBuffers("StandardItem");
         ItiDbUpdateBuffers("Job");
         ItiDbUpdateBuffers("Project");
         ItiDbUpdateBuffers("HistoricalProject");
         ItiDbUpdateBuffers("HistoricalProposal");
      
         } /* end if (bMetricYearGiv... */


      WinMessageBox (HWND_DESKTOP, pglobSpecYr->hwndAppFrame,
                     "The METRIC unit system is now being used.",
                     "-- Spec Book Status --", 0, MB_OK );
      ItiErrWriteDebugMessage (" MSG: The METRIC unit system is now in use.");
      }
   else
      {
      DosBeep (1680, 10);
      sprintf (szQry,
          " /* specyear.c.ToggleUnitSystem */ "
          "IF EXISTS (select UserKey from UnitTypeSystem where UserKey = %s ) "
              " UPDATE UnitTypeSystem SET UnitType = 0 where UserKey = %s "
          " ELSE INSERT INTO UnitTypeSystem (UnitType, UserKey) VALUES (0, %s) ",
          pglobSpecYr->pszUserKey,
          pglobSpecYr->pszUserKey,
          pglobSpecYr->pszUserKey);
      ItiDbExecSQLStatement (hhpLoc, szQry);

      if (bEnglishYearGiven)
         {
         /* -- Set the current SpecYear selection here. */
         ItiStrCpy (szQry, 
            "IF EXISTS (select UserKey from CurrentSpecYear where UserKey = ", sizeof szQry);
         ItiStrCat (szQry, pglobSpecYr->pszUserKey, sizeof szQry);
         ItiStrCat (szQry, " ) UPDATE CurrentSpecYear SET SpecYear = ", sizeof szQry);
         ItiStrCat (szQry, szEnglishYear, sizeof szQry);
         ItiStrCat (szQry, " where UserKey = ", sizeof szQry);
         ItiStrCat (szQry, pglobSpecYr->pszUserKey, sizeof szQry);
         ItiStrCat (szQry, " ELSE INSERT INTO CurrentSpecYear (UserKey, SpecYear) VALUES ( ", sizeof szQry);
         ItiStrCat (szQry, pglobSpecYr->pszUserKey , sizeof szQry);
         ItiStrCat (szQry, " , ", sizeof szQry);
         ItiStrCat (szQry, szEnglishYear, sizeof szQry);
         ItiStrCat (szQry, " ) ", sizeof szQry);
      
         if (ItiDbExecSQLStatement (hhpLoc, szQry))
            {
            ItiErrWriteDebugMessage ("ERROR: SQL execution problem. ");
            }
      
         pglobSpecYr->pszCurrentSpecYear = szEnglishYear;
      
         ItiStrCpy (szQry, "MESSAGE: The SpecYear is now set to ", sizeof szQry);
         ItiStrCat (szQry, szEnglishYear, sizeof szQry);
         ItiErrWriteDebugMessage (szQry);
      
         ItiDbUpdateBuffers("MajorItem");
         ItiDbUpdateBuffers("StandardItem");
         ItiDbUpdateBuffers("Job");
         ItiDbUpdateBuffers("Project");
         ItiDbUpdateBuffers("HistoricalProject");
         ItiDbUpdateBuffers("HistoricalProposal");
      
         } /* end if (bEnglishYearGiv... */

      WinMessageBox (HWND_DESKTOP, pglobSpecYr->hwndAppFrame,
                     "The ENGLISH unit system is now being used.",
                     "-- Spec Book Status --", 0, MB_OK );
      ItiErrWriteDebugMessage (" MSG: The ENGLISH unit system is now in use.");
      }


   /* -- Set the Application title bar. */
   SetTitleBarString ();
   hwndTitleBar = WinWindowFromID(pglobSpecYr->hwndAppFrame, FID_TITLEBAR);
   WinSetWindowText(hwndTitleBar, szTitleBarString);

   /* -- Update buffers that can be affected by the change. -------------- */
   ItiDbUpdateBuffers("Material");
   ItiDbUpdateBuffers("MaterialPrice");
   ItiDbUpdateBuffers("CostSheet");
   ItiDbUpdateBuffers("Job");
   ItiDbUpdateBuffers("MajorItem");
   ItiDbUpdateBuffers("StandardItem");
   ItiDbUpdateBuffers("Task");
   ItiDbUpdateBuffers("Project");
   ItiDbUpdateBuffers("HistoricalProposal");
   ItiDbUpdateBuffers("HistoricalProject");
   ItiDbUpdateBuffers("CostBasedEstimate");
   ItiDbUpdateBuffers("StdItemDefaultPrices");

   if (hhpLoc != NULL)
      ItiMemDestroyHeap (hhpLoc);

   return bMetricFlag;

   }/* End of Function ToggleUnitSystem */




BOOL EXPORT SetSpecYrTitle (void)
   {
   HWND hwndTitleBar;
   CHAR  szMsg[200] = "";
   PGLOBALS pglobSpecYr = NULL;

   pglobSpecYr = ItiGlobQueryGlobalsPointer();

   /* -- Set the Application title bar. */
   SetTitleBarString ();
   hwndTitleBar = WinWindowFromID(pglobSpecYr->hwndAppFrame, FID_TITLEBAR);
   WinSetWindowText(hwndTitleBar, szTitleBarString);

   ItiDbUpdateBuffers("StandardItem");
   ItiDbUpdateBuffers("Job");
   ItiDbUpdateBuffers("Project");
   ItiDbUpdateBuffers("HistoricalProject");
   ItiDbUpdateBuffers("HistoricalProposal");

   return TRUE;
   }/* End of Function SetSpecYrTitle */
