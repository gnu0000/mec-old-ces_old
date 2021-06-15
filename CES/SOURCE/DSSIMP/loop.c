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


#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itiutil.h"
#include "..\include\itifmt.h"
#include "..\include\itiwin.h"
#include "..\include\itiest.h"
#include "..\include\winid.h"
#include "..\include\itiglob.h"
#include "..\include\itiimp.h"
#include "..\include\itierror.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dssimp.h"
#include "loop.h"
#include "initdll.h"




/*
 * This function executes the query usQueryID once for every key in
 * ppszKeys.  It replaces the following % strings:
 *     ProductionDatabase
 *     Key1...Keyn where n = usKeyCount.
 *
 * usKeyCount is the number of keys per ppszKey.
 */

USHORT LoopAway (HHEAP  hheap,
                 PPSZ   ppszKeys,
                 USHORT usKeyCount,
                 PSZ    pszProdDatabase,
                 USHORT usQueryID)
   {
   USHORT   i, j, usError;
   CHAR     szTemp [2040];
   CHAR     szQuery [2040];
   PPSZ     ppszTokens, ppszValues;

   ppszTokens = ItiMemAlloc (hheap, sizeof (PSZ) * usKeyCount + 2, MEM_ZERO_INIT);
   ppszValues = ItiMemAlloc (hheap, sizeof (PSZ) * usKeyCount + 2, MEM_ZERO_INIT);
   if (ppszTokens == NULL || ppszValues == NULL)
      return -3;


   ppszTokens [0] = ItiStrDup (hheap, "ProductionDatabase");
   ppszValues [0] = ItiStrDup (hheap, pszProdDatabase);
   ppszTokens [usKeyCount + 1] = NULL;
   ppszValues [usKeyCount + 1] = NULL;
   for (i=1; i < usKeyCount + 1; i++)
      {
      sprintf (szTemp, "Key%d", i);
      ppszTokens [i] = ItiStrDup (hheap, szTemp);
      }

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, usQueryID, szTemp, sizeof szTemp);

   for (i=0, usError = 0; ppszKeys [i] && usError == 0; i++)
      {
      for (j=1; j < usKeyCount + 1; j++)
         ppszValues [j] = ItiDbGetZStr (ppszKeys [i], j - 1);

      ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, ppszTokens, 
                           ppszValues, NUM_ITERATIONS);
      usError = ItiDbExecSQLStatement (hheap, szQuery);
      printf (".");
      }

   for (j=1; j < usKeyCount + 1; j++)
      ppszValues [j] = NULL;

   ItiFreeStrArray (hheap, ppszTokens, usKeyCount + 2);
   ItiFreeStrArray (hheap, ppszValues, usKeyCount + 2);
   return usError;
   }


USHORT LoopAwayAndStore (HHEAP     hheap,
                         PPSZ      ppszKeys,
                         USHORT    usKeyCount,
                         PSZ       pszProdDatabase,
                         USHORT    usQueryID,
                         PPSZ      *pppszResults,
                         PUSHORT   pusNumResults)
   {
   USHORT   i, j, usError, usNumCols;
   CHAR     szTemp [2040];
   CHAR     szQuery [2040];
   PPSZ     ppszTokens, ppszValues, ppsz;
   HQRY     hqry;

   *pusNumResults = 0;
   *pppszResults = NULL;

   ppszTokens = ItiMemAlloc (hheap, sizeof (PSZ) * usKeyCount + 2, MEM_ZERO_INIT);
   ppszValues = ItiMemAlloc (hheap, sizeof (PSZ) * usKeyCount + 2, MEM_ZERO_INIT);
   if (ppszTokens == NULL || ppszValues == NULL)
      return -3;


   ppszTokens [0] = ItiStrDup (hheap, "ProductionDatabase");
   ppszValues [0] = ItiStrDup (hheap, pszProdDatabase);
   ppszTokens [usKeyCount + 1] = NULL;
   ppszValues [usKeyCount + 1] = NULL;
   for (i=1; i < usKeyCount + 1; i++)
      {
      sprintf (szTemp, "Key%d", i);
      ppszTokens [i] = ItiStrDup (hheap, szTemp);
      }

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, usQueryID, szTemp, sizeof szTemp);

   *pppszResults = ItiMemAlloc (hheap, sizeof (PSZ), MEM_ZERO_INIT);
   for (i=0, usError = 0; ppszKeys [i] && usError == 0; i++)
      {
      for (j=1; j < usKeyCount + 1; j++)
         ppszValues [j] = ItiDbGetZStr (ppszKeys [i], j - 1);

      ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, ppszTokens, 
                           ppszValues, NUM_ITERATIONS);

      hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usNumCols, &usError);
      usError = 0;
      while (hqry && ItiDbGetQueryRow (hqry, &ppsz, &usError))
         {
         *pusNumResults += 1;

         *pppszResults = ItiMemRealloc (hheap, *pppszResults,
                                        *pusNumResults * sizeof (PSZ),
                                        MEM_ZERO_INIT);
         for (j=0; j < usNumCols; j++)
            {
            ItiDbAddZStr (hheap, (*pppszResults) + *pusNumResults - 1, ppsz [j]);
            ItiMemFree (hheap, ppsz [j]);
            }
         ItiMemFree (hheap, ppsz);
         printf (".");
         }
      }

   for (j=1; j < usKeyCount + 1; j++)
      ppszValues [j] = NULL;

   ItiFreeStrArray (hheap, ppszTokens, usKeyCount + 2);
   ItiFreeStrArray (hheap, ppszValues, usKeyCount + 2);
   return usError;
   }




USHORT LoopTheLoop (HHEAP     hheap,
                    PPSZ      ppszKeys,
                    USHORT    usKeyCount,
                    USHORT    usKeyCount2,
                    PSZ       pszProdDatabase,
                    USHORT    usQuery1,
                    USHORT    usQuery2)
   {
   USHORT   i, j, usError, usNumCols;
   CHAR     szTemp [2040];
   CHAR     szQuery [2040];
   PPSZ     ppszTokens, ppszValues, ppsz;
   HQRY     hqry;

   ppszTokens = ItiMemAlloc (hheap, sizeof (PSZ) * usKeyCount + usKeyCount2 + 2, MEM_ZERO_INIT);
   ppszValues = ItiMemAlloc (hheap, sizeof (PSZ) * usKeyCount + usKeyCount2 + 2, MEM_ZERO_INIT);
   if (!ppszTokens || !ppszValues)
      return -3;


   ppszTokens [0] = ItiStrDup (hheap, "ProductionDatabase");
   ppszValues [0] = ItiStrDup (hheap, pszProdDatabase);
   ppszTokens [usKeyCount + usKeyCount2 + 1] = NULL;
   ppszValues [usKeyCount + usKeyCount2 + 1] = NULL;

   for (i=1; i < usKeyCount + 1; i++)
      {
      sprintf (szTemp, "Key%d", i);
      ppszTokens [i] = ItiStrDup (hheap, szTemp);
      }

   for (i=1; i < usKeyCount2 + 1; i++)
      {
      sprintf (szTemp, "LoopKey%d", i);
      ppszTokens [i + usKeyCount] = ItiStrDup (hheap, szTemp);
      }


   for (i=0, usError = 0; ppszKeys [i] && usError == 0; i++)
      {
      printf (".");

      for (j=1; j < usKeyCount + 1; j++)
         ppszValues [j] = ItiDbGetZStr (ppszKeys [i], j - 1);

      ItiMbQueryQueryText (hmod, ITIRID_IMPORT, usQuery1, szTemp, sizeof szTemp);
      ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, ppszTokens, 
                           ppszValues, NUM_ITERATIONS);

      hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usNumCols, &usError);
      usError = 0;
      while (usError == 0 && hqry && ItiDbGetQueryRow (hqry, &ppsz, &usError))
         {
         for (j=usKeyCount; j < usKeyCount + usKeyCount2 + 1; j++)
            ppszValues [j+1] = ppsz [j-usKeyCount];
         ItiMbQueryQueryText (hmod, ITIRID_IMPORT, usQuery2, szTemp, sizeof szTemp);
         ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, ppszTokens, 
                              ppszValues, NUM_ITERATIONS);
         usError = ItiDbExecSQLStatement (hheap, szQuery);

         ItiFreeStrArray (hheap, ppsz, usNumCols);
         printf (".");
         }
      }
   
//   for (j=1; j < usKeyCount + usKeyCount2 + 2; j++)
//      ppszValues [j] = NULL;
//   ItiFreeStrArray (hheap, ppszValues, usKeyCount + usKeyCount2 + 2);

   ItiMemFree (hheap, ppszValues[0]);
   ItiMemFree (hheap, ppszValues);

   ItiFreeStrArray (hheap, ppszTokens, usKeyCount + usKeyCount2 + 2);
   return usError;
   }



