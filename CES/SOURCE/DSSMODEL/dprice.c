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
 * dPrice.c
 *
 * Mark Hampton
 * Timothy Blake
 *
 * This is the dss price analysis model.
 */


#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"
#include "..\include\itimbase.h"
#include "..\include\itidbase.h"
#include "..\include\itiglob.h"
#include "..\include\itiFmt.h"
#include "..\include\paracalc.h"

#include "..\include\itirpt.h"

#include <stdio.h>
#include <string.h>
#include <share.h>
#include <math.h>

#include "dPrice.h"
#include "gauss.h"

#include "..\include\itiimp.h"




#define BIG_QUERY  6000
#define MAX_TYPES   256

/* ======================================================================= *\
 *                                             MODULE'S GLOBAL VARIABLES   *
\* ======================================================================= */

/*
 * hmod is the handle to this DLL
 */

HMODULE hmod = -1;
PGLOBALS pglobals = NULL;


static char    szCompileDateTime[] = __DATE__ " " __TIME__ ;
static CHAR szDllVersion[] = "1.1a0 dprice.dll";

static COLTTLS acttl[32];
static USHORT  usCurrentPg = 0;

static BOOL    bInitBasis = FALSE;

static CHAR    aszWorkType[MAX_TYPES][32];
static CHAR    aszRoadType[MAX_TYPES][32];
static CHAR    aszDistrict[MAX_TYPES][32];

static USHORT  usWorkType;
static USHORT  usRoadType;
static USHORT  usDistrict;

static CHAR szBasePrice [32] = "";  /* price of the median qty. */
static CHAR szMedianQty [32] = "";
static CHAR szMedianQtyRaw[32] = "";

static   CHAR  szMedianQtyUnitPrice[30]="";
static   CHAR  szQuantityAdjustment[30]="";




USHORT SetTitles (PREPT preptitles, INT argcnt, CHAR * argvars[])
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



 BOOL EXPORT BasisInit (HHEAP hhp, PSZ pszWhereClause)
   {
   PSZ      *ppsz;
   HQRY     hqry;
   USHORT   usNumCols, usError, us;
   CHAR     szQuery [BIG_QUERY] = "";


   /* -- Init the type arrays. */
   for(us = 0; us < MAX_TYPES-1; us++)
      {
      ItiStrCpy (aszWorkType[us], " ", sizeof aszWorkType[us]);
      ItiStrCpy (aszRoadType[us], " ", sizeof aszRoadType[us]);
      ItiStrCpy (aszDistrict[us], " ", sizeof aszDistrict[us]);
      }
   usWorkType = 0;
   usRoadType = 0;
   usDistrict = 0;


   /* -- WORK TYPES */
   ItiMbQueryQueryText (hmod, ITIRID_RPT, GET_WORKTYPE_INFO,
                        szQuery, sizeof szQuery);
   ItiStrCat (szQuery, pszWhereClause, sizeof szQuery);

   hqry = ItiDbExecQuery (szQuery, hhp, hmod, ITIRID_RPT,
                          GET_WORKTYPE_INFO, &usNumCols, &usError);
   if (hqry == NULL)
      return FALSE;

   us = 0;
   while (ItiDbGetQueryRow (hqry, &ppsz, &usError))
      {
      if (ppsz[WORK] != NULL)
         {
         ItiStrCpy (aszWorkType[us], ppsz[WORK], sizeof aszWorkType[us]);
         usWorkType++;/* inc the count of how many types are in the array.*/
         }

      us++;
      ItiFreeStrArray (hhp, ppsz, usNumCols);
      }


   /* -- ROAD TYPES */
   ItiMbQueryQueryText (hmod, ITIRID_RPT, GET_ROADTYPE_INFO,
                        szQuery, sizeof szQuery);
   ItiStrCat (szQuery, pszWhereClause, sizeof szQuery);

   hqry = ItiDbExecQuery (szQuery, hhp, hmod, ITIRID_RPT,
                          GET_ROADTYPE_INFO, &usNumCols, &usError);
   if (hqry == NULL)
      return FALSE;

   us = 0;
   while (ItiDbGetQueryRow (hqry, &ppsz, &usError))
      {
      if (ppsz[ROAD] != NULL)
         {
         ItiStrCpy (aszRoadType[us], ppsz[ROAD], sizeof aszRoadType[us]);
         usRoadType++;
         }

      us++;
      ItiFreeStrArray (hhp, ppsz, usNumCols);
      }


   /* -- DISTRICTS */
   ItiMbQueryQueryText (hmod, ITIRID_RPT, GET_DISTRICT_INFO,
                        szQuery, sizeof szQuery);
   ItiStrCat (szQuery, pszWhereClause, sizeof szQuery);

   hqry = ItiDbExecQuery (szQuery, hhp, hmod, ITIRID_RPT,
                          GET_DISTRICT_INFO, &usNumCols, &usError);
   if (hqry == NULL)
      return FALSE;

   us = 0;
   while (ItiDbGetQueryRow (hqry, &ppsz, &usError))
      {
      if (ppsz[DIST] != NULL)
         {
         ItiStrCpy (aszDistrict[us], ppsz[DIST], sizeof aszDistrict[us]);
         usDistrict++;
         }

      us++;
      ItiFreeStrArray (hhp, ppsz, usNumCols);
      }


   }/* End of Function BasisInit */





 DOUBLE EXPORT Basis (USHORT   i,
                     DOUBLE   dLogQ,
                     PSZ      pszWorkType,
                     PSZ      pszRoadType,
                     PSZ      pszDistrict)
   {
   USHORT us = 2;


   switch (i)
      {
      case 0:
         return 1;

      case 1:
         return dLogQ;

      default:
         if (i <= (us + usWorkType))
            {
            if (0 == ItiStrCmp(pszWorkType, aszWorkType[i-us]) ) 
               return 1;
            return 0;
            }

         if (i <= (us + usWorkType+usRoadType))
            {
            if (0 == ItiStrCmp(pszRoadType, aszRoadType[i-(us+usWorkType)]) )
               return 1;
            return 0;
            }

         if (i <= (us+usWorkType+usRoadType+usDistrict))
            {
            if (0 == ItiStrCmp(pszDistrict,
                               aszDistrict[i-(us+usWorkType+usRoadType+usDistrict)]) )
               return 1;
            return 0;
            }

         return 0;
      }

   }/* End Function Basis */




/* call this for each major item (uses one db connection) */

 BOOL EXPORT DoRegression (HHEAP     hheap,
                          PSZ       pszWhere,
                          PSZ       pszMajorItemKey,
                          PULONG    pulNumObs,
                          PMATRIX   *ppmat,
                          PMATRIX   *ppmatResults)
   {
   USHORT   i, j, n;
   PSZ      *ppszDoReg;
   HQRY     hqry;
   USHORT   usNumCols, usError;
   DOUBLE   dPrice, dLogQ;
   BOOL     bRes;
   CHAR     szQuery [BIG_QUERY] = "";

   *pulNumObs = 0;

   ItiMbQueryQueryText (hmod, ITIRID_RPT, GET_MAJOR_ITEM_INFO,
                        szQuery, sizeof szQuery);
   ItiStrCat (szQuery, pszWhere, sizeof szQuery);
   ItiStrCat (szQuery, " AND HPMI.MajorItemKey = ", sizeof szQuery);
   ItiStrCat (szQuery, pszMajorItemKey, sizeof szQuery);

//////// () where is number two?
   ItiRptUtSetError(NULL,szQuery);

   /* figure out how many rows and columns; first-cut */
   n = ItiDbGetQueryCount (szQuery, &usNumCols, &usError);
   if (n < 1)
      return FALSE;
   else
      n = n + 2;

   bRes = AllocMatrix (n, n + 1, &(*ppmat));
   bRes = AllocMatrix (n, 1, &(*ppmatResults));

   if (*ppmat == NULL || *ppmatResults == NULL)
      return FALSE;
///////

   hqry = ItiDbExecQuery (szQuery, hheap, hmod, ITIRID_RPT,
                          GET_MAJOR_ITEM_INFO, &usNumCols, &usError);
   if (hqry == NULL)
      return FALSE;

   /* -- Initialize the basis "functions" */
   BasisInit (hheap, pszWhere);

   while (ItiDbGetQueryRow (hqry, &ppszDoReg, &usError))
      {
      *pulNumObs += 1;

      for (i=0; i < (*ppmat)->usRows; i++)
         {
         dLogQ = ItiStrToDouble (ppszDoReg [0]);
         if (dLogQ > 0)
            dLogQ = log (dLogQ);
         else
            dLogQ = 0;
         dPrice = ItiStrToDouble (ppszDoReg [1]);

         for (j=0; j < (*ppmat)->usCols; j++)
            (*ppmat)->pdMat [i][j] = 0;

         for (j=0; j < (*ppmat)->usCols - 1; j++)
            (*ppmat)->pdMat [i][j] += Basis(i, dLogQ, ppszDoReg [2], ppszDoReg [3], ppszDoReg [4]) *
                                  Basis(j, dLogQ, ppszDoReg [2], ppszDoReg [3], ppszDoReg [4]);
         (*ppmat)->pdMat [i][(*ppmat)->usCols-1] += dPrice * 
                                            Basis(i, dLogQ, ppszDoReg [2], ppszDoReg [3], ppszDoReg [4]);
         }
      ItiFreeStrArray (hheap, ppszDoReg, usNumCols);
      }

   bRes = SolveMatrix (*ppmat, *ppmatResults);
   return bRes;
   } /* End of function DoRegression */







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
   CHAR   szH10[150] = "";
   CHAR   szH20[220] = "";
   CHAR   szH30[220] = "";
   CHAR   szH40[220] = "";
   CHAR   szH50[220] = "";
   CHAR   szH60[220] = "";
   CHAR   szH70[220] = "";
   CHAR   szH80[220] = "";
   CHAR   szH90[220] = "";
   CHAR   szH100[150] = "";
   //USHORT usNumCols;
   USHORT usColCnt, usError, i;
   HQRY  hqry2;
   PPSZ  ppszRes;
   DOUBLE dVal;
   PSZ pszMarker = NULL;


   for (i=0; i < 10; i++)
      ulCount [i] = 0L;

   i = 0;

   ItiStrCpy(szQuery,
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

   return;

//   // Old development code, why is this kept??
//   /* - The estimated amount of the major item which is to
//      - be compared to the major items in the database.
//      - Where does this value come from??                 */
//   ItiStrCpy(szEstAmt, "100", sizeof szEstAmt);
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
//   ItiStrCat(pszMajorItemKey, " ", sizeof szQuery);
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







 void EXPORT DoWithin (HHEAP   hheap, 
                      PSZ     pszWhere,
                      PSZ     pszMajorItemKey, 
                      ULONG   ulNumObs,
                      PMATRIX pmat)
   {
   USHORT   i;
   char     szTemp [254] = "";
   char     szOut [254] = "";
   ULONG    ulCount [10+1];
//   PSZ     *ppsz = NULL;
   long double   dpct;


   GetHistogram (hheap, pszWhere, pszMajorItemKey, pmat, ulCount);
   
   /* === Observations section: */
   ItiPrtPutTextAt(CURRENT_LN, 5, LEFT, (4 | REL_LEFT), "The ");

   sprintf (szTemp, "%ld", ulNumObs);
   ItiFmtFormatString (szTemp, szOut, sizeof szOut, "Number,,", NULL);
   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+2, LEFT, (6|REL_LEFT), szOut);

   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+2, LEFT, USE_RT_EDGE,
            " observations are estimated with the following distributions: ");
   ItiPrtIncCurrentLn ();

   /* -- # within n% lines: */
   for (i = 0; i < 9; i++)
      {
      /* -- write data: */
      sprintf (szTemp, "%ld", ulCount [i]);
      ItiFmtFormatString (szTemp, szOut, sizeof szOut, "Number,,", NULL);
      ItiPrtPutTextAt (CURRENT_LN, 13, RIGHT, 6 | REL_LEFT, szOut);

      if (ulNumObs != 0L)
         dpct = ((DOUBLE)ulCount[i] / (DOUBLE)ulNumObs);
      else
         dpct = 0.0;
      sprintf (szTemp, "%Lf", dpct );
      ItiFmtFormatString (szTemp, szOut, sizeof szOut, "Number,%,.", NULL);
      ItiPrtPutTextAt (CURRENT_LN, 20, LEFT, 2 | REL_LEFT, "(");
      ItiPrtPutTextAt (CURRENT_LN, 21, RIGHT, 8 | REL_LEFT, szOut);
      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1, LEFT, 2 | REL_LEFT, ")");

      /* -- make the label. */
      sprintf (szTemp, " within %d0%% ", i+1);
      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+2, LEFT, USE_RT_EDGE, szTemp);
      ItiPrtIncCurrentLn ();
      }
   ItiPrtIncCurrentLn ();


   /* -- more than 90% from... line */
   sprintf (szTemp, "%ld", ulCount [9]);
   ItiFmtFormatString (szTemp, szOut, sizeof szOut, "Number,,", NULL);
   ItiPrtPutTextAt (CURRENT_LN,13, RIGHT, 6 | REL_LEFT, szOut);

      if (ulNumObs != 0L)
         dpct = ((DOUBLE)ulCount[9] / (DOUBLE)ulNumObs);
      else
         dpct = 0.0;
   sprintf (szTemp, "%Lf", dpct);
   ItiFmtFormatString (szTemp, szOut, sizeof szOut, "Number,%,.", NULL);
   ItiPrtPutTextAt (CURRENT_LN, 20, LEFT, 2 | REL_LEFT, "(");
   ItiPrtPutTextAt (CURRENT_LN, 21, RIGHT, 8 | REL_LEFT, szOut);
   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1, LEFT, 2 | REL_LEFT, ")");

   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+2, LEFT, USE_RT_EDGE,
                    " more than 90% from estimate ");

   ItiPrtIncCurrentLn ();
   ItiPrtIncCurrentLn (); //blank line


   }/* End of function DoWithin */





 void EXPORT DoCalcSection (HHEAP hheap, PSZ pszWhereClause, PMATRIX  pmat, PSZ pszMajorItemKey)
   {
#define  FACTOR_SIZE  14
   CHAR szTemp [900] = "";
   CHAR szQry  [1020] = "";
   CHAR szQuery [1020] = "";
//   CHAR szMedianQty   [32] = "5";
//   CHAR szMedianQtyRaw[32] = "";
//   CHAR szBasePrice   [2 * FACTOR_SIZE] = "";
   CHAR szAdjPct      [32] = "";
   CHAR szSlope     [ FACTOR_SIZE + 1 ] = "";  
   CHAR szIntercept [ FACTOR_SIZE + 1 ] = "";  
//   HQRY     hqry;
//   USHORT   usNumCols, usCnt, us;
//   USHORT   usError;
//   PSZ      *ppszDCS;
   PSZ pszSlope, pszIntercept, pszRes;
//   DOUBLE   dC1, dC2;

   /* -- Figure out the median quantity (szMedianQty). */
   pszRes = ParametMajorItemMedianQty (hheap, TRUE, pszMajorItemKey, pszWhereClause);
   ItiStrCpy (szMedianQty, pszRes, sizeof szMedianQty);

   //pszRes = ParametMajorItemMedianQty (hheap, FALSE, pszMajorItemKey, pszWhereClause);
   ItiExtract (pszRes, ",");
   ItiStrCpy(szMedianQtyRaw, pszRes, sizeof szMedianQtyRaw);//unformatted, ie. no commas
    

   ItiPrtPutTextAt(CURRENT_LN, 9, LEFT, (20|REL_LEFT), "Median Quantity =  ");
   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL, LEFT, USE_RT_EDGE, szMedianQty);
   /* need to determine and write the units here. */
   ItiPrtIncCurrentLn ();


   /* -- Now determine the base unit price for the median quantity. */
   /* -- Formula used:  exp( Intercept + Slope * log(MedianQty) )   */  
   /* -- Formula used:  exp(c1 + c2*log(MedianQty))                 */  

   /* -- Compute the Slope. */
   pszSlope = ParametMajorItemSlopeP (hheap, pszMajorItemKey, pszWhereClause);
   if ((pszSlope != NULL) && ((*pszSlope) != '\0'))
      ItiStrCpy(szSlope, pszSlope, FACTOR_SIZE);
   else
      ItiStrCpy(szSlope, " 1 ", FACTOR_SIZE);


   /* --  Compute the Intercept. */
   pszIntercept = ParametMajorItemInterceptP (hheap, pszMajorItemKey, pszWhereClause);
   ItiErrWriteDebugMessage (pszIntercept);
   if ((pszIntercept != NULL) && ((*pszIntercept) != '\0'))
      ItiStrCpy(szIntercept, pszIntercept, FACTOR_SIZE);
   else
      ItiStrCpy(szIntercept, " 1 ", FACTOR_SIZE);



   ItiMbQueryQueryText (hmod, ITIRID_RPT, GET_BASE_PRICE, szTemp, sizeof szTemp);
   sprintf(szQry, szTemp, szIntercept, szSlope, szMedianQtyRaw);
   ItiErrWriteDebugMessage (szQry);

   //pszRes = ItiDbGetRow1(szQry,hheap,hmod,ITIRID_RPT,GET_BASE_PRICE,&usNumCols);
   pszRes = ItiDbGetRow1Col1(szQry,hheap,0,0,0);
   if ((pszRes != NULL) && ((*pszRes) != '\0'))
      ItiStrCpy(szBasePrice, pszRes, FACTOR_SIZE + FACTOR_SIZE);
   else
      ItiStrCpy(szBasePrice, " 0.0 ", 5);


   ItiPrtPutTextAt(CURRENT_LN, 9, LEFT, (20|REL_LEFT), "Base Unit Price =  ");
   ItiFmtFormatString (szBasePrice, szTemp, sizeof szTemp, "Number,$,....", NULL);
   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL, LEFT, USE_RT_EDGE, szTemp);
   ItiPrtIncCurrentLn ();
   ItiPrtIncCurrentLn ();
     


   /* -- Decrease by x% if quantity doubles. */
   /* -- Formula used:  exp(-c2 * log(2)) -1 */  
   //dC2 = pmat->pdMat[1][0] * (-1.0); <== wrong
   ItiMbQueryQueryText (hmod, ITIRID_RPT, GET_ADJ_PCT, szTemp, sizeof szTemp);
   //sprintf(szQry, szTemp, dC2, 2.0); /* doubled */
   sprintf(szQry, szTemp, szSlope, "2.0");

   pszRes = ItiDbGetRow1Col1(szQry,hheap,0,0,0);
                             // hmod,ITIRID_RPT,GET_ADJ_PCT,&usNumCols);

   if ((pszRes != NULL) && ((*pszRes) != '\0'))
      ItiStrCpy(szAdjPct, pszRes, sizeof szAdjPct);
   else
      ItiStrCpy(szAdjPct, "0.0", 5);

   /* -- Copy Adj value so it can be copied into the csv file. */
   ItiStrCpy(szQuantityAdjustment, szAdjPct, sizeof szQuantityAdjustment);

   ItiPrtPutTextAt(CURRENT_LN, 9, LEFT, (20|REL_LEFT), "Decrease price by  ");
   ItiFmtFormatString (szAdjPct, szTemp, sizeof szTemp, "Number,%,..", NULL);
   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL, LEFT, USE_RT_EDGE, szTemp);
   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL, LEFT, USE_RT_EDGE,
                   "  each time the quantity doubles.");
   ItiPrtIncCurrentLn ();





   /* -- Increase by x% if quantity halves. */
   /* -- Formula used:  exp(-c2 * log(0.5)) -1 */  
   // dC2 = pmat->pdMat[1][0] * (-1.0);
   ItiMbQueryQueryText (hmod, ITIRID_RPT, GET_ADJ_PCT, szTemp, sizeof szTemp);
   // sprintf(szQry, szTemp, dC2, 0.5);  /* halved */
   sprintf(szQry, szTemp, szSlope, "0.5");

   pszRes = ItiDbGetRow1Col1(szQry,hheap,0,0,0);
                             // hmod,ITIRID_RPT,GET_ADJ_PCT,&usNumCols);

   if ((pszRes != NULL) && ((*pszRes) != '\0'))
      ItiStrCpy(szAdjPct, pszRes, sizeof szAdjPct);
   else
      ItiStrCpy(szAdjPct, "0.0", 5);


   ItiPrtPutTextAt(CURRENT_LN, 9, LEFT, (20|REL_LEFT), "Increase price by  ");
   ItiFmtFormatString (szAdjPct, szTemp, sizeof szTemp, "Number,%,..", NULL);
   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL, LEFT, USE_RT_EDGE, szTemp);
   ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL, LEFT, USE_RT_EDGE,
                   "  each time the quantity is halved.");
   ItiPrtIncCurrentLn ();

   }/* END of Function DoCalc */







BOOL EXPORT DoMajorItemPrice (HHEAP hheap,
                         FILE  *pf,
                         PSZ   pszWhere,
                         PSZ   *ppszDMIP)
   {
   static PMATRIX  pmatReg, pmatResults;
   BOOL   bAnalyzed;
//   BOOL   bRet;
   ULONG  ulNumObs;
   USHORT usNumCols, usError;
   CHAR  szMajorItemKeyText[20]=""; 
   CHAR  szBaseDate [30]="";
   CHAR  szQuery [710] = "";
   CHAR  szTmp [600] = "";
//   CHAR  szMedianQty[30]="";
//   CHAR  szMedianQtyUnitPrice[30]="";
//   CHAR  szQuantityAdjustment[30]="";
   PSZ  pszBaseDate = NULL;
   CHAR szGetBaseDate[] =
           "select LTRIM(STR(DATEPART(mm,MAX(BaseDate)))) "
           " + CHAR(047) + LTRIM(STR(DATEPART(dd,MAX(BaseDate)))) "
           " + CHAR(047) + LTRIM(STR(DATEPART(yy,MAX(BaseDate)))) "
           " + CHAR(032) + LTRIM(STR(DATEPART(hh,MAX(BaseDate)))) "
           " + CHAR(058) + LTRIM(STR(DATEPART(mi,MAX(BaseDate)))) "  
           "from BaseDate ";



   /* -- Check for new page. */
   if ((ItiPrtKeepNLns(8) == PG_WAS_INC)
       || (ItiPrtQueryCurrentPgNum() != usCurrentPg ))
      {
      usCurrentPg = ItiPrtQueryCurrentPgNum();
      ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
      }

   /* -- Create the MajorItemKeyText value. */
   ItiStrCpy (szMajorItemKeyText, ppszDMIP[1], 5 );
   ItiStrCat (szMajorItemKeyText, " ", sizeof szMajorItemKeyText );

   /* -- If spec year is null, then explicitly say so. */
   if ((ppszDMIP[3] == NULL) || (ppszDMIP[3][0] == '\0'))
      ItiStrCat (szMajorItemKeyText, "NULL", sizeof szMajorItemKeyText );
   else
      ItiStrCat (szMajorItemKeyText, ppszDMIP[3], sizeof szMajorItemKeyText );

   /* -- Get the Basedate value. */
   pszBaseDate = ItiDbGetRow1Col1(szGetBaseDate, hheap, 0, 0, 0);
   if ((pszBaseDate != NULL) && (*pszBaseDate != '\0'))
      ItiStrCpy (szBaseDate, pszBaseDate, sizeof szBaseDate);
   else
      ItiStrCpy (szBaseDate, UNDECLARED_BASE_DATE, sizeof szBaseDate);

// new section...

   ItiStrCpy (szQuery, " SELECT HPMI.Quantity ",  sizeof szQuery);
   ItiMbQueryQueryText (hmod, ITIRID_RPT, GET_S_I,
                        szTmp, sizeof szTmp);
   ItiStrCat (szQuery, szTmp, sizeof szQuery);
   ItiStrCat (szQuery, ppszDMIP[0], sizeof szQuery);
   ItiStrCat (szQuery, "  ", sizeof szQuery);
   ItiStrCat (szQuery, pszWhere, sizeof szQuery);

   ulNumObs = (ULONG) ItiDbGetQueryCount (szQuery, &usNumCols, &usError);

   DoWithin (hheap, pszWhere, ppszDMIP [0], ulNumObs, pmatResults); // test
   DoCalcSection (hheap, pszWhere, pmatResults, ppszDMIP[0]);       // test

   /* -- Write the export lines for MajorItemPriceRegression. */
   ItiCSVWriteLine (pf, "MajorItemPriceRegression",
                 szMajorItemKeyText, 
                 szBaseDate,
                 szMedianQtyRaw,
                 szBasePrice,          /* ie. szMedianQtyUnitPrice */
                 szQuantityAdjustment, /* computed in DoCalcSection */
                 NULL);
   /* -- end write to export file for this section. */

 return TRUE;

// ...to replace the following:  

   /* do the regression */
   bAnalyzed = DoRegression (hheap, pszWhere,
                             ppszDMIP [0],
                             &ulNumObs, 
                             &pmatReg, &pmatResults);
   if (bAnalyzed)
      {
      sprintf (szBaseDate,           "%lf", pmatResults->pdMat [0] [0]);
      sprintf (szMedianQty,          "%lf", pmatResults->pdMat [1] [0]);
      sprintf (szMedianQtyUnitPrice, "%lf", pmatResults->pdMat [2] [0]);
      sprintf (szQuantityAdjustment, "%lf", pmatResults->pdMat [3] [0]);

      /* -- Write the export lines for MajorItemPriceRegression. */
      ItiCSVWriteLine (pf, "MajorItemPriceRegression",
                    szMajorItemKeyText, 
                    szBaseDate,
                    szMedianQty,
                    szMedianQtyUnitPrice, 
                    szQuantityAdjustment, 
                    NULL);
      /* -- end write to export file for this section. */

      DoWithin (hheap, pszWhere, ppszDMIP [0], ulNumObs, pmatResults);
      DoCalcSection (hheap, pszWhere, pmatResults, ppszDMIP[0]);

      FreeMatrix (pmatReg);
      FreeMatrix (pmatResults);
      }
   else
      {
      //DoWithin (hheap, pszWhere, ppszDMIP [0], ulNumObs, pmatResults); // test
      DoCalcSection (hheap, pszWhere, pmatResults, ppszDMIP[0]);                 // test

      /* -- Write the empty export lines for MajorItemPriceRegression. */
      ItiCSVWriteLine (pf, "MajorItemPriceRegression",
                    szMajorItemKeyText, 
                    szBaseDate,
                    szMedianQty,  /* szMedianQty          */
                    szBasePrice,  /* szMedianQtyUnitPrice */
                    " ",       /* szQuantityAdjustment */
                    NULL);
      /* -- end write to export file for this section. */

      ItiPrtPutTextAt (CURRENT_LN, 5, LEFT | WRAP, 5 | FROM_RT_EDGE, 
                       "There is not enough information for this major item to be analyzed.");
      ItiPrtIncCurrentLn ();
      }

   return bAnalyzed;
   }

        




 BOOL EXPORT MajorItemSection (HHEAP hheap, PSZ pszWhere, PSZ pszFileName)
   {
   FILE     *pf;
   HHEAP    hhpLoc = NULL;
   char     szQuery [4090] = "";
   CHAR     szMajorItemKeyText[20] = "";
   CHAR     szQry[200] = "";
   HQRY     hqry, hqry2;
   USHORT   usNumCols, usColCnt, usError, usC;
   PSZ      *ppsz, *ppszRes, pszRes = NULL;
   PSZ      *ppszMIS;

   pf = _fsopen (pszFileName, "wt", SH_DENYWR);
   if (pf == NULL)
      return FALSE;

   fprintf (pf, ";created by dprice model, compiled on %s\n", 
            szCompileDateTime);
   fprintf (pf, ";Model run on %.2d/%.2d/%.4d %.2d:%.2d%cm.\n",
            (USHORT) pglobals->pgis->month,
            (USHORT) pglobals->pgis->day,
            (USHORT) pglobals->pgis->year,
            (USHORT) pglobals->pgis->hour % 12,
            (USHORT) pglobals->pgis->minutes,
            pglobals->pgis->hour > 12 ? (char) 'p' : (char) 'a');

   ItiCSVWriteLine (pf, "DS/Shell Import File", "1.00", NULL);
   ItiCSVWriteLine (pf, "MajorItemPriceRegression", "MajorItemKey", 
                    "BaseDate", "MedianQuantity", "MedianQuantityUnitPrice", 
                    "QuantityAdjustment", 
                    NULL);


   ItiMbQueryQueryText (hmod, ITIRID_RPT, GET_MAJOR_ITEMS,
                        szQuery, sizeof szQuery);
   ItiStrCat (szQuery, pszWhere, sizeof szQuery);
   ItiStrCat (szQuery, " ORDER BY MI.MajorItemID ", sizeof szQuery);

   hqry = ItiDbExecQuery (szQuery, hheap, hmod, ITIRID_RPT,
                          GET_MAJOR_ITEMS, &usNumCols, &usError);
   if (hqry == NULL)
      return FALSE;

   while (ItiDbGetQueryRow (hqry, &ppszMIS, &usError))
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

      /* MajorItemID ppszMIS[1] */
      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+1, LEFT,(8|REL_LEFT), ppszMIS[1]); 
      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+2, LEFT,(3|REL_LEFT), " - ");

      /* Description ppszMIS[2] */
      ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL+1,LEFT,(40|REL_LEFT), ppszMIS[2]);
      ItiPrtIncCurrentLn (); //line feed
      ItiPrtIncCurrentLn (); //blank line

      DoMajorItemPrice (hheap, pf, pszWhere, ppszMIS);

      /* -- Finish out this Major-Item section. */
      ItiPrtIncCurrentLn ();
      ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
      ItiPrtIncCurrentLn ();

      ItiFreeStrArray (hheap, ppszMIS, usNumCols);
      }

/////////////////////////
   /* write the export lines */
   /* -- MajorItemQuantityVariable table section. JAN 96 */
   hqry = ItiDbExecQuery (szQuery, hheap, hmod, ITIRID_RPT,
                          GET_MAJOR_ITEMS, &usNumCols, &usError);
   if (hqry == NULL)
      return FALSE;

   ItiCSVWriteLine (pf, "MajorItemPriceVariable"
                      , "MajorItemKey", "BaseDate"
                      , "QualitativeVariable", NULL);

   /* -- need another heap here. */
   hhpLoc = ItiMemCreateHeap (MAX_HEAP_SIZE);
   usC = 0;
   while (ItiDbGetQueryRow (hqry, &ppsz, &usError))
      {
      /* -- increment heap tracker. */
      usC++;
      if (usC > 25)
        {
        ItiMemDestroyHeap (hhpLoc);
        hhpLoc = ItiMemCreateHeap (MAX_HEAP_SIZE);
        usC = 0;
        }

      ItiStrCpy (szMajorItemKeyText, ppsz[1], 5 );
      ItiStrCat (szMajorItemKeyText, " ", sizeof szMajorItemKeyText );

      /* -- If spec year is null, then explicitly say so. */
      if ((ppsz[3] == NULL) || (ppsz[3][0] == '\0'))
         ItiStrCat (szMajorItemKeyText, "NULL", sizeof szMajorItemKeyText );
      else
         ItiStrCat (szMajorItemKeyText, ppsz[3], sizeof szMajorItemKeyText );

      ItiStrCpy (szQry,
         "select BaseDate, QualitativeVariable from MajorItemPriceVariable "
         " where BaseDate = (select MAX(BaseDate) from MajorItemPriceVariable)"
         " and MajorItemKey = ", sizeof szQry);
      ItiStrCat (szQry, ppsz[0],sizeof szQry);

      hqry2 = ItiDbExecQuery (szQry, hhpLoc, 0, 0, 0, &usColCnt, &usError);
      if (hqry2 != NULL)
         while (ItiDbGetQueryRow (hqry2, &ppszRes, &usError))
            {
            if (ppszRes != NULL) 
               ItiCSVWriteLine (pf, "MajorItemPriceVariable"
                            , szMajorItemKeyText    /* MajorItemKeyText */
                            , ppszRes[0], ppszRes[1], NULL);
            ItiFreeStrArray (hhpLoc, ppszRes, usColCnt);
            }

      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }

   if (hhpLoc != NULL)
      ItiMemDestroyHeap (hhpLoc);


   /* -- MajorItemPriceAdjustment table section. JAN 96 */
   hqry = ItiDbExecQuery (szQuery, hheap, hmod, ITIRID_RPT,
                          GET_MAJOR_ITEMS, &usNumCols, &usError);
   if (hqry == NULL)
      return FALSE;

   ItiCSVWriteLine (pf, "MajorItemPriceAdjustment"
                      , "MajorItemKey", "BaseDate"
                      , "QualitativeVariable"
                      , "QualitativeValue", "PriceAdjustment", NULL);

   /* -- need another heap here. */
   hhpLoc = ItiMemCreateHeap (MAX_HEAP_SIZE);
   usC = 0;
   while (ItiDbGetQueryRow (hqry, &ppsz, &usError))
      {
      /* -- increment heap tracker. */
      usC++;
      if (usC > 25)
        {
        ItiMemDestroyHeap (hhpLoc);
        hhpLoc = ItiMemCreateHeap (MAX_HEAP_SIZE);
        usC = 0;
        }

      ItiStrCpy (szMajorItemKeyText, ppsz[1], 5 );
      ItiStrCat (szMajorItemKeyText, " ", sizeof szMajorItemKeyText );

      /* -- If spec year is null, then explicitly say so. */
      if ((ppsz[3] == NULL) || (ppsz[3][0] == '\0'))
         ItiStrCat (szMajorItemKeyText, "NULL", sizeof szMajorItemKeyText );
      else
         ItiStrCat (szMajorItemKeyText, ppsz[3], sizeof szMajorItemKeyText );

      ItiStrCpy (szQry,
         "select BaseDate, QualitativeVariable, QualitativeValue, PriceAdjustment"
         " from MajorItemPriceAdjustment "
         " where BaseDate = (select MAX(BaseDate) from MajorItemPriceAdjustment)"
         " and MajorItemKey = ", sizeof szQry);
      ItiStrCat (szQry, ppsz[0],sizeof szQry);
   
      hqry2 = ItiDbExecQuery (szQry, hhpLoc, 0, 0, 0, &usColCnt, &usError);
      if (hqry2 != NULL)
         while (ItiDbGetQueryRow (hqry2, &ppszRes, &usError))
            {
            if (ppszRes != NULL) 
               ItiCSVWriteLine (pf, "MajorItemPriceAdjustment"
                       , szMajorItemKeyText        /* MajorItemKeyText */
                       , ppszRes[0], ppszRes[1], ppszRes[2], ppszRes[3], NULL);

            ItiFreeStrArray (hhpLoc, ppszRes, usColCnt);
            }

      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }

   if (hhpLoc != NULL)
      ItiMemDestroyHeap (hhpLoc);

/////////////////////////
   fclose (pf);
   return TRUE;
   }



 BOOL EXPORT DoHeader (HHEAP    hheap,
                      PSZ      pszWhere,
                      PSZ      pszFileName,
                      USHORT   usFileName)
   {
   static USHORT usCount = 0;
   USHORT usLn = 10;
   CHAR szXlatedWhere [1000] = ""; 

   sprintf (pszFileName, "dpri%.4x.csv", usCount);

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
   // ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1, LEFT | WRAP, 
   //                  2 | FROM_RT_EDGE, pszWhere);
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

 void EXPORT GetInfo (PSZ      pszFile, 
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



 BOOL EXPORT PriceAnalysis (HHEAP hheap, 
                            PSZ pszInfoFile)
   {
   char     szWhere [2040]="";
   char  szFileName [250]="";

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
   char        szKeyFile [250];
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

   ItiPrtBeginReport ("Major Item Price Analysis");

   szKeyFile [0] = '\0';
   for (i=0; i < iArgCount; i++)
      {
      if (ItiStrICmp (ppszArgs [i], "-modelinfo") == 0 && i+1 < iArgCount)
         ItiStrCpy (szKeyFile, ppszArgs [i+1], sizeof szKeyFile);
      }

   PriceAnalysis (hheap, szKeyFile);

   /* ------------------------------------------------------------------- *\
    * Finish up and quit.                                                 *
   \* ------------------------------------------------------------------- */
   ItiPrtEndReport ();

   return 0;

   }/* END OF FUNCTION ItiRptDLLPrintReport */








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
   return TRUE;
   }



