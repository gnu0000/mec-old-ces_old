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
 * ..\include\CBEst.h
 * Mark Hampton
 *
 * This module provides the Cost Based Estimation Module.
 */





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
 *
 * Needs:  ppszSelKeys - an array containing the JobItemKeys
 *                       that the user has selected to be deleted
 *         
 */

extern BOOL EXPORT DoDelCostBasedEstForJobItem(HHEAP hheap1, 
                                               PSZ pszJobKey,
                                               PSZ pszJobItemKey);

extern BOOL EXPORT DelCBEForJobItem (HHEAP hheap, PSZ pszJobKey, PSZ pszJobItemKey);


/* 
 * This function loops through a stored array of JobCostBasedEstimates that have         
 * been selected by the user to be deleted. All of the JobCostSheets associated with a                                    
 * particular CBE are then retrieved and stored in another array.  This array is then    
 * passed to another function which deletes all of the Crews, Laborers, Equipment        
 * and Materials associated with these JobCostSheets                                     
 */                    
extern BOOL EXPORT DoDelJobCostSheetsForCBE (HHEAP hheapKeys, PSZ *ppszSelKeys, USHORT usCBEst);



/*
 * This higher level function is called when a new JobCostSheet has been added. 
 * It calls functions to calculate the LaborDailyCost, EquipmentDailyCost,
 * MaterialsUnitCost and the JobCostSheet's UnitPrice.
 *   
 *
 * Needs:  hheap
 *         JobKey
 *         JobCostSheetKey
 */

extern void EXPORT DoAddJobCostShtCalc (HHEAP hheap, 
                                        PSZ pszJobKey, 
                                        PSZ pszJobCostSheetKey);




/* Inserts the Rate and OvertimeRate in the JobCrewEquipmentUsed table,
 * after inserting  new Equipment in the JobCrew.
 *
 * Needs: JobKey, JobCostSheetKey, JobCrewKey, EquipmentKey
 *        
 *
 */
extern int EXPORT CalcEquipmentRate (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);





/* Calculates the MaterialsUnitPrice in the JobMaterialsUsed table
 *
 * Needs: JobKey, JobCostSheetKey, MaterialKey
 *        
 *
 */
extern int EXPORT CalcMaterialsUnitPrice (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);




/* This function changes the UnitPrice and ExtendedAmount fields in the JobItem
 * table to be .00, meaning that the active JobCostBasedEstimate 
 * does not have any JobCostSheets from which to calculate the JobItem UnitPrice  
 * 
 *
 * Needs: hheap, 
 *        pszJobKey, 
 *        pszJobItemKey, 
 *
 */

extern int EXPORT SetJobItemUnitPriceToZero (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);



extern int EXPORT CalcJobCostShtUnitPrice (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);

extern DOUBLE EXPORT CalcJobCostShtDuration (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);

extern int EXPORT CalcJobCostShtProductionRate (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);


/* This higher level calc function  calls  a function to recalculate 
 * the  Job Items's UnitPrice and  ExtendedAmount when CBEST is used
 * to determine the JobItem's unit price.
 *
 * Needs: hheap, pszJobKey, pszJobItemKey
 *
 */

extern int EXPORT DoJobItemReCalcCBEST (HHEAP heap, PSZ pszJobKey, PSZ pszJobItemKey);




/* ------------------------------------------------------------- */
/* --   FUNCTION: RecalcJobCostSheet                             */
/* -- PARAMETERS: hheap                                          */
/* --             pszJobKey                                      */
/* --             pszJobCostSheetKey                             */
/* --                                                            */
/* -- DESCRIPTION:                                               */
/* --    Recalculates the given job cost sheet materials         */
/* --    and crews.                                              */
/* --                                                            */
/* -- RETURN: void                                               */
/* ------------------------------------------------------------- */
void EXPORT RecalcJobCostSheet (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey);



extern USHORT EXPORT ReCalcJobCstShtMaterials (PSZ pszJobKey, PSZ pszJobCostSheetKey);

extern USHORT EXPORT ReCalcJobCstShtCrewEquipment (PSZ pszJobKey, PSZ pszJobCostSheetKey, PSZ pszJobCrewKey);
extern USHORT EXPORT ReCalcJobCstShtCrewLabor (PSZ pszJobKey, PSZ pszJobCostSheetKey, PSZ pszJobCrewKey);
extern USHORT EXPORT ReCalcJobCstShtCrew (PSZ pszJobKey, PSZ pszJobCostSheetKey);

/* returns TRUE if a Job CBEst is active for the given job item */
BOOL EXPORT ItiCesQueryJobCBEstActive (HHEAP hheap, PPSZ ppszNames, PPSZ ppszValues);

/* 
 * This function obtains all of the JobCostSheets (that are part of the
 * active CostBasedEstimate), for a JobItem,
 * and stores them in an array
 *
 * Needs: hheap
 *        JobKey
 *        JobItemKey
 *        CBEstKey
 *        **pppszJCstShtKeys
 */

BOOL EXPORT GetJobCostSheetsForJobItem(HHEAP hheap, 
                                       PSZ pszJobKey, 
                                       PSZ pszJobItemKey, 
                                       PSZ pszCBEstKey, 
                                       PSZ **pppszJCstShtKeys);

/* ------------------------------------------------------------- */
/* --   FUNCTION: CopyCBEForJobItem                              */
/* -- PARAMETERS: hheap                                          */
/* --             pszJobKey                                      */
/* --             pszJobItemKey                                  */
/* --             pszStandardItemKey                             */
/* --                                                            */
/* -- DESCRIPTION:                                               */
/* --    If there exists CostBasedEstimates associated with the  */
/* --    given StandardItemKey, then those CBEs are copied into  */
/* --    a new JobCostBasedEstimate entry, along with all the    */
/* --    crews and materials into new JobCostSheets, etc.        */
/* --                                                            */
/* -- RETURN: TRUE if okay                                       */
/* --         FALSE if not                                       */
/* ------------------------------------------------------------- */
BOOL EXPORT CopyCBEForJobItem (HHEAP hheap, 
                               PSZ   pszJobKey,
                               PSZ   pszJobItemKey, 
                               PSZ   pszStandardItemKey);

