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
 * ..\cbest\CBEst.h
 * Mark Hampton
 *
 * This module provides the Cost Based Estimation Module.
 */



USHORT usRoundToDecimalPlace;



extern MRESULT EXPORT JobCostShtStaticProc (HWND   hwnd,
                                            USHORT usMessage,
                                            MPARAM mp1,
                                            MPARAM mp2);





extern MRESULT EXPORT EditJobCostSheetProc (HWND    hwnd,
                                            USHORT  uMsg,
                                            MPARAM  mp1,
                                            MPARAM  mp2);



/*
 * This function builds an array that contains all of the 
 * CostBasedEstimateKeys associated with a job item, and
 * an array that contains all of the JobCostSheets associated
 * with each of the CostBasedEstimates.  These stored keys are
 * used to delete all of the JobCostBasedEstimates, JobCostSheets,
 * Materials, Crews, etc. when a job item has been deleted.
 *  
 * Needs:  ppszSelKeys - an array containing the JobItemKeys
 *                       that the user has selected to be deleted
 *         
 */

extern BOOL EXPORT DoDelCostBasedEstForJobItems(HHEAP hheapKeys, 
                                                PSZ *ppszSelKeys);



/* 
 * This function loops through a stored array of JobCostBasedEstimates that have         
 * been selected by the user to be deleted. All of the JobCostSheets associated with a                                    
 * particular CBE are then retrieved and stored in another array.  This array is then    
 * passed to another function which deletes all of the Crews, Laborers, Equipment        
 * and Materials associated with these JobCostSheets                                     
 */                    

// static BOOL EXPORT DoDelJobCostSheetsForCBE (HHEAP hheapKeys, PSZ *ppszSelKeys, USHORT usCBEst);
extern BOOL EXPORT DoDelJobCostSheetsForCBE (HHEAP hheapKeys, PSZ *ppszSelKeys, USHORT usCBEst);






/*
 * This function deletes all of the JobCostSheets that are associated
 * with a particular job item.
 *  
 * This function is one of the functions called when
 * a job item has been deleted.
 *
 * Needs:  ppszCBEKeys - an array containing the JobKey, the JobCostSheetKey,
 *                       the JobItemCostBasedEstimateKey for a
 *                       particular job item.
 *         
 */

static BOOL DelJobCostShts (HHEAP hheap, 
                            PSZ *ppszCBEKeys);



/*
 * This function deletes all of the Labor and Equipment Crews as well
 * as the Materials used for a particular jobitem.  
 *  
 * This function is one of the functions called when
 * a job item has been deleted.
 *
 * Needs:  ppszJCShtKeys - an array containing the JobKey, the JobCostSheetKey,
 *                         the JobItemCostBasedEstimateKey for a
 *                         particular job item.
 *         
 */

BOOL EXPORT DelCrewsAndMaterialsForJobItem (HHEAP hheap,
                                            PSZ *ppszJCShtKeys);




/* Calculates the JobItem UnitPrice and ExtendedAmount when CBE is used
 * 
 * This function sums the UnitPrices of all of the JobCostSheets
 * (that are associated with the Job and JobItem), and are associated
 * with the active JobCostBasedEstimate.
 *
 * Needs: JobKey, JobItemKey
 *
 */

extern int EXPORT CalcJobItem (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);




/* 
 * This function retrieves the EstimationMethod used in
 * in determining a JobItem's unit price.
 *
 * Needs: hwnd to obtain JobKey and JobCostSheetKey
 *
 */

int EXPORT DetermineEstMethod(HWND hwnd);




/* 
 * This function determines the active JobCostBasedEstimation 
 *
 * Needs: hwnd 
 *
 */

 extern int EXPORT DetermineIfCBEActive(HWND hwnd);



/* 
 * This function obtains the JobKey and JobItemKey
 * and calls a function do recalculate the JobItem UnitPrice and
 * ExtendedAmount when CBEST is used.
 *
 * Needs: hwnd 
 *
 * This function obtains the JobKey and JobCostSheetKey from hwnd,
 * and executes a query to obtain the needed JobItemKey.
 * It then calls a function to recalculate CBEST.
 *
 */

 int EXPORT GetJobItemKeyAndRecalc(HWND hwnd);


/* 
 * This function obtains the JobKey and JobItemKey from hwnd,
 * and calls a function do recalculate the JobItem UnitPrice and
 * ExtendedAmount when CBEST is used.
 *
 * Needs: hwnd 
 *
 */

 int EXPORT GetKeysAndRecalc(HWND hwnd);




/* 
 * This function is called when a JobCrew has been deleted.
 *
 * This function obtains the JobKey and JobCostSheetKey from the hwnd.
 * It executes two queries to retrieve the JobItemKey and JobBreakdownKey,
 * sinces it is not available from the dialog box or JobCrew window.
 *
 * It then calls a calc function to recalculate the Job and JobBreakdown
 *
 * Needs: hwnd 
 *
 */

 int EXPORT GetJobBreakKeyAndRecalc(HWND hwnd);





/* 
 * This function retrieves the EstimationMethod used in
 * in determining a JobItem's unit price.
 *
 * Needs: hwnd to obtain JobKey and JobItemKey
 *        
 */

 extern int EXPORT DetermineJobItemEstMethod(HWND hwnd);




/* 
 * This function deletes all of the JobCostSheets associated with
 * a particular JobCostBasedEstimate
 *        
 * Needs: An array containing all of the JobCostSheetKeys to be deleted 
 *
 */

 BOOL EXPORT DelJobCostSheetsForCBE(HHEAP hheap, PSZ *ppszJCShtKeys);





/* 
 * This function obtains the currently active JobCostBasedEstimate,
 * if one exists, and changes the Active field to 0, indicating it
 * is no longer active.
 *
 * Needs: hwnd
 *
 */

 int EXPORT DeactivatePreviouslyActiveCBE (HWND hwnd);



/* 
 * This function determines if the currently active JobCostBasedEstimate
 * has any JobCostSheets associated with it.
 * 
 * It returns 0 if there are JobCostSheets, and 1  if none exist
 * 
 * Needs: hheap, pszJobKey, pszJobItemKey
 *        
 */

 int EXPORT DetermineIfJCShtsExist (HHEAP hheap, PSZ pszJobKey, PSZ pszJobItemKey);






/* 
 * This function determines the active JobCostBasedEstimation 
 *
 * Needs: hheap, pszJobKey, pszJobCostSheetKey
 *
 */

int EXPORT IsCBEActive(HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey);




/* 
 * This function determines the if a particular JobCostBasedEstimation is active 
 *
 * Needs: hheap, pszJobKey, pszJobItemKey, pszJobItemCostBasedEstimateKey
 *
 */

 int EXPORT IsCBEForJobItemActive(HHEAP hheap, PSZ pszJobKey, PSZ pszJobItemKey, PSZ pszJobItemCostBasedEstimateKey);




/* 
 * This function determines the if a particular JobCostBasedEstimation is active 
 *
 * Needs: Handle to window to obtain JobKey, JobItemKey, and JobItemCostBasedEstimationKey
 *
 */

int EXPORT DetermineIfSelectedCBEActive(HWND hwnd);



