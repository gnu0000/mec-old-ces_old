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
 * ItiEst.h
 */


/*
 * ItiEstGetJobWorkType returns a string that is the key for the 
 * work type for the given job.  The function first checks the Work-Type
 * field of the Job-Breakdown table.  If the Job-Breakdown cannot be found
 * or the Work-Type for the Job-Breakdown is 0, then the Work-Type is taken
 * from the Job table.
 *
 * Parameters: hheap                   The heap to allocate from.
 * 
 *             pszJobKey               The Job key.  This must not be null.
 *
 *             pszJobBreakdownKey      The Job-Breakdown key.  If this 
 *                                     parameter is NULL, then the Work-Type
 *                                     is retreived from the Job table.
 *
 * Return Value: NULL if the Work-Type cannot be found or an error occured,
 * or a string allocated from hheap.
 *
 */

extern PSZ EXPORT ItiEstGetJobWorkType (HHEAP hheap,
                                        PSZ   pszJobKey,
                                        PSZ   pszJobBreakdownKey);


/*
 * ItiEstGetJobArea returns the area that the job is in.
 *
 * Parameters: hheap             The heap to allocate from.
 *
 *             pszJobKey         The job to check.
 *
 *             pszAreaType       The Area-Type key.
 *
 *             pszBaseDate       The Base-Date.  If this is NULL,
 *                               the function will find the base
 *                               date on its own.
 *
 * Return value: NULL on error, or a pointer to a string that is 
 * the Area-Key.
 */

extern PSZ EXPORT ItiEstGetJobArea (HHEAP   hheap, 
                                    PSZ     pszJobKey,
                                    PSZ     pszAreaType,
                                    PSZ     pszBaseDate);




/*
 * ItiEstIsDavisBaconJob returns TRUE if the Job is using Davis-Bacon
 * wages.
 *
 * Parameters: hheap             A Heap.
 *
 *             pszJobKey         The job to check.
 *
 * Return value: FALSE if the Job is not using Davis-Bacon wages,
 *               TRUE if the Job is using Davis-Bacon wages.
 */

extern BOOL EXPORT ItiEstIsDavisBaconJob (HHEAP hheap,
                                          PSZ   pszJobKey);





/*
 * returns TRUE if given estimation method is the current one used for 
 * the item
 */

extern BOOL EXPORT ItiEstIsActive (HHEAP  hheap,
                                   PSZ    pszJobKey,
                                   PSZ    pszJobItemKey,
                                   USHORT usMethod);

/* estimation methods */
#define EST_AD_HOC                  0
#define EST_DEFAULT                 1
#define EST_PEMETH_AVERAGE          2
#define EST_PEMETH_REGRESSION       3
#define EST_COST_BASED              4

#define EST_NUM_METHODS             5


/*
 * ItiEstQueryActiveEstMethod returns the abbeviation of the estimation
 * method used to estimate the given item.
 *
 * Parameters: hheap                The heap to allocate from.
 *
 *             pszJobKey            The job to check.
 *
 *             pszJobItemKey        The job item key.
 *
 * Return value: NULL on error, or a pointer to a string that is 
 * the abbeviation.
 */

extern PSZ EXPORT ItiEstQueryActiveEstMethod (HHEAP hheap,
                                              PSZ   pszJobKey,
                                              PSZ   pszJobItemKey);




/*
 * ItiEstIsActiveCostSheet returns TRUE if the job cost sheet is 
 * used to determine the unit price of the job item.
 *
 * Parameters: hheap                The heap to allocate from.
 *
 *             pszJobKey            The job to check.
 *
 *             pszJobItemKey        The job item key.
 *
 *             pszJobCostSheetKey   The job cost sheet to check.
 *
 * Return value: TRUE if (1) the job item is using the Cost Based 
 * Estimation method and (2) the job cost sheet is in the active
 * cost based estimate for that job item.
 */

extern BOOL EXPORT ItiEstIsActiveCostSheet (HHEAP hheap, 
                                            PSZ   pszJobKey,
                                            PSZ   pszJobItemKey, 
                                            PSZ   pszJobCostSheetKey);



/*
 * ItiEstRoundUnitPrice rounds the unit price based on the standard 
 * item catalog's Rounding-Precision field.
 *
 * Parameters: hheap                A heap used for temporary allocations.
 *                                  MUST BE THE HHEAP THAT pszPrice WAS
 *                                  ALLOCATED FROM.
 *
 *             dPrice               The unit price to round.
 *
 *             pszStandardItemKey   The stanard item.
 *
 * Return value: the rounded unit price.
 */

extern PSZ EXPORT ItiEstRoundUnitPrice (HHEAP  hheap, 
                                        DOUBLE dPrice, 
                                        PSZ    pszStandardItemKey);


/*
 * ItiEstGetJobBaseDate returns the base date for the given job.
 *
 * Parameters: hheap          The heap to allocate from.
 *
 *             pszJobKey      The Job to check.
 *
 */

extern PSZ EXPORT ItiEstGetJobBaseDate (HHEAP hheap, PSZ pszJobKey);




/* 
 * returns TRUE if the given standard item has a pszMethodID item
 * estimation method available, or false if not.  If pszBaseDate is
 * NULL, then the UNDECLARED_BASE_DATE is used.
 */

extern BOOL EXPORT ItiEstQueryMethod (HHEAP hhpQM,
                                      PSZ   pszStandardItemKey,
                                      PSZ   pszBaseDate,
                                      PSZ   pszMethodID);


extern HWND EXPORT ItiEstBuildMethodWnd (HWND  hwnd,
                                         PSZ   pszMethodAbbrev);


typedef PSZ (EXPORT *PFNEST) (HHEAP hheap,
                              PSZ   pszBaseDate,
                              PSZ   pszJobKey,
                              PSZ   pszJobBreakdownKey,
                              PSZ   pszStandardItemKey,
                              PSZ   pszQuantity);

typedef BOOL (EXPORT *PFNJAVAIL) (HHEAP hheap,
                                  PSZ   pszJobKey,
                                  PSZ   pszJobItemKey);


typedef BOOL (EXPORT *PFNAVAIL) (HHEAP hheap,
                                 PSZ   pszStandardItemKey,
                                 PSZ   pszBaseDate);




extern USHORT EXPORT ItiEstChangeViewMenu (HWND     hwndMenu,
                                           HWND     hwndList,
                                           USHORT   usCommmandID,
                                           PSZ      pszJobKey,
                                           PSZ      pszJobItemKey,
                                           HHEAP    hhpCVM );

extern HWND EXPORT ItiEstProcessViewCommand (HWND   hwndParent,
                                             USHORT usCommandStart,
                                             USHORT usCommand);

/* called by applications that need estimation */
extern BOOL EXPORT ItiEstInitialize (void);



/* 
 * this function will call the ItiEstEstimateItem function 
 * for the specified item estimation method, pszEstimationID.
 */

extern PSZ EXPORT ItiEstEstimateItem (HHEAP   hhpEI, 
                                      PSZ     pszEstimationID,
                                      PSZ     pszBaseDate, 
                                      PSZ     pszJobKey, 
                                      PSZ     pszJobBreakdownKey,
                                      PSZ     pszStandardItemKey, 
                                      PSZ     pszQuantity);
