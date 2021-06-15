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
 *  CalcFn.c
 *
 *  This module contains all the functions that perform calculations
 *
 */

#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\winid.h"
#include "..\include\itidbase.h"
#include "..\include\itiutil.h"
#include "..\include\itiglob.h"
#include "..\include\itibase.h"
#include "..\include\itierror.h"
#include "..\include\colid.h"
#include "..\include\cesutil.h"
#include "..\include\itifmt.h"
#include "..\include\itiest.h"
#include "..\include\cbest.h"
#include "bamsces.h"
#include "init.h"
#include "calcfn.h"
#include <stdio.h>
#include <process.h>
#include <stddef.h>

#define KEYLEN      8
//#define SZLEN      15
#define SZLEN      8

#define  TRCALC(f)   /* ItiErrWriteDebugMessage ( f )  */


static CHAR szEstKeyRegress[] = "1000002";
static CHAR szEstKeyAverage[] = "1000003";
static CHAR szEstKeyCBEst[]   = "1000001";
static CHAR szEstKeyDefault[] = "1000004";
static CHAR szEstKeyAdHoc[]   = "1000000";

static CHAR szEstIDRegress[] = "Regress";
static CHAR szEstIDAverage[] = "Average";
static CHAR szEstIDCBEst[]   = "CBEst";
static CHAR szEstIDDefault[] = "Default";
static CHAR szEstIDAdHoc[]   = "Ad-Hoc";

static CHAR  szTempCASEM [250];
static CHAR  szTmpCASEM  [250];
static CHAR  szTempAEM   [570];

static CHAR aszJIKeys  [NUMBER_OF_JI_KEYS + 1][3][SZLEN];

static char  szTmp [1244] = "";

/*
 * 1> High level functions, such as DoJobRecalc, which get keys
 *     and call the ItiDBDoCalc function.
 *    These are called by you and return immediately 
 *
 * 2> Low Level functions, such as CalcDetailedEstimateCost, which
 *     are called by the recalc thread to do the actual calculation.
 *    These block.
 *
 *
 */



static void ShowStackInfo (HWND hwnd)
   {
   PCHAR    pcStack;
   SEL      selStack;
   USHORT   usStackMax;
   BOOL     bDone;
   USHORT   usUsed;
   char     szMessage [160];
   PVOID    pvBottom;

   pvBottom = &pvBottom;
   pcStack = (PCHAR) &pcStack;
   selStack = SELECTOROF (pcStack);
   pcStack = MAKEP (selStack, 0);
   usStackMax = ItiMemQuerySeg (&selStack);

   /* search for bottom of stack */
   bDone = FALSE;
   while (OFFSETOF (pcStack) < usStackMax && !bDone)
      {
      bDone = *pcStack != '\0';
      pcStack++;
      }
   pcStack--;

   usUsed = usStackMax - OFFSETOF (pcStack);
   sprintf (szMessage, "Stack size = %u (0x%x)  "
                       "Stack Bottom = %u (0x%x)  "
                       "Current SP = %u:%u (0x%x:0x%x)  "
                       "Bytes Used = %u (0x%x)",
            usStackMax, usStackMax, 
            OFFSETOF (pcStack), OFFSETOF (pcStack),
            selStack, OFFSETOF (pvBottom), selStack, OFFSETOF (pvBottom), 
            usUsed, usUsed);

   ItiErrWriteDebugMessage (szMessage);
   // WinMessageBox (HWND_DESKTOP, hwnd, szMessage, "StackInfo", 0, MB_OK);

   } /* End of Function ShowStackInfo */



USHORT ErrCalc (PSZ psz1, PSZ psz2)
   {
   //char  szTmp [200];

   sprintf (szTmp, "CALC: %s %s", psz1, psz2);
   ItiErrWriteDebugMessage (szTmp);

   DosBeep (1000, 10); DosBeep (1502, 22);

   if (pglobals && pglobals->hwndDesktop && pglobals->hwndAppFrame)
      WinMessageBox (pglobals->hwndDesktop, pglobals->hwndAppFrame, szTmp,
                     "CALC", 0, MB_OK | MB_SYSTEMMODAL | MB_MOVEABLE);
   return 1;
   }




/*******************************************************************/
/*                    HIGH LEVEL FUNCTIONS                         */
/*    These functions are here for convenience only                */
/*******************************************************************/






void DoJobItemCalc(HHEAP hheap, PSZ pszJobKey, PSZ pszJobItemKey)

   {
   PSZ *ppszPNames, *ppszParams;

	if (pszJobItemKey == NULL)
		{
		ItiErrWriteDebugMessage
			("Error Condition: NULL JobItemKey given to DoJobItemCalc.");
      return;
		}


   ppszPNames = ItiStrMakePPSZ (hheap, 3, "JobKey", "JobItemKey", NULL);
   ppszParams = ItiStrMakePPSZ (hheap, 3, pszJobKey, pszJobItemKey, NULL);

   TRCALC( "Do Calc: CalcJobItem" );
   CalcJobItem (hheap, ppszPNames, ppszParams);

   ItiFreeStrArray (hheap, ppszPNames, 3);
   ItiFreeStrArray (hheap, ppszParams, 3);
   }





void DoJobBreakdownFundReCalc (HWND hwnd, HWND hwndList)
   {
   USHORT uRow;
   PSZ   pszJobKey, pszFundKey, pszJobBreakdownKey, pszProgramKey;
   //char  szTmp [140];
   PSZ   *ppszPNames, *ppszParams;
   HHEAP hheap, hheapStatic;
   HWND  hwndStatic;



   hwndStatic = QW(hwnd, ITIWND_OWNERWND, 1, 0, 0);


   hheapStatic       = (HHEAP) QW (hwndStatic, ITIWND_HEAP, 0, 0, 0);
   pszJobKey         = (PSZ)   QW (hwndStatic, ITIWND_DATA, 0, 0, cJobKey);
   pszJobBreakdownKey= (PSZ)   QW (hwndStatic, ITIWND_DATA, 0, 0, cJobBreakdownKey);
   hheap             = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0);
   uRow              = (UM)    QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);
   pszFundKey        = (PSZ)   QW (hwndList, ITIWND_DATA, 0, uRow, cFundKey);

   /* Determines whether the Job is currently in a Program */

   sprintf(szTmp,"SELECT ProgramKey "
                  "FROM ProgramJob "
                  "WHERE JobKey = %s", pszJobKey);

   pszProgramKey = ItiDbGetRow1Col1(szTmp, hheap, 0, 0, 0);
   

   ppszPNames = ItiStrMakePPSZ (hheap, 5, "JobKey", "FundKey", "JobBreakdownKey", "ProgramKey", NULL);
   ppszParams = ItiStrMakePPSZ (hheap, 5, pszJobKey, pszFundKey, pszJobBreakdownKey,  pszProgramKey, NULL);

	TRCALC( "Do Calc: CalcPctFundedInitFund" );
   ItiDBDoCalc (CalcPctFundedInitFund, ppszPNames, ppszParams, 0);


   ItiMemFree(hheapStatic, pszJobKey);
   ItiMemFree(hheapStatic, pszJobBreakdownKey);
   ItiMemFree(hheap, pszProgramKey);
   ItiFreeStrArray (hheap, ppszPNames, 5);
   ItiFreeStrArray (hheap, ppszParams, 5);
   }



void DoJobReCalcForJIDelete (HWND hwnd, PSZ *ppszSelKeys)
   {
   PSZ      pszJobKey, pszJobBreakdnKey;
   PSZ      *ppszPNames, *ppszParams;
   HHEAP    hheap;
   USHORT   usJobItemsExist;


   hheap            = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
   pszJobKey        = (PSZ)   QW (hwnd, ITIWND_DATA, 0, 0, cJobKey);
   pszJobBreakdnKey = (PSZ)   QW (hwnd, ITIWND_DATA, 0, 0, cJobBreakdownKey);

   ppszPNames = ItiStrMakePPSZ (hheap, 3, "JobBreakdownKey", "JobKey", NULL);
   ppszParams = ItiStrMakePPSZ (hheap, 3, pszJobBreakdnKey, pszJobKey, NULL);

   /* Check if more jobitems are associated with a particular Job and      */
   /* JobBreakdown. If more items exist, call the regular recalc functions */
   /* If you are deleting the last item in this breakdown, call a function */
   /* to set the DetailedEstimateCost field to .00.  A function is then    */
   /* called to finish recalculating JobAlternates.                        */


   usJobItemsExist = DoMoreJobItemsExist (hheap, pszJobKey, pszJobBreakdnKey);

   if (usJobItemsExist > 0) 
		{
      TRCALC( "Do Calc: CalcDetailedEstimateCost" );
      CalcDetailedEstimateCost (hheap, ppszPNames, ppszParams);
		}
   else
      {
      TRCALC( "Do Calc: SetDetailedEstimateCostToZero" );
      SetDetailedEstimateCostToZero (hheap, ppszPNames, ppszParams);
//      ReCalcJobAltEstForDelete(hwnd, ppszSelKeys);
      }

   ReCalcJobAltEstForDelete(hwnd, ppszSelKeys);

   ItiMemFree(hheap, pszJobKey);
   ItiMemFree(hheap, pszJobBreakdnKey);
   ItiFreeStrArray (hheap, ppszPNames, 3);
   ItiFreeStrArray (hheap, ppszParams, 3);
   }




int EXPORT ReCalcJobAltEstForDelete(HWND hwnd, PSZ *ppszSelKeys)

   {

   char   szTemp [250];
   USHORT i;
   PSZ    pszMoreJIForAlt, pszJobKey, pszJobBreakdnKey;
   PSZ    *ppszPNames, *ppszParams;
   HHEAP  hheap;


   hheap            = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
   pszJobKey        = (PSZ)   QW (hwnd, ITIWND_DATA, 0, 0, cJobKey);
   pszJobBreakdnKey = (PSZ)   QW (hwnd, ITIWND_DATA, 0, 0, cJobBreakdownKey);

   /* For every jobitem that the user is deleting, determine if there are any           */
   /* remaining job items associated with the same Job, JobAlternate, JobAlternateGroup */
   /* If there are no remaining jobitems, call a function to set the Estimate field     */
   /* in the JobAlternate table to .00.  If there are remaining jobitems, call the      */
   /* regular calculation function.                                                     */     


   for (i = 0; ppszSelKeys[i] != NULL; i++)
      {

      sprintf(szTemp,
                      "/*  ReCalcJobAltEstForDelete */ " 
                      "SELECT JobItemKey "
                      "FROM JobItem "
                      "WHERE JobAlternateGroupKey = %s "
                      "AND JobAlternateKey = %s "
                      "AND JobKey = %s", 
                       ItiDbGetZStr(ppszSelKeys[i],2), 
                       ItiDbGetZStr(ppszSelKeys[i],3),
                       ItiDbGetZStr(ppszSelKeys[i],0));

      pszMoreJIForAlt = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);
      
      if (pszMoreJIForAlt != NULL)
         {
         ppszPNames = ItiStrMakePPSZ (hheap, 3, "JobBreakdownKey", "JobKey", NULL);
         ppszParams = ItiStrMakePPSZ (hheap, 3, pszJobBreakdnKey, pszJobKey, NULL);
         TRCALC( "Do Calc: CalcAlternateGroupEstimate" );
         CalcAlternateGroupEstimate (hheap, ppszPNames, ppszParams);
         ItiFreeStrArray(hheap, ppszPNames, 3);
         ItiFreeStrArray(hheap, ppszParams, 3);
         }

      else
         {
         ppszPNames = ItiStrMakePPSZ (hheap, 4, "JobKey", "JobAlternateGroupKey", "JobAlternateKey", NULL);
         ppszParams = ItiStrMakePPSZ (hheap, 4, pszJobKey, ItiDbGetZStr(ppszSelKeys[i], 2), ItiDbGetZStr(ppszSelKeys[i], 3), NULL);

			TRCALC( "Do Calc: SetJobAltEstToZero" );
         SetJobAltEstToZero (hheap, ppszPNames, ppszParams);
         ItiFreeStrArray(hheap, ppszPNames, 4);
         ItiFreeStrArray(hheap, ppszParams, 4);
         }

      }

      ItiMemFree(hheap, pszJobKey);
      ItiMemFree(hheap, pszJobBreakdnKey);
      return 0;

   }




void DoDelProgJobReCalc(HWND hwnd)
   {

   PSZ    pszProgramKey;
   PSZ    *ppszPNames, *ppszParams;
   HHEAP  hheap;
   HWND   hwndParent;



   hwndParent  = (HWND) QW (hwnd, ITIWND_OWNERWND,0, 0, 0);
   hheap = (HHEAP) QW (hwndParent, ITIWND_HEAP, 0, 0, 0);
   pszProgramKey = (PSZ) QW (hwndParent, ITIWND_DATA, 0, 0, cProgramKey);

   ppszPNames = ItiStrMakePPSZ (hheap, 2, "ProgramKey", NULL);
   ppszParams = ItiStrMakePPSZ (hheap, 2, pszProgramKey, NULL);

	TRCALC( "Do Calc: ReCalcProgram" );
   ItiDBDoCalc(ReCalcProgram, ppszPNames, ppszParams, 0);
	TRCALC( "Do Calc: DelObsoleteProgFunds" );
   ItiDBDoCalc(DelObsoleteProgFunds, ppszPNames, ppszParams, 0);

   ItiMemFree(hheap, pszProgramKey);
   ItiFreeStrArray (hheap, ppszPNames, 2);
   ItiFreeStrArray (hheap, ppszParams, 2);
   }







void DoDelFundReCalc (HWND hwnd, HHEAP hheapKeys, PSZ *ppszSelKeys)
   {

   USHORT i;
   PSZ *ppszPNames, apszKeyParams [4];


   i = 0;


   ppszPNames = ItiStrMakePPSZ(hheapKeys, 4, "JobKey", "JobBreakdownKey", "FundKey", NULL);
   apszKeyParams[3] = NULL;


   /* updating JobBreakdownFund after deletions in the JBF table */
   /* getting JobKey and JobBreakdownKey                         */

   apszKeyParams[0] =  ItiDbGetZStr(ppszSelKeys[i],0);
   apszKeyParams[1] =  ItiDbGetZStr(ppszSelKeys[i],1);
   apszKeyParams[2] =  ItiDbGetZStr(ppszSelKeys[i],2);

	TRCALC( "Do Calc: CalcJobBreakFundAfterDel" );
	ItiDBDoCalc(CalcJobBreakFundAfterDel, ppszPNames, apszKeyParams, 0);


   for (i = 0; ppszSelKeys[i] != NULL; i++)
      {

      apszKeyParams[0] = ItiDbGetZStr(ppszSelKeys[i],0);
      apszKeyParams[1] = ItiDbGetZStr(ppszSelKeys[i],1);
      apszKeyParams[2] = ItiDbGetZStr(ppszSelKeys[i],2);



		TRCALC( "Do Calc: CalcJobFundAfterDel" );
		ItiDBDoCalc(CalcJobFundAfterDel,ppszPNames, apszKeyParams, 0);
      ItiMemFree (hheapKeys, ppszSelKeys [i]);
      }
   ItiFreeStrArray(hheapKeys, ppszPNames, 4);
   ItiMemFree (hheapKeys, ppszSelKeys);
   }




void DoProgramJobReCalc(HWND hwnd, HWND hwndList, PSZ pszJobKey, PSZ pszProgramKey)
   {
   PSZ    *ppszPNames, *ppszParams;
   HHEAP  hheap;

   hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);

   ppszPNames = ItiStrMakePPSZ (hheap, 3, "JobKey", "ProgramKey", NULL);
   ppszParams = ItiStrMakePPSZ (hheap, 3, pszJobKey, pszProgramKey, NULL);

	TRCALC( "Do Calc: CalcEstTotlProgJob" );
	ItiDBDoCalc (CalcEstTotlProgJob, ppszPNames, ppszParams, 0);

   ItiFreeStrArray (hheap, ppszPNames, 3);
   ItiFreeStrArray (hheap, ppszParams, 3);
   }




void DoRecalcJobTotal(HWND hwnd)

   {
   PSZ   pszJobKey;
   PSZ   *ppszPNames, *ppszParams;
   HHEAP hheap;

   hheap              = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
   pszJobKey          = (PSZ)   QW (hwnd, ITIWND_DATA, 0, 0, cJobKey);

   ppszPNames = ItiStrMakePPSZ (hheap, 2, "JobKey", NULL);
   ppszParams = ItiStrMakePPSZ (hheap, 2, pszJobKey, NULL);

	TRCALC( "Do Calc: CalcJobEstimateTotal" );
	CalcJobEstimateTotal (hheap, ppszPNames, ppszParams);

   ItiMemFree(hheap, pszJobKey);
   ItiFreeStrArray (hheap, ppszPNames, 2);
   ItiFreeStrArray (hheap, ppszParams, 2);
   }







/*******************************************************************/
/*                     LOW LEVEL FUNCTIONS                         */
/*    These functions must all have the same header definition,    */
/*    (I.E. hheap, ppsz, ppsz, and return an int)                  */
/*******************************************************************/

/*
 * This function sets the Estimate field in the JobAlternate table to .00.
 * This function is called when there are no remaining job items associated
 * with this JobAlternate.
 *
 * Needs:  JobKey
 *
 */ 

int EXPORT SetJobAltEstToZero (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
   //char   szTmp [250];
   USHORT uRet;

   uRet = ItiStrReplaceParams (szTmp,
                           "/*  SetJobAltEstToZero */ "
                           " UPDATE JobAlternate "
                           " SET Estimate = 0.0 " 
                           " WHERE JobKey = %JobKey "
                           " AND JobAlternateGroupKey = %JobAlternateGroupKey "
                           " AND JobAlternateKey = %JobAlternateKey",
                           sizeof szTmp,
                           ppszPNames,
                           ppszParams,
                           100);

   if (uRet)
      {
      ErrCalc ("Not all params available for SetJobAltEstToZero", szTmp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTmp))
      {
      ErrCalc ("Bad Exec for SetJobAltEstToZero", szTmp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   /*--- do buffer updates after all calcs ---*/
   uRet = CalcEstimateJobAltGroup(hheap, ppszPNames, ppszParams);

   ItiDbUpdateBuffers ("JobAlternate");

   return uRet;
   }





int EXPORT DoMoreJobItemsInJobAltExist (HHEAP hheap, PSZ pszJobKey)
   {
   char   szTemp [400];
   PSZ    pszSum;



   sprintf(szTemp,
        "/*  DoMoreJobItemsInJobAltExist */ "
        "SELECT SUM(JobItem.ExtendedAmount) "
        "FROM JobItem JI, JobBreakdown JB, JobAlternate JA "
        "WHERE JI.JobKey =  %JobKey "
        "AND JB.JobKey = %JobKey " 
        "AND JB.DetailedEstimate = 1 "
        "AND JB.JobBreakdownKey = JI.JobBreakdownKey "
        "AND JA.JobAlternateKey = JI.JobAlternateKey "
        "AND JA.JobAlternateGroupKey = JI.JobAlternateGroupKey ",
        pszJobKey);


   pszSum = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);

   /* If the result of this query is NULL, then there are no other         */
   /* JobItems associated with this Job, JobAlternate, JobAlternateGroup.  */
   /* If the  result is Null, return 1, else return 0.                     */

   if (pszSum != NULL)
      return 0;
   else return 1;

   }








/*
 * Adjusts the PercentFunded field for FundKey = 1, in the JobBreakdownFund table.
 * Initially, the PercentFunded for FundKey = 1 is 100%.  As new JobBreakdownFunds
 * are added the PercentFunded for FundKey =1, is decreased.  When
 * JobBreakdownFunds are deleted, this field is increased.  And, when the 
 * PercentFunded for a  current JobBreakdownFund is changed, the field must
 * also be adjusted.
 *
 * Needs:  JobKey, JobBreakdownKey
 *
 */ 

int EXPORT CalcPctFundedInitFund (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
//   char   szTmp [1020];
   USHORT uRet;

   uRet = ItiStrReplaceParams (szTmp,
               "/*CalcPctFundedInitFund*/ "
               "UPDATE JobBreakdownFund "
               "SET PercentFunded = 1.00 - "
               "(SELECT SUM(PercentFunded) "
               "FROM JobBreakdownFund "
               "WHERE JobBreakdownKey = %JobBreakdownKey "
               "AND JobKey = %JobKey "
               "AND FundKey != 1) "
               "WHERE JobKey = %JobKey "
               "AND JobBreakdownKey = %JobBreakdownKey " 
               "AND FundKey = 1  "
               "UPDATE JobBreakdownFund "
               "SET AmountFunded = PercentFunded * "
               "(Select (((1 - DetailedEstimate) * PreliminaryEstimateCost) "
               "+ (convert(int,DetailedEstimate) * DetailedEstimateCost)) "
               "From JobBreakdown "
               "WHERE JobBreakdown.JobKey = %JobKey "
               "AND JobBreakdown.JobBreakdownKey = %JobBreakdownKey) "
               "WHERE JobKey = %JobKey "
               "AND JobBreakdownKey = %JobBreakdownKey "
               "AND FundKey = 1 ", 
               sizeof szTmp,
               ppszPNames,
               ppszParams,
               100);


   if (uRet)
      {
      ErrCalc ("Not all params available for CalcPctFundedInitFund", szTmp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTmp))
      {
      ErrCalc ("Bad Exec for CalcPctFundedInitFund", szTmp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   /*--- do buffer updates after all calcs ---*/
   uRet = CalcJobBreakAmtFunded (hheap, ppszPNames, ppszParams);
   ItiDbUpdateBuffers ("JobBreakdownFund");
   return uRet;
   }






/*
 * Calculates the AmountFunded for the JobBreakdownFund table
 *
 * Needs: JobKey, JobBreakdownKey, FundKey
 */

int EXPORT CalcJobBreakAmtFunded (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
//   char   szTmp [450];
   USHORT uRet;

   uRet = ItiStrReplaceParams (szTmp,
         "/*  CalcJobBreakAmtFunded */ "
         "UPDATE JobBreakdownFund "
         "SET AmountFunded = PercentFunded * "
         "(Select (((1 - DetailedEstimate) * PreliminaryEstimateCost) "
         "+ (convert(int,DetailedEstimate)  * DetailedEstimateCost)) "
         "From JobBreakdown "
         "WHERE JobBreakdown.JobKey = %JobKey "
         "AND JobBreakdown.JobBreakdownKey = %JobBreakdownKey) "
         "WHERE JobKey = %JobKey "
         "AND JobBreakdownKey = %JobBreakdownKey "
         "AND FundKey = %FundKey ", 
          sizeof szTmp,
          ppszPNames,
          ppszParams,
          100);


   if (uRet)
      {
      ErrCalc ("Not all params available for CalcJobBreakdownFund", szTmp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTmp))
      {
      ErrCalc ("Bad Exec for CalcJobBreakdownFund", szTmp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   /*--- do buffer updates after all calcs ---*/
   uRet = CalcJobFund (hheap, ppszPNames, ppszParams);
   ItiDbUpdateBuffers ("JobBreakdownFund");
   return uRet;
   }





/*
 * Calculates the AmountFunded for the JobFund table
 *
 * Needs: JobKey, FundKey
 */

int EXPORT CalcJobFund (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
//   char   szTmp [500] = "";
   char   szTmp2[500] = "";
   char   szTmp3[500] = "";
   char   szTmp4[500] = "";
   char   szTmp5[500] = "";
   char   szTmp6[500] = "";
   PSZ    pszPctFnd1, pszPctFnd2, pszContingPct, pszConstrEngrPct, *ppszPct;
   PSZ    pszJobKey, pszAmt, pszAmt1, pszAmtTtl;
   USHORT uRet, usErr, uCols [] = {1,2,0};
   INT    i;


// ItiErrWriteDebugMessage ("\n The ppszParms in CalcJobFund.");
// i = 0;
// while (ppszParams[i] != NULL)
// 	{
//    ItiErrWriteDebugMessage (ppszPNames[i]);
//    ItiErrWriteDebugMessage (ppszParams[i]);
// 	i++;
// 	}
// ItiErrWriteDebugMessage ("Done listing ppszParms in CalcJobFund. \n ");


   /* Get ContingencyPercent and ConstructionEngineeringPct from Job */
   pszJobKey = ItiDbGetZStr(ppszParams[0],0); 


   sprintf (szTmp5, " SELECT ContingencyPercent, ConstructionEngineeringPct "
                    " FROM Job "
                    " WHERE JobKey = %s", pszJobKey);

   usErr = ItiDbBuildSelectedKeyArray2(hheap, 0, szTmp5,
                                       &ppszPct, uCols);

   if (usErr != 0)
      return FALSE;

   pszContingPct = ItiDbGetZStr (ppszPct [0], 0);
   pszConstrEngrPct = ItiDbGetZStr (ppszPct [0], 1);



	/* -- Check for null subqueries. */
   uRet = ItiStrReplaceParams (szTmp2,
               "/*  CalcJobFund */ "
               "SELECT SUM(AmountFunded) "
               "FROM JobBreakdownFund "
               "WHERE FundKey = %FundKey "
               "AND JobKey = %JobKey",
               sizeof szTmp2,
               ppszPNames,
               ppszParams,
               100);
   pszAmt = ItiDbGetRow1Col1(szTmp2, hheap, 0, 0, 0);

   uRet = ItiStrReplaceParams (szTmp3, 
               "/*  CalcJobFund */ "
               "SELECT SUM(AmountFunded) "
               "FROM JobBreakdownFund "
               "WHERE FundKey = 1 "
               "AND JobKey = %JobKey",
               sizeof szTmp3,
               ppszPNames,
               ppszParams,
               100);
   pszAmt1 = ItiDbGetRow1Col1(szTmp3, hheap, 0, 0, 0);
   
	if ( ((pszAmt1 == NULL) && (pszAmt == NULL))
		   || ( ((*pszAmt1) == '0') && ((*(pszAmt)) == '0') ) )
		{/* -- Query of all fund keys came back null, so set to 0.00 */
		uRet = ItiStrReplaceParams (szTmp3, 
               "/*  CalcJobFund */ "
               "UPDATE JobFund "
               "SET AmountFunded = 0.00 "
               "WHERE JobKey = %JobKey "
               "AND FundKey = 1 "
               "UPDATE JobFund "
               "SET AmountFunded = 0.00 "
               "WHERE JobKey = %JobKey "
               "AND FundKey = %FundKey ",
               sizeof szTmp3,
               ppszPNames,
               ppszParams,
               100);
		}
	else if ((pszAmt1 == NULL)
		     || ( ((*pszAmt1) == '0') && ((*(pszAmt1+1)) == '0') ) )
		{ /* -- Only the fund key == 1 was null. */
      uRet = ItiStrReplaceParams (szTmp3, 
               "/*  CalcJobFund */ "
               "UPDATE JobFund "
               "SET AmountFunded = 0.00 "
               "WHERE JobKey = %JobKey "
               "AND FundKey = 1 "
               "UPDATE JobFund "
               "SET AmountFunded =  "
               "(SELECT (SELECT SUM(AmountFunded) "
               "FROM JobBreakdownFund "
               "WHERE FundKey = %FundKey "
               "AND JobKey = %JobKey) * (1 + %s + %s)) "
               "WHERE JobKey = %JobKey "
               "AND FundKey = %FundKey ",
               sizeof szTmp3,
               ppszPNames,
               ppszParams,
               100);
		}
	else if ((pszAmt == NULL)
		     || ( ((*pszAmt) == '0') && ((*(pszAmt+1)) == '0') ) )
		{
      uRet = ItiStrReplaceParams (szTmp3, 
               "/*  CalcJobFund */ "
               "UPDATE JobFund "
               "SET AmountFunded =  "
               "(SELECT (SELECT SUM(AmountFunded) "
               "FROM JobBreakdownFund "
               "WHERE FundKey = 1 "
               "AND JobKey = %JobKey) * (1 + %s + %s)) "
               "WHERE JobKey = %JobKey "
               "AND FundKey = 1 "
               "UPDATE JobFund "
               "SET AmountFunded = 0.00 "
               "WHERE JobKey = %JobKey "
               "AND FundKey = %FundKey ",
               sizeof szTmp3,
               ppszPNames,
               ppszParams,
               100);
		}
	else
      uRet = ItiStrReplaceParams (szTmp3, 
               "/*  CalcJobFund */ "
               "UPDATE JobFund "
               "SET AmountFunded = "
               "(SELECT (SELECT SUM(AmountFunded) "
               "FROM JobBreakdownFund "
               "WHERE FundKey = 1 "
               "AND JobKey = %JobKey) * (1 + %s + %s)) "
               "WHERE JobKey = %JobKey "
               "AND FundKey = 1 "
               "UPDATE JobFund "
               "SET AmountFunded = "
               "(SELECT (SELECT SUM(AmountFunded) "
               "FROM JobBreakdownFund "
               "WHERE FundKey = %FundKey "
               "AND JobKey = %JobKey) * (1 + %s + %s)) "
               "WHERE JobKey = %JobKey "
               "AND FundKey = %FundKey ",
               sizeof szTmp3,
               ppszPNames,
               ppszParams,
               100);

   sprintf(szTmp6, szTmp3, pszContingPct, pszConstrEngrPct, pszContingPct, pszConstrEngrPct);
   ItiFreeStrArray(hheap, ppszPct, 2);

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTmp6))
      {
      ErrCalc ("Failed Execution for CalcJobFund-AmtFunded", szTmp6);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);


    /* Determining Percent Funded for FundKey 1 - Must be calculated */
    /* after the Amount Funded fields have been updated              */
   uRet = ItiStrReplaceParams(szTmp2,
             "/*  CalcJobFund */ "
             "SELECT EstimateTotal FROM Job WHERE JobKey = %JobKey ",
             sizeof szTmp2, ppszPNames, ppszParams, 100);
   pszAmtTtl = ItiDbGetRow1Col1(szTmp2, hheap, 0, 0, 0);

 	if ( ((pszAmt1 == NULL) && (pszAmtTtl != NULL))
		 || ( ((*pszAmt1) == '0') && ((*(pszAmt1+1)) == '0') ) )
      { /* -- In this case we have a sum(AmountFunded{key = 1}) = 0 since
         * -- the query result was null; so we have the situation of
         * -- 0/EstimateTotal which means we have NO precentage of the
         * -- total funded.
         */
      uRet = ItiStrReplaceParams(szTmp2,
                "SELECT 0.00 ", sizeof szTmp2, ppszPNames, ppszParams, 100);
		}
	else if ((pszAmtTtl == NULL)
		      || ((pszAmtTtl != NULL) && ((*pszAmtTtl) == '0') ) ) 
      { /* -- Here we have a non-zero sum(AmountFunded) but the EstimateTotal
         * -- query is zero or null leaving us with a division by zero
         * -- situation.  For whatever reason the Job breakdown total
         * -- estimates have not been placed into the Job table; so now
         * -- how do we report the percentage of the "0" job total that is
         * -- funded?  For now we will say 0%.
         * -- Perhaps we should force a Job recalculation here to update the
         * -- Job Estimate Total.
         */
      uRet = ItiStrReplaceParams(szTmp2, "SELECT 0.0 ", sizeof szTmp2,
                                 ppszPNames, ppszParams, 100);

//      TRCALC( "Do Calc: update CalcDetailedEstimateCost for Fund calcs." );
//      ItiDBDoCalc (CalcDetailedEstimateCost, ppszPNames, ppszParams, 0);
//		  /* -- Do some kind of <wait> for results here. */
//      uRet = ItiStrReplaceParams(szTmp2,
//                "SELECT (select sum(AmountFunded) "
//                "FROM JobFund WHERE JobKey = %JobKey AND FundKey = 1)/ "
//                "(SELECT EstimateTotal FROM Job WHERE JobKey = %JobKey) ",
//                sizeof szTmp2, ppszPNames, ppszParams, 100);
		}
	else
      uRet = ItiStrReplaceParams(szTmp2,
                             "/*  CalcJobFund */ "
                             "SELECT (select sum(AmountFunded) "
                             "FROM JobFund "
                             "WHERE JobKey = %JobKey "
                             "AND FundKey = 1)/ "
                             "(SELECT EstimateTotal "
                             "FROM Job "
                             "WHERE JobKey = %JobKey) ",
                             sizeof szTmp2,
                             ppszPNames,
                             ppszParams,
                             100);
//   if (uRet)
//      {
//      ErrCalc ("Not all params available for Calc ", szTmp2);
//      return 1;
//      }

   pszPctFnd1 = ItiDbGetRow1Col1(szTmp2, hheap, 0, 0, 0);




	 /* ============================================================= */
    /*        The Percent Funded for %FundKey  -- Must be updated    */
    /* after the Amount Funded fields have been updated and the      */
	 /* percentages have been calculated.										*/


 	if ( ((pszAmt == NULL) && (pszAmtTtl != NULL))
		 || ( ((*pszAmt) == '0') && ((*(pszAmt+1)) == '0') ) )
      { /* -- In this case we have a sum(AmountFunded) = 0 since
         * -- the query result was null; so we have the situation of
         * -- 0/EstimateTotal which means we have NO precentage of the
         * -- total funded.
         */
      uRet = ItiStrReplaceParams(szTmp,
                "SELECT 0.00 ", sizeof szTmp, ppszPNames, ppszParams, 100);
		}
	else if ((pszAmtTtl == NULL)
		      || ((pszAmtTtl != NULL) && ((*pszAmtTtl) == '0') ) ) 
      { /* -- Here we have a non-zero sum(AmountFunded) but the EstimateTotal
         * -- query is zero or null leaving us with a division by zero
         * -- situation.  For whatever reason the Job breakdown total
         * -- estimates have not been placed into the Job table; so now
         * -- how do we report the percentage of the "0" job total that is
         * -- funded?  For now we will say 0%.
         */
      uRet = ItiStrReplaceParams(szTmp, "SELECT 0.0 ", sizeof szTmp,
                                 ppszPNames, ppszParams, 100);
		}
	else
      uRet = ItiStrReplaceParams(szTmp,
                             "/*  CalcJobFund */ "
                             "SELECT (select sum(AmountFunded) "
                             "FROM JobFund "
                             "WHERE JobKey = %JobKey "
                             "AND FundKey = %FundKey)/ "
                             "(SELECT EstimateTotal "
                             "FROM Job "
                             "WHERE JobKey = %JobKey) ",
                             sizeof szTmp,
                             ppszPNames,
                             ppszParams,
                             100);



//   if (uRet)
//      {
//      ErrCalc ("Not all params available for Calc", szTmp2);
//      return 1;
//      }



   pszPctFnd2 = ItiDbGetRow1Col1(szTmp, hheap, 0, 0, 0);

   uRet = ItiStrReplaceParams (szTmp4,
                              "/*  CalcJobFund */ "
                              " UPDATE JobFund "
                              " SET PercentFunded = %s "
                              " WHERE JobKey = %JobKey "
                              " AND FundKey = 1 "
                              " UPDATE JobFund "
                              " SET PercentFunded = %s "
                              " WHERE JobKey = %JobKey "
                              " AND FundKey = %FundKey ",
                               sizeof szTmp3,
                               ppszPNames,
                               ppszParams,
                               100);



   sprintf (szTmp, szTmp4, pszPctFnd1, pszPctFnd2);

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTmp))
      {
      ErrCalc ("Bad Exec for CalcJobFundPctFunded", szTmp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);


   /*--- Do buffer updates after all calcs; iff we   */
   /* -- find the program key in the parameter list. */
   i = 0;
   while ((ppszPNames[i] != NULL) && (ppszPNames[i][0] != '\0'))
      {
      if (0 == ItiStrCmp(ppszPNames[i], "ProgramKey"))
         {
//BSR# 930108-4101 Although ppszPNames did contain 'ProgramKey'
//                 however, somehow the corresponding ppszParams
//                 value is null so the following check was added.
         if ((ppszParams[i] != NULL) && (ppszParams[i][0] != '\0'))
            uRet = CalcProgramFund (hheap, ppszPNames, ppszParams);
//			else
//            ItiErrWriteDebugMessage
//				   (" -- CalcJobFund ProgramKey parameter is null.");
			break;
         }
      else
         i++;
      }

   
   ItiDbUpdateBuffers ("JobFund");
   return uRet;
   } /* -- End of Function CalcJobFund */








/*
 * Calculates the AmountFunded and PercentFunded for the ProgramFund table
 *
 * Needs: ProgramKey, FundKey, and JobKey in the parameter list.
 */

int EXPORT CalcProgramFund (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
   USHORT uRet;
//	INT    i;

//ItiErrWriteDebugMessage ("\n The ppszParms in ..\\bamsces\\calcfn.c -- CalcProgramFund.");
//i = 0;
//while (ppszParams[i] != NULL)
//	{
//   ItiErrWriteDebugMessage (ppszPNames[i]);
//   ItiErrWriteDebugMessage (ppszParams[i]);
//	i++;
//	}
//ItiErrWriteDebugMessage (" Done listing ppszParms in CalcProgramFund. \n ");


   uRet = CalcAllProgramFunds (hheap, ppszPNames, ppszParams);
   if (uRet)
      {
      ErrCalc ("Call to CalcAllProgramFunds failed in CalcProgramFund. ", ppszParams[3]);
      return 1;
      }

   /*--- do buffer updates after all calcs ---*/
   ItiDbUpdateBuffers ("ProgramFund");
   return 0;
   }





/* Recalculates the JobBreakdownFund after a Deletion
 *
 *
 * Needs:  JobKey, JobBreakdownKey
 *
 */


 int EXPORT CalcJobBreakFundAfterDel (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
//   char   szTmp  [550];
   char   szTmp1 [550];
   USHORT uRet;
   PSZ    pszPctFnd1;
   char   szPercent [50];



  /*
   * To determine if there are any remaining funds for this JobBreakdown 
   * in order to recalculate FundKey  = 1 
   */


   uRet = ItiStrReplaceParams (szTmp, 
               "/*  CalcJobBreakFundAfterDel */ "
                               "SELECT SUM(PercentFunded) "
                               "FROM JobBreakdownFund "
                               "WHERE JobBreakdownKey = %JobBreakdownKey "
                               "AND JobKey = %JobKey "
                               "AND FundKey != 1 ",
                               sizeof szTmp,
                               ppszPNames,
                               ppszParams,
                               100);

  if (uRet)
      {
      ErrCalc ("Not all params available for CalcPctFundedInitFund", szTmp);
      return 1;
      }

   pszPctFnd1 = ItiDbGetRow1Col1(szTmp, hheap, 0,0,0);


   if (pszPctFnd1[0] == '\0')
      ItiStrCpy(szPercent, "1.00", 6);
   else
      sprintf(szPercent, "1.00 - %s", pszPctFnd1);


      
   uRet = ItiStrReplaceParams(szTmp1,
               "/*  CalcJobBreakFundAfterDel */ "
                             "UPDATE JobBreakdownFund "
                             "SET PercentFunded = %s "
                             "WHERE JobKey = %JobKey "
                             "AND JobBreakdownKey = %JobBreakdownKey " 
                             "AND FundKey = 1 "
                             "UPDATE JobBreakdownFund "
                             "SET AmountFunded = PercentFunded * "
                             "(Select (((1 - DetailedEstimate) * PreliminaryEstimateCost) "
                             "+ (convert(int,DetailedEstimate)  * DetailedEstimateCost)) "
                             "FROM JobBreakdown "
                             "WHERE JobBreakdown.JobKey = %JobKey "
                             "AND JobBreakdown.JobBreakdownKey = %JobBreakdownKey) "
                             "WHERE JobKey = %JobKey "
                             "AND JobBreakdownKey = %JobBreakdownKey "
                             "AND FundKey = 1 ",
                              sizeof szTmp1,
                              ppszPNames,
                              ppszParams,
                              100);


   sprintf(szTmp, szTmp1, szPercent);

// if (uRet)
//   {
//   ErrCalc ("Not all params available CalcJobBreakFundAfterDel szTmp, szTmp1);
//   return 1;
//   }


   ItiDbBeginTransaction(hheap);
   if (ItiDbExecSQLStatement(hheap, szTmp))
      {
      ErrCalc ("Bad Exec for CalcJobBreakFundAfterDel szTemp", szTmp);
      ItiDbRollbackTransaction(hheap);
      return 1;
      }

   ItiDbCommitTransaction (hheap);

   /* do buffer updates after all calcs */

   ItiDbUpdateBuffers ("JobBreakdownFund");
   return uRet;
   }






 int EXPORT CalcJobFundAfterDel (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)

   {
//   char     szTmp  [500];
   char     szTmp1 [500];
   char     szTmp2 [500];
   char     szTmp3 [500];
   char     szTmp4 [500];
   USHORT  uRet, usErr, uCols [] = {1,2,0};
   PSZ     pszFndUsed, pszPctFnd1, *ppszPct, pszContingPct, pszConstrEngrPct;
   PSZ     pszJobKey;

   /* determine whether the deleted JBF fund is used by another breakdown in this job */ 

   uRet = ItiStrReplaceParams(szTmp,
               "/*  CalcJobFundAfterDel */ "
                             "SELECT JobBreakdownKey "
                             "FROM JobBreakdownFund "
                             "WHERE JobKey = %JobKey "
                             "AND FundKey = %FundKey ",
                             sizeof szTmp,
                             ppszPNames,
                             ppszParams,
                             100);

//   if (uRet)
//      {
//      ErrCalc ("Not all params available for Calc", szTmp2);
//      return 1;
//      }

   pszFndUsed = ItiDbGetRow1Col1(szTmp, hheap, 0, 0, 0);




   if (pszFndUsed == NULL)

      /* no remaining breakdowns for this job using this fund */  
      {

       /* Get ContingencyPercent and ConstructionEngineeringPct from Job */

       pszJobKey = ItiDbGetZStr(ppszParams[0],0); 


       sprintf (szTmp4, " SELECT ContingencyPercent, ConstructionEngineeringPct "
                        " FROM Job "
                        " WHERE JobKey = %s", pszJobKey);

       usErr = ItiDbBuildSelectedKeyArray2(hheap, 0, szTmp4,
                                           &ppszPct, uCols);

       if (usErr != 0)
         return FALSE;


       pszContingPct = ItiDbGetZStr (ppszPct [0], 0);
       pszConstrEngrPct = ItiDbGetZStr (ppszPct [0], 1);

      uRet = ItiStrReplaceParams (szTmp1,
                                 "/*  CalcJobFundAfterDel */ "
                                 "DELETE FROM JobFund "
                                 "WHERE JobKey = %JobKey "
                                 "AND FundKey = %FundKey "
                                 "UPDATE JobFund "
                                 "SET AmountFunded =  "
                                 "(SELECT (SELECT SUM(AmountFunded) "
                                 "FROM JobBreakdownFund "
                                 "WHERE FundKey = 1 "
                                 "AND JobKey = %JobKey) * (1 + %s + %s)) "
                                 "WHERE JobKey = %JobKey "
                                 "AND FundKey = 1 ",
                                 sizeof szTmp1,
                                 ppszPNames,
                                 ppszParams,
                                 100);

      sprintf(szTmp3, szTmp1, pszContingPct, pszConstrEngrPct);
      ItiFreeStrArray(hheap, ppszPct, 2);

      ItiDbBeginTransaction (hheap);
      if (ItiDbExecSQLStatement (hheap, szTmp3))
         {
         ErrCalc ("Bad Exec for CalcJobFundAfterDel-AmtFunded", szTmp3);
         ItiDbRollbackTransaction (hheap);
         return 1;
         }
      ItiDbCommitTransaction (hheap);


      /* Determining Percent Funded for FundKey 1 - Must be calculated */
      /* after the Amount Funded fields have been updated              */


      uRet = ItiStrReplaceParams(szTmp1,
               "/*  CalcJobFundAfterDel */ "
                                "SELECT (select AmountFunded "
                                "FROM JobFund "
                                "WHERE JobKey = %JobKey "
                                "AND FundKey = 1)/ "
                                "(SELECT EstimateTotal "
                                "FROM Job "
                                "WHERE JobKey = %JobKey) ",
                                sizeof szTmp1,
                                ppszPNames,
                                ppszParams,
                                100);

      //   if (uRet)
      //      {
      //      ErrCalc ("Not all params available for Calc", szTmp1);
      //      return 1;
      //      }

      pszPctFnd1 = ItiDbGetRow1Col1(szTmp1, hheap, 0, 0, 0);

      uRet = ItiStrReplaceParams (szTmp2,
               "/*  CalcJobFundAfterDel */ "
                                 "UPDATE JobFund "
                                 "SET PercentFunded = %s "
                                 "WHERE JobKey = %JobKey "
                                 "AND FundKey = 1 ",
                                 sizeof szTmp2,
                                 ppszPNames,
                                 ppszParams,
                                 100);

      sprintf (szTmp, szTmp2, pszPctFnd1);

      ItiDbBeginTransaction (hheap);
      if (ItiDbExecSQLStatement (hheap, szTmp))
         {
         ErrCalc ("Bad Exec for CalcJobFundPctFunded", szTmp);
         ItiDbRollbackTransaction (hheap);
         return 1;
         }
      ItiDbCommitTransaction (hheap);

      /*--- do buffer updates after all calcs ---*/

      ItiDbUpdateBuffers ("JobFund");

      return uRet;

      }
   else
      {

      uRet = CalcJobFund(hheap, ppszPNames, ppszParams);
      }


      return uRet;
     }
















int EXPORT DelObsoleteProgFunds(HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)

   {
//   char      szTmp[300];
   USHORT    uRet;


   uRet = ItiStrReplaceParams(szTmp,
                              "DELETE FROM ProgramFund "
                              "WHERE ProgramKey = %ProgramKey "
                              "AND ProgramFund.FundKey NOT IN "
                              "(SELECT FundKey "
                              "FROM JobFund, ProgramJob "
                              "WHERE ProgramJob.ProgramKey = %ProgramKey "
                              "AND ProgramJob.JobKey = JobFund.JobKey) ",
                              sizeof szTmp,
                              ppszPNames,
                              ppszParams,
                              100);

   if (uRet)
      {
      ErrCalc ("Not all params available for DelObsoleteProgFunds", szTmp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);

   if (ItiDbExecSQLStatement (hheap, szTmp))
      {
      ErrCalc ("Bad Exec for DelObsoleteProgFunds", szTmp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   ItiDbUpdateBuffers ("ProgramFund");
   return 0;
   }



/* Calculates the Unit Price and ExtendedAmount
 * of the JobItem 
 * This function is called when a job item is added
 * to the job, and after all the associated CostBasedEstimates,
 * CostSheets, Crews, Materials, Equipment, Laborers, etc. have been copied
 *
 * Needs: JobKey
 *        JobItemKey
 *        
 *
 */

int EXPORT CalcJobItem (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
//   char   szTmp [1244];
   USHORT uRet;

   if (!ItiCesQueryJobCBEstActive (hheap, ppszPNames, ppszParams))
      {
      // return 0;
      uRet = ItiStrReplaceParams (szTmp, 
                 "/*  CalcJobItem */ "
                 "UPDATE JobItem"
                 " SET UnitPrice = 0,"
                 " ExtendedAmount = 0"
                 " WHERE JobKey = %JobKey"
                 " AND JobItemKey = %JobItemKey",
                 sizeof szTmp,
                 ppszPNames,
                 ppszParams,
                 100);
      }
   else
      {
      uRet = ItiStrReplaceParams (szTmp, 
                 " /*  CalcJobItem */ "
                 "UPDATE JobItem "
                 " SET UnitPrice = " 
                 " (SELECT SUM (UnitPrice * QuantityPerItemUnit) "
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
                 " WHERE JobKey= %JobKey"
                 " AND JobItemKey = %JobItemKey)"
                 " WHERE JobKey = %JobKey "
                 " AND JobItemKey = %JobItemKey ",
                 sizeof szTmp,
                 ppszPNames,
                 ppszParams,
                 100);
      }


   if (uRet)
      {
      ErrCalc ("Not all params available for CalcJobItem", szTmp);
      return 1;
      }

   ItiDbBeginTransaction (hheap);

   if (ItiDbExecSQLStatement (hheap, szTmp))
      {
      ErrCalc ("Bad Exec for CalcJobItem", szTmp);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   /*--- do buffer updates after all calcs ---*/
   ItiDbUpdateBuffers ("JobItem");
   ItiDbUpdateBuffers ("JobBreakdown");
   ItiDbUpdateBuffers ("Job");
   return 0;

  }






 static int DoesJobAltHaveMoreJobItems (HHEAP hheap, PSZ pszJobKey, PSZ pszJobAlternateGroupKey, PSZ pszJobAlternateKey)
   {
   char  szTemp [500];
   PSZ   pszJobItemsExist;

   sprintf(szTemp,
                   "/* DoesJobAltHaveMoreJobItems */ "
                   "SELECT JobItemKey "
                   "FROM JobItem, JobBreakdown "
                   "WHERE JobItem.JobKey = %s "
                   "AND JobAlternateGroupKey = %s "
                   "AND JobAlternateKey = %s "
                   "AND JobBreakdown.JobKey = %s "
                   "AND JobBreakdown.DetailedEstimate = 1", pszJobKey,
                    pszJobAlternateGroupKey, pszJobAlternateKey, pszJobKey);

   pszJobItemsExist = ItiDbGetRow1Col1(szTemp, hheap, 0, 0, 0);

   /* If there are more JobItems associated with this JobAlternate return  */
   /* 0, to indicate that the JobAlternate's Estimate field can be         */
   /* recalculated by summing up the ExtendedAmounts in the JobItem table. */
   /* If there are no JobItems associated with the JobAlternate, return 1, */
   /* so $.00 can be placed in the JobAlternate's Estimate field.          */

   if (pszJobItemsExist != NULL)
      return 0;
   else return 1;

   }



/* Calculates EstimateTotal
 *
 * Needs: JobKey
 *
 */

int EXPORT CalcJobEstimateTotal (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams)
   {
//   char   szTmp [300];
   CHAR szTmp2[300], szTmp3[300];
   char   szContPct[12] = "";
   char   szConstEngrPct[12] = "";
   USHORT usQtyLen;
   USHORT uRet, usErr, uCols [] = {1,2,0};
   PSZ    pszJobKey, pszConstrEngrPct, pszContingPct, *ppszPct;


   /* Get ContingencyPercent and ConstructionEngineeringPct from Job */

   pszJobKey = ItiDbGetZStr(ppszParams[0],0); 

   sprintf (szTmp2,
                    " /*  CalcJobEstimateTotal */ "
                    " SELECT ContingencyPercent, ConstructionEngineeringPct "
                    " FROM Job "
                    " WHERE JobKey = %s", pszJobKey);

   usErr = ItiDbBuildSelectedKeyArray2(hheap, 0, szTmp2, &ppszPct, uCols);
   if (usErr != 0)
      return FALSE;

   pszContingPct = ItiDbGetZStr (ppszPct [0], 0);
   pszConstrEngrPct = ItiDbGetZStr (ppszPct [0], 1);

   ItiFmtFormatString (pszContingPct, szContPct, sizeof szContPct,
                       "Number,....", &usQtyLen);

   pszContingPct = szContPct;

   ItiFmtFormatString (pszConstrEngrPct, szConstEngrPct, sizeof szConstEngrPct,
                       "Number,....", &usQtyLen);

   pszConstrEngrPct = szConstEngrPct;


   uRet = ItiStrReplaceParams (szTmp, 
                    "/*  CalcJobEstimateTotal */ "
                    "UPDATE Job "
                    "SET EstimateTotal = EstimatedCost + "
                    "EstimatedCost * %s + " 
                    "EstimatedCost * %s " 
                    "WHERE JobKey=%JobKey ",
                    sizeof szTmp,
                    ppszPNames,
                    ppszParams,
                    100);

   sprintf(szTmp3, szTmp, pszContingPct, pszConstrEngrPct);

   ItiFreeStrArray(hheap, ppszPct, 2);

   ItiDbBeginTransaction (hheap);
   if (ItiDbExecSQLStatement (hheap, szTmp3))
      {
      ErrCalc ("Bad Exec for CalcJobEstimateTotal", szTmp3);
      ItiDbRollbackTransaction (hheap);
      return 1;
      }
   ItiDbCommitTransaction (hheap);

   ItiDbUpdateBuffers ("Job");
   return 0;
   }


/* ---------------------------------------------------------------------- *
 * The AvailableEstMethod function determines the first available         *
 * estimation method from the following ordered list:                     *
 *   (currently:)  Regression, Average,      Default, AdHoc               *
 *   (eventually:) Regression, Average, CBEst, Default, AdHoc             *
 * The psz returned is the ItemEstimationMethod key value,                *
 * AdHoc is the default catch all value.                                  *
 * ---------------------------------------------------------------------- */
PSZ AvailableEstMethod (HHEAP hp, PSZ pszStdItemKey, PSZ pszBaseDate, PSZ pszQuantity)
   {
//   CHAR  szTempAEM [510];
   LONG  lMin= 0L;
   LONG  lQty= 0L;
   LONG  lMax= 0L;
   PSZ   pszTmp, pszResult = NULL;

   ItiStrToLONG (pszQuantity, &lQty);

////////////////// For Minnesota test for CBEst first.
if (bDoCBEFirst)
   {
   /* -- CBEst ------------------------------------------------ */
   ItiStrCpy (szTempAEM,
          "/* bamsces.calcfn.AvailableEstMethod */ "
          " SELECT CBE.CostBasedEstimateKey "
          " FROM CostBasedEstimate CBE, StandardItem SI "
          " WHERE CBE.Active = 1 AND "
            " CBE.CostBasedEstimateKey IN"
             " (SELECT CostBasedEstimateKey"
              " FROM Task T WHERE T.CostSheetKey IN"
                 " (select CostSheetKey from CostSheet"
                  " where Deleted = NULL"
                  " and (UnitType = NULL OR"
                       " UnitType = (select UnitType from StandardItem"
                                   " where StandardItemKey = ", sizeof szTempAEM);
   ItiStrCat (szTempAEM, pszStdItemKey, sizeof szTempAEM);
   ItiStrCat (szTempAEM,
          "))) AND CBE.StandardItemKey = SI.StandardItemKey"
          " AND (CBE.UnitType = SI.UnitType"
                " OR CBE.UnitType = NULL OR SI.UnitType = NULL))"
          " AND CBE.StandardItemKey = ", sizeof szTempAEM);
   ItiStrCat (szTempAEM, pszStdItemKey, sizeof szTempAEM);


   pszResult = ItiDbGetRow1Col1(szTempAEM, hp, 0, 0, 0);
   if (pszResult != NULL)
      {
      ItiMemFree(hp, pszResult);
      return szEstKeyCBEst;  
      }
   }/* end of if (bDoCBEFirst... */
//////////////////////////////////

   /* -- Regression ------------------------------------------ */
   ItiStrCpy (szTempAEM, 
               "/* bamsces.calcfn.AvailableEstMethod */ "
               "SELECT MinimumQuantity"
               " FROM PEMETHRegression"
               " WHERE StandardItemKey = ", sizeof szTempAEM);
   ItiStrCat (szTempAEM, pszStdItemKey, sizeof szTempAEM);
   ItiStrCat (szTempAEM, 
               " AND ImportDate = "
                 "(SELECT ImportDate "
                  " FROM StandardItemPEMETHRegression"
                  " WHERE StandardItemKey = ", sizeof szTempAEM);
   ItiStrCat (szTempAEM, pszStdItemKey, sizeof szTempAEM); 
   ItiStrCat (szTempAEM, " AND BaseDate = '", sizeof szTempAEM);
   ItiStrCat (szTempAEM, pszBaseDate, sizeof szTempAEM); 
   ItiStrCat (szTempAEM, "' )", sizeof szTempAEM);

   pszResult = ItiDbGetRow1Col1(szTempAEM, hp, 0, 0, 0);
   if (pszResult != NULL)
      {
      pszTmp = ItiExtract (pszResult, ",$");
      ItiStrToLONG (pszTmp, &lMin);
      ItiMemFree(hp, pszResult);

      ItiStrCpy (szTempAEM, 
                  "/* bamsces.calcfn.AvailableEstMethod */ "
                  "SELECT MaximumQuantity"
                  " FROM PEMETHRegression"
                  " WHERE StandardItemKey = ", sizeof szTempAEM);
      ItiStrCat (szTempAEM, pszStdItemKey, sizeof szTempAEM);
      ItiStrCat (szTempAEM, 
                 " AND ImportDate = "
                   "(SELECT ImportDate "
                    "FROM StandardItemPEMETHRegression "
                     " WHERE StandardItemKey = ", sizeof szTempAEM);
      ItiStrCat (szTempAEM, pszStdItemKey, sizeof szTempAEM); 
      ItiStrCat (szTempAEM, " AND BaseDate = '", sizeof szTempAEM);
      ItiStrCat (szTempAEM, pszBaseDate, sizeof szTempAEM); 
      ItiStrCat (szTempAEM, "' )", sizeof szTempAEM);

      pszResult = ItiDbGetRow1Col1(szTempAEM, hp, 0, 0, 0);
      if (pszResult != NULL)
         {
         pszTmp = ItiExtract (pszResult, ",$");
         ItiStrToLONG (pszTmp, &lMax);
         ItiMemFree(hp, pszResult);
         }

      /* now test if qty in range */ 
      if ((lMax >= lQty) && (lQty >= lMin))
         {
         return szEstKeyRegress;
         }
      }/* end of if (pszResult != NULL) Regression */


   /* -- Average ---------------------------------------------- */
   ItiStrCpy (szTempAEM,
        "/* bamsces.calcfn.AvailableEstMethod */ "
        "SELECT FifthPercentile"
        " FROM PEMETHAverage"
               " WHERE StandardItemKey = ", sizeof szTempAEM);
   ItiStrCat (szTempAEM, pszStdItemKey, sizeof szTempAEM);
   ItiStrCat (szTempAEM, 
        " AND ImportDate = "
          "(SELECT ImportDate "
           "FROM StandardItemPEMETHAverage "
                  " WHERE StandardItemKey = ", sizeof szTempAEM);
   ItiStrCat (szTempAEM, pszStdItemKey, sizeof szTempAEM); 
   ItiStrCat (szTempAEM, " AND BaseDate = '", sizeof szTempAEM);
   ItiStrCat (szTempAEM, pszBaseDate, sizeof szTempAEM); 
   ItiStrCat (szTempAEM, "' )", sizeof szTempAEM);

   pszResult = ItiDbGetRow1Col1(szTempAEM, hp, 0, 0, 0);
   if (pszResult != NULL)
      {
      pszTmp = ItiExtract (pszResult, ",$");
      ItiStrToLONG (pszTmp, &lMin);
      ItiMemFree(hp, pszResult);

      ItiStrCpy (szTempAEM,
                  "/* bamsces.calcfn.AvailableEstMethod */ "
                  "SELECT NinetyFifthPercentile"
                  " FROM PEMETHAverage"
                  " WHERE StandardItemKey = ", sizeof szTempAEM);
      ItiStrCat (szTempAEM, pszStdItemKey, sizeof szTempAEM);
      ItiStrCat (szTempAEM, 
                 " AND ImportDate = "
                   "(SELECT ImportDate "
                    "FROM StandardItemPEMETHAverage "
                    "WHERE StandardItemKey = ", sizeof szTempAEM);
      ItiStrCat (szTempAEM, pszStdItemKey, sizeof szTempAEM); 
      ItiStrCat (szTempAEM, " AND BaseDate = '", sizeof szTempAEM);
      ItiStrCat (szTempAEM, pszBaseDate, sizeof szTempAEM); 
      ItiStrCat (szTempAEM, "' )", sizeof szTempAEM);

      pszResult = ItiDbGetRow1Col1(szTempAEM, hp, 0, 0, 0);
      if (pszResult != NULL)
         {
         pszTmp = ItiExtract (pszResult, ",$");
         ItiStrToLONG (pszTmp, &lMax);
         ItiMemFree(hp, pszResult);
         }

      /* now test if qty in range */ 
      if ((lMax > lQty) && (lQty > lMin))
         {
         return szEstKeyAverage;
         }
      }

   /* -- CBEst ------------------------------------------------ */
   ItiStrCpy (szTempAEM,
          "/* bamsces.calcfn.AvailableEstMethod */ "
          " SELECT CBE.CostBasedEstimateKey "
          " FROM CostBasedEstimate CBE, StandardItem SI "
          " WHERE CBE.Active = 1 AND "
            " CBE.CostBasedEstimateKey IN"
             " (SELECT CostBasedEstimateKey"
              " FROM Task T WHERE T.CostSheetKey IN"
                 " (select CostSheetKey from CostSheet"
                  " where Deleted = NULL"
                  " and (UnitType = NULL OR"
                       " UnitType = (select UnitType from StandardItem"
                                   " where StandardItemKey = ", sizeof szTempAEM);
   ItiStrCat (szTempAEM, pszStdItemKey, sizeof szTempAEM);
   ItiStrCat (szTempAEM,
          "))) AND CBE.StandardItemKey = SI.StandardItemKey"
          " AND (CBE.UnitType = SI.UnitType"
                " OR CBE.UnitType = NULL OR SI.UnitType = NULL))"
          " AND CBE.StandardItemKey = ", sizeof szTempAEM);
   ItiStrCat (szTempAEM, pszStdItemKey, sizeof szTempAEM);


   pszResult = ItiDbGetRow1Col1(szTempAEM, hp, 0, 0, 0);
   if (pszResult != NULL)
      {
      ItiMemFree(hp, pszResult);
      return szEstKeyCBEst;  
//      return szEstKeyAdHoc; 
      }



   /* -- Default ---------------------------------------------- */
   ItiStrCpy (szTempAEM,
           "/* bamsces.calcfn.AvailableEstMethod */ "
           "SELECT UnitPrice"
           " FROM StdItemDefaultPrices"
           " WHERE Deleted = NULL "
           " AND StandardItemKey = ", sizeof szTempAEM);
   ItiStrCat (szTempAEM, pszStdItemKey, sizeof szTempAEM); 

   pszResult = ItiDbGetRow1Col1(szTempAEM, hp, 0, 0, 0);
   if (pszResult != NULL)
      {
      ItiMemFree(hp, pszResult);
      return szEstKeyDefault;
      }

   /* -- Adhoc ------------------------------------------------ */
   return szEstKeyAdHoc;

   }/* End of Function AvailableEstMethod */




/* -- CalcAutoSelectEstimateMethods
 * --   For the given job and job breakdown keys,
 * --   each job item will have its estimation method reset.
 * --
 * -- Returns:
 * --           0   No problems.
 * --          13   ItiDbExecQuery error.
 * --           
 * -- Parameters, ppszParam keys needed:
 * --               JobKey
 * --               JobBreakdownKey
 */
int EXPORT CalcAutoSelectEstimateMethods (HWND hwnd, PSZ *ppszPNames, PSZ *ppszParams)
   {
   HHEAP hhp, hhp2;
   HQRY  hqry;
   INT   iRet = 0;
   BOOL  bShowStack = TRUE;
   USHORT uCols, uErr, uRet, usCnt, usExistingCount;
   PSZ  *ppszTmp, psz, pszTmp, pszCnt, pszEM, pszBaseDate = NULL;
//   CHAR szTmpCASEM[256]   = "";
   CHAR szJobKey[9]       = "";
   CHAR szJobBrkdwnKey[9] = "";
   CHAR szEstMethodKey[9] = "";
   CHAR szBaseDate[36]    = "";
//   CHAR aszJIKeys  [NUMBER_OF_JI_KEYS + 1][3][SZLEN];
   // CHAR aszOldKeys [NUMBER_OF_OLD_KEYS + 1][4][SZLEN];
   PSZ    *ppszOldKeys = NULL;
   USHORT ausCols[5]    = {1, 2, 3, 4, 0};
//   CHAR   szTempCASEM[256]  = "";
   CHAR   szMsg[80] = "";


   ItiErrWriteDebugMessage ("   ");
   ItiErrWriteDebugMessage
      ("====== ENTERING bamsces.calcfn.CalcAutoSelectEstimateMethods ======");

   hhp = ItiMemCreateHeap (16000);  /* 16384 == 16k */

   uRet = ItiStrGet1Param (szJobKey,
             "JobKey", sizeof szJobKey, ppszPNames, ppszParams);
   uRet += ItiStrGet1Param (szJobBrkdwnKey,
             "JobBreakdownKey", sizeof szJobBrkdwnKey, ppszPNames, ppszParams);

   /* -- Get the Job's BaseDate. */
   sprintf (szTmpCASEM,
             "/* bamsces.Calcfn.CalcAutoSelectEstimateMethods */ "
             "SELECT BaseDate "
             " FROM Job "
             " WHERE JobKey = %s "
             , szJobKey);

   pszBaseDate = ItiDbGetRow1Col1 (szTmpCASEM, hhp, 0, 0, 0);
   if (NULL == pszBaseDate)
      {
		ItiErrWriteDebugMessage
			("bamsces.calcfn.CalcAutoSelectEstimateMethods, "
          "failed to get Job's basedate.");
      ItiMemDestroyHeap (hhp);
      return 13;
      }
   ItiStrCpy(szBaseDate, pszBaseDate, sizeof szBaseDate);
   ItiMemFree(hhp, pszBaseDate);
   
#define JOBKEY   0
#define JICBEKEY 1
#define ACTIVE   3

#define SIKEY   0
#define QTY     1
#define JIKEY   2

   /* -- Build JobItem Key list for the job breakdown. */
   sprintf (szTmpCASEM,
      "/* bamsces.Calcfn.CalcAutoSelectEstimateMethods, build key list. */ "
      "SELECT StandardItemKey, Quantity, JobItemKey "
      " FROM JobItem "
      " WHERE JobKey = %s "
      " AND JobBreakdownKey = %s "
      ,szJobKey, szJobBrkdwnKey);

   if (!(hqry = ItiDbExecQuery(szTmpCASEM, hhp, 0, 0, 0, &uCols, &uErr)))
      {
		ItiErrWriteDebugMessage
			("bamsces.calcfn.CalcAutoSelectEstimateMethods, failed ExecQuery.");
      ItiMemDestroyHeap (hhp);
      return 13;
      }

   aszJIKeys[0][0][0] = '\0';
   usCnt = 0;
   while (ItiDbGetQueryRow(hqry, &ppszTmp, &uErr)) 
      {
      if (usCnt <= NUMBER_OF_JI_KEYS)
         {
         ItiStrCpy(aszJIKeys[usCnt][SIKEY], ppszTmp[SIKEY], SZLEN);  // StandardItemKey
         ItiStrCpy(aszJIKeys[usCnt][JIKEY], ppszTmp[JIKEY], SZLEN);  // JobItemKey

         pszTmp = ItiExtract (ppszTmp[QTY], ",");
         ItiStrCpy(aszJIKeys[usCnt][QTY], pszTmp, SZLEN);  // Quantity

         aszJIKeys[usCnt+1][0][0] = '\0';
         }

      ItiFreeStrArray(hhp, ppszTmp, uCols); // 94sep13
      usCnt++;
      }

   if (usCnt > NUMBER_OF_JI_KEYS)
      {
      DosBeep (1400, 10); 
      DosBeep (1500, 10); 
      WinMessageBox (pglobals->hwndDesktop, hwnd,
         "Sorry, only the first 400 Job Items will"
         " have their estimation methods reset,"
         " the remainder will need be set by you.",
         "Internal CES Limit Exceeded",
         0, MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
		ItiErrWriteDebugMessage
			("Internal CES limit exceeded in bamsces.calcfn.CalcAutoSelectEstimateMethods.");
      }


   usCnt = 0;
   while (aszJIKeys[usCnt][0][0] != '\0')
      {
		ItiErrWriteDebugMessage("  ");
      sprintf(szMsg, "START of loop for JobItemKey: %s ", aszJIKeys[usCnt][JIKEY]);
		ItiErrWriteDebugMessage(szMsg);

      hhp2 = ItiMemCreateHeap (MAX_HEAP_SIZE);

//  if (bShowStack)
//     ShowStackInfo (hwnd);

      /* -- Next select ItemEstimateID for the current job item... */
      pszEM = AvailableEstMethod
           (hhp2, aszJIKeys[usCnt][SIKEY], szBaseDate, aszJIKeys[usCnt][QTY]);
      ItiStrCpy(szEstMethodKey, pszEM, sizeof szEstMethodKey);

      switch (szEstMethodKey[6])
         {
         case '0': psz = szEstIDAdHoc;
                   break;
         case '1': psz = szEstIDCBEst;
                   break;
         case '2': psz = szEstIDRegress;
                   break;
         case '3': psz = szEstIDAverage;
                   break;
         case '4': psz = szEstIDDefault;
                   break;
         default:  psz = "ERR, No method key found";
         }
      sprintf(szMsg, "Available Est method found: %s ", psz);
      ItiErrWriteDebugMessage (szMsg);

      /* -- If a CostBased estimate is available we need to... */
      if (szEstMethodKey[6] == '1')
         {
         /* ...first, we check if any JobCBEs already exist... */
         ItiStrCpy (szTempCASEM,
                    "/* bamsces.Calcfn.CalcAutoSelectEstimateMethods,"
                    " CBE section. */ "
                    "SELECT COUNT(*) "
                    " FROM JobCostBasedEstimate "
                    " WHERE JobKey = ", sizeof szTempCASEM);
         ItiStrCat (szTempCASEM, szJobKey, sizeof szTempCASEM);
         ItiStrCat (szTempCASEM, " AND JobItemKey = ", sizeof szTempCASEM);
         ItiStrCat (szTempCASEM, aszJIKeys[usCnt][JIKEY], sizeof szTempCASEM);
         pszCnt = ItiDbGetRow1Col1 (szTempCASEM, hhp2, 0, 0, 0);
         if (NULL != pszCnt)
            {
            ItiStrToUSHORT (pszCnt, &usExistingCount);
            ItiMemFree(hhp2, pszCnt);
            if (usExistingCount > 0)
               {
               // /* ...if so, setup JobItem by deleting existing CBEs... */
               // DelCBEForJobItem
               //    (hhp2, szJobKey, aszJIKeys[usCnt][JIKEY]);

               // /* No, rather lets just set existing JCBEs to inactive. */
               ItiStrCpy (szTempCASEM,
                       " UPDATE JobCostBasedEstimate SET Active = 0 "
                       " WHERE JobKey = ", sizeof szTempCASEM);
               ItiStrCat (szTempCASEM, szJobKey, sizeof szTempCASEM);
               ItiStrCat (szTempCASEM, " AND JobItemKey = ", sizeof szTempCASEM);
               ItiStrCat (szTempCASEM, aszJIKeys[usCnt][JIKEY], sizeof szTempCASEM);
               ItiDbExecSQLStatement (hhp2, szTempCASEM);
               }/* end of if usExistingCount.. */

            /* ...then copy the current catalog CBEs for the */
            /*    standard item to the JobItem. */
            CopyCBEForJobItem (hhp2, 
                               szJobKey,
                               aszJIKeys[usCnt][JIKEY], 
                               aszJIKeys[usCnt][SIKEY]);
            }
         else    /* for some reason we got a null so reset */
            {    /* the method key to ad-hoc.              */
            szEstMethodKey[6] = '0';
            ItiErrWriteDebugMessage
               ("Note: null count returned, what does this mean here?");
            }/* end of if NULL.. */
         }/* end if (szEstMethodKey[6].. */

      /* -- ...then call the ItiEstEstimateItem function to apply  */
      /* -- the specified item estimation method, pszEstimationID. */
      iRet = RecalcJobItem (hhp2, szJobKey, szJobBrkdwnKey,
                            aszJIKeys[usCnt][JIKEY],
                            aszJIKeys[usCnt][QTY],
                            aszJIKeys[usCnt][SIKEY],
                            szEstMethodKey );

      /* -- Now update the JobItem's ItemEstimationMethodKey. */
      if (iRet == 0)
         {
         sprintf (szTmpCASEM,
             "/* bamsces.Calcfn.CalcAutoSelectEstimateMethods */ "
             "UPDATE JobItem "
             " SET ItemEstimationMethodKey = %s "
             " WHERE JobKey = %s "
             " AND JobItemKey = %s "
             ,szEstMethodKey, szJobKey, aszJIKeys[usCnt][JIKEY]);

         if (ItiDbExecSQLStatement (hhp2, szTmpCASEM))
            {
            ErrCalc ("Bad Exec in bamsces.Calcfn.CalcAutoSelectEstimateMethods ", szTmpCASEM);
            }
         }

      if (hhp2 != NULL)
         ItiMemDestroyHeap (hhp2);

		ItiErrWriteDebugMessage("END loop for JIK. ");

      usCnt++;
      }/* end of while(aszJIKeys[usCnt]... */

   //Now do the recalc for the breakdown and job.
   if ((szJobKey[0] != '\0') && (szJobBrkdwnKey[0] != '\0'))
      {
      CalcJobItemChgJobBrkdwn (hhp, ppszPNames, ppszParams);
      CalcJobItemChgJob       (hhp, ppszPNames, ppszParams);
      ItiDbUpdateBuffers ("Job");
      ItiDbUpdateBuffers ("JobBreakdown");
      }

//   if (hhp != NULL)
//      ItiMemDestroyHeap (hhp);

   ItiErrWriteDebugMessage
      ("====== EXITING bamsces.calcfn.CalcAutoSelectEstimateMethods ======");
   ItiErrWriteDebugMessage ("  ");

   DosBeep (1200, 15);  // Let the user know we are done.
   return 0;
   }/* End of Function CalcAutoSelectEstimateMethods */

