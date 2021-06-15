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
 * average.c
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "average.h"
#include "initdll.h"






#define AREA_WEIGHT           1
#define WORKTYPE_WEIGHT       2
#define QUANTITYLEVEL_WEIGHT  4
#define MAX_WEIGHT   (AREA_WEIGHT + QUANTITYLEVEL_WEIGHT + WORKTYPE_WEIGHT)

#define AREA                  0
#define QUANTITYLEVEL         1
#define WORKTYPE              2
#define PRICE                 3


#define  NUM_OF_SI  1000
#define  SZLEN        14

static CHAR  szStar[10] = "-\\|/+   ";
static CHAR  aszResults [NUM_OF_SI + 2] [3] [SZLEN];



/*
 * ItiEstInstall Creates all the necessary tables, indicies, etc for both
 * the production database and import database.
 *
 * Returns TRUE on success, FALSE on error.
 */

BOOL EXPORT ItiEstInstall (PSZ szDatabaseName,
                           PSZ pszImportDatabase)
   {
   return TRUE;
   }


/*
 * ItiEstDeinstall deletes all the necessary tables, indicies, etc for both
 * the production database and import database.
 *
 * Returns TRUE on success, FALSE on error.
 */

BOOL EXPORT ItiEstDeinstall (PSZ szDatabaseName,
                             PSZ pszImportDatabase)
   {
   return TRUE;
   }




/*
 * ItiEstTableChanged is used to tell an installable estimation method
 * that a table has changed.
 *
 * Parameters: usTable        The ID of the table that has changed.
 *
 *             ausColumns     An array from 0..usNumColumns, that
 *                            contain the IDs of the columns that
 *                            have changed.  If this parameter is
 *                            NULL, then all columns have changed.
 *
 *             usNumColumns   The number of columns in ausColumns.
 *
 *             pszKey         A pointer to a string that contains
 *                            the unique key.  The string is in the
 *                            form "ColumnName = ColumnValue
 *                            [AND ColumnName = ColumnValue]".
 */

void EXPORT ItiEstTableChanged (USHORT   usTable,
                                USHORT   ausColumns [],
                                USHORT   usNumColumns,
                                PSZ      pszKey)
   {
   }





int EXPORT GetAreaWeight (PSZ     *ppsz,
                   PSZ     pszArea,
                   PSZ     pszWorkType,
                   USHORT  usQLevel)
   {
   int iWeight;
   USHORT   usQLevelIn;

   iWeight = 0;
   usQLevelIn = atoi (ppsz [QUANTITYLEVEL]);
   if (usQLevelIn > 0)
      /* usQLevel == 1 means 0-5%ile, usQLevel == 2 means 5%ile-25%ile, etc. */
      usQLevelIn -= 1;

   if (usQLevelIn == 0)
      {
      /* do nothing -- a wild card is the lowest priority match */
      }
   else if (usQLevel == usQLevelIn - 1)
      {
      /* a match */
      iWeight += QUANTITYLEVEL_WEIGHT;
      }
   else
      {
      /* not a match */
      return -MAX_WEIGHT;
      }

   if (atol (ppsz [WORKTYPE]) == 1)
      {
      /* do nothing -- a wild card is the lowest priority match */
      }
   else if (pszWorkType != NULL && strcmp (pszWorkType, ppsz [WORKTYPE]) == 0)
      {
      /* a match */
      iWeight += WORKTYPE_WEIGHT;
      }
   else
      {
      /* not a match */
      return -MAX_WEIGHT;
      }

   if (atol (ppsz [AREA]) == 1)
      {
      /* do nothing -- a wild card is the lowest priority match */
      }
   else if (pszArea != NULL && strcmp (pszArea, ppsz [AREA]) == 0)
      {
      /* a match */
      iWeight += AREA_WEIGHT;
      }
   else
      {
      /* not a match */
      return -MAX_WEIGHT;
      }

   return iWeight;
   }



PSZ EXPORT GetBestAverage (HHEAP  hheap,
                    USHORT usQLevel,
                    PSZ    pszWorkType,
                    PSZ    pszArea,
                    PSZ    pszStandardItemKey,
                    PSZ    pszBaseDate)
   {
   char   szTemp [400] = "";
   HQRY   hqry = NULL;
   HHEAP  hhp = NULL;
   USHORT usState, usNumCols;
   PSZ    *ppsz, pszPrice = NULL;
   int    iMaxWeight, iWeight;
   DOUBLE dPrice = 0.0;

   if (
      (NULL == pszWorkType)
      || (NULL == pszArea)
      || (NULL == pszStandardItemKey)
      || (NULL == pszBaseDate)
      )
      return " 0 ";

   hhp = ItiMemCreateHeap (MAX_HEAP_SIZE);

   ItiStrCpy (szTemp,
            "SELECT WAUP.AreaKey, WAUP.QuantityLevel, WAUP.WorkType, "
            "WAUP.WeightedAverageUnitPrice "
            "FROM StandardItemPEMETHAverage SIPA, "
            "WeightedAverageUnitPrice WAUP "
            "WHERE SIPA.StandardItemKey = ", sizeof szTemp);

   ItiStrCat (szTemp, pszStandardItemKey, sizeof szTemp);
   ItiStrCat (szTemp, " AND SIPA.BaseDate = '", sizeof szTemp);
   ItiStrCat (szTemp, pszBaseDate, sizeof szTemp);
   ItiStrCat (szTemp, 
            "' AND SIPA.StandardItemKey = WAUP.StandardItemKey "
            "AND SIPA.ImportDate = WAUP.ImportDate ", sizeof szTemp);
             

   hqry = ItiDbExecQuery (szTemp, hhp, 0, 0, 0, &usNumCols, &usState);
   pszPrice = NULL;

   iMaxWeight = -MAX_WEIGHT;
   if (hqry != NULL)
      {
      while (ItiDbGetQueryRow (hqry, &ppsz, &usState))
         {
         iWeight = GetAreaWeight (ppsz, pszArea, pszWorkType, usQLevel);
         if (iWeight > iMaxWeight)
            {
            iMaxWeight = iWeight;
            if (pszPrice)
               ItiMemFree (hhp, pszPrice);
            pszPrice = ItiStrDup (hhp, ppsz [PRICE]);
            }
         ItiFreeStrArray (hhp, ppsz, usNumCols);
         }
      }

   /* 11/12/1992: added rounding code. mdh bsr 920923-4101 */
   if (pszPrice)
      {
      dPrice = ItiStrToDouble (pszPrice);
      ItiMemFree (hhp, pszPrice);
      pszPrice = ItiEstRoundUnitPrice (hheap, dPrice, pszStandardItemKey);
      }               // Since this psz is returned, use the hheap passed in.
   else
      pszPrice = NULL;


   if (NULL != hhp)
      ItiMemDestroyHeap (hhp);

   return pszPrice;
   }



PSZ * EXPORT GetBestAverageKey (HHEAP  hheap,
                                USHORT usQLevel,
                                PSZ    pszWorkType,
                                PSZ    pszArea,
                                PSZ    pszStandardItemKey,
                                PSZ    pszBaseDate)
   {
   char   szTemp [400] = "";
   HQRY   hqry;
   USHORT usState, usNumCols;
   PSZ    *ppsz, *ppszKey;
   int    iMaxWeight, iWeight;

   ItiStrCpy (szTemp,
            "SELECT WAUP.StandardItemKey, WAUP.ImportDate, WAUP.AreaKey, "
            "WAUP.QuantityLevel, WAUP.WorkType "
            "FROM StandardItemPEMETHAverage SIPA, "
            "WeightedAverageUnitPrice WAUP "
            "WHERE SIPA.StandardItemKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszStandardItemKey, sizeof szTemp);
   ItiStrCat (szTemp,  " AND SIPA.BaseDate = '", sizeof szTemp);
   ItiStrCat (szTemp, pszBaseDate, sizeof szTemp);
   ItiStrCat (szTemp,  "' AND SIPA.StandardItemKey = WAUP.StandardItemKey "
                    " AND SIPA.ImportDate = WAUP.ImportDate ", sizeof szTemp);

   hqry = ItiDbExecQuery (szTemp, hheap, 0, 0, 0, &usNumCols, &usState);
   ppszKey = NULL;

   iMaxWeight = -MAX_WEIGHT;
   if (hqry != NULL)
      {
      while (ItiDbGetQueryRow (hqry, &ppsz, &usState))
         {
         iWeight = GetAreaWeight (ppsz, pszArea, pszWorkType, usQLevel);
         if (iWeight > iMaxWeight)
            {
            iMaxWeight = iWeight;
            if (ppszKey)
               ItiFreeStrArray (hheap, ppszKey, usNumCols);
            ppszKey = ppsz;
            }
         else
            ItiFreeStrArray (hheap, ppsz, usNumCols);
         }
      }
   return ppszKey;
   }




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

USHORT EXPORT GetQLevel (HHEAP hheap,
                         PSZ   pszStandardItemKey,
                         PSZ   pszQuantity,
                         PSZ   pszBaseDate,
                         PSZ   *ppszAreaType)
   {
   char szTemp [400] = "";
   PSZ *ppsz;
   USHORT usQLevel, i, usNumCols;
   double dQuantity, d;

   if (ppszAreaType)
      *ppszAreaType = NULL;

   ItiStrCpy (szTemp,
            "SELECT PA.FifthPercentile, PA.TwentyFifthPercentile, "
            "PA.FiftiethPercentile, PA.SeventyFifthPercentile, "
            "PA.NinetyFifthPercentile, PA.AreaTypeKey "
            "FROM StandardItemPEMETHAverage SIPA, PEMETHAverage PA "
            "WHERE SIPA.StandardItemKey = ", sizeof szTemp); 
   ItiStrCat (szTemp, pszStandardItemKey, sizeof szTemp);
   ItiStrCat (szTemp, " AND SIPA.BaseDate = '", sizeof szTemp);
   ItiStrCat (szTemp, pszBaseDate, sizeof szTemp);
   ItiStrCat (szTemp,  "' AND SIPA.ImportDate = PA.ImportDate "
            "AND SIPA.StandardItemKey = PA.StandardItemKey ", sizeof szTemp);

   ppsz = ItiDbGetRow1 (szTemp, hheap, 0, 0, 0, &usNumCols);
   if (ppsz == NULL)
      return QLEVEL_ERROR;

   dQuantity = ItiStrToDouble (pszQuantity);
   usQLevel = MAX_QLEVEL;
   for (i = MIN_QLEVEL; i < MAX_QLEVEL && usQLevel == MAX_QLEVEL; i++)
      {
      d = ItiStrToDouble (ppsz [i]);
      if (dQuantity < d)
         usQLevel = i;
      }

   if (ppszAreaType)
      *ppszAreaType = ItiStrDup (hheap, ppsz [5]);

   ItiFreeStrArray (hheap, ppsz, usNumCols);

   return usQLevel;
   }






/*
 * ItiEstItem returns a string to be used as the unit price for
 * a given standard item.  This function is usefull when the item has
 * not yet been added to the data base.
 *
 * Parameters: hheap                A heap to allocate from.
 *
 *             pszBaseDate          The base date for the price.  This
 *                                  parameter may be null iff pszJobKey
 *                                  is not NULL.  If this parameter and
 *                                  pszJobKey are supplied, pszBaseDate
 *                                  overrides the Job's base date.
 *
 *             pszJobKey            The job that the item is in.
 *                                  If the item is not in a job,
 *                                  set this parameter to NULL.
 *
 *             pszJobBreakdownKey   The job breakdown that the
 *                                  item is in.  If the item is not
 *                                  in a job, set this parameter to NULL.
 *
 *             pszStandardItemKey   The standard item to price.  This
 *                                  parameter may not be NULL.
 *
 *             pszQuantity          The quantity for this item.  If the
 *                                  quantity is unknown, use NULL.
 *
 * Return value:  A pointer to a null terminated string containing the
 * unit price.  The string is allocated from hheap.  Special return
 * values:
 *
 *    NULL:          An error occured estimating the item.
 *
 *    empty string:  Not enough information was supplied to estimate
 *                   the given item.
 *
 * Notes:  The caller is responsible for freeing the unit price from the
 * heap.
 *
 * The unit price is rounded according to the RoundingPrecision field
 * in the standard item catalog.
 */

PSZ EXPORT ItiEstItem (HHEAP hheap,
                       PSZ   pszBaseDate,
                       PSZ   pszJobKey,
                       PSZ   pszJobBreakdownKey,
                       PSZ   pszStandardItemKey,
                       PSZ   pszQuantity)
   {
   BOOL     bGotBaseDate = FALSE;
   PSZ      pszPrice, pszWorkType, pszAreaType, pszArea;
   USHORT   usQLevel;

   if (pszStandardItemKey == NULL ||
       (pszJobKey == NULL && pszBaseDate == NULL))
      return NULL;

   if (pszBaseDate == NULL)
      {
      pszBaseDate = ItiEstGetJobBaseDate (hheap, pszJobKey);
      if (pszBaseDate == NULL)
         return NULL;

      bGotBaseDate = TRUE;
      }

   if (pszQuantity == NULL)
      return ItiStrDup (hheap, "");

   pszPrice = NULL;

   usQLevel = GetQLevel (hheap, pszStandardItemKey, pszQuantity,
                         pszBaseDate, &pszAreaType);
   pszWorkType = ItiEstGetJobWorkType (hheap, pszJobKey, pszJobBreakdownKey);
   pszArea = ItiEstGetJobArea (hheap, pszJobKey, pszAreaType, pszBaseDate);

//   6/16/93 mdh: commented this out to allow average prices to always 
//   be returned.
//
//   if (usQLevel != QLEVEL_ERROR &&
//       usQLevel > MIN_QLEVEL &&
//       usQLevel < MAX_QLEVEL)
      pszPrice = GetBestAverage (hheap, usQLevel, pszWorkType, pszArea,
                                 pszStandardItemKey, pszBaseDate);

   if (bGotBaseDate)
      ItiMemFree (hheap, pszBaseDate);

   ItiMemFree (hheap, pszWorkType);
   ItiMemFree (hheap, pszAreaType);
   ItiMemFree (hheap, pszArea);

   return pszPrice;
   }


BOOL EXPORT ItiEstQueryJobAvail (HHEAP hheap,
                                 PSZ   pszJobKey,
                                 PSZ   pszJobItemKey)
   {
   char szTemp [400] = "";
   PSZ psz;

   sprintf (szTemp,
            "SELECT SIPA.ImportDate "
            "FROM StandardItemPEMETHAverage SIPA, "
            "Job J, JobItem JI "
            "WHERE JI.JobItemKey = %s "
            "AND J.JobKey = %s "
            "AND J.JobKey = JI.JobKey "
            "AND SIPA.StandardItemKey = JI.StandardItemKey "
            "AND SIPA.BaseDate = J.BaseDate ",
            pszJobItemKey, pszJobKey);

   psz = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);
   if (psz == NULL)
      return FALSE;

   ItiMemFree (hheap, psz);
   return TRUE;
   }

BOOL EXPORT ItiEstQueryAvail (HHEAP hheap,
                              PSZ   pszStandardItemKey,
                              PSZ   pszBaseDate)
   {
   CHAR szTemp [200] = "";
   CHAR szBaseDate [126] = " '";
   PSZ psz;

   if (NULL == hheap)
      return FALSE;

   if ((pszStandardItemKey == NULL) || (pszStandardItemKey[0] == '\0') )
      return FALSE;

   ItiStrCpy (szTemp,
            "SELECT MAX (BaseDate) "
            "FROM StandardItemPEMETHAverage "
            "WHERE StandardItemKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszStandardItemKey, sizeof szTemp);

   psz = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);
   if ((psz == NULL) || ((*psz) == '\0'))
      return FALSE;

   ItiMemFree (hheap, psz);
   return TRUE;
   }


/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/*                                                                        */
/* Stuff for import                                                       */
/*                                                                        */
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/


USHORT EXPORT ItiImpCopyAvg (HHEAP       hheap,
                             PIMPORTINFO pii,
                             PSZ         pszProdDatabase)
   {
//   char  szQuery [1000];
//   char  szTemp [1000];
   char     szQuery [2020] = "";
   char     szTemp [2020] = "";
   PSZ   apszTokens [3], apszValues [3];

   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszProdDatabase;
   apszTokens [1] = "TableName";
   apszValues [1] = pii->pti->pszTableName;
   apszTokens [2] = "KeyValue";
   apszValues [2] = UNDECLARED_BASE_DATE;

   /* copy PEMETHAverage to the production database */
   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, COPYAVERAGES,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery,
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
   if (pii->usError)
      return pii->usError;


   /* -- Clean up the mapping tables */
   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, DELETEMAPPING,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery,
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
   if (pii->usError)
      return pii->usError;

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, DELETEMAPPING_2,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery,
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
   if (pii->usError)
      return pii->usError;

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, DELETEMAPPING_3,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery,
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
   if (pii->usError)
      return pii->usError;



   /* Build the mapping table */
   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, MAKEMAPPING,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery,
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
   if (pii->usError)
      return pii->usError;

   return pii->usError;
   }



USHORT EXPORT ItiImpCopyAvgPrice (HHEAP       hheap,
                                  PIMPORTINFO pii,
                                  PSZ         pszProdDatabase)
   {
//   char  szQuery [1024];
//   char  szTemp [1024];
   char     szQuery [2020] = "";
   char     szTemp [2020] = "";
   PSZ   apszTokens [2], apszValues [2];

   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszProdDatabase;
   apszTokens [1] = "TableName";
   apszValues [1] = pii->pti->pszTableName;

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, COPYAVERAGEPRICES,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery,
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);

   return pii->usError;
   }





USHORT EXPORT ItiImpAreaTypeKey (HHEAP       hheap,
                                 PIMPORTINFO pii,
                                 PCOLINFO    pci,
                                 PSZ         pszProdDatabase)
   {
//   char  szQuery [1024];
//   char  szTemp [1024];
   char     szQuery [2020] = "";
   char     szTemp [2020] = "";
   PSZ   apszTokens [4], apszValues [4];

   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = ItiImpQueryImportDatabase ();
   apszTokens [1] = "TableName";
   apszValues [1] = pii->pti->pszTableName;
   apszTokens [2] = "ColumnName";
   apszValues [2] = pci->pszColumnName;
   apszTokens [3] = "TargetColumn";
   apszValues [3] = "AreaTypeKey";

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, UPDATEAREATYPEKEY,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery,
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);

   if (pii->usError)
      return pii->usError;

   apszValues [0] = pszProdDatabase;
   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, UPDATEAREATYPEKEY,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery,
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);

   return pii->usError;
   }





USHORT EXPORT ItiImpAreaKey (HHEAP       hheap,
                             PIMPORTINFO pii,
                             PCOLINFO    pci,
                             PSZ         pszProdDatabase)
   {
//   char  szQuery [1024];
//   char  szTemp [1024];
   char     szQuery [2020] = "";
   char     szTemp [2020] = "";
   PSZ   apszTokens [4], apszValues [4];

   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = ItiImpQueryImportDatabase ();
   apszTokens [1] = "TableName";
   apszValues [1] = pii->pti->pszTableName;
   apszTokens [2] = "ColumnName";
   apszValues [2] = pci->pszColumnName;
   apszTokens [3] = "TargetColumn";
   apszValues [3] = "AreaKey";

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, UPDATEAREAKEY,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery,
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);

   if (pii->usError)
      return pii->usError;

   apszValues [0] = pszProdDatabase;
   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, UPDATEAREAKEY,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery,
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   pii->usError = ItiDbExecSQLStatement (hheap, szQuery);

   return pii->usError;
   }



USHORT EXPORT ItiImpWorkType (HHEAP       hheap,
                             PIMPORTINFO pii,
                             PCOLINFO    pci,
                             PSZ         pszProdDatabase)
   {
   char     szQuery [2020] = "";
   char     szTemp [2020] = "";
   PSZ      apszTokens [6], apszValues [6];
   HQRY     hqry;
   PSZ      *ppsz;
   USHORT   usNumCols, usState;
   BOOL     bDone;

   apszTokens [0] = "ProductionDatabase";
   apszValues [0] = pszProdDatabase;
   apszTokens [1] = "TableName";
   apszValues [1] = pii->pti->pszTableName;
   apszTokens [2] = "ColumnName";
   apszValues [2] = pci->pszColumnName;
   apszTokens [3] = "KeyValue";
   apszValues [3] = NULL;
   apszTokens [4] = "CodeTableName";
   apszValues [4] = "WRKTYP";
   apszTokens [5] = "TargetColumn";
   apszValues [5] = "WorkType";

   ItiMbQueryQueryText (0, ITIRID_IMPORT, ITIIMP_GETDISTINCTFKS,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery,
                        apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usNumCols, &usState);
   if (hqry == NULL)
      return pii->usError = -2;

   ItiMbQueryQueryText (0, ITIRID_IMPORT, ITIIMP_UPDATECODEVALUE,
                        szTemp, sizeof szTemp);
   while (!(bDone = !ItiDbGetQueryRow (hqry, &ppsz, &usState)) &&
          pii->usError == 0)
      {
      apszValues [3] = *ppsz;
      ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery,
                           apszTokens, apszValues, sizeof apszTokens / sizeof (PSZ));
      pii->usError = ItiDbExecSQLStatement (hheap, szQuery);
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }

   if (!bDone)
      ItiDbTerminateQuery (hqry);

   return pii->usError;
   }





USHORT EXPORT ItiImpStandardItemKey (HHEAP       hheap,
                                     PIMPORTINFO pii,
                                     PCOLINFO    pci,
                                     PSZ         pszProdDatabase)
   {
   //BOOL     bDone;
   HQRY     hqry;
   HHEAP    hhpLocSIK = NULL;
   PSZ      *ppsz;
   USHORT   usNumCols, usState, usCnt;
   PSZ      apszTokens [8+1], apszValues [8+1];
   CHAR     szQuery [300] = "";
   CHAR     szTemp  [300] = "";
   CHAR     szMark[3] = "";

   hhpLocSIK = ItiMemCreateHeap (MAX_HEAP_SIZE);

#define SI_KEY     0  
#define SI_ID      1  
#define SI_SPYR    2  

   apszTokens [0] = "ProductionDatabase"; apszValues [0] = pszProdDatabase;
   apszTokens [1] = "TableName";      apszValues [1] = pii->pti->pszTableName;
   apszTokens [2] = "ColumnName";     apszValues [2] = pci->pszColumnName;
   apszTokens [3] = "TargetColumn";   apszValues [3] = "StandardItemKey";
   apszTokens [4] = "TargetSpecYear"; apszValues [4] = ""; //ItiDbGetDbValue (hhpLocSIK, ITIDB_MAX_SPEC_YEAR);
   apszTokens [5] = "TargetUnitType";
        //  apszValues [5] = (pii->  ) ? " 1 " : " 0 ";
            apszValues [5] = " 0 ";
   apszTokens [6] = "StdItemNumber";   apszValues [6] = "StandardItemNumber";
   apszTokens [7] = "StdItemKey";      apszValues [7] = "";
   apszTokens [8] = ""; apszValues [8] = "";

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, UPDATESTDITEMKEY_SUBQRY,
                         szTemp, sizeof szTemp);

   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery,
                         apszTokens, apszValues,
                         sizeof apszTokens / sizeof (PSZ));

   hqry = ItiDbExecQuery (szQuery, hhpLocSIK, 0, 0, 0, &usNumCols, &usState);
   if (hqry == NULL)
      return pii->usError = -2;

   aszResults[0][0][0] = '\0';
   usCnt = 0;
   while (ItiDbGetQueryRow(hqry, &ppsz, &usState)) 
      {
      if (usCnt <= NUM_OF_SI)
         {
         ItiStrCpy(aszResults[usCnt][SI_KEY ], ppsz[SI_KEY ], SZLEN);  // StandardItemKey
         ItiStrCpy(aszResults[usCnt][SI_ID  ], ppsz[SI_ID  ], SZLEN);  // StandardItemNumber
         ItiStrCpy(aszResults[usCnt][SI_SPYR], ppsz[SI_SPYR], SZLEN);  // StandardItem SpecYear
   
         aszResults[usCnt+1][0][0] = '\0';
         }
   
      ItiFreeStrArray (hhpLocSIK, ppsz, usNumCols);
      usCnt++;
      }

   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, UPDATESTANDARDITEMKEY,
                        szTemp, sizeof szTemp);

   //while (!(bDone = !ItiDbGetQueryRow (hqry, &ppsz, &usState)) &&
   //       pii->usError == 0)

   usCnt = 0;
   while (aszResults[usCnt][0][0] != '\0')
      {
      apszValues [7] = aszResults [usCnt] [SI_KEY ];  /* Key  */
      apszValues [6] = aszResults [usCnt] [SI_ID  ];  /* ID   */
      apszValues [4] = aszResults [usCnt] [SI_SPYR];  /* SpecYear */

      ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery,
                           apszTokens, apszValues,
                           sizeof apszTokens / sizeof (PSZ));
      pii->usError = ItiDbExecSQLStatement (hhpLocSIK, szQuery);

      /* -- Show the user that something is happening. */
      szMark[0] = szStar[ (usCnt % 6) ];
      szMark[1] = '\0';
      fprintf(stderr, " %s \r", szMark);

      //if (usCnt % 32  == 0)
      //   puts (".");

      usCnt++;
      }

   // puts(" \n");
   //if (!bDone)
   //   ItiDbTerminateQuery (hqry);
   //
   //ItiMemFree (hhpLocSIK, apszValues [4]);

   if (NULL != hhpLocSIK)
      ItiMemDestroyHeap (hhpLocSIK);

   return pii->usError;
   }

