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
 *  ..\cbest\CalcFn.h
 *
 *  This module contains all the functions that perform calculations
 *
 */





/* Calculates  the MaterialsUnitCost in the JobCostSheet table
 *
 * Needs: JobKey, JobCostSheetKey
 *        
 *
 */
int EXPORT CalcMaterialsUnitCost (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);




/* This function changes the LaborDailyCost in the JobCostSheet table to .00.
 * This function is called when the last laborer has been deleted 
 * and there are no entries to sum in the JobCrewLaborUsed table.  
 * 
 *
 * Needs: hheap, 
 *        pszJobKey, 
 *        pszJobCostSheetKey, 
 *
 */

int EXPORT SetLaborDailyCostToZero (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);




/* This function changes the MaterialsUnitCost in the JobCostSheet table to .00.
 * This function is called when the last JobMaterial has been deleted 
 * and there are no entries to sum in the JobMaterialUsed table.  
 * 
 *
 * Needs: hheap, 
 *        pszJobKey, 
 *        pszJobCostSheetKey, 
 *
 */

int EXPORT SetMaterialsUnitCostToZero (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);





/* This function changes the EquipmentDailyCost in the JobCostSheet table to .00.
 * This function is called when the last piece of equipment for a CostSheet is deleted 
 * and there are no entries to sum in the CrewEquipmentUsed table.  
 * 
 *
 * Needs: hheap, 
 *        pszJobKey, 
 *        pszJobCostSheetKey, 
 *
 */

int EXPORT SetEquipmentDailyCostToZero (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);






/* Inserts the Rate and OvertimeRate in the JobCrewLaborUsed table
 * after inserting a new Laborer in the JobCrew
 *
 * Needs: JobKey, JobCostSheetKey, JobCrewKey, LaborerKey
 *        
 *
 */
int EXPORT CalcLaborRate (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);





/* Calculates the LaborDailyCost in the JobCostSheet Table
 *
 *
 * Needs: JobCostSheetKey, JobKey
 *
 */

int EXPORT CalcLaborDailyCost (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);






/* Calculates the EquipmentDailyCost in the JobCostSheet Table
 *
 *
 * Needs: JobCostSheetKey, JobKey
 *
 */

int EXPORT CalcEquipmentDailyCost (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);


/* Calculates the UnitPrice in the JobCostSheet table
 *
 *
 * Needs: JobCostSheetKey, JobKey
 *
 */

int EXPORT CalcJobCostShtUnitPrice (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);




/* Calculates the Unit Price and ExtendedAmount
 * of the JobItem 
 *
 * Needs: ppszPNames 
 *        ppszParams (JobKey, JobItemKey)
 *        
 *
 */

 int EXPORT CalcJobItem (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);




/* Determines the item estimation method used by this JobItem
 *
 * Needs: ppszPNames 
 *        ppszParams (JobKey, JobItemKey)
 *        
 *
 */

 int EXPORT UpDateJobItemEstMethod (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);








/* This higher level calc function  calls  a function to update 
 * the  Job Items's UnitPrice and  ExtendedAmount to $.00
 * This occurs when an active JobCostBasedEstimate has added by
 * the user or the user changes a JobCostBasedEstimate to be active,
 * and there are currently no JobCostSheets from which to determine
 * the JobItem's UnitPrice.
 *
 * Needs: hheap, pszJobKey, pszJobItemKey
 *
 */
void EXPORT DoSetJobItemUnitPriceToZero (HHEAP heap, PSZ pszJobKey, PSZ pszJobItemKey);


void EXPORT DoUpDateJobItemUnitPrice (HHEAP heap, PSZ pszJobKey, PSZ pszJobItemKey);



/* This higher level calc function obtains the necessary keys
 * and calls lower level calc functions to recalculate
 * the Job and JobBreakdown. 
 * 
 * This function is needed when a JobCrew has been deleted or changed.
 *
 * Needs: hheap, pszJobKey, pszJobBreakdownKey
 *
 */

void EXPORT DoJobAndJobBreakReCalcCBEST(HHEAP hheap, PSZ pszJobKey, PSZ pszJobBreakdownKey);








/* This function changes the Active field in the JobCostBasedEstimate
 * table to be 0, meaning it is no longer active.   
 * 
 *
 * Needs: hheap, 
 *        pszJobKey, 
 *        pszJobItemKey, 
 *        pszJobItemCostBasedEstimateKey
 *
 */

int EXPORT UpDateJobItemCostBasedEstimate (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);


int EXPORT DoLaborersExist (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey);
int EXPORT DoesEquipExist (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey);
int EXPORT DoJobCrewsExist (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey);
int EXPORT DoJobMaterialsExist (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey);

extern void EXPORT DoJobCostShtReCalc (HWND hwnd);
extern void EXPORT DoDelJobMaterialsUsedReCalc (HWND hwnd);
extern void EXPORT DoJobMaterialsUsedReCalc (HWND hwnd, HWND hwndList);
extern void EXPORT DoCalcJobCrewLaborRate (HHEAP hheap, HWND hwndList, PSZ pszJobKey, PSZ pszJobCostSheetKey, PSZ pszJobCrewKey);
extern void EXPORT DoCalcJobCrewEquipRate (HHEAP hheap, HWND hwndList, PSZ pszJobKey, PSZ pszJobCostSheetKey, PSZ pszJobCrewKey);
/*
 * Needs: hheap, 
 *        pszJobKey, 
 *        pszJobItemKey, 
 *        pszJobBreakdownKey, 
 */
extern void EXPORT DoChgJobCostShtCalc (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);


extern int EXPORT ResetProdRate (HWND hwnd, BOOL bForAJob, PSZ pszInitialGranularity);


extern int EXPORT RecalcCBEJobItemUnitPrice (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);
