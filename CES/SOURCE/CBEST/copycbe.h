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
 * ..\cbest\CopyCBE.h
 * Mark Hampton
 *
 * This file contains the copy functions for cost based estimation.
 */



extern BOOL EXPORT CopyCrewsForNewTask (HHEAP  hheap, 
                                 PSZ    pszJobKey,
                                 PSZ    pszJobCostSheetKey, 
                                 PSZ    pszCostSheetKey);


extern BOOL EXPORT CreateJobMaterialsUsed (HHEAP   hheap, 
                                           PSZ     pszJobKey, 
                                           PSZ     pszJobCostSheetKey, 
                                           PSZ     pszCostSheetKey);


extern BOOL EXPORT CompleteAddJobCostShtToTask (HHEAP hheap, 
                                                PSZ   pszJobKey, 
                                                PSZ   pszJobCostSheetKey, 
                                                PSZ   pszCostSheetKey);

extern BOOL CalculateNewJobCstShts (HHEAP hheap, 
                                    PSZ   pszJobKey, 
                                    PSZ   *ppszJCShtKeys);

extern BOOL CreateJobCrews (HHEAP hheap, 
                            HWND  hwnd, 
                            PSZ   pszJobKey, 
                            PSZ   *ppszJCShtKeys);

/*
 * This function calculates and inserts the Duration field in the JobCostSheet 
 * table.
 *
 * Duration = (Quantity[from JobItem] * QuantityItemPerUnit[from Task])/ProductionRate
 *             [from CostSheet]
 *  
 *
 * Needs:  pszCostSheetKey, pszJobKey, pszJobItemKey, pszCostBasedEstimateKey,
 *         pszTaskKey, pszJobCostSheetKey
 *         
 */

extern BOOL CalcAndInsertDuration (HHEAP  hheap, 
                                   PSZ    pszCostSheetKey, 
                                   PSZ    pszJobKey, 
                                   PSZ    pszJobItemKey,
                                   PSZ    pszCostBasedEstimateKey, 
                                   PSZ    pszTaskKey, 
                                   PSZ    pszJobCostSheetKey);

extern BOOL EXPORT CreateJobCostSheetForCBE (HHEAP  hheap, 
                                      HWND   hwnd, 
                                      PSZ    pszJobKey, 
                                      PSZ    pszJobItemKey,
                                      PSZ    *ppszCBEKeys);



extern BOOL EXPORT CopyCostSheetToJob (HHEAP   hheap,
                                HWND    hwnd,
                                PSZ     pszJobKey,
                                PSZ     pszJobItemKey,
                                PSZ     pszJobCostSheetKey,
                                PSZ     pszCostSheetKey);


extern BOOL EXPORT CopyCBEForJobItem (HHEAP hheap, 
                                      PSZ   pszJobKey,
                                      PSZ   pszJobItemKey, 
                                      PSZ   pszStandardItemKey);

/* calc query IDs */
#define COPY_CREW             1
#define COPY_COST_SHEET       2
#define COPY_COST_SHEET_TPA1  3
#define MATERIALS             4
#define MATERIALS_DEFZONE     5
// define COPY_MATERIAL       6 

#define COPY_CREW_EQ          7
#define COPY_CREW_EQ_DEF      8
#define COPY_CREW_LAB         9
#define COPY_CREW_LAB_DEF    10

#define MATERIALS_CNT        11
#define CBE_KEYS             12
#define CBE_KEYS_2           13
#define MATERIALS_CPY_CNT    14
 
#define CBE_KEYS_SUFFIX      15
#define CBE_KEYS_2_SUFFIX    16
