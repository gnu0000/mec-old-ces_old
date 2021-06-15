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
 *  include\cesutil.h
 *
 *  This module contains all the functions that perform calculations
 *  These functions were initially in Bamsces\Calcfn.h
 */

                     /* Key values from ItemEstimationMethod table */
#define ADHOC            1000000
#define CBEST            1000001
#define PEMETHRegression 1000002
#define PEMETHAverage    1000003
#define DEFAULTMTH       1000004

             /* Maximum number of jobitem keys; used to build an array. */
// #define NUMBER_OF_JI_KEYS   100
//#define NUMBER_OF_JI_KEYS   400
#define NUMBER_OF_JI_KEYS   360

/* Calculates JobBreakdown Detailed Estimate Cost
 *
 * Needs: JobBreakdownKey
 *        JobKey
 *
 */
int EXPORT CalcDetailedEstimateCost (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);





/*
 * Calculates Job Alternate Group Estimate.
 *
 * Needs: JobKey
 */
int EXPORT CalcAlternateGroupEstimate (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);



/* Calculates the EstimateTotalAdjForInflation field, when a Program Job is added
 * to a program.
 *
 * Needs:  JobKey
 *         Subsequent functions that are triggered need ProgramKey
 *           so it is retrieved in this function
 *
 */

int EXPORT CalcEstTotlProgJob (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);


/* Recalculates the TotalCost and TotalAdjustedForInflation fields in the Program
 * table when a new program job is added
 *
 * Needs:  ProgramKey
 *
 */

int EXPORT ReCalcProgram (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);





/* Recalculates the AmountFunded and PercentFunded fields in the ProgramFund
 * table when a new program job is added
 * 
 * Needs:  ProgramKey
 *
 */

int EXPORT CalcAllProgramFunds (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);



/* Recalculates the TotalCost and EstimateTotalAdjForInflation fields
 * in the Program table.
 * 
 * It currently does not take base date or inflation rates into consideration
 * This must be added.
 *
 * Needs:  ProgramKey
 *
 */

int EXPORT ReCalcProgram(HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);




/* Recalculates all of the AmountFunded fields of the JobFund table
 * for a particular Job.
 *
 * Needs:  JobKey        DO WE NEED THIS
 *
 */
int EXPORT CalcAllJobFundsForJob(HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);



/* Recalculates all of the AmountFunded fields of the JobBreakdownFund table
 * for a particular Job.
 * 
 * Needs:  JobKey
 *
 */
int EXPORT CalcAllJobBreakFundsForJob(HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);



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
/* ------------------------------------------------------------- */
int EXPORT RecalcCBEST(HHEAP hheap,
                       PSZ   pszJobKey,
                       PSZ   pszJobBreakdownKey,
                       PSZ   pszJobItemKey,
                       PSZ   pszQuantity,
                       PSZ   pszStandardItemKey);



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
/* --    Sets the JobItem unit price with the price from the     */
/* --    recalculated PEMETH Average estimate.                   */
/* --                                                            */
/* -- RETURN: void                                               */
/* ------------------------------------------------------------- */
void EXPORT RecalcPEMETHAverage(HHEAP hheap, 
                                PSZ pszJobKey, 
                                PSZ pszJobBreakdownKey, 
                                PSZ pszJobItemKey,
                                PSZ pszQuantity,
                                PSZ pszStandardItemKey);





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
/* --    Sets the JobItem unit price with the price from the     */
/* --    recalculated PEMETH Regression estimate.                */
/* --                                                            */
/* -- RETURN: void                                               */
/* ------------------------------------------------------------- */
void EXPORT RecalcPEMETHRegression(HHEAP hheap, 
                                   PSZ pszJobKey, 
                                   PSZ pszJobBreakdownKey, 
                                   PSZ pszJobItemKey,
                                   PSZ pszQuantity,
                                   PSZ pszStandardItemKey);




/* Lookup and set JobItem unit price . */
/* needs JobKey, JobItemKey, StandardItemKey */

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
/* --    default price catalog, if available.                    */
/* --                                                            */
/* -- RETURN: 0 if okay                                          */
/* ------------------------------------------------------------- */
int EXPORT RecalcDefaultPrice (HHEAP hheap,
                                PSZ   pszJobKey,
                                PSZ   pszJobBreakdownKey,
                                PSZ   pszJobItemKey,
                                PSZ   pszQuantity,
                                PSZ   pszStandardItemKey);





/* 
 * Calls the appropriate recalc function for the given job item, 
 * i.e. PEMETHAvg, PEMETHRegression, CostBasedEstimation
 *
 * Needs: hheap
 *        JobKey
 *        JobBreakdownKey
 *
 */
int EXPORT RecalcJobItem
      (HHEAP hheap, PSZ pszJobKey, PSZ pszJobBreakdownKey, PSZ pszJobItemKey,
       PSZ pszItemEstMethodKey,    PSZ pszQuantity,        PSZ pszStdItemKey);

 

/* 
 * Retrieves the JobItems associated with a JobBreakdown and 
 * calls the appropriate recalc function for each job item, 
 * i.e. PEMETHAvg, PEMETHRegression, CostBasedEstimation
 *
 * Needs: hheap
 *        JobKey
 *        JobBreakdownKey
 *
 */

int EXPORT RecalcAllJobBrkdwnItems(HHEAP hheap, 
                                   PSZ pszJobKey, 
                                   PSZ pszJobBreakdownKey);
 

/* 
 * Calculates the UnitPrice and the ExtendedAmount fields for a JobItem.
 * This function is called when CBE is used.
 *
 * Needs: hheap
 *        JobKey
 *        JobItemKey
 *
 */

int  EXPORT CalcJobItemUnitPrice (HHEAP hheap, 
                                  PSZ *ppszPNames, 
                                  PSZ *ppszParams);


/* 
 * Updates the UnitPrice and calculates the ExtendedAmount fields for a JobItem.
 * This function is called after the JobItems's unit price is calculated,
 * using PEMETHRegression or PEMETHAverage.
 *
 * Needs: hheap
 *        JobKey
 *        JobItemKey
 *
 */

int  EXPORT CalcJIUnitPricePEMETH (HHEAP hheap, 
                                   PSZ *ppszPNames, 
                                   PSZ *ppszParams);





/* 
 * This function determines if there are any other job items that are
 * associated with a particular JobBreakdown
 *
 * Needs: hheap
 *        JobKey
 *        JobBreakdownKey
 */

int EXPORT DoMoreJobItemsExist(HHEAP hheap,
                               PSZ   pszJobKey,
                               PSZ   pszJobBreakdownKey);


/* 
 * This function obtains all of the JobBreakdowns for a particular Job
 * and calls a function to recalculate the JobBreakdown
 *
 * Needs: hwnd
 *        JobKey
 *
 */

void EXPORT NewRecalcJob (PSZ pszJobKey);




/*	This function recalcs all of a job's cost sheet
 * materials, crew labor, and crew equipment.
 *
 *  Needs: JobKey
 */
void EXPORT DoJobCostSheetReCalc (PSZ pszJobKey);


/*
 * This function sets the EstimateCost field in JobBreakdown table to .00.
 * This function is called when there are no remaining job items associated
 * with this JobBreakdown.
 *
 * Needs:  JobKey, JobBreakdownKey
 *
 */ 

int EXPORT SetDetailedEstimateCostToZero (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);

/*
 * This function calculates the Estimate field in the JobAlternateGroup table.
 *
 * Needs:  hheap, JobKey
 *
 */ 

int EXPORT CalcEstimateJobAltGroup (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);

/*
 * This function is called by the NewRecalcJob function.
 * It determines if a job is in a program and calls
 * a function to calculate the JobFunds for a job.
 *
 *
 * Needs:  hheap, JobKey
 *
 */ 

int EXPORT RecalcJFForJob(HHEAP hheap, PSZ pszJobKey);



void EXPORT DoJobRecalc (HWND hwnd);

void EXPORT DoReCalcJobBBreakJobFunds (HWND hwnd);

void EXPORT DoReCalcJobBreakJobFunds (HWND hwnd);



extern BOOL EXPORT ItiCesIsJobWindow (HWND hwnd, int usID);
extern BOOL EXPORT ItiCesIsProgramWindow (HWND hwnd, int usID);
extern BOOL EXPORT ItiCesIsCatalogWindow (HWND hwnd, int usID);


extern void EXPORT DoJobEstimateRecalc (HWND hwnd);

extern int EXPORT CalcJobItemActiveCostSht (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);
extern int EXPORT CalcJobItemChg (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);
extern int EXPORT CalcJobItemChgJobBrkdwn (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);
extern int EXPORT CalcJobItemChgJob (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);


