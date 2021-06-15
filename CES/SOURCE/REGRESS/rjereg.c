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


/*  CES Report DLL Source Code
 *
 *  Timothy Blake (tlb)
 */


#define     INCL_BASE
#define     INCL_DOS
#define     INCL_DEV
#define     INCL_GPI
#define     INCL_WIN

#include    "..\include\iti.h"     
#include    "..\include\itibase.h"
#include    "..\include\itiutil.h"
#include    "..\include\itierror.h"
#include    "..\include\itimbase.h"
#include    "..\include\itidbase.h"
#include    "..\include\itifmt.h"
#include    "..\include\colid.h"
#include    "..\include\winid.h"
#include    "..\include\itiwin.h"
#include    "..\include\dialog.h"
#include    "..\include\itimenu.h"
#include    "..\include\itiEst.h"

#include    "dialog.h"

#include    "..\include\itirptdg.h"
#include    "..\include\itirpt.h"


/*
 * The following are support modules that are always used by a report DLL.
 */
#if !defined (INCL_ITIPRT)
#include "..\include\itiprt.h"
#endif

#if !defined (INCL_RPTUTIL)
#include "..\include\itirptut.h"
#endif


#include    "rindljer.h"


#include "rJEReg.h"                                       


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>




/* ======================================================================= *\
 *                                         MANIFEST CONSTANT DEFINITIONS   *
\* ======================================================================= */
#define  BLANK_LINE_INTERVAL    4
#define  PRT_Q_NAME             "JE: Regression"

#define  QUERY_ARRAY_SIZE       1800

#define  DATE_STR_LEN           SMARRAY
#define  KEY_STR_LEN            SMARRAY
#define  STR_SPC                SMARRAY


#define DAYS_PER_MONTH      30.4375
#define DAYS_PER_YEAR      365.25



#define SETQERRMSG(QY) ItiStrCpy (szErrorMsg,                                \
                              "ERROR: Query failed for ",                    \
                              sizeof szErrorMsg);                            \
                                                                             \
                   ItiStrCat (szErrorMsg, TITLE, sizeof szErrorMsg);         \
                   ItiStrCat (szErrorMsg,", Error Code: ",sizeof szErrorMsg);\
                                                                             \
                   ItiStrUSHORTToString                                      \
                      (usErr, &szErrorMsg[ItiStrLen(szErrorMsg)],            \
                       sizeof szErrorMsg);                                   \
                                                                             \
                   ItiStrCat (szErrorMsg,".  Query: ", sizeof szErrorMsg);   \
                   ItiStrCat (szErrorMsg, QY, sizeof szErrorMsg);            \
                                                                             \
                   ItiRptUtErrorMsgBox (NULL, szErrorMsg)




/* ======================================================================= *\
 *                                             MODULE'S GLOBAL VARIABLES   *
\* ======================================================================= */
static  CHAR  szCompileDateTime[] = __DATE__ " " __TIME__ ;
static CHAR szDllVersion[] = "1.1a0 rjereg.dll";

static  CHAR  szOrderingStr [SMARRAY + 1] = "";

static  CHAR  szFailedDLL [CCHMAXPATH + 1] = "";
static  CHAR  szErrorMsg  [1024] = "";

static  USHORT usNumOfTitles;
static  USHORT usCurrentPg = 0;


static  CHAR  szQry    [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szQryLoc [QUERY_ARRAY_SIZE + 1] = "";

static  CHAR  szJobKey [SMARRAY + 1] = "";


static  CHAR  szQryTitleNames [SIZE_OF_QRY_TITLE_NAMES + 1] = "";
static  CHAR  szCurCountyKey  [SMARRAY + 1] = "";
static  CHAR  szCurCountyID   [SMARRAY + 1] = "";


static  CHAR  szQryJobBrkdwn [SIZE_OF_QRY_JOB_BRKDWN  + 1] = "";
static  CHAR  szQryBDKeys    [SIZE_OF_QRY_JOB_BRKDWN + 32] = "";

static  CHAR  szQryJobItem  [SIZE_OF_QRY_JOB_ITEM + 1] = "";
static  CHAR  szConjJobItem [SIZE_OF_CONJ_JOB_ITEM + 1] = "";

static  CHAR  szQryBPQ           [SIZE_OF_QRY_BPQ + 1] = "";
static  CHAR  szConjBPQCap       [2 * SMARRAY + 1] = "";
static  CHAR  szConjBPQKey       [SMARRAY + 1] = "";
static  CHAR  szConjBPQItemKey   [SMARRAY + 1] = "";
static  CHAR  szConjBPQBrkdwnKey [SMARRAY + 1] = "";


static  USHORT usBrkdwnCnt = 0;
static  CHAR   szCurJobBrkdwnKey [SMARRAY + 1] = "";
static  CHAR   szCurJobBrkdwnID  [SMARRAY + 1] = "";
static  CHAR   szCurWorkType     [SMARRAY + 1] = "";

static  PSZ  pszCurJobBrkdwnKey  = szCurJobBrkdwnKey;
static  PSZ  pszCurJobBrkdwnID   = szCurJobBrkdwnID; 
static  PSZ  pszCurWorkType      = szCurWorkType;

static  CHAR  szQryImp         [4*SMARRAY + 1] = "";


/* -- Adjustment queries. */
static  CHAR  szSlcAdj         [2*SMARRAY + 1] = "";
static  CHAR  szSlcAdjRaw      [2*SMARRAY + 1] = "";


static  CHAR  szQryArea         [SIZE_OF_QRY_AREA + 1] = "";
static  CHAR  szConjAreaCKey    [SMARRAY + 1] = "";
static  CHAR  szConjAreaSIKey   [SMARRAY + 1] = "";

static  CHAR  szQrySeason       [SIZE_OF_QRY_SEASON + 1] = "";
static  CHAR  szConjSeason      [4 * SMARRAY + 1] = "";

static  CHAR  szQryWorkType     [SIZE_OF_QRY_WORKTYPE + 1] = "";
static  CHAR  szConjWorkType    [SMARRAY + 1] = "";


static  CHAR  szConjAdjImpDate [SMARRAY + 1] = "";
static  CHAR  szConjAdjImpDateCap [SMARRAY + 1] = "";


static  PSZ   *appsz[1024];
static  USHORT usIndex_appsz = 0;


           /* Used by DbExecQuer & DbGetQueryRow functions: */
static  HHEAP   hhprJEReg  = NULL;
static  HHEAP   hhpCalcReg = NULL;

static  HQRY    hqry   = NULL;
static  USHORT  usResId   = ITIRID_RPT;
static  USHORT  usId      = ID_QRY_LISTING;
static  USHORT  usNumCols = 0;
static  USHORT  usErr     = 0;
static  PSZ   * ppsz   = NULL;

static  HQRY    hqryBDKeys = NULL;
static  PSZ   * ppszBDKeys = NULL;
static  USHORT  usNumColsBDKeys = 0;


           /* Title strings for this module. */
static  REPTITLES rept;
static  PREPT     prept = &rept;

static  COLTTLS  acttl[RPT_TTL_NUM + 1];





static DOUBLE SignedLn (DOUBLE d)
   {
   if (d == 0)
      return 0;
   else if (d < 0)
      return -log (-d);
   else 
      return log (d);
   }






static BOOL GetJobInfo (HHEAP hheap,           //In
                        PSZ   pszJobKey,       //In
                        PSZ   pszBaseDate,   //Out
                        PSZ   pszMonth)      //Out
   {
   char   szTemp [256];
   PSZ    *ppszJobInfo;
   USHORT usNumCols;



   sprintf (szTemp,
            "SELECT BaseDate, LettingMonth "
            "FROM Job "
            "WHERE JobKey = %s ",
            pszJobKey);

   ppszJobInfo = ItiDbGetRow1 (szTemp, hheap, 0, 0, 0, &usNumCols);
   if (ppszJobInfo == NULL)
      return FALSE;

   if (pszBaseDate != NULL)
      ItiStrCpy(pszBaseDate, ppszJobInfo[0], STR_SPC);

   if (pszMonth != NULL)
      ItiStrCpy(pszMonth, ppszJobInfo[1], STR_SPC);

   ItiMemFree (hheap, ppszJobInfo);

   return TRUE;
   }/* End of Function GetJobinfo */






static BOOL GetAreaID (HHEAP hheap,           //In
                       PSZ   pszAreaKey,      //In
                       PSZ   pszAreaTypeKey,  //In
                       PSZ   pszAreaID)       //Out

   {
   char   szTemp [256];
   PSZ    *ppszAreaInfo;
   USHORT usNumCols;



   sprintf (szTemp,
            "SELECT AreaID "
            "FROM Area "
            "WHERE AreaTypeKey = %s "
            "AND AreaKey = %s ",
            pszAreaTypeKey, pszAreaKey);

   ppszAreaInfo = ItiDbGetRow1 (szTemp, hheap, 0, 0, 0, &usNumCols);
   if (ppszAreaInfo == NULL)
      return FALSE;


   if (pszAreaID != NULL)
      ItiStrCpy(pszAreaID, ppszAreaInfo[0], STR_SPC);

   ItiMemFree (hheap, ppszAreaInfo);

   return TRUE;
   }/* End of Function GetAreaID */







static BOOL GetJobItemQty (HHEAP hhpJIQ,        //In
                           PSZ   pszJobKey,     //In
                           PSZ   pszJobItemKey, //In
                           PSZ   pszQty)        //Out
   {
   CHAR   szTemp [128];
   PSZ    *ppszResult;
   USHORT usNumCol;


   if ((pszQty != NULL) && (pszJobKey != NULL) && (pszJobItemKey != NULL))
      pszQty[0] = '\0';
   else
      return FALSE;

   sprintf (szTemp,
            "SELECT Quantity "
            "FROM JobItem "
            "WHERE JobKey = %s "
            "AND JobItemKey = %s ",
            pszJobKey, pszJobItemKey);

   ppszResult = ItiDbGetRow1 (szTemp, hhpJIQ, 0, 0, 0, &usNumCol);
   if (ppszResult == NULL)
      return FALSE;

   if (pszQty != NULL)
      ItiStrCpy(pszQty, ppszResult[0], STR_SPC);

   ItiMemFree (hhpJIQ, ppszResult);

   return TRUE;
   }/* End of Function GetJobItemQty */





static DOUBLE GetAdjustment (HHEAP   hheap,                   //In
                             PSZ     pszImportDate,           //In
                             PSZ     pszStandardItemKey,      //In
                             PSZ     pszAdjustmentType,       //In
                             PSZ     pszAdjustmentKeyColumn,  //In
                             PSZ     pszAdjustmentKeyValue,   //In
                             PSZ     pszAdjustment)           //Out
   {
   CHAR   szTemp [1024];
   PSZ    pszAdj;
   DOUBLE d;

   sprintf (szTemp,
            "SELECT Adjustment "
            "FROM %sAdjustment ADJ, PEMETHRegression PR "
            "WHERE PR.StandardItemKey = %s "
            " AND  PR.StandardItemKey = ADJ.StandardItemKey "
            " AND  PR.ImportDate = '%s' "
            " AND  PR.ImportDate = ADJ.ImportDate "
            " AND %s = %s ",
            pszAdjustmentType, pszStandardItemKey, 
            pszImportDate, pszAdjustmentKeyColumn, pszAdjustmentKeyValue);

   pszAdj = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);
   if (pszAdj == NULL)
      return 0;

   d = ItiStrToDouble (pszAdj);

   if (pszAdjustment != NULL)
      {
      ItiFmtFormatString (pszAdj,
                          pszAdjustment,
                          STR_SPC,
                          "Number,%,..",
                          NULL);
      }

   ItiMemFree (hheap, pszAdj);
   return d;

   }/* End of Function GetAdjustment */




DOUBLE CalcReg (PSZ     pszBaseDate,          //In
                PSZ     pszImportDate,        //In
                DOUBLE  dBasePrice,
                DOUBLE  dQuantity,
                DOUBLE  dMedianQuantity,
                DOUBLE  dQuantityAdjustment,
                DOUBLE  dAnnualInflation,
                DOUBLE  dAreaAdj,
                DOUBLE  dWorkTypeAdj,
                DOUBLE  dSeasonAdj,
                DOUBLE  *pdLog,
                DOUBLE  *pdBasePriceBase,
                DOUBLE  *pdAreaExt,
                DOUBLE  *pdWorkTypeExt,
                DOUBLE  *pdSeasonExt)
   {
   DOUBLE dPrice, dExponent, dYears;
   USHORT usDays;

   /* assumes base date always >= import date (the function returns the 
      absolute value of the arithmetic) */
   usDays = ItiRptUtDateDiff (pszBaseDate, pszImportDate);
   dYears = (DOUBLE) usDays / DAYS_PER_YEAR;
   dExponent = SignedLn (dQuantity / dMedianQuantity) / NATLOG_2;
   *pdLog = dExponent;

   dPrice  = dBasePrice;

   dPrice  = dPrice * pow (1.0 + dQuantityAdjustment, dExponent);
   dPrice  = dPrice * pow (1.0 + dAnnualInflation, dYears);

   *pdBasePriceBase = dPrice;


   *pdAreaExt = dPrice * dAreaAdj;
   dPrice  += dPrice * dAreaAdj;


   *pdSeasonExt = dPrice * dSeasonAdj;
   dPrice  += dPrice * dSeasonAdj;


   *pdWorkTypeExt = dPrice * dWorkTypeAdj;
   dPrice  += dPrice * dWorkTypeAdj;


   return dPrice;

   }/* End of Function CalcReg */





static BOOL GetRegressionInfo (HHEAP   hhpGetReg,
                               PSZ     pszBaseDate,
                               PSZ     pszStandardItemKey,
                               PDOUBLE pdBasePrice,
                               PDOUBLE pdAnnualInflation,
                               PDOUBLE pdQuantityAdjustment,
                               PDOUBLE pdMedianQuantity,
                               PSZ     *ppszImportDate, 
                               PSZ     *ppszAreaTypeKey)
   {
   char     szTemp [1024];
   PSZ      *ppszRegInfo;
   USHORT   usNumCols;

   *pdBasePrice = 0;
   *pdAnnualInflation = 0;
   *pdQuantityAdjustment = 0;
   *pdMedianQuantity = 0;
   *ppszImportDate = NULL;
   *ppszAreaTypeKey = NULL;


   sprintf (szTemp,
            "SELECT PR.BaseUnitPrice, PR.AnnualInflation, "
            "PR.QuantityAdjustment, PR.MedianQuantity, "
            "PR.ImportDate, PR.AreaTypeKey "
            "FROM StandardItemPEMETHRegression SIPR, PEMETHRegression PR "
            "WHERE PR.StandardItemKey = %s "
            "AND SIPR.StandardItemKey = PR.StandardItemKey "
            "AND SIPR.BaseDate = '%s' "
            "AND SIPR.ImportDate = PR.ImportDate ",
            pszStandardItemKey, pszBaseDate);

   ppszRegInfo = ItiDbGetRow1 (szTemp, hhpGetReg, 0, 0, 0, &usNumCols);
   if (ppszRegInfo == NULL)
      {
#ifdef DEBUG
      ItiErrWriteDebugMessage (szTemp);
#endif
      return FALSE;
      }

   *pdBasePrice = ItiStrToDouble (ppszRegInfo [0]);
   *pdAnnualInflation = ItiStrToDouble (ppszRegInfo [1]);
   *pdQuantityAdjustment = ItiStrToDouble (ppszRegInfo [2]);
   *pdMedianQuantity = ItiStrToDouble (ppszRegInfo [3]);
   *ppszImportDate = ItiStrDup (hhpGetReg, ppszRegInfo [4]);
   *ppszAreaTypeKey = ItiStrDup (hhpGetReg, ppszRegInfo [5]);


   ItiFreeStrArray (hhpGetReg, ppszRegInfo, usNumCols);
   return TRUE;
   }/* End of Function GetRegressionInfo */





USHORT CalculateRegression (HHEAP  hhpCalcReg,          //In
                            PSZ    pszJobKey,           //In
                            PSZ    pszWorkType,         //In
                            PSZ    pszWorkTypeID,       //In
                            PSZ    pszJobItemKey,       //In
                            PSZ    pszStandardItemKey,  //In
                                                      //Out:
                            PSZ    *ppszBasePriceBase, 
                            PSZ    *ppszQuantity,      
                            PSZ    *ppszQtyAdjustment, 
                            PSZ    *ppszMedianQty,     
                            PSZ    *ppszBasePrice,     
                            PSZ    *ppszAreaExt,       
                            PSZ    *ppszAdjPctArea,    
                            PSZ    *ppszAreaID,       
                            PSZ    *ppszSeaExt,        
                            PSZ    *ppszAdjPctSeason,  
                            PSZ    *ppszSeasonFmt,     
                            PSZ    *ppszWorkTypeExt,   
                            PSZ    *ppszAdjPctWorkType,
                            PSZ    *ppszWorkTypeID,
                            PSZ    *ppszRegValue,
                            PSZ    *ppszLog)           
   {
   PSZ * ppszRes = NULL;

   DOUBLE   dAreaPct, dWorkTypePct, dSeasonPct;
   DOUBLE   dAreaExt, dWorkTypeExt, dSeasonExt;
   DOUBLE   dQuantityAdjustment, dAnnualInflation, dLog;
   DOUBLE   dBasePrice, dBasePriceBase, dQuantity, dPrice, dMedianQuantity;
   USHORT   usMonth;

   PSZ      pszImportDate;
   PSZ      pszAreaKey, pszAreaTypeKey;

   CHAR  szSeason [8];
   CHAR  szSeasonFmt[16] = " ";
   CHAR  szSeaExt     [16] = " ";
   CHAR  szAreaExt    [16] = " ";
   CHAR  szWorkTypeExt[16] = " ";
   CHAR  szLog        [16] = " ";

   CHAR  szTemp [64];

   CHAR  szBaseDate[STR_SPC]   = " ";
   CHAR  szMonth[STR_SPC]      = " ";
   CHAR  szQuantity[STR_SPC]   = " ";  
   CHAR  szAreaID  [STR_SPC]   = " ";  
   CHAR  szBasePriceBase[STR_SPC] = " ";  
   CHAR  szQtyAdjustment[STR_SPC] = " ";  
   CHAR  szMedianQty[STR_SPC]     = " ";  
   CHAR  szBasePrice[STR_SPC]     = " ";  
   CHAR  szAdjPctArea[STR_SPC]     = " "; 
   CHAR  szAdjPctWorkType[STR_SPC] = " "; 
   CHAR  szAdjPctSeason[STR_SPC]   = " "; 

   PSZ   pszRegValue; 



   if (!GetJobInfo(hhpCalcReg, pszJobKey, &szBaseDate[0], &szMonth[0]))
      {
      sprintf(szErrorMsg,"Failed to get Job's basedate; "
                         "no regression done. JobKey: %s ", pszJobKey);
      ItiErrWriteDebugMessage (szErrorMsg);
      return 13;
      }


   if (!GetJobItemQty(hhpCalcReg, pszJobKey, pszJobItemKey, &szQuantity[0]))
      {
      sprintf(szErrorMsg,"Failed to get job item's quantity; "
                         "no regression done. JobItemKey: %s ",pszJobItemKey);
      ItiErrWriteDebugMessage (szErrorMsg);
      return 13;
      }



   if (!ItiStrToUSHORT (szMonth, &usMonth))
      usMonth = 0;
   szSeason [0] = (char) (usMonth / 4) + (char) '1';
   if (szSeason [0] > '4')
      szSeason [0] = '4';
   else if (szSeason [0] < '0')
      szSeason [0] = '0';
   szSeason [1] = '\0';

   ItiFmtFormatString(szSeason,szSeasonFmt, sizeof szSeasonFmt,"Season",NULL);



   if (!GetRegressionInfo(hhpCalcReg, szBaseDate, pszStandardItemKey,
                          &dBasePrice, &dAnnualInflation, &dQuantityAdjustment,
                          &dMedianQuantity, &pszImportDate, &pszAreaTypeKey))
      return 13;

   sprintf(szBasePrice,     "$%.4lf", dBasePrice);
   sprintf(szQtyAdjustment,  "%.4lf", dQuantityAdjustment);
   sprintf(szMedianQty,      "%.4lf", dMedianQuantity);


   pszAreaKey = ItiEstGetJobArea (hhpCalcReg, pszJobKey, pszAreaTypeKey, szBaseDate);


   GetAreaID(hhpCalcReg, pszAreaKey, pszAreaTypeKey, szAreaID);


   dAreaPct = GetAdjustment (hhpCalcReg, pszImportDate, pszStandardItemKey, 
                             "Area", "AreaKey", pszAreaKey, szAdjPctArea);

   dSeasonPct = GetAdjustment (hhpCalcReg, pszImportDate, pszStandardItemKey, 
                               "Season", "Season", szSeason, szAdjPctSeason);

   dWorkTypePct = GetAdjustment (hhpCalcReg, pszImportDate, pszStandardItemKey, 
                                 "WorkType", "WorkType", pszWorkType, szAdjPctWorkType);

   dQuantity = ItiStrToDouble (szQuantity);



   dPrice = CalcReg (szBaseDate, pszImportDate, dBasePrice, dQuantity,
                     dMedianQuantity, dQuantityAdjustment, dAnnualInflation,
                     dAreaPct,  dWorkTypePct,  dSeasonPct, &dLog, &dBasePriceBase,
                     &dAreaExt, &dWorkTypeExt, &dSeasonExt);


   /* -- Format the values into strings. */
   sprintf(szBasePriceBase, "$%.4lf", dBasePriceBase);
   sprintf(szAreaExt,       "$%.4lf", dAreaExt);
   sprintf(szSeaExt,        "$%.4lf", dSeasonExt);
   sprintf(szWorkTypeExt,   "$%.4lf", dWorkTypeExt);
   sprintf(szLog,           "%.4lf",  dLog);

   /* -- This is the bottom line value;  = $99,999,999.9999 / XXXX. */
   sprintf(szTemp,        "$%.4lf", dPrice);
   pszRegValue = szTemp;


//   ppszRes = ItiStrMakePPSZ (hhpCalcReg, 16,
//                           szBasePriceBase,    
//                           szQuantity,         
//
//                           pszQtyAdjustment,   
//                           pszMedianQty,       
//                           szBasePrice,        
//
//                           szAreaExt,          
//                           szAdjPctArea,       
//                           pszAreaKey,         
//
//                           szSeaExt,           
//                           szAdjPctSeason,     
//                           szSeasonFmt,        
//
//                           szWorkTypeExt,      
//                           szAdjPctWorkType,   
//                           pszWorkTypeID,      
//
//                           pszRegValue,        
//                           szLog               
//                          );



  *ppszBasePriceBase = ItiStrDup(hhpCalcReg, szBasePrice);     /* REG_BASE_PRICE_BASE 0 */
  *ppszQuantity      = ItiStrDup(hhpCalcReg, szQuantity);      /* REG_QUANTITY        1 */

  *ppszQtyAdjustment = ItiStrDup(hhpCalcReg, szQtyAdjustment); /* REG_QTY_ADJ         2 */
  *ppszMedianQty     = ItiStrDup(hhpCalcReg, szMedianQty);     /* REG_MEDIAN_QTY      3 */
  *ppszBasePrice     = ItiStrDup(hhpCalcReg, szBasePriceBase); /* REG_BASE_PRICE      4 */
                      
  *ppszAreaExt       = ItiStrDup(hhpCalcReg, szAreaExt);       /* REG_AREA_EXT        5 */
  *ppszAdjPctArea    = ItiStrDup(hhpCalcReg, szAdjPctArea);    /* REG_AREA_ADJ        6 */
                                                               
  *ppszAreaID        = ItiStrDup(hhpCalcReg, szAreaID);        /* REG_AREA_ID         7 */
                                                               
  *ppszSeaExt        = ItiStrDup(hhpCalcReg, szSeaExt);        /* REG_SEA_EXT         8 */
  *ppszAdjPctSeason  = ItiStrDup(hhpCalcReg, szAdjPctSeason);  /* REG_SEA_ADJ         9 */
  *ppszSeasonFmt     = ItiStrDup(hhpCalcReg, szSeasonFmt);     /* REG_SEA_ID         10 */
                                                               
  *ppszWorkTypeExt   = ItiStrDup(hhpCalcReg, szWorkTypeExt);   /* REG_WRK_EXT        11 */
  *ppszAdjPctWorkType= ItiStrDup(hhpCalcReg, szAdjPctWorkType);/* REG_WRK_ADJ        12 */
  *ppszWorkTypeID    = ItiStrDup(hhpCalcReg, pszWorkTypeID);   /* REG_WRK_ID         13 */
                                                               
  *ppszRegValue      = ItiStrDup(hhpCalcReg, pszRegValue);     /* REG_REG_VAL        14 */
  *ppszLog           = ItiStrDup(hhpCalcReg, szLog);           /* REG_LOG_VAL        15 */


   ItiMemFree (hhpCalcReg, pszImportDate);
   ItiMemFree (hhpCalcReg, pszAreaTypeKey);
   ItiMemFree (hhpCalcReg, pszAreaKey);

   return 0;
   }/* End of Function CalculateRegression */




/* ======================================================================== */





USHORT SetTitles (PREPT preptitles, INT argcnt, CHAR * argvars[])
   {
   USHORT us;
   CHAR   szNewTitle[TITLE_LEN +1] = "";
   PSZ   *ppszTitleNames = NULL;
   CHAR   szOpt[FOOTER_LEN + 1];
   PSZ    pszOpt = szOpt;


   if (preptitles == NULL)
      return (ITIRPT_INVALID_PARM);



   ItiMbQueryQueryText(hmodrJEReg, ITIRID_RPT, ID_QRY_TITLE_NAMES,
                       szQryTitleNames, sizeof szQryTitleNames );

   /* Append the Job's key to the query; the LAST argvar is the JobKey. */
   ItiStrCat(szQryTitleNames, argvars[argcnt-1], sizeof(szQryTitleNames));

   ppszTitleNames = ItiDbGetRow1(szQryTitleNames, hhprJEReg, hmodrJEReg,
                                ITIRID_RPT, ID_QRY_TITLE_NAMES, &usNumCols);
   if (ppszTitleNames != NULL)
      {
      ItiStrCpy(szNewTitle, TITLE_LEAD,             sizeof(szNewTitle) );
      ItiStrCat(szNewTitle, ppszTitleNames[JOB_ID], sizeof(szNewTitle) );

      /* -- Remove trailing blanks. */
      us = (ItiStrLen(szNewTitle)) - 1;
      while ((us > 1) && (szNewTitle[us] == ' ') && (szNewTitle[us-1] == ' ') )
         {
         szNewTitle[us] = '\0';
         us--;
         }

      ItiStrCat(szNewTitle, TITLE_TAIL,  sizeof(szNewTitle) );


      ItiStrCpy(preptitles->szSubSubTitle,
                "County: ",
                sizeof(preptitles->szSubSubTitle));

      ItiStrCat(preptitles->szSubSubTitle,
                ppszTitleNames[COUNTY_NAME],
                sizeof(preptitles->szSubSubTitle) );


//      ItiStrCpy(szCurCountyID, ppszTitleNames[COUNTY_ID], sizeof szCurCountyID);
      ItiStrCpy(szCurCountyKey,ppszTitleNames[COUNTY_KEY],sizeof szCurCountyKey);

      ItiFreeStrArray (hhprJEReg, ppszTitleNames, usNumCols);
      }
   else
      {
      SETQERRMSG(szQryTitleNames);
      ItiStrCpy(szNewTitle, TITLE_LEAD, sizeof(szNewTitle) );
      ItiStrCat(szNewTitle, TITLE_TAIL, sizeof(szNewTitle) );
      }



   ItiStrCpy(preptitles->szTitle,   szNewTitle, sizeof(preptitles->szTitle) );
   ItiStrCpy(preptitles->szSubTitle,SUB_TITLE,sizeof(preptitles->szSubTitle));
   ItiStrCpy(preptitles->szFooter,  FOOTER,   sizeof(preptitles->szFooter)  );


   /* -- Check for use of footer override option. */
   ItiRptUtGetOption (ITIRPTUT_FOOTER_SWITCH,
                      preptitles->szFooter, sizeof(preptitles->szFooter),
                      argcnt, argvars);


   return 0;
   }/* END OF FUNCTION SetTitles */







USHORT SetColumnTitles (USHORT usNumOfTitles)
   /* -- Print the column titles. */
   {
   USHORT us;
   USHORT usErr;

      /* -- Actual printing. */
   for (us=1; us <= usNumOfTitles; us++)
      {
      usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[us].usTtlCol,
                               LEFT, (acttl[us].usTtlColWth | REL_LEFT),
                               acttl[us].szTitle);
      if (usErr != 0)
         break;
      }
   ItiPrtIncCurrentLn (); //line feed

   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   ItiPrtIncCurrentLn (); //line feed

   return usErr;
   }/* END OF FUNCTION SetColumnTitles */






USHORT ConvertOrderingStr (PSZ pszOrderingStr)
   /* -- Converts OrderingStr from ColIDs into ListingQuery column value. */
   {
   switch (pszOrderingStr[0])
      {
      case 'x':
      case 'X':
      default:
      ItiStrCat(pszOrderingStr, " order by 1 ", sizeof szOrderingStr);
         break;
      }/* end switch */

   return (0);
   }/* End of Function ConvertOrderingStr */



BOOL IsZero (PSZ pszNumStr)
   {
   BOOL bool = FALSE;

   while((pszNumStr != NULL) && (*pszNumStr != '\0')
         &&((*pszNumStr == '0')||(*pszNumStr == '.')||(*pszNumStr == ' ')
            ||(*pszNumStr == ',')||(*pszNumStr == '$')))
      {
      /* inc to the next character of the string. */
      pszNumStr++;
      }

   if (pszNumStr == NULL)
      return FALSE;
   else
      return (*pszNumStr == '\0');
   }





USHORT SummarySectionFor (PSZ *ppsz,             //In
                          PSZ pszJobBrkdwnID,    //In
                          PSZ pszJobItemKey,     //Out
                          PSZ pszStdItemKey)     //Out
   {
   USHORT  usLn;
   PSZ *ppszDates = NULL;


   if (ppsz != NULL)
      {
      if ((ItiPrtKeepNLns(18) == PG_WAS_INC)
          || (ItiPrtQueryCurrentPgNum() != usCurrentPg ))
         {
         usCurrentPg = ItiPrtQueryCurrentPgNum();
         ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
         }

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_1].usTtlCol,
                       LEFT, (acttl[RPT_TTL_1].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_1].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_1].usDtaCol,
                       LEFT, (acttl[RPT_TTL_1].usDtaColWth | REL_LEFT),
                       pszJobBrkdwnID);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1, LEFT, USE_RT_EDGE, ", ");

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                       LEFT, (acttl[RPT_TTL_2].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_2].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                       LEFT, (acttl[RPT_TTL_2].usDtaColWth | REL_LEFT),
                       ppsz[JOB_ITEM_SIN]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                       LEFT, (acttl[RPT_TTL_3].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_3].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                       LEFT, USE_RT_EDGE,
                       ppsz[JOB_ITEM_DESC]);
      ItiPrtIncCurrentLn();
      usLn = ItiPrtIncCurrentLn();

      /* -- The following keys and dates are used else where. */
      ItiStrCpy(pszJobItemKey, ppsz[JOB_ITEM_KEY],   KEY_STR_LEN);
      ItiStrCpy(pszStdItemKey, ppsz[JOB_ITEM_SIKEY], KEY_STR_LEN);
      }
   else
      {
      return ITIRPTUT_ERROR;
      }


   return usLn;
   }/* End of Function SummarySectionFor */





USHORT PrintRegResults (PSZ pszUnit, PSZ *ppszRIS)
   {
   USHORT usLn, usC;
   BOOL   bNewID = TRUE;
   BOOL   bLabeled   = FALSE;
   CHAR   szRunningTotal [SMARRAY + 1] = "";
   CHAR   szTmp          [SMARRAY + 1] = "";
   CHAR   szAreaTypeKey  [SMARRAY + 1] = "";
   CHAR   szQtyAdjXBup   [SMARRAY + 1] = "";
   CHAR   szUnit         [SMARRAY + 1] = "";

   USHORT  usNumColsLoc = 0;
   PSZ   * ppszLoc      = NULL;
   PSZ     psztmp = NULL;



   /* -- This is the column for '(999.9% for Xxxxx)' of the adj/ lines. */
   usC = acttl[RPT_TTL_18].usDtaCol + acttl[RPT_TTL_18].usDtaColWth + 3;


   if (ppszRIS == NULL)
      {
      SETQERRMSG("Empty Results from CalcRegression.");

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_4].usTtlCol,
                    LEFT, (acttl[RPT_TTL_4].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_4].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_4].usDtaCol,
                    LEFT,
                    (acttl[RPT_TTL_4].usDtaColWth | REL_LEFT),
                    " \?\?\? ");

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                    LEFT, (acttl[RPT_TTL_5].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_5].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                    LEFT,
                    (acttl[RPT_TTL_5].usDtaColWth | REL_LEFT),
                    "xxxx");
      ItiStrCpy(szUnit, "xxxx", sizeof szUnit);
      ItiPrtIncCurrentLn (); //line feed


      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_6].usTtlCol,
                    LEFT, (acttl[RPT_TTL_6].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_6].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_6].usDtaCol,
                    LEFT,
                    (acttl[RPT_TTL_6].usDtaColWth | REL_LEFT),
                    " \?\?\? ");
      ItiPrtIncCurrentLn (); //line feed
      ItiPrtIncCurrentLn (); //blank line

      return 13;
      }





   /* -- Check for new page. */
   if (ItiPrtKeepNLns(16) == PG_WAS_INC)
      {
      usCurrentPg = ItiPrtQueryCurrentPgNum();
      ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
      }

   /* -- Base Price line */
   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_4].usTtlCol,
                 LEFT, (acttl[RPT_TTL_4].usTtlColWth | REL_LEFT),
                 acttl[RPT_TTL_4].szTitle);

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_4].usDtaCol,
                 LEFT,
                 (acttl[RPT_TTL_4].usDtaColWth | REL_LEFT),
                 ppszRIS[REG_BASE_PRICE_BASE]);


   if (pszUnit != NULL)
      {
      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (acttl[RPT_TTL_5].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_5].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT,
                       (acttl[RPT_TTL_5].usDtaColWth | REL_LEFT),
                       pszUnit);
      }
   ItiPrtIncCurrentLn (); //line feed


   /* -- Quantity line */
   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_6].usTtlCol,
                 LEFT, (acttl[RPT_TTL_6].usTtlColWth | REL_LEFT),
                 acttl[RPT_TTL_6].szTitle);

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_6].usDtaCol,
                 LEFT,
                 (acttl[RPT_TTL_6].usDtaColWth | REL_LEFT),
                 ppszRIS[REG_QUANTITY]);

   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                 LEFT, (acttl[RPT_TTL_7].usTtlColWth | REL_LEFT),
                 acttl[RPT_TTL_7].szTitle);


   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                 LEFT,
                 (acttl[RPT_TTL_7].usDtaColWth | REL_LEFT),
                 ppszRIS[REG_LOG_VAL]);

   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                 LEFT, (acttl[RPT_TTL_8].usTtlColWth | REL_LEFT),
                 acttl[RPT_TTL_8].szTitle);

   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                 LEFT,
                 (acttl[RPT_TTL_8].usDtaColWth | REL_LEFT),
                 ppszRIS[REG_MEDIAN_QTY]);

   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                 LEFT, (acttl[RPT_TTL_9].usTtlColWth | REL_LEFT),
                 acttl[RPT_TTL_9].szTitle);
   ItiPrtIncCurrentLn (); //line feed
   ItiPrtIncCurrentLn (); //blank line

   /* -- Unit Price = Base Price incre... */
   if (
      ((ppszRIS[REG_QTY_ADJ][0] != '-') && (ppszRIS[REG_LOG_VAL][0] != '-'))
      )
      {
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_10].usTtlCol,
                       LEFT, (acttl[RPT_TTL_10].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_10].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT,
                       (acttl[RPT_TTL_10].usDtaColWth | REL_LEFT),
                       ppszRIS[REG_QTY_ADJ]);


      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 2,
                       LEFT,
                       (acttl[RPT_TTL_7].usDtaColWth | REL_LEFT),
                       ppszRIS[REG_LOG_VAL]);
      }
   else if (
            (ppszRIS[REG_QTY_ADJ][0]=='-')&&(ppszRIS[REG_LOG_VAL][0]!='-')
           )

      {/* it it negative, so print decrease instead of increase. */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_19].usTtlCol,
                    LEFT, (acttl[RPT_TTL_19].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_19].szTitle);

      psztmp = &ppszRIS[REG_QTY_ADJ][1]; /* skip over the negative sign. */
      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                    LEFT,
                    (acttl[RPT_TTL_10].usDtaColWth | REL_LEFT),
                    psztmp);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 2,
                       LEFT,
                       (acttl[RPT_TTL_7].usDtaColWth | REL_LEFT),
                       ppszRIS[REG_LOG_VAL]);
      }
   else if (
            (ppszRIS[REG_QTY_ADJ][0]!='-')&&(ppszRIS[REG_LOG_VAL][0]=='-')
           )

      {/* it it negative, so print decrease instead of increase. */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_19].usTtlCol,
                    LEFT, (acttl[RPT_TTL_19].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_19].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                    LEFT,
                    (acttl[RPT_TTL_10].usDtaColWth | REL_LEFT),
                    ppszRIS[REG_QTY_ADJ]);

      psztmp = &ppszRIS[REG_LOG_VAL][1]; /* skip over the negative sign. */
      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 2,
                       LEFT,
                       (acttl[RPT_TTL_7].usDtaColWth | REL_LEFT),
                       psztmp);
      }
   else /* they are both negative so is increasing */
        /*((ppszRIS[REG_QTY_ADJ][0]=='-')&&(ppszRIS[REG_LOG_VAL][0]=='-')) */
      {
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_10].usTtlCol,
                       LEFT, (acttl[RPT_TTL_10].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_10].szTitle);

      psztmp = &ppszRIS[REG_QTY_ADJ][1]; /* skip over the negative sign. */
      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                    LEFT,
                    (acttl[RPT_TTL_10].usDtaColWth | REL_LEFT),
                    psztmp);

      psztmp = &ppszRIS[REG_LOG_VAL][1]; /* skip over the negative sign. */
      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 2,
                       LEFT,
                       (acttl[RPT_TTL_7].usDtaColWth | REL_LEFT),
                       psztmp);
      }




   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                 LEFT, (acttl[RPT_TTL_11].usTtlColWth | REL_LEFT),
                 acttl[RPT_TTL_11].szTitle);
   ItiPrtIncCurrentLn (); //line feed






   /* == Adjusment label lines. */
   if ((ppszRIS[REG_AREA_EXT] != NULL)
       && (!IsZero(ppszRIS[REG_AREA_EXT])) )
      {
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usTtlCol,
                    LEFT, (acttl[RPT_TTL_12].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_12].szTitle);
      ItiPrtIncCurrentLn (); //line feed
      }

   


   /* -- Season Adj. */
   if ((ppszRIS[REG_SEA_EXT] != NULL)
       && (!IsZero(ppszRIS[REG_SEA_EXT])) )
      {
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_13].usTtlCol,
                    LEFT, (acttl[RPT_TTL_13].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_13].szTitle);
      ItiPrtIncCurrentLn (); //line feed
      }



   /* -- WorkType Adj. */
   if ((ppszRIS[REG_WRK_EXT] != NULL)
       && (!IsZero(ppszRIS[REG_WRK_EXT])) )
      {
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_14].usTtlCol,
                    LEFT, (acttl[RPT_TTL_14].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_14].szTitle);
      ItiPrtIncCurrentLn (); //line feed
      }



   /* -- " = " line L */
   ItiPrtIncCurrentLn (); //blank line
   if (
        ((ppszRIS[REG_AREA_EXT] != NULL) && (!IsZero(ppszRIS[REG_AREA_EXT])))
      ||((ppszRIS[REG_SEA_EXT]  != NULL) && (!IsZero(ppszRIS[REG_SEA_EXT])))
      ||((ppszRIS[REG_WRK_EXT] != NULL) && (!IsZero(ppszRIS[REG_WRK_EXT])))
      )
      {
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_18].usTtlCol,
                    LEFT, (acttl[RPT_TTL_18].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_18].szTitle);


      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_18].usDtaCol,
                       RIGHT,
                       (acttl[RPT_TTL_18].usDtaColWth | REL_LEFT),
                       ppszRIS[REG_BASE_PRICE]);

      ItiPrtIncCurrentLn (); //line feed
      }

   


   /* -- next the Area Adjustment line. */
   if ((ppszRIS[REG_AREA_EXT] != NULL)
       && (!IsZero(ppszRIS[REG_AREA_EXT])) )
      {
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usTtlCol,
                    LEFT, (3 | REL_LEFT),
                    acttl[RPT_TTL_12].szTitle);
   
   
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_18].usDtaCol,
                       RIGHT,
                       (acttl[RPT_TTL_18].usDtaColWth | REL_LEFT),
                       ppszRIS[REG_AREA_EXT]);
   
   
   
      ItiPrtPutTextAt (CURRENT_LN, usC,
                       LEFT, (2 | REL_LEFT), "(");
   
      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                    RIGHT,
                    (acttl[RPT_TTL_11].usDtaColWth | REL_LEFT),
                    ppszRIS[REG_AREA_ADJ]);
   
      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 2,
                    LEFT,
                    (acttl[RPT_TTL_15].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_15].szTitle);
   
      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                    LEFT,
                    (acttl[RPT_TTL_15].usDtaColWth | REL_LEFT),
                    ppszRIS[REG_AREA_ID]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (2 | REL_LEFT), ")");
      ItiPrtIncCurrentLn (); //line feed
      }





   /* -- then the Season Adjustment line. */
   if ((ppszRIS[REG_SEA_EXT] != NULL)
       && (!IsZero(ppszRIS[REG_SEA_EXT])) )
      {
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usTtlCol,
                    LEFT, (3 | REL_LEFT),
                    acttl[RPT_TTL_13].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_18].usDtaCol,
                       RIGHT,
                       (acttl[RPT_TTL_18].usDtaColWth | REL_LEFT),
                       ppszRIS[REG_SEA_EXT]);



      ItiPrtPutTextAt (CURRENT_LN, usC,
                       LEFT, (2 | REL_LEFT), "(");

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                    RIGHT,
                    (acttl[RPT_TTL_11].usDtaColWth | REL_LEFT),
                    ppszRIS[REG_SEA_ADJ]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 2,
                    LEFT,
                    (acttl[RPT_TTL_16].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_16].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                    LEFT,
                    (acttl[RPT_TTL_16].usDtaColWth | REL_LEFT),
                    ppszRIS[REG_SEA_ID]);
      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (2 | REL_LEFT), ")");
      ItiPrtIncCurrentLn (); //line feed
      }




   /* -- the Work-Type Adjustment line. */
   if ((ppszRIS[REG_WRK_EXT] != NULL)
       && (!IsZero(ppszRIS[REG_WRK_EXT])) )
      {
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usTtlCol,
                    LEFT, (3 | REL_LEFT),
                    acttl[RPT_TTL_14].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_18].usDtaCol,
                       RIGHT,
                       (acttl[RPT_TTL_18].usDtaColWth | REL_LEFT),
                       ppszRIS[REG_WRK_EXT]);


      ItiPrtPutTextAt (CURRENT_LN, usC,
                       LEFT, (2 | REL_LEFT), "(");

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                    RIGHT,
                    (acttl[RPT_TTL_11].usDtaColWth | REL_LEFT),
                    ppszRIS[REG_WRK_ADJ]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 2,
                    LEFT,
                    (acttl[RPT_TTL_17].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_17].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                    LEFT,
                    (acttl[RPT_TTL_17].usDtaColWth | REL_LEFT),
                    ppszRIS[REG_WRK_ID]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (2 | REL_LEFT), ")");
      ItiPrtIncCurrentLn (); //line feed
      }


   /* -- finally the summation line. */
   ItiPrtIncCurrentLn (); //blank line 

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_18].usTtlCol,
                 LEFT, (3 | REL_LEFT),
                 acttl[RPT_TTL_18].szTitle);


   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_18].usDtaCol,
                 RIGHT,
                 (acttl[RPT_TTL_18].usDtaColWth | REL_LEFT),
                 ppszRIS[REG_REG_VAL]);
   
   if (pszUnit != NULL)
      {
      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT,
                       (3 | REL_LEFT),
                       " / ");

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT,
                       (SMARRAY | REL_LEFT),
                       pszUnit);
      }

   ItiPrtIncCurrentLn (); //line feed



   /* -- Finishout this section. */
   ItiPrtIncCurrentLn (); //blank line 
   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   usLn = ItiPrtIncCurrentLn (); //line feed

   return usLn;
   }/* End of Function PrintRegResults */











USHORT ItemsFor (PSZ pszJobKey, PSZ pszJobBrkdwnKey, PSZ pszJobBrkdwnID,
                 PSZ pszJobBrkdwnWorkType, PSZ pszJobBrkdwnWorkTypeID)
   {                      
   USHORT usLn, us, usCnt;
   PSZ   *ppsz = NULL;
   PSZ   *ppszRegResults = NULL;
   CHAR   szCurJobItemKey      [KEY_STR_LEN + 1] = "";
   CHAR   szCurStandardItemKey [KEY_STR_LEN + 1] = "";
   CHAR   szCurImportDate      [DATE_STR_LEN+ 1] = "";
   CHAR   szCurImportDateCap   [DATE_STR_LEN+ 1] = "";
   CHAR   szUnit[SMARRAY] = "";

   PSZ    apszResults[17];



   if ((pszJobKey == NULL) || (*pszJobKey == '\0')
       || (pszJobBrkdwnKey == NULL) || (*pszJobBrkdwnKey == '\0'))
      return 13;

   /* -- Build the query. */
   ItiStrCpy(szQry, szQryJobItem,  sizeof szQry);
   ItiStrCat(szQry, pszJobKey,     sizeof szQry);
   ItiStrCat(szQry, szConjJobItem, sizeof szQry);
   ItiStrCat(szQry, pszJobBrkdwnKey, sizeof szQry);
   ItiStrCat(szQry, " order by 2 ", sizeof szQry);

   hqry = ItiDbExecQuery (szQry, hhprJEReg, hmodrJEReg, ITIRID_RPT,
                          ID_QRY_JOB_ITEM, &usNumCols, &usErr);
   if (hqry == NULL)
      {
      SETQERRMSG("Query failed for PEMETH Regression JobItems in a JobBreakdown.");
      SETQERRMSG(szQry);

      usCurrentPg = ItiPrtQueryCurrentPgNum();
      ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");

      ItiPrtPutTextAt (CURRENT_LN + 3, 0,
                       CENTER, USE_RT_EDGE,
                       "Report not generated; ");
      ItiPrtPutTextAt (CURRENT_LN + 5, 0,
                       CENTER, USE_RT_EDGE,
                       "Query did not succeed. ");

      usLn = ITIRPTUT_ERROR;
      }
   else
      {
      usCnt = 0;
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
         {
         usCnt++;

         if (ppsz != NULL)
            {
            if (ppsz[JOB_ITEM_UNIT] != NULL)
               ItiStrCpy(szUnit, ppsz[JOB_ITEM_UNIT], sizeof szUnit);

            us = SummarySectionFor(ppsz,
                                   pszJobBrkdwnID,
                                   &szCurJobItemKey[0],
                                   &szCurStandardItemKey[0]);

                                   
            if (us != ITIRPTUT_ERROR)
               {
               us = CalculateRegression (hhpCalcReg,
                                         pszJobKey,           
                                         pszJobBrkdwnWorkType,
                                         pszJobBrkdwnWorkTypeID,
                                         ppsz[JOB_ITEM_KEY], 
                                         ppsz[JOB_ITEM_SIKEY],
                                         &apszResults[0],
                                         &apszResults[1],
                                         &apszResults[2],
                                         &apszResults[3],
                                         &apszResults[4],
                                         &apszResults[5],
                                         &apszResults[6],
                                         &apszResults[7],
                                         &apszResults[8],
                                         &apszResults[9],
                                         &apszResults[10],
                                         &apszResults[11],
                                         &apszResults[12],
                                         &apszResults[13],
                                         &apszResults[14],
                                         &apszResults[15]);
               if (us == 0)
                  PrintRegResults(szUnit, apszResults);
               }


            }

         ItiFreeStrArray (hhprJEReg, ppsz, usNumCols);
         }/* End of while loop */

      if (usCnt == 0)
         {
//         SETQERRMSG("Empty Query Results for PEMETHAvg. JobItem in a JobBreakdown.");
//         SETQERRMSG(szQry);
//         
//         usCurrentPg = ItiPrtQueryCurrentPgNum();
//         ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
//
//         ItiPrtPutTextAt (CURRENT_LN + 3, 0,
//                          CENTER, USE_RT_EDGE,
//                          "-- Report not generated --");
//         ItiPrtPutTextAt (CURRENT_LN + 5, 0,
//                          CENTER, USE_RT_EDGE,
//                          "No data found that fits search criteria.");
//
         usLn = ITIRPTUT_ERROR;
         }


      }/* End of else clause */

   return us;
   }/* End of Function ItemsFor */








USHORT PrintProc (HWND hwnd, MPARAM mp1, MPARAM mp2)
   {
   USHORT us = 13;
                   /* Not used in this report. */
   return (us);

   }/* End of PrintProc Function */










/* ======================================================================= *\
 *                                                    EXPORTED FUNCTIONS   *
\* ======================================================================= */
/* -- ItiDllQueryVersion returns the version number of this DLL. */
USHORT EXPORT ItiDllQueryVersion (VOID)
   {
   return (DLL_VERSION);
   }


/*
 * ItiDllQueryCompatibility returns TRUE if this DLL is compatable
 * with the caller's version numer, otherwise FALSE is returned.
 */

BOOL EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion)

   {
   DBGMSG(" *** In DLL version check function.");

   if ( ItiVerCmp(szDllVersion) )
      return (TRUE);

   return (FALSE);
   }








VOID EXPORT ItiDllQueryMenuName (PSZ     pszBuffer,   
                                 USHORT  usMaxSize,   
                                 PUSHORT pusWindowID) 
   {                                                  
   ItiStrCpy (pszBuffer, "JobEst:(PEMETH Regression)", usMaxSize);   
   *pusWindowID = rJEReg_RptDlgBox;                       
   }/* END OF FUNCTION */




BOOL EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID, 
                                     USHORT usActiveWindowID) 
   {                                                          
      return FALSE;                                           
   }/* END OF FUNCTION */


MRESULT EXPORT ItiDllrJERegRptDlgProc (HWND     hwnd,     
                                    USHORT   usMessage,
                                    MPARAM   mp1,
                                    MPARAM   mp2)
   {
   USHORT us = 0;
   CHAR   szCmdLn  [LGARRAY+1] = RPT_DLL_NAME " ";
   CHAR   szJobKeyStr [SMARRAY+1] = "";

   szJobKeyStr[0] = '\0';

   switch (usMessage)
      {
      case WM_INITDLG:
         us = ItiRptUtInitDlgBox (hwnd, RPT_RJEREG_SESSION, 0, NULL);
         break;


      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_PRINT:
               WinSetPointer (HWND_DESKTOP,
                           WinQuerySysPointer (HWND_DESKTOP, SPTR_WAIT, 0));

               us = PrintProc(hwnd, mp1, mp2);

               WinSetPointer (HWND_DESKTOP,
                           WinQuerySysPointer (HWND_DESKTOP, SPTR_ARROW, 0));

               if (us == 0)
                  {
                  WinDismissDlg (hwnd, TRUE);
                  return 0;
                  }
               break;


            case DID_CANCEL:
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;

            }/* end of switch(SHORT1... */

         break;

      default:
         us = ItiRptUtChkDlgMsg (hwnd, usMessage, mp1, mp2);
         break;
      }

   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);

   }/* END OF FUNCTION ItiDll...RptDlgProc */








USHORT EXPORT ItiRptDLLPrintReport
                 (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[])
   {
   PSZ    pszJobKey = szJobKey;
   USHORT us = 0;
   USHORT usBKindex;



   /* ------------------------------------------------------------------- *\
    *  Create a memory heap.                                              *
   \* ------------------------------------------------------------------- */
   hhprJEReg  = ItiMemCreateHeap (MIN_HEAP_SIZE);
   hhpCalcReg = ItiMemCreateHeap (MIN_HEAP_SIZE);


   /* -- Get any query ordering information, BEFORE setting rpt titles. */
   ItiRptUtGetOption (ITIRPTUT_ORDERING_SWITCH, szOrderingStr, LGARRAY,
                      argcnt, argvars);
   ConvertOrderingStr (szOrderingStr);


   /* -- Get the report's titles for this DLL. */
   ItiRptUtLoadLabels (hmodrJEReg, &usNumOfTitles, &acttl[0]);

   /* -- Set the report's titles for this DLL. */
   SetTitles(prept, argcnt, argvars);



   


   /* ------------------------------------------------------------------- *\
    * Do any run-time formatting then start the report.                   *
   \* ------------------------------------------------------------------- */
   us =  ItiPrtLoadDLLInfo(hab, hmq, prept, argcnt, argvars);
   if (us != 0 )
      {
      DBGMSG("FAILED to ItiPrtDLLLoadInfo.");
      return (ITIPRT_NOT_INITIALIZED);
      }

   ItiPrtBeginReport (PRT_Q_NAME);


   /* -- Initialize the query strings. */
   ItiMbQueryQueryText(hmodrJEReg, ITIRID_RPT, ID_QRY_TITLE_NAMES,
                       szQryTitleNames, sizeof szQryTitleNames );

   ItiMbQueryQueryText(hmodrJEReg, ITIRID_RPT, ID_QRY_JOB_BRKDWN,
                       szQryJobBrkdwn, sizeof szQryJobBrkdwn );

   ItiMbQueryQueryText(hmodrJEReg, ITIRID_RPT, ID_QRY_JOB_ITEM,
                       szQryJobItem, sizeof szQryJobItem );

   ItiMbQueryQueryText(hmodrJEReg, ITIRID_RPT, ID_CONJ_JOB_ITEM,
                       szConjJobItem, sizeof szConjJobItem );

   ItiMbQueryQueryText(hmodrJEReg, ITIRID_RPT, ID_QRY_BPQ,
                       szQryBPQ, sizeof szQryBPQ );

   ItiMbQueryQueryText(hmodrJEReg, ITIRID_RPT, ID_CONJ_BPQ_CAP,
                       szConjBPQCap, sizeof szConjBPQCap );

   ItiMbQueryQueryText(hmodrJEReg, ITIRID_RPT, ID_CONJ_BPQ_KEY,
                       szConjBPQKey, sizeof szConjBPQKey );

   ItiMbQueryQueryText(hmodrJEReg, ITIRID_RPT, ID_CONJ_BPQ_ITEM_KEY,
                       szConjBPQItemKey, sizeof szConjBPQItemKey );

   ItiMbQueryQueryText(hmodrJEReg, ITIRID_RPT, ID_CONJ_BPQ_BRKDWN_KEY,
                       szConjBPQBrkdwnKey, sizeof szConjBPQBrkdwnKey );



   ItiMbQueryQueryText(hmodrJEReg, ITIRID_RPT, ID_SLC_ADJ,
                       szSlcAdj, sizeof szSlcAdj );

   ItiMbQueryQueryText(hmodrJEReg, ITIRID_RPT, ID_SLC_ADJ_RAW,
                       szSlcAdjRaw, sizeof szSlcAdjRaw );

   ItiMbQueryQueryText(hmodrJEReg, ITIRID_RPT, ID_QRY_AREA,
                       szQryArea, sizeof szQryArea );

   ItiMbQueryQueryText(hmodrJEReg, ITIRID_RPT, ID_CONJ_AREA_CKEY,
                       szConjAreaCKey, sizeof szConjAreaCKey );

   ItiMbQueryQueryText(hmodrJEReg, ITIRID_RPT, ID_CONJ_AREA_SIKEY,
                       szConjAreaSIKey, sizeof szConjAreaSIKey );


   ItiMbQueryQueryText(hmodrJEReg, ITIRID_RPT, ID_QRY_SEASON,
                       szQrySeason, sizeof szQrySeason );

   ItiMbQueryQueryText(hmodrJEReg, ITIRID_RPT, ID_CONJ_SEASON,
                       szConjSeason, sizeof szConjSeason );


   ItiMbQueryQueryText(hmodrJEReg, ITIRID_RPT, ID_QRY_WORKTYPE,
                       szQryWorkType, sizeof szQryWorkType );

   ItiMbQueryQueryText(hmodrJEReg, ITIRID_RPT, ID_CONJ_WORKTYPE,
                       szConjWorkType, sizeof szConjWorkType );


   ItiMbQueryQueryText(hmodrJEReg, ITIRID_RPT, ID_CONJ_ADJ_IMPDATE,
                       szConjAdjImpDate, sizeof szConjAdjImpDate );

   ItiMbQueryQueryText(hmodrJEReg, ITIRID_RPT, ID_CONJ_ADJ_IMPDATE_CAP,
                       szConjAdjImpDateCap, sizeof szConjAdjImpDateCap );


   ItiMbQueryQueryText(hmodrJEReg, ITIRID_RPT, ID_QRY_IMP,
                       szQryImp, sizeof szQryImp );



   /* -------------------------------------------------------------------- *\
    * -- For each Key...                                                   *
   \* -------------------------------------------------------------------- */
   ItiRptUtInitKeyList(argcnt, argvars); /*  */
   while (ItiRptUtGetNextKey(pszJobKey,sizeof(szJobKey)) != ITIRPTUT_NO_MORE_KEYS)
      {
      /* -- Breakdowns */
      ItiStrCpy(szQryBDKeys, szQryJobBrkdwn, sizeof szQry);
      ItiStrCat(szQryBDKeys, pszJobKey, sizeof szQry);


      /* -- Do the database query. */
      hqryBDKeys = ItiDbExecQuery (szQryBDKeys, hhprJEReg, hmodrJEReg, ITIRID_RPT,
                                   ID_QRY_JOB_BRKDWN, &usNumColsBDKeys, &usErr);
      if (hqryBDKeys == NULL)
         {
         SETQERRMSG(szQryBDKeys);
         break;
         }                     
      else
         {
         usIndex_appsz = 0;  
         while( ItiDbGetQueryRow (hqryBDKeys, &ppszBDKeys, &usErr) )
            {
            appsz[usIndex_appsz] = ItiStrMakePPSZ (hhprJEReg, 4,
                                    ppszBDKeys[JOB_BRKDWN_KEY],
                                    ppszBDKeys[JOB_BRKDWN_ID],
                                    ppszBDKeys[JOB_BRKDWN_WORK_TYPE],
                                    ppszBDKeys[JOB_BRKDWN_WORK_TYPE_ID]);

            ItiFreeStrArray (hhprJEReg, ppszBDKeys, usNumColsBDKeys);
            usIndex_appsz++;
            }

         /* -- Now, with each breakdown in the list... */
         usBKindex = 0;
         while (usBKindex < usIndex_appsz)
            {
            ItemsFor (pszJobKey,
                      appsz[usBKindex][JOB_BRKDWN_KEY],
                      appsz[usBKindex][JOB_BRKDWN_ID],
                      appsz[usBKindex][JOB_BRKDWN_WORK_TYPE],
                      appsz[usBKindex][JOB_BRKDWN_WORK_TYPE_ID]);

            usBKindex++; /* Increment the index to the next breakdown. */
            }

         }/* end of else clause */

      }/* End of WHILE GetNextKey */



   /* ------------------------------------------------------------------- *\
    * Finish up and quit.                                                 *
   \* ------------------------------------------------------------------- */
   ItiPrtEndReport();

   DBGMSG("Exit ***DLL function ItiRptDLLPrintReport");
   return 0;

   }/* END OF FUNCTION ItiRptDLLPrintReport */
