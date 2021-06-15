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
 *  CalcFn.h
 *
 *  This module contains all the functions that perform calculations
 *  Some Calculation functions were moved to cestuil.h
 */




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

int EXPORT CalcPctFundedInitFund (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);


/*
 * Calculates the AmountFunded for the JobBreakdownFund table
 *
 * Needs: JobKey, JobBreakdownKey, FundKey
 */

int EXPORT CalcJobBreakAmtFunded (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);




/*
 * Calculates the AmountFunded for the JobFund table
 *
 * Needs: JobKey, FundKey
 */

int EXPORT CalcJobFund (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);



/*
 * Calculates the AmountFunded for the JobFund table
 *
 * Needs: FundKey, ProgramKey
 */

int EXPORT CalcProgramFund (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);




/* Recalculates the JobBreakdownFund after a Deletion
 *
 * Needs:  JobKey, JobBreakdownKey
 *
 */

int EXPORT CalcJobBreakFundAfterDel (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);





/* Recalculates the JobFund table after a JobBreakdownFund has been deleted
 *
 * Needs:  JobKey, JobBreakdownKey
 *
 */

int EXPORT CalcJobFundAfterDel (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);




/* This function deletes program funds that are no longer used to fund a program.
 * This could occur as a result of a  program job being deleted.
 * 
 * Needs:  ProgramKey
 *
 */

int EXPORT DelObsoleteProgFunds (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);




/* Calculates the UnitPrice and ExtendedAmount in the JobItem table.
 * This function is called when a job item is added
 * to the job, and after all the associated CostBasedEstimates,
 * CostSheets, Crews, Materials, Equipment, Laborers, etc. have been copied
 *
 * Needs: JobKey, JobItemKey
 *
 */

int EXPORT CalcJobItem (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);





/*
 * This function sets the Estimate field in the JobAlternate table to .00.
 * This function is called when there are no remaining job items associated
 * with this Job, JobAlternate and JobAlternateGroup.
 *
 * Needs:  JobKey, JobAlternateGroupKey, JobAlternateKey
 *
 */ 

int EXPORT SetJobAltEstToZero (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);






/*
 * This function calculates the EstimateTotal in the Job table.
 *
 * Needs:  hheap, JobKey
 *
 */ 

int EXPORT CalcJobEstimateTotal (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);





 /* This function is passed a pointer to an array containing all of
  * the job items the user selected to delete, their corresponding
  * JobAlternate and JobAlternateGroup keys.  For each job item being
  * deleted, the function checks to see if here are other remaining
  * jobitems associated with the same Job, JobAlternate, JobAlternateGroup.
  * If there are no other jobitems, the Estimate field in JobAlternate
  * is set to .00.  If there are other items, the regular calc function
  * is called.
  *
  * Needs:  JobAlternateGroupKey, JobAlternateKey, JobKey    
  *
  */

int EXPORT ReCalcJobAltEstForDelete(HWND hwnd, PSZ *ppszSelKeys);





void DoJobBreakdownFundReCalc (HWND hwnd, HWND hwndList);

void DoJobBreakFundInitialPctReCalc(HWND hwnd);

void DoDelFundReCalc(HWND hwnd, HHEAP hheapKeys, PSZ *ppszSelKeys);

void DoProgramJobReCalc(HWND hwnd, HWND hwndList, PSZ pszJobKey, PSZ pszProgramKey);

void DoDelProgJobReCalc(HWND hwnd);

void DoRecalcJobTotal(HWND hwnd);

void DoJobItemCalc(HHEAP hheap, PSZ pszJobKey, PSZ pszJobItemKey);

void DoJobReCalcForJIDelete(HWND hwnd, PSZ *ppszSelKeys);


#define NUMBER_OF_OLD_KEYS  32

int EXPORT CalcAutoSelectEstimateMethods (HWND hwnd, PSZ *ppszPNames, PSZ *ppszParams);

