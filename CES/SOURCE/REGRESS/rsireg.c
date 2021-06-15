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


#include    "rindlsir.h"


#include "rSIReg.h"                                       



/* ======================================================================= *\
 *                                         MANIFEST CONSTANT DEFINITIONS   *
\* ======================================================================= */
#define  BLANK_LINE_INTERVAL    4
#define  PRT_Q_NAME             "StdItm: PEMETH Regress"

#define  QUERY_ARRAY_SIZE       1023

#define  SIZE_OF_QUERY_RPT_DESC QUERY_ARRAY_SIZE


#define SETQERRMSG ItiStrCpy (szErrorMsg,                                    \
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
                   ItiStrCat (szErrorMsg, szQuery, sizeof szErrorMsg);       \
                                                                             \
                   ItiRptUtErrorMsgBox (NULL, szErrorMsg)




/* ======================================================================= *\
 *                                             MODULE'S GLOBAL VARIABLES   *
\* ======================================================================= */
static  CHAR  szCompileDateTime[] = __DATE__ " " __TIME__ ;
static CHAR szDllVersion[] = "1.1a0 rsireg.dll";

static  CHAR  szCurStdItm [SMARRAY + 1] = "";
static  CHAR  szImpDate   [SMARRAY + 1] = "";
static  CHAR  szOrderingStr [SMARRAY + 1] = "";

static  CHAR  szFailedDLL [CCHMAXPATH + 1] = "";
static  CHAR  szErrorMsg  [1024] = "";

static  USHORT usNumOfTitles;
static  USHORT usCurrentPg = 0;

static  CHAR  szQuery  [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szQueryA [SIZE_OF_QUERY_AREA   + 32] = "";
static  CHAR  szQueryS [SIZE_OF_QUERY_SEASON + 32] = "";
static  CHAR  szQueryW [SIZE_OF_QUERY_WRKTYP + 32] = "";
static  CHAR  szKey    [SMARRAY + 1] = "";

static  CHAR  szQueryListing [SIZE_OF_QUERY_LISTING + 1] = "";
static  CHAR  szQueryArea    [SIZE_OF_QUERY_AREA    + 1] = "";
static  CHAR  szQuerySeason  [SIZE_OF_QUERY_SEASON  + 1] = "";
static  CHAR  szQueryWrkTyp  [SIZE_OF_QUERY_WRKTYP  + 1] = "";
static  CHAR  szConjSAW      [SIZE_OF_CONJ_SAW + 1] = "";


           /* Used by DbExecQuer & DbGetQueryRow functions: */
static  HHEAP   hhprSIReg  = NULL;
static  HQRY    hqry   = NULL;
static  USHORT  usResId   = ITIRID_RPT;
static  USHORT  usId      = ID_QUERY_LISTING;
static  USHORT  usNumCols = 0;
static  USHORT  usErr     = 0;
static  PSZ   * ppsz   = NULL;

           /* Title strings for this module. */
static  REPTITLES rept;
static  PREPT     prept = &rept;

static  COLTTLS  acttl[RPT_TTL_NUM + 1];
                



USHORT SetTitles (PREPT preptitles, INT argcnt, CHAR * argvars[])
   {
   USHORT us = 1;
   CHAR szOpt[FOOTER_LEN + 1];
   PSZ  pszOpt = szOpt;

   if (preptitles == NULL)
      return (ITIRPT_INVALID_PARM);

   ItiStrCpy(preptitles->szTitle,   TITLE,    sizeof(preptitles->szTitle)   );
   ItiStrCpy(preptitles->szSubTitle,SUB_TITLE,sizeof(preptitles->szSubTitle));
   ItiStrCpy(preptitles->szFooter,  FOOTER,   sizeof(preptitles->szFooter)  );

   /* -- Set the subsubtitle text to be empty for this report. */
   preptitles->szSubSubTitle[0] = '\0';


   /* -- Check for use of footer override option. */
   ItiRptUtGetOption (ITIRPTUT_FOOTER_SWITCH,
                      preptitles->szFooter, sizeof(preptitles->szFooter),
                      argcnt, argvars);

   return 0;
   }/* END OF FUNCTION SetTitles */







USHORT SetColumnTitles (void)
   /* -- Print the column titles. */
   {
   USHORT us;


   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_17].usTtlCol,
                    LEFT, (acttl[RPT_TTL_17].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_17].szTitle);
   ItiPrtIncCurrentLn (); //line feed

   for (us=RPT_TTL_18; us <= RPT_TTL_20; us++)
      {
      ItiPrtPutTextAt (CURRENT_LN, acttl[us].usTtlCol,
                       LEFT, (acttl[us].usTtlColWth | REL_LEFT),
                       acttl[us].szTitle);
      ItiPrtDrawSeparatorLn (CURRENT_LN +1,
                       acttl[us].usDtaCol,
                       (acttl[us].usDtaCol + acttl[us].usDtaColWth));

      }
   ItiPrtIncCurrentLn (); //line feed
   us = ItiPrtIncCurrentLn (); //line feed

   return us;
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








USHORT SummarySectionFor (PSZ pszKey, PSZ pszImpDate)
   {
   USHORT usLn, us;
   PSZ * ppsz = NULL;


   if ((pszKey == NULL) || (*pszKey == '\0'))
      return 13;

   /* -- Build the query. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryListing, " ", pszKey);


   ppsz = ItiDbGetRow1 (szQuery, hhprSIReg, hmodrSIReg, ITIRID_RPT,
                        ID_QUERY_LISTING, &usNumCols);

   if (ppsz != NULL)
      {
//      usLn = N + ItiStrLen(ppsz[DESC]) / acttl[RPT_TTL_n2].usDtaColWth;
      if ((ItiPrtKeepNLns(20) == PG_WAS_INC)
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
                       ppsz[SIN]);
      ItiStrCpy(szCurStdItm, ppsz[SIN], sizeof szCurStdItm);

      us = acttl[RPT_TTL_1].usDtaCol + ItiStrLen(ppsz[SIN]);

      ItiPrtPutTextAt (CURRENT_LN, us,
                       LEFT, (acttl[RPT_TTL_2].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_2].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL +1,
                       LEFT,
                       (2 | FROM_RT_EDGE),
                       //(acttl[RPT_TTL_2].usDtaColWth | REL_LEFT),
                       ppsz[DESC]);
      ItiPrtIncCurrentLn();
      ItiPrtIncCurrentLn();

      /* -- PEMETH Reg... */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_3].usTtlCol,
                       LEFT, (acttl[RPT_TTL_3].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_3].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_3].usDtaCol,
                       LEFT, (acttl[RPT_TTL_3].usDtaColWth | REL_LEFT),
                       ppsz[IMPDATE]);
      /* Save the FORMATTED import date to pass on to the adjustments section. */
      ItiStrCpy(pszImpDate, " '", sizeof szImpDate);
      ItiStrCat(pszImpDate, ppsz[QRY_IMPDATE], sizeof szImpDate);
      ItiStrCat(pszImpDate, "' ", sizeof szImpDate);
      ItiRptUtErrorMsgBox (NULL, pszImpDate);

      ItiPrtIncCurrentLn();

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_4].usTtlCol,
                       LEFT, (acttl[RPT_TTL_4].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_4].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_4].usDtaCol,
                       LEFT, (acttl[RPT_TTL_4].usDtaColWth | REL_LEFT),
                       ppsz[AREA]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 2,
                       LEFT, (acttl[RPT_TTL_5].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_5].szTitle);
      ItiPrtIncCurrentLn();
      ItiPrtIncCurrentLn(); // blank line

      /* -- Only items with... */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_6].usTtlCol,
                       LEFT, (acttl[RPT_TTL_6].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_6].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (acttl[RPT_TTL_6].usDtaColWth | REL_LEFT),
                       ppsz[MIN_QTY]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (acttl[RPT_TTL_7].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_7].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (acttl[RPT_TTL_7].usDtaColWth | REL_LEFT),
                       ppsz[MAX_QTY]);
      ItiPrtIncCurrentLn();

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_8].usTtlCol,
                       LEFT, (acttl[RPT_TTL_8].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_8].szTitle);
      ItiPrtIncCurrentLn();
      ItiPrtIncCurrentLn(); // blank line



      /* -- Median Quant... sub-section */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_9].usTtlCol,
                       LEFT, (acttl[RPT_TTL_9].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_9].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (acttl[RPT_TTL_9].usDtaColWth | REL_LEFT),
                       ppsz[MED_QTY]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 2,
                       LEFT, (acttl[RPT_TTL_9].usDtaColWth | REL_LEFT),
                       ppsz[CODE]);
      ItiPrtIncCurrentLn();

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_10].usTtlCol,
                       LEFT, (acttl[RPT_TTL_10].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_10].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (acttl[RPT_TTL_10].usDtaColWth | REL_LEFT),
                       ppsz[BASE_DATE]);
      ItiPrtIncCurrentLn();

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_11].usTtlCol,
                       LEFT, (acttl[RPT_TTL_11].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_11].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (acttl[RPT_TTL_11].usDtaColWth | REL_LEFT),
                       ppsz[BASE_PRICE]);
      ItiPrtIncCurrentLn();
      ItiPrtIncCurrentLn(); // blank line


      /* -- Annual infla... sub-section */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usTtlCol,
                       LEFT, (acttl[RPT_TTL_12].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_12].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (acttl[RPT_TTL_12].usDtaColWth | REL_LEFT),
                       ppsz[INFLA_YR]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (acttl[RPT_TTL_13].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_13].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (acttl[RPT_TTL_14].usDtaColWth | REL_LEFT),
                       ppsz[INFLA_MO]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (acttl[RPT_TTL_14].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_14].szTitle);
      ItiPrtIncCurrentLn();

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_15].usTtlCol,
                       LEFT, (acttl[RPT_TTL_15].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_15].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (acttl[RPT_TTL_15].usDtaColWth | REL_LEFT),
                       ppsz[ADJ]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (acttl[RPT_TTL_16].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_16].szTitle);
      ItiPrtIncCurrentLn();
      usLn = ItiPrtIncCurrentLn(); // blank line


      ItiFreeStrArray (hhprSIReg, ppsz, usNumCols);
      }
   else
      return ITIRPTUT_ERROR;

   return usLn;
   }/* End of Function SummarySectionFor */





USHORT Adjustments (PSZ pszKey, PSZ pszImpDate)
   {
//   BOOL   bTest;
   BOOL   bLabeled = FALSE;
   USHORT us;

   HQRY    hqry   = NULL;
   USHORT  usCols = 0;
   USHORT  usErr  = 0;
   PSZ   * ppsz   = NULL;


   USHORT usAreaCnt;
   USHORT usSeasonCnt;
   USHORT usWrkTypCnt;
   PP_KEYSIDS ppkisArea   = NULL;
   PP_KEYSIDS ppkisSeason = NULL;
   PP_KEYSIDS ppkisWrkTyp = NULL;
   PSZ pszAdjTxt; 
   PSZ pszAdjPct;  


   if ((pszKey == NULL) || (*pszKey == '\0') || (pszImpDate == NULL))
      return 13;


   /* -- Build the AREA list of qualitative adjustments. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryArea, pszKey, szConjSAW);
   ItiStrCat(szQuery, pszImpDate, sizeof szQuery);
   ItiStrCat(szQuery, " order by 1 ", sizeof szQuery);


   usAreaCnt   = ItiDbGetQueryCount(szQuery, &usCols, &usErr);

   if (usAreaCnt)
      ItiRptUtErrorMsgBox (NULL, " Area count is NOT zero.");
   ItiRptUtErrorMsgBox (NULL, szQuery);

   ItiRptUtEstablishKeyIDList (hhprSIReg, (usAreaCnt + 1),
                               sizeof(P_KEYID), &ppkisArea);

   hqry = ItiDbExecQuery (szQuery, hhprSIReg, hmodrSIReg, ITIRID_RPT,
                          ID_QUERY_AREA, &usNumCols, &usErr);
   
   /* -- Process the query results for each row. */
   if (hqry == NULL)
      {
      SETQERRMSG;
      }
   else
      {
      us = 0;
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
         {
         pszAdjTxt = (ppsz[ADJ_TXT] != NULL) ? ppsz[ADJ_TXT] : " -0- ";
         pszAdjPct = (ppsz[ADJ_PCT] != NULL) ? ppsz[ADJ_PCT] : " N/A ";
         ItiRptUtAddToKeyIDList(hhprSIReg, pszAdjPct, pszAdjTxt,
                                us, ppkisArea, usAreaCnt);
         us++;
         ItiFreeStrArray (hhprSIReg, ppsz, usNumCols);
         }/* end of while */
      }/* end of if (hqry... else clause */




   /* -- Build the SEASON list of qualitative adjustments. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQuerySeason, pszKey, szConjSAW);
   ItiStrCat(szQuery, pszImpDate, sizeof szQuery);
   ItiStrCat(szQuery, " order by 1 ", sizeof szQuery);


   usSeasonCnt = ItiDbGetQueryCount(szQuery, &usCols, &usErr);

   if (usSeasonCnt)
      ItiRptUtErrorMsgBox (NULL, " Season count is NOT zero.");
   ItiRptUtErrorMsgBox (NULL, szQuery);

   ItiRptUtEstablishKeyIDList (hhprSIReg, (usSeasonCnt + 1),
                               sizeof(P_KEYID), &ppkisSeason);

   hqry = ItiDbExecQuery (szQuery, hhprSIReg, hmodrSIReg, ITIRID_RPT,
                          ID_QUERY_SEASON, &usNumCols, &usErr);
   
   /* -- Process the query results for each row. */
   if (hqry == NULL)
      {
      SETQERRMSG;
      }
   else
      {
      us = 0;
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
         {
         pszAdjTxt = (ppsz[ADJ_TXT] != NULL) ? ppsz[ADJ_TXT] : " -0- ";
         pszAdjPct = (ppsz[ADJ_PCT] != NULL) ? ppsz[ADJ_PCT] : " N/A ";
         ItiRptUtAddToKeyIDList(hhprSIReg, pszAdjPct, pszAdjTxt,
                                us, ppkisSeason, usSeasonCnt);
         us++;
         ItiFreeStrArray (hhprSIReg, ppsz, usNumCols);
         }/* end of while */
      }/* end of if (hqry... else clause */




   /* -- Build the WROK TYPE list of qualitative adjustments. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryWrkTyp, pszKey, szConjSAW);
   ItiStrCat(szQuery, pszImpDate, sizeof szQuery);
   ItiStrCat(szQuery, " order by 1 ", sizeof szQuery);

   usWrkTypCnt = ItiDbGetQueryCount(szQuery, &usCols, &usErr);

   if (usWrkTypCnt)
      ItiRptUtErrorMsgBox (NULL, " WorkType count is NOT zero.");
   ItiRptUtErrorMsgBox (NULL, szQuery);


   ItiRptUtEstablishKeyIDList (hhprSIReg, (usWrkTypCnt + 1),
                               sizeof(P_KEYID), &ppkisWrkTyp);

   hqry = ItiDbExecQuery (szQuery, hhprSIReg, hmodrSIReg, ITIRID_RPT,
                          ID_QUERY_WRKTYP, &usNumCols, &usErr);
   
   /* -- Process the query results for each row. */
   if (hqry == NULL)
      {
      SETQERRMSG;
      }
   else
      {
      us = 0;
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
         {
         pszAdjTxt = (ppsz[ADJ_TXT] != NULL) ? ppsz[ADJ_TXT] : " -0- ";
         pszAdjPct = (ppsz[ADJ_PCT] != NULL) ? ppsz[ADJ_PCT] : " N/A ";
         ItiRptUtAddToKeyIDList(hhprSIReg, pszAdjPct, pszAdjTxt,
                                us, ppkisWrkTyp, usWrkTypCnt);
         us++;
         ItiFreeStrArray (hhprSIReg, ppsz, usNumCols);
         }/* end of while */
      }/* end of if (hqry... else clause */


//   bTest = FALSE;
   bLabeled = FALSE;
   us = 0;
   while ((us < usAreaCnt) || (us < usSeasonCnt) || (us < usWrkTypCnt)   )
//          || bTest)
      {
      if (ItiPrtKeepNLns(5) == PG_WAS_INC)
         {
         usCurrentPg = ItiPrtQueryCurrentPgNum();
         ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
         if (bLabeled)
            {
            ItiPrtPutTextAt (CURRENT_LN, 1, LEFT, (20 | REL_LEFT),
                          "(cont.) Item ");
            ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1, LEFT, (30 | REL_LEFT),
                             szCurStdItm);
            ItiPrtIncCurrentLn (); //line feed
            }
         SetColumnTitles();
         bLabeled = TRUE;
         }

      if (!bLabeled)
         {
         SetColumnTitles();
         bLabeled = TRUE;
         }

      /* -- Now write out the adjustment text and percentages. */
      /* Area */
      if (us < usAreaCnt)
         {
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_18].usDtaCol,
                          LEFT, (acttl[RPT_TTL_18].usDtaColWth | REL_LEFT),
                          ppkisArea[us]->szKey);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_18].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_18].usDtaColWth | REL_LEFT),
                          ppkisArea[us]->pszID);
         }


      /* Season */
      if (us < usSeasonCnt)
         {
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_19].usDtaCol,
                          LEFT, (acttl[RPT_TTL_19].usDtaColWth | REL_LEFT),
                          ppkisSeason[us]->szKey);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_19].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_19].usDtaColWth | REL_LEFT),
                          ppkisSeason[us]->pszID);
         }


      /* Work Type */
      if (us < usWrkTypCnt)
         {
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_20].usDtaCol,
                          LEFT, (acttl[RPT_TTL_20].usDtaColWth | REL_LEFT),
                          ppkisWrkTyp[us]->szKey);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_20].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_20].usDtaColWth | REL_LEFT),
                          ppkisWrkTyp[us]->pszID);
         }



//      if (bTest)
//         {
//         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_18].usDtaCol,
//                          LEFT, (acttl[RPT_TTL_18].usDtaColWth | REL_LEFT),
//                          "AreaXxxx");
//
//         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_18].usDtaCol,
//                          RIGHT, (acttl[RPT_TTL_18].usDtaColWth | REL_LEFT),
//                          "-99,999.99%");
//
//         if (us < 4)
//            {
//            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_19].usDtaCol,
//                             LEFT, (acttl[RPT_TTL_19].usDtaColWth | REL_LEFT),
//                             "SeasonXx");
//                           
//            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_19].usDtaCol,
//                             RIGHT, (acttl[RPT_TTL_19].usDtaColWth | REL_LEFT),
//                             "-99,999.99%");
//            }
//
//         if (us < 6)
//            {
//            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_20].usDtaCol,
//                             LEFT, (acttl[RPT_TTL_20].usDtaColWth | REL_LEFT),
//                             "WorkType");
//
//            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_20].usDtaCol,
//                             RIGHT, (acttl[RPT_TTL_20].usDtaColWth | REL_LEFT),
//                             "-99,999.99%");
//            }
//         }/* end if bTest */                

      ItiPrtIncCurrentLn (); //line feed
      us++;

//      bTest = (us <= 7);
      }/* end while */


   /* -- Recover list's memory. */
   ItiRptUtDestroyKeyIDList (hhprSIReg, &ppkisArea,   usAreaCnt);
   ItiRptUtDestroyKeyIDList (hhprSIReg, &ppkisSeason, usSeasonCnt);
   ItiRptUtDestroyKeyIDList (hhprSIReg, &ppkisWrkTyp, usWrkTypCnt);



   /* -- Finishout this section. */
   ItiPrtIncCurrentLn (); //blank line 
   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   ItiPrtIncCurrentLn (); //line feed

   }/* End of Function Adjustments */






USHORT PrintProc (HWND hwnd, MPARAM mp1, MPARAM mp2)
   {
   USHORT us    = 0;
//   BOOL   bChecked = FALSE;
//   HFILE  hAltFile;
//   HHEAP  hhprLocal = NULL;
//   CHAR   szKey        [SMARRAY] = "";
//   CHAR   szOptSwStr   [SMARRAY] = "";
//   CHAR   szCmdLn      [2*SMARRAY +1] = "";
//   CHAR   szKeyFileName[CCHMAXPATH + 1] = "";
//
//
//
//   /* -- reset the command line. */
//   ItiStrCpy (szCmdLn, RPT_DLL_NAME, sizeof(szCmdLn) );
//
//
//   bChecked = (USHORT) (ULONG) WinSendDlgItemMsg (hwnd, 
//                               DID_SELECTED, BM_QUERYCHECK, 0, 0);
//   if (bChecked)
//      {
//      /* -- Build cmdln string to do those selected. */
//      us = ItiRptUtBuildSelectedKeysFile(hwnd,
//                             Xxx_Cat_List,
//                             cXxxKey,
//                             szKeyFileName, sizeof szKeyFileName);
//
//      if ((us == ITIRPTUT_PARM_ERROR)|| (us == ITIRPTUT_ERROR))
//         ItiRptUtErrorMsgBox (hwnd,
//                "ERROR: Failed to get selected keys in "__FILE__);
//      else
//         {/* -- Prep the CmdLn to use the key file. */
//         ItiStrCat (szCmdLn, ITIRPT_OPTSW_KEYFILE, sizeof szCmdLn);
//         ItiStrCat (szCmdLn, szKeyFileName, sizeof szCmdLn);
//         }/* end of if ((us... */
//
//      }/* end of if (bChecked... then clause */
//   else
//      {/* Do ALL of the keys. */
//      hhprLocal = ItiMemCreateHeap (MIN_HEAP_SIZE);
//
//      ItiRptUtUniqueFile (szKeyFileName, sizeof szKeyFileName, TRUE);
//
//      /* -- Build the cost sheet key list query. */
//      ItiMbQueryQueryText(hmodrXxx, ITIRID_RPT, ID_QUERY_KEYS,
//                          szQueryKeys, sizeof szQueryKeys );
//
//
//      /* -- Do the database query. */
//      hqry = ItiDbExecQuery (szQueryKeys, hhprLocal, hmodrXxx, ITIRID_RPT,
//                             ID_QUERY_KEYS, &usNumCols, &usErr);
//      
//      /* ---------------------------------------------------------------- *\
//       * Process the query results for each row.                          *
//      \* ---------------------------------------------------------------- */
//      if (hqry == NULL)
//         {
//         SETQERRMSG;
//         }                     
//      else
//         {
//         ItiRptUtStartKeyFile (&hAltFile, szKeyFileName, sizeof szKeyFileName);
//
//         while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )
//            {
//            ItiRptUtWriteToKeyFile (hAltFile, ppsz[XXX_KEY], NULL);
//            ItiFreeStrArray (hhprLocal, ppsz, usNumCols);
//            }/* End of while... */
//
//         ItiRptUtEndKeyFile (hAltFile);
//
//         ItiStrCat (szCmdLn, ITIRPT_OPTSW_KEYFILE, sizeof szCmdLn);
//         ItiStrCat (szCmdLn, szKeyFileName, sizeof szCmdLn);
//         }/* else clause */
//
//      }/* end of if (bChecked... else clause */
//
//
//   us = ItiRptUtExecRptCmdLn (hwnd, szCmdLn, RPT_XXX_SESSION);

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
   ItiStrCpy (pszBuffer, "StdItm: PEMETH Reg.", usMaxSize);   
   *pusWindowID = rSIReg_RptDlgBox;                       
   }/* END OF FUNCTION */



BOOL EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID, 
                                     USHORT usActiveWindowID) 
   {                                          
   if ((usTargetWindowID == rSIReg_RptDlgBox)     
       && ( (usActiveWindowID == StandardItemCatL)
          ||(usActiveWindowID == StandardItemCatS)) )

      return TRUE;   
   else              
      return FALSE;  
   }/* END OF FUNCTION */




MRESULT EXPORT ItiDllrSIRegRptDlgProc (HWND  hwnd,  
                                    USHORT   usMessage,
                                    MPARAM   mp1,
                                    MPARAM   mp2)
   {
   USHORT us = 0;
   CHAR   szCmdLn  [LGARRAY+1] = RPT_DLL_NAME " ";
   CHAR   szKeyStr [SMARRAY+1] = "";

   szKeyStr[0] = '\0';

   switch (usMessage)
      {
      case WM_INITDLG:
         us = ItiRptUtInitDlgBox (hwnd, RPT_RSIREG_SESSION, 0, NULL);
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
   PSZ     pszKey = szKey;

           /* General return code variable: */
   USHORT  us = 0;

           /* Page formatting location variables: */
   USHORT  usLn  = 1;
   USHORT  usCol = 0;
   LONG    lColWidthPgUnits = 0L;
   INT     iBreakLnCnt = 0;      /* line counter */
   BOOL    bTitlesSet  = FALSE;



   /* ------------------------------------------------------------------- *\
    *  Create a memory heap.                                              *
   \* ------------------------------------------------------------------- */
   hhprSIReg = ItiMemCreateHeap (MIN_HEAP_SIZE);


   /* ------------------------------------------------------------------- *\
    * Initialize the strings.                                             *
   \* ------------------------------------------------------------------- */
   ItiStrCpy(szImpDate, UNDECLARED_BASE_DATE, sizeof(szImpDate));

   /* -- Get any query ordering information, BEFORE setting rpt titles. */
   ItiRptUtGetOption (ITIRPTUT_ORDERING_SWITCH, szOrderingStr, LGARRAY,
                      argcnt, argvars);
   ConvertOrderingStr (szOrderingStr);


//   ItiRptUtGetOption (OPT_SW_INCL_XYZ_RPT, szSuppRpt, sizeof szSuppRpt,
//                      argcnt, argvars);
//   if (szSuppRpt[0] == '\0') 
//      bPrtRpt = FALSE;
//   else
//      bPrtRpt = TRUE;



   /* -- Get the report's titles for this DLL. */
   ItiRptUtLoadLabels (hmodrSIReg, &usNumOfTitles, &acttl[0]);

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


   ItiMbQueryQueryText(hmodrSIReg, ITIRID_RPT, ID_QUERY_LISTING,
                       szQueryListing, sizeof szQueryListing );

   ItiMbQueryQueryText(hmodrSIReg, ITIRID_RPT, ID_QUERY_AREA,
                       szQueryArea, sizeof szQueryArea );

   ItiMbQueryQueryText(hmodrSIReg, ITIRID_RPT, ID_QUERY_SEASON,
                       szQuerySeason, sizeof szQuerySeason );

   ItiMbQueryQueryText(hmodrSIReg, ITIRID_RPT, ID_QUERY_WRKTYP,
                       szQueryWrkTyp, sizeof szQueryWrkTyp );

   ItiMbQueryQueryText(hmodrSIReg, ITIRID_RPT, ID_CONJ_SAW,
                       szConjSAW, sizeof szConjSAW );


   /* -------------------------------------------------------------------- *\
    * -- For each Key...                                                   *
   \* -------------------------------------------------------------------- */
   ItiRptUtInitKeyList(argcnt, argvars); /*  */
   while (ItiRptUtGetNextKey(pszKey,sizeof(szKey)) != ITIRPTUT_NO_MORE_KEYS)
      {
      us = SummarySectionFor (pszKey, szImpDate);

      if (us != ITIRPTUT_ERROR)
         {
         Adjustments (pszKey, szImpDate);
         }

      }/* End of WHILE GetNextKey */



   /* ------------------------------------------------------------------- *\
    * Finish up and quit.                                                 *
   \* ------------------------------------------------------------------- */
   ItiPrtEndReport();

   DBGMSG("Exit ***DLL function ItiRptDLLPrintReport");
   return 0;

   }/* END OF FUNCTION ItiRptDLLPrintReport */





