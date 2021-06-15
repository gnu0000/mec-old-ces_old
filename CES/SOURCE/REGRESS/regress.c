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


/* regress.c
 */

#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itiutil.h"
#include "..\include\itifmt.h"
#include "..\include\itiest.h"
#include "..\include\itirptut.h"
#include "..\include\itierror.h"
#include "..\include\itiglob.h"
#include "..\include\winid.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "regress.h"
#include "winreg.h"
#include "initdll.h"
#include <math.h>

#define QLEVEL_ERROR -1
#define MIN_QLEVEL   0
#define MAX_QLEVEL   5

#define DAYS_PER_MONTH     30.4375
#define DAYS_PER_YEAR      365.25


static  CHAR szTempGetAdj     [1024] = "";
static  CHAR szTempGetRegInfo [1024] = "";
static  CHAR szTempQry        [1024] = "";
static  CHAR szTempQryAvail   [1024] = "";


PGLOBALS pglobals;
DOUBLE   d;
DOUBLE   dCompPrice;
PSZ      pszReturnResult;


/*
 * ItiEstInstall Creates all the necessary tables, indicies, etc for both
 * the production database and import database.
 *
 * Returns TRUE on success, FALSE on error.
 */

BOOL EXPORT ItiEstInstall (PSZ szDatabaseName, 
                           PSZ pszImportDatabase)
   {
   return TRUE;
   }


/*
 * ItiEstDeinstall deletes all the necessary tables, indicies, etc for both
 * the production database and import database.
 *
 * Returns TRUE on success, FALSE on error.
 */

BOOL EXPORT ItiEstDeinstall (PSZ szDatabaseName, 
                             PSZ pszImportDatabase)
   {
   return TRUE;
   }


/*
 * ItiDllQueryVersion
 *
 */

USHORT EXPORT ItiDllQueryVersion (void)
   {
   return 1;
   }



/*
 * ItiEstTableChanged is used to tell an installable estimation method
 * that a table has changed.  
 *
 * Parameters: usTable        The ID of the table that has changed.
 *
 *             ausColumns     An array from 0..usNumColumns, that 
 *                            contain the IDs of the columns that
 *                            have changed.  If this parameter is
 *                            NULL, then all columns have changed.
 *
 *             usNumColumns   The number of columns in ausColumns.
 *
 *             pszKey         A pointer to a string that contains
 *                            the unique key.  The string is in the 
 *                            form "ColumnName = ColumnValue 
 *                            [AND ColumnName = ColumnValue]".
 */

void EXPORT ItiEstTableChanged (USHORT   usTable, 
                                USHORT   ausColumns [],
                                USHORT   usNumColumns,
                                PSZ      pszKey)
   {
//   if (usTable == tJob && 
//       (ItiEstSearchCol (ausColumns, usNumColumns, cLettingMonth) ||
//        ItiEstSearchCol (ausColumns, usNumColumns, cBaseDate) ||
//        ItiEstSearchCol (ausColumns, usNumColumns, cWorkType))
//      {
//
//      }
   }



static DOUBLE SignedLn (DOUBLE d)
   {
   if (d == 0)
      return 0;
   else if (d < 0)
      return -log (-d);
   else 
      return log (d);
   }





DOUBLE GetAdjustment (HHEAP   hheap,
                      PSZ     pszImportDate,
                      PSZ     pszStandardItemKey,
                      PSZ     pszAdjustmentType,
                      PSZ     pszAdjustmentKeyColumn,
                      PSZ     pszAdjustmentKeyValue)
   {
   // char szTempGetAdj [1024] = "";
   // DOUBLE d;
   PSZ psz;

   ItiStrCpy (szTempGetAdj,
              "/* GetAdjustment */ SELECT Adjustment FROM ", sizeof szTempGetAdj);
   ItiStrCat (szTempGetAdj, pszAdjustmentType, sizeof szTempGetAdj);
   ItiStrCat (szTempGetAdj,
            "Adjustment ADJ, PEMETHRegression PR "
            "WHERE PR.StandardItemKey = ", sizeof szTempGetAdj);
   ItiStrCat (szTempGetAdj, pszStandardItemKey, sizeof szTempGetAdj);
   ItiStrCat (szTempGetAdj,
            " AND  PR.StandardItemKey = ADJ.StandardItemKey "
            " AND  PR.ImportDate = ADJ.ImportDate "
            " AND  PR.ImportDate = '", sizeof szTempGetAdj);
   ItiStrCat (szTempGetAdj, pszImportDate, sizeof szTempGetAdj);
   ItiStrCat (szTempGetAdj, "' AND ", sizeof szTempGetAdj);
   ItiStrCat (szTempGetAdj, pszAdjustmentKeyColumn, sizeof szTempGetAdj);
   ItiStrCat (szTempGetAdj, " = ", sizeof szTempGetAdj);
   ItiStrCat (szTempGetAdj, pszAdjustmentKeyValue, sizeof szTempGetAdj);
             

   psz = ItiDbGetRow1Col1 (szTempGetAdj, hheap, 0, 0, 0);
   if (psz == NULL)
      return 0;

   d = ItiStrToDouble (psz);

   ItiMemFree (hheap, psz);
   return d;
   }


static BOOL GetRegressionInfo (HHEAP   hheap,
                               PSZ     pszBaseDate,
                               PSZ     pszStandardItemKey,
                               PDOUBLE pdBasePrice,
                               PDOUBLE pdAnnualInflation,
                               PDOUBLE pdQuantityAdjustment,
                               PDOUBLE pdMedianQuantity,
                               PSZ     *ppszImportDate, 
                               PSZ     *ppszAreaTypeKey)
   {
//   char     szTempGetRegInfo [1024];
   PSZ      *ppsz;
   USHORT   usNumCols;

   *pdBasePrice = 0;
   *pdAnnualInflation = 0;
   *pdQuantityAdjustment = 0;
   *pdMedianQuantity = 0;
   *ppszImportDate = NULL;
   *ppszAreaTypeKey = NULL;

   ItiStrCpy (szTempGetRegInfo,
            "/* GetRegressionInfo */ "
            "SELECT PR.BaseUnitPrice, PR.AnnualInflation, "
            "PR.QuantityAdjustment, PR.MedianQuantity, "
            "PR.ImportDate, PR.AreaTypeKey "
            "FROM StandardItemPEMETHRegression SIPR, PEMETHRegression PR "
            "WHERE PR.StandardItemKey = ", sizeof szTempGetRegInfo);
   ItiStrCat (szTempGetRegInfo, pszStandardItemKey, sizeof szTempGetRegInfo);
   ItiStrCat (szTempGetRegInfo,
            " AND SIPR.StandardItemKey = PR.StandardItemKey "
            "AND SIPR.ImportDate = PR.ImportDate "
            "AND SIPR.BaseDate = '", sizeof szTempGetRegInfo);
   ItiStrCat (szTempGetRegInfo, pszBaseDate, sizeof szTempGetRegInfo);
   ItiStrCat (szTempGetRegInfo, "' ", sizeof szTempGetRegInfo);


   ppsz = ItiDbGetRow1 (szTempGetRegInfo, hheap, 0, 0, 0, &usNumCols);
   if (ppsz == NULL)
      {
#ifdef DEBUG
      ItiErrWriteDebugMessage (szTempGetRegInfo);
#endif
      return FALSE;
      }

   *pdBasePrice = ItiStrToDouble (ppsz [0]);
   *pdAnnualInflation = ItiStrToDouble (ppsz [1]);
   *pdQuantityAdjustment = ItiStrToDouble (ppsz [2]);
   *pdMedianQuantity = ItiStrToDouble (ppsz [3]);
   *ppszImportDate = ItiStrDup (hheap, ppsz [4]);
   *ppszAreaTypeKey = ItiStrDup (hheap, ppsz [5]);

   ItiFreeStrArray (hheap, ppsz, usNumCols);
   return TRUE;
   }



DOUBLE CalcReg (PSZ     pszBaseDate,
                PSZ     pszImportDate,
                DOUBLE  dBasePrice,
                DOUBLE  dQuantity,
                DOUBLE  dMedianQuantity,
                DOUBLE  dQuantityAdjustment,
                DOUBLE  dAnnualInflation,
                DOUBLE  dAreaAdj,
                DOUBLE  dWorkTypeAdj,
                DOUBLE  dSeasonAdj)
   {
   DOUBLE dExponent, dYears;
   USHORT usDays;

   /* assumes base date always >= import date (the function returns the 
      absolute value of the arithmetic) */
   usDays = ItiRptUtDateDiff (pszBaseDate, pszImportDate);
   dYears = (DOUBLE) usDays / DAYS_PER_YEAR;
   dExponent = SignedLn (dQuantity / dMedianQuantity) / NATLOG_2;

   dCompPrice  = dBasePrice;

   dCompPrice  = dCompPrice * pow (1.0 + dQuantityAdjustment, dExponent);
   dCompPrice  = dCompPrice * pow (1.0 + dAnnualInflation, dYears);

   dCompPrice += dCompPrice * dAreaAdj;
   dCompPrice += dCompPrice * dWorkTypeAdj;
   dCompPrice += dCompPrice * dSeasonAdj;

   return dCompPrice;
   }


PSZ CalculateRegression (HHEAP  hheap, 
                         PSZ    pszJobKey,
                         PSZ    pszWorkType, 
                         PSZ    pszStandardItemKey,
                         PSZ    pszBaseDate,
                         PSZ    pszQuantity,
                         PSZ    pszMonth)
   {
   DOUBLE   dArea, dWorkType, dSeason, dQuantityAdjustment, dAnnualInflation;
   DOUBLE   dBasePrice, dQuantity, dPrice, dMedianQuantity;
   char     szSeason [2];
   PSZ      pszImportDate;
   USHORT   usMonth;
   PSZ      pszArea, pszAreaTypeKey;

   if (!ItiStrToUSHORT (pszMonth, &usMonth))
      usMonth = 0;
   szSeason [0] = (char) (usMonth / 4) + (char) '1';
   if (szSeason [0] > '4')
      szSeason [0] = '4';
   else if (szSeason [0] < '0')
      szSeason [0] = '0';
   szSeason [1] = '\0';

   if (!GetRegressionInfo (hheap, pszBaseDate, pszStandardItemKey,
                      &dBasePrice, &dAnnualInflation, &dQuantityAdjustment,
                      &dMedianQuantity, &pszImportDate, &pszAreaTypeKey))
      return NULL;

   pszArea = ItiEstGetJobArea (hheap, pszJobKey, pszAreaTypeKey, pszBaseDate);

   dArea = GetAdjustment (hheap, pszImportDate, pszStandardItemKey, 
                          "Area", "AreaKey", pszArea);

   dWorkType = GetAdjustment (hheap, pszImportDate, pszStandardItemKey, 
                              "WorkType", "WorkType", pszWorkType);

   dSeason = GetAdjustment (hheap, pszImportDate, pszStandardItemKey, 
                            "Season", "Season", szSeason);

   dQuantity = ItiStrToDouble (pszQuantity);

   dPrice = CalcReg (pszBaseDate, pszImportDate, dBasePrice, dQuantity,
                     dMedianQuantity, dQuantityAdjustment, dAnnualInflation,
                     dArea, dWorkType, dSeason);

   ItiMemFree (hheap, pszImportDate);
   ItiMemFree (hheap, pszAreaTypeKey);
   ItiMemFree (hheap, pszArea);


//   this was the old code:  -- mdh 9/23/1992
//   sprintf (szTemp, "$%.4lf", dPrice);
//   return ItiStrDup (hheap, szTemp);

//   this is the corrected code: -- mdh 9/23/1992
   pszReturnResult = ItiEstRoundUnitPrice (hheap, dPrice, pszStandardItemKey);
   return pszReturnResult;
   }






static BOOL  ItiEstGetJobInfo (HHEAP hheap, 
                              PSZ   pszJobKey, 
                              PSZ   *ppszBaseDate, 
                              PSZ   *ppszMonth)
   {
   char     szTemp [128] = "";
   PSZ      *ppsz;
   USHORT   usNumCols;

   if (pszJobKey == NULL)
      return FALSE;

   if (ppszBaseDate != NULL)
      *ppszBaseDate = '\0';

   if (ppszMonth != NULL)
      *ppszMonth = '\0';

   ItiStrCpy (szTemp, 
            "SELECT BaseDate, LettingMonth "
            "FROM Job "
            "WHERE JobKey = ", sizeof szTemp);

   ItiStrCat (szTemp,  pszJobKey, sizeof szTemp);

   ppsz = ItiDbGetRow1 (szTemp, hheap, 0, 0, 0, &usNumCols);
   if (ppsz == NULL)
      return FALSE;

   if (ppszBaseDate != NULL)
      *ppszBaseDate = ppsz [0];

   if (ppszMonth != NULL)
      *ppszMonth = ppsz [1];

   return TRUE;
   }


/*
 * ItiEstItem returns a string to be used as the unit price for
 * a given standard item.  This function is usefull when the item has
 * not yet been added to the data base.
 *
 * Parameters: hheap                A heap to allocate from.
 *
 *             pszBaseDate          The base date for the price.  This
 *                                  parameter may be null iff pszJobKey
 *                                  is not NULL.  If this parameter and 
 *                                  pszJobKey are supplied, pszBaseDate
 *                                  overrides the Job's base date.
 *
 *             pszJobKey            The job that the item is in. 
 *                                  If the item is not in a job, 
 *                                  set this parameter to NULL.
 *
 *             pszJobBreakdownKey   The job breakdown that the
 *                                  item is in.  If the item is not
 *                                  in a job, set this parameter to NULL.
 *
 *             pszStandardItemKey   The standard item to price.  This
 *                                  parameter may not be NULL.
 *
 *             pszQuantity          The quantity for this item.  If the
 *                                  quantity is unknown, use NULL.
 *
 * Return value:  A pointer to a null terminated string containing the
 * unit price.  The string is allocated from hheap.  Special return
 * values:
 *
 *    NULL:          An error occured estimating the item.
 *
 *    empty string:  Not enough information was supplied to estimate
 *                   the given item.
 *
 * Notes:  The caller is responsible for freeing the unit price from the
 * heap.
 *
 * The unit price is rounded according to the RoundingPrecision field
 * in the standard item catalog.
 */ 

PSZ EXPORT ItiEstItem (HHEAP hheap,
                       PSZ   pszBaseDate,
                       PSZ   pszJobKey,
                       PSZ   pszJobBreakdownKey,
                       PSZ   pszStandardItemKey,
                       PSZ   pszQuantity)
   {
   PSZ      pszPrice, pszWorkType;
   PSZ      pszTempBaseDate, pszMonth;

   if (pszStandardItemKey == NULL || 
       (pszJobKey == NULL && pszBaseDate == NULL))
      return NULL;

   if (!ItiEstGetJobInfo (hheap, pszJobKey, &pszTempBaseDate, &pszMonth))
      return NULL;

   if (pszBaseDate == NULL)
      pszBaseDate = pszTempBaseDate;

   if (pszQuantity == NULL)
      return ItiStrDup (hheap, "");

   pszPrice = NULL;

   pszWorkType = ItiEstGetJobWorkType (hheap, pszJobKey, pszJobBreakdownKey);

   pszPrice = CalculateRegression (hheap, pszJobKey, pszWorkType,  
                                   pszStandardItemKey, pszBaseDate, 
                                   pszQuantity, pszMonth);

   ItiMemFree (hheap, pszTempBaseDate);
   ItiMemFree (hheap, pszMonth);
   ItiMemFree (hheap, pszWorkType);

   return pszPrice;
   }




VOID EXPORT ItiEstQueryMenuName (PSZ      pszBuffer, 
                                 USHORT   usMaxSize,
                                 PUSHORT  pusWindowID)
   {
   ItiStrCpy (pszBuffer, "PEMETH ~Regression", usMaxSize);
   *pusWindowID = JobPemethRegS;
   }



BOOL EXPORT ItiEstQueryJobAvail (HHEAP hheap, 
                                 PSZ   pszJobKey,
                                 PSZ   pszJobItemKey)
   {
   // char szTempQry [1024];
   PSZ psz;

   if (pszJobKey == NULL || pszJobItemKey == NULL)
      return FALSE;

   ItiStrCpy (szTempQry,
            "/* ItiEstQueryJobAvail */ SELECT count(SIPR.ImportDate) "
            "FROM StandardItemPEMETHRegression SIPR, "
            "Job J, JobItem JI  WHERE JI.JobItemKey = ", sizeof szTempQry);
   ItiStrCat (szTempQry,  pszJobItemKey, sizeof szTempQry);
   ItiStrCat (szTempQry, " AND J.JobKey = ", sizeof szTempQry);
   ItiStrCat (szTempQry,  pszJobKey, sizeof szTempQry);
   ItiStrCat (szTempQry,  
            " AND J.JobKey = JI.JobKey "
            "AND SIPR.StandardItemKey = JI.StandardItemKey "
            "AND SIPR.BaseDate = J.BaseDate ", sizeof szTempQry);

   psz = ItiDbGetRow1Col1 (szTempQry, hheap, 0, 0, 0);
   if ((psz == NULL) || ( (*psz) == '0'))
      return FALSE;

   ItiMemFree (hheap, psz);
   return TRUE;
   }


BOOL EXPORT ItiEstQueryAvail (HHEAP hheap, 
                              PSZ   pszStandardItemKey,
                              PSZ   pszBaseDate)
   {
   // char szTempQryAvail [1024];
   PSZ psz;

   if (NULL == hheap)
      return FALSE;

   if ((pszStandardItemKey == NULL) || (pszStandardItemKey[0] == '\0') )
      return FALSE;

   if (pszBaseDate == NULL)
      pszBaseDate = UNDECLARED_BASE_DATE;

 //  ItiStrCpy (szTempQryAvail,
 //           "/* regress.c.ItiEstQueryAvail */ "
 //           "SELECT count(SIPR.ImportDate) "
 //           "FROM StandardItemPEMETHRegression SIPR "
 //           "WHERE SIPR.StandardItemKey = ", sizeof szTempQryAvail);
 //  ItiStrCat (szTempQryAvail, pszStandardItemKey, sizeof szTempQryAvail);
 //  ItiStrCat (szTempQryAvail, " AND (SIPR.BaseDate = '", sizeof szTempQryAvail);
 //  ItiStrCat (szTempQryAvail, pszBaseDate, sizeof szTempQryAvail);
 //  ItiStrCat (szTempQryAvail, "' OR SIPR.BaseDate = (select MAX(BaseDate) from StandardItemPEMETHRegression) ) ", sizeof szTempQryAvail);

   ItiStrCpy (szTempQryAvail,
            "/* regress.c.ItiEstQueryAvail */ "
            "SELECT MAX(BaseDate) "
            "FROM StandardItemPEMETHRegression SIPR "
            "WHERE SIPR.StandardItemKey = ", sizeof szTempQryAvail);
   ItiStrCat (szTempQryAvail, pszStandardItemKey, sizeof szTempQryAvail);


   psz = ItiDbGetRow1Col1 (szTempQryAvail, hheap, 0, 0, 0);
   if ((psz == NULL) || ( (*psz) == '0'))
      return FALSE;

   ItiMemFree (hheap, psz);
   return TRUE;
   }





BOOL EXPORT ItiDllInitDll (void)
   {
   static BOOL bInited = FALSE;

   if (bInited)
      return TRUE;

   bInited = TRUE;

   pglobals = ItiGlobQueryGlobalsPointer ();

   if (ItiMbRegisterStaticWnd (JobPemethRegS, JobPemethRegSProc, hmod))
      return FALSE;
   if (ItiMbRegisterStaticWnd (PemethRegS, PemethRegSProc, hmod))
      return FALSE;
   return TRUE;
   }


