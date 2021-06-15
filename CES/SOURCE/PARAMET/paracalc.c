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
 * When a job is ceated, we need to first calculate 
 * Major Item Quantity Regression and then Major Item Price Regression.
 */

#define INCL_DOSMISC
#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itiutil.h"
#include "..\include\itibase.h"
#include "..\include\itimbase.h"
#include "..\include\itidbase.h"
#include "..\include\winid.h"
#include "..\include\itibase.h"
#include "..\include\itiest.h"

// include "..\include\itiutil.h"
#include "..\include\itimenu.h"
#include "..\include\dialog.h"
#include "..\include\colid.h"
#include "..\include\itierror.h"
#include "..\include\itifmt.h"

#include "initcat.h"
#include "paramet.h"
#include "..\include\paracalc.h"

#include <stdio.h>
#include <ctype.h>
#include <math.h>

static CHAR szTmp[2000] = "";
static CHAR szBigTemp[2000] = "";

#define  FACTOR_SIZE  14
static   CHAR szMedianQty   [32] = "5";
static   CHAR szMedianQtyRaw[32] = "";
static   CHAR szBasePrice   [2 * FACTOR_SIZE] = "";
static   CHAR szBasePriceAdj   [2 * FACTOR_SIZE] = "";

static   CHAR szSlope     [ FACTOR_SIZE + 1 ] = "";  
static   CHAR szIntercept [ FACTOR_SIZE + 1 ] = "";  

static   CHAR szSlopeQ     [ FACTOR_SIZE + 1 ] = "";  
static   CHAR szInterceptQ [ FACTOR_SIZE + 1 ] = "";  
static   CHAR szS [500] = "";

static   CHAR szUpper     [ 2 * FACTOR_SIZE + 1 ] = "";  
static   CHAR szLower     [ 2 * FACTOR_SIZE + 1 ] = "";  
static   CHAR szRSQ       [ 2 * FACTOR_SIZE + 1 ] = "";  
static   CHAR szSSxy      [ 2 * FACTOR_SIZE + 1 ] = "";  
static   CHAR szSSxx      [ 2 * FACTOR_SIZE + 1 ] = "";  
static   CHAR szSSyy      [ 2 * FACTOR_SIZE + 1 ] = "";  


static   CHAR szQtyValue [30] = "";

static   CHAR szBlankStr[30] = "            ";


static USHORT gus1 = 0;
static USHORT gus2 = 0;
static USHORT gus3 = 0;


static DOUBLE  EXPORT  GetQuantityAdjustment (HHEAP  hheap, 
                                     PSZ    *ppszTokens, 
                                     PSZ    *ppszValues, 
                                     USHORT usNumTokens,
                                     USHORT usVariable);

static DOUBLE  EXPORT GetPriceAdjustment (HHEAP  hheap, 
                                  PSZ    *ppszTokens, 
                                  PSZ    *ppszValues, 
                                  USHORT usNumTokens,
                                  USHORT usVariable);

static BOOL  EXPORT CalcJBMIQuantities (HHEAP  hheap, 
                                PSZ    *ppszTokens, 
                                PSZ    *ppszValues, 
                                USHORT usNumTokens);



static BOOL  EXPORT CalcJBMIPrices (HHEAP  hheap, 
                            PSZ    *ppszTokens, 
                            PSZ    *ppszValues, 
                            USHORT usNumTokens);

USHORT EXPORT ParaCalcDetermineVorAorL (PSZ pszMajorItemKey);


HQRY EXPORT ExecQuery (HHEAP   hheap,
                USHORT  usResID,
                USHORT  usTokenID,
                PSZ     *ppszTokens,
                PSZ     *ppszValues,
                USHORT  usNumTokens,
                PUSHORT pusNumRows)
   {
   char szQuery [2046], szTemp [2046];
   
   ItiMbQueryQueryText (hmodMe, usResID, usTokenID, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, ppszTokens, 
                        ppszValues, usNumTokens);
   return ItiDbExecQuery (szQuery, hheap, 0, 0, 0, pusNumRows, &usNumTokens);
   }



PSZ EXPORT GetRow1Col1 (HHEAP    hheap,
                 USHORT   usResID,
                 USHORT   usTokenID,
                 PSZ      *ppszTokens,
                 PSZ      *ppszValues,
                 USHORT   usNumTokens)
   {
   char szQuery [2040], szTemp [2040];

   ItiMbQueryQueryText (hmodMe, usResID, usTokenID, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, ppszTokens, 
                        ppszValues, usNumTokens);
   return ItiDbGetRow1Col1 (szQuery, hheap, 0, 0, 0);
   }




BOOL EXPORT SQLExec (HHEAP    hheap,
              USHORT   usResID,
              USHORT   usTokenID,
              PSZ      *ppszTokens,
              PSZ      *ppszValues,
              USHORT   usNumTokens)
   {
   char szQuery [2040], szTemp [2040];

   ItiMbQueryQueryText (hmodMe, usResID, usTokenID, szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery, ppszTokens, 
                        ppszValues, usNumTokens);
   return 0 == ItiDbExecSQLStatement (hheap, szQuery);
   }




/*
 * This function populates the JobBreakdownMajorItem table for a 
 * given job breakdown.  Any existing JobBreakdownMajorItems are deleted.
 */


 
#define JOBKEY                   0
#define JOBBREAKDOWNKEY          1
#define PROFILEKEY               2
#define BASEDATE                 3
#define QUANTITY                 4
#define PRICE                    5
#define MAJORITEMKEY             6
#define TEMP                     7






BOOL EXPORT MakeJobBreakdownMajorItem (HHEAP hheap, 
                                PSZ   pszJobKey, 
                                PSZ   pszJobBreakdownKey)
   {
   BOOL rc;
   PSZ *ppszTokens, *ppszValues;
//   CHAR szBlankStr[30] = "                  ";

#define   NUM_TOKENS   8

   ppszTokens = ItiStrMakePPSZ (hheap, NUM_TOKENS, "JobKey", 
                                "JobBreakdownKey", "ProfileKey", "BaseDate",
                                "Quantity", "Price", "MajorItemKey", "Temp");
   if (ppszTokens == NULL)
      return FALSE;


   ppszValues = ItiStrMakePPSZ (hheap, NUM_TOKENS, pszJobKey, 
                                pszJobBreakdownKey, szBlankStr, szBlankStr,
                                szBlankStr, szBlankStr,
                                szBlankStr, szBlankStr);
   if (ppszValues == NULL)
      {
      ItiFreeStrArray (hheap, ppszTokens, NUM_TOKENS);
      return FALSE;
      }

   /* get the profile key */
   ppszValues [PROFILEKEY] = GetRow1Col1 (hheap, ITIRID_CALC, GET_PROFILE_KEY, 
                                 ppszTokens, ppszValues, NUM_TOKENS);

   /* get the base date */
   ppszValues [BASEDATE] = ItiEstGetJobBaseDate (hheap, pszJobKey);
                    
   /* delete the old info */
   rc = SQLExec (hheap, ITIRID_CALC, DELETE_JBMI, ppszTokens, ppszValues, NUM_TOKENS);
   
   /* make the new info */
   rc = SQLExec (hheap, ITIRID_CALC, MAKE_JBMI, ppszTokens, ppszValues, NUM_TOKENS);
   
   /* calculate the quantities */
   CalcJBMIQuantities (hheap, ppszTokens, ppszValues, NUM_TOKENS);

   /* calculate the prices */
   CalcJBMIPrices (hheap, ppszTokens, ppszValues, NUM_TOKENS);

   /* calculate the extensions */
   rc = SQLExec (hheap, ITIRID_CALC, CALC_JBMI_EXT, ppszTokens, ppszValues, NUM_TOKENS);

   /* update the JobBreakdown table */
// because of a bug in SQL server, we have to do this in 2 steps 
//   SQLExec (hheap, ITIRID_CALC, UPDATE_JB, ppszTokens, ppszValues, NUM_TOKENS);
   ppszValues [TEMP] = GetRow1Col1 (hheap, ITIRID_CALC, UPDATE_JB1, 
                                    ppszTokens, ppszValues, NUM_TOKENS);
   ItiExtract (ppszValues [TEMP], ",");
   rc = SQLExec (hheap, ITIRID_CALC, UPDATE_JB2, ppszTokens, ppszValues, NUM_TOKENS);

   /* update the buffers */
   ItiDbUpdateBuffers ("JobBreakdown");
   ItiDbUpdateBuffers ("JobBreakdownMajorItem");

   /* clean up */
   ppszValues [JOBKEY] = NULL;
   ppszValues [JOBBREAKDOWNKEY] = NULL;
   ItiFreeStrArray (hheap, ppszTokens, NUM_TOKENS);
   //ItiFreeStrArray (hheap, ppszValues, NUM_TOKENS);
   return rc;
   }

#define MAX_MAJOR_ITEMS    50

static BOOL  EXPORT CalcJBMIQuantities (HHEAP  hheap, 
                                PSZ    *ppszTokens, 
                                PSZ    *ppszValues, 
                                USHORT usNumTokens)
   {
   HQRY     hqry;
   USHORT   usNumRows, usError;
   //USHORT   usNumCols, uState;
   USHORT    i, x, usQryID = USE_LENGTH;
   PSZ      *ppsz; 
   BOOL     bMore = FALSE;
   CHAR     aszMajorItemKey [MAX_MAJOR_ITEMS+1]   [10];
   DOUBLE   dQuantity [MAX_MAJOR_ITEMS+1];
   char     szQuan [15] = " ";
   char     szTemp [500] = " ";
   char     szTemp2 [500] = " ";

   for (i = 0; i < MAX_MAJOR_ITEMS ; i++)
      {
      aszMajorItemKey  [i][0] = '\0';
      //aszMajorItemUnit [i][0] = '\0';
      dQuantity [i] = 10.0;
      }

   hqry = ExecQuery (hheap, ITIRID_CALC, GETQTY_MIKEY,
                     ppszTokens, ppszValues, usNumTokens, &usNumRows);
   if (hqry == NULL)
      return FALSE;
   else
      i = 0;

   /* get the major item keys */ 
   while (ItiDbGetQueryRow (hqry, &ppsz, &usError))
      {
      ItiStrCpy (aszMajorItemKey [i], ppsz [0], sizeof aszMajorItemKey [i]);
      //ItiStrCpy (aszMajorItemUnit[i], ppsz [1], sizeof aszMajorItemUnit[i]);

      ItiFreeStrArray (hheap, ppsz, usNumRows);
      i++;
      }
   usNumRows = i;
 
 
    /* -- Now set the Quantities for each major item. */
    //for (x = 0; x < usNumRows; x++)
    x = 0;
    while (x < usNumRows)
      {
      usQryID = ParaCalcDetermineVorAorL ( aszMajorItemKey[x] );

      ppszValues [MAJORITEMKEY] = aszMajorItemKey [x];

    //   switch (usQryID)
    //      {
    //      case PARACALC_VOLUME:
    //         usQryID = USE_VOL;
    //         break;
    // 
    //      case PARACALC_AREA:
    //         usQryID = USE_AREA;
    //         break;
    // 
    //      default:
    //         usQryID = USE_LENGTH;
    //         break;
    //      }/* end switch */
    // 
    // 
    //   hqry = ExecQuery (hheap, ITIRID_CALC, usQryID,
    //                     ppszTokens, ppszValues, usNumTokens, &usNumCols);
    //   if (hqry == NULL)
    //      return FALSE;
    // 
    //   /* get the raw, unadjusted quantity */
    //   ItiDbGetQueryRow (hqry, &ppsz, &usError);
    //   if ((ppsz != NULL) && (ppsz[0] != NULL) && ((*ppsz) != '\0'))
    //      {
    //      ItiStrCpy (ppszValues [QUANTITY], ppsz[0], sizeof szBlankStr);
    //      ItiExtract (ppszValues[QUANTITY], ",");
    //      // dQuantity [x] = ItiStrToDouble (ppsz [0]);
    //      ItiFreeStrArray (hheap, ppsz, usNumCols);
    //         /*--- Force the completion of the query. ---*/
    //      while (ItiDbGetQueryRow (hqry, &ppsz, &usError))
    //         ItiFreeStrArray (hheap, ppsz, usNumCols);
    //      //    /* apply qualitative adjustments */
    //      //   //  dAdjustment = 0;
    //      //   //  for (j=0; j < NUM_VARIABLES; j++)
    //      //   //     dQuantity [i] *= 1.0 + GetQuantityAdjustment (hheap, ppszTokens,
    //      //   //                                                   ppszValues,
    //      //   //                                                   usNumTokens, j);
    //      }
    //   else
    //      ItiStrCpy (ppszValues [QUANTITY], " 10.50 ", sizeof ppszValues [QUANTITY]);
    //  SQLExec (hheap, ITIRID_CALC, UPDATEQUANTITY, ppszTokens, ppszValues, usNumTokens);

      ppszValues [QUANTITY] = ResetJBMIQty (hheap
                                            ,ppszValues [JOBKEY]
                                            ,ppszValues [JOBBREAKDOWNKEY]
                                            ,ppszValues [MAJORITEMKEY]
                                            , usQryID
                                            , "", TRUE);
      ItiStrCpy (szQuan, ppszValues [QUANTITY], sizeof szQuan);
      ItiExtract (szQuan, ",");
      ParametMajorItemNewQtyPrice 
              (hheap
              ,ppszValues [MAJORITEMKEY]
              ,ppszValues [JOBKEY]
              ,ppszValues [JOBBREAKDOWNKEY]
              ,szQuan
              , " ", TRUE);

      x++;
      } /* end of while (x... */



   ppszValues [MAJORITEMKEY] = NULL;
   //ppszValues [QUANTITY] = NULL;
   return TRUE;
   } /* End of Function */



static BOOL  EXPORT CalcJBMIPrices (HHEAP  hheap, 
                            PSZ    *ppszTokens, 
                            PSZ    *ppszValues, 
                            USHORT usNumTokens)
   {
   HQRY     hqry;
   USHORT   usNumRows, usError, i, j;
   PSZ      *ppsz;
   PSZ      apszMajorItemKey [MAX_MAJOR_ITEMS];
   DOUBLE   dPrice [MAX_MAJOR_ITEMS], dAdjustment;
   CHAR     szTemp [50] = "";
   BOOL     rc;

   hqry = ExecQuery (hheap, ITIRID_CALC, GETPRICE,
                     ppszTokens, ppszValues, usNumTokens, &usNumRows);
   if (hqry == NULL)
      return FALSE;
   i = 0;
   /* get the raw, unadjusted quantity */
   while (ItiDbGetQueryRow (hqry, &ppsz, &usError))
      {
      apszMajorItemKey [i] = ItiStrDup (hheap, ppsz [0]);
      dPrice [i] = ItiStrToDouble (ppsz [1]);
      dAdjustment = ItiStrToDouble (ppsz [2]); 
      dPrice [i] = (exp (dPrice [i]) / NATLOG_2) * dAdjustment;
      ItiFreeStrArray (hheap, ppsz, usNumRows);
      i++;
      }

   usNumRows = i;
   ppszValues [PRICE] = szTemp;
   for (i = 0; i < usNumRows; i++)
      {
      ppszValues [MAJORITEMKEY] = apszMajorItemKey [i];

      /* apply qualitative adjustments */
      dAdjustment = 0;
      for (j=0; j < NUM_VARIABLES; j++)
         dAdjustment += GetPriceAdjustment (hheap, ppszTokens, ppszValues,
                                            usNumTokens, j);

      dPrice [i] *= dAdjustment;
      sprintf (szTemp, "$%f", dPrice [i]);

      rc = SQLExec (hheap, ITIRID_CALC, UPDATEPRICE, ppszTokens, ppszValues,
               usNumTokens);

      if (apszMajorItemKey[i]) 
         ItiMemFree (hheap, apszMajorItemKey [i]);
      }
   ppszValues [MAJORITEMKEY] = NULL;
   ppszValues [PRICE] = NULL;
   return TRUE;
   }




static DOUBLE EXPORT GetQuantityAdjustment (HHEAP  hheap, 
                                     PSZ    *ppszTokens, 
                                     PSZ    *ppszValues, 
                                     USHORT usNumTokens,
                                     USHORT usVariable)
   {
   PSZ      psz;
   DOUBLE   d;

   if (usVariable >= NUM_VARIABLES)
      return 0; 

   psz = GetRow1Col1 (hheap, ITIRID_CALC, GETQADJUST_FIRST + usVariable,
                      ppszTokens, ppszValues, usNumTokens);
   if (psz == NULL)
      return 0;

   d = ItiStrToDouble (psz);
   ItiMemFree (hheap, psz);
   return d;
   }


static DOUBLE EXPORT  GetPriceAdjustment (HHEAP  hheap, 
                                  PSZ    *ppszTokens, 
                                  PSZ    *ppszValues, 
                                  USHORT usNumTokens,
                                  USHORT usVariable)
   {
   PSZ      psz;
   DOUBLE   d;

   if (usVariable >= NUM_VARIABLES)
      return 0; 

   psz = GetRow1Col1 (hheap, ITIRID_CALC, GETPADJUST_FIRST + usVariable,
                      ppszTokens, ppszValues, usNumTokens);
   if (psz == NULL)
      return 0;

   d = ItiStrToDouble (psz);
   ItiMemFree (hheap, psz);
   return d;
   }



PSZ EXPORT ResetJBMIQty (HHEAP hhpLoc, PSZ pszJobKey, PSZ pszJobBrkKey, PSZ pszJobBrkMIKey, USHORT usVAL, PSZ pszConv, BOOL bChgDB)
   {                                       
   HHEAP hhpTmp;
   PSZ  *ppszRLocPNames;                  
   PSZ  *ppszRLocParams;
   PSZ  pszResQty = NULL;
   USHORT us, uRt, usNcnt = 0;
   BOOL bIsNeg = FALSE;
   CHAR szVorAorL[250] = "";
   CHAR szVolVal[] = " 0  ";
   CHAR szAreaVal[] = " 0  ";
   CHAR szLenVal[] = " 0  ";
   CHAR szVolConv[15] = " 1.0 ";
   CHAR szAreaConv[15] = " 1.0 ";
   CHAR szLenConv[15] = " 1.0 ";


   hhpTmp = ItiMemCreateHeap (3000);

   /* -- set the value of Vol or Area or Length here. */
   if ((usVAL > PARACALC_VOLUME) || (usVAL < PARACALC_LENGTH))
      {
      us = ParaCalcDetermineVorAorL (pszJobBrkMIKey);
      }
   else
      us = usVAL;

do {
   switch (us)
      {
      case PARACALC_VOLUME :
         ItiStrCpy(szVorAorL,
                   "((Length * (1760.0+(uJ.UnitType * (-760.0))) ) * (PavementWidth / (3.0+(uJ.UnitType * (-2.0))) ) * (PavementDepth/ (36.0+(uJ.UnitType * (964.0))) )) "
                   , sizeof szVorAorL);
         ItiStrCpy(szVolVal, " 1 ", sizeof szVolVal);
         if ((pszConv != NULL) && ((*pszConv) != '\0') && ((*pszConv) != '<'))
            ItiStrCpy(szVolConv, pszConv, sizeof szVolConv);
         else
            {
            ItiStrCpy(szTmp
                      ,"/* ResetJBMIQty */ select ConversionFactorVolume from JobBreakdownMajorItem where JobKey = "
                      ,sizeof szTmp);
            ItiStrCat(szTmp, pszJobKey, sizeof szTmp);
            ItiStrCat(szTmp, " and JobBreakdownKey = ", sizeof szTmp);
            ItiStrCat(szTmp, pszJobBrkKey, sizeof szTmp);
            ItiStrCat(szTmp, " and MajorItemKey = ", sizeof szTmp);
            ItiStrCat(szTmp, pszJobBrkMIKey, sizeof szTmp);

            ItiErrWriteDebugMessage (szTmp);
            pszResQty = ItiDbGetRow1Col1 (szTmp, hhpTmp, 0,0,0);
            if ((pszResQty != NULL) && ((*pszResQty) != '\0'))
               ItiStrCpy(szVolConv, pszResQty, sizeof szVolConv);
            else
               ItiStrCpy(szVolConv, " 1.0 ", sizeof szVolConv);
            }
         break;

      case PARACALC_AREA :
         ItiStrCpy(szVorAorL,
            " ((Length * (1760.0+(uJ.UnitType * (-760.0)))) * (PavementWidth / (3.0+(uJ.UnitType * (-2.0)))) ) "
          , sizeof szVorAorL);
         ItiStrCpy(szAreaVal, " 1 ", sizeof szAreaVal);
         if ((pszConv != NULL) && ((*pszConv) != '\0') && ((*pszConv) != '<'))
            ItiStrCpy(szAreaConv, pszConv, sizeof szAreaConv);
         else
            {
            ItiStrCpy(szTmp
                      ,"select ConversionFactorArea from JobBreakdownMajorItem where JobKey = "
                      ,sizeof szTmp);
            ItiStrCat(szTmp, pszJobKey, sizeof szTmp);
            ItiStrCat(szTmp, " and JobBreakdownKey = ", sizeof szTmp);
            ItiStrCat(szTmp, pszJobBrkKey, sizeof szTmp);
            ItiStrCat(szTmp, " and MajorItemKey = ", sizeof szTmp);
            ItiStrCat(szTmp, pszJobBrkMIKey, sizeof szTmp);

            ItiErrWriteDebugMessage (szTmp);
            pszResQty = ItiDbGetRow1Col1 (szTmp, hhpTmp, 0,0,0);
            if ((pszResQty != NULL) && ((*pszResQty) != '\0'))
               ItiStrCpy(szAreaConv, pszResQty, sizeof szAreaConv);
            else
               ItiStrCpy(szAreaConv, " 1.0 ", sizeof szAreaConv);
            }

         break;

      case PARACALC_LENGTH :
      default:
         ItiStrCpy(szVorAorL,
            " (Length * (1760.0+(uJ.UnitType * (-760.0)))) "
            , sizeof szVorAorL);
         ItiStrCpy(szLenVal, " 1 ", sizeof szLenVal);
         if ((pszConv != NULL) && ((*pszConv) != '\0') && ((*pszConv) != '<'))
            ItiStrCpy(szLenConv, pszConv, sizeof szLenConv);
         else
            {
            ItiStrCpy(szTmp
                      ,"select ConversionFactorLength from JobBreakdownMajorItem where JobKey = "
                      ,sizeof szTmp);
            ItiStrCat(szTmp, pszJobKey, sizeof szTmp);
            ItiStrCat(szTmp, " and JobBreakdownKey = ", sizeof szTmp);
            ItiStrCat(szTmp, pszJobBrkKey, sizeof szTmp);
            ItiStrCat(szTmp, " and MajorItemKey = ", sizeof szTmp);
            ItiStrCat(szTmp, pszJobBrkMIKey, sizeof szTmp);

            ItiErrWriteDebugMessage (szTmp);
            pszResQty = ItiDbGetRow1Col1 (szTmp, hhpTmp, 0,0,0);
            if ((pszResQty != NULL) && ((*pszResQty) != '\0'))
               ItiStrCpy(szLenConv, pszResQty, sizeof szLenConv);
            else
               ItiStrCpy(szLenConv, " 1.0 ", sizeof szLenConv);
            }

         break;
      }/* end of switch (us */


   /* -- ======================== Compute new Qty, but don't chg DB here. */
   ppszRLocPNames = ItiStrMakePPSZ (hhpTmp, 11, "VorAorL", "JobKey", "JobBreakdownKey", "MajorItemKey", "Vol", "ConvFacVolume", "Area", "ConvFacArea", "Len", "ConvFacLength", NULL);
   ppszRLocParams = ItiStrMakePPSZ (hhpTmp, 11, szVorAorL,pszJobKey, pszJobBrkKey, pszJobBrkMIKey,      szVolVal,   szVolConv, szAreaVal,  szAreaConv, szLenVal,       szLenConv, NULL);

   ItiMbQueryQueryText (hmodMe, ITIRID_CALC, QUERY_JMIQTYREGS,
                        szBigTemp, sizeof szBigTemp);
   uRt = ItiStrReplaceParams (szTmp, szBigTemp,
             sizeof szTmp, ppszRLocPNames, ppszRLocParams, 20);

   // -- Get and set szQtyValue value here. 
   ItiErrWriteDebugMessage (szTmp);
   pszResQty = ItiDbGetRow1Col1 (szTmp, hhpTmp, 0,0,0);
   if ((pszResQty != NULL) && ((*pszResQty) != '\0'))
      {
      ItiStrCpy (szQtyValue, pszResQty, sizeof szQtyValue);
      bIsNeg = ((*pszResQty) == '-');
      if (bIsNeg)
         {
         usNcnt++;
         if (usNcnt == 1)
            us = gus2;
         else
            us = gus3;

         }/* end if (bIsNeg... */
      }
   else
      ItiStrCpy (szQtyValue, " 1.0 ", sizeof szQtyValue);

} while (bIsNeg && (usNcnt < 3) );

   if (bChgDB == FALSE)
      {
      ItiMemDestroyHeap (hhpTmp);
      return szQtyValue;
      }



   ppszRLocPNames = ItiStrMakePPSZ (hhpTmp, 11, "VorAorL", "JobKey", "JobBreakdownKey", "MajorItemKey", "Vol", "ConvFacVolume", "Area", "ConvFacArea", "Len", "ConvFacLength", NULL);
   ppszRLocParams = ItiStrMakePPSZ (hhpTmp, 11, szVorAorL,pszJobKey, pszJobBrkKey, pszJobBrkMIKey,      szVolVal,   szVolConv, szAreaVal,  szAreaConv, szLenVal,       szLenConv, NULL);


   ItiMbQueryQueryText (hmodMe, ITIRID_CALC, UPDATE_JMIQTYREGS,
                        szBigTemp, sizeof szBigTemp);
   uRt = ItiStrReplaceParams (szTmp, szBigTemp,
             sizeof szTmp, ppszRLocPNames, ppszRLocParams, 20);
   //ItiDbExecSQLStatement (hhpLoc, szTmp);
   ItiDbExecSQLStatement (hhpTmp, szTmp);


   /* -- ===================================== Reset the price extensions. */
   ItiMbQueryQueryText (hmodMe, ITIRID_CALC, CALC_JBMI_EXT,
                        szBigTemp, sizeof szBigTemp);
   uRt = ItiStrReplaceParams (szTmp, szBigTemp,
             sizeof szTmp, ppszRLocPNames, ppszRLocParams, 20);
   //ItiDbExecSQLStatement (hhpLoc, szTmp);
   ItiDbExecSQLStatement (hhpTmp, szTmp);

   /* -- Now reset the totals in Job Breakdown & Job windows. */
   ItiMbQueryQueryText (hmodMe, ITIRID_CALC, UPDATE_JB,
                        szBigTemp, sizeof szBigTemp);
   uRt = ItiStrReplaceParams (szTmp, szBigTemp,
             sizeof szTmp, ppszRLocPNames, ppszRLocParams, 20);
   //ItiDbExecSQLStatement (hhpLoc, szTmp);
   ItiDbExecSQLStatement (hhpTmp, szTmp);
   
   ItiMbQueryQueryText (hmodMe, ITIRID_CALC, UPDATE_J,
                        szBigTemp, sizeof szBigTemp);
   uRt = ItiStrReplaceParams (szTmp, szBigTemp,
             sizeof szTmp, ppszRLocPNames, ppszRLocParams, 20);
   //ItiDbExecSQLStatement (hhpLoc, szTmp);
   ItiDbExecSQLStatement (hhpTmp, szTmp);

   ItiMemDestroyHeap (hhpTmp);

   return szQtyValue;
   }/* End of Function ResetJBMIQty */



void EXPORT  ResetMIValues (HWND hwnd)
   {
   HHEAP    hheap;
   PSZ      pszJobKey = NULL;
   PSZ      pszJobBrkKey = NULL;
   PSZ      pszJobBrkMIKey = NULL;
//   PSZ      *ppszLocalPNames;
//   PSZ      *ppszLocalParams;
   USHORT   uRow;
   HWND     hwndList;
   CHAR     szJobKeyLoc[12] = "";
   CHAR     szJobBrkKeyLoc[12] = "";
   CHAR     szMIKeyLoc[12] = "";
   CHAR     szVorAorL[250] = "";

   ItiErrWriteDebugMessage("MSG: Resetting Job parametric values: ----------------------------------------");

   hwndList = WinWindowFromID (hwnd, JobBreakdownMajorItemL);

   hheap        = (HHEAP)QW (hwndList, ITIWND_HEAP, 0, 0, 0);
   uRow         = (UM)   QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);
   pszJobKey      = (PSZ)  QW (hwndList, ITIWND_DATA, 0, uRow, cJobKey);
   pszJobBrkKey   = (PSZ)  QW (hwndList, ITIWND_DATA, 0, uRow, cJobBreakdownKey);
   pszJobBrkMIKey = (PSZ)  QW (hwndList, ITIWND_DATA, 0, uRow, cMajorItemKey);

   ItiStrCpy (szJobKeyLoc,    pszJobKey, sizeof szJobKeyLoc);
   ItiStrCpy (szJobBrkKeyLoc, pszJobBrkKey, sizeof szJobBrkKeyLoc);
   ItiStrCpy (szMIKeyLoc,     pszJobBrkMIKey, sizeof szMIKeyLoc);

   if (szJobKeyLoc[0] != '\0') 
      ItiErrWriteDebugMessage(szJobKeyLoc);
   else
      {
      ItiErrWriteDebugMessage(" NO szJobKeyLoc was gotten! ");
      return;
      }

   if (szJobBrkKeyLoc[0] != '\0') 
      ItiErrWriteDebugMessage(szJobBrkKeyLoc);
   else      
      {
      ItiErrWriteDebugMessage(" NO szJobBrkKeyLoc was gotten! ");
      return;
      }

   if (szMIKeyLoc[0] != '\0') 
      ItiErrWriteDebugMessage(szMIKeyLoc);
   else      
      {
      ItiErrWriteDebugMessage(" NO szMIKeyLoc was gotten! ");
      return;
      }

   ItiWndSetHourGlass (TRUE);

   ResetJBMIQty (hheap, szJobKeyLoc, szJobBrkKeyLoc, szMIKeyLoc, 0, NULL, TRUE);

   ItiWndSetHourGlass (FALSE);
   ItiErrWriteDebugMessage("MSG: Finished reset of Job parametric values. --------------------------------");
   }/* End Function ResetMIValues */




PSZ EXPORT ParametMajorItemMedianQty (HHEAP hhp, BOOL bFormatted, PSZ pszMajorItemKey, PSZ pszWhereClause)
   {
   HHEAP hhpL = NULL;
   PSZ pszResultString = " 0 ";
   CHAR szTemp [900] = "";
   CHAR szLow [20] = "1";
   CHAR szUp [20] = "1";
   CHAR szQry  [1020] = "";
   CHAR szQuery [1020] = "";
   CHAR szAdjPct      [32] = "";
//   CHAR szSlope     [ FACTOR_SIZE + 1 ] = "";  
//   CHAR szIntercept [ FACTOR_SIZE + 1 ] = "";  
   HQRY     hqry;
   BOOL  bHaveMedian = FALSE;
   USHORT   usNumCols, usError, usCnt, us, usHhpSize;
   PSZ      *ppszDCS;
#define HHP_MIN  800
   ItiErrWriteDebugMessage("MSG: In ParametMajorItemMedianQty -------------------------");

   /* -- Figure out the median quantity (szMedianQty) of an historical major item. */
   ItiStrCpy(szQry, " SELECT Quantity, Quantity ", sizeof szQry);
   ItiMbQueryQueryText (hmodMe, ITIRID_CALC, GET_S_I, szTemp, sizeof szTemp);
   ItiStrCat (szQry, szTemp, sizeof szQry);               
   ItiStrCat (szQry, pszMajorItemKey, sizeof szQry);               
   ItiStrCat (szQry, "  ", sizeof szQry);
   ItiStrCat (szQry, pszWhereClause, sizeof szQry);
   ItiStrCat (szQry, " order by 1 ", sizeof szQry);//qtys from low to high

   hhpL = ItiMemCreateHeap (MAX_HEAP_SIZE);
   hqry = ItiDbExecQuery (szQry, hhpL, hmodMe, ITIRID_CALC,
                          GET_QTYS, &usNumCols, &usError);
   if (hqry == NULL)
      return NULL;
   usCnt = ItiDbGetQueryCount(szQry, &usNumCols, &usError);

   // usHhpSize = HHP_MIN;
   us = 1;
   while (ItiDbGetQueryRow (hqry, &ppszDCS, &usError))
      {  
      ItiErrWriteDebugMessage(ppszDCS[0]);

      /* Check heap space */
    //  usHhpSize = ItiMemQueryAvail (hhpL);
    //  if (usHhpSize < HHP_MIN) 
    //     {
    //     ItiMemDestroyHeap (hhpL);
    //     hhpL = ItiMemCreateHeap (2000);
    //     }

      if (us == usCnt) /* odd count */
         {
         ItiStrCpy(szMedianQty,    ppszDCS[0], sizeof szMedianQty);
         ItiStrCpy(szMedianQtyRaw, ppszDCS[1], sizeof szMedianQtyRaw);//unformatted, ie. no commas
         bHaveMedian = TRUE;
         }

      if ((us + 1) == usCnt) /* even count */
         {
         ItiStrCpy(szLow, ppszDCS[1], sizeof szLow);
         }

      if ((us - 1) == usCnt) /* even count */
         {
         ItiStrCpy(szUp, ppszDCS[1], sizeof szUp);
         }

      usCnt--;
      us++;
      ItiFreeStrArray (hhpL, ppszDCS, usNumCols);
      }


   if (bHaveMedian == FALSE)
      {
      ItiStrCpy(szQry, " SELECT (", sizeof szQry);
      ItiStrCat(szQry, szUp, sizeof szQry);
      ItiStrCat(szQry, " + ", sizeof szQry);
      ItiStrCat(szQry, szLow, sizeof szQry);
      ItiStrCat(szQry, ") / 2 , ( ", sizeof szQry);
      ItiStrCat(szQry, szUp, sizeof szQry);
      ItiStrCat(szQry, " + ", sizeof szQry);
      ItiStrCat(szQry, szLow, sizeof szQry);
      ItiStrCat(szQry, ") / 2  ", sizeof szQry);

      ppszDCS = ItiDbGetRow1 (szQry, hhp, hmodMe, ITIRID_CALC,
                              GET_QTYS, &usNumCols);
      if ((ppszDCS != NULL) && (ppszDCS[0][0] != '\0'))
         {
         ItiStrCpy(szMedianQty,    ppszDCS[0], sizeof szMedianQty);
         ItiStrCpy(szMedianQtyRaw, ppszDCS[1], sizeof szMedianQtyRaw);//unformatted, ie. no commas
         }
      else
        ItiErrWriteDebugMessage ("ERROR In ParametMajorItemMedianQty, even count.");

      }

   ItiMemDestroyHeap (hhpL);

   if (bFormatted)
      return &szMedianQty[0];
   else
      return &szMedianQtyRaw[0];
   }/* End of Function */




PSZ EXPORT ParametMajorItemSlopeP (HHEAP hhp, PSZ pszMajorItemKey, PSZ pszWhereClause)
   {
   CHAR szTemp [900] = "";
   CHAR szQry  [1020] = "";
   PSZ   pszSlope;

   /* -- Compute the Slope. */
   ItiMbQueryQueryText (hmodMe, ITIRID_CALC, GET_P_SLOPE, szQry, sizeof szQry);
   ItiMbQueryQueryText (hmodMe, ITIRID_CALC, GET_S_I, szTemp, sizeof szTemp);
   ItiStrCat (szQry, szTemp, sizeof szQry);               
   ItiStrCat (szQry, pszMajorItemKey, sizeof szQry);               
   ItiStrCat (szQry, " ", sizeof szQry);
   ItiStrCat (szQry, pszWhereClause, sizeof szQry);

   pszSlope = ItiDbGetRow1Col1(szQry,hhp,0,0,0);
   if ((pszSlope != NULL) && ((*pszSlope) != '\0'))
      ItiStrCpy(szSlope, pszSlope, FACTOR_SIZE);
   else
      ItiStrCpy(szSlope, " 1 ", FACTOR_SIZE);

   return &szSlope[0];
   }/* End of Function ParametMajorItemSlopeP */


PSZ EXPORT ParametMajorItemInterceptP (HHEAP hhp, PSZ pszMajorItemKey, PSZ pszWhereClause)
   {
   CHAR szTemp [900] = "";
   CHAR szQry  [1020] = "";
   PSZ      pszIntercept, pszSlopeLoc;

   pszSlopeLoc = ParametMajorItemSlopeP (hhp, pszMajorItemKey, pszWhereClause);

   /* --  Compute the  Price Intercept point. */
   //ItiMbQueryQueryText (hmodMe, ITIRID_CALC, GET_P_INTERCEPT, szQry, sizeof szQry);
   ItiStrCpy (szQry, " select AVG(LOG(WeightedAverageUnitPrice)) - ((", sizeof szQry);
   ItiStrCat (szQry, pszSlopeLoc, sizeof szQry);
   ItiStrCat (szQry, ") * AVG(LOG(Quantity))) ", sizeof szQry);

   ItiMbQueryQueryText (hmodMe, ITIRID_CALC, GET_S_I, szTemp, sizeof szTemp);
   ItiStrCat (szQry, szTemp, sizeof szQry);               
   ItiStrCat (szQry, pszMajorItemKey, sizeof szQry);
   ItiStrCat (szQry, " ", sizeof szQry);
   ItiStrCat (szQry, pszWhereClause, sizeof szQry);

   pszIntercept = ItiDbGetRow1Col1(szQry,hhp,0,0,0);
   if ((pszIntercept != NULL) && ((*pszIntercept) != '\0'))
      ItiStrCpy(szIntercept, pszIntercept, FACTOR_SIZE);
   else
      ItiStrCpy(szIntercept, " 1 ", FACTOR_SIZE);

   return &szIntercept[0];
   }/* End of Function */



PSZ EXPORT ParametMajorItemNewQtyPrice
       (HHEAP hhp, PSZ pszMajorItemKey, PSZ pszJobKey, PSZ pszJobBrkdwnKey,
        PSZ pszQty, PSZ pszWhereClause, BOOL bResetDB)
   {
   CHAR szQry  [1020] = "";
   PSZ      pszLocIntercept, pszLocSlope, pszRes;
   CHAR szErrQty[] = " 1.0 ";

   /* -- 1st, Need to set the R squared value for the other functions. */
   ParametMajorItemRSQ (hhp, pszMajorItemKey, pszWhereClause, 1);


   pszLocSlope = ParametMajorItemSlopeP (hhp, pszMajorItemKey, pszWhereClause);
   pszLocIntercept = ParametMajorItemInterceptP (hhp, pszMajorItemKey, pszWhereClause);

   //ItiMbQueryQueryText (hmodMe, ITIRID_CALC, GET_BASE_PRICE, szTemp, sizeof szTemp);
   // sprintf(szQry, szTemp, pszLocIntercept, pszLocSlope, pszQty);
   ItiStrCpy (szQry, "/* ParametMajorItemNewQtyPrice */ SELECT EXP( ", sizeof szQry);
   ItiStrCat (szQry, pszLocIntercept, sizeof szQry);
   ItiStrCat (szQry, " + (( ", sizeof szQry);
   ItiStrCat (szQry, pszLocSlope, sizeof szQry);
   ItiStrCat (szQry, ") * LOG( ", sizeof szQry);

   if ((pszQty != NULL) && ((*pszQty) != '\0'))
      ItiStrCat (szQry, pszQty, sizeof szQry);
   else
      {
      ItiStrCat (szQry, szErrQty, sizeof szQry);
      ItiErrWriteDebugMessage
        ("*** WARNING MSG: ParametMajorItemNewQtyPrice was given a NULL quantity; using a value of 1.0 for the quantity.");
      }

   ItiStrCat (szQry, ")) ) ", sizeof szQry);

   pszRes = ItiDbGetRow1Col1(szQry,hhp,0,0,0);
   if ((pszRes != NULL) && ((*pszRes) != '\0'))
      ItiStrCpy(szBasePrice, pszRes, FACTOR_SIZE + FACTOR_SIZE);
   else
      ItiStrCpy(szBasePrice, "0.0", 5);


   //ItiMbQueryQueryText (hmodMe, ITIRID_CALC, GET_BASE_PRICE_QTY_ADJ, szTemp, sizeof szTemp);
   //sprintf(szQry, szTemp, pszMajorItemKey, pszMajorItemKey, pszJobKey, pszMajorItemKey);
   ItiStrCpy (szQry, "SELECT SUM(PriceAdjustment) "  
      "FROM MajorItemPriceAdjustment A, Job "
      "WHERE Job.BaseDate >= "
      "(select MAX(BaseDate) from MajorItemPriceAdjustment "
           " where MajorItemKey = ", sizeof szQry);
   ItiStrCat (szQry, pszMajorItemKey, sizeof szQry);
   ItiStrCat (szQry,   ") AND A.BaseDate >= "
      "(select MAX(BaseDate) from MajorItemPriceAdjustment "
           " where MajorItemKey = ", sizeof szQry);
   ItiStrCat (szQry, pszMajorItemKey, sizeof szQry);
   ItiStrCat (szQry,   ") AND Job.JobKey = ", sizeof szQry);
   ItiStrCat (szQry, pszJobKey, sizeof szQry);
   ItiStrCat (szQry,   "AND A.MajorItemKey = ", sizeof szQry);
   ItiStrCat (szQry, pszMajorItemKey, sizeof szQry);


   pszRes = ItiDbGetRow1Col1(szQry,hhp,0,0,0);
   if ((pszRes != NULL) && ((*pszRes) != '\0'))
      ItiStrCpy(szBasePriceAdj, pszRes, FACTOR_SIZE + FACTOR_SIZE);
   else
      ItiStrCpy(szBasePriceAdj, "0.0", 5);


   if (bResetDB)
      { /* Reset the database value. */
      ItiStrCpy (szQry,
                 "/* ParametMajorItemNewQtyPrice */"
                 " UPDATE JobBreakdownMajorItem SET UnitPrice = "
                 , sizeof szQry);
      ItiStrCat (szQry, szBasePrice, sizeof szQry);
      ItiStrCat (szQry, " + (", sizeof szQry);
      ItiStrCat (szQry, szBasePrice, sizeof szQry);
      ItiStrCat (szQry, " * ", sizeof szQry);
      ItiStrCat (szQry, szBasePriceAdj, sizeof szQry);
      ItiStrCat (szQry, ") WHERE JobKey = ", sizeof szQry);
      ItiStrCat (szQry, pszJobKey, sizeof szQry);
      ItiStrCat (szQry, " AND JobBreakdownKey = ", sizeof szQry);
      ItiStrCat (szQry, pszJobBrkdwnKey, sizeof szQry);
      ItiStrCat (szQry, " AND MajorItemKey = ", sizeof szQry);
      ItiStrCat (szQry, pszMajorItemKey, sizeof szQry);
      ItiDbExecSQLStatement (hhp, szQry);

      ItiStrCpy (szQry,
                 "/* ParametMajorItemNewQtyPrice */"
                 " UPDATE JobBreakdownMajorItem "
                 " SET ExtendedAmount = Quantity * UnitPrice  "
                 , sizeof szQry);
      ItiStrCat (szQry, " WHERE JobKey = ", sizeof szQry);
      ItiStrCat (szQry, pszJobKey, sizeof szQry);
      ItiStrCat (szQry, " AND JobBreakdownKey = ", sizeof szQry);
      ItiStrCat (szQry, pszJobBrkdwnKey, sizeof szQry);
      ItiStrCat (szQry, " AND MajorItemKey = ", sizeof szQry);
      ItiStrCat (szQry, pszMajorItemKey, sizeof szQry);
      ItiDbExecSQLStatement (hhp, szQry);
      }

   return &szBasePrice[0];
   }/* End of Function */



USHORT EXPORT ParaCalcDetermineVorAorL (PSZ pszMajorItemKey)
   {                 /* hard coded for Colorado keys for now. */
   LONG lKey = 0L;
   HHEAP hhpLoc;
   USHORT us = PARACALC_LENGTH;
   DOUBLE dAreaR = 0.0;
   DOUBLE dLenR = 0.0;
   DOUBLE dVolR = 0.0;
   DOUBLE dTmp = 0.0;
   PSZ pszRvalue = NULL;
   PSZ pszID = NULL;
   char szQtmp[100] = " select '    MajorItem '+MajorItemID from MajorItem where MajorItemKey = ";

   if ((pszMajorItemKey == NULL) || ((*pszMajorItemKey) == '\0'))
      return PARACALC_VOLUME;

   hhpLoc = ItiMemCreateHeap (2000);
   if (hhpLoc == NULL)
      return PARACALC_LENGTH;

   ItiErrWriteDebugMessage ("\n  *-*-* Determine R squared Value for: VOLUME ");
   pszRvalue = ParametMajorItemRSQ (hhpLoc, pszMajorItemKey, " ", PARACALC_VOLUME);
   if (pszRvalue != NULL)
      {
      dVolR = ItiStrToDouble (pszRvalue);
      dVolR = fabs(dVolR);
      }

   ItiErrWriteDebugMessage ("\n  *-*-* Determine R squared Value for: AREA ");
   pszRvalue = ParametMajorItemRSQ (hhpLoc, pszMajorItemKey, " ", PARACALC_AREA);
   if (pszRvalue != NULL)
      {
      dAreaR = ItiStrToDouble (pszRvalue);
      dAreaR = fabs(dAreaR);
      }

   ItiErrWriteDebugMessage ("\n  *-*-* Determine R squared Value for: LENGTH ");
   pszRvalue = ParametMajorItemRSQ (hhpLoc, pszMajorItemKey, " ", PARACALC_LENGTH);
   if (pszRvalue != NULL)
      {
      dLenR = ItiStrToDouble (pszRvalue);
      dLenR = fabs(dLenR);
      }

   // figure out which one to use here!
   if (dLenR > dAreaR)
      {
      dTmp = dLenR;
      us = PARACALC_LENGTH;
      gus1 = PARACALC_LENGTH;
      gus2 = PARACALC_AREA;
      }
   else
      {
      dTmp = dAreaR;
      us = PARACALC_AREA;
      gus1 = PARACALC_AREA;
      gus2 = PARACALC_LENGTH;
      }

   if (dVolR > dTmp)
      {
      us = PARACALC_VOLUME;
      if (gus1 == PARACALC_AREA)
         {
         gus2 = PARACALC_AREA;
         gus3 = PARACALC_LENGTH;
         }
      else
         {
         gus3 = PARACALC_AREA;
         gus2 = PARACALC_LENGTH;
         }

      gus1 = PARACALC_VOLUME;
      }
   else /* Tmp is greater than volume */
      {
      if (gus1 == PARACALC_AREA)
         {
         if (dLenR > dVolR)
            {
            gus3 = PARACALC_VOLUME;
            gus2 = PARACALC_LENGTH;
            }
         else
            {
            gus2 = PARACALC_VOLUME;
            gus3 = PARACALC_LENGTH;
            }
         }
      else /* gus1 is _LENGTH */
         {
         if (dAreaR > dVolR)
            {
            gus2 = PARACALC_AREA;
            gus3 = PARACALC_VOLUME;
            }
         else
            {
            gus2 = PARACALC_VOLUME;
            gus3 = PARACALC_AREA;
            }
         } /* end of else gus1 is _LENGTH clause */

      }/* end of if (dVolR > dTmp... else clause */

   ItiStrCat (szQtmp, pszMajorItemKey, sizeof szQtmp);               
   pszID = ItiDbGetRow1Col1(szQtmp,hhpLoc,0,0,0);

   if (us == PARACALC_LENGTH)
      {
      ItiErrWriteDebugMessage ("    Based on R-squared value, LENGTH was choosen for ");
      }

   if (us == PARACALC_AREA)
      {
      ItiErrWriteDebugMessage ("    Based on R-squared value, AREA was choosen for ");
      }

   if (us == PARACALC_VOLUME)
      {
      ItiErrWriteDebugMessage ("    Based on R-squared value, VOLUME was choosen for ");
      }

   if (NULL != pszID)
      ItiErrWriteDebugMessage (pszID);

   ItiErrWriteDebugMessage ("  *-*-*  \n\n");

   ItiMemDestroyHeap (hhpLoc);

   return us;
   }/* end of function */


 


/////////////////////////////////

/* -- Returns SSxy used in the square of the Pearson product moment. 
 *
 *                SSxy =  ‰(XY) - [(‰X)(‰Y)]/n
 */
PSZ EXPORT SSxy (HHEAP hhp, PSZ pszMajorItemKey, PSZ pszWhereClause, USHORT usVorAorL)
   {
   CHAR szTemp [1500] = "";
   CHAR szQry  [1500] = "";
   PSZ   pszUpper = NULL;
   USHORT usi;

   for (usi = 0; usi < sizeof szS; usi++)
      szS[usi] = '\0';

   /* -- Find the V or A or L value for the %s substitution via szS. */
               /* 5280 / 3 = 1760 ; conversion for miles into yards. */
               /* The (HP.UnitType * (-760.0)) selects between english */ 
               /* or metric conversion factor.  */
   switch (usVorAorL)
      {
      case PARACALC_VOLUME:
         ItiStrCpy (szS,
                    "((ProposalLength * (1760.0+(HP.UnitType * (-760.0))) ) * (AveragePavementWidth / (3.0+(HP.UnitType * (-2.0))) ) * (AveragePavementDepth/ (36.0+(HP.UnitType * (964.0))) )) "
                    , sizeof szS);
         break;

      case PARACALC_AREA:
         ItiStrCpy (szS,
            " ((ProposalLength * (1760.0+(HP.UnitType * (-760.0)))) * (AveragePavementWidth / (3.0+(HP.UnitType * (-2.0)))) ) "
            , sizeof szS);
         break;

      case PARACALC_LENGTH:
      default:
         ItiStrCpy (szS, " (ProposalLength * (1760.0+(HP.UnitType * (-760.0)))) ", sizeof szS);
         break;
      }/* end of switch */


   /* -- Next, Compute the   (‰XY) - (‰X)(‰Y)/n   */
   ItiStrCpy(szQry, " select ( ( SUM( ", sizeof szQry);
   ItiStrCat (szQry,                 szS, sizeof szQry);               
   ItiStrCat (szQry,               " * Quantity) ) - (SUM( ", sizeof szQry);
   ItiStrCat (szQry, szS, sizeof szQry);               
   ItiStrCat (szQry, ") * SUM(Quantity) ) /count(*) ) ",sizeof szQry);

   ItiMbQueryQueryText (hmodMe, ITIRID_CALC, GET_S_I, szTemp, sizeof szTemp);
   ItiStrCat (szQry, szTemp, sizeof szQry);               
   ItiStrCat (szQry, pszMajorItemKey, sizeof szQry);               
   ItiStrCat (szQry, " ", sizeof szQry);
   ItiStrCat (szQry, pszWhereClause, sizeof szQry);

   pszUpper = ItiDbGetRow1Col1(szQry,hhp,0,0,0);
   if ((pszUpper != NULL) && ((*pszUpper) != '\0'))
      {
      ItiStrCpy (szSSxy, pszUpper, sizeof szSSxy);
      ItiExtract (szSSxy, ",");
      }
   else
      ItiStrCpy(szSSxy, " 0.0 ", sizeof szSSxy);

   return &szSSxy[0];
   }/* End of Function SSxy */




/* -- Returns SSyy used in the square of the Pearson product moment. 
 *    Array Y is of the historical Quantities of the job.
 *    The n is the count of the quantities.
 *
 *                SSyy =  ‰(Y˝) - (‰Y)˝/n
 */
PSZ EXPORT SSyy (HHEAP hhp, PSZ pszMajorItemKey, PSZ pszWhereClause, USHORT usVorAorL)
   {
   CHAR szTemp [1500] = "";
   CHAR szQry  [1500] = "";
   PSZ   pszUpper = NULL;
   PSZ   pszLower = NULL;
   USHORT usi;

   for (usi = 0; usi < sizeof szS; usi++)
      szS[usi] = '\0';

   ItiStrCpy (szQry,
    " select SUM (Quantity * Quantity) - ((SUM(Quantity) * SUM(Quantity)) / count(*) ) "
    , sizeof szQry);

   ItiMbQueryQueryText (hmodMe, ITIRID_CALC, GET_S_I, szTemp, sizeof szTemp);
   ItiStrCat (szQry, szTemp, sizeof szQry);               
   ItiStrCat (szQry, pszMajorItemKey, sizeof szQry);               
   ItiStrCat (szQry, " ", sizeof szQry);
   ItiStrCat (szQry, pszWhereClause, sizeof szQry);


   pszLower = ItiDbGetRow1Col1 (szQry, hhp, 0,0,0);
   if ((pszLower != NULL) && ((*pszLower) != '\0'))
      {
      ItiStrCpy(szSSyy, pszLower, sizeof szSSyy);
      ItiExtract (szSSyy, ",");
      }
   else
      ItiStrCpy(szSSyy, "1.00 ", sizeof szSSyy);

   return &szSSyy[0];
   }/* End of Function SSyy */


    

/* -- Returns SSxx used in the square of the Pearson product moment. 
 *    Array X is either Volume, Area, or Length of the job.
 *
 *                SSxx =  ‰(X˝) - (‰X)˝/n
 */
PSZ EXPORT SSxx (HHEAP hhp, PSZ pszMajorItemKey, PSZ pszWhereClause, USHORT usVorAorL)
   {
   CHAR szTemp [1500] = "";
   CHAR szQry  [1500] = "";
   PSZ   pszUpper = NULL;
   PSZ   pszLower = NULL;
   USHORT usi;

   /* Init the clause string. */
   for (usi = 0; usi < sizeof szS; usi++)
      szS[usi] = '\0';

   /* -- Find the V or A or L value for the %s substitution via szS. */
               /* 5280 / 3 = 1760 ; conversion for miles into yards. */
               /* The (HP.UnitType * (-760.0)) selects between english */ 
               /* or metric conversion factor.  */
   switch (usVorAorL)
      {
      case PARACALC_VOLUME:
         ItiStrCpy (szS,
                    "((ProposalLength * (1760.0+(HP.UnitType * (-760.0))) ) * (AveragePavementWidth / (3.0+(HP.UnitType * (-2.0))) ) * (AveragePavementDepth/ (36.0+(HP.UnitType * (964.0))) )) "
                    , sizeof szS);
         break;

      case PARACALC_AREA:
         ItiStrCpy (szS,
            " ((ProposalLength * (1760.0+(HP.UnitType * (-760.0)))) * (AveragePavementWidth / (3.0+(HP.UnitType * (-2.0)))) ) "
            , sizeof szS);
         break;

      case PARACALC_LENGTH:
      default:
         ItiStrCpy (szS, " (ProposalLength * (1760.0+(HP.UnitType * (-760.0)))) ", sizeof szS);
         break;
      }/* end of switch */


   ItiStrCpy (szQry, " select ( ( SUM( ", sizeof szQry);
   ItiStrCat (szQry,  szS, sizeof szQry);               
   ItiStrCat (szQry, " * ", sizeof szQry);
   ItiStrCat (szQry,  szS, sizeof szQry);               
   ItiStrCat (szQry, " ) ) - (SUM( ", sizeof szQry);
   ItiStrCat (szQry, szS, sizeof szQry);               
   ItiStrCat (szQry, ") * SUM ( ",sizeof szQry);
   ItiStrCat (szQry, szS, sizeof szQry);               
   ItiStrCat (szQry, " ) ) /count(*) ) ",sizeof szQry);


   ItiMbQueryQueryText (hmodMe, ITIRID_CALC, GET_S_I, szTemp, sizeof szTemp);
   ItiStrCat (szQry, szTemp, sizeof szQry);               
   ItiStrCat (szQry, pszMajorItemKey, sizeof szQry);               
   ItiStrCat (szQry, " ", sizeof szQry);
   ItiStrCat (szQry, pszWhereClause, sizeof szQry);


   pszLower = ItiDbGetRow1Col1 (szQry, hhp, 0,0,0);
   if ((pszLower != NULL) && ((*pszLower) != '\0'))
      {
      ItiStrCpy(szSSxx, pszLower, sizeof szSSxx);
      ItiExtract (szSSxx, ",");
      }
   else
      ItiStrCpy(szSSxx, "1.00 ", sizeof szSSxx);

   return &szSSxx[0];
   }/* End of Function SSxx */




/* -- Returns the Pearson product moment coefficient of correlation r. 
 *  This is a measure of the strength of the linear relationship between 
 *  the two variables, in this case Quantity (y) and the job dimensions (x).
 *
 *                SSxy                       (‰(XY) - (‰X*‰Y)/n)
 *      r = ________________   =   ________________________________________
 *            _____________          _____________________________________
 *           ˚ SSxx * SSyy          ˚(‰(X˝) - (‰X)˝/n) * (‰(Y˝) - (‰Y)˝/n)
 *
 *    y                       y                      y
 *    ≥           ˛    ˛      ≥      ˛    ˛          ≥ ˛   ˛          
 *    ≥        ˛ ˛  ˛         ≥   ˛    ˛ ˛  ˛        ≥    ˛   ˛ ˛  
 *    ≥       ˛   ˛  ˛        ≥       ˛   ˛  ˛       ≥       ˛   ˛  
 *    ≥     ˛   ˛             ≥     ˛   ˛  ˛         ≥         ˛   ˛
 *    ≥  ˛   ˛                ≥      ˛      ˛        ≥            ˛   ˛
 *    ¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒx    ¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒx   ¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒx
 *        Positive r              r close to 0           Negative r
 *
 *    Quantity increases as    Little or no linear    Quantity decreases as
 *    the job parameters       relationship between   the job parameters
 *    (volume or area or       quantity and job       (volume or area or
 *    length) increase.        parameters.            length) increase. 
 */
PSZ EXPORT ParametMajorItemRSQ (HHEAP hhp, PSZ pszMajorItemKey, PSZ pszWhereClause, USHORT usVorAorL)
   {
   CHAR  szQry[120] = "";
   PSZ   pszSSxx = NULL;
   PSZ   pszSSxy = NULL;
   PSZ   pszSSyy = NULL;
   PSZ   pszLower = NULL;

   pszSSxy = SSxy (hhp, pszMajorItemKey, pszWhereClause, usVorAorL);
   pszSSxx = SSxx (hhp, pszMajorItemKey, pszWhereClause, usVorAorL);
   pszSSyy = SSyy (hhp, pszMajorItemKey, pszWhereClause, usVorAorL);

   /* -- Test for nulls. */
   if ((pszSSxy == NULL) || (pszSSxx == NULL) || (pszSSyy == NULL))
      {  /* -- Problems so assume no correlationship. */
      ItiErrWriteDebugMessage ("Error Msg: ParametMajorItemRSQ computed one or more null sub values. ");
      ItiStrCpy(szRSQ, " 0.0 ", sizeof szRSQ);
      return &szRSQ[0];
      }

   /* -- Test for magnitude problems here, fix if needed. */
      // The SQL Server's limit is 922,337,203,685,477.580
      // If the SSxx * SSyy product exceeds that limit
      // we have to reduce the SSzz values accordingly.
      ItiErrWriteDebugMessage ("  *--* Msg: computed SSxy, SSxx, SSyy values: ");
      ItiErrWriteDebugMessage (pszSSxy);
      ItiErrWriteDebugMessage (pszSSxx);
      ItiErrWriteDebugMessage (pszSSyy);
//      ItiErrWriteDebugMessage ("  *-*-* ");


   /* -- Set up to compute the r value. */
   ItiStrCpy (szQry, " select ", sizeof szQry);
   ItiStrCat (szQry, pszSSxy, sizeof szQry);
   ItiStrCat (szQry, " / SQRT( ", sizeof szQry);
   ItiStrCat (szQry, pszSSxx, sizeof szQry);
   ItiStrCat (szQry, " * ", sizeof szQry);
   ItiStrCat (szQry, pszSSyy, sizeof szQry);
   ItiStrCat (szQry, " ) ", sizeof szQry);

   pszLower = ItiDbGetRow1Col1 (szQry, hhp, 0,0,0);
   if ((pszLower != NULL) && ((*pszLower) != '\0'))
      {
      ItiStrCpy (szRSQ, pszLower, sizeof szRSQ);
      ItiExtract (szRSQ, ",");
      }
   else
      ItiStrCpy(szRSQ, " 0.0 ", sizeof szRSQ);


   ItiErrWriteDebugMessage (" R squared value (computed): ");
   ItiErrWriteDebugMessage (szRSQ);
   ItiErrWriteDebugMessage ("  *--* ");

   return &szRSQ[0];
   }/* End of Function ParametMajorItemRSQ */




/* Returns the slope for the Quantity 
 *
 *              SSxy        (‰(XY) - (‰X*‰Y)/n)
 *   slopeQ = --------  =  ---------------------
 *              SSxx          ‰(X˝) - (‰X)˝/n
 */
PSZ EXPORT ParametMajorItemSlopeQ
   (HHEAP hhp, PSZ pszMajorItemKey, PSZ pszWhereClause, USHORT usVorAorL)
   {
//   CHAR szTemp [2000] = "";
//   CHAR szQry  [2000] = "";
//   USHORT usi;
   CHAR szQry  [120] = "";
   PSZ   pszSlope = NULL;
   PSZ   pszSxx = NULL;
   PSZ   pszSxy = NULL;

   pszSxy = SSxy (hhp, pszMajorItemKey, pszWhereClause, usVorAorL);
   pszSxx = SSxx (hhp, pszMajorItemKey, pszWhereClause, usVorAorL);

   /* -- Test for nulls. */
   if ((pszSxy == NULL) || (pszSxx == NULL) )
      {  /* -- Problems. */
      ItiErrWriteDebugMessage ("Error Msg: ParametMajorItemSlopeQ computed one or more null sub values. ");
      ItiStrCpy(szSlopeQ, " 1.00 ", sizeof szSlopeQ);
      return &szSlopeQ[0];
      }

   ItiStrCpy (szQry, " select ", sizeof szQry);
   ItiStrCat (szQry, pszSxy, sizeof szQry);
   ItiStrCat (szQry, " / ", sizeof szQry);
   ItiStrCat (szQry, pszSxx, sizeof szQry);


//   for (usi = 0; usi < sizeof szS; usi++)
//      szS[usi] = '\0';
//
//   /* -- Find the V or A or L value for the %s substitution via szS. */
//               /* 5280 / 3 = 1760 ; conversion for miles into yards. */
//               /* The (HP.UnitType * (-760.0)) selects between english */ 
//               /* or metric conversion factor.  */
//   switch (usVorAorL)
//      {
//      case PARACALC_VOLUME:
//         ItiStrCpy (szS,
//                    "((ProposalLength * (1760.0+(HP.UnitType * (-760.0))) ) * (AveragePavementWidth / (3.0+(HP.UnitType * (-2.0))) ) * (AveragePavementDepth/ (36.0+(HP.UnitType * (964.0))) )) "
//                    , sizeof szS);
//         break;
//
//      case PARACALC_AREA:
//         ItiStrCpy (szS,
//            " ((ProposalLength * (1760.0+(HP.UnitType * (-760.0)))) * (AveragePavementWidth / (3.0+(HP.UnitType * (-2.0)))) ) "
//            , sizeof szS);
//         break;
//
//      case PARACALC_LENGTH:
//      default:
//         ItiStrCpy (szS, " (ProposalLength * (1760.0+(HP.UnitType * (-760.0)))) ", sizeof szS);
//         break;
//      }/* end of switch */
//
//
//   /* -- Next, Compute the Slope for quantities. */
//   //ItiMbQueryQueryText (hmodMe, ITIRID_CALC, GET_Q_SLOPE, szTemp, sizeof szTemp);
//   // Problem with => sprintf(szQry,szTemp, szS,szS,szS,szS,szS,szS);
//   ItiStrCpy(szQry, " select ((count(*) * SUM(( ", sizeof szQry);
//   ItiStrCat (szQry, szS, sizeof szQry);               
//   ItiStrCat (szQry, ") * (Quantity))) - (SUM(( ", sizeof szQry);
//   ItiStrCat (szQry, szS, sizeof szQry);               
//   ItiStrCat (szQry, ")) * SUM((Quantity)))) / ( (count(*) * SUM(( ",sizeof szQry);
//   ItiStrCat (szQry, szS, sizeof szQry);               
//   ItiStrCat (szQry, ") * ( ", sizeof szQry);
//   ItiStrCat (szQry, szS, sizeof szQry);               
//   ItiStrCat (szQry, "))) - (SUM( ", sizeof szQry);
//   ItiStrCat (szQry, szS, sizeof szQry);               
//   ItiStrCat (szQry, ") * SUM( ", sizeof szQry);
//   ItiStrCat (szQry, szS, sizeof szQry);               
//   ItiStrCat (szQry, "))) ", sizeof szQry);
//
//   ItiMbQueryQueryText (hmodMe, ITIRID_CALC, GET_S_I, szTemp, sizeof szTemp);
//   ItiStrCat (szQry, szTemp, sizeof szQry);               
//   ItiStrCat (szQry, pszMajorItemKey, sizeof szQry);               
//   ItiStrCat (szQry, " ", sizeof szQry);
//   ItiStrCat (szQry, pszWhereClause, sizeof szQry);



   pszSlope = ItiDbGetRow1Col1(szQry,hhp,0,0,0);
   if ((pszSlope != NULL) && ((*pszSlope) != '\0'))
      ItiStrCpy(szSlopeQ, pszSlope, FACTOR_SIZE);
   else
      ItiStrCpy(szSlopeQ, " 1.00 ", FACTOR_SIZE);

   ItiErrWriteDebugMessage (" Msg: Computed Quantity slope: ");
   ItiErrWriteDebugMessage (szSlopeQ);

   return &szSlopeQ[0];
   }/* End of Function */




PSZ EXPORT ParametMajorItemInterceptQ (HHEAP hhp, PSZ pszMajorItemKey, PSZ pszWhereClause, USHORT usVorAorL)
   {
   CHAR szTemp [900] = "";
   CHAR szQry  [1020] = "";
   PSZ    pszIntercept, pszSlopeLoc;
   USHORT usVAL = 1;

   /* -- set the value of Vol or Area or Length here. */
   if ((usVorAorL > PARACALC_VOLUME) || (usVorAorL < PARACALC_LENGTH))
      usVAL = ParaCalcDetermineVorAorL (pszMajorItemKey);
   else
      usVAL = usVorAorL;

   pszSlopeLoc = ParametMajorItemSlopeQ (hhp, pszMajorItemKey, pszWhereClause, usVAL);

   /* --  Compute the Quantity Intercept point. */
   //ItiMbQueryQueryText (hmodMe, ITIRID_CALC, GET_Q_INTERCEPT, szTemp, sizeof szTemp);
   //sprintf(szQry,szTemp, szSlopeQ, szS);
  // ItiStrCpy (szQry, " select AVG(Quantity) - (( ", sizeof szQry);
  // ItiStrCat (szQry, pszSlopeLoc, sizeof szQry);               
  // ItiStrCat (szQry, ") * AVG( ",sizeof szQry);
  // ItiStrCat (szQry, szS, sizeof szQry);               
  // ItiStrCat (szQry, ")) ", sizeof szQry);

   ItiStrCpy (szQry, " select (SUM(Quantity)/count(*) ) - (( ", sizeof szQry);
   ItiStrCat (szQry, pszSlopeLoc, sizeof szQry);               
   ItiStrCat (szQry, ") * (SUM( ",sizeof szQry);
   ItiStrCat (szQry, szS, sizeof szQry);               
   ItiStrCat (szQry, ")/count(*) )  ) ", sizeof szQry);


   ItiMbQueryQueryText (hmodMe, ITIRID_CALC, GET_S_I, szTemp, sizeof szTemp);
   ItiStrCat (szQry, szTemp, sizeof szQry);               
   ItiStrCat (szQry, pszMajorItemKey, sizeof szQry);
   ItiStrCat (szQry, " ", sizeof szQry);

   ItiStrCat (szQry, pszWhereClause, sizeof szQry);

   pszIntercept = ItiDbGetRow1Col1(szQry,hhp,0,0,0);
   if ((pszIntercept != NULL) && ((*pszIntercept) != '\0'))
      {
      ItiStrCpy(szInterceptQ, pszIntercept, FACTOR_SIZE);
      ItiExtract (szInterceptQ, ",");
      }
   else
      ItiStrCpy(szInterceptQ, " 1.00 ", FACTOR_SIZE);

   ItiErrWriteDebugMessage (" Msg: Computed Quantity intercept: ");
   ItiErrWriteDebugMessage (szInterceptQ);

   return &szInterceptQ[0];
   }/* End of Function */




// ////////////////////////////////////////////////////// APR 97

USHORT EXPORT ParametResetHistPropMajItms (BOOL bReset)
   {
#define PRH_HP_KEY      0
#define PRH_VENDOR_KEY  1

#define PRH_QTY     0
#define PRH_EXTAMT  1
#define PRH_WTAVG   2

   HHEAP hhp, hhp2;

   CHAR szInsert [] = "/* ParametResetHistPropMajItms */ "
     " INSERT INTO HistoricalProposalMajorItem (ProposalKey, MajorItemKey,"
     " Quantity, ExtendedAmount, WeightedAverageUnitPrice) VALUES ( ";

   CHAR szUpdate [] = "/* ParametResetHistPropMajItms */ "
     " UPDATE HistoricalProposalMajorItem "
     " SET Quantity = ";
                  
   CHAR szTemp [600] = "/* ParametResetHistPropMajItms */ "
     " select SUM(SIMI.QuantityConversionFactor * HPI.Quantity),"
     "  SUM(HPI.Quantity * HIB.UnitPrice),"
     "  SUM(HPI.Quantity * HIB.UnitPrice)" 
     "  / SUM(SIMI.QuantityConversionFactor * HPI.Quantity)"
     " from HistoricalProposalItem HPI, StandardItemMajorItem SIMI "
     " , MajorItem MI, HistoricalItemBid HIB "
     " where HPI.ItemKey = SIMI.StandardItemKey "
     " AND  SIMI.MajorItemKey = MI.MajorItemKey "
     " AND HIB.ProposalKey = HPI.ProposalKey "
     " AND HIB.ProposalItemKey = HPI.ProposalItemKey "
     ;
   CHAR szQuery [820] = "";
   CHAR szQry   [250] = "SELECT DISTINCT MajorItemKey FROM MajorItem WHERE MajorItemKey > 999999 AND Deleted = NULL ";
   HQRY     hqry;
   USHORT   usNumCols, usCols, usError, usCnt, us, i, usHPMICnt;
   PSZ      *ppsz, *ppszLoop, pszHPMICnt;
   CHAR     aszMajorItemKey [MAX_MAJOR_ITEMS+1] [10];

   hhp = ItiMemCreateHeap (2000);
   if (hhp == NULL)
      return 13;

   hhp2 = ItiMemCreateHeap (8000);
   if (hhp == NULL)
      return 13;

/* -- First, build an array of major item keys. */
   /* Init the key array. */
   for (i = 0; i < MAX_MAJOR_ITEMS ; i++)
      aszMajorItemKey [i][0] = '\0';

   hqry = ItiDbExecQuery (szQry, hhp, 0, 0, 0, &usNumCols, &usError);
   if (hqry == NULL)
      return FALSE;
   else
      i = 0;

   /* Get the major item keys and init the key array. */ 
   while (ItiDbGetQueryRow (hqry, &ppsz, &usError))
      {
      ItiStrCpy (aszMajorItemKey [i], ppsz [0], sizeof aszMajorItemKey [i]);
      ItiFreeStrArray (hhp, ppsz, usNumCols);
      i++;
      }
   usCnt = i; /* Set the number of major item keys found. */

   if (!bReset)
      ItiErrWriteDebugMessage ("\n   Display of HPMI queries only, NOT executed! \n");
   else
      ItiErrWriteDebugMessage ("\n   Recalculation of HPMI data! \n");

/* -- Second, with each MajItmKey loop over HistoricalProposals    */
/* -- getting the HistoricalProposalKey and AwardedBidderKeys.     */
   i = 0;
   while ((i < usCnt) && (aszMajorItemKey[i][0] != '\0'))
      {
      /* Ugh, we need to create/destroy heap space, we've runout before! */ 
      DosBeep (1200, 10); 
      if (hhp != NULL)
         ItiMemDestroyHeap (hhp);
      hhp = ItiMemCreateHeap (4000);
      if (hhp == NULL)
         return 13;

      ItiStrCpy(szQry, " SELECT DISTINCT ProposalKey, AwardedBidderKey "
                       " FROM HistoricalProposal "
                       " WHERE AwardedBidderKey > 0 "
                     , sizeof szQry);

      hqry = ItiDbExecQuery (szQry, hhp2, 0, 0, 0, &usNumCols, &usError);
      if (hqry == NULL)
         continue;

      while (ItiDbGetQueryRow (hqry, &ppszLoop, &usError))
         {
/* -- Third, inside the MjrItm & HistProp loops query, compute and get */
/* -- the HPMI's Qty, ExtAmt, and WtAvgUnitPrice.                      */
         /* Reset the head of the query. */
         ItiStrCpy (szQuery, szTemp, sizeof szQuery);
         ItiStrCat (szQuery, " AND HIB.VendorKey = ", sizeof szQuery);
         ItiStrCat (szQuery, ppszLoop[PRH_VENDOR_KEY], sizeof szQuery);
   
         ItiStrCat (szQuery, " AND HPI.ProposalKey = ", sizeof szQuery);
         ItiStrCat (szQuery, ppszLoop[PRH_HP_KEY], sizeof szQuery);
   
         ItiStrCat (szQuery, " AND MI.MajorItemKey = ", sizeof szQuery);
         ItiStrCat (szQuery, aszMajorItemKey[i], sizeof szQuery);

         /* Get the HPMI's Qty, ExtAmt, and WtAvgUnitPrice. */
         ppsz = ItiDbGetRow1 (szQuery, hhp, 0, 0, 0, &usCols);

     /* -- Forth, UPDATE the HPMI's Qty, ExtAmt, and WtAvgUnitPrice.   */
         if (ppsz[PRH_QTY][0] != '0') /* if Qty is zero, skip to next. */ 
            {
            /* We had a non-zero quantity, so see if that PK & MIK     */
            /* combination already exist in th HPMI table to determine */
            /* if we do an insert or an update statement.              */

            ItiStrCpy (szQry,
                       " select count(*) from HistoricalProposalMajorItem "
                       " where MajorItemKey = "
                       , sizeof szQry);
            ItiStrCat (szQry, aszMajorItemKey[i], sizeof szQry);
            ItiStrCat (szQry, " AND ProposalKey = ", sizeof szQry);
            ItiStrCat (szQry, ppszLoop[PRH_HP_KEY], sizeof szQry);

            pszHPMICnt = ItiDbGetRow1Col1(szQry,hhp,0,0,0);
            if ((pszHPMICnt != NULL) && ((*pszHPMICnt) != '\0'))
               {
               ItiStrToUSHORT (pszHPMICnt, &usHPMICnt);
               ItiErrWriteDebugMessage (pszHPMICnt);
               }
            else
               {
               ItiErrWriteDebugMessage (" *=*=* ERROR with: ");
               ItiErrWriteDebugMessage (szQry);
               continue;
               }

            if (usHPMICnt > 0)
               {  /* Use the UPDATE statement. */
               ItiStrCpy (szQuery, szUpdate, sizeof szQuery);
               ItiExtract (ppsz[PRH_QTY], ",");
               ItiStrCat (szQuery, ppsz[PRH_QTY], sizeof szQuery);
         
               ItiStrCat (szQuery, ", ExtendedAmount = ", sizeof szQuery);
               ItiExtract (ppsz[PRH_EXTAMT], ",");
               ItiStrCat (szQuery, ppsz[PRH_EXTAMT], sizeof szQuery);
         
               ItiStrCat (szQuery, ", WeightedAverageUnitPrice = ", sizeof szQuery);
               ItiExtract (ppsz[PRH_WTAVG], ",");
               ItiStrCat (szQuery, ppsz[PRH_WTAVG], sizeof szQuery);
         
               ItiStrCat (szQuery, " WHERE MajorItemKey = ", sizeof szQuery);
               ItiStrCat (szQuery, aszMajorItemKey[i], sizeof szQuery);
      
               ItiStrCat (szQuery, " AND ProposalKey = ", sizeof szQuery);
               ItiStrCat (szQuery, ppszLoop[PRH_HP_KEY], sizeof szQuery);
               }
            else
               { /* Use the INSERT statement. */
               ItiStrCpy (szQuery, szInsert, sizeof szQuery);
               ItiStrCat (szQuery, ppszLoop[PRH_HP_KEY], sizeof szQuery);
               ItiStrCat (szQuery, ", ", sizeof szQuery);

               ItiStrCat (szQuery, aszMajorItemKey[i], sizeof szQuery);
               ItiStrCat (szQuery, ", ", sizeof szQuery);

               ItiExtract (ppsz[PRH_QTY], ",");
               ItiStrCat (szQuery, ppsz[PRH_QTY], sizeof szQuery);
               ItiStrCat (szQuery, ", ", sizeof szQuery);
         
               ItiExtract (ppsz[PRH_EXTAMT], ",");
               ItiStrCat (szQuery, ppsz[PRH_EXTAMT], sizeof szQuery);
               ItiStrCat (szQuery, ", ", sizeof szQuery);
         
               ItiExtract (ppsz[PRH_WTAVG], ",");
               ItiStrCat (szQuery, ppsz[PRH_WTAVG], sizeof szQuery);
               ItiStrCat (szQuery, ") ", sizeof szQuery);
               }

            if (bReset)
               ItiDbExecSQLStatement (hhp, szQuery); /* Do the update. */
            else
               ItiErrWriteDebugMessage (szQuery);

            } /* end of if (ppsz[PRH_QTY][0]... then clause */
         else
            ItiErrWriteDebugMessage ("zero Qty found");

         ItiFreeStrArray (hhp2, ppszLoop, usNumCols);
         ItiFreeStrArray (hhp, ppsz, usCols);
         }/* end of while (ItiDbGet... */


      i++; /* use next array element (major item key) */
      } /* end of while ((i < usCnt... loop */

   if (!bReset)
      ItiErrWriteDebugMessage ("\n   End of display of HPMI queries only. \n");
   else
      ItiErrWriteDebugMessage ("\n   End of recalculation of HPMI data! \n");

   if (hhp != NULL)
      ItiMemDestroyHeap (hhp);
   if (hhp2 != NULL)
      ItiMemDestroyHeap (hhp2);
   return 0;
   }/* End of Function ParametResetHistPropMajItms */
// ////////////////////////////////////////////////////// APR 97
