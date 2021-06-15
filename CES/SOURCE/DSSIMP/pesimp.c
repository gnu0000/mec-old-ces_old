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
 * pesimp.c
 */

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
#include "..\include\dssimp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dssimp.h"
#include "initdll.h"
#include "loop.h"


static USHORT DoProjectUpdates (HHEAP  hheap,
                                PPSZ   ppszJobs,
                                PPSZ   ppszPCNs,
                                PPSZ   ppszCats,
                                PSZ    pszProdDatabase);


static void SetProductionDatabase (HHEAP hheap, PSZ pszProdDB)
   {
   char szQuery [128];

   sprintf (szQuery, "USE %s", pszProdDB);
   ItiDbExecSQLStatement (hheap, szQuery);
   }


static void SetImportDatabase (HHEAP hheap)
   {
   char szQuery [128];

   sprintf (szQuery, "USE %s", pszImportDatabase);
   ItiDbExecSQLStatement (hheap, szQuery);
   }





/*
 * This function fills an array with the values returned by the query
 * uQueryID.
 */

USHORT EXPORT GetKeys (HHEAP     hheap, 
                       HMODULE   hmod,
                       USHORT    uQueryID,
                       PPSZ      ppszTokens,   // terminate array w/null pointer
                       PPSZ      ppszValues,   // terminate array w/null pointer
                       PSZ       **pppszKeys,
                       USHORT    usKeys[])
   {
   char szTmp [4090], szQuery [4090];

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, uQueryID, szTmp, sizeof szTmp);
   ItiStrReplaceParams (szQuery, szTmp, sizeof szQuery, ppszTokens, 
                        ppszValues, NUM_ITERATIONS);

   return ItiDbBuildSelectedKeyArray2 (hheap, hmod, szQuery, pppszKeys, usKeys);
   }
                     

/*
 * this function frees the values returned from GetKeys
 */

void FreeKeys (HHEAP hheap, PPSZ ppsz)
   {
   USHORT i;

   for (i=0; ppsz [i]; i++)
      ;
   ItiFreeStrArray (hheap, ppsz, i);
   }





/*
 * This function fills in column ProjectItem.StandardItemKey by parsing
 * ProjectItem.StandardItemKeyText to extract the spec year.
 */

USHORT EXPORT ItiImpPesProjItemKey (HHEAP       hheap,
                                    PIMPORTINFO pii,
                                    PCOLINFO    pci,
                                    PSZ         pszProdDatabase)
   {
   PSZ apszTokens [6], apszValues [6];
   char     szQuery [1024];
   char     szTmp [1024];
   HQRY     hqry;
   PSZ      *ppsz;
   USHORT   uNumCols, uState;
   BOOL     bDone;

   /* first time arround, check the import database for the foreign keys */

   ItiFillPPSZ (apszTokens, 6,"ProductionDatabase",
                              "TableName",
                              "ColumnName",
                              "KeyValue",
                              "SpecYear",
                              "StandardItemNumber");

   ItiFillPPSZ (apszValues, 6,pszImportDatabase,
                              pii->pti->pszTableName,
                              pci->pszColumnName,
                              NULL,
                              NULL,
                              NULL);

   /* delete the temporary table */
   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        DELTEMPLISTTABLE, szTmp, sizeof szTmp);
   ItiStrReplaceParams (szQuery, szTmp, sizeof szQuery, apszTokens, 
                        apszValues, sizeof apszValues / sizeof (PSZ));
   ItiDbExecSQLStatement (hheap, szQuery);

   /* make the temporary table */
   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        MAKEPROJSTDITEMLIST, szTmp, sizeof szTmp);
   ItiStrReplaceParams (szQuery, szTmp, sizeof szQuery, apszTokens, 
                        apszValues, sizeof apszValues / sizeof (PSZ));
   if (ItiDbExecSQLStatement (hheap, szQuery))
      return pii->usError = -3;

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        PROJSTDITEMLIST, szTmp, sizeof szTmp);
   ItiStrReplaceParams (szQuery, szTmp, sizeof szQuery, apszTokens, 
                        apszValues, sizeof apszValues / sizeof (PSZ));
   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &uNumCols, &uState);
   if (hqry == NULL)
      return pii->usError = -2;

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        PROJITEMKEY, szTmp, sizeof szTmp);

   while (!(bDone = !ItiDbGetQueryRow (hqry, &ppsz, &uState)) && 
          pii->usError == 0)
      {
      apszValues [3] = ppsz [0];
      apszValues [5] = ItiStrDup (hheap, ppsz [0]);
      apszValues [4] = GetSpecYear (apszValues [5]);

      ItiStrReplaceParams (szQuery, szTmp, sizeof szQuery, apszTokens, 
                           apszValues, sizeof apszValues / sizeof (PSZ));
      pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
      ItiMemFree (hheap, apszValues [5]);
      ItiFreeStrArray (hheap, ppsz, uNumCols);
      }

   if (!bDone)
      ItiDbTerminateQuery (hqry);

   /* second time arround, check the production database for the foreign keys */
   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszProdDatabase;
   apszValues [3] = NULL;
   apszValues [4] = NULL;

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        PROJSTDITEMLIST, szTmp, sizeof szTmp);
   ItiStrReplaceParams (szQuery, szTmp, sizeof szQuery, apszTokens, 
                        apszValues, sizeof apszValues / sizeof (PSZ));
   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &uNumCols, &uState);
   if (hqry == NULL)
      return pii->usError = -2;

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        PROJITEMKEY, szTmp, sizeof szTmp);

   while (!(bDone = !ItiDbGetQueryRow (hqry, &ppsz, &uState)) && 
          pii->usError == 0)
      {
      apszValues [3] = ppsz [0];
      apszValues [5] = ItiStrDup (hheap, ppsz [0]);
      apszValues [4] = GetSpecYear (apszValues [5]);

      ItiStrReplaceParams (szQuery, szTmp, sizeof szQuery, apszTokens, 
                           apszValues, sizeof apszValues / sizeof (PSZ));
      pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
      ItiMemFree (hheap, apszValues [5]);
      ItiFreeStrArray (hheap, ppsz, uNumCols);
      }

   if (!bDone)
      ItiDbTerminateQuery (hqry);

   /* delete the temporary table */
   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        DELTEMPLISTTABLE, szTmp, sizeof szTmp);
   ItiStrReplaceParams (szQuery, szTmp, sizeof szQuery, apszTokens, 
                        apszValues, sizeof apszValues / sizeof (PSZ));
   ItiDbExecSQLStatement (hheap, szQuery);

   return pii->usError;
   }





USHORT EXPORT ItiImpCopyPesProjItem (HHEAP       hheap,
                                     PIMPORTINFO pii,
                                     PSZ         pszProdDatabase)
   {
   USHORT   uRet;
   PSZ      apszTokens [2], apszValues [2];
   PPSZ     ppszPCNs, ppszCats, ppszJobs;
   USHORT   usKeys [] = {1,0,0};

   uRet = ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYPROJITEM);
   if (uRet)
      return uRet;

   /* get the keys we'll need */
   ItiFillPPSZ (apszTokens, 2, "ProductionDatabase", NULL);
   ItiFillPPSZ (apszValues, 2,  pszProdDatabase,     NULL);

   uRet = GetKeys (hheap, hmod, GETPESPROJECTKEYS, apszTokens, apszValues,
                    &ppszPCNs, usKeys);
   if (uRet)
      return uRet;
   uRet = GetKeys (hheap, hmod, GETPESJOBKEYS, apszTokens, apszValues,
                    &ppszJobs, usKeys);
   if (uRet)
      {
      FreeKeys (hheap, ppszPCNs);
      return uRet;
      }

   usKeys [1] = 2;
   uRet = GetKeys (hheap, hmod, GETPESCATEGORYKEYS, apszTokens, apszValues,
                    &ppszCats, usKeys);
   if (uRet)
      {
      FreeKeys (hheap, ppszPCNs);
      FreeKeys (hheap, ppszJobs);
      return uRet;
      }

   uRet = DoProjectUpdates (hheap, ppszJobs, ppszPCNs, ppszCats, pszProdDatabase);

   FreeKeys (hheap, ppszPCNs);
   FreeKeys (hheap, ppszJobs);
   FreeKeys (hheap, ppszCats);
   return uRet;
   }


USHORT EXPORT ItiImpCopyPesCategory (HHEAP       hheap,
                                     PIMPORTINFO pii,
                                     PSZ         pszProdDatabase)
   {
   return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYCATEGORY);
   }


USHORT EXPORT ItiImpCopyPesProject (HHEAP       hheap,
                                    PIMPORTINFO pii,
                                    PSZ         pszProdDatabase)
   {
   PSZ apszTokens [1], apszValues [1];
   char     szQuery [1024];
   char     szTmp [1024];
   HQRY     hqry;
   PSZ      *ppsz;
   USHORT   uNumCols, uState;
   USHORT   uRet;

   if (uRet = ItiImpExec (hheap, pii, pszProdDatabase, hmod, DELETEOLDPESPROJ))
      return uRet;

   uRet = ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYPESPROJECT);

   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszProdDatabase;
   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        LISTBADPESPROJECTS, szTmp, sizeof szTmp);
   ItiStrReplaceParams (szQuery, szTmp, sizeof szQuery, apszTokens, 
                        apszValues, sizeof apszValues / sizeof (PSZ));

   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &uNumCols, &uState);
   if (hqry != NULL)
      {
      while (ItiDbGetQueryRow (hqry, &ppsz, &uState))
         {
         sprintf (szTmp, "Project \"%s\" not loaded, since no Job references it.",
                  ppsz [0]);
         puts (szTmp);
         ItiErrWriteDebugMessage (szTmp);
         ItiFreeStrArray (hheap, ppsz, uNumCols);
         }
      }
   return uRet;
   }



USHORT EXPORT ItiImpCalcPesProjItem (HHEAP       hheap,
                                     PIMPORTINFO pii,
                                     PSZ         pszProdDatabase)
   {
   pii->usError = ItiImpExec (hheap, pii, pszProdDatabase, hmod, CALCPROJITEM);
   return pii->usError;
   }



/*
 * This sets the jobItem quantities based on the sum of the 
 * project item quantities that were just imported.
 */
USHORT SetJobItemQuantities (HHEAP hheap, PSZ pszJobKey)
   {
   char szTmp [512], szQuery [512];

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, UPDATEJOBQUAN, szTmp, sizeof szTmp);
   sprintf (szQuery, szTmp, pszJobKey);
   ItiDbExecSQLStatement (hheap, szQuery);
   return 0;
   }


/*
 * this function does the following:
 *    1) Delete JobItems that are no longer in the project.
 *    2) Delete JobItemProjectItems that are no longer in the project.
 *    3) Add new JobItems.
 *    4) Add new JobItemProjectItems.
 */


static USHORT DoProjectUpdates (HHEAP  hheap,
                                PPSZ   ppszJobs,
                                PPSZ   ppszPCNs,
                                PPSZ   ppszCats,
                                PSZ    pszProdDatabase)
   {
   char     szTmp [4096], szQuery [4096], szUnitPrice [128];
   PSZ      apszTokens [13], apszValues [13];
   PPSZ     ppsz, ppszKeys;
   USHORT   uNumCols, i, uNumKeys;

   /*--- delete job items that no longer exist ---*/
   printf (" Previous references.");
   LoopTheLoop (hheap, ppszJobs, 1, 2, pszProdDatabase, LISTBADJOBITEMS, DELETEJOBITEMS);

   /*--- delete job item/project items that have no project items. ---*/
   printf ("\n Existing references.");
   LoopTheLoop (hheap, ppszJobs, 1, 5, pszProdDatabase, LISTBADJOBPROJITEMS, DELETEJOBPROJITEMS);
              
   /*--- Add job items ---*/
   printf ("\n New references.");
   LoopAwayAndStore (hheap, ppszJobs, 1, pszProdDatabase, LISTNEWJOBITEMS, &ppszKeys, &uNumKeys);
   printf ("ù\n");

   ItiFillPPSZ (apszTokens, 13,  "ProductionDatabase",
                                 "JobKey",
                                 "JobBreakdownKey",
                                 "JobAlternateGroupKey",
                                 "JobAlternateKey",
                                 "ItemEstimationMethodKey",
                                 "StandardItemKey",
                                 "UnitPrice",
                                 "JobItemKey",
                                 "ProjectControlNumber",
                                 "Category",
                                 "SequenceNumber",
                                 NULL);
                                                      
   ItiFillPPSZ (apszValues, 13,  pszProdDatabase,
                                 NULL, NULL, NULL, NULL, NULL, NULL,
                                 NULL, NULL, NULL, NULL, NULL, NULL);


   for (i=0; i < uNumKeys; i++)
      {
      ItiMbQueryQueryText (hmod, ITIRID_IMPORT, GETJOBKEYS, szTmp, sizeof szTmp);
      apszValues [1] = ppszKeys [i];
      apszValues [9] = ItiDbGetZStr (ppszKeys [i], 3);
      ItiStrReplaceParams (szQuery, szTmp, sizeof szQuery, apszTokens, 
                           apszValues, NUM_ITERATIONS);

      ppsz = ItiDbGetRow1 (szQuery, hheap, 0, 0, 0, &uNumCols);
      /* ===col values ===
       * JB.JobKey
       * JB.JobBreakdownKey
       * JAG.JobAlternateGroupKey
       * JAG.ChosenAlternateKey
       * IEM.ItemEstimationMethodKey
       */

      if (!ppsz)
         continue;

      /*------------------------    J        JB       JAG      JA       estmethod --*/
      ItiFillPPSZ (apszValues+1, 5, ppsz[0], ppsz[1], ppsz[2], ppsz[3], ppsz[4]);

      /*--- remove any offending commas from the unit price ---*/
      strcpy (szUnitPrice, ItiDbGetZStr (ppszKeys [i], 2));
      ItiExtract (szUnitPrice, ",$");

      ItiFillPPSZ (apszValues+6, 6,
                     ItiDbGetZStr (ppszKeys [i], 1),     // standard item key
                     szUnitPrice,                        // unit price
                     ItiDbGetKey  (hheap, "JobItem", 1), // get new key
                     ItiDbGetZStr (ppszKeys [i], 3),     // PCN
                     ItiDbGetZStr (ppszKeys [i], 4),     // Category
                     ItiDbGetZStr (ppszKeys [i], 5));    // Sequence #

      /*--- add the job item ---*/
      ItiMbQueryQueryText (hmod, ITIRID_IMPORT, INSERTJOBITEMS, szTmp, sizeof szTmp);
      ItiStrReplaceParams (szQuery, szTmp, sizeof szQuery, apszTokens, 
                           apszValues, NUM_ITERATIONS);
      ItiDbExecSQLStatement (hheap, szQuery);

      /*--- add the job item project item ---*/
      ItiMbQueryQueryText (hmod, ITIRID_IMPORT, INSERTJOBPROJITEMS, szTmp, sizeof szTmp);
      ItiStrReplaceParams (szQuery, szTmp, sizeof szQuery, apszTokens, 
                           apszValues, NUM_ITERATIONS);
      ItiDbExecSQLStatement (hheap, szQuery);

      ItiFreeStrArray (hheap, ppsz, uNumCols);
      ItiMemFree (hheap, apszValues [8]);
      printf (".");
      }

   /*--- populate the sparce JI/PI matrix ---*/
   printf ("\n Recalculating Job Item Totals.\n");
   SetProductionDatabase (hheap, pszProdDatabase);
   for (i=0; ppszJobs[i]; i++)
      {
      printf (".");
      SetJobItemQuantities (hheap, ppszJobs[i]);
      printf ("ù");
      MapJobItemProjItem   (ppszJobs[i]);
      }
   SetImportDatabase (hheap);

   ItiFreeStrArray (hheap, ppszKeys, uNumKeys);

   return 0;
   }

