/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1992 by Info Tech, Inc.  All Rights Reserved.         |
|                                                                           |
|       This program module is part of DS/Shell (PC), Info Tech's           |
|       database interfaces for OS/2, a proprietary product of              |
|       Info Tech, Inc., no part of which may be reproduced or              |
|       transmitted in any form or by any means, electronic,                |
|       mechanical, or otherwise, including photocopying and recording      |
|       or in connection with any information storage or retrieval          |
|       system, without permission in writing from Info Tech, Inc.          |
|                                                                           |
+--------------------------------------------------------------------------*/


#define INCL_DOS
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "..\include\iti.h"
#include "..\include\itiutil.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itifmt.h"
#include "..\include\colid.h"
#include "..\include\itiimp.h"
#include "..\include\itiglob.h"
#include "..\include\itirptut.h"
#include "import.h"
// #include "util.h"
#include "load.h"
#include "highest.h"
#include "project.h"


#define NUM_FILES          10

#define DAYS_PER_MONTH     30.4375
#define DAYS_PER_YEAR      365.25


static char szAreaTypeKey [50] = "";
static char szImportDate [50] = "";
static char szImpDateCpy [50] = "";

/*
 * code by SuperKludge (tm)
 */
#define MAX_AREAS 512
#define AREA_SIZE   9

static BOOL EXPORT IsAreaInList (char      aszAreas [MAX_AREAS] [AREA_SIZE], 
                          PSZ       pszArea,
                          PUSHORT   pusNumAreas)
   {
   USHORT i;

   for (i=0; i < *pusNumAreas; i++)
      {
      if (ItiStrCmp (pszArea, aszAreas [i]) == 0)
         return TRUE;
      }

   if (*pusNumAreas >= MAX_AREAS - 1)
      return FALSE;

   *pusNumAreas += 1;
   ItiStrCpy (aszAreas [(*pusNumAreas)-1], pszArea, AREA_SIZE);
   return FALSE;
   }


static USHORT EXPORT ProcessCounties (PCSVFILEINFO  pcfi,
                               FILE          *pfArea,
                               FILE          *pfAreaCounty)
   {
   USHORT usCount;
   char   szTemp [254];
   char   szTemp2 [254];
   char   szTemp3 [254];
   char   aszAreas [MAX_AREAS][AREA_SIZE];
   USHORT usNumAreas = 0;

   if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp) ||
       !ItiStrToUSHORT (szTemp, &usCount))
      return ERROR_HIGHEST_CSV_ERROR;

   /* get a new AreaTypeKey (and AreaTypeID, etc) */
   sprintf (szAreaTypeKey, "%.4d%.2d%.2d-%.2d%.2d%.2d",
            (USHORT) pglobals->pgis->year,
            (USHORT) pglobals->pgis->month,
            (USHORT) pglobals->pgis->day,
            (USHORT) pglobals->pgis->hour,
            (USHORT) pglobals->pgis->minutes,
            (USHORT) pglobals->pgis->seconds);

   /* write out header line for the area types and areas */
   ItiCsvWriteLine (pfArea, "AreaType", "AreaTypeID", "Description", NULL);
   ItiCsvWriteLine (pfArea, "AreaType", szAreaTypeKey, "Area Type for PEMETH run", NULL);
   ItiCsvWriteLine (pfAreaCounty, "AreaCounty", "AreaTypeKey", "AreaKey", "CountyKey", NULL);
   ItiCsvWriteLine (pfArea, "Area", "AreaTypeKey", "AreaID", "Description", NULL);

   ItiFileSkipCSVLine (pcfi);
   while (usCount)
      {
      if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp) ||
          ItiStrICmp (szTemp, "COUNTY") != 0)
         return ERROR_HIGHEST_CSV_ERROR;

      /* get the county name (and toss it) */
      if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))
         return ERROR_HIGHEST_CSV_ERROR;

      /* get the district ID */
      if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))
         return ERROR_HIGHEST_CSV_ERROR;
      if (!IsAreaInList (aszAreas, szTemp, &usNumAreas))
         ItiCsvWriteLine (pfArea, "Area", szAreaTypeKey, szTemp, szTemp, NULL);

      /* get something, and toss it since we don't know what it is */
      if (!ItiFileGetCSVWord (pcfi, szTemp3, NULL, sizeof szTemp3))
         return ERROR_HIGHEST_CSV_ERROR;

      /* get the county ID */
      if (!ItiFileGetCSVWord (pcfi, szTemp2, NULL, sizeof szTemp2))
         return ERROR_HIGHEST_CSV_ERROR;

      ItiCsvWriteLine (pfAreaCounty, "AreaCounty",
                       szAreaTypeKey, szTemp, szTemp2, NULL);

      ItiFileSkipCSVLine (pcfi);
      usCount--;
      }
   return 0;
   }



static USHORT EXPORT ProcessAveragePrices (PCSVFILEINFO   pcfi,
                                    FILE           *pfAverages,
                                    FILE           *pfAveragePrices,
                                    PSZ            pszItem,
                                    PSZ            pszItemSpecYr, 
                                    PSZ            pszAveragesLine,
                                    BOOL           bBadAverage)
   {
   static CHAR   szTemp [250] = "";
   static CHAR   szTemp2 [250] = "";
   static CHAR   szBuffer [510] = "";
   static CHAR   szCpyItem [30] = "";
   static CHAR   szCpyItemSpecYr [30] = ""; 
   USHORT usCount, usError, i;
   BOOL   bLoaded, bNotNull;

   if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp) ||
       !ItiStrToUSHORT (szTemp, &usCount))
      return ERROR_HIGHEST_CSV_ERROR;

   ItiStrCpy (szCpyItem      , pszItem       ,sizeof szCpyItem);
   ItiStrCpy (szCpyItemSpecYr, pszItemSpecYr ,sizeof szCpyItemSpecYr);

   bLoaded = FALSE;
   usError = 0;
   ItiFileSkipCSVLine (pcfi);

   if (bBadAverage)
      {
      while (usCount--)
         ItiFileSkipCSVLine (pcfi);
      }
   else
      {
      while (usCount && usError == 0)
         {
         bLoaded = TRUE;
         
         if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp) ||
             ItiStrICmp (szTemp, "BIDAVERAGEELEMENT") != 0)
            return ERROR_HIGHEST_CSV_ERROR;
         
         /* this slows it down but sometime we have unexplained corup...  */
         i = 0;
         while ((szBuffer[i] != '\0') && (i < sizeof szBuffer))
            szBuffer[i++] = '\0';

         ItiStrCpy (szBuffer, "Weighted-Average-Unit-Price,", sizeof szBuffer);
         ItiStrCat (szBuffer, szImportDate, sizeof szBuffer);
         ItiStrCat (szBuffer, ",", sizeof szBuffer);
         //ItiStrCat (szBuffer, pszItem, sizeof szBuffer);
         ItiStrCat (szBuffer, szCpyItem, sizeof szBuffer);
         ItiStrCat (szBuffer, ",", sizeof szBuffer);
         //ItiStrCat (szBuffer, pszItemSpecYr, sizeof szBuffer);
         ItiStrCat (szBuffer, szCpyItemSpecYr, sizeof szBuffer);
         // ItiStrCat (szBuffer, ",", sizeof szBuffer);
         //   /* note 1: comma NOT added here */

         /* write out header */
         fprintf (pfAveragePrices, szBuffer);
           //fprintf (pfAveragePrices, "Weighted-Average-Unit-Price,%s,%s,%s,",
           //         szImportDate, pszItem, pszItemSpecYr);
         
         /* write out the average info data */
         bNotNull = TRUE;
         while (bNotNull)
            {
            if (ItiFileGetCSVWord(pcfi, szTemp, NULL, sizeof szTemp) == TRUE)
               {
               fputc (',', pfAveragePrices);  /* note 1: comma added here */
               ItiCsvMakeField (szTemp, szTemp2, sizeof szTemp2);
               if (szTemp2[0] != '\0')
                  fputs (szTemp2, pfAveragePrices);
               }
            else
               bNotNull = FALSE;
            }

         //while (ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))
         //   {
         //   ItiCsvMakeField (szTemp, szTemp2, sizeof szTemp2);
         //   fputs (szTemp2, pfAveragePrices);
         //   fputc (',', pfAveragePrices);
         //   }

         fputc ('\n', pfAveragePrices);
         
         ItiFileSkipCSVLine (pcfi);
         usCount--;
         }/* end of while (usCount... */ 
      }

   if (bLoaded)
      fputs (pszAveragesLine, pfAverages);

   return usError;
   }



static USHORT EXPORT DumpAdjustmentLine (PCSVFILEINFO   pcfi,
                                  PSZ            pszRecordType,
                                  PSZ            pszItem, 
                                  PSZ            pszItmSpecYear, 
                                  FILE           *pfOutFile)
   {
   char   szTemp [250] = "";
   char   szTemp2 [250] = "";
   double d;

   /* get the record key */
   if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))
      return ERROR_HIGHEST_CSV_ERROR;

   ItiCsvMakeField (szTemp, szTemp2, sizeof szTemp2);

   /* get the adjustment value */
   if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))
      return ERROR_HIGHEST_CSV_ERROR;

   /* convert it to a percent */
   d = ItiStrToDouble (szTemp);
   d = exp (d) - 1;

   fprintf (pfOutFile, "%s,%s,%s,%s,%s,%lf\n",
            pszRecordType, szImportDate, pszItem, pszItmSpecYear, szTemp2, d);
   return 0;
   }
                                  


static USHORT EXPORT ProcessRegression (PCSVFILEINFO   pcfi,
                                 FILE           *pfRegression,
                                 FILE           *pfAreaCoef,
                                 FILE           *pfSeasonCoef,
                                 FILE           *pfWorkTypeCoef,
                                 PSZ            pszMedianQuantity,
                                 PSZ            pszItem,
                                 PSZ            pszItemSpecYear, 
                                 PSZ            pszRegressionLine,
                                 BOOL           bBadRegression)
   {
   USHORT usCount;
   char   szTemp [256] = "";
   USHORT usError;
   char   szDate [20] = "";
   char   szIntercept [20] = "";
   char   szQuantity [20] = "";
   DOUBLE dMedianQuantity, dDate, dQuantity, dBase, dDays, dInflation, dQuantityAdjustment;
   BOOL   bLoaded;

   szIntercept [0] = szQuantity [0] = szDate [0] = '\0';
   usError = 0;
   bLoaded = FALSE;

   if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp) ||
       !ItiStrToUSHORT (szTemp, &usCount))
      return ERROR_HIGHEST_CSV_ERROR;

   ItiFileSkipCSVLine (pcfi);

   if (bBadRegression)
      {
      while (usCount--)
         ItiFileSkipCSVLine (pcfi);
      }
   else
      {
      while (usCount && usError == 0)
         {
         bLoaded = TRUE;
      
         if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp) ||
             ItiStrICmp (szTemp, "BIDREGRESSIONELEMENT") != 0)
            return ERROR_HIGHEST_CSV_ERROR;
      
         if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))
            return ERROR_HIGHEST_CSV_ERROR;
      
         if (ItiStrICmp (szTemp, "DATETYPE") == 0)
            {
            if (!ItiFileGetCSVWord (pcfi, szDate, NULL, sizeof szDate))
               return ERROR_HIGHEST_CSV_ERROR;
            if (!ItiFileGetCSVWord (pcfi, szDate, NULL, sizeof szDate))
               return ERROR_HIGHEST_CSV_ERROR;
            }
         else if (ItiStrICmp (szTemp, "INTERCEPTTYPE") == 0)
            {
            if (!ItiFileGetCSVWord (pcfi, szIntercept, NULL, sizeof szIntercept))
               return ERROR_HIGHEST_CSV_ERROR;
            if (!ItiFileGetCSVWord (pcfi, szIntercept, NULL, sizeof szIntercept))
               return ERROR_HIGHEST_CSV_ERROR;
            }
         else if (ItiStrICmp (szTemp, "QUANTITYTYPE") == 0)
            {
            if (!ItiFileGetCSVWord (pcfi, szQuantity, NULL, sizeof szQuantity))
               return ERROR_HIGHEST_CSV_ERROR;
            if (!ItiFileGetCSVWord (pcfi, szQuantity, NULL, sizeof szQuantity))
               return ERROR_HIGHEST_CSV_ERROR;
            }
         else if (ItiStrICmp (szTemp, "AREATYPE") == 0)
            {
            usError = DumpAdjustmentLine (pcfi, "AreaAdjustment",
                                          pszItem, pszItemSpecYear,
                                          pfAreaCoef);
            }
         else if (ItiStrICmp (szTemp, "SEASONTYPE") == 0)
            {
            usError = DumpAdjustmentLine (pcfi, "SeasonAdjustment",
                                          pszItem, pszItemSpecYear,
                                          pfSeasonCoef);
            }
         else if (ItiStrICmp (szTemp, "WORKTYPE") == 0)
            {
            usError = DumpAdjustmentLine (pcfi, "WorkTypeAdjustment",
                                          pszItem, pszItemSpecYear,
                                          pfWorkTypeCoef);
            }
      
         ItiFileSkipCSVLine (pcfi);
         usCount--;
         }
      }

   if (bLoaded)
      {
      /* calculate the base unit price */
      dDays = (DOUBLE) ItiRptUtDateDiff (szImportDate, "1/1/1960");
      dMedianQuantity = ItiStrToDouble (pszMedianQuantity);
      dDate = ItiStrToDouble (szDate);
      dQuantity = ItiStrToDouble (szQuantity);
      dBase = ItiStrToDouble (szIntercept);
      
      dBase = exp (dBase + 
                   dQuantity * log (dMedianQuantity) + 
                   (dDate * dDays) / 1000000.0);
      
      dInflation = exp ((DAYS_PER_YEAR * dDate) / 1000000.0) - 1;
      dQuantityAdjustment = exp (dQuantity * NATLOG_2) - 1;
      
      fprintf (pfRegression, "%s%lf,%lf,%lf\n", 
               pszRegressionLine, dBase, dInflation, dQuantityAdjustment);
      }

   return usError;
   }




static USHORT EXPORT ProcessBidCatElement (PCSVFILEINFO   pcfi, 
                                    PSZ            pszItem, 
                                    USHORT         usMaxItem, 
                                    PSZ            pszItemSpecYr, 
                                    USHORT         usMaxItemSpecYr, 
                                    PSZ            pszMedianQuantity, 
                                    USHORT         usMedianQuantity,
                                    PSZ            pszAveragesLine,
                                    PSZ            pszRegressionLine,
                                    USHORT         usLineMax,
                                    PBOOL          pbBadAverage,
                                    PBOOL          pbBadRegression) 
   {
   char     szTemp [256] = "";
   char     szTemp2 [256] = "";
 //  CHAR     szSpecYr [10] = "";
   USHORT   usSpecYear = 50;
   DOUBLE   dPrevAverage, dPrevRegression, dCurrent;

   if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))
      return ERROR_HIGHEST_CSV_ERROR;

   ItiCsvMakeField (szTemp, pszItem, usMaxItem);

   /* -- Get the SpecYear here. */
   if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp))
      return ERROR_HIGHEST_CSV_ERROR;
   /* now convert a 2 digit value to a 4 digit value. */
   ItiStrToSHORT (szTemp, &usSpecYear);
   if ((usSpecYear <= 99) && (usSpecYear > 50))
      usSpecYear = usSpecYear + 1900;
   else
      usSpecYear = usSpecYear + 2000;

   ItiStrUSHORTToString (usSpecYear, szTemp, sizeof szTemp);
   ItiCsvMakeField (szTemp, pszItemSpecYr, usMaxItemSpecYr);



   *pbBadAverage = *pbBadRegression = TRUE;
   /* build headers */
   sprintf (pszAveragesLine, "PEMETH-Average,%s,%s,%s,%s,", 
            szAreaTypeKey, szImportDate, pszItem, pszItemSpecYr);
   sprintf (pszRegressionLine, "PEMETH-Regression,%s,%s,%s,%s,", 
            szAreaTypeKey,szImportDate, pszItem, pszItemSpecYr);

   /* add the 5%ile */
   ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp);
   dPrevAverage = dPrevRegression = ItiStrToDouble (szTemp);
   ItiCsvMakeField (szTemp, szTemp2, sizeof szTemp2);
   ItiStrCat (pszAveragesLine, szTemp2, usLineMax);
   ItiStrCat (pszAveragesLine, ",", usLineMax);
   ItiStrCat (pszRegressionLine, szTemp2, usLineMax);
   ItiStrCat (pszRegressionLine, ",", usLineMax);

   /* add the 25%ile */
   ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp);
   dCurrent = ItiStrToDouble (szTemp);
   if (dCurrent != dPrevAverage)
      *pbBadAverage = FALSE;
   dPrevAverage = dCurrent;
   ItiCsvMakeField (szTemp, szTemp2, sizeof szTemp2);
   ItiStrCat (pszAveragesLine, szTemp2, usLineMax);
   ItiStrCat (pszAveragesLine, ",", usLineMax);

   /* add the 50%ile */
   ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp);
   dCurrent = ItiStrToDouble (szTemp);
   if (dCurrent != dPrevAverage)
      *pbBadAverage = FALSE;
   dPrevAverage = dCurrent;
   if (dCurrent != dPrevRegression)
      *pbBadRegression = FALSE;
   dPrevRegression = dCurrent;
   ItiStrCpy (pszMedianQuantity, szTemp, usMedianQuantity);
   ItiCsvMakeField (szTemp, szTemp2, sizeof szTemp2);
   ItiStrCat (pszAveragesLine, szTemp2, usLineMax);
   ItiStrCat (pszAveragesLine, ",", usLineMax);
   ItiStrCat (pszRegressionLine, szTemp2, usLineMax);
   ItiStrCat (pszRegressionLine, ",", usLineMax);

   /* add the 75%ile */
   ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp);
   dCurrent = ItiStrToDouble (szTemp);
   if (dCurrent != dPrevAverage)
      *pbBadAverage = FALSE;
   dPrevAverage = dCurrent;
   ItiCsvMakeField (szTemp, szTemp2, sizeof szTemp2);
   ItiStrCat (pszAveragesLine, szTemp2, usLineMax);
   ItiStrCat (pszAveragesLine, ",", usLineMax);

   /* add the 95%ile */
   ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp);
   dCurrent = ItiStrToDouble (szTemp);
   if (dCurrent != dPrevAverage)
      *pbBadAverage = FALSE;
   dPrevAverage = dCurrent;
   if (dCurrent != dPrevRegression)
      *pbBadRegression = FALSE;
   dPrevRegression = dCurrent;
   ItiCsvMakeField (szTemp, szTemp2, sizeof szTemp2);
   ItiStrCat (pszAveragesLine, szTemp2, usLineMax);
   ItiStrCat (pszAveragesLine, "\n", usLineMax);
   ItiStrCat (pszRegressionLine, szTemp2, usLineMax);
   ItiStrCat (pszRegressionLine, ",", usLineMax);
   
   ItiFileSkipCSVLine (pcfi);

   return 0;
   }



static USHORT EXPORT ProcessBidCat (PCSVFILEINFO   pcfi,
                             FILE           *pfAverages,
                             FILE           *pfAveragePrices,
                             FILE           *pfRegression,
                             FILE           *pfAreaCoef,
                             FILE           *pfSeasonCoef,
                             FILE           *pfWorkTypeCoef)
   {
   char   szTemp [256];
   char   szItem [60];
   char   szItemSpecYear [30];
   char   szMedianQuantity [250];
   char   szAveragesLine [256];
   char   szRegressionLine [256];
   USHORT usError;
   BOOL   bBadRegression, bBadAverage;

   /* write out header lines */
   fputs ("PEMETH-Average,Area-Type-Key,Import-Date,Standard-Item-Key,SpecYear,"
          "Fifth-Percentile,Twenty-Fifth-Percentile,Fiftieth-Percentile,"
          "Seventy-Fifth-Percentile,Ninety-Fifth-Percentile\n", pfAverages);

   fputs ("Weighted-Average-Unit-Price,Import-Date,Standard-Item-Key,SpecYear,"
          "Quantity-Level,Work-Type,Area-Key,Weighted-Average-Unit-Price\n", 
          pfAveragePrices);

   fputs ("PEMETH-Regression,AreaTypeKey,Import-Date,StandardItemKey,SpecYear,"
          "MinimumQuantity,MedianQuantity,MaximumQuantity,"
          "BaseUnitPrice,AnnualInflation,QuantityAdjustment\n", pfRegression);

   fputs ("Area-Adjustment,ImportDate,StandardItemKey,SpecYear,AreaKey,Adjustment\n",
          pfAreaCoef);

   fputs ("SeasonAdjustment,ImportDate,StandardItemKey,SpecYear,Season,Adjustment\n",
          pfSeasonCoef);

   fputs ("WorkTypeAdjustment,ImportDate,StandardItemKey,SpecYear,WorkType,Adjustment\n",
          pfWorkTypeCoef);

   do
      {
      /* get the standard item number */
      usError = ProcessBidCatElement (pcfi,
                                      szItem, sizeof szItem,
                                      szItemSpecYear, sizeof szItemSpecYear,
                                      szMedianQuantity, sizeof szMedianQuantity,
                                      szAveragesLine, szRegressionLine,
                                      sizeof szRegressionLine,
                                      &bBadAverage, &bBadRegression);

      /* process the averages */
      if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp) ||
          ItiStrICmp (szTemp, "BIDAVERAGE") != 0)
         return ERROR_HIGHEST_CSV_ERROR;
      usError = ProcessAveragePrices (pcfi, pfAverages, pfAveragePrices, 
                                      szItem, szItemSpecYear, 
                                      szAveragesLine, bBadAverage);

      if (usError == 0)
         {
         if (!ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp) ||
             ItiStrICmp (szTemp, "BIDREGRESSION") != 0)
            return ERROR_HIGHEST_CSV_ERROR;
         usError = ProcessRegression (pcfi, pfRegression, pfAreaCoef,
                                      pfSeasonCoef, pfWorkTypeCoef, 
                                      szMedianQuantity, szItem, szItemSpecYear, 
                                      szRegressionLine, bBadRegression);
         }
      } while (usError == 0 &&
               ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp) &&
               ItiStrICmp (szTemp, "BIDCATELEMENT") == 0);
   return usError;
   }




#define BUF_SIZE 1020
static USHORT EXPORT AppendFile (FILE *pfDest, PSZ pszSource)
   {
   FILE  *pfTemp;
   USHORT i;
   char  szTemp [BUF_SIZE + 1] = "";
   // char  szTemp [256];

   pfTemp = fopen (pszSource, "rt");
   if (pfTemp == NULL)
     return ERROR_CANT_OPEN_TEMP_FILE;

   while (fgets (szTemp, sizeof szTemp, pfTemp))
      {
      if ( !ItiStrIsCharInString('\n', szTemp) ) /* missing an end-of-line */
         ItiStrCat (szTemp, "\n", sizeof szTemp);

      fputs (szTemp, pfDest);
      for (i = 0; i < BUF_SIZE; i++)
          szTemp[i] = '\0';
      }

   fclose (pfTemp);
   unlink (pszSource);
   return 0;
   }
#undef BUF_SIZE 



USHORT EXPORT ConvertHighEstFile (PCSVFILEINFO pcfi, 
                           HHEAP        hheap, 
                           PSZ          pszFileName,
                           PSZ          pszNewFile,
                           USHORT       usNewFile)
   {
   PSZ      pszTemp;
   char     szTemp [255] = "";
   char     szArea [255] = "";
   char     szAreaCounty [255] = "";
   char     szAverages [255] = "";
   char     szAveragePrices [255] = "";
   char     szRegression [255] = "";
   char     szAreaCoef [255] = "";
   char     szSeasonCoef [255] = "";
   char     szWorkTypeCoef [255] = "";
   static   USHORT usCount = 0;
   USHORT   usError;
   FILE     *pfArea, *pfAreaCounty;
   FILE     *pfAverages, *pfAveragePrices;
   FILE     *pfRegression, *pfAreaCoef, *pfSeasonCoef, *pfWorkTypeCoef;

   /* build import date/time */
   usError = 0;
   sprintf (szImportDate, "%d/%d/%d %d:%.2d:00",
            (USHORT) pglobals->pgis->month,
            (USHORT) pglobals->pgis->day,
            (USHORT) pglobals->pgis->year,
            (USHORT) pglobals->pgis->hour,
            (USHORT) pglobals->pgis->minutes);

   ItiStrCpy (szImpDateCpy, szImportDate, sizeof szImpDateCpy );

   if (DosScanEnv ("TEMP", &pszTemp))
      if (DosScanEnv ("TMP", &pszTemp))
         pszTemp = ".";

   sprintf (szArea, "%s\\imp%.5d.%.3d", 
            pszTemp, pglobals->plis->pidCurrent, usCount++);
   pfArea = fopen (szArea, "wt");
   if (pfArea == NULL)
     return ERROR_CANT_OPEN_TEMP_FILE;
   ItiStrCpy (pszNewFile, szArea, usNewFile);

   sprintf (szAreaCounty, "%s\\imp%.5d.%.3d", 
            pszTemp, pglobals->plis->pidCurrent, usCount++);
   pfAreaCounty = fopen (szAreaCounty, "wt");
   if (pfAreaCounty == NULL)
     return ERROR_CANT_OPEN_TEMP_FILE;

   sprintf (szAverages, "%s\\imp%.5d.%.3d", 
            pszTemp, pglobals->plis->pidCurrent, usCount++);
   pfAverages = fopen (szAverages, "wt");
   if (pfAverages == NULL)
     return ERROR_CANT_OPEN_TEMP_FILE;

   sprintf (szAveragePrices, "%s\\imp%.5d.%.3d", 
            pszTemp, pglobals->plis->pidCurrent, usCount++);
   pfAveragePrices = fopen (szAveragePrices, "wt");
   if (pfAveragePrices == NULL)
     return ERROR_CANT_OPEN_TEMP_FILE;

   sprintf (szRegression, "%s\\imp%.5d.%.3d", 
            pszTemp, pglobals->plis->pidCurrent, usCount++);
   pfRegression = fopen (szRegression, "wt");
   if (pfRegression == NULL)
     return ERROR_CANT_OPEN_TEMP_FILE;

   sprintf (szAreaCoef, "%s\\imp%.5d.%.3d", 
            pszTemp, pglobals->plis->pidCurrent, usCount++);
   pfAreaCoef = fopen (szAreaCoef, "wt");
   if (pfAreaCoef == NULL)
     return ERROR_CANT_OPEN_TEMP_FILE;

   sprintf (szSeasonCoef, "%s\\imp%.5d.%.3d", 
            pszTemp, pglobals->plis->pidCurrent, usCount++);
   pfSeasonCoef = fopen (szSeasonCoef, "wt");
   if (pfSeasonCoef == NULL)
     return ERROR_CANT_OPEN_TEMP_FILE;

   sprintf (szWorkTypeCoef, "%s\\imp%.5d.%.3d", 
            pszTemp, pglobals->plis->pidCurrent, usCount++);
   pfWorkTypeCoef = fopen (szWorkTypeCoef, "wt");
   if (pfWorkTypeCoef == NULL)
     return ERROR_CANT_OPEN_TEMP_FILE;

   fputs ("DS/Shell Import File,1.00,CSV\n", pfArea);

   while (ItiFileGetCSVWord (pcfi, szTemp, NULL, sizeof szTemp) && 
          usError == 0)
      {
      if (ItiStrICmp (szTemp, "COUNTIES") == 0)
         usError = ProcessCounties (pcfi, pfArea, pfAreaCounty);
      else if (ItiStrICmp (szTemp, "BIDCATELEMENT") == 0)
         {
         usError = ProcessBidCat (pcfi, pfAverages, pfAveragePrices,
                                  pfRegression, pfAreaCoef, pfSeasonCoef,
                                  pfWorkTypeCoef);
         }
      else if (ItiStrICmp (szTemp, "PROJECTHEADER") == 0)
         usError = ProcessHeader (pcfi, pfAverages, pfAveragePrices);
      else
         ItiFileSkipCSVLine (pcfi);
      }

   fclose (pfAverages);
   fclose (pfAveragePrices);
   fclose (pfRegression);
   fclose (pfAreaCoef);
   fclose (pfSeasonCoef);
   fclose (pfWorkTypeCoef);
   fclose (pfAreaCounty);

   if (usError = AppendFile (pfArea, szAreaCounty))
      {
      fclose (pfArea);
      return usError;
      }

   if (usError = AppendFile (pfArea, szAverages))
      {
      fclose (pfArea);
      return usError;
      }

   if (usError = AppendFile (pfArea, szAveragePrices))
      {
      fclose (pfArea);
      return usError;
      }

   if (usError = AppendFile (pfArea, szRegression))
      {
      fclose (pfArea);
      return usError;
      }

   if (usError = AppendFile (pfArea, szAreaCoef))
      {
      fclose (pfArea);
      return usError;
      }

   if (usError = AppendFile (pfArea, szSeasonCoef))
      {
      fclose (pfArea);
      return usError;
      }

   if (usError = AppendFile (pfArea, szWorkTypeCoef))
      {
      fclose (pfArea);
      return usError;
      }

   fclose (pfArea);

   usError = ItiFileOpenCSV (pcfi, pszNewFile, "1.00", "1.01");
   return usError;
   }

