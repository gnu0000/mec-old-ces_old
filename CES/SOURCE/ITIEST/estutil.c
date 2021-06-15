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
 * EstUtil.c
 * Mark Hampton
 *
 * Various estimation utility functions.
 *
 * This module is not part of DS/Shell
 */

#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itiutil.h"
#include "..\include\itifmt.h"
#include "..\include\itiest.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


PSZ  pszWorkType;


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

PSZ EXPORT ItiEstGetJobWorkType (HHEAP hheap,
                                 PSZ   pszJobKey,
                                 PSZ   pszJobBreakdownKey)
   {
   char szTemp [1024];

   pszWorkType = NULL;

   if (pszJobKey == NULL || *pszJobKey == '\0')
      return NULL;

   if (pszJobBreakdownKey != NULL && *pszJobBreakdownKey != '\0')
      {
      // sprintf (szTemp,
      //          "SELECT WorkType "
      //          "FROM JobBreakdown "
      //          "WHERE JobKey = %s "
      //          "AND JobBreakdownKey = %s",
      //          pszJobKey, pszJobBreakdownKey);

      ItiStrCpy (szTemp,
               "/* ItiEstGetJobWorkType */ "
               "SELECT WorkType "
               "FROM JobBreakdown "
               "WHERE JobKey = ", sizeof szTemp);
      ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
      ItiStrCat (szTemp, " AND JobBreakdownKey = ", sizeof szTemp);
      ItiStrCat (szTemp, pszJobBreakdownKey, sizeof szTemp);

      pszWorkType = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);
      if (pszWorkType != NULL && atoi (pszWorkType) == 0)
         {
         ItiMemFree (hheap, pszWorkType);
         pszWorkType = NULL;
         }
      }

   if (pszWorkType == NULL)
      {
      ItiStrCpy (szTemp,
               "/* ItiEstGetJobWorkType */ "
               "SELECT WorkType "
               "FROM Job "
               "WHERE JobKey = ", sizeof szTemp);
      ItiStrCat (szTemp, pszJobKey, sizeof szTemp);

      pszWorkType = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);
      if (pszWorkType != NULL && atoi (pszWorkType) == 0)
         {
         ItiMemFree (hheap, pszWorkType);
         pszWorkType = NULL;
         }
      }

   return pszWorkType;
   }



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
 * the Area-Key.  If the Job's area cannot be found, a pointer to
 * the string "1" is returned.
 */

PSZ EXPORT ItiEstGetJobArea (HHEAP   hheap, 
                             PSZ     pszJobKey,
                             PSZ     pszAreaType,
                             PSZ     pszBaseDate)
   {
   char  szTemp [1024];
   PSZ   pszArea;
   BOOL  bGotBaseDate = FALSE;

   if (pszAreaType == NULL || *pszAreaType == '\0' || 
       pszJobKey == NULL   || *pszJobKey == '\0')
      return NULL;

   if (pszBaseDate == NULL)
      {
      pszBaseDate = ItiEstGetJobBaseDate (hheap, pszJobKey);
      if (pszBaseDate == NULL)
         return NULL;
      bGotBaseDate = TRUE;
      }

   pszArea = NULL;

   ItiStrCpy (szTemp,
            "SELECT AreaKey "
            "FROM AreaCounty, County, Job "
            "WHERE JobKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
   ItiStrCat (szTemp,
            " AND PrimaryCountyKey = County.CountyKey "
            "AND County.CountyKey = AreaCounty.CountyKey "
            "AND AreaTypeKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszAreaType, sizeof szTemp);
   ItiStrCat (szTemp, " AND AreaCounty.BaseDate = '", sizeof szTemp);
   ItiStrCat (szTemp, pszBaseDate, sizeof szTemp); 
   ItiStrCat (szTemp, "' ", sizeof szTemp);

   pszArea = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);

   if (bGotBaseDate)
      ItiMemFree (hheap, pszBaseDate);

   if (pszArea == NULL)
      pszArea = ItiStrDup (hheap, "1");

   return pszArea;
   }


DOUBLE ConvertRoundingEnum (USHORT usRound)
   {
   switch (usRound)
      {
      case ROUND_0_001 :
         return 1000.0;
         break;

      case ROUND_0_01  :
         return 100.0;
         break;

      case ROUND_0_1   :
         return 10.0;
         break;

      case ROUND_1     :
         return 1.0;
         break;

      case ROUND_10    :
         return 0.1;
         break;

      case ROUND_100   :
         return 0.01;
         break;

      case ROUND_1000  :
         return 0.001;
         break;

      case ROUND_10000 :
         return 0.0001;
         break;

      case ROUND_100000:
         return 0.00001;
         break;

      default:
         return 0;
         break;
      }
   }

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

PSZ EXPORT ItiEstRoundUnitPrice (HHEAP  hheap, 
                                 DOUBLE dPrice, 
                                 PSZ    pszStandardItemKey)
   {
   USHORT   usRound;
   PSZ      pszRoundingPrecision;
   static char  szTemp [400] = "";
   DOUBLE   dFactor = 0.0;

   if (pszStandardItemKey == NULL || *pszStandardItemKey == '\0')
      {
      if (dPrice == 0.0)
         dPrice = 1.0;
      // sprintf (szTemp, " $%.4lf ", dPrice);
      ItiStrDoubleToString (dPrice, szTemp, sizeof szTemp);

      return ItiStrDup (hheap, szTemp);
      }

   ItiStrCpy (szTemp,
            "/* ItiEst.estutil.ItiEstRoundUnitPrice */ "
            "SELECT RoundingPrecision "
            "FROM StandardItem "
            "WHERE StandardItemKey = ", sizeof szTemp);
   ItiStrCat (szTemp, pszStandardItemKey, sizeof szTemp);

   pszRoundingPrecision = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);
   if (pszRoundingPrecision == NULL || 
       *pszRoundingPrecision == '\0' ||
       !ItiStrToUSHORT (pszRoundingPrecision, &usRound))
      {
      //sprintf (szTemp, " $%.4lf ", dPrice);
      ItiStrDoubleToString (dPrice, szTemp, sizeof szTemp);

      return ItiStrDup (hheap, szTemp);
      }

   /* convert rounding precision to a double */
   dFactor = ConvertRoundingEnum (usRound);
   if (dFactor == 0)
      {
      //sprintf (szTemp, " $%.4lf ", dPrice);
      ItiStrDoubleToString (dPrice, szTemp, sizeof szTemp);
      return ItiStrDup (hheap, szTemp);
      }

   if (dFactor != 0.0)
      dPrice = floor (dPrice * dFactor + 0.5) / dFactor;

   /* for now, just give them the unrounded unit price */
   //sprintf (szTemp, " $%.4lf ", dPrice);
   ItiStrDoubleToString (dPrice, szTemp, sizeof szTemp);
   return ItiStrDup (hheap, szTemp);
   }


/*
 * ItiEstGetJobBaseDate returns the base date for the given job.
 *
 * Parameters: hheap          The heap to allocate from.
 *
 *             pszJobKey      The Job to check.
 *
 */

PSZ EXPORT ItiEstGetJobBaseDate (HHEAP hheap, PSZ pszJobKey)
   {
   CHAR szTemp [100] = "SELECT BaseDate FROM Job WHERE JobKey =            ";
                    //  012345678901234567890123456789012345678901234567890
   PSZ pszBaseDate, psz;
   INT i;

   if (pszJobKey == NULL || *pszJobKey == '\0')
      return NULL;

//   sprintf (szTemp,
//            "/* ItiEst.estutil.ItiEstGetJobBaseDate */ "
//            "SELECT BaseDate FROM Job WHERE JobKey = %s"
//            , pszJobKey);

      psz = pszJobKey;
      i = 39;
      do
         {
         i++;
         szTemp[i] = *psz;
         psz++;
         } while ((*psz) != '\0');

   pszBaseDate = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);
   return pszBaseDate;
   }




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

BOOL EXPORT ItiEstIsDavisBaconJob (HHEAP hheap, PSZ pszJobKey)
   {
   char szTemp [128];
   PSZ  psz;
   BOOL bDavisBacon;

   ItiStrCpy (szTemp,
              "/* ItiEstIsDavisBaconJob */ "
              "SELECT DavisBaconWages FROM Job WHERE JobKey = ",
              sizeof szTemp);
   ItiStrCat (szTemp, pszJobKey, sizeof szTemp);

   psz = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);
   if (psz == NULL)
      return FALSE;

   bDavisBacon = *psz == '1';
   ItiMemFree (hheap, psz);
   return bDavisBacon;
   }


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

PSZ EXPORT ItiEstQueryActiveEstMethod (HHEAP hheap,
                                       PSZ   pszJobKey,
                                       PSZ   pszJobItemKey)
   {
   char  szTemp [256];
   PSZ   psz;

   sprintf (szTemp,
            " SELECT IEM.EstimationMethodAbbreviation"
            " FROM JobItem JI, ItemEstimationMethod IEM"
            " WHERE JI.JobKey = %s"
            " AND JI.JobItemKey = %s"
            " AND JI.ItemEstimationMethodKey = IEM.ItemEstimationMethodKey",
            pszJobKey,
            pszJobItemKey);

   psz = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);
   return psz;
   }


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

BOOL EXPORT ItiEstIsActiveCostSheet (HHEAP hheap, 
                                     PSZ   pszJobKey,
                                     PSZ   pszJobItemKey, 
                                     PSZ   pszJobCostSheetKey)
   {
   char szTemp [1024];
   BOOL bRet;
   PSZ  psz;

   if (!ItiEstIsActive (hheap, pszJobKey, pszJobItemKey, EST_COST_BASED))
      return FALSE;

   sprintf (szTemp,
            "/* ItiEstIsActiveCostSheet */ "
            " SELECT JCS.JobCostSheetKey"
            " FROM JobCostBasedEstimate JCBE, JobCostSheet JCS"
            " WHERE JCBE.JobKey = %s"
            " AND JCBE.JobItemKey = %s"
            " AND JCBE.Active = 1"
            " AND JCBE.JobItemCostBasedEstimateKey = JCS.JobItemCostBasedEstimateKey"
            " AND JCS.JobKey = %s"
            " AND JCS.JobItemKey = %s"
            " AND JCS.JobCostSheetKey = %s",
            pszJobKey,
            pszJobItemKey,
            pszJobKey,
            pszJobItemKey,
            pszJobCostSheetKey);

   psz = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);
   bRet = psz ? (*psz != '\0') : FALSE;
   ItiMemFree (hheap, psz);

   return bRet;
   }


static PSZ apszEstMethod [EST_NUM_METHODS] =
   {
   "Adh",
   "Def",
   "Avg",
   "Reg",
   "Cst",
   };

/*
 * returns TRUE if given estimation method is the current one used for 
 * the item
 */

BOOL EXPORT ItiEstIsActive (HHEAP  hheap,
                            PSZ    pszJobKey,
                            PSZ    pszJobItemKey,
                            USHORT usMethod)
   {
   PSZ   psz;
   BOOL  bRet;

   if (usMethod >= EST_NUM_METHODS)
      return FALSE;

   psz = ItiEstQueryActiveEstMethod (hheap, pszJobKey, pszJobItemKey);
   if (!psz)
      return usMethod == EST_AD_HOC;

   bRet = ItiStrCmp (psz, apszEstMethod [usMethod]) == 0;
   ItiMemFree (hheap, psz);
   return bRet;
   }



