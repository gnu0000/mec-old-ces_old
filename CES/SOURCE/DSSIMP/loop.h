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
 * This function executes the query usQueryID once for every key in
 * ppszKeys.  It replaces the following % strings:
 *     ProductionDatabase
 *     Key1...Keyn where n = usKeyCount.
 *
 * usKeyCount is the number of keys per ppszKey.
 */

#define NUM_ITERATIONS  400


extern USHORT LoopAway (HHEAP  hheap,
                        PPSZ   ppszKeys,
                        USHORT usKeyCount,
                        PSZ    pszProdDatabase,
                        USHORT usQueryID);


extern USHORT LoopAwayAndStore (HHEAP     hheap,
                                PPSZ      ppszKeys,
                                USHORT    usKeyCount,
                                PSZ       pszProdDatabase,
                                USHORT    usQueryID,
                                PPSZ      *pppszResults,
                                PUSHORT   pusNumResults);


extern USHORT LoopTheLoop (HHEAP     hheap,
                           PPSZ      ppszKeys,
                           USHORT    usKeyCount,
                           USHORT    usKeyCount2,
                           PSZ       pszProdDatabase,
                           USHORT    usQuery1,
                           USHORT    usQuery2);



