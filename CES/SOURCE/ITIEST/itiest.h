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
 * ItiEstRoundUnitPrice rounds the unit price based on the standard 
 * item catalog's Rounding-Precision field.
 *
 * Parameters: hheap                A heap used for temporary allocations.
 *                                  MUST BE THE HHEAP THAT pszPrice WAS
 *                                  ALLOCATED FROM.
 *
 *             pszPrice             The unit price to round.
 *
 *             pszStandardItemKey   The stanard item.
 *
 * Return value: the rounded unit price.
 */

extern PSZ EXPORT ItiEstRoundUnitPrice (HHEAP  hheap, 
                                        PSZ    pszPrice, 
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

