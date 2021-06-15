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



typedef int (*PFNCALCFN) (HHEAP hheap, PSZ *ppszPNames, PSZ *ppszParams);

typedef struct _pcelement
   {
   struct _pcelement *forward;
   struct _pcelement *backward;
   PFNCALCFN         pfnCalc;
   PSZ               *ppszPNames;
   PSZ               *ppszParams;
   USHORT            uPriority;
   } CALCQELEMENT;

typedef CALCQELEMENT *PCALCQELEMENT;




/*
 * This is the external function used to perform all calculations.
 *
 * pfnCalc ....... Ptr to calc function
 * *ppszParams ... key values
 * *ppszPNames ... key names
 * uCount ........ count of keys (0 means key name/values are null term)
 * uPriority ..... priority level >Num = >priority (0 = default priority)
 *
 * returns insert position relative to tail (as if anyone really cared)
 *
 */

USHORT ItiDBDoCalc ( PFNCALCFN pfnCalc,
                     PSZ       *ppszPNames,
                     PSZ       *ppszParams,
                     USHORT    uPriority);




/*
 * Required init fn to set up calculation queue
 *
 */
SHORT InitCalcQ (void);



/*
 * This function is for terminating the damn thread
 * which the os cannot seem to kill
 */
void DeInitCalcQ (void);

