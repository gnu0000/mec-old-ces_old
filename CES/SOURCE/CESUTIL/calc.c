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
 *  cesutil\calc.c
 * Mark Hampton
 * Timothy Blake
 */

#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itierror.h"
#include "..\include\itidbase.h"
#include "..\include\colid.h"
#include "..\include\itiest.h"
#include "..\include\cbest.h"
#include "..\bamsces\calcfn.h"
#include "..\include\itibase.h"
#include "..\include\itiperm.h"
#include "..\include\itiglob.h"
#include "..\include\itiwin.h"
#include "..\include\winid.h"
#include "..\include\itifmt.h"
#include "..\include\itiutil.h"
#include "..\include\cesutil.h"
#include "..\cbest\calcfn.h"
#include "init.h"
#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include <stddef.h>

#define UNFUNDEDKEY    1L
#define KEYLENGTH      10
#define RETURN_ERROR   13
#define MAXCREWSPERJCS    20
#define MAXLABORERSPERJCS 50
#define MAXEQUIPSPERJCS   50

static  CHAR szQryStr      [1000] = "";
static  CHAR szTmpCJIC     [1200] = "";
static  CHAR szPatCJB       [750] = "";
static  CHAR szTmpCJB       [750] = "";
static  CHAR szCJICJQry     [800] = "";
static  CHAR szJobKey        [15] = "";
static  CHAR szRecalcBuffer [510] = "";

USHORT EXPORT ErrCalc (PSZ psz1, PSZ psz2)
   {
   ItiErrWriteDebugMessage ("\n *** Error, problems with: ");
   ItiErrWriteDebugMessage (psz1);
   ItiErrWriteDebugMessage (psz2);
   ItiErrWriteDebugMessage (" *** \n ");
   DosBeep (1502, 22);
   return 1;
   }







BOOL GetJobKeysForProgram(HHEAP hheap, PSZ pszProgramKey, PSZ **pppszJobKeys)
   {
   char    szTemp [120] = "";
   USHORT  usErr;
   USHORT  usCols [] = {1,0};

   /* This query obtains all of the JobKeys     */
   /* associated with a Program and stores them in an array  */
   ItiStrCpy (szTemp, "SELECT JobKey " 
                      "FROM ProgramJob "
                      "WHERE ProgramKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszProgramKey, sizeof szTemp);

   usErr = ItiDbBuildSelectedKeyArray2(hheap, 0, szTemp,
                                       pppszJobKeys, usCols);
   if (usErr != 0)
      return FALSE;

   return TRUE;
   }



BOOL GetProgramFundKeys(HHEAP hheap, PSZ pszProgramKey, PSZ **pppszPFKeys)
   {
   char    szTemp [120] = "";
   USHORT  usErr;
   USHORT  usCols [] = {1,0};

   /* This query obtains all of the FundKeys associated with a Program  */
   /* and stores the keys in an array                                   */

   ItiStrCpy (szTemp, "SELECT FundKey " 
                      "FROM ProgramFund "
                      "WHERE ProgramKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszProgramKey, sizeof szTemp);

   usErr = ItiDbBuildSelectedKeyArray2(hheap, 0, szTemp, pppszPFKeys, usCols);
   if (usErr != 0)
      return FALSE;

   return TRUE;
   }



/* ---------------------------------------------------------------------*
 * Calculates the JobItem UnitPrice and ExtendedAmount                  *
 *                                                                      *
 * Needs: JobKey                                                        *
 *        JobItemKey                                                    *
 *                                                                      *
 ---------------------------------------------------------------------- */

int  EXPORT CalcJobItemUnitPrice (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
   char   szTemp  [510] = "";
   char   szTemp2 [510] = ""; /* was 1024 */
   CHAR   szZero[8] = " 0.0 ";
   CHAR   szXPriceFmt[16] = "";
   USHORT usRet, usLen;
   PSZ    pszXPrice, pszStdItemKey;
   DOUBLE dXPrice = 0.0;
   HHEAP  hhpLocal;

   hhpLocal = ItiMemCreateHeap (6140);

   usRet = ItiStrReplaceParams (szTemp,
                    "/* CESUtil.calc.CalcJobItemUnitPrice */ "
                    " SELECT SUM (QuantityPerItemUnit * UnitPrice) "
                    " FROM JobCostSheet JCS, JobCostBasedEstimate JCBE "
                    " WHERE JCS.JobKey= %JobKey "
                    " AND JCS.JobItemKey = %JobItemKey "
                    " AND JCS.JobItemCostBasedEstimateKey = "
                        " JCBE.JobItemCostBasedEstimateKey "
                    " AND JCBE.JobKey = %JobKey "
                    " AND JCBE.JobItemKey = %JobItemKey "
                    " AND JCBE.Active = 1 ",
              sizeof szTemp,
              ppszPNames,
              ppszParams,
              100);

   pszXPrice = ItiDbGetRow1Col1(szTemp, hhpLocal, 0, 0, 0);

   if (pszXPrice == NULL)
      {
      ItiErrWriteDebugMessage
         ("WARNING: cesutil calc CalcJobItemUnitPrice pszXPrice was NULL, from: ");
      ItiErrWriteDebugMessage (szTemp);
      pszXPrice = szZero;
      }
   else
      {
      dXPrice = ItiStrToDouble (pszXPrice);
// ItiErrWriteDebugMessage (" *** cesutil.calc *** CALCed JOB ITEM UNIT PRICE: ***** ");
// ItiErrWriteDebugMessage (pszXPrice);
// ItiErrWriteDebugMessage (szTemp);
      if (NULL != pszXPrice)
         ItiMemFree (hhpLocal, pszXPrice);
      ItiStrReplaceParams (szTemp, 
                           " SELECT StandardItemKey"
                           " FROM JobItem"
                           " WHERE JobItemKey = %JobItemKey"
                           " AND JobKey = %JobKey",
                           sizeof szTemp, ppszPNames, ppszParams, 100);
      pszStdItemKey = ItiDbGetRow1Col1 (szTemp, hhpLocal, 0, 0, 0);
      pszXPrice = ItiEstRoundUnitPrice (hhpLocal, dXPrice, pszStdItemKey);
      if (pszStdItemKey != NULL)
         {
         ItiMemFree (hhpLocal, pszStdItemKey);
         pszStdItemKey = NULL;
         }
      }

   ItiFmtFormatString (pszXPrice, szXPriceFmt, sizeof szXPriceFmt, "Number,......", &usLen);
//   ItiErrWriteDebugMessage (szXPriceFmt);

   ItiStrCpy (szTemp2,
              " /* CESUtil.calc.CalcJobItemUnitPrice */ "
              "UPDATE JobItem "
              " SET UnitPrice = ", sizeof szTemp2);
   ItiStrCat (szTemp2, szXPriceFmt, sizeof szTemp2);
   ItiStrCat (szTemp2,
              " WHERE JobKey = %JobKey "
              " AND JobItemKey = %JobItemKey "
              "UPDATE JobItem"
              " SET ExtendedAmount = ", sizeof szTemp2);
   ItiStrCat (szTemp2, szXPriceFmt, sizeof szTemp2);
   ItiStrCat (szTemp2,
                    " * (SELECT Quantity  "
                    " FROM JobItem "
                    " WHERE JobKey= %JobKey "
                    " AND JobItemKey = %JobItemKey)"
              " WHERE JobKey = %JobKey "
              " AND JobItemKey = %JobItemKey ", sizeof szTemp2);

 ItiErrWriteDebugMessage (szTemp);
   usRet = ItiStrReplaceParams (szTemp, szTemp2,
              sizeof szTemp,
              ppszPNames,
              ppszParams,
              100);

   if (usRet)
      {
      if (NULL != hhpLocal)
         ItiMemDestroyHeap (hhpLocal);
      return 1;
      }

   ItiDbBeginTransaction (hhpLocal);
   if (ItiDbExecSQLStatement (hhpLocal, szTemp))
      {
      ItiDbRollbackTransaction (hhpLocal);
      if (NULL != hhpLocal)
         ItiMemDestroyHeap (hhpLocal);
      return 1;
      }
   ItiDbCommitTransaction (hhpLocal);

   if (NULL != hhpLocal)
      ItiMemDestroyHeap (hhpLocal);
   return 0;
   } /* End Of Function CalcJobItemUnitPrice */



// *** New Calcs ***************************************************

 SHORT EXPORT LocalStrCmp (PSZ pszStr1, PSZ pszStr2)
    {
    while ('\0' != *pszStr1 && '\0' != *pszStr2 && *pszStr1 == *pszStr2)
       {
       pszStr1++;
       pszStr2++;
       }
    return ((*pszStr1 - *pszStr2) == 0) ? 0 : 
           (((*pszStr1 - *pszStr2)  < 0) ? -1 : 1);
    }
 
 
 
 PSZ EXPORT LocalStrCpy (PSZ pszDest, PSZ pszSource, USHORT usDestMax)
    {
    USHORT   i;
 
    if ((pszSource == NULL) || (pszDest == NULL))
       {
       pszDest = NULL;
       return pszDest;
       }
 
    for (i=0; i < usDestMax && pszSource [i] != '\0'; i++)
       {
       pszDest [i] = pszSource [i];
       }
    i = (i > usDestMax - 1) ? usDestMax - 1 : i;
    pszDest [i] = '\0';
    return pszDest;
    }




int EXPORT CalcJobItemActiveCostSht (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {    //Misnamed, calcs (active?) CBE that can have many job cost sheets.
   HQRY    hqry;
//   HHEAP   hhpLocal;
   USHORT  uRet, us, uErr, uCols, usCnt;
   char    szJobKey [15] = "";
   char    szCBEKey [15] = "";
   char    szTmp [250] = "";
   char    aszJCSKeys [50][KEYLENGTH];
   PSZ     pszRes = NULL;
   PSZ     *ppszTmp, *ppszLocPNames, *ppszLocParams;


   uRet = ItiStrReplaceParams (szCBEKey, "%JobItemCostBasedEstimateKey",
             sizeof szTmp, ppszPNames, ppszParams, 100);
   if (uRet)
      {
      /* This is not an error if the Item's estimation method is not CBE. */
      ItiErrWriteDebugMessage
        ("JobItemCostBasedEstimateKey NOT available for CESUtil.calc.CalcJobItemActiveCostSht.");
      return 1;
      }

   uRet = ItiStrReplaceParams (szJobKey, " %JobKey ",
             sizeof szTmp, ppszPNames, ppszParams, 100);
   if (uRet)
      {
      ItiErrWriteDebugMessage
        ("JobKey NOT available for CESUtil.calc.CalcJobItemActiveCostSht.");
      return 1;
      }

//   hhpLocal = ItiMemCreateHeap (MAX_HEAP_SIZE);
//   if (hhpLocal == NULL)
//      {
//      ItiErrWriteDebugMessage
//         ("CESUtil calc CalcJobItemActiveCostSht failed to create hhpLocal.");
//      return 13;
//      }



   uRet = ItiStrReplaceParams (szTmp,
             "/* CESUtil.Calc.CalcJobItemActiveCostSht */ "
             "SELECT JobCostSheetKey "
             " FROM JobCostSheet "
             " WHERE JobKey = %JobKey "
             " AND JobItemKey = %JobItemKey "
             " AND JobItemCostBasedEstimateKey = %JobItemCostBasedEstimateKey ",
             sizeof szTmp, ppszPNames, ppszParams, 100);

   ItiDbBeginTransaction (hheap);
   if (!(hqry = ItiDbExecQuery(szTmp, hheap, 0, 0, 0, &uCols, &uErr)))
      {
      return 13;
      }


   aszJCSKeys[0][0] = '\0';
   usCnt = 0;
   while (ItiDbGetQueryRow(hqry, &ppszTmp, &uErr)) 
      {
      ItiStrCpy (aszJCSKeys[usCnt], ppszTmp[0], KEYLENGTH);
      usCnt++;
      aszJCSKeys[usCnt][0] = '\0';
      }

   usCnt = 0;
   while (aszJCSKeys[usCnt][0] != '\0')
      {
      us = ReCalcJobCstShtCrew (szJobKey, aszJCSKeys[usCnt]);
      us = us + ReCalcJobCstShtMaterials (szJobKey, aszJCSKeys[usCnt]);
      if (us != 0)
         {
         ErrCalc ("Failed func for CalcJobItemActiveCostSht", szTmp);
         ItiDbRollbackTransaction (hheap);
         return 13;
         }

      ppszLocPNames = ItiStrMakePPSZ (hheap, 3, "JobKey", "JobCostSheetKey",  NULL);
      ppszLocParams = ItiStrMakePPSZ (hheap, 3, szJobKey, aszJCSKeys[usCnt],         NULL);

      /* -- Now recalc the Job Cost Sheet's ProductionRate. */
      CalcJobCostShtProductionRate (hheap, ppszLocPNames, ppszLocParams);

      CalcJobCostShtUnitPrice (hheap, ppszLocPNames, ppszLocParams);

      ItiFreeStrArray (hheap, ppszLocPNames, 3);
      ItiFreeStrArray (hheap, ppszLocParams, 3);
      usCnt++;
      }


   ItiDbUpdateBuffers ("JobCostSheet");
   ItiDbUpdateBuffers ("Task");
   ItiDbCommitTransaction (hheap);

//   if (NULL != hhpLocal)
//      ItiMemDestroyHeap (hhpLocal);
   return 0;
  } /* End of Function CalcJobItemActiveCostSht */





/* Calculates the Unit Price and ExtendedAmount
 * of the JobItem 
 * This function is called when a job item is added
 * to the job, and after all the associated CostBasedEstimates,
 * CostSheets, Crews, Materials, Equipment, Laborers, etc. have been copied
 *
 * Needs: JobKey
 *        JobItemKey
 */

int EXPORT CalcJobItemChg (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
   USHORT  uRet = 0;
   PSZ  pszTestKeyValue = NULL;
   CHAR szJobKey[15] = "";
//   CHAR szTmpCJIC [1224] = "";
//   CHAR szQryStr  [1024] = "";

   ItiStrCpy (szQryStr,
              "/* CESUtil calc.c CalcJobItemChg */ "
              "UPDATE JobItem "
              " SET UnitPrice = " 
              " (SELECT SUM (UnitPrice) "
              " FROM JobCostSheet, JobCostBasedEstimate "
              " WHERE JobCostSheet.JobKey= %JobKey "
              " AND JobCostSheet.JobItemKey = %JobItemKey "
              " AND JobCostSheet.JobItemCostBasedEstimateKey = "
              " JobCostBasedEstimate.JobItemCostBasedEstimateKey "
              " AND JobCostBasedEstimate.JobKey = %JobKey "
              " AND JobCostBasedEstimate.JobItemKey = %JobItemKey "
              " AND JobCostBasedEstimate.Active = 1) "
              " WHERE JobKey = %JobKey "
              " AND JobItemKey = %JobItemKey "
              " UPDATE JobItem"
              " SET ExtendedAmount = "
              " (SELECT Quantity * UnitPrice "
              " FROM JobItem "
              " WHERE JobKey= %JobKey "
              " AND JobItemKey = %JobItemKey )"
              " WHERE JobKey = %JobKey "
              " AND JobItemKey = %JobItemKey "
            , sizeof szQryStr);


   uRet = ItiStrReplaceParams (szJobKey,
                        "%JobKey ", sizeof szJobKey, ppszPNames, ppszParams, 100);
   if ((uRet) || (szJobKey == NULL) || (szJobKey[0] == '\0'))
      {
      ErrCalc ("Missing keys in CalcJobItemChg", szJobKey);
      return 1;
      }


   if (!ItiCesQueryJobCBEstActive (hheap, ppszPNames, ppszParams))
      {
      // -- This job item has an active CBE, now test to see if the 
      // -- job item is using cost sheets for the estimation method
      // -- before zeroing the unit price for recalculation.
      uRet = ItiStrReplaceParams (szTmpCJIC, 
                 "/* CESUtil calc.c CalcJobItemChg */ "
                " SELECT ItemEstimationMethodKey"
                " FROM JobItem"
                " WHERE ItemEstimationMethodKey = "
                       "(SELECT ItemEstimationMethodKey"
                       " FROM ItemEstimationMethod"
                       " WHERE EstimationMethodAbbreviation = 'Cst')"  
                 " AND JobKey = %JobKey "
                 " AND JobItemKey = %JobItemKey ",
                 sizeof szTmpCJIC,
                 ppszPNames,
                 ppszParams,
                 100);
      pszTestKeyValue = ItiDbGetRow1Col1 (szTmpCJIC, hheap, 0, 0, 0);
      if (NULL == pszTestKeyValue)
         return 0;

      uRet = ItiStrReplaceParams (szTmpCJIC, 
                 "/* CESUtil calc.c CalcJobItemChg */ "
                 "UPDATE JobItem"
                 " SET UnitPrice = 0,"
                 " ExtendedAmount = 0 "
                 " WHERE JobKey = %JobKey "
                 " AND JobItemKey = %JobItemKey ",
                 sizeof szTmpCJIC,
                 ppszPNames,
                 ppszParams,
                 100);
      }
   else
      {
      uRet = ItiStrReplaceParams
                (szTmpCJIC, szQryStr, sizeof szTmpCJIC, ppszPNames, ppszParams, 100);
      }

   if (uRet)
      {
      ErrCalc ("Not all params available for CalcJobItemChg", szTmpCJIC);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTmpCJIC))
      {
      ErrCalc ("Bad Exec for CalcJobItemChg", szTmpCJIC);
      ItiDbRollbackTransaction (hheap);
      return 13;
      }

   ItiDbUpdateBuffers ("JobItem");
   ItiDbCommitTransaction (hheap);
   return 0;
  } /* End of Function CalcJobItemChg */



int EXPORT CalcJobItemChgJobBrkdwn (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
   USHORT uRet;
//   static char   szTmpCJB [712];
//   static char   szPatCJB [712] = "";

   ItiStrCpy (szPatCJB,
              "/* CESUtil calc.c CalcJobItemChgJobBrkdwn */ "
              "IF NULL != (SELECT SUM(ExtendedAmount) FROM JobItem "
                         " WHERE JobKey = %JobKey "
                         " AND JobBreakdownKey = %JobBreakdownKey ) "
              " UPDATE JobBreakdown"
              " SET DetailedEstimateCost = "
              " (SELECT SUM(ExtendedAmount) FROM JobItem "
                " WHERE JobKey = %JobKey "
                " AND JobBreakdownKey = %JobBreakdownKey )"
              " WHERE JobKey = %JobKey"
              " AND JobBreakdownKey = %JobBreakdownKey "
            " ELSE "
              "UPDATE JobBreakdown"
              " SET DetailedEstimateCost = 0 "
              " WHERE JobKey = %JobKey"
              " AND JobBreakdownKey = %JobBreakdownKey "
            ,sizeof szPatCJB);



   uRet = ItiStrReplaceParams
             (szTmpCJB, szPatCJB, sizeof szTmpCJB, ppszPNames, ppszParams, 100);

   if (uRet)
      {
      ErrCalc ("Not all params available for CalcJobItemChgJobBrkdwn", szTmpCJB);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTmpCJB))
      {
      ErrCalc ("Bad Exec for CalcJobItemChgJobBrkdwn", szTmpCJB);
      ItiDbRollbackTransaction (hheap);
      return 13;
      }

   ItiDbUpdateBuffers ("JobBreakdown");
   ItiDbCommitTransaction (hheap);
   return 0;
  } /* End of Function CalcJobItemChgJobBrkdwn */

           

int EXPORT CalcJobItemChgJob (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
   USHORT uRet;
//   CHAR szJobKey [16] = "";
//   static char szCJICJQry [800] = "";
      

   uRet = ItiStrGet1Param
             (szJobKey, "JobKey", sizeof szJobKey, ppszPNames, ppszParams);
   if (uRet)
      {
      ErrCalc ("Not all params available for CalcJobItemChgJob", " JobKey ");
      return 1;
      }

   ItiStrCpy (szCJICJQry, "/* CESUtil calc.c CalcJobItemChgJob 0 */ "
              "UPDATE Job"
              " SET EstimatedCost ="
              " (SELECT SUM(DetailedEstimateCost) "
                " FROM JobBreakdown WHERE JobKey = ", sizeof szCJICJQry);
   ItiStrCat (szCJICJQry, szJobKey, sizeof szCJICJQry);
   ItiStrCat (szCJICJQry, ") WHERE JobKey = ", sizeof szCJICJQry);
   ItiStrCat (szCJICJQry, szJobKey, sizeof szCJICJQry);

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szCJICJQry))
      {
      ErrCalc ("Bad Exec for CalcJobItemChgJob, query 0", szCJICJQry);
      ItiDbRollbackTransaction (hheap);
      return 13;
      }

   ItiStrCpy (szCJICJQry, " /* CESUtil calc.c CalcJobItemChgJob 1 */ " 
                     "IF (NULL != (SELECT SUM(PreliminaryEstimateCost) "
                                " FROM JobBreakdown"
                                " WHERE JobKey = ", sizeof szCJICJQry);
   ItiStrCat (szCJICJQry, szJobKey, sizeof szCJICJQry);
   ItiStrCat (szCJICJQry, " AND DetailedEstimate = 0 )) "
                         " UPDATE Job "
                         " SET EstimatedCost = EstimatedCost +"
                               " (SELECT SUM(PreliminaryEstimateCost) "
                               " FROM JobBreakdown"
                               " WHERE JobKey = ", sizeof szCJICJQry);
   ItiStrCat (szCJICJQry, szJobKey, sizeof szCJICJQry);
   ItiStrCat (szCJICJQry," AND DetailedEstimate = 0 ) WHERE JobKey = ", sizeof szCJICJQry);
   ItiStrCat (szCJICJQry, szJobKey, sizeof szCJICJQry);


   if (ItiDbExecSQLStatement (hheap, szCJICJQry))
      {
      ErrCalc ("Bad Exec for CalcJobItemChgJob, query 1", szCJICJQry);
      ItiDbRollbackTransaction (hheap);
      return 13;
      }

   ItiStrCpy (szCJICJQry, "/* CESUtil calc.c CalcJobItemChgJob 2 */ "
              " UPDATE Job SET EstimateTotal = "
              " (EstimatedCost * (1.00 + ConstructionEngineeringPct + ContingencyPercent) ) "
              " WHERE JobKey = ", sizeof szCJICJQry);
   ItiStrCat (szCJICJQry, szJobKey, sizeof szCJICJQry);

   if (ItiDbExecSQLStatement (hheap, szCJICJQry))
      {
      ErrCalc ("Bad Exec for CalcJobItemChgJob, query 2", szCJICJQry);
      ItiDbRollbackTransaction (hheap);
      return 13;
      }

   ItiDbUpdateBuffers ("Job");
   ItiDbCommitTransaction (hheap);
   return 0;
  } /* End of Function CalcJobItemChgJob */




/* -- Needs the following keys:
   -- JobKey
   -- JobBreakdownKey
   -- JobItemKey
 */
int EXPORT DoJobItemChangedRecalc (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
   if ((ppszPNames == NULL) || (ppszParams == NULL))
      return 13;

   ItiErrWriteDebugMessage ("Entered DoJobItemChangedRecalc");

   ItiDBDoCalc (CalcJobItemActiveCostSht, ppszPNames, ppszParams, 0);

//   ItiDBDoCalc (CalcJobItemChg, ppszPNames, ppszParams, 0);
   ItiDBDoCalc (CalcJobItemUnitPrice, ppszPNames, ppszParams, 0);
   ItiDBDoCalc (CalcJobItemChgJobBrkdwn, ppszPNames, ppszParams, 0);
   ItiDBDoCalc (CalcJobItemChgJob, ppszPNames, ppszParams, 0);
//	ItiDBDoCalc (CalcJobEstimateTotal, ppszPNames, ppszParams, 0);

   ItiDbUpdateBuffers ("Job");
   ItiDbUpdateBuffers ("JobBreakdown");

   ItiErrWriteDebugMessage ("Exiting DoJobItemChangedRecalc");

   return 0;
   } /* End of Function DoJobItemChangedRecalc */




/* -- Needs the following keys:
   -- JobKey
   -- JobBreakdownKey
 */
int EXPORT DoJAndJBUpdateRecalc (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
   if ((ppszPNames == NULL) || (ppszParams == NULL))
      return 13;

   ItiErrWriteDebugMessage ("Entered DoJAndJBUpdateRecalc");

   ItiDBDoCalc (CalcJobItemChgJobBrkdwn, ppszPNames, ppszParams, 0);
   ItiDBDoCalc (CalcJobItemChgJob, ppszPNames, ppszParams, 0);

   ItiDbUpdateBuffers ("Job");
   ItiDbUpdateBuffers ("JobBreakdown");

   ItiErrWriteDebugMessage ("Exiting DoJAndJBUpdateRecalc");

   return 0;
   } /* End of Function DoJAndJBUpdateRecalc */


// *********************************************************************************


/* Calculates EstimateCost
 *
 * Needs: JobKey
 *
 */

int EXPORT CalcJobEstimateCost (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
   char   szTemp [1024];
   USHORT usRet;

   usRet = ItiStrReplaceParams (szTemp, 
                              " /*  CESUtil calc CalcJobEstimateCost */ "
                              "UPDATE Job "
                              "SET EstimatedCost = " 
                              "((SELECT SUM (Estimate) "
                              "  FROM JobAlternateGroup "
                              "  WHERE JobKey=%JobKey) "
                              "+(SELECT SUM ((1-DetailedEstimate) * "
                                          "  PreliminaryEstimateCost) "
                              "  FROM JobBreakdown "
                              "  WHERE JobKey=%JobKey)) "
                              "WHERE JobKey=%JobKey "
                              "UPDATE Job "
                              "SET EstimateTotal = EstimatedCost + "
                              "EstimatedCost * ConstructionEngineeringPct + " 
                              "EstimatedCost * ContingencyPercent " 
                              "WHERE JobKey=%JobKey ",
                              sizeof szTemp,
                              ppszPNames,
                              ppszParams,
                              100);

   if (usRet)
      {
      ErrCalc ("Not all params available for CalcJobEstimate", szTemp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      {
      ErrCalc ("Bad Exec for CalcJobEstimate", szTemp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   ItiDbUpdateBuffers ("Job");
   return 0;
   }




/*******************************************************************/
/*                    HIGH LEVEL FUNCTIONS                         */
/*    These functions are here for convenience only                */
/*******************************************************************/





void EXPORT DoJobBRecalc (HWND hwnd)                /* *********** */
   {                                                /* *********** */
   PSZ      pszJobKey, pszJobBreakdownKey;      
   PSZ      *ppszPNames, *ppszParams;
   HHEAP    hheap;

   hheap              = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
   pszJobKey          = (PSZ)   QW (hwnd, ITIWND_DATA, 0, 0, cJobKey);
   pszJobBreakdownKey = (PSZ)   QW (hwnd, ITIWND_DATA, 0, 0, cJobBreakdownKey);

   if ((pszJobBreakdownKey == NULL) || (pszJobKey == NULL))
      {
      ItiErrWriteDebugMessage
         ("Failed to get keys in DoJobBRecalc from the hwnd.");
      return;
      }

   ppszPNames = ItiStrMakePPSZ (hheap, 3, "JobBreakdownKey", "JobKey", NULL);
   ppszParams = ItiStrMakePPSZ (hheap, 3, pszJobBreakdownKey, pszJobKey, NULL);

   ItiDBDoCalc (CalcJobItemChgJobBrkdwn, ppszPNames, ppszParams, 0);
   ItiDBDoCalc (CalcJobItemChgJob, ppszPNames, ppszParams, 0);
//   ItiDBDoCalc (CalcDetailedEstimateCost, ppszPNames, ppszParams, 0);


   if (NULL !=  pszJobKey)
      ItiMemFree(hheap, pszJobKey);
   if (NULL !=  pszJobBreakdownKey)
      ItiMemFree(hheap, pszJobBreakdownKey);

   ItiFreeStrArray (hheap, ppszPNames, 3);
   ItiFreeStrArray (hheap, ppszParams, 3);
   }



void EXPORT DoJobRecalc (HWND hwnd)
   {
   PSZ      pszJobKey, pszJobBreakdnKey;
   PSZ      *ppszPNames, *ppszParams;
   HHEAP    hheap;

   hheap            = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);

   pszJobKey        = (PSZ)   QW (hwnd, ITIWND_DATA, 0, 0, cJobKey);
   pszJobBreakdnKey = (PSZ)   QW (hwnd, ITIWND_DATA, 0, 0, cJobBreakdownKey);

   ppszPNames = ItiStrMakePPSZ (hheap, 3, "JobKey",  "JobBreakdownKey", NULL);
   ppszParams = ItiStrMakePPSZ (hheap, 3, pszJobKey, pszJobBreakdnKey,  NULL);

   ItiDBDoCalc (CalcDetailedEstimateCost, ppszPNames, ppszParams, 0);
   ItiDBDoCalc (CalcAlternateGroupEstimate, ppszPNames, ppszParams, 0);
   ItiDBDoCalc (CalcJobEstimateCost, ppszPNames, ppszParams, 0);

   /* -- added 23 march */
   DoJobCostSheetReCalc (pszJobKey);

   if (NULL !=  pszJobKey)
      ItiMemFree (hheap, pszJobKey);
   if (NULL !=  pszJobBreakdnKey)
      ItiMemFree (hheap, pszJobBreakdnKey);

   ItiFreeStrArray (hheap, ppszPNames, 3);
   ItiFreeStrArray (hheap, ppszParams, 3);

   } /* End of Function DoJobRecalc */




void EXPORT DoJobEstimateRecalc (HWND hwnd)
   {
   PSZ      pszJobKey;
   PSZ      *ppszPNames, *ppszParams;
   HHEAP    hheap;

   hheap            = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
   pszJobKey        = (PSZ)   QW (hwnd, ITIWND_DATA, 0, 0, cJobKey);

   ppszPNames = ItiStrMakePPSZ (hheap, 2, "JobKey", NULL);
   ppszParams = ItiStrMakePPSZ (hheap, 2, pszJobKey, NULL);

   ItiDBDoCalc (CalcAlternateGroupEstimate, ppszPNames, ppszParams, 0);

   if (NULL !=  pszJobKey)
      ItiMemFree (hheap, pszJobKey);

   ItiFreeStrArray (hheap, ppszPNames, 2);
   ItiFreeStrArray (hheap, ppszParams, 2);
   }




void EXPORT DoReCalcJobBBreakJobFunds (HWND hwnd)         /* ************   */
                                                          /* write Comments */
   {
   PSZ    pszJobKey, pszJobBreakdownKey, pszProgramKey;
   char   szTemp [1024];
   PSZ    *ppszPNames, *ppszParams;
   HHEAP  hheap;

   hheap            = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
   pszJobKey        = (PSZ)   QW (hwnd, ITIWND_DATA, 0, 0, cJobKey);
   pszJobBreakdownKey = (PSZ) QW (hwnd, ITIWND_DATA, 0, 0, cJobBreakdownKey);

   /* Determines whether the Job is currently in a Program */

   ItiStrCpy (szTemp,"SELECT ProgramKey "
                     "FROM ProgramJob "
                     "WHERE JobKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobKey, sizeof szTemp);

   pszProgramKey = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);
   
   ppszPNames = ItiStrMakePPSZ (hheap, 4, "JobKey",  "JobBreakdownKey",  "ProgramKey", NULL);
   ppszParams = ItiStrMakePPSZ (hheap, 4, pszJobKey, pszJobBreakdownKey, pszProgramKey, NULL);

   ItiDBDoCalc (CalcAllJobBreakFundsForJob, ppszPNames, ppszParams, 0);

   if (NULL !=  pszJobKey)
      ItiMemFree(hheap, pszJobKey);
   if (NULL !=  pszJobBreakdownKey)
      ItiMemFree(hheap, pszJobBreakdownKey);

   ItiFreeStrArray (hheap, ppszPNames, 4);
   ItiFreeStrArray (hheap, ppszParams, 4);
   }





void EXPORT DoReCalcJobBreakJobFunds (HWND hwnd)

   {
   USHORT usRow;
   PSZ    pszJobKey, pszJobBreakdownKey, pszProgramKey;
   char   szTemp [1024];
   PSZ    *ppszPNames, *ppszParams;
   HHEAP  hheap;

   hwnd = QW (hwnd, ITIWND_OWNERWND, 1, 0, 0);
   hheap            = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
   usRow             = (UM)    QW (hwnd, ITIWND_ACTIVE, 0, 0, 0);
   pszJobKey        = (PSZ)   QW (hwnd, ITIWND_DATA, 0, usRow, cJobKey);
   pszJobBreakdownKey = (PSZ) QW (hwnd, ITIWND_DATA, 0, usRow, cJobBreakdownKey);

   /* Determines whether the Job is currently in a Program */

   ItiStrCpy (szTemp,"SELECT ProgramKey "
                     "FROM ProgramJob "
                     "WHERE JobKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobKey, sizeof szTemp);

   pszProgramKey = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);
   
   ppszPNames = ItiStrMakePPSZ (hheap, 4, "JobKey",  "JobBreakdownKey",  "ProgramKey", NULL);
   ppszParams = ItiStrMakePPSZ (hheap, 4, pszJobKey, pszJobBreakdownKey, pszProgramKey, NULL);

   ItiDBDoCalc (CalcAllJobBreakFundsForJob, ppszPNames, ppszParams, 0);

   if (NULL != pszJobKey)
      ItiMemFree(hheap, pszJobKey);
   if (NULL !=  pszJobBreakdownKey)
      ItiMemFree(hheap, pszJobBreakdownKey);
   if (NULL !=  pszProgramKey)
      ItiMemFree(hheap, pszProgramKey);

   ItiFreeStrArray (hheap, ppszPNames, 4);
   ItiFreeStrArray (hheap, ppszParams, 4);
   }



/*******************************************************************/
/*                     LOW LEVEL FUNCTIONS                         */
/*    These functions must all have the same header definition,    */
/*    (I.E. hheap, ppsz, ppsz, and return an USHORT)                  */
/*******************************************************************/






int EXPORT CalcDetailedEstimateCost (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
   char   szTemp [512];
   char   szTest [512];
   char   szResFmt [64];
   USHORT usRet, usLen;
   PSZ    pszRes;
   DOUBLE dRes;

   usRet = ItiStrReplaceParams (szTest,
              " /*  cesutil calc CalcDetailedEstimateCost */ "
              " SELECT SUM(ExtendedAmount)"
              " FROM JobItem JI, JobAlternateGroup JAG"
              " WHERE JI.JobKey= %JobKey"
              " AND JobBreakdownKey= %JobBreakdownKey"
              " AND JI.JobAlternateKey = JAG.ChosenAlternateKey "
              " AND JI.JobKey = JAG.JobKey "
              " AND JI.JobAlternateGroupKey = JAG.JobAlternateGroupKey ",
              sizeof szTest,
              ppszPNames,
              ppszParams,
              100);

// ItiErrWriteDebugMessage (szTest);

   pszRes = ItiDbGetRow1Col1(szTest, hheap, 0, 0, 0);
	if (pszRes != NULL)
      {
      dRes = ItiStrToDouble (pszRes);
      ItiFmtFormatString (pszRes, szResFmt,
                          sizeof szResFmt, "Number,....", &usLen);
      }
   else
      dRes = 0.0;

   if (dRes != 0.0)
      {
         //sprintf (szTest,  
      ItiStrCpy (szTest,
         " /* cesutil calc CalcDetailedEstimateCost */ "
         "UPDATE JobBreakdown"
         " SET DetailedEstimateCost = ", sizeof szTest);

      ItiStrCat (szTest, szResFmt, sizeof szTest);

      ItiStrCat (szTest, " WHERE JobKey = %JobKey"
                 " AND JobBreakdownKey= %JobBreakdownKey",sizeof szTest);

         //  " AND JobBreakdownKey= %%JobBreakdownKey" );

      usRet = ItiStrReplaceParams
                 (szTemp, szTest, sizeof szTemp, ppszPNames, ppszParams, 100);
      }
   else
      usRet = ItiStrReplaceParams (szTemp,  
                 " /* CESUtil calc CalcDetailedEstimateCost */ "
                 "UPDATE JobBreakdown"
                 " SET DetailedEstimateCost = 0.0" 
                 " WHERE JobKey = %JobKey"
                 " AND JobBreakdownKey= %JobBreakdownKey",
                 sizeof szTemp,
                 ppszPNames,
                 ppszParams,
                 100);

   if (pszRes != NULL)
      ItiMemFree (hheap, pszRes);

   if (usRet)
      {
      ErrCalc ("Not all params available for CalcDetailedEstimateCost", szTemp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      {
      ErrCalc ("Bad Exec for CalcDetailedEstimateCost", szTemp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }


   ItiDbCommitTransaction (hheap);

   /*--- do buffer updates after all calcs ---*/
   ItiDbUpdateBuffers ("JobBreakdown");
//   ItiDbUpdateBuffers ("Job");
   return usRet;
   }





/*
 * Calculates chosen alternate totals for all alternate groups
 *
 * Needs: JobKey
 */

int EXPORT CalcAlternateGroupEstimate (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
   char   szTemp [1120];
   USHORT usRet;

      usRet = ItiStrReplaceParams (szTemp, 
                           " /*  CESUtil calc CalcAlternateGroupEstimate */ "
                           "UPDATE JobAlternate"
                           " SET Estimate =" 
                           " (SELECT SUM (JI.ExtendedAmount)"
                           "  FROM JobItem JI, JobBreakdown JB "
                           "  WHERE JI.JobKey= %JobKey "
                           "  AND JB.JobKey= %JobKey "
                           "  AND JB.DetailedEstimate = 1"
                           "  AND JB.JobBreakdownKey = JI.JobBreakdownKey"
                           "  AND JobAlternate.JobAlternateKey=JI.JobAlternateKey"
                           "  AND JobAlternate.JobAlternateGroupKey=JI.JobAlternateGroupKey)"
                           " WHERE JobKey= %JobKey ",
                           sizeof szTemp,
                           ppszPNames,
                           ppszParams,
                           100);

   if (usRet)
      {
      ErrCalc ("Not all params available for CalcAlternateGroups", szTemp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      {
      ErrCalc ("Bad Exec for CalcAlternateGroups", szTemp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   usRet = ItiStrReplaceParams (szTemp, 
                           " /*  CESUtil calc CalcAlternateGroupEstimate */ "
                           "UPDATE JobAlternateGroup"
                           " SET Estimate =" 
                           " (SELECT SUM(JA.Estimate)"
                           "  FROM JobAlternate JA"
                           "  WHERE JA.JobKey=%JobKey "
                           "  AND JobAlternateGroup.ChosenAlternateKey=JA.JobAlternateKey"
                           "  AND JobAlternateGroup.JobAlternateGroupKey=JA.JobAlternateGroupKey)"
                           " WHERE JobKey=%JobKey",
                           sizeof szTemp,
                           ppszPNames,
                           ppszParams,
                           100);

   if (usRet)
      {
      ErrCalc ("Not all params available for CalcAlternateGroups", szTemp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      {
      ErrCalc ("Bad Exec for CalcAlternateGroups", szTemp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

//   ItiDBDoCalc (CalcJobEstimateCost, ppszPNames, ppszParams, 0);

   /*--- do buffer updates after all calcs ---*/
   ItiDbUpdateBuffers ("JobAlternateGroup");
   ItiDbUpdateBuffers ("JobAlternate");
   return usRet;
   }











/* Calculates the AmountFunded for a particular JobBreakdown
 *
 * Needs: JobKey, JobBreakdownKey
 *
 */

int EXPORT CalcJobBreakFundForJob(HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)

   {
   char      szTemp[1024];
   USHORT    usRet;

   usRet = ItiStrReplaceParams(szTemp,
                              " /* CESUtil calc CalcJobBreakFundForJob */ "
                              "UPDATE JobBreakdownFund "
                              "SET AmountFunded = PercentFunded * "
                                  "(SELECT (((1-DetailedEstimate) * "
                                  "PreliminaryEstimateCost) "
                                  "+ (convert(int, DetailedEstimate) * "
                                  "DetailedEstimateCost)) "
                                  "FROM JobBreakdown JB "
                                  "WHERE JB.JobKey = JobBreakdownFund.JobKey "
                                  "AND JB.JobBreakdownKey = JobBreakdownFund.JobBreakdownKey) "
                              "WHERE JobBreakdownFund.JobKey =  %JobKey "
                              "AND JobBreakdownFund.JobBreakdownKey = %JobBreakdownKey ",
                               sizeof szTemp,
                               ppszPNames,
                               ppszParams,
                               100);
   if (usRet)
      {
      ErrCalc ("Not all params available for CalcJobBreakFundForJob", szTemp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      {
      ErrCalc ("Bad Exec for CalcJobBreakFundForJob", szTemp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   /*--- do buffer updates after all calcs ---*/
   ItiDbUpdateBuffers ("JobBreakdownFund");
   ItiDbUpdateBuffers ("Job");
   return 0;
   }




int EXPORT CalcAmtFundedJobFunds(HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
   char   szTemp[1024], szTemp2[1024], szTemp3[1024], szAmtSumFmt[32];
   USHORT usRet, usErr, usLen, usCols [] = {1,2,0};
   PSZ    pszJobKey, pszConstrEngrPct, pszContingPct, pszAmtSum, *ppszPct;
      


   /* Get ContingencyPercent and ConstructionEngineeringPct from Job */

   pszJobKey = ItiDbGetZStr(ppszParams[1],0); 


   ItiStrCpy (szTemp2, " SELECT ContingencyPercent, ConstructionEngineeringPct "
                    " FROM Job "
                    " WHERE JobKey = ", sizeof szTemp2);
   ItiStrCat (szTemp2, pszJobKey, sizeof szTemp2);

   usErr = ItiDbBuildSelectedKeyArray2(hheap, 0, szTemp2,
                                       &ppszPct, usCols);

   if (usErr != 0)
      return FALSE;

   pszContingPct = ItiDbGetZStr (ppszPct [0], 0);
   pszConstrEngrPct = ItiDbGetZStr (ppszPct [0], 1);



   usRet = ItiStrReplaceParams(szTemp,
                              " /* CESUtil calc CalcAmtFundedJobFunds */ "
                                 " SELECT SUM(JBF.AmountFunded) "
                                 " FROM JobBreakdownFund JBF, JobFund JF "
                                 " WHERE JBF.JobKey = JF.JobKey "
                                 " AND JBF.FundKey = JF.FundKey "
                                 " AND JF.JobKey = %JobKey ",
                               sizeof szTemp,
                               ppszPNames,
                               ppszParams,
                               100);
   pszAmtSum = ItiDbGetRow1Col1(szTemp, hheap, 0,0,0);
   if (NULL != pszAmtSum)
      {
      ItiFmtFormatString (pszAmtSum, szAmtSumFmt,
                          sizeof szAmtSumFmt, "Number,....", &usLen);
      ItiMemFree(hheap, pszAmtSum);
      }
   else
      {
      szAmtSumFmt[0] = '0';
      szAmtSumFmt[1] = '\0';
      }

   sprintf(szTemp2," /* CESUtil calc CalcAmtFundedJobFunds */ "
                   "UPDATE JobFund "
                   "SET AmountFunded = (%s * (1 + %s + %s))"
                   "WHERE JobFund.JobKey = %%JobKey ",
                    szAmtSumFmt, pszContingPct, pszConstrEngrPct);

   usRet = ItiStrReplaceParams(szTemp, szTemp2, sizeof szTemp,
                               ppszPNames, ppszParams, 100);

   sprintf(szTemp3, szTemp, pszContingPct, pszConstrEngrPct);

   ItiFreeStrArray(hheap, ppszPct, 2);

   ItiDbBeginTransaction (hheap);

   if (ItiDbExecSQLStatement (hheap, szTemp3))
      {
      ErrCalc ("Bad Exec for CalcAmtFundedJobFunds", szTemp3);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   /*--- do buffer updates after all calcs ---*/
   ItiDbUpdateBuffers ("JobFund");
   ItiDbUpdateBuffers ("Job");

   return 0;
   }






BOOL EXPORT GetJobFundsForJob(HHEAP hheap, PSZ pszJobKey, PSZ **pppszJFKeys)
    {
    char   szTemp [1024];
    USHORT usErr, usCols[] = {1,0};

   /* This query obtains all of the FundKeys associated with a Job */
   /* and stores the keys in an array                              */

   sprintf (szTemp, "SELECT FundKey " 
                    "FROM JobFund "
                    "WHERE JobKey = %s", pszJobKey);

   usErr = ItiDbBuildSelectedKeyArray2(hheap, 0, szTemp,
                                       pppszJFKeys, usCols);
   if (usErr != 0)
      return FALSE;

   return TRUE;
   } /* END of Function GetJobFundsForJob */


int EXPORT CalcJFPctFunded(HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
   char   szTemp [1024] = "";
   char   szTemp2 [512] = "";
   PSZ    pszTtl = NULL;
   PSZ    pszAmt = NULL;
   USHORT usRet;

   usRet = ItiStrReplaceParams (szTemp, 
                           " /*  CESUtil calc.c CalcJFPctFunded */ "
                           " SELECT EstimateTotal "
                           " FROM Job "
                           " WHERE JobKey = %JobKey",
                           sizeof szTemp,
                           ppszPNames,
                           ppszParams,
                           100);
   pszTtl = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);

   usRet = ItiStrReplaceParams (szTemp2, 
                           " /*  CESUtil calc.c CalcJFPctFunded */ "
                           " SELECT SUM(AmountFunded) "
                           " FROM JobFund "
                           " WHERE JobKey = %JobKey "
                           " AND FundKey = %FundKey ",
                           sizeof szTemp2,
                           ppszPNames,
                           ppszParams,
                           100);
   pszAmt = ItiDbGetRow1Col1(szTemp2, hheap, 0, 0, 0);

   /* -- If either the Estimate or AmountFunded is null or zero,
      -- then the PercentFunded will be set to zero.
    */
   if ( ((pszTtl == NULL) || ((*pszTtl) == '0'))
      ||((pszAmt == NULL) || ((*pszAmt) == '0')) )
      {
      usRet = ItiStrReplaceParams (szTemp, 
                           " /*  CESUtil calc.c CalcJFPctFunded */ "
                           "UPDATE JobFund "
                           " SET PercentFunded = 0.00 "
                           " WHERE JobKey = %JobKey "
                           " AND FundKey = %FundKey",
                           sizeof szTemp,
                           ppszPNames,
                           ppszParams,
                           100);
      }
   else
      {
      usRet = ItiStrReplaceParams (szTemp, 
                           " /*  CESUtil calc.c CalcJFPctFunded */ "
                           "UPDATE JobFund "
                           " SET PercentFunded = " 
                             " (SELECT SUM(AmountFunded) "
                             " FROM JobFund "
                             " WHERE JobKey = %JobKey "
                             " AND FundKey = %FundKey)/ "
                               " (SELECT EstimateTotal "
                               " FROM Job "
                               " WHERE JobKey = %JobKey)"
                           " WHERE JobKey = %JobKey "
                           " AND FundKey = %FundKey",
                           sizeof szTemp,
                           ppszPNames,
                           ppszParams,
                           100);
      }

   if (pszAmt != NULL)
      ItiMemFree (hheap, pszAmt);
   if (pszTtl != NULL)
      ItiMemFree (hheap, pszTtl);

   if (usRet)
      {
      ErrCalc ("Not all params available for CalcJFPctFunded", szTemp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      {
      ErrCalc ("Bad Exec for CalcJFPctFunded", szTemp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   ItiDbUpdateBuffers ("JobFund");

   return 0;
   }






/* This function calls several functions to:
 *  (1) calculate the AmountFunded for all JobFunds
 *  (2) Get all of the funds associated with a job
 *  (3) calculate the JobFund PercentFunded
 *  (4) calculate the EstimateTotal for ProgramJob, if this job
 *      is in a program.
 *
 * Needs: hheap, JobKey
 *
 */

int EXPORT RecalcJFForJob(HHEAP hheap, PSZ pszJobKey)
   {
   char szTemp [1024];
   PSZ  *ppszPNames, *ppszParams, pszProgramKey;
   PSZ  *ppszJFPNames, *ppszJFParams, *ppszJFKeys;
   USHORT i, usRet;

   /* Determines whether Job is currently in Program */

   sprintf(szTemp, "SELECT ProgramKey "
                  "FROM ProgramJob "
                  "WHERE JobKey = %s", pszJobKey);

   pszProgramKey = ItiDbGetRow1Col1(szTemp, hheap, 0,0,0);


   ppszPNames = ItiStrMakePPSZ(hheap, 4, "JobKey", "JobKey", "ProgramKey", NULL);
   ppszParams = ItiStrMakePPSZ(hheap, 4, pszJobKey, pszJobKey, pszProgramKey, NULL);

   ItiDBDoCalc(CalcAmtFundedJobFunds, ppszPNames, ppszParams,0);


   GetJobFundsForJob (hheap, pszJobKey, &ppszJFKeys);

   if (ppszJFKeys != NULL)
      {
      /* for each JobFund for this Job, recalc the PercentFunded */

      ppszJFPNames = ItiStrMakePPSZ (hheap, 3, "JobKey", "FundKey", NULL);

      for (i = 0; ppszJFKeys [i] != NULL; i++)

        {
        ppszJFParams = ItiStrMakePPSZ (hheap, 3, pszJobKey, ItiDbGetZStr(ppszJFKeys[i], 0), NULL);
        ItiDBDoCalc(CalcJFPctFunded, ppszJFPNames, ppszJFParams, 0);
        ItiFreeStrArray (hheap, ppszJFParams, 3);
        }
      ItiFreeStrArray (hheap, ppszJFPNames, 3);

      }

   if (ppszParams[2] != NULL)
      {
      usRet = CalcEstTotlProgJob (hheap, ppszPNames, ppszParams);
      }

   ItiFreeStrArray (hheap, ppszPNames, 4);
   ItiFreeStrArray (hheap, ppszParams, 4);

   return 0;
   }








int EXPORT CalcAllJobBreakFundsForJob(HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
   char      szTemp[1024];
   USHORT    usRet;

   usRet = ItiStrReplaceParams(szTemp,
                              " /* CESUtil calc CalcAllJobBreakFundsForJob */ "
                              "UPDATE JobBreakdownFund "
                              "SET AmountFunded = PercentFunded * "
                                  "(SELECT (((1-DetailedEstimate) * "
                                  "PreliminaryEstimateCost) "
                                  "+ (convert(int, DetailedEstimate) * "
                                  "DetailedEstimateCost)) "
                                  "FROM JobBreakdown JB "
                                  "WHERE JB.JobKey = JobBreakdownFund.JobKey "
                                  "AND JB.JobBreakdownKey = JobBreakdownFund.JobBreakdownKey) "
                              "WHERE JobBreakdownFund.JobKey =  %JobKey "
                              "AND JobBreakdownFund.JobBreakdownKey = %JobBreakdownKey ",
                               sizeof szTemp,
                               ppszPNames,
                               ppszParams,
                               100);
   if (usRet)
      {
      ErrCalc ("Not all params available for CalcAllJobBreakFundsForJob", szTemp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      {
      ErrCalc ("Bad Exec for CalcAllJobBreakFundsForJob", szTemp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   /*--- do buffer updates after all calcs ---*/
   usRet = CalcAllJobFundsForJob (hheap, ppszPNames, ppszParams);
   ItiDbUpdateBuffers ("JobBreakdownFund");
   ItiDbUpdateBuffers ("JobBreakdown");
   ItiDbUpdateBuffers ("Job");
   return usRet;
   }






int EXPORT CalcEstTotlProgJob(HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)

   {
   char szTemp [1024];
   USHORT usRet;


   usRet = ItiStrReplaceParams(szTemp,
                              " /* CESUtil calc CalcEstTotlProgJob */ "
                              "UPDATE ProgramJob "
                              "SET EstimateTotalAdjForInflation = "
                                 "((SELECT EstimateTotal "
                                 "FROM Job "
                                 "WHERE JobKey = %JobKey) * "
                                    "(SELECT InitialPrediction " 
                                    "FROM InflationPrediction, ProgramJob "
                                    "WHERE ProgramJob.InflationPredictionKey = InflationPrediction.InflationPredictionKey "
                                    "AND ProgramJob.JobKey = %JobKey) + (SELECT EstimateTotal FROM Job "
                              "WHERE JobKey = %JobKey)) "
                              "WHERE JobKey = %JobKey ",
                              sizeof szTemp,
                              ppszPNames,
                              ppszParams,
                              100);

   if (usRet)
      {
      ErrCalc ("Not all params available for CalcEstTotlProgJob", szTemp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      {
      ErrCalc ("Bad Exec for CalcEstTotlProgJob", szTemp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   /*--- do buffer updates after all calcs ---*/
   usRet =  ReCalcProgram(hheap, ppszPNames, ppszParams);
   ItiDbUpdateBuffers ("ProgramJob");
   ItiDbUpdateBuffers ("Program");
   return usRet;
   }




int EXPORT ReCalcProgram(HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)

   {
   char      szTemp[1024];
   USHORT    usRet;


   usRet = ItiStrReplaceParams(szTemp,
                             " /* CESUtil calc ReCalcProgram */ "
                             "UPDATE Program "
                             "SET TotalCost = "
                                "(SELECT SUM(EstimateTotal) "
                                "FROM Job "
                                "WHERE JobKey IN "
                                   "(SELECT JobKey FROM ProgramJob "
                                   "WHERE ProgramKey = %ProgramKey)) "
                             "WHERE ProgramKey = %ProgramKey "
                             "UPDATE Program "
                             "SET TotalAdjustedForInflation = "
                                "(SELECT SUM(EstimateTotalAdjForInflation) "
                                "FROM ProgramJob "
                                "WHERE ProgramKey = %ProgramKey) "
                             "WHERE ProgramKey = %ProgramKey ",
                              sizeof szTemp,
                              ppszPNames,
                              ppszParams,
                              100);


   if (usRet)
      {
      ErrCalc ("Not all params available for ReCalcProgram", szTemp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      {
      ErrCalc ("Bad Exec for ReCalcProgram", szTemp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   /*--- do buffer updates after all calcs ---*/
   usRet = CalcAllProgramFunds(hheap, ppszPNames, ppszParams);
   ItiDbUpdateBuffers ("Program");
   return usRet;
   }




int EXPORT CalcAllProgramFunds(HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)

   {
   char   szFundKeys[256];
   char   szQryAmt[1024];
   char   szQryPct[512];
   INT    i, iCnt, iFx, iInx = -1;
   LONG   lFundKey = 0L;
   PSZ   *ppsz = NULL;
   PSZ   *ppszPFKeys = NULL;
   PSZ   *ppszFundKeys = NULL;
   PSZ   *ppszJobKeys = NULL;
   PSZ    pszAmt = NULL;
   HQRY   hqry = NULL;


   /* -- Find the index value of the program key in the parameter list. */
   i = 0;
   while ((ppszPNames[i] != NULL) && (ppszPNames[i][0] != '\0'))
      {
      if (0 == ItiStrCmp(ppszPNames[i], "ProgramKey"))
         {
         iInx = i;
         break;
         }
      else
         i++;
      }

//  930108-4101 1/15/93 mdh: added check for a value of -1 for iInx.
   if (iInx < 0)
      return iInx;

   /* -- Build arrays of Program Job & Fund Keys. */
   GetJobKeysForProgram (hheap, ppszPNames[iInx], &ppszJobKeys);
   GetProgramFundKeys (hheap, ppszParams[iInx], &ppszPFKeys);


   ItiDbBeginTransaction (hheap);
   /* ----------------------------- */

   /* -- Delete (zero out) from ProgramFund
    * -- all of the program's fund amounts;
    * -- they are all going to be recalculated.
    */
   iCnt = 0;
   while (ppszPFKeys[iCnt] != NULL)
      {
      sprintf (szQryAmt,
         "DELETE FROM ProgramFund "
         "WHERE ProgramKey = %s "
         "AND ProgramFund.FundKey = %s" ,
         ppszParams[iInx],
         ppszPFKeys[iCnt] );

      if (ItiDbExecSQLStatement (hheap, szQryAmt))
         {                          
         ErrCalc("Bad Exec in cesutil calc function CalcAllProgramFunds ",szQryAmt);
         ItiDbRollbackTransaction (hheap);
         return 1;
         }
      iCnt++;
      }



   /* -- Now initialize ProgramFund table for updates.
    * -- We zero out all of the amounts & percentages
    * -- because the will recalced.
    */
   sprintf (szQryAmt,
      " /* CESUtil calc CalcAllProgramFunds */ "
      "INSERT INTO ProgramFund (ProgramKey, FundKey, AmountFunded, PercentFunded) "
      "select distinct ProgramKey, JF.FundKey , 0.0, 0.0 "
      "from JobFund JF, ProgramJob PJ "
      "where JF.JobKey = PJ.JobKey "
      "and PJ.ProgramKey = %s ",
      ppszParams[iInx] );

   if (ItiDbExecSQLStatement (hheap, szQryAmt))
      {
      ErrCalc("Bad Exec in cesutil calc.c function CalcAllProgramFunds ",szQryAmt);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }




   i = 0;
   
   while ( (iInx != -1) && (ppszJobKeys[i] != '\0') )
       {
       /* -- Get all of the fund keys for the
          -- given program's current job key. */
       sprintf (szFundKeys,
                           "select FundKey from JobFund "
                           "where FundKey > 0 "
                           "and JobKey = %s ",
                           ppszJobKeys[i]);

       GetJobFundsForJob (hheap, ppszJobKeys[i], &ppszFundKeys);


       /* -- For each Job's FundKey... */
       for (iFx = 0; (iInx != -1) && (ppszFundKeys[iFx] != NULL); iFx++)
             {
             ItiStrToLONG (ppszFundKeys[iFx], &lFundKey);
             if (lFundKey != UNFUNDEDKEY)
                { /* Fund key is NOT unfunded, so ... */
                  /* ...test the subquery.            */
                sprintf (szQryAmt,
                         " /* CESUtil calc CalcAllProgramFunds */ "
                         "select JF.AmountFunded "
                         "from ProgramFund PF, JobFund JF, Job J, ProgramJob PJ "
                         "where PF.FundKey = JF.FundKey "
                           "and JF.JobKey = J.JobKey "
                           "and PJ.JobKey = JF.JobKey "
                           "and PJ.ProgramKey = PF.ProgramKey "
                           "and PF.ProgramKey = %s "
                           "and JF.FundKey = %s "
                           "and PF.FundKey = %s " 
                           "and JF.JobKey = %s ",
                         ppszParams[iInx],
                         ppszFundKeys[iFx],
                         ppszFundKeys[iFx],
                         ppszJobKeys[i] );

                pszAmt = ItiDbGetRow1Col1(szQryAmt, hheap, 0, 0, 0);
                
                if ((pszAmt == NULL) || ((*pszAmt) == '0'))
                   {
                   /* Test subquery was empty, so set Amount to zero. */
                   sprintf (szQryAmt,
                      " /* CESUtil calc CalcAllProgramFunds */ "
                      "UPDATE ProgramFund "
                      "SET AmountFunded = 0.00 "
                      "WHERE ProgramKey = %s "
                      "AND ProgramFund.FundKey = %s" ,
                      ppszParams[iInx],
                      ppszFundKeys[iFx] );
                   }
                else
                   {
                   sprintf (szQryAmt,
                        " /* CESUtil calc CalcAllProgramFunds */ "
                        "UPDATE ProgramFund "
                        "SET AmountFunded =  AmountFunded + "
                        "(select JF.AmountFunded "
                         "from ProgramFund PF, JobFund JF, Job J, ProgramJob PJ "
                         "where PF.FundKey = JF.FundKey "
                           "and JF.JobKey = J.JobKey "
                           "and PJ.JobKey = JF.JobKey "
                           "and PJ.ProgramKey = PF.ProgramKey "
                           "and PF.ProgramKey = %s "
                           "and JF.FundKey = %s "
                           "and PF.FundKey = %s " 
                           "and JF.JobKey = %s) "
                         " WHERE ProgramKey = %s "
                         "and ProgramFund.FundKey = %s" ,
                         ppszParams[iInx],
                         ppszFundKeys[iFx],
                         ppszFundKeys[iFx],
                         ppszJobKeys[i],
                         ppszParams[iInx],
                         ppszFundKeys[iFx] );
                   }
                }
             else
  { /* the fund key is the unfunded account (key = 1). */
                sprintf (szQryAmt,
                     " /* CESUtil calc CalcAllProgramFunds */ "
                     "UPDATE ProgramFund "
                     "SET AmountFunded = "
                        "(select SUM(AmountFunded) "
                         "from JobFund "
                         "where FundKey = 1 "
                           "and JobKey IN "
             "(SELECT JobKey FROM ProgramJob PJ "
              "WHERE PJ.ProgramKey = %s ) "
                        ") WHERE ProgramKey = %s "
                        "and ProgramFund.FundKey = 1 ",
                      ppszParams[iInx],
                      ppszParams[iInx] );
                }/* End of else, if (lFundKey !=... */


             sprintf (szQryPct,
                        " /* CESUtil calc CalcAllProgramFunds */ "
                        "UPDATE ProgramFund "
                        "SET PercentFunded =  AmountFunded/ "
                        "(SELECT TotalCost "
                        "FROM Program "
                        "WHERE ProgramKey = %s) "
                        "WHERE ProgramFund.ProgramKey = %s "
                        "and ProgramFund.FundKey = %s" ,

                         ppszParams[iInx],
                         ppszParams[iInx],
                         ppszFundKeys[iFx]);

//   ItiErrWriteDebugMessage ("Calc PF Qry used: ");
//   ItiErrWriteDebugMessage (szQryAmt);

             /* -- Now do the Amount update. */
             if (ItiDbExecSQLStatement (hheap, szQryAmt))
                {
                ErrCalc ("Bad Exec for CalcAllProgramFunds Amounts", szQryAmt);
                ItiDbRollbackTransaction (hheap);
                return 1;
                }

             /* -- Now do the update the percentages. */
             if (ItiDbExecSQLStatement (hheap, szQryPct))
                {
                ErrCalc ("Bad Exec for CalcAllProgramFunds Percentages", szQryPct);
                ItiDbRollbackTransaction (hheap);
                return 1;
                }

             } /* end of for */

       i++; /* Increment the index for the ppszJobKeys array. */
       }

   /* ----------------------------- */
   ItiDbCommitTransaction (hheap);



   /*--- do buffer updates after all calcs ---*/
   ItiDbUpdateBuffers ("ProgramFund");
   return 0;
   } /* End of Function CalcAllProgramFunds */



int EXPORT CalcJobCostSheets (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
   USHORT auKeys[] = {1, 0};
   CHAR   szJobKey[32];
//   PSZ    *ppszJobCostSheetKey;
//   PSZ    pszJobKey;
//   CHAR  szTemp[255];


   ItiStrReplaceParams(szJobKey, "%JobKey ",sizeof szJobKey, ppszPNames, ppszParams,100);
//   pszJobKey = szJobKey;


   /* -- Since the cost sheet data has changed...  */
   /* -- Now recalc all of the pricing information */
   /* -- for the job and its various breakdowns.   */
   NewRecalcJob (&szJobKey[0]);


   /*--- do buffer updates after all calcs ---*/
//   ItiDbUpdateBuffers ("JobMaterialUsed");
//   ItiDbUpdateBuffers ("JobCrewLaborUsed");
//   ItiDbUpdateBuffers ("JobCrewEquipmentUsed");
   ItiDbUpdateBuffers ("JobCostSheet");
   ItiDbUpdateBuffers ("JobBreakdown");
   ItiDbUpdateBuffers ("Job");

   return 0;
   }/* End of Function CalcJobCostSheets */





void EXPORT DoJobCostSheetReCalc (PSZ pszJobKey)
   {
   // HHEAP    hhpLocDJCSR;
   // PSZ      *ppszPNames, *ppszParams;

/////////////////////////////////
   /* -- Since the cost sheet data has changed...  */
   /* -- Now recalc all of the pricing information */
   /* -- for the job and its various breakdowns.   */
   if ((NULL != pszJobKey) && (*pszJobKey != '\0'))
      {
      NewRecalcJob (pszJobKey);
      ItiDbUpdateBuffers ("JobCostSheet");
      ItiDbUpdateBuffers ("JobBreakdown");
      ItiDbUpdateBuffers ("Job");
      }

   return;

///////////////////////////////// 96 MAR

   // hhpLocDJCSR = ItiMemCreateHeap (4096);
   // if (hhpLocDJCSR == NULL)
   //    {
   //    ItiErrWriteDebugMessage
   //       ("CESUtil calc DoJobCostSheetReCalc failed to create hhpLocDJCSR.");
   //    return;
   //    }
   // 
   // ppszPNames = ItiStrMakePPSZ (hhpLocDJCSR, 2, "JobKey", NULL);
   // ppszParams = ItiStrMakePPSZ (hhpLocDJCSR, 2, pszJobKey, NULL);
   // 
   // /* -- Recalculate all of the job's cost sheets. */
   // //ItiDBDoCalc (CalcJobCostSheets, ppszPNames, ppszParams, 0);
// 96 MAR replacement code inserted above /// CalcJobCostSheets (hhpLocDJCSR, ppszPNames, ppszParams);

   // ItiFreeStrArray (hhpLocDJCSR, ppszPNames, 2);
   // ItiFreeStrArray (hhpLocDJCSR, ppszParams, 2);

   } /* End of Function DoJobCostSheetReCalc */






BOOL EXPORT GetJobBreakdownsForJob(HHEAP hheap, PSZ pszJobKey, PSZ **pppszJBKeys)
   {
   char    szTemp [1024];
   USHORT  usErr;
   USHORT  usCols [] = {1,0};

   /* This query obtains all of the JobBreakdowns associated with a Job */
   /* and stores the keys in an array                                   */

   sprintf (szTemp, "SELECT JobBreakdownKey " 
                    "FROM JobBreakdown "
                    "WHERE JobKey = %s", pszJobKey);

   usErr = ItiDbBuildSelectedKeyArray2(hheap, 0, szTemp,
                                       pppszJBKeys, usCols);
   if (usErr != 0)
      return FALSE;

   return TRUE;
   }


int EXPORT RecalcJobBreakdown(HHEAP hheap, PSZ pszJobKey, PSZ pszJobBreakdownKey)
   {
   char   szTemp [125] = "";
   PSZ    pszEstMethod;
   USHORT usRet;


   /* Determines the estimation method used for this JobBreakdown */
   sprintf(szTemp, "SELECT DetailedEstimate "
                   "FROM JobBreakdown "
                   "WHERE JobKey = %s "
                   "AND JobBreakdownKey = %s", pszJobKey, pszJobBreakdownKey);

   pszEstMethod = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);

   if (pszEstMethod[0]  == '0')
     {
//     usRet = RecalcParametric(hheap, pszJobKey, pszJobBreakdownKey);
     }
   else 
     {
     usRet = RecalcAllJobBrkdwnItems(hheap, pszJobKey, pszJobBreakdownKey);
     }

	if (pszEstMethod != NULL)
      ItiMemFree (hheap, pszEstMethod);

   return 0;
   }



/* This function creates ppszParams and ppszPNames
 * and calls a function to recalculate a JobBreakdownFund
 * for a particular JobBreakdown 
 *
 * Needs: JobKey, JobBreakdownKey
 *
 */

int EXPORT RecalcJBFunds(HHEAP hheap, PSZ pszJobKey, PSZ pszJobBreakdownKey)

   {
   PSZ  *ppszPNames, *ppszParams;


   ppszPNames = ItiStrMakePPSZ(hheap, 3, "JobBreakdownKey", "JobKey", NULL);
   ppszParams = ItiStrMakePPSZ(hheap, 3, pszJobBreakdownKey, pszJobKey, NULL);

   ItiDBDoCalc(CalcJobBreakFundForJob, ppszPNames, ppszParams, 0);

   ItiFreeStrArray (hheap, ppszPNames, 3);
   ItiFreeStrArray (hheap, ppszParams, 3);
   return 0;

   }





BOOL EXPORT GetJobAlternatesForJob(HHEAP hheap, PSZ pszJobKey, PSZ **pppszJAltKeys)
   {
   char    szTempGJA [200] =
                    "/* CESUtil calc.c GetJobAlternatesForJob */ "
                    "SELECT JobAlternateGroupKey, JobAlternateKey " 
                    "FROM JobAlternate "
                    "WHERE JobKey = ";

   USHORT  usErr;
   USHORT  usCols [] = {1,2,0};

   /* -- This query obtains all of the JobAlternateKeys     */
   /* -- associated with a Job and stores them in an array. */
   // sprintf (szTempGJA,
   //                  "/* CESUtil calc.c GetJobAlternatesForJob */ "
   //                  "SELECT JobAlternateGroupKey, JobAlternateKey " 
   //                  "FROM JobAlternate "
   //                  "WHERE JobKey = %s", pszJobKey);

   ItiStrCat (szTempGJA, pszJobKey, sizeof szTempGJA);

   usErr = ItiDbBuildSelectedKeyArray2(hheap, 0, szTempGJA,
                                       pppszJAltKeys, usCols);
   if (usErr != 0)
      return FALSE;

   return TRUE;
   }/* End of Function GetJobAlternatesForJob */





int EXPORT ChangeJobAltEstToZero (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
   char   szTemp [250] = "";
   USHORT usRet;

   usRet = ItiStrReplaceParams (szTemp,
               " /*  CESUtil calc ChangeJobAltEstToZero */ "
               "UPDATE JobAlternate "
               " SET Estimate = .00 " 
               " WHERE JobKey = %JobKey "
               " AND JobAlternateGroupKey = %JobAlternateGroupKey "
               " AND JobAlternateKey = %JobAlternateKey",
               sizeof szTemp,
               ppszPNames,
               ppszParams,
               100);

   if (usRet)
      {
      ErrCalc ("Not all params available for ChangeJobAltEstToZero", szTemp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      {
      ErrCalc ("Bad Exec for ChangeJobAltEstToZero", szTemp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   /*--- do buffer updates after all calcs ---*/

   ItiDbUpdateBuffers ("JobAlternate");

   return 0;
   }/* End of Function ChangeJobAltEstToZero */



/*
 * Calculates chosen alternate totals for all alternate groups
 *
 * Needs: JobKey
 */

int EXPORT CalcAlternateEstimate (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
   char   szTemp  [540] = "";
   char   szTemp2 [540] = "";
   PSZ    pszSumAmt = NULL;
   USHORT usRet;

   usRet = ItiStrReplaceParams (szTemp2, 
                           " /*  CESUtil calc.c CalcAlternateEstimate */ "
                           " SELECT SUM (JI.ExtendedAmount)"
                           " FROM JobItem JI, JobBreakdown JB "
                           " WHERE JI.JobKey = %JobKey "
                           " AND JB.JobKey = %JobKey "
                           " AND JB.DetailedEstimate = 1"
                           " AND JB.JobBreakdownKey = JI.JobBreakdownKey"
                           " AND JI.JobAlternateKey = %JobAlternateKey"
                           " AND JI.JobAlternateGroupKey = %JobAlternateGroupKey",
                           sizeof szTemp2,
                           ppszPNames,
                           ppszParams,
                           100);
   pszSumAmt = ItiDbGetRow1Col1(szTemp2, hheap, 0, 0, 0);

   if ((pszSumAmt == NULL) || ((*pszSumAmt) == '0') )
      {
      usRet = ItiStrReplaceParams (szTemp, 
                           " /*  CESUtil calc.c CalcAlternateEstimate */ "
                           "UPDATE JobAlternate"
                           " SET Estimate = 0.00" 
                           " WHERE JobKey = %JobKey"
                           " AND JobAlternateGroupKey = %JobAlternateGroupKey"
                           " AND JobAlternateKey = %JobAlternateKey",
                           sizeof szTemp,
                           ppszPNames,
                           ppszParams,
                           100);
      }
   else
      {
      usRet = ItiStrReplaceParams (szTemp, 
                  "/* calc.c CalcAlternateEstimate */ "
                  "UPDATE JobAlternate"
                  " SET Estimate =" 
                  " (SELECT SUM (JI.ExtendedAmount)"
                  " FROM JobItem JI, JobBreakdown JB "
                  " WHERE JI.JobKey = %JobKey "
                  " AND JB.JobKey = %JobKey "
                  " AND JB.DetailedEstimate = 1"
                  " AND JB.JobBreakdownKey = JI.JobBreakdownKey"
                  " AND JI.JobAlternateKey = %JobAlternateKey"
                  " AND JI.JobAlternateGroupKey = %JobAlternateGroupKey)"
                  " WHERE JobKey = %JobKey"
                  " AND JobAlternateGroupKey = %JobAlternateGroupKey"
                  " AND JobAlternateKey = %JobAlternateKey",
                  sizeof szTemp,
                  ppszPNames,
                  ppszParams,
                  100);
      }

	if (pszSumAmt != NULL)
      ItiMemFree (hheap, pszSumAmt);

   if (usRet)
      {
      ErrCalc ("Not all params available for CalcAlternateEstimate", szTemp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      {
      ErrCalc ("Bad Exec for CalcAlternateEstimate", szTemp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);
   ItiDbUpdateBuffers ("JobAlternate");
   return 0;
   }/* End of Function CalcAlternateEstimate */







int EXPORT RecalcJobAltForJob(HHEAP hheap, PSZ pszJobKey)

   {
   char   szTemp [250] = "";
   USHORT i;
   PSZ    *ppszJAltKeys, *ppszPNames, *ppszParams;
   PSZ    pszMoreJIForAlt;
   BOOL   bRet;

  /* Get the JobAlternateGroupKeys and JobAlternateKeys for this Job */

  bRet = GetJobAlternatesForJob(hheap, pszJobKey, &ppszJAltKeys);

  ppszPNames = ItiStrMakePPSZ (hheap, 4, "JobKey", "JobAlternateGroupKey", "JobAlternateKey", NULL);

  if (ppszJAltKeys != NULL)
     {
     /* Determine if there are any jobitems associated with each of the    */
     /* JobAlternateGroups and JobAlternates (across all JobBreakdowns for */
     /* this Job).  If there are no associated job items, set JobAlternate */
     /* Estimate to .00.  If there are job items, calculate the Estimate.  */

     for (i = 0; ppszJAltKeys[i] != NULL; i++)
        {
        sprintf(szTemp, "SELECT JobItemKey "
                        "FROM JobItem "
                        "WHERE JobAlternateGroupKey = %s "
                        "AND JobAlternateKey = %s "
                        "AND JobKey = %s", 
                         ItiDbGetZStr(ppszJAltKeys[i],0), 
                         ItiDbGetZStr(ppszJAltKeys[i],1),
                         pszJobKey);

        pszMoreJIForAlt = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);
    
        ppszParams = ItiStrMakePPSZ (hheap, 4, pszJobKey, ItiDbGetZStr(ppszJAltKeys[i],0), ItiDbGetZStr(ppszJAltKeys[i], 1), NULL);

        if (pszMoreJIForAlt != NULL)
           {
           ItiDBDoCalc(CalcAlternateEstimate, ppszPNames, ppszParams, 0);
           ItiFreeStrArray(hheap, ppszParams, 4);
           }

        else
           {
           ItiDBDoCalc(ChangeJobAltEstToZero, ppszPNames, ppszParams,0);
           ItiFreeStrArray(hheap, ppszParams, 4);
           }

        }/* end of for (i = 0; ppszJAltKeys[i]... loop */

        /* All of the JobAlternates are recalculated, now recalculate the  */
        /* Estimate field of all of the entries in JobAlternateGroup       */
        /* CalcEstimateJobAltGroup also calls function to recalculate the  */
        /* Job EstimateCost                                                */

        ppszParams = ItiStrMakePPSZ (hheap, 4, pszJobKey, ItiDbGetZStr(ppszJAltKeys[0],0), ItiDbGetZStr(ppszJAltKeys[0], 1), NULL);
        ItiDBDoCalc(CalcEstimateJobAltGroup, ppszPNames, ppszParams, 0);

     } /* end of if (ppszJAltKeys... */ 
     ItiFreeStrArray(hheap, ppszParams, 4);
     ItiFreeStrArray(hheap, ppszPNames, 4);
     ItiFreeStrArray (hheap, ppszJAltKeys, (i?i:1));   
     return 0;

   } /* End of function RecalcJobAltForJob */



void EXPORT NewRecalcJob (PSZ pszJobKey)
   {
   PSZ      pszJBKey, *ppszJBKeys, *ppszPNames, *ppszParams;
   USHORT   i, usJobItemsExist;
   HHEAP    hhpNewJobRecalc;

   hhpNewJobRecalc = ItiMemCreateHeap (4096);
   if (hhpNewJobRecalc == NULL)
      {
      ItiErrWriteDebugMessage
         ("CESUtil calc NewRecalcJob failed to create hhpNewJobRecalc.");
      return;
      }

   GetJobBreakdownsForJob (hhpNewJobRecalc, pszJobKey, &ppszJBKeys);
   ppszPNames = ItiStrMakePPSZ (hhpNewJobRecalc, 3, "JobKey", "JobBreakdownKey", NULL);

   if (ppszJBKeys != NULL)
      {
      /* for each JobBreakdown in the Job, calculate the JobBreakdown */
      for (i = 0; ppszJBKeys [i] != NULL; i++)
         {
         pszJBKey = ItiDbGetZStr (ppszJBKeys [i], 0);
         ppszParams = ItiStrMakePPSZ (hhpNewJobRecalc, 3, pszJobKey, pszJBKey, NULL);

         RecalcJobBreakdown (hhpNewJobRecalc, pszJobKey, pszJBKey);

   /* Check if there are any jobitems associated with a particular Job */
   /* and JobBreakdown.  If items exist, call the regular recalc       */
   /* functions.  If there are no job items in this breakdown, call a  */
   /* function to set the DetailedEstimateCost field to .00.           */
   /* A function is then called to finish recalculating JobAlternates. */

         usJobItemsExist = DoMoreJobItemsExist (hhpNewJobRecalc, pszJobKey, pszJBKey);

         if (usJobItemsExist > 0)
            ItiDBDoCalc (CalcDetailedEstimateCost, ppszPNames, ppszParams, 0);
         else
            ItiDBDoCalc(SetDetailedEstimateCostToZero, ppszPNames, ppszParams, 0);

         ItiFreeStrArray (hhpNewJobRecalc, ppszParams, 3);
         RecalcJBFunds(hhpNewJobRecalc, pszJobKey, pszJBKey);
         }/* End of for loop */

      ItiFreeStrArray (hhpNewJobRecalc, ppszJBKeys, (i?i:1));        
      ItiFreeStrArray (hhpNewJobRecalc, ppszPNames, 3);
      RecalcJFForJob(hhpNewJobRecalc, pszJobKey);
      }

   RecalcJobAltForJob(hhpNewJobRecalc, pszJobKey);
   if (NULL != hhpNewJobRecalc)
      ItiMemDestroyHeap (hhpNewJobRecalc);

   }/* End of Function NewRecalcJob */









      

// 
// void EXPORT RecalcJobCrewLaborUsed (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey, PSZ pszJobCrewKey)
// 
//    {
//    PSZ    *ppszLaborerKeys, *ppszParams, *ppszPNames;
//    USHORT i;
// 
//    GetLaborerKeysForCrew (hheap, pszJobKey, pszJobCostSheetKey,
//                           pszJobCrewKey, &ppszLaborerKeys);
// 
//    if (ppszLaborerKeys == NULL)
//       return;
// 
//    /* for each Laborer for this JobCrew, get Rate and OvertimeRate */
//    ppszPNames = ItiStrMakePPSZ (hheap, 5, "JobKey", "JobCostSheetKey",
//                                 "JobCrewKey", "LaborerKey", NULL);
// 
//    for (i = 0; ppszLaborerKeys[i] != NULL; i++)
//       {
//       ppszParams = ItiStrMakePPSZ (hheap, 5, pszJobKey, pszJobCostSheetKey,
//                                    pszJobCrewKey,
//                                    ItiDbGetZStr (ppszLaborerKeys [i], 0),
//                                    NULL);
// 
//       DetermineLaborerRates (hheap, ppszPNames, ppszParams);
//       ItiFreeStrArray (hheap, ppszParams, 5);
//       }
//    ItiFreeStrArray (hheap, ppszLaborerKeys, (i?i:1));
//    ItiFreeStrArray (hheap, ppszPNames, 5);
//    }
// 
// 
// 
// 
// void EXPORT RecalcJobCrewEquipmentUsed (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey, PSZ pszJobCrewKey)
// 
//    {
//    PSZ    *ppszEquipmentKeys;
//    PSZ    *ppszPNames, *ppszParams;
//    USHORT i;
// 
//    GetEquipmentKeysForCrew (hheap, pszJobKey, pszJobCostSheetKey,
//                             pszJobCrewKey, &ppszEquipmentKeys);
//    if (ppszEquipmentKeys == NULL)
//       return;
// 
//    /* for each EquipmentKey for this JobCrew, refresh Rate and OvertimeRate */
//    ppszPNames = ItiStrMakePPSZ (hheap, 5, "JobKey", "JobCostSheetKey", "JobCrewKey", "EquipmentKey", NULL);
// 
//    for (i = 0; ppszEquipmentKeys[i] != NULL; i++)
//       {
//       ppszParams = ItiStrMakePPSZ (hheap, 5, pszJobKey,
//                                    pszJobCostSheetKey, pszJobCrewKey,
//                                    ItiDbGetZStr (ppszEquipmentKeys [i], 0),
//                                    NULL);
// 
//       ItiDBDoCalc (CalcEquipmentRate, ppszPNames, ppszParams, 0);
//       ItiFreeStrArray (hheap, ppszParams, 5);
//       }
//    ItiFreeStrArray (hheap, ppszEquipmentKeys, (i?i:1));
//    ItiFreeStrArray (hheap, ppszPNames, 5);
//    }
// 
// 
// 
// 
// 
// void EXPORT RecalcJobMaterialsUsed (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey)
// 
// 
//    {
//    PSZ    *ppszJobMaterialKeys;
//    PSZ    *ppszParams, *ppszPNames;
//    USHORT i;
// 
//    GetMaterialKeysForJob (hheap, pszJobKey, pszJobCostSheetKey,
//                           &ppszJobMaterialKeys);
//    if (ppszJobMaterialKeys == NULL)
//       return;
// 
//    /* for each MaterialKey for this Job, refresh UnitPrice and Cost */
// 
//    ppszPNames = ItiStrMakePPSZ (hheap, 4, "JobKey", "JobCostSheetKey",
//                                 "MaterialKey", NULL);
// 
//    for (i = 0; ppszJobMaterialKeys[i] != NULL; i++)
//       {
//       ppszParams = ItiStrMakePPSZ (hheap, 4, pszJobKey, pszJobCostSheetKey,
//                                    ItiDbGetZStr (ppszJobMaterialKeys [i], 0),
//                                    NULL);
// 
//       ItiDBDoCalc (CalcMaterialsUnitPrice, ppszPNames, ppszParams, 0);
//       ItiFreeStrArray (hheap, ppszParams, 4);
//       }
//    ItiFreeStrArray (hheap, ppszJobMaterialKeys, (i?i:1));
//    ItiFreeStrArray (hheap, ppszPNames, 4);
//    }
// 



// int EXPORT DetermineLaborerRates (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
//    {
//    char   szTemp  [1024];
//    char   szTemp2 [1024];
//    char   szTemp3 [1024];
//    char   szTemp4  [1024];
//    HWND   hwnd = NULL;
//    PSZ    pszDavisBacon, pszPrefix, pszJobKey, pszRes = NULL;
//    USHORT usRet;
// 
// 
//    /* -- Test if have a value for the LaborRate, otherwise the subqurey   */
//    /* -- would cause the update to fail by trying to insert a null value. */
//    usRet = ItiStrReplaceParams (szTemp,
//                                " SELECT %%sDavisBaconRate"
//                                " FROM LaborerWageAndBenefits, Job, Labor, JobZone"
//                                " WHERE LaborerWageAndBenefits.LaborerKey = %LaborerKey"
//                                " AND LaborerWageAndBenefits.ZoneKey= JobZone.ZoneKey"
//                                " AND JobZone.ZoneTypeKey = Labor.ZoneTypeKey"
//                                " AND JobZone.JobKey = %JobKey"
//                                " AND LaborerWageAndBenefits.LaborerKey = Labor.LaborerKey"       
//                                " AND LaborerWageAndBenefits.BaseDate = Job.BaseDate"
//                                " AND Job.JobKey = %JobKey ",
//                                sizeof szTemp, 
//                                ppszPNames, 
//                                ppszParams, 
//                                100);
// 
//    /* Determines whether the Job uses DavisBacon or NonDavisBacon wages  */
// 
//    pszJobKey = ppszParams[0];
// 
//    sprintf(szTemp2,"SELECT DavisBaconWages "
//                    "FROM Job "
//                    "WHERE JobKey = %s", pszJobKey);
// 
//    pszDavisBacon = ItiDbGetRow1Col1(szTemp2, hheap, 0, 0, 0);
// 
//    /* if the job uses NonDavisBacon wages, the prefix, Non, is attached */
// 
//    pszPrefix = ((pszDavisBacon[0] == '1') ? "" : "Non");
// 	if (pszDavisBacon != NULL)
//       ItiMemFree (hheap, pszDavisBacon);
//    
// 
//    sprintf(szTemp3, szTemp, pszPrefix, pszPrefix);
// 
//    usRet = ItiWndReplaceAllParams(szTemp4, szTemp3, hwnd, hwnd,
//                                  NULL, NULL, 0, FALSE, sizeof szTemp4);
//    if (usRet)
//       {
//       ErrCalc ("Not all params available for CalcLaborRates", szTemp);
//       return 1;
//       }
// 
//    pszRes = ItiDbGetRow1Col1(szTemp4, hheap, 0, 0, 0);
// 
//    if ((pszRes == NULL) || ((*pszRes) == '0'))
//       {
//       usRet = ItiStrReplaceParams (szTemp,
//                                "UPDATE JobCrewLaborUsed"
//                                " SET Rate = 0.00"
//                                ", OvertimeRate = 0.00"
//                                " WHERE JobKey = %JobKey"
//                                " AND JobCostSheetKey = %JobCostSheetKey"
//                                " AND JobCrewKey = %JobCrewKey"
//                                " AND LaborerKey = %LaborerKey",
//                                sizeof szTemp, 
//                                ppszPNames, 
//                                ppszParams, 
//                                100);
// //      ItiErrWriteDebugMessage
// //         ("The following query was used in place of a NULL subquery value: ");
// //      ItiErrWriteDebugMessage(szTemp);
//       }
//    else
//       {
//       usRet = ItiStrReplaceParams (szTemp,
//                                "UPDATE JobCrewLaborUsed"
//                                " SET Rate ="
//                                " (SELECT %%sDavisBaconRate"
//                                " FROM LaborerWageAndBenefits, Job, Labor, JobZone"
//                                " WHERE LaborerWageAndBenefits.LaborerKey = %LaborerKey"
//                                " AND LaborerWageAndBenefits.ZoneKey= JobZone.ZoneKey"
//                                " AND JobZone.ZoneTypeKey = Labor.ZoneTypeKey"
//                                " AND JobZone.JobKey = %JobKey"
//                                " AND LaborerWageAndBenefits.LaborerKey = Labor.LaborerKey"       
//                                " AND LaborerWageAndBenefits.BaseDate = Job.BaseDate"
//                                " AND Job.JobKey = %JobKey),"
//                                " OvertimeRate ="
//                                " (SELECT %%sDavisBaconOvertimeRate"
//                                " FROM LaborerWageAndBenefits, Job, Labor, JobZone"
//                                " WHERE LaborerWageAndBenefits.LaborerKey = %LaborerKey"
//                                " AND LaborerWageAndBenefits.ZoneKey= JobZone.ZoneKey"
//                                " AND JobZone.ZoneTypeKey = Labor.ZoneTypeKey"
//                                " AND JobZone.JobKey = %JobKey"
//                                " AND LaborerWageAndBenefits.LaborerKey = Labor.LaborerKey"       
//                                " AND LaborerWageAndBenefits.BaseDate = Job.BaseDate"
//                                " AND Job.JobKey = %JobKey)"
//                                " WHERE JobKey = %JobKey"
//                                " AND JobCostSheetKey = %JobCostSheetKey"
//                                " AND JobCrewKey = %JobCrewKey"
//                                " AND LaborerKey = %LaborerKey",
//                                sizeof szTemp, 
//                                ppszPNames, 
//                                ppszParams, 
//                                100);
// 
//       /* Determines whether the Job uses DavisBacon or NonDavisBacon wages  */
// 
//       pszJobKey = ppszParams[0];
// 
//       sprintf(szTemp2,"SELECT DavisBaconWages "
//                       "FROM Job "
//                       "WHERE JobKey = %s", pszJobKey);
// 
//       pszDavisBacon = ItiDbGetRow1Col1(szTemp2, hheap, 0, 0, 0);
// 
//       /* if the job uses NonDavisBacon wages, the prefix, Non, is attached */
// 
//       pszPrefix = ((pszDavisBacon[0] == '1') ? "" : "Non");
//       if (pszDavisBacon != NULL)
//          ItiMemFree (hheap, pszDavisBacon);
//       
//       sprintf(szTemp3, szTemp, pszPrefix, pszPrefix);
// 
//       usRet = ItiWndReplaceAllParams(szTemp4, szTemp3, hwnd, hwnd,
//                                     NULL, NULL, 0, FALSE, sizeof szTemp4);
//       if (usRet)
//          return 1;
// 
//       }
// 
// 
//    ItiDbBeginTransaction (hheap);
// 
//    if (ItiDbExecSQLStatement (hheap, szTemp4))
//       {
//       ErrCalc ("Bad Exec for CalcLaborerRates", szTemp4);
//       ItiDbRollbackTransaction (hheap);
//       return 1;
//       }
//    ItiDbCommitTransaction (hheap);
// 
// 
// 
//    /*--- do buffer updates after all calcs ---*/
//    ItiDbUpdateBuffers ("JobCrewLaborUsed");
//    
//    return 0;
//    }







/*
 * Sets the Updates the JobItem UnitPrice field and the ExtendedAmount
 * when PEMETH Average or PEMETH Regression is used 
 *
 * Needs: JobKey
 *        JobItemKey
 *        UnitPrice
 */

int  EXPORT CalcJIUnitPricePEMETH (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
   char   szTemp [500] = "";
   USHORT usRet;

   usRet = ItiStrReplaceParams (szTemp,
                           "/* cesutil.calc.CalcJIUnitPricePEMETH */ " 
                           "UPDATE JobItem "
                             " SET UnitPrice = %UnitPrice " 
                           " WHERE JobKey = %JobKey "
                           " AND JobItemKey = %JobItemKey "
                           "UPDATE JobItem"
                           " SET ExtendedAmount = "
                                 " (SELECT Quantity * UnitPrice "
                                 " FROM JobItem "
                                 " WHERE JobKey= %JobKey"
                                 " AND JobItemKey = %JobItemKey)"
                           " WHERE JobKey = %JobKey "
                           " AND JobItemKey = %JobItemKey ",
                           sizeof szTemp,
                           ppszPNames,
                           ppszParams,
                           100);
   if (usRet)
      {
      return 1;
      }

//   ItiDbBeginTransaction (hheap);
//   if (

      ItiDbExecSQLStatement (hheap, szTemp)  ;

//      )
//      {
//      ItiDbRollbackTransaction (hheap);
//      return 1;
//      }
//   ItiDbCommitTransaction (hheap);

   /*--- do buffer updates after all calcs ---*/
//   ItiDbUpdateBuffers ("JobItem");
   return 0;
   }




int EXPORT DoMoreJobItemsExist (HHEAP hheap, PSZ pszJobKey, PSZ pszJobBreakdownKey)
   {
   char   szTemp [120] = "";
   PSZ    pszJobItemsExist;
   ULONG  ulCount = 0L;

   sprintf(szTemp, "SELECT COUNT(JobItemKey) "
                   "FROM JobItem "
                   "WHERE JobKey = %s "
                   "AND JobBreakdownKey = %s ",
                    pszJobKey, pszJobBreakdownKey);

   pszJobItemsExist = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);

   /* If there are more JobItems associated with this JobBreakdown         */
   /* return the count, to indicate that the JobBreakdown can be recalced  */
   /* by summing up the JobItems in this JobBreakdown.                     */
   /* If there are no JobItems associated with the JobBreakdown, return 0, */
   /* so $.00 can be placed in the JobBreakdown's DetailedEstimate field.  */

   if (pszJobItemsExist != NULL)
      {
      ItiStrToULONG (pszJobItemsExist, &ulCount);
      ItiMemFree (hheap, pszJobItemsExist);
      if (ulCount > 32000L)
         return 32000;
      return (int) ulCount;
      }

   return 0;
   }




int EXPORT SetDetailedEstimateCostToZero (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
   char   szTemp [200] = "";
   USHORT usRet;

   usRet = ItiStrReplaceParams (szTemp, 
                           "UPDATE JobBreakdown "
                           " SET DetailedEstimateCost = 0.0 " 
                           " WHERE JobKey=%JobKey "
                           " AND JobBreakdownKey = %JobBreakdownKey ",
                           sizeof szTemp,
                           ppszPNames,
                           ppszParams,
                           100);
   if (usRet)
      {
      ErrCalc ("Not all params available for SetDetailedEstimateCostToZero", szTemp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      {
      ErrCalc ("Bad Exec for SetDetailedEstimateCostToZero", szTemp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   /*--- do buffer updates after all calcs ---*/

   ItiDbUpdateBuffers ("JobBreakdown");
   ItiDbUpdateBuffers ("Job");

   return 0;
   }





int EXPORT CalcEstimateJobAltGroup (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
   char   szTemp  [510] = "";
   char   szTemp2 [510] = "";
   PSZ    pszAmt = NULL;
   USHORT usRet;

   usRet = ItiStrReplaceParams (szTemp2, 
                           " /*  CESUtil calc.c CalcEstimateJobAltGroup */ "
                           " SELECT JA.Estimate"
                           " FROM JobAlternate JA, JobAlternateGroup JAG"
                           " WHERE JA.JobKey=%JobKey "
                           " AND JAG.JobKey=%JobKey "
                           " AND JAG.ChosenAlternateKey=JA.JobAlternateKey"
                           " AND JAG.JobAlternateGroupKey=JA.JobAlternateGroupKey",
                           sizeof szTemp2,
                           ppszPNames,
                           ppszParams,
                           100);

   pszAmt = ItiDbGetRow1Col1(szTemp2, hheap, 0, 0, 0);

   if ((pszAmt == NULL) || ((*pszAmt) == '0') )
      {
      usRet = ItiStrReplaceParams (szTemp, 
                           "/* CESUtil calc.c CalcEstimateJobAltGroup */ "
                           "UPDATE JobAlternateGroup"
                           " SET Estimate = 0.00" 
                           " WHERE JobKey=%JobKey",
                           sizeof szTemp,
                           ppszPNames,
                           ppszParams,
                           100);
      }
   else
      {
      usRet = ItiStrReplaceParams (szTemp, 
                           "/* calc.c CalcEstimateJobAltGroup */ "
                           "UPDATE JobAlternateGroup"
                           " SET Estimate =" 
                           " (SELECT JA.Estimate"
                           " FROM JobAlternate JA"
                           " WHERE JA.JobKey=%JobKey "
                           " AND JobAlternateGroup.ChosenAlternateKey=JA.JobAlternateKey"
                           " AND JobAlternateGroup.JobAlternateGroupKey=JA.JobAlternateGroupKey)"
                           " WHERE JobKey=%JobKey",
                           sizeof szTemp,
                           ppszPNames,
                           ppszParams,
                           100);
      }

   if (pszAmt != NULL)
      ItiMemFree (hheap, pszAmt);

   if (usRet)
      {
      ErrCalc ("Not all params available for CalcEstimateJobAltGroup", szTemp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTemp))
      {
      ErrCalc ("Bad Exec for CalcEstimateJobAltGroup", szTemp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   usRet = CalcJobEstimateCost (hheap, ppszPNames, ppszParams);

   /*--- do buffer updates after all calcs ---*/

   ItiDbUpdateBuffers ("JobAlternateGroup");

   return usRet;
   }








int EXPORT CalcAllJobFundsForJob (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
   CHAR   szJobKey  [10] = "";
   CHAR   szTemp2  [510] = "";
   CHAR   szTemp3  [510] = "";
   USHORT usRet, i, usCols [] = {1,2,0};
   PSZ    pszJobKey, pszConstrEngrPct, pszContingPct, *ppszJFKeys;
   PSZ    pszAmt = NULL;
   PSZ    *ppszJFPNames, *ppszJFParams;
      
   /* Get ContingencyPercent and ConstructionEngineeringPct from Job */
   pszJobKey = ItiDbGetZStr(ppszParams[1],0); 
   if (NULL == pszJobKey)
      return 13;
   ItiStrCpy (szJobKey, pszJobKey, KEYLENGTH);


   sprintf (szTemp2, " SELECT ContingencyPercent "
                    " FROM Job "
                    " WHERE JobKey = %s", szJobKey);
   pszContingPct = ItiDbGetRow1Col1(szTemp2, hheap, 0, 0, 0);

   sprintf (szTemp2, " SELECT ConstructionEngineeringPct "
                    " FROM Job "
                    " WHERE JobKey = %s", szJobKey);
   pszConstrEngrPct = ItiDbGetRow1Col1(szTemp2, hheap, 0, 0, 0);


   GetJobFundsForJob (hheap, szJobKey, &ppszJFKeys);

   ItiDbBeginTransaction (hheap);

for (i = 0; ppszJFKeys[i] != NULL; i++)
   {
   /* -- Check subquery for a null value. */
   sprintf(szTemp3,
                 "/* CESUtil calc.c CalcAllJobFundsForJob */ "
                 "SELECT SUM(JBF.AmountFunded) "
                 "FROM JobBreakdownFund JBF, JobFund JF "
                 "WHERE JBF.JobKey = JF.JobKey "
                 "AND JBF.FundKey = JF.FundKey "
                 "AND JF.JobKey = %s "
                 "AND JF.FundKey = %s "
                 , szJobKey, ppszJFKeys[i]);

   pszAmt = ItiDbGetRow1Col1(szTemp3, hheap, 0, 0, 0);

   if ((pszAmt == NULL) || ((*pszAmt) == '0') )
      {
      sprintf (szTemp3,
                 "/* CESUtil calc.c CalcAllJobFundsForJob */ "
                 "UPDATE JobFund "
                 "SET AmountFunded = 0.00 "
                 "WHERE JobFund.JobKey = %JobKey "
                 " AND JobFund.FundKey = %s ",
                 szJobKey, ppszJFKeys[i]);
      }
   else
      {
       sprintf(szTemp3, 
                   "/* CESUtil calc.c CalcAllJobFundsForJob */ "
                   "UPDATE JobFund "
                   "SET AmountFunded = "
                   "( (SELECT SUM(JBF.AmountFunded) "
                   "FROM JobBreakdownFund JBF, JobFund JF "
                   "WHERE JBF.JobKey = JF.JobKey "
                   "AND JF.JobKey = %s "
                   "AND JF.FundKey = %s "
                   "AND JBF.FundKey = JF.FundKey) * (1 + %s + %s))"
                   " WHERE JobFund.JobKey = %s "
                   "AND JobFund.FundKey = %s ",
              pszJobKey, ppszJFKeys[i],
              pszContingPct, pszConstrEngrPct,
              pszJobKey, ppszJFKeys[i]);
      }

   if (pszAmt != NULL)
      ItiMemFree (hheap, pszAmt);

   if (ItiDbExecSQLStatement (hheap, szTemp3))
      {
      ErrCalc ("Bad Exec for CalcAllJobFundsForJob", szTemp3);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }

   }/* end for */

   ItiDbCommitTransaction (hheap);
   if (ppszJFKeys != NULL)
      {
      /* for each JobFund for this Job, recalc the PercentFunded */
      ppszJFPNames = ItiStrMakePPSZ (hheap, 3, "JobKey", "FundKey", NULL);

      for (i = 0; ppszJFKeys [i] != NULL; i++)
        {
        ppszJFParams = ItiStrMakePPSZ (hheap, 3, pszJobKey,
                                       ItiDbGetZStr(ppszJFKeys[i], 0), NULL);
        // ItiDBDoCalc(CalcJFPctFunded, ppszJFPNames, ppszJFParams, 0);
        CalcJFPctFunded (hheap, ppszJFPNames, ppszJFParams);
        ItiFreeStrArray (hheap, ppszJFParams, 3);
        }
      ItiFreeStrArray (hheap, ppszJFPNames, 3);

      }
   
   /*--- do buffer updates after all calcs ---*/
   if (ppszParams[2] != NULL)
      {
      usRet = CalcEstTotlProgJob (hheap, ppszPNames, ppszParams);
      }

   ItiDbUpdateBuffers ("JobFund");
   return 0;
   }/* End of Function CalcAllJobFundsForJob */






/* =============================================================================== */
/* =============================================================================== */
/*                                                                                 */
/* Functions that obtain arrays of keys needed for recalc functions                */
/*                                                                                 */
/* =============================================================================== */
/* =============================================================================== */








BOOL EXPORT GetCrewsForJob (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey, 
                            PSZ **pppszJobCrewKeys)
   {
   char    szTemp [125] = "";
   USHORT  usErr;
   USHORT  usCols [] = {1,0};

   /* This query obtains all of the Crews associated with a Job   */
   /* and stores them in an array                                 */ 

   sprintf (szTemp, "SELECT JobCrewKey " 
                    "FROM JobCrewUsed "
                    "WHERE JobKey = %s "
                    "AND JobCostSheetKey = %s", pszJobKey,
                                                pszJobCostSheetKey);

   usErr = ItiDbBuildSelectedKeyArray2(hheap, 0, szTemp,
                                       pppszJobCrewKeys, usCols);
   if (usErr != 0)
      return FALSE;

   return TRUE;
   }/* End of Function GetCrewsForJob */




BOOL EXPORT GetLaborerKeysForCrew (HHEAP hheap, PSZ pszJobKey, 
                                   PSZ pszJobCostSheetKey, PSZ pszJobCrewKey,
                                   PSZ **pppszLaborerKeys)
   {
   char    szTempGLK [300] = "";
   USHORT  usErr;
   USHORT  usCols [] = {1,0};

   /* This query obtains all of the Laborers associated with a JobCrew */
   /* and stores them in an array                                      */ 

   sprintf (szTempGLK, "SELECT LaborerKey " 
                    "FROM JobCrewLaborUsed "
                    "WHERE JobKey = %s "
                    "AND JobCostSheetKey = %s "
                    "AND JobCrewKey = %s", pszJobKey,
                                           pszJobCostSheetKey,
                                           pszJobCrewKey);

   usErr = ItiDbBuildSelectedKeyArray2(hheap, 0, szTempGLK,
                                       pppszLaborerKeys, usCols);
   if (usErr != 0)
      return FALSE;

   return TRUE;
   }/* End of Function GetLaborerKeysForCrew */






BOOL EXPORT GetEquipmentKeysForCrew (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey,
                                     PSZ pszJobCrewKey,
                                     PSZ **pppszEquipmentKeys)
   {
   char    szTempE [250] = "";
   USHORT  usErr;
   USHORT  usCols [] = {1,0};

   /* This query obtains all of the Equipment associated with a JobCrew */
   /* and stores them in an array                                       */ 

   sprintf (szTempE, "SELECT EquipmentKey " 
                    "FROM JobCrewEquipmentUsed "
                    "WHERE JobKey = %s "
                    "AND JobCostSheetKey = %s "
                    "AND JobCrewKey = %s", pszJobKey,
                                           pszJobCostSheetKey,
                                           pszJobCrewKey);

   usErr = ItiDbBuildSelectedKeyArray2(hheap, 0, szTempE,
                                       pppszEquipmentKeys, usCols);
   if (usErr != 0)
      return FALSE;

   return TRUE;
   }/* End of Function GetEquipmentKeysForCrew */




BOOL EXPORT GetMaterialKeysForJob (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey, 
                                   PSZ **pppszJobMaterialKeys)
   {
   char    szTempM [250] = "";
   USHORT  usErr;
   USHORT  usCols [] = {1,0};

   /* This query obtains all of the Materials associated with a Job   */
   /* and stores them in an array                                     */ 

   sprintf (szTempM, "SELECT MaterialKey " 
                    "FROM JobMaterialUsed "
                    "WHERE JobKey = %s "
                    "AND JobCostSheetKey = %s", pszJobKey,
                                                pszJobCostSheetKey);

   usErr = ItiDbBuildSelectedKeyArray2(hheap, 0, szTempM,
                                       pppszJobMaterialKeys, usCols);
   if (usErr != 0)
      return FALSE;

   return TRUE;
   }/* End of Function GetMaterialKeysForJob */



// ********************************** REWRITTEN FUNCTIONS FOR 1.0g *****

/* ------------------------------------------------------------- */
/* --   FUNCTION: GetJobItemsForJobBreakdown                     */
/* -- PARAMETERS: hheap                                          */
/* --             pszJobKey                                      */
/* --             pszJobBreakdownKey                             */
/* --             **pppszJobItemKeys                             */
/* --                                                            */
/* -- DESCRIPTION:                                               */
/* --    This function obtains all of the JobItems associated    */
/* --    with a JobBreakdown and stores the JobItem keys in an   */
/* --    array.                                                  */
/* --                                                            */
/* -- RETURN: TURE  Array was built.                             */
/* --         FALSE Failed to build array.                       */
/* ------------------------------------------------------------- */
BOOL EXPORT GetJobItemsForJobBreakdown (HHEAP hheap, PSZ pszJobKey, PSZ pszJobBreakdownKey, PSZ **pppszJobItemKeys)
   {
   char    szTemp [250] = "";
   USHORT  usErr;
   USHORT  usCols [] = {1,2,3,4,0};

   sprintf (szTemp, "SELECT JobItemKey, ItemEstimationMethodKey,"
                    " Quantity, StandardItemKey " 
                    " FROM JobItem"
                    " WHERE JobKey = %s "
                    " AND JobBreakdownKey = %s"
                    ,pszJobKey, pszJobBreakdownKey);

// Due to memory problems, try filling a static array here.
   usErr = ItiDbBuildSelectedKeyArray2(hheap, 0, szTemp,
                                       pppszJobItemKeys, usCols);
   if (usErr != 0)
      return FALSE;

   return TRUE;        
   }/* End of Function GetJobItemsForJobBreakdown */






/* ------------------------------------------------------------- */
/* --   FUNCTION: RecalcPEMETHAverage                            */
/* -- PARAMETERS: hheap                                          */
/* --             pszJobKey                                      */
/* --             pszJobBreakdownKey                             */
/* --             pszJobItemKey                                  */
/* --             pszQuantity                                    */
/* --             pszStandardItemKey                             */
/* --                                                            */
/* -- DESCRIPTION:                                               */
/* --    Estimates the JobItem unit price with the PEMETH        */
/* --    Average data, iff it is within 5 to 95%iles for         */
/* --    the given StandardItem.                                 */
/* --                                                            */
/* -- RETURN: void                                               */
/* ------------------------------------------------------------- */
void EXPORT RecalcPEMETHAverage (HHEAP hheap,
                                 PSZ   pszJobKey,
                                 PSZ   pszJobBreakdownKey,
                                 PSZ   pszJobItemKey,
                                 PSZ   pszQuantity,
                                 PSZ   pszStandardItemKey)
   {
   PSZ    pszUnitPrice = NULL;
   PSZ    *ppszPNames, *ppszParams;
   CHAR szTemp [500] = "";

   pszUnitPrice = ItiEstEstimateItem(hheap, "Average", NULL, pszJobKey,
                                     pszJobBreakdownKey, pszStandardItemKey,
                                     pszQuantity);

   if ((pszUnitPrice == NULL) || (*pszUnitPrice == '\0'))
      return;
   else
      {
      ItiExtract(pszUnitPrice, ",");
      ppszPNames = ItiStrMakePPSZ (hheap, 4, "JobKey", "JobItemKey", "UnitPrice", NULL);
      ppszParams = ItiStrMakePPSZ (hheap, 4, pszJobKey, pszJobItemKey, pszUnitPrice, NULL);
      // ItiDBDoCalc(CalcJIUnitPricePEMETH, ppszPNames, ppszParams, 0);
//////    /////
//  ---sub'ed below:  CalcJIUnitPricePEMETH (hheap, ppszPNames, ppszParams);
/////     /////
      ItiStrReplaceParams(szTemp,
         " UPDATE JobItem "
            "SET UnitPrice = %UnitPrice "
         " WHERE JobKey = %JobKey "
         " AND JobItemKey = %JobItemKey "
         " UPDATE JobItem "
            "SET ExtendedAmount = "
            " (select Quantity * UnitPrice "
              "from JobItem "
              "where JobKey = %JobKey "
              "and JobItemKey = %JobItemKey ) "
            "WHERE JobKey = %JobKey "
            "AND JobItemKey = %JobItemKey "
            , sizeof szTemp, ppszPNames, ppszParams, 20);
      ItiDbExecSQLStatement (hheap, szTemp);
////      /////
      ItiFreeStrArray(hheap, ppszPNames, 4);
      ItiFreeStrArray(hheap, ppszParams, 4);
      ItiMemFree (hheap, pszUnitPrice);
      }
   }/* End of Function RecalcPEMETHAverage */




/* ------------------------------------------------------------- */
/* --   FUNCTION: RecalcPEMETHRegression                         */
/* -- PARAMETERS: hheap                                          */
/* --             pszJobKey                                      */
/* --             pszJobBreakdownKey                             */
/* --             pszJobItemKey                                  */
/* --             pszQuantity                                    */
/* --             pszStandardItemKey                             */
/* --                                                            */
/* -- DESCRIPTION:                                               */
/* --    Estimates the JobItem unit price with the PEMETH        */
/* --    Regression data, iff it is within the minimum and       */
/* --    maximum limits for that StandardItem.                   */
/* --                                                            */
/* -- RETURN: void                                               */
/* ------------------------------------------------------------- */
void EXPORT RecalcPEMETHRegression (HHEAP hheap,
                                    PSZ   pszJobKey,
                                    PSZ   pszJobBreakdownKey,
                                    PSZ   pszJobItemKey,
                                    PSZ   pszQuantity,
                                    PSZ   pszStandardItemKey)
   {
   PSZ    pszUnitPrice;
   PSZ    *ppszPNames, *ppszParams;

   pszUnitPrice = ItiEstEstimateItem(hheap, "Regress", NULL, pszJobKey,
                                     pszJobBreakdownKey, pszStandardItemKey,
                                     pszQuantity);

   if ((pszUnitPrice == NULL) || (*pszUnitPrice == '\0'))
      return;
   else 
      {
      ItiExtract(pszUnitPrice, ",");
      ppszPNames = ItiStrMakePPSZ (hheap, 4, "JobKey", "JobItemKey", "UnitPrice", NULL);
      ppszParams = ItiStrMakePPSZ (hheap, 4, pszJobKey, pszJobItemKey, pszUnitPrice, NULL);
      // ItiDBDoCalc(CalcJIUnitPricePEMETH, ppszPNames, ppszParams, 0);
///// //
      CalcJIUnitPricePEMETH (hheap, ppszPNames, ppszParams);
/////

///// //
      ItiFreeStrArray(hheap, ppszPNames, 4);
      ItiFreeStrArray(hheap, ppszParams, 4);
      ItiMemFree (hheap, pszUnitPrice);
      }
   }/* End of Function RecalcPEMETHRegression */



/* ------------------------------------------------------------- */
/* --   FUNCTION: RecalcDefaultPrice                             */
/* -- PARAMETERS: hheap                                          */
/* --             pszJobKey                                      */
/* --             pszJobBreakdownKey                             */
/* --             pszJobItemKey                                  */
/* --             pszQuantity                                    */
/* --             pszStandardItemKey                             */
/* --                                                            */
/* -- DESCRIPTION:                                               */
/* --    Sets the JobItem unit price with the price from the     */
/* --    Standard Item Default Price catalog, rounded to the     */
/* --    unit specified in the Standard Item Catalog.            */
/* --                                                            */
/* -- RETURN: 0 if okay                                          */
/* --         1 if no default price available                    */
/* --        13 if an error                                      */
/* ------------------------------------------------------------- */
int EXPORT RecalcDefaultPrice (HHEAP hheap,
                                PSZ   pszJobKey,
                                PSZ   pszJobBreakdownKey,
                                PSZ   pszJobItemKey,
                                PSZ   pszQuantity,
                                PSZ   pszStandardItemKey)
   {
   PSZ  pszRes = NULL;  
   PSZ  pszPrice = NULL;  
   CHAR szTemp [510] = "";

   ItiStrCpy (szTemp,"/* cesutil.calc.RecalcDefaultPrice */ "
                  " SELECT UnitPrice "
                  " FROM StdItemDefaultPrices "
                  " WHERE Deleted = NULL"
                  " AND BaseDate = "
                            "(SELECT MAX(BaseDate) "
                             " FROM StdItemDefaultPrices"
                             " WHERE Deleted = NULL"
                             " AND StandardItemKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszStandardItemKey, sizeof szTemp);
   ItiStrCat (szTemp, " )  AND StandardItemKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszStandardItemKey, sizeof szTemp);

   pszRes = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);
   if ((NULL != pszRes) && (*pszRes != '\0'))
        { /* -- We found a default unit price so use it. */
        ItiExtract(pszRes, ",$");
        pszPrice = ItiEstRoundUnitPrice
                      (hheap, ItiStrToDouble (pszRes), pszStandardItemKey);
        ItiMemFree (hheap, pszRes);

        ItiStrCpy (szTemp, "/* cesutil.calc.RecalcDefaultPrice */ "
                       "UPDATE JobItem "
                       " SET UnitPrice = ", sizeof szTemp);
        ItiStrCat (szTemp, pszPrice, sizeof szTemp);
        ItiStrCat (szTemp, " , ExtendedAmount = (Quantity * ", sizeof szTemp);
        ItiStrCat (szTemp, pszPrice, sizeof szTemp);
        ItiStrCat (szTemp, ") WHERE JobKey = ", sizeof szTemp);
        ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
        ItiStrCat (szTemp, " AND JobItemKey = ", sizeof szTemp);
        ItiStrCat (szTemp,  pszJobItemKey, sizeof szTemp);

        if (ItiDbExecSQLStatement (hheap, szTemp))
           {
           ErrCalc ("Bad Exec for: ", szTemp);
           ItiMemFree (hheap, pszPrice);
           return 13;
           }
        ItiMemFree (hheap, pszPrice);
        }
      else
        return 1; /* no default price available */

   return 0;
   }/* End of Function RecalcDefaultPrice */




/* ------------------------------------------------------------- */
/* --   FUNCTION: RecalcCBEST                                    */
/* -- PARAMETERS: hheap                                          */
/* --             pszJobKey                                      */
/* --             pszJobBreakdownKey                             */
/* --             pszJobItemKey                                  */
/* --             pszQuantity                                    */
/* --             pszStandardItemKey                             */
/* --                                                            */
/* -- DESCRIPTION:                                               */
/* --    Sets the JobItem unit price with the price from the     */
/* --    recalculated ACTIVE cost based estimate.                */
/* --                                                            */
/* -- RETURN: 0 if okay                                          */
/* --         1 if no cost sheets are available                  */
/* --        13 if error occured                                 */
/* ------------------------------------------------------------- */
int EXPORT RecalcCBEST(HHEAP hheap,
                       PSZ   pszJobKey,
                       PSZ   pszJobBreakdownKey,
                       PSZ   pszJobItemKey,
                       PSZ   pszQuantity,
                       PSZ   pszStandardItemKey)
   {
//   char   szRecalcBuffer [1024];
   PSZ    pszJICBEKey, pszJCSKey;
   PSZ    *ppszJCstShtKeys;
   USHORT i;

   ItiDbExecSQLStatement (hheap,"select '*-* BEGIN RecalcCBEST ' ");

   /* -- If exists, get the Active CostBasedEstimate for this JobItem. */
   ItiStrCpy (szRecalcBuffer,
            "/* CESUtil.calc.RecalcCBEST */ "
            " SELECT JobItemCostBasedEstimateKey"
            " FROM JobCostBasedEstimate"
            " WHERE JobKey = ", sizeof szRecalcBuffer);
   ItiStrCat (szRecalcBuffer, pszJobKey, sizeof szRecalcBuffer);
   ItiStrCat (szRecalcBuffer, " AND JobItemKey = ", sizeof szRecalcBuffer);
   ItiStrCat (szRecalcBuffer, pszJobItemKey, sizeof szRecalcBuffer);
   ItiStrCat (szRecalcBuffer,
            " AND 0 < "
            " (SELECT COUNT (*)"
            "  FROM JobCostSheet JCS"
            "  WHERE JCS.JobItemCostBasedEstimateKey ="
                   " JobCostBasedEstimate.JobItemCostBasedEstimateKey)"
            " AND Active = 1 ", sizeof szRecalcBuffer);

   pszJICBEKey = ItiDbGetRow1Col1 (szRecalcBuffer, hheap, 0, 0, 0);
   if (pszJICBEKey == NULL)
      return 1;

   /* -- Get JobCostSheetKeys (tasks) that are associated with the */
   /* -- Active CostBasedEstimate for this particular JobItem.     */
   GetJobCostSheetsForJobItem (hheap, pszJobKey, pszJobItemKey, 
                               pszJICBEKey, &ppszJCstShtKeys);
   if (ppszJCstShtKeys == NULL)
      return 1;

   /* -- For each JobCostSheet for the JobItem, calculate         */
   /* -- the Labor, Material, Equipment and UnitPrice.            */
   for (i = 0; ppszJCstShtKeys[i] != NULL; i++)
      {
      pszJCSKey = ItiDbGetZStr (ppszJCstShtKeys [i], 0);
      RecalcJobCostSheet (hheap, pszJobKey, pszJCSKey);
      }
   ItiFreeStrArray (hheap, ppszJCstShtKeys, i);

   /* -- Now update JobItem table data. */
   ItiStrCpy (szRecalcBuffer,
            "/* CESUtil.calc.RecalcCBEST */ "
            " UPDATE JobItem "
            " SET UnitPrice = "
            "(SELECT SUM(QuantityPerItemUnit * UnitPrice) "
            " FROM JobCostSheet "
            " WHERE JobKey = ", sizeof szRecalcBuffer);
   ItiStrCat (szRecalcBuffer, pszJobKey, sizeof szRecalcBuffer);
   ItiStrCat (szRecalcBuffer, 
            " AND JobItemKey = ", sizeof szRecalcBuffer);
   ItiStrCat (szRecalcBuffer, pszJobItemKey, sizeof szRecalcBuffer);
   ItiStrCat (szRecalcBuffer, 
            " AND JobItemCostBasedEstimateKey = ", sizeof szRecalcBuffer);
   ItiStrCat (szRecalcBuffer, pszJICBEKey, sizeof szRecalcBuffer);
   ItiStrCat (szRecalcBuffer,
            " ) WHERE JobKey = ", sizeof szRecalcBuffer);
   ItiStrCat (szRecalcBuffer, pszJobKey, sizeof szRecalcBuffer);
   ItiStrCat (szRecalcBuffer,
            " AND JobItemKey = ", sizeof szRecalcBuffer);
   ItiStrCat (szRecalcBuffer, pszJobItemKey, sizeof szRecalcBuffer);

	if (pszJICBEKey != NULL)
      ItiMemFree (hheap, pszJICBEKey);

   if (ItiDbExecSQLStatement (hheap, szRecalcBuffer))
      {
      ErrCalc ("Bad Exec for: ", szRecalcBuffer);
      return 13;
      }

   ItiStrCpy (szRecalcBuffer,
                  "/* cesutil.calc.RecalcCBEST */ "
                  "UPDATE JobItem "
                  " SET"
                  " ExtendedAmount = (Quantity * UnitPrice)"
                  " WHERE JobKey = ", sizeof szRecalcBuffer);
   ItiStrCat (szRecalcBuffer, pszJobKey, sizeof szRecalcBuffer);
   ItiStrCat (szRecalcBuffer, " AND JobItemKey = ", sizeof szRecalcBuffer);
   ItiStrCat (szRecalcBuffer,  pszJobItemKey, sizeof szRecalcBuffer);

   if (ItiDbExecSQLStatement (hheap, szRecalcBuffer))
      {
      ErrCalc ("Bad Exec for: ", szRecalcBuffer);
      return 13;
      }

   ItiDbExecSQLStatement (hheap,"select '*-* END RecalcCBEST ' ");

   return 0;
   }/* End of Function RecalcCBEST */



/* ------------------------------------------------------------- */
/* --   FUNCTION: RecalcJobItem                                  */
/* -- PARAMETERS: hheap                                          */
/* --             pszJobKey                                      */
/* --             pszJobBreakdownKey                             */
/* --             pszJobItemKey                                  */
/* --             pszQuantity                                    */
/* --             pszStandardItemKey                             */
/* --             pszItemEstMethodKey                            */
/* --                                                            */
/* -- DESCRIPTION:                                               */
/* --    Calls a Recalc function selected by using the given     */
/* --    ItemEstMethodKey.                                       */ 
/* --                                                            */
/* -- RETURN: 0 if okay                                          */
/* --        13 if missing keys                                  */
/* ------------------------------------------------------------- */
int EXPORT RecalcJobItem (HHEAP hheap,
                          PSZ   pszJobKey,
                          PSZ   pszJobBreakdownKey,
                          PSZ   pszJobItemKey,
                          PSZ   pszQuantity,
                          PSZ   pszStandardItemKey,
                          PSZ   pszItemEstMethodKey)
   {
   USHORT usRet = 0;

   if (pszItemEstMethodKey == NULL)
      {
      ItiErrWriteDebugMessage
         ("\nCESUtil.calc.c RecalcJobItem was given a NULL ItemEstMethodKey. \n");
      return 13;
      }

   if (NULL == pszJobKey)
      {
      ItiErrWriteDebugMessage
         ("CESUtil.calc.c RecalcJobItem has a NULL pszJobKey");
      return 13;
      }
   if (NULL == pszJobBreakdownKey)
      {
      ItiErrWriteDebugMessage
         ("CESUtil.calc.c RecalcJobItem has a NULL pszJobBreakdownKey");
      }
   if (NULL == pszJobItemKey)
      {
      ItiErrWriteDebugMessage
         ("CESUtil.calc.c RecalcJobItem has a NULL pszJobItemKey");
      }
   if (NULL == pszQuantity)
      {
      ItiErrWriteDebugMessage
         ("CESUtil.calc.c RecalcJobItem has a NULL pszQuantity");
      }
   if (NULL == pszStandardItemKey)
      {
      ItiErrWriteDebugMessage
         ("CESUtil.calc.c RecalcJobItem has a NULL pszStandardItemKey");
      }


   switch (atol (pszItemEstMethodKey))
      {  
      case PEMETHAverage:
          RecalcPEMETHAverage (hheap,
                               pszJobKey,
                               pszJobBreakdownKey,
                               pszJobItemKey,
                               pszQuantity,
                               pszStandardItemKey);
          break;

      case PEMETHRegression:
          RecalcPEMETHRegression (hheap,
                                  pszJobKey,
                                  pszJobBreakdownKey,
                                  pszJobItemKey,
                                  pszQuantity,
                                  pszStandardItemKey);
          break;
        
      case CBEST:
         { 
         if ((NULL == pszJobItemKey) || (NULL == pszJobKey))
            return 13;

         RecalcCBEST (hheap,
                      pszJobKey,
                      pszJobBreakdownKey,
                      pszJobItemKey,
                      pszQuantity,
                      pszStandardItemKey);
         break;
         }

      case DEFAULTMTH: /* Need a reprice function here. */
         RecalcDefaultPrice (hheap,
                             pszJobKey,
                             pszJobBreakdownKey,
                             pszJobItemKey,
                             pszQuantity,
                             pszStandardItemKey);
         break;
        
      case ADHOC:
      default:
           /* No recalc needed for AdHoc */
         break;
      } /* end switch */

   return 0;
   } /* End of function RecalcJobItem */




/* ------------------------------------------------------------- */
/* --   FUNCTION: RecalcAllJobBrkdwnItems                        */
/* -- PARAMETERS: hheap                                          */
/* --             pszJobKey                                      */
/* --             pszJobBreakdownKey                             */
/* --                                                            */
/* -- DESCRIPTION:                                               */
/* --    Bulids a list of the Job Breakdown's Items and calls    */
/* --    the RecalcJobItem function for each item in the list.   */
/* --                                                            */
/* -- RETURN: 0 if okay                                          */
/* --         1 if there are no JobItem keys                     */
/* --        13 if no heap space is available                    */
/* ------------------------------------------------------------- */
int EXPORT RecalcAllJobBrkdwnItems (HHEAP hheap, PSZ pszJobKey, PSZ pszJobBreakdownKey)
   {     /* Was previously named RecalcJobItems */
   CHAR szJobKey           [KEYLENGTH+1] = "";
   CHAR szJobBrkdwnKey     [KEYLENGTH+1] = "";
   CHAR szJobItemKey       [KEYLENGTH+1] = "";
   CHAR szItemEstMethodKey [KEYLENGTH+1] = "";
   CHAR szQuantity         [KEYLENGTH+1] = "";
   CHAR szStdItemKey       [KEYLENGTH+1] = "";
   PSZ  pszTemp;
   PSZ    *ppszJobItemKeys = NULL;  
   BOOL   bRet;
   USHORT i;
   HHEAP  hhp;
   
   /* -- Make local copies of the keys. */
   ItiStrCpy (szJobKey, pszJobKey, KEYLENGTH);
   ItiStrCpy (szJobBrkdwnKey, pszJobBreakdownKey, KEYLENGTH);

   hhp = ItiMemCreateHeap (MAX_HEAP_SIZE);
   if (NULL == hhp)
      {
      ItiErrWriteDebugMessage
         ("CESUtil.calc.c RecalcAllJobBrkdwnItems could not create heap.");
      return 13;
      }

   bRet = GetJobItemsForJobBreakdown
              (hhp, szJobKey, szJobBrkdwnKey, &ppszJobItemKeys);

   if (ppszJobItemKeys == NULL)
      return 1;

   /* -- For each JobItem in the JobBreakdown, recalculate the JobItem. */
   for (i = 0; ppszJobItemKeys [i] != NULL; i++)
      {
      pszTemp = ItiDbGetZStr(ppszJobItemKeys[i],0);  //pszJobItemKey
      ItiStrCpy(szJobItemKey, pszTemp, KEYLENGTH);

      pszTemp = ItiDbGetZStr(ppszJobItemKeys[i],1);  //pszItemEstMethodKey
      ItiStrCpy(szItemEstMethodKey, pszTemp, KEYLENGTH);

      pszTemp = ItiDbGetZStr(ppszJobItemKeys[i],2);  //pszQuantity
      ItiStrCpy(szQuantity, pszTemp, KEYLENGTH);

      pszTemp = ItiDbGetZStr(ppszJobItemKeys[i],3);  //pszStdItemKey
      ItiStrCpy(szStdItemKey, pszTemp, KEYLENGTH);

      RecalcJobItem(hhp,
                    pszJobKey,
                    pszJobBreakdownKey,
                    szJobItemKey,        
                    szQuantity,          
                    szStdItemKey,       
                    szItemEstMethodKey);  
      } /* end for loop */

   ItiFreeStrArray(hhp, ppszJobItemKeys, (i?i:1));

   if (NULL != hhp)
      ItiMemDestroyHeap (hhp);

   return 0;
   }/* End of Function RecalcAllJobBrkdwnItems */


#undef RETURN_ERROR 
#undef UNFUNDEDKEY    
#undef KEYLENGTH      
#undef MAXCREWSPERJCS 
#undef MAXLABORERSPERJCS 
#undef MAXEQUIPSPERJCS 

