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
#include "bamsdss.h"
#include "SpecYear.h"
#include "dialog.h"
#include <stdio.h>
#include <process.h>
#include <errno.h>
#include <string.h>

CHAR  szMetricYear[YEARLEN] = "" ;
CHAR  szEnglishYear[YEARLEN] = "" ;
BOOL bEnglishYearGiven, bMetricYearGiven;

#define SPECYEAR_STACKSIZE 16386

BOOL EXPORT SetTitleBarString (void)
   {
   CHAR  szMsg[256] = "";
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


// BOOL EXPORT ToggleUnitSystem (void)
//    {
//    HWND hwndTitleBar;
//    BOOL bMetricFlag;
//    HHEAP hhpLoc;
//    PGLOBALS pglobSpecYr = NULL;
//    CHAR szQry[256] = "";
// 
//    hhpLoc = ItiMemCreateHeap (1024);
// 
//    pglobSpecYr = ItiGlobQueryGlobalsPointer();
//    if ((NULL == pglobSpecYr) || (hhpLoc == NULL))
//       {
//       DosBeep(1200, 20); DosBeep(1300, 20); DosBeep(1400, 20);
//       ItiErrWriteDebugMessage
//          ("MSG: Could not access the GLOBALS pointer in SpecYear.ToggleUnitSystem ");
//       WinMessageBox (HWND_DESKTOP, pglobSpecYr->hwndAppFrame,
//                      "*** Unable to change the SpecBook at this time! ***",
//                      "-- Spec Book Status --", 0, MB_OK | MB_ICONHAND);
//       if (hhpLoc != NULL)
//          ItiMemDestroyHeap (hhpLoc);
//       return FALSE;
//       }
// 
//    /* -- Toggle the metric flag: */
//    bMetricFlag = !(pglobSpecYr->bUseMetric);  
//    pglobSpecYr->bUseMetric = bMetricFlag;
// 
//    if (bMetricFlag)
//       {
//       DosBeep (1600, 10);
//       sprintf (szQry,
//           " /* specyear.c.ToggleUnitSystem */ "
//           "IF EXISTS (select UserKey from UnitTypeSystem where UserKey = %s ) "
//               " UPDATE UnitTypeSystem SET UnitType = 1 where UserKey = %s "
//           " ELSE INSERT INTO UnitTypeSystem (UnitType, UserKey) VALUES (1, %s) ",
//           pglobSpecYr->pszUserKey,
//           pglobSpecYr->pszUserKey,
//           pglobSpecYr->pszUserKey);
//       ItiDbExecSQLStatement (hhpLoc, szQry);
// 
//       WinMessageBox (HWND_DESKTOP, pglobSpecYr->hwndAppFrame,
//                      "The METRIC unit system is now being used.",
//                      "-- Spec Book Status --", 0, MB_OK );
//       ItiErrWriteDebugMessage (" MSG: The METRIC unit system is now in use.");
//       }
//    else
//       {
//       DosBeep (1680, 10);
//       sprintf (szQry,
//           " /* specyear.c.ToggleUnitSystem */ "
//           "IF EXISTS (select UserKey from UnitTypeSystem where UserKey = %s ) "
//               " UPDATE UnitTypeSystem SET UnitType = 0 where UserKey = %s "
//           " ELSE INSERT INTO UnitTypeSystem (UnitType, UserKey) VALUES (0, %s) ",
//           pglobSpecYr->pszUserKey,
//           pglobSpecYr->pszUserKey,
//           pglobSpecYr->pszUserKey);
//       ItiDbExecSQLStatement (hhpLoc, szQry);
// 
// 
//       WinMessageBox (HWND_DESKTOP, pglobSpecYr->hwndAppFrame,
//                      "The ENGLISH unit system is now being used.",
//                      "-- Spec Book Status --", 0, MB_OK );
//       ItiErrWriteDebugMessage (" MSG: The ENGLISH unit system is now in use.");
//       }
// 
// 
//    /* -- Set the Application title bar. */
//    SetTitleBarString ();
//    hwndTitleBar = WinWindowFromID(pglobSpecYr->hwndAppFrame, FID_TITLEBAR);
//    WinSetWindowText(hwndTitleBar, szTitleBarString);
// 
//    /* -- Update buffers that can be affected by the change. -------------- */
//    ItiDbUpdateBuffers("Material");
//    ItiDbUpdateBuffers("MaterialPrice");
//    ItiDbUpdateBuffers("CostSheet");
//    ItiDbUpdateBuffers("Job");
//    ItiDbUpdateBuffers("MajorItem");
//    ItiDbUpdateBuffers("StandardItem");
//    ItiDbUpdateBuffers("Task");
//    ItiDbUpdateBuffers("Project");
//    ItiDbUpdateBuffers("HistoricalProposal");
//    ItiDbUpdateBuffers("HistoricalProject");
//    ItiDbUpdateBuffers("CostBasedEstimate");
//    ItiDbUpdateBuffers("StdItemDefaultPrices");
// 
//    if (hhpLoc != NULL)
//       ItiMemDestroyHeap (hhpLoc);
// 
//    return bMetricFlag;
// 
//    }/* End of Function ToggleUnitSystem */





BOOL EXPORT ToggleUnitSystem (void)
   {
   HWND hwndTitleBar;
   BOOL bMetricFlag;
   HHEAP hhpLoc;
   PGLOBALS pglobSpecYr = NULL;
   CHAR szQry[256] = "";
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


 //  /* -- Set the Application title bar. */
   SetTitleBarString ();
   hwndTitleBar = WinWindowFromID(pglobSpecYr->hwndAppFrame, FID_TITLEBAR);
   WinSetWindowText(hwndTitleBar, szTitleBarString);

 //  /* -- Update buffers that can be affected by the change. -------------- */
 //  ItiDbUpdateBuffers("Material");
 //  ItiDbUpdateBuffers("MaterialPrice");
 //  ItiDbUpdateBuffers("CostSheet");
 //  ItiDbUpdateBuffers("Job");
 //  ItiDbUpdateBuffers("MajorItem");
 //  ItiDbUpdateBuffers("StandardItem");
 //  ItiDbUpdateBuffers("Task");
 //  ItiDbUpdateBuffers("Project");
 //  ItiDbUpdateBuffers("HistoricalProposal");
 //  ItiDbUpdateBuffers("HistoricalProject");
 //  ItiDbUpdateBuffers("CostBasedEstimate");
 //  ItiDbUpdateBuffers("StdItemDefaultPrices");

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
