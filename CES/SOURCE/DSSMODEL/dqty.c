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
 * dqty.c
 *
 * This is the dss parametric profile analysis model.
 */

#define INCL_DOS

#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"
#include "..\include\itimbase.h"
#include "..\include\itidbase.h"
#include "..\include\colid.h"
#include "..\include\itiglob.h"
#include "..\include\itirpt.h"
#include "..\include\itifmt.h"
#include "..\include\paracalc.h"
#include <stdio.h>
#include <string.h>
#include <share.h>

#include "dQty.h"
#include "gauss.h"

#include "..\include\itiimp.h"


#define AUL_CNT_SIZE  11


/* ======================================================================= *\
 *                                             MODULE'S GLOBAL VARIABLES   *
\* ======================================================================= */

/*
 * hmod is the handle to this DLL
 */

HMODULE hmod = -1;
PGLOBALS pglobals = NULL;

static  CHAR     szCompileDateTime[] = __DATE__ " " __TIME__ ;
static CHAR szDllVersion[] = "1.1b0 dqty.dll";

static  COLTTLS  acttl[32];
static  USHORT   usCurrentPg = 0;

// static  PMATRIX  pmatDMIQReg, pmatDMIQResults;
   static ULONG    ulNumObsDMIQ;
   static ULONG    ulNumObsStruc;

static BOOL bUseParaConv = FALSE;


#define  FACTOR_SIZE  14
static   CHAR szBaseQty   [2 * FACTOR_SIZE] = " ";
static   CHAR szSlope     [ FACTOR_SIZE + 1 ] = "";  
static   CHAR szIntercept [ FACTOR_SIZE + 1 ] = "";  


USHORT EXPORT SetTitles (PREPT preptitles, INT argcnt, CHAR * argvars[])
   {
   USHORT us = 1;
   CHAR szOpt[FOOTER_LEN + 1];
   PSZ  pszOpt = szOpt;

   if (preptitles == NULL)
      return ITIRPT_INVALID_PARM;

   ItiStrCpy (preptitles->szTitle,
              TITLE,
              sizeof preptitles->szTitle);

   //ItiStrCpy (preptitles->szSubTitle, "", sizeof preptitles->szSubTitle);
   preptitles->szSubTitle [0] = '\0';
   preptitles->szSubTitle [1] = '\0';

   //ItiStrCpy (preptitles->szFooter, "", sizeof preptitles->szFooter);
   preptitles->szFooter [0] = '\0';
   preptitles->szFooter [1] = '\0';


   /* -- Set the subsubtitle text to be empty for this report. */
   preptitles->szSubSubTitle [0] = '\0';
   preptitles->szSubSubTitle [1] = '\0';

   /* -- Check for use of footer override option. */
   ItiRptUtGetOption (ITIRPTUT_FOOTER_SWITCH,
                      preptitles->szFooter, sizeof preptitles->szFooter,
                      argcnt, argvars);
   return 0;
   }





USHORT SetColumnTitles (void)
   /* -- Print the column titles. */
   {
   USHORT us;

   for (us = 1; us <= 5; us++)
      {
      ItiPrtPutTextAt (CURRENT_LN, acttl[us].usTtlCol,
                       LEFT, (acttl[us].usTtlColWth | REL_LEFT),
                       acttl[us].szTitle);

      if (us == RPT_TTL_4)
         ItiPrtIncCurrentLn ();
      }

   ItiPrtIncCurrentLn ();
   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   ItiPrtIncCurrentLn ();

   return 0;
   }


DOUBLE EXPORT  Basis (USHORT i,
                     DOUBLE dLength,
                     DOUBLE dDepth,
                     DOUBLE dWidth,
                     DOUBLE dQty)
   {
   DOUBLE dConvLength = 5280.0;     /* miles to feet conversion */
   DOUBLE dConvDepth = 1.0 / 12.0;  /* inches to feet conversion */

   switch (i)
      {
      case 0:
         //return 1.0;
         if (bUseParaConv)
            return dLength * dConvLength;

         return dLength;
         break;

      case 1:
         //return dLength;
         if (bUseParaConv)
            return (dLength*dConvLength) * (dDepth*dConvDepth);

         return (dLength * dDepth);
         break;

      case 2:
         //return dDepth;
         if (bUseParaConv)
            return (dLength * dConvLength) * dWidth;

         return (dLength * dWidth);
         break;

      case 3:
         //return dWidth;
         if (bUseParaConv)
            return (dLength*dConvLength) * dWidth * (dDepth*dConvDepth);

         return (dLength * dWidth * dDepth);
         break;

      case 4:
         //return dQty;
         return 1;

      default:
         return 0;
      }
   }


/*
 *  The regression model equation: 
 *     /\
 *    Qty = B  + B Length + B Depth + B Width
 *           0    1          2         3
 *
 *  The matrix equation set:
 *
 *   nb        + b ä(Len)      + b ä(Dpth)     + b ä(Wth)      = ä(Qty)
 *     0          1               2               3
 *
 *   b ä(Len)  + b ä(Lený)     + b ä(Len*Dpth) + b ä(Len*Wth)  = ä(Qty*Len)
 *    0           1               2               3             
 *
 *   b ä(Dpth) + b ä(Dpth*Len) + b ä(Dpthý)    + b ä(Dpth*Wth) = ä(Qty*Dpth)
 *    0           1               2               3          
 *
 *   b ä(Wth)  + b ä(Wth*Len)  + b ä(Wth*Dpth) + b ä(Wthý)    = ä(Qty*Wth)
 *    0           1               2               3      
 */

BOOL EXPORT  DoRegression (HHEAP     hheap,
                          PSZ       pszWhere,
                          PSZ       pszMajorItemKey,
                          PULONG    pulNumObs,
                          PMATRIX   pmat,
                          PMATRIX   pmatResults)
   {            /* call this for each major item (uses one db connection) */
   USHORT   i, j;
   DOUBLE   dQuantity, dLength, dDepth, dWidth;
   PSZ      *ppsz;
   HQRY     hqry;
   USHORT   usNumCols, usError;
   BOOL     bResult = FALSE; 
   CHAR     szQuery [6000] = "";
   ULONG    ulObs = 0UL;

   *pulNumObs = 0UL;

   ItiMbQueryQueryText (hmod, ITIRID_RPT, GET_MAJOR_ITEM_INFO,
                        szQuery, sizeof szQuery);
   ItiStrCat (szQuery, pszWhere, sizeof szQuery);
   ItiStrCat (szQuery, " AND HPMI.MajorItemKey = ", sizeof szQuery);
   ItiStrCat (szQuery, pszMajorItemKey, sizeof szQuery);
   ItiStrCat (szQuery, " ORDER BY HPMI.Quantity ", sizeof szQuery);

   hqry = ItiDbExecQuery (szQuery, hheap, hmod, ITIRID_RPT,
                          GET_MAJOR_ITEM_INFO, &usNumCols, &usError);
   if (hqry == NULL)
      return FALSE;

   while (ItiDbGetQueryRow (hqry, &ppsz, &usError))
      {
      // (*pulNumObs) += 1;
      ulObs++;

      dQuantity = ItiStrToDouble (ppsz [0]);
      dLength   = ItiStrToDouble (ppsz [1]);
      dDepth    = ItiStrToDouble (ppsz [2]);
      dWidth    = ItiStrToDouble (ppsz [3]);

      for (i=0; i < pmat->usRows; i++)
         {
         for (j=0; j < pmat->usCols; j++)
            pmat->pdMat [i][j] += Basis (i, dLength, dDepth, dWidth, dQuantity) *
                                  Basis (j, dLength, dDepth, dWidth, dQuantity);

         //  pmat->pdMat [i][pmat->usCols-1] += dQuantity * 
         //                                   Basis (i, dLength, dDepth, dWidth, dQuantity);
         }
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      } /* end while */

printf ("\n MajorItemKey: %s \n", pszMajorItemKey);
   bResult = SolveMatrix (pmat, pmatResults);

   (*pulNumObs) = ulObs;

   return bResult;
   }

// 
// void EXPORT  GetHistogram (HHEAP     hheap, 
//                           PSZ       pszWhere, 
//                           PSZ       pszMajorItemKey, 
//                           PMATRIX   pmat, 
//                           ULONG     aulCnt [AUL_CNT_SIZE])
//    {
//    CHAR     szQuery [1300] = "";
//    CHAR     szTemp  [1200] = "";
//    CHAR     szBase  [10] = " 1.0 ";
//    CHAR     szDepth [10] = " 1.0 ";
//    CHAR     szWidth [10] = " 1.0 ";
//    CHAR     szCross [10] = " 1.0 ";
//    HQRY     hqry;
//    USHORT   i, uCols, uErr, usCnt;
//    PSZ      *ppsz, pszRes, apszTokens [7], apszValues [7];
//    DOUBLE   d = 0.0;
// #define NUMBER_OF_P_KEYS 400
// #define SZKEY  8
//    CHAR aszKeys  [NUMBER_OF_P_KEYS + 1][SZKEY];
// 
//    for (i=0; i < AUL_CNT_SIZE; i++)
//       aulCnt [i] = 0UL; 
// 
// //   if (pmat->pdMat[0][0] != 0.0)
// //      sprintf (szBase,  "%lf", pmat->pdMat [0][0]);
// //
// //   if (pmat->pdMat[1][0] != 0.0)
// //      sprintf (szDepth, "%lf", pmat->pdMat [1][0]);
// //
// //   if (pmat->pdMat[2][0] != 0.0)
// //      sprintf (szWidth, "%lf", pmat->pdMat [2][0]);
// //
// //   if (pmat->pdMat[3][0] != 0.0)
// //      sprintf (szCross, "%lf", pmat->pdMat [3][0]);
// 
//    apszTokens [0] = "Base";           apszValues [0] = szBase;
//    apszTokens [1] = "Depth";          apszValues [1] = szDepth;
//    apszTokens [2] = "Width";          apszValues [2] = szWidth;
//    apszTokens [3] = "ProposalKey";    apszValues [3] = NULL;
//    apszTokens [4] = "MajorItemKey";   apszValues [4] = pszMajorItemKey;
//    apszTokens [5] = "Where";          apszValues [5] = pszWhere;
//    apszTokens [6] = NULL;             apszValues [6] = NULL;
// 
// 
//    /* -- Build list of proposal keys for this major item. */
// 
//    sprintf (szTemp,
//      "SELECT distinct HPMI.ProposalKey "
//      "FROM HistoricalProposal HP, "
//      "HistoricalProposalMajorItem HPMI "
//      "WHERE HP.ProposalKey = HPMI.ProposalKey "
//      "AND HPMI.Quantity > 0 "
//      "AND HP.ProposalLength > 0 "
//      "AND HP.AveragePavementDepth > 0 "
//      "AND HP.AveragePavementWidth > 0 "
//      "AND HPMI.MajorItemKey = %s "
//       , pszMajorItemKey);
// 
// 
// //   sprintf (szTemp,
// //      "/* Build proposal key list. */ "
// //      "SELECT distinct ProposalKey "
// //      " FROM HistoricalProposalMajorItem "
// //      " WHERE MajorItemKey = %s "
// //      , pszMajorItemKey);
// 
//    if (!(hqry = ItiDbExecQuery(szTemp, hheap, 0, 0, 0, &uCols, &uErr)))
//       {
//       return;
//       }
// 
//    aszKeys[0][0] = '\0';
//    usCnt = 1;
//    while (ItiDbGetQueryRow(hqry, &ppsz, &uErr)) 
//       {
//       if (usCnt <= NUMBER_OF_P_KEYS)
//          {
//          ItiStrCpy(aszKeys[usCnt], ppsz[0], SZKEY);  
//          aszKeys[usCnt+1][0] = '\0';
//          }
// 
//       ItiFreeStrArray(hheap, ppsz, uCols); 
//       usCnt++;
//       }
// /////////////////////////////////
// 
//    ItiMbQueryQueryText(hmod, ITIRID_RPT, DO_HISTOGRAM, szTemp, sizeof szTemp);
// 
//    usCnt--;
//    while (usCnt > 0)
//       {
//       apszValues [3] = aszKeys[usCnt];
// 
//       ItiStrReplaceParams
//          (szQuery,szTemp,sizeof szQuery,apszTokens,apszValues,6);
// 
//       pszRes = ItiDbGetRow1Col1(szQuery, hheap, 0, 0, 0);
//       if ((NULL != pszRes) && (pszRes[0] != '\0'))
//          {
//          d = ItiStrToDouble (pszRes);  
//          i = (int) d;
//          if (i > 9)  
//             i = 10;
//          aulCnt[i] = aulCnt[i] + 1UL;
//          }
// 
//       usCnt--;
//       } /* end of while loop */
// 
// 
// //   hqry = ItiDbExecQuery (szQuery, hheap, hmod, ITIRID_RPT,
// //                          DO_HISTOGRAM, &usNumCols, &usError);
// //   if (hqry == NULL)
// //      return;
// //
// //   while (ItiDbGetQueryRow (hqry, &ppsz, &usError))
// //      {
// //      d = ItiStrToDouble (ppsz[0]);  
// //      i = (int) d;
// //      if (i > 9)  
// //         i = 10;
// //      aulCnt[i] = aulCnt[i] + 1UL;
// //      ItiFreeStrArray (hheap, ppsz, usNumCols);
// //      }
// 
//    return;
//    } /* -- End of Function GetHistogram */





void EXPORT GetHistogram (HHEAP     hheap, 
                          PSZ       pszWhere, 
                          PSZ       pszMajorItemKey, 
                          PMATRIX   pmat, 
                          ULONG     ulCount [10])
   {
   CHAR   szQuery [2040] = "";
   CHAR   szQBase [1020] = "";
   CHAR   szTmp[250] = "";
   CHAR   szEstAmt[64] = "";
   CHAR   szH10[220] = "";
   CHAR   szH20[220] = "";
   CHAR   szH30[220] = "";
   CHAR   szH40[220] = "";
   CHAR   szH50[220] = "";
   CHAR   szH60[220] = "";
   CHAR   szH70[220] = "";
   CHAR   szH80[220] = "";
   CHAR   szH90[220] = "";
   CHAR   szH100[220] = "";
   USHORT usColCnt, usError, i;
   HQRY  hqry2;
   PPSZ  ppszRes;
   DOUBLE dVal;
   PSZ pszMarker = NULL;


   for (i=0; i <= AUL_CNT_SIZE; i++)
      ulCount [i] = 0L;

   i = 0;

   ItiStrCpy(szQuery,  " /* GetHistogram */ "
     " select EngineersEstimate / AwardedBidTotal from HistoricalProposal HP "
     " where HP.ProposalKey IN (select ProposalKey from HistoricalProposalMajorItem "
                              " where MajorItemKey = "
     , sizeof szQuery);

   ItiStrCat(szQuery, pszMajorItemKey, sizeof szQuery);
   ItiStrCat(szQuery," ) and AwardedBidTotal > 0 and EngineersEstimate > 0 ", sizeof szQuery);

   ItiStrCpy (szQBase, pszWhere, sizeof szQBase);
   /* -- Strip of the "AND HPMI.MajorItemKey..." end of the where clause. */ 
   pszMarker = strstr (szQBase, "AND HPMI.MajorItemKey" );
   if (NULL != pszMarker)
      *pszMarker = '\0';
   ItiStrCat(szQuery, szQBase, sizeof szQuery);


   hqry2 = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usColCnt, &usError);
   if (hqry2 != NULL)
      while (ItiDbGetQueryRow (hqry2, &ppszRes, &usError))
         {
         if (ppszRes == NULL) 
            continue;

         ItiErrWriteDebugMessage (ppszRes[0]);
             
         dVal = ItiStrToDouble (ppszRes[0]);
         if (dVal > 1.0)
            dVal = dVal - 1.0;
         else
            dVal = 1.0 - dVal;

         /* -- Within 10 percent range. */
         if ((dVal >= 0.0) && (dVal < 0.1))
            ulCount[0] = ulCount[0] + 1L;
      
         /* -- Within 20 percent range. */
         if ((dVal >= 0.1) && (dVal < 0.2))
            ulCount[1] = ulCount[1] + 1L;
      
         /* -- Within 30 percent range. */
         if ((dVal >= 0.2) && (dVal < 0.3))
            ulCount[2] = ulCount[2] + 1L;
      
         /* -- Within 40 percent range. */
         if ((dVal >= 0.3) && (dVal < 0.4))
            ulCount[3] = ulCount[3] + 1L;
      
         /* -- Within 50 percent range. */
         if ((dVal >= 0.4) && (dVal < 0.5))
            ulCount[4] = ulCount[4] + 1L;
      
         /* -- Within 60 percent range. */
         if ((dVal >= 0.5) && (dVal < 0.6))
            ulCount[5] = ulCount[5] + 1L;
      
         /* -- Within 70 percent range. */
         if ((dVal >= 0.6) && (dVal < 0.7))
            ulCount[6] = ulCount[6] + 1L;
      
         /* -- Within 80 percent range. */
         if ((dVal >= 0.7) && (dVal < 0.8))
            ulCount[7] = ulCount[7] + 1L;
      
         /* -- Within 90 percent range. */
         if ((dVal >= 0.8) && (dVal < 0.9))
            ulCount[8] = ulCount[8] + 1L;
      
         /* -- Greater than the 90 percent range. */
         if (dVal >= 0.9)
            ulCount[9] = ulCount[9] + 1L;
  
         ItiFreeStrArray (hheap, ppszRes, usColCnt);
         }

   ItiErrWriteDebugMessage ("       /* Finished in GetHistogram */  ");

   return;

//   // Old development code, why is this kept??
//   /* - The estimated amount of the major item which is to
//      - be compared to the major items in the database.
//      - Where does this value come from??                 */
//   ItiStrCpy(szEstAmt, " 100 ", sizeof szEstAmt);
//
//   ItiMbQueryQueryText(hmod,ITIRID_RPT, DO_HISTOGRAM, szQBase, sizeof szQBase);
//
//   ItiMbQueryQueryText(hmod,ITIRID_RPT, DO_HISTOGRAM_10, szH10, sizeof szH10);
//   ItiMbQueryQueryText(hmod,ITIRID_RPT, DO_HISTOGRAM_20, szH20, sizeof szH20);
//   ItiMbQueryQueryText(hmod,ITIRID_RPT, DO_HISTOGRAM_30, szH30, sizeof szH30);
//   ItiMbQueryQueryText(hmod,ITIRID_RPT, DO_HISTOGRAM_40, szH40, sizeof szH40);
//   ItiMbQueryQueryText(hmod,ITIRID_RPT, DO_HISTOGRAM_50, szH50, sizeof szH50);
//   ItiMbQueryQueryText(hmod,ITIRID_RPT, DO_HISTOGRAM_60, szH60, sizeof szH60);
//   ItiMbQueryQueryText(hmod,ITIRID_RPT, DO_HISTOGRAM_70, szH70, sizeof szH70);
//   ItiMbQueryQueryText(hmod,ITIRID_RPT, DO_HISTOGRAM_80, szH80, sizeof szH80);
//   ItiMbQueryQueryText(hmod,ITIRID_RPT, DO_HISTOGRAM_90, szH90, sizeof szH90);
//   ItiMbQueryQueryText(hmod,ITIRID_RPT, DO_HISTOGRAM_100, szH100, sizeof szH100);
//            
//   i = 0;
//   // ItiStrCat(pszMajorItemKey, " ", sizeof szQuery);
//  
//   /* -- Within 10 percent range. */
//   ItiStrCpy(szQuery, szQBase, sizeof szQuery);
//   ItiStrCat(szQuery, pszMajorItemKey, sizeof szQuery);
//   ItiStrCat(szQuery, pszWhere, sizeof szQuery);
//   sprintf(szTmp, szH10, szEstAmt, szEstAmt);
//   ItiStrCat(szQuery, szTmp, sizeof szQuery);
//   ulCount[i++] = ItiDbGetQueryCount (szQuery, &usNumCols, &usError);
//  
//   /* -- Within 20 percent range. */
//   ItiStrCpy(szQuery, szQBase, sizeof szQuery);
//   ItiStrCat(szQuery, pszMajorItemKey, sizeof szQuery);
//   ItiStrCat(szQuery, pszWhere, sizeof szQuery);
//   sprintf(szTmp, szH20, szEstAmt, szEstAmt, szEstAmt, szEstAmt);
//   ItiStrCat(szQuery, szTmp, sizeof szQuery);
//   ulCount[i++] = ItiDbGetQueryCount (szQuery, &usNumCols, &usError);
//  
//   /* -- Within 30 percent range. */
//   ItiStrCpy(szQuery, szQBase, sizeof szQuery);
//   ItiStrCat(szQuery, pszMajorItemKey, sizeof szQuery);
//   ItiStrCat(szQuery, pszWhere, sizeof szQuery);
//   sprintf(szTmp, szH30, szEstAmt, szEstAmt, szEstAmt, szEstAmt);
//   ItiStrCat(szQuery, szTmp, sizeof szQuery);
//   ulCount[i++] = ItiDbGetQueryCount (szQuery, &usNumCols, &usError);
//  
//   /* -- Within 40 percent range. */
//   ItiStrCpy(szQuery, szQBase, sizeof szQuery);
//   ItiStrCat(szQuery, pszMajorItemKey, sizeof szQuery);
//   ItiStrCat(szQuery, pszWhere, sizeof szQuery);
//   sprintf(szTmp, szH40, szEstAmt, szEstAmt, szEstAmt, szEstAmt);
//   ItiStrCat(szQuery, szTmp, sizeof szQuery);
//   ulCount[i++] = ItiDbGetQueryCount (szQuery, &usNumCols, &usError);
//  
//   /* -- Within 50 percent range. */
//   ItiStrCpy(szQuery, szQBase, sizeof szQuery);
//   ItiStrCat(szQuery, pszMajorItemKey, sizeof szQuery);
//   ItiStrCat(szQuery, pszWhere, sizeof szQuery);
//   sprintf(szTmp, szH50, szEstAmt, szEstAmt, szEstAmt, szEstAmt);
//   ItiStrCat(szQuery, szTmp, sizeof szQuery);
//   ulCount[i++] = ItiDbGetQueryCount (szQuery, &usNumCols, &usError);
//  
//   /* -- Within 60 percent range. */
//   ItiStrCpy(szQuery, szQBase, sizeof szQuery);
//   ItiStrCat(szQuery, pszMajorItemKey, sizeof szQuery);
//   ItiStrCat(szQuery, pszWhere, sizeof szQuery);
//   sprintf(szTmp, szH60, szEstAmt, szEstAmt, szEstAmt, szEstAmt);
//   ItiStrCat(szQuery, szTmp, sizeof szQuery);
//   ulCount[i++] = ItiDbGetQueryCount (szQuery, &usNumCols, &usError);
//  
//   /* -- Within 70 percent range. */
//   ItiStrCpy(szQuery, szQBase, sizeof szQuery);
//   ItiStrCat(szQuery, pszMajorItemKey, sizeof szQuery);
//   ItiStrCat(szQuery, pszWhere, sizeof szQuery);
//   sprintf(szTmp, szH70, szEstAmt, szEstAmt, szEstAmt, szEstAmt);
//   ItiStrCat(szQuery, szTmp, sizeof szQuery);
//   ulCount[i++] = ItiDbGetQueryCount (szQuery, &usNumCols, &usError);
//  
//   /* -- Within 80 percent range. */
//   ItiStrCpy(szQuery, szQBase, sizeof szQuery);
//   ItiStrCat(szQuery, pszMajorItemKey, sizeof szQuery);
//   ItiStrCat(szQuery, pszWhere, sizeof szQuery);
//   sprintf(szTmp, szH80, szEstAmt, szEstAmt, szEstAmt, szEstAmt);
//   ItiStrCat(szQuery, szTmp, sizeof szQuery);
//   ulCount[i++] = ItiDbGetQueryCount (szQuery, &usNumCols, &usError);
//  
//   /* -- Within 90 percent range. */
//   ItiStrCpy(szQuery, szQBase, sizeof szQuery);
//   ItiStrCat(szQuery, pszMajorItemKey, sizeof szQuery);
//   ItiStrCat(szQuery, pszWhere, sizeof szQuery);
//   sprintf(szTmp, szH90, szEstAmt, szEstAmt, szEstAmt, szEstAmt);
//   ItiStrCat(szQuery, szTmp, sizeof szQuery);
//   ulCount[i++] = ItiDbGetQueryCount (szQuery, &usNumCols, &usError);
//  
//   /* -- Greater than the 90 percent range. */
//   ItiStrCpy(szQuery, szQBase, sizeof szQuery);
//   ItiStrCat(szQuery, pszMajorItemKey, sizeof szQuery);
//   ItiStrCat(szQuery, pszWhere, sizeof szQuery);
//   sprintf(szTmp, szH100, szEstAmt, szEstAmt);
//   ItiStrCat(szQuery, szTmp, sizeof szQuery);
//   ulCount[i++] = ItiDbGetQueryCount (szQuery, &usNumCols, &usError);
//
//
   }/* End of function GetHistogram */




/*
 */

void EXPORT  DoWithin (HHEAP   hheap, 
                      PSZ     pszWhere,
                      PSZ     pszMajorItemKey, 
                      ULONG   ulNumObs,
                      PMATRIX pmat)
   {
   USHORT   i;
   CHAR     szTemp [254] = "";
   CHAR     szOut [254] = "";
   ULONG    ulTtl, aulCount [AUL_CNT_SIZE + 2];
   PSZ     *ppsz = NULL;
   long double   dpct;
   ULONG   ulNumObsLoc = 0UL;


   GetHistogram (hheap, pszWhere, pszMajorItemKey, pmat, aulCount);

   /* === Observations section: */
   ItiPrtPutTextAt(CURRENT_LN, 5, LEFT, (4 | REL_LEFT), "The ");

   for (i=0; i <= AUL_CNT_SIZE; i++)
      ulNumObsLoc = ulNumObsLoc + aulCount[i];

   ulNumObs = ulNumObsLoc;

   // sprintf (szTemp, "%lu", ulNumObs);
   sprintf (szTemp, "%lu", ulNumObsLoc);
   ItiFmtFormatString (szTemp, szOut, sizeof szOut, "Number,,", NULL);
   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+2, LEFT, (6|REL_LEFT), szOut);

   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+2, LEFT, USE_RT_EDGE,
            " observations are estimated with the following distributions: ");
   ItiPrtIncCurrentLn ();

   /* -- # within n% lines: */
   for (i = 0; i < 9; i++)
      {
      /* -- write data: */
      sprintf (szTemp, "%ld", aulCount [i]);
      ItiFmtFormatString (szTemp, szOut, sizeof szOut, "Number,,", NULL);
      ItiPrtPutTextAt (CURRENT_LN, 13, RIGHT, 6 | REL_LEFT, szOut);

      if (ulNumObsLoc != 0UL)
         dpct = ((DOUBLE)aulCount[i] / (DOUBLE)ulNumObsLoc);
      else
         dpct = 0.0;
      sprintf (szTemp, "%Lf", dpct );
      ItiFmtFormatString (szTemp, szOut, sizeof szOut, "Number,%,..", NULL);
      ItiPrtPutTextAt (CURRENT_LN, 20, LEFT, 2 | REL_LEFT, "(");
      ItiPrtPutTextAt (CURRENT_LN, 21, RIGHT, 8 | REL_LEFT, szOut);
      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1, LEFT, 2 | REL_LEFT, ")");

      /* -- make the label. */
      sprintf (szTemp, " within %d0%% ", i+1);
      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+2, LEFT, USE_RT_EDGE, szTemp);
      ItiPrtIncCurrentLn ();
      } /* end of for loop */
   ItiPrtIncCurrentLn ();


   /* -- more than 90% from... line */
   ulTtl = aulCount [9] + aulCount [10];
   sprintf (szTemp, "%ld", ulTtl);
   ItiFmtFormatString (szTemp, szOut, sizeof szOut, "Number,,", NULL);
   ItiPrtPutTextAt (CURRENT_LN,13, RIGHT, 6 | REL_LEFT, szOut);

   if (ulNumObsLoc != 0UL)
      // dpct = ((DOUBLE)aulCount[10] / (DOUBLE)ulNumObs);
      dpct = (((DOUBLE)aulCount[9]+(DOUBLE)aulCount[10]) / (DOUBLE)ulNumObsLoc);
   else
      dpct = 0.0;

   sprintf (szTemp, "%Lf", dpct);
   ItiFmtFormatString (szTemp, szOut, sizeof szOut, "Number,%,..", NULL);
//   ItiPrtPutTextAt (CURRENT_LN,20, RIGHT, 8 | REL_LEFT, szOut);
      ItiPrtPutTextAt (CURRENT_LN, 20, LEFT, 2 | REL_LEFT, "(");
      ItiPrtPutTextAt (CURRENT_LN, 21, RIGHT, 8 | REL_LEFT, szOut);
  //    ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1, LEFT, 2 | REL_LEFT, ")");

   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1, LEFT, USE_RT_EDGE,
                    ")   more than 90% from engineering estimate.");

   ItiPrtIncCurrentLn ();
   ItiPrtIncCurrentLn ();
   }



//void EXPORT  DoCalcSection (PMATRIX  pmat, PMATRIX  pmatStruc)
//   {
//   CHAR szTemp [256] = "";
//   CHAR szNumber [256] = "";
//
//   /* === Quantity = Length... section */
//   sprintf (szTemp, "%lf", pmat->pdMat [0][0]);
//   ItiFmtFormatString (szTemp, szNumber, sizeof szNumber, "Number,,....", NULL);
//   ItiPrtPutTextAt(CURRENT_LN, 5,
//                   LEFT, (22|REL_LEFT), "Quantity = Length * ( ");
//   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+1,
//                   RIGHT, (12|REL_LEFT), szNumber);
//   ItiPrtIncCurrentLn ();
//
//
//   /* -- Pavement-Depth: */
//   sprintf (szTemp, "%lf", pmat->pdMat [1][0]);
//   ItiFmtFormatString (szTemp, szNumber, sizeof szNumber, "Number,,....", NULL);
//   ItiPrtPutTextAt(CURRENT_LN, 25, LEFT, (2|REL_LEFT), "+ ");
//
//   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+1,
//                   RIGHT,(12|REL_LEFT), szNumber);
//
//   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+2,
//                   LEFT,USE_RT_EDGE, " * Pavement-Depth ");
//   ItiPrtIncCurrentLn ();
//
//
//
//   /* -- Pavement-Width: */
//   sprintf (szTemp, "%lf", pmat->pdMat [2][0]);
//   ItiFmtFormatString (szTemp, szNumber, sizeof szNumber, "Number,,....", NULL);
//   ItiPrtPutTextAt(CURRENT_LN, 25, LEFT, (2|REL_LEFT), "+ ");
//
//   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+1,
//                   RIGHT,(12|REL_LEFT), szNumber);
//
//   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+2,
//                   LEFT,USE_RT_EDGE, " * Pavement-Width ");
//   ItiPrtIncCurrentLn ();
//
//   /* -- Pavement-Depth * Pavement-Width: */
//   sprintf (szTemp, "%lf", pmat->pdMat [3][0]);
//   ItiFmtFormatString (szTemp, szNumber, sizeof szNumber, "Number,,....", NULL);
//   ItiPrtPutTextAt(CURRENT_LN, 25, LEFT, (2|REL_LEFT), "+ ");
//
//   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+1,
//                   RIGHT,(12|REL_LEFT), szNumber);
//
//   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+2,
//                   LEFT,USE_RT_EDGE, " * Pavement-Depth * Pavement-Width)");
//   ItiPrtIncCurrentLn ();
 //
// The structure sub section was omitted. MAR 96
//   /* -- Structure sub-section: */
//   sprintf (szTemp, "%lf", pmatStruc->pdMat [0][0]);
//   ItiFmtFormatString (szTemp, szNumber, sizeof szNumber, "Number,,....", NULL);
//   ItiPrtPutTextAt(CURRENT_LN, 14, LEFT, (4|REL_LEFT), "+ ( ");
//
//   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+1,
//                   RIGHT,(12|REL_LEFT), szNumber);
//   ItiPrtIncCurrentLn ();
//
//
//   /* -- Structure-length: */
//   ItiPrtPutTextAt(CURRENT_LN, 16, LEFT, (2|REL_LEFT), "+ ");
//
//   sprintf (szTemp, "%lf", pmatStruc->pdMat [0][1]);
//   ItiFmtFormatString (szTemp, szNumber, sizeof szNumber, "Number,,....", NULL);
//   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+1,
//                   RIGHT,(12|REL_LEFT), szNumber);
//
//   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+2,
//                   LEFT,USE_RT_EDGE, " * Structure-Length ");
//   ItiPrtIncCurrentLn ();
//
//
//   /* -- Structure-width: */
//   ItiPrtPutTextAt(CURRENT_LN, 16, LEFT, (2|REL_LEFT), "+ ");
//
//   sprintf (szTemp, "%lf", pmatStruc->pdMat [0][2]);
//   ItiFmtFormatString (szTemp, szNumber, sizeof szNumber, "Number,,....", NULL);
//   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+1,
//                   RIGHT,(12|REL_LEFT), szNumber);
//
//   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+2,
//                   LEFT,USE_RT_EDGE, " * Structure-Width ");
//   ItiPrtIncCurrentLn ();
//
//
//   /* -- Structure-length * structure-width: */
//   ItiPrtPutTextAt(CURRENT_LN, 16, LEFT, (2|REL_LEFT), "+ ");
//
//   sprintf (szTemp, "%lf", pmatStruc->pdMat [0][3]);
//   ItiFmtFormatString (szTemp, szNumber, sizeof szNumber, "Number,,....", NULL);
//   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+1,
//                   RIGHT,(12|REL_LEFT), szNumber);
//
//   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+2,
//                   LEFT,USE_RT_EDGE, " * Structure-Length * Structure-Width)");
//
//   ItiPrtIncCurrentLn ();
//   } /* End orginal calc section */



void EXPORT DoCalcSection (HHEAP hheap, PSZ pszWhereClause, PSZ pszMajorItemKey)
   {
   CHAR szTemp [30] = "";
   CHAR szQry  [1020] = " /* Q DoCalcSection */"
                        " select AVG(AveragePavementDepth) "
                        ", AVG(AveragePavementWidth), AVG(ProposalLength) "
                        " from HistoricalProposal HP "
                        ", HistoricalProposalMajorItem HPMI "
                        " where HPMI.ProposalKey = HP.ProposalKey "
                        " and HP.AwardedBidTotal > 0 "
                        " and ProposalLength > 0.0 "
                        " and AveragePavementWidth > 0.0 "
                        " and AveragePavementDepth > 0.0 "
          " AND  HP.ProposalKey in ( select ProposalKey "
                                   " from HistoricalProposalMajorItem "
                                   " where MajorItemKey = ";
   CHAR szQuery [1020] = "";
   CHAR szText [100] = "";
   USHORT   us, usNumCol;
   PSZ      pszSlope, pszIntercept, psz, pszRvalue;
   PPSZ     ppszAvgs = NULL;


   /* Show the average Length, Width, and Depth of the selected proposals. */
   ItiStrCat (szQry, pszMajorItemKey, sizeof szQry);
   ItiStrCat (szQry, " ) ", sizeof szQry);
   ItiStrCat (szQry, pszWhereClause, sizeof szQry);

      ppszAvgs = ItiDbGetRow1 (szQry, hheap, 0, 0, 0, &usNumCol);
      if ((ppszAvgs != NULL) && (ppszAvgs[0][0] != '\0'))
         {
         ItiPrtPutTextAt(CURRENT_LN, 5, LEFT, (37|REL_LEFT),
                   "Observed proposals have averages of: ");
         ItiPrtIncCurrentLn ();
         ItiPrtPutTextAt(CURRENT_LN, 7, LEFT, (8|REL_LEFT),
                   "Depth = ");
         psz = ppszAvgs[0];
         while ((psz != NULL) && (*psz) ==  ' ')
            psz++;
         ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+1,
                   LEFT, (9|REL_LEFT), psz);

         ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+1,
                   LEFT, (10|REL_LEFT), ", Width = ");
         psz = ppszAvgs[1];
         while ((psz != NULL) && (*psz) ==  ' ')
            psz++;
         ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+1,
                   LEFT, (9|REL_LEFT), psz);

         ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+1,
                   LEFT, (11|REL_LEFT), ", Length = ");
         psz = ppszAvgs[2];
         while ((psz != NULL) && (*psz) ==  ' ')
            psz++;
         ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+1,
                   LEFT, (9|REL_LEFT), psz);
         }
      else
        {
        ItiErrWriteDebugMessage ("** ERROR trying to compute averages in dQty.c.");
        ItiPrtPutTextAt(CURRENT_LN, 5, LEFT, (55|REL_LEFT),
                  "*** ERROR trying to compute selected proposal averages.");
        }

   ItiPrtIncCurrentLn ();
   ItiPrtIncCurrentLn ();
   szQry[0] = '\0';

   /* === Determine which formula to use:  ====================  */
   us = ParaCalcDetermineVorAorL (pszMajorItemKey);
   switch (us)
      {
      case PARACALC_VOLUME:
         pszRvalue = ParametMajorItemRSQ (hheap, pszMajorItemKey, pszWhereClause, PARACALC_VOLUME);
         if (pszRvalue != NULL)
            {
            psz = pszRvalue;
            for (us=0; us<8; us++)
                psz++;
            (*psz) = '\0';
            }
         else                
            pszRvalue = "0.0";

         ItiStrCpy (szText, "Quantity calculations used Volume with Rý value of ", sizeof szText);
         ItiStrCat (szText, pszRvalue, sizeof szText);
         ItiPrtPutTextAt(CURRENT_LN, 5,
                   LEFT, (65|REL_LEFT), szText);
                 //  LEFT, (22|REL_LEFT), "Quantity = Volume * ( ");
         break;

      case PARACALC_AREA:
         pszRvalue = ParametMajorItemRSQ (hheap, pszMajorItemKey, pszWhereClause, PARACALC_AREA);
         if (pszRvalue != NULL)
            {
            psz = pszRvalue;
            for (us=0; us<8; us++)
                psz++;
            (*psz) = '\0';
            }
         else                
            pszRvalue = "0.0";

         ItiStrCpy (szText, "Quantity calculations used Area with Rý value of ", sizeof szText);
         ItiStrCat (szText, pszRvalue, sizeof szText);
         ItiPrtPutTextAt(CURRENT_LN, 5,
                   LEFT, (70|REL_LEFT), szText);
         // ItiPrtPutTextAt(CURRENT_LN, 5,
         //          LEFT, (22|REL_LEFT), "Quantity = Area * ( ");
         break;

      case PARACALC_LENGTH:
      default:
         pszRvalue = ParametMajorItemRSQ (hheap, pszMajorItemKey, pszWhereClause, PARACALC_LENGTH);
         if (pszRvalue != NULL)
            {
            psz = pszRvalue;
            for (us=0; us<8; us++)
                psz++;
            (*psz) = '\0';
            }
         else                
            pszRvalue = "0.0";

         ItiStrCpy (szText, "Quantity calculations used Length with Rý value of ", sizeof szText);
         ItiStrCat (szText, pszRvalue, sizeof szText);
         ItiPrtPutTextAt(CURRENT_LN, 5,
                   LEFT, (70|REL_LEFT), szText);
         // ItiPrtPutTextAt(CURRENT_LN, 5,
         //          LEFT, (22|REL_LEFT), "Quantity = Length * ( ");
         break;
      }/* end switch */


   ItiPrtIncCurrentLn ();
   return;

   /* -- Compute the Slope. */
   pszSlope = ParametMajorItemSlopeQ (hheap, pszMajorItemKey, pszWhereClause, us);
   if ((pszSlope != NULL) && ((*pszSlope) != '\0'))
      ItiStrCpy(szTemp, pszSlope, FACTOR_SIZE);
   else
      ItiStrCpy(szTemp, " 1.09578 ", FACTOR_SIZE);

   ItiFmtFormatString (szTemp, szSlope, sizeof szSlope, "Number,,.....", NULL);
   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+1,
                   LEFT, (12|REL_LEFT), szSlope);
   /* -- Now reformat without commas for the csv file. */
   ItiFmtFormatString (szTemp, szSlope, sizeof szSlope, "Number,.....", NULL);

   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL, LEFT, USE_RT_EDGE, " ) + ");


   /* --  Compute the Intercept. */
   pszIntercept = ParametMajorItemInterceptQ (hheap, pszMajorItemKey, pszWhereClause, us);
   ItiErrWriteDebugMessage (pszIntercept);
   if ((pszIntercept != NULL) && ((*pszIntercept) != '\0'))
      ItiStrCpy(szTemp, pszIntercept, FACTOR_SIZE);
   else
      ItiStrCpy(szTemp, " 12021.98 ", FACTOR_SIZE);

   ItiFmtFormatString (szTemp, szIntercept, sizeof szIntercept, "Number,,.....", NULL);
   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+1,
                   LEFT, (20|REL_LEFT), szIntercept);
   /* -- Now reformat without commas for the csv file. */
   ItiFmtFormatString (szTemp, szIntercept, sizeof szIntercept, "Number,.....", NULL);

   ItiPrtIncCurrentLn ();
   return;
   }/* END of Function DoCalc */






BOOL EXPORT  DoMajorItemQty (HHEAP hheap,
                         FILE  *pf,
                         PSZ   pszWhere,
                         PSZ   *ppsz)
   {
   static PMATRIX  pmatDMIQResults;
//   static PMATRIX  pmatDMIQReg, pmatStrucReg, pmatStrucResults;
   BOOL bAnalyzed = TRUE;
   CHAR szBase [30]="", szDepth [30]="", szWidth [30]="", szCrossSection [30]="";
   CHAR szStrucBase [30]="", szStrucLength [30]="", szStrucWidth [30]="";
   CHAR szStrucLgthWdth [30]="";
   CHAR szMajorItemKeyText [20] = "";
   CHAR szMajorItemKey [15] = "";
   CHAR szBaseDate[30] = "";
   PSZ  pszBaseDate = NULL;
   PSZ  pszIntercept, pszSlope;
   CHAR szGetBaseDate[] =
           "select LTRIM(STR(DATEPART(mm,MAX(BaseDate)))) "
           " + CHAR(047) + LTRIM(STR(DATEPART(dd,MAX(BaseDate)))) "
           " + CHAR(047) + LTRIM(STR(DATEPART(yy,MAX(BaseDate)))) "
           " + CHAR(032) + LTRIM(STR(DATEPART(hh,MAX(BaseDate)))) "
           " + CHAR(058) + LTRIM(STR(DATEPART(mi,MAX(BaseDate)))) "  
           "from BaseDate ";

   ItiStrCpy (szMajorItemKey, ppsz[0], sizeof szMajorItemKey );

   ItiStrCpy (szMajorItemKeyText, ppsz[1], 5 );
   ItiStrCat (szMajorItemKeyText, " ", sizeof szMajorItemKeyText );

   /* -- If spec year is null, then explicitly say so. */
   if ((ppsz[3] == NULL) || (ppsz[3][0] == '\0'))
      ItiStrCat (szMajorItemKeyText, "NULL", sizeof szMajorItemKeyText );
   else
      ItiStrCat (szMajorItemKeyText, ppsz[3], sizeof szMajorItemKeyText );

   /* -- Basedate */
   pszBaseDate = ItiDbGetRow1Col1(szGetBaseDate, hheap, 0, 0, 0);
   if ((pszBaseDate != NULL) && (*pszBaseDate != '\0'))
      ItiStrCpy (szBaseDate, pszBaseDate, sizeof szBaseDate);
   else                                                                              
      ItiStrCpy (szBaseDate, UNDECLARED_BASE_DATE, sizeof szBaseDate);


   DoWithin (hheap, pszWhere, ppsz [0], ulNumObsDMIQ, pmatDMIQResults);
   DoCalcSection (hheap, pszWhere, ppsz[0]);

   /* -- Compute the slopes & intercepts for Vol, Area, Length. */
   /* -- VOLUME */                           
   pszSlope = ParametMajorItemSlopeQ (hheap, szMajorItemKey, pszWhere, PARACALC_VOLUME);
   if ((pszSlope != NULL) && ((*pszSlope) != '\0'))
      ItiStrCpy(szSlope, pszSlope, FACTOR_SIZE);
   else
      ItiStrCpy(szSlope, " 1.09578 ", FACTOR_SIZE);

   pszIntercept = ParametMajorItemInterceptQ (hheap, szMajorItemKey, pszWhere, PARACALC_VOLUME);
   if ((pszIntercept != NULL) && ((*pszIntercept) != '\0'))
      {
      ItiStrCpy(szIntercept, pszIntercept, FACTOR_SIZE);
      ItiExtract (szIntercept, ",");
      }
   else
      ItiStrCpy(szIntercept, " 10.0 ", FACTOR_SIZE);

   /* -- AREA */
   pszSlope = ParametMajorItemSlopeQ (hheap, szMajorItemKey, pszWhere, PARACALC_AREA);
   if ((pszSlope != NULL) && ((*pszSlope) != '\0'))
      ItiStrCpy(szWidth, pszSlope, FACTOR_SIZE);
   else
      ItiStrCpy(szWidth, " 1.09578 ", FACTOR_SIZE);

   pszIntercept = ParametMajorItemInterceptQ (hheap, szMajorItemKey, pszWhere, PARACALC_AREA);
   if ((pszIntercept != NULL) && ((*pszIntercept) != '\0'))
      {
      ItiStrCpy(szCrossSection, pszIntercept, FACTOR_SIZE);
      ItiExtract (szCrossSection, ",");
      }
   else
      ItiStrCpy(szCrossSection, " 10.0 ", FACTOR_SIZE);

   /* -- LENGTH */
   pszSlope = ParametMajorItemSlopeQ (hheap, szMajorItemKey, pszWhere, PARACALC_LENGTH);
   if ((pszSlope != NULL) && ((*pszSlope) != '\0'))
      ItiStrCpy(szStrucBase, pszSlope, FACTOR_SIZE);
   else
      ItiStrCpy(szStrucBase, " 1.09578 ", FACTOR_SIZE);

   pszIntercept = ParametMajorItemInterceptQ (hheap, szMajorItemKey, pszWhere, PARACALC_LENGTH);
   if ((pszIntercept != NULL) && ((*pszIntercept) != '\0'))
      {
      ItiStrCpy(szStrucLength, pszIntercept, FACTOR_SIZE);
      ItiExtract (szStrucLength, ",");
      }
   else
      ItiStrCpy(szStrucLength, " 10.0 ", FACTOR_SIZE);

   /* -- MajorItemQuantityRegression table section. */
   ItiCSVWriteLine (pf, "MajorItemQuantityRegression"
                 , szMajorItemKeyText                /* MajorItemKeyText */
                 , szBaseDate 
                 , szSlope     /* ,szBase New chgs for Qty=slope*VOL+intercept */
                 , szIntercept /* ,szDepth */
                 , szWidth
                 , szCrossSection
                 , szStrucBase, szStrucLength, szStrucWidth
                 , szStrucLgthWdth, NULL);
   //else
   //   {
   //   ItiPrtPutTextAt (CURRENT_LN, 5, LEFT | WRAP, 5 | FROM_RT_EDGE, 
   //                    "There is not enough information for this major item to be analyzed.");
   //   ItiPrtIncCurrentLn ();
   //   }

   return bAnalyzed;
   }



BOOL EXPORT  MajorItemSection (HHEAP hheap, PSZ pszWhere, PSZ pszFileName)
   {
   FILE     *pf;
   CHAR     szQuery [4090] = "";
   CHAR     szMajorItemKeyText[20] = "";
   CHAR     szQry[200] = "";
   HQRY     hqry, hqry2;
   USHORT   usNumCols, usColCnt, usError;
   PSZ      *ppsz, *ppszRes, pszRes = NULL;

   pf = _fsopen (pszFileName, "wt", SH_DENYWR);
   if (pf == NULL)
      return FALSE;

   fprintf (pf, ";created by dqty model, compiled on %s\n", 
            szCompileDateTime);
   fprintf (pf, ";Model run on %.2d/%.2d/%.4d %.2d:%.2d%cm.\n",
            (USHORT) pglobals->pgis->month,
            (USHORT) pglobals->pgis->day,
            (USHORT) pglobals->pgis->year,
            (USHORT) pglobals->pgis->hour % 12,
            (USHORT) pglobals->pgis->minutes,
            pglobals->pgis->hour > 12 ? (char) 'p' : (char) 'a');

   ItiCSVWriteLine (pf, "DS/Shell Import File", "1.00", NULL);
   ItiCSVWriteLine (pf, "MajorItemQuantityRegression", "MajorItemKey", 
                    "BaseDate", "BaseCoefficient", "DepthAdjustment", 
                    "WidthAdjustment", "CrossSectionAdjustment", 
                    "StructureBaseAdjustment", "StructureWidthAdjustment",
                    "StructureLengthAdjustment", "StructureAreaAdjustment", 
                    NULL);

   ItiMbQueryQueryText (hmod, ITIRID_RPT, GET_MAJOR_ITEMS,
                        szQuery, sizeof szQuery);
   ItiStrCat (szQuery, pszWhere, sizeof szQuery);
   ItiStrCat (szQuery, " ORDER BY MI.MajorItemID ", sizeof szQuery);

   hqry = ItiDbExecQuery (szQuery, hheap, hmod, ITIRID_RPT,
                          GET_MAJOR_ITEMS, &usNumCols, &usError);
   if (hqry == NULL)
      return FALSE;

   while (ItiDbGetQueryRow (hqry, &ppsz, &usError))
      {
      /* -- Check for new page. */
      if ((ItiPrtKeepNLns(8) == PG_WAS_INC)
          || (ItiPrtQueryCurrentPgNum() != usCurrentPg ))
         {
         usCurrentPg = ItiPrtQueryCurrentPgNum();
         ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
         }
      
      /* === Section label for Major Item ID and description fields: */
      ItiPrtPutTextAt(CURRENT_LN, 0, LEFT, (12 | REL_LEFT), "Major Item: ");

      /* MajorItemID ppsz[1] */
      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+1, LEFT,(8|REL_LEFT), ppsz[1]); 
      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+2, LEFT,(3|REL_LEFT), " - ");

      /* Description ppsz[2] */
      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+1,LEFT,(40|REL_LEFT), ppsz[2]);
      ItiPrtIncCurrentLn (); //line feed
      ItiPrtIncCurrentLn (); //blank line

      DoMajorItemQty (hheap, pf, pszWhere, ppsz);

      /* -- Finish out this Major-Item section. */
      ItiPrtIncCurrentLn ();
      ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
      ItiPrtIncCurrentLn ();

      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }



   /* -- MajorItemQuantityVariable table section. JAN 96 */
   hqry = ItiDbExecQuery (szQuery, hheap, hmod, ITIRID_RPT,
                          GET_MAJOR_ITEMS, &usNumCols, &usError);
   if (hqry == NULL)
      return FALSE;

   ItiCSVWriteLine (pf, "MajorItemQuantityVariable"
                      , "MajorItemKey", "BaseDate"
                      , "QualitativeVariable", NULL);

   while (ItiDbGetQueryRow (hqry, &ppsz, &usError))
      {
      ItiStrCpy (szMajorItemKeyText, ppsz[1], 5 );
      ItiStrCat (szMajorItemKeyText, " ", sizeof szMajorItemKeyText );

      /* -- If spec year is null, then explicitly say so. */
      if ((ppsz[3] == NULL) || (ppsz[3][0] == '\0'))
         ItiStrCat (szMajorItemKeyText, "NULL", sizeof szMajorItemKeyText );
      else
         ItiStrCat (szMajorItemKeyText, ppsz[3], sizeof szMajorItemKeyText );

      ItiStrCpy (szQry,
         "select BaseDate, QualitativeVariable from MajorItemQuantityVariable "
         " where BaseDate = (select MAX(BaseDate) from MajorItemQuantityVariable)"
         " and MajorItemKey = ", sizeof szQry);
      ItiStrCat (szQry, ppsz[0],sizeof szQry);

      hqry2 = ItiDbExecQuery (szQry, hheap, 0, 0, 0, &usColCnt, &usError);
      if (hqry2 != NULL)
         while (ItiDbGetQueryRow (hqry2, &ppszRes, &usError))
            {
            if (ppszRes != NULL) 
               ItiCSVWriteLine (pf, "MajorItemQuantityVariable"
                            , szMajorItemKeyText    /* MajorItemKeyText */
                            , ppszRes[0], ppszRes[1], NULL);
            ItiFreeStrArray (hheap, ppszRes, usColCnt);
            }

      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }



   /* -- MajorItemQuantityAdjustment table section. JAN 96 */
   hqry = ItiDbExecQuery (szQuery, hheap, hmod, ITIRID_RPT,
                          GET_MAJOR_ITEMS, &usNumCols, &usError);
   if (hqry == NULL)
      return FALSE;

   ItiCSVWriteLine (pf, "MajorItemQuantityAdjustment"
                      , "MajorItemKey", "BaseDate"
                      , "QualitativeVariable"
                      , "VariableValue", "QuantityAdjustment", NULL);
   while (ItiDbGetQueryRow (hqry, &ppsz, &usError))
      {
      ItiStrCpy (szMajorItemKeyText, ppsz[1], 5 );
      ItiStrCat (szMajorItemKeyText, " ", sizeof szMajorItemKeyText );

      /* -- If spec year is null, then explicitly say so. */
      if ((ppsz[3] == NULL) || (ppsz[3][0] == '\0'))
         ItiStrCat (szMajorItemKeyText, "NULL", sizeof szMajorItemKeyText );
      else
         ItiStrCat (szMajorItemKeyText, ppsz[3], sizeof szMajorItemKeyText );

      ItiStrCpy (szQry,
         "select BaseDate, QualitativeVariable, VariableValue, QuantityAdjustment"
         " from MajorItemQuantityAdjustment "
         " where BaseDate = (select MAX(BaseDate) from MajorItemQuantityAdjustment)"
         " and MajorItemKey = ", sizeof szQry);
      ItiStrCat (szQry, ppsz[0],sizeof szQry);
   
      hqry2 = ItiDbExecQuery (szQry, hheap, 0, 0, 0, &usColCnt, &usError);
      if (hqry2 != NULL)
         while (ItiDbGetQueryRow (hqry2, &ppszRes, &usError))
            {
            if (ppszRes != NULL) 
               ItiCSVWriteLine (pf, "MajorItemQuantityAdjustment"
                       , szMajorItemKeyText        /* MajorItemKeyText */
                       , ppszRes[0], ppszRes[1], ppszRes[2], ppszRes[3], NULL);

            ItiFreeStrArray (hheap, ppszRes, usColCnt);
            }

      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }


   fclose (pf);
   return TRUE;
   }







/***************************************************************************
 * Analysis
 **************************************************************************/



BOOL EXPORT  DoHeader (HHEAP    hheap,
                      PSZ      pszWhere,
                      PSZ      pszFileName,
                      USHORT   usFileName)
   {
   static USHORT usCount = 0;
   USHORT usLn = 10;
   CHAR szXlatedWhere [1000] = ""; 

   sprintf (pszFileName, "dqty%.4x.csv", usCount);

   /* -- Check for new page. */
   if ((ItiPrtKeepNLns(usLn) == PG_WAS_INC)
       || (ItiPrtQueryCurrentPgNum() != usCurrentPg ))
      {
      usCurrentPg = ItiPrtQueryCurrentPgNum();
      ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
      }
   /* -- Translate selection clause key values to IDs here. */
   ItiDbXlateKeys (hheap, pszWhere, szXlatedWhere, sizeof szXlatedWhere);

   ItiPrtPutTextAt (CURRENT_LN, 0, LEFT, 17 | REL_LEFT, "Selection Clause:");
   //ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1, LEFT | WRAP,
   //                 2 | FROM_RT_EDGE, pszWhere);
   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1, LEFT | WRAP, 
                    2 | FROM_RT_EDGE, szXlatedWhere);
   ItiPrtIncCurrentLn ();
   ItiPrtIncCurrentLn ();

   ItiPrtPutTextAt (CURRENT_LN, 0, LEFT, 17 | REL_LEFT, "Output file name:");
   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1, LEFT | WRAP,
                    1 | FROM_RT_EDGE, pszFileName);
   ItiPrtIncCurrentLn ();

   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   ItiPrtIncCurrentLn ();

   return TRUE;
   }





/***************************************************************************
 * Main loop
 **************************************************************************/

void EXPORT  GetInfo (PSZ      pszFile,
                     PSZ      pszWhere,
                     USHORT   usWhere)
   {
   FILE     *pf;
   USHORT   i;

   pszWhere [0] = '\0';

   if (pszFile && *pszFile)
      {
      pf = _fsopen (pszFile, "rt", SH_DENYWR);
      if (pf == NULL)
         return;

      /* skip first two lines */
      fgets (pszWhere, usWhere, pf);
      fgets (pszWhere, usWhere, pf);

      fgets (pszWhere, usWhere, pf);
      i = ItiStrLen (pszWhere) - 1;
      if (pszWhere [i] == '\n')
         pszWhere [i] = ' ';

      fclose (pf);
      }
   }



BOOL EXPORT  QuantityAnalysis (HHEAP hheap,
                              PSZ   pszInfoFile)
   {
   char  szWhere [2040] = "";
   char  szFileName [254] = "";

   GetInfo (pszInfoFile, szWhere, sizeof szWhere);

   if (!DoHeader (hheap, szWhere, szFileName, sizeof szFileName))
      return FALSE;

   return MajorItemSection (hheap, szWhere, szFileName);
   }



/* ======================================================================= *\
 *                                                    EXPORTED FUNCTIONS   *
\* ======================================================================= */
/* -- ItiDllQueryVersion returns the version number of this DLL. */
USHORT EXPORT ItiDllQueryVersion (VOID)
   {
   return DLL_VERSION;
   }


/*
 * ItiDllQueryCompatibility returns TRUE if this DLL is compatable
 * with the caller's version numer, otherwise FALSE is returned.
 */

BOOL EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion)

   {
   if ( ItiVerCmp(szDllVersion) )
      return TRUE;
   return FALSE;
   }





USHORT EXPORT ItiRptDLLPrintReport (HAB      hab,
                                    HMQ      hmq,
                                    int      iArgCount,
                                    CHAR     *ppszArgs [])
   {
   int         i = 0;
   HHEAP       hheap;
   REPTITLES   rept;
   char        szKeyFile [256];
   USHORT      usNumTitles;

   /* ------------------------------------------------------------------- *\
    *  Create a heap.                                                     *
   \* ------------------------------------------------------------------- */
   hheap = ItiMemCreateHeap (MIN_HEAP_SIZE);

   /* ------------------------------------------------------------------- *\
    * Do any run-time formatting then start the report.                   *
   \* ------------------------------------------------------------------- */

   /* -- Get the report's column titles for this DLL. */
   ItiRptUtLoadLabels (hmod, &usNumTitles, &acttl[0]);

   SetTitles (&rept, iArgCount, ppszArgs);

   i =  ItiPrtLoadDLLInfo (hab, hmq, &rept, iArgCount, ppszArgs);
   if (i != 0 )
      {
      DBGMSG("FAILED to ItiPrtDLLLoadInfo.");
      return ITIPRT_NOT_INITIALIZED;
      }

   ItiPrtBeginReport ("Major Item Quantity Analysis");

   szKeyFile [0] = '\0';
   for (i=0; i < iArgCount; i++)
      if (ItiStrICmp (ppszArgs [i], "-modelinfo") == 0 && i+1 < iArgCount)
         ItiStrCpy (szKeyFile, ppszArgs [i+1], sizeof szKeyFile);

   QuantityAnalysis (hheap, szKeyFile);

   /* ------------------------------------------------------------------- *\
    * Finish up and quit.                                                 *
   \* ------------------------------------------------------------------- */
   ItiPrtEndReport ();

   return 0;

   }








/*
 * DllInit is called by either ITIDLLI.OBJ or ITIDLLC.OBJ.  This functino
 * must return non-zero if the initialization succeded, or FALSE
 * if initialization failed.
 */



BOOL DLLINITPROC DllInit (HMODULE hmodDll)
   {
   /* module initialization functions. */
   hmod = hmodDll;
   pglobals = ItiGlobQueryGlobalsPointer ();


   return TRUE;
   }





BOOL EXPORT ItiDllInitDll (VOID)
   {
   PSZ pszEnv = NULL;

   DosScanEnv("PARACONV", &pszEnv);
   if (pszEnv != NULL)
      bUseParaConv = TRUE;
   else
      bUseParaConv = FALSE;

   return TRUE;
   }



