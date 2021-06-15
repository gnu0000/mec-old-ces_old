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



#define MAKEMAPPING           2
#define COPYAVERAGES          3
#define COPYAVERAGEPRICES     4
#define UPDATEAREAKEY         5
#define UPDATESTANDARDITEMKEY 6
#define UPDATEAREATYPEKEY     7

#define DELETEMAPPING             8
#define DELETEMAPPING_2           9
#define DELETEMAPPING_3          10

#define UPDATESTDITEMKEY_SUBQRY  11

#if !defined (RC_INVOKED)


#define QLEVEL_ERROR         -1
#define MIN_QLEVEL            0
#define MAX_QLEVEL            5



/*
 * GetQLevel returns the quantity level.
 *
 * Parameters: hheap                A heap.
 *
 *             pszStandardItemKey   The standard item to look at.
 *
 *             pszQuantity          The quantity in question.
 *
 *             pszBaseDate          The base date to look at.
 *
 *             ppszAreaType         This will be filled in with a
 *                                  PSZ allocated from hheap that contains
 *                                  the area type key for the matching
 *                                  PEMETH average.  If this parameter
 *                                  is NULL, then it is ignored.
 *
 * Return value: The quantity level, or QLEVEL_ERROR on error.
 */

extern USHORT EXPORT GetQLevel (HHEAP hheap,
                                PSZ   pszStandardItemKey,
                                PSZ   pszQuantity,
                                PSZ   pszBaseDate,
                                PSZ   *ppszAreaType);






/*
 * GetBestAverageKey returns the key to the best matching PEMETH average.
 *
 * Parameters: hheap                A heap.
 *
 *             usQLevel             The Quantity level to look for.
 *
 *             pszWorkType          The work type key to look for, or
 *                                  a pointer to "1" for don't care.
 *
 *             pszArea              The area key to look for, or
 *                                  a pointer to "1" for don't care.
 *
 *             pszStandardItemKey   The standard item to look at.
 *
 *             pszBaseDate          The base date to look at.
 *
 * Return value: the key is an array of PSZs as follows:
 *  ppsz [0] = StandardItemKey
 *  ppsz [1] = ImportDate
 *  ppsz [2] = AreaKey
 *  ppsz [3] = QuantityLevel
 *  ppsz [4] = WorkType
 *
 * These can be used as a key into WeightedAverageUnitPrice.
 */

extern PSZ * EXPORT GetBestAverageKey (HHEAP  hheap, 
                                       USHORT usQLevel, 
                                       PSZ    pszWorkType, 
                                       PSZ    pszArea,
                                       PSZ    pszStandardItemKey,
                                       PSZ    pszBaseDate);
 

#endif 

