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
 *
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


#include "rindljea.h"

#include "rJEAvg.h"                                       

#include "average.h"



/* ======================================================================= *\
 *                                         MANIFEST CONSTANT DEFINITIONS   *
\* ======================================================================= */
#define  BLANK_LINE_INTERVAL    4
#define  PRT_Q_NAME             "JE: Averages"

#define  QUERY_ARRAY_SIZE       1500

//#define  SIZE_OF_QUERY_RPT_DESC QUERY_ARRAY_SIZE

#define  ITEM_KEY_LEN           32
#define  DATE_LEN               32
#define  QTY_LEN                32


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
static CHAR szDllVersion[] = "1.1a0 rJEAvg.dll";

static  CHAR  szOrderingStr [SMARRAY + 1] = "";

static  CHAR  szFailedDLL [CCHMAXPATH + 1] = "";
static  CHAR  szErrorMsg  [1024] = "";

static  USHORT usNumOfTitles;
static  USHORT usCurrentPg = 0;


static  CHAR  szQry       [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szWhereConj     [SIZE_OF_CONJ + 1] = "";
static  CHAR  szJobKey             [SMARRAY + 1] = "";


static  CHAR  szQryTitleNames [SIZE_OF_QRY_TITLE_NAMES + 1] = "";
static  CHAR  szCurCountyKey  [ITEM_KEY_LEN + 1] = "";
static  CHAR  szCurCountyID   [SMARRAY + 1] = "";
static  CHAR  szCurBaseDate   [DATE_LEN + 1] = "";

static  CHAR  szCurJobItemKey      [ITEM_KEY_LEN + 1] = "";
static  CHAR  szCurStdItemKey      [ITEM_KEY_LEN + 1] = "";
static  CHAR  szCurItemImportDate      [DATE_LEN + 1] = "";
static  CHAR  szCurAreaTypeKey     [ITEM_KEY_LEN + 1] = "";
static  CHAR  szCurQuantity             [QTY_LEN + 1] = "";



static  CHAR  szQryJobBrkdwn [SIZE_OF_QRY_JOB_BRKDWN  + 1] = "";
static  CHAR  szQryBDKeys    [SIZE_OF_QRY_JOB_BRKDWN + 32] = "";

static  CHAR  szQryJobItem  [SIZE_OF_QRY_JOB_ITEM + 1] = "";
static  CHAR  szConjJobItem [SIZE_OF_CONJ_JOB_ITEM + 1] = "";

static  CHAR  szQryPcts  [SIZE_OF_QRY_PCTS  + 1] = "";
static  CHAR  szConjPcts [SIZE_OF_CONJ_PCTS + 1] = "";


static  CHAR  szQryPrices      [SIZE_OF_QRY_PRICES + 1] = "";
static  CHAR  szPriceOrder     [2 * SMARRAY + 1] = "";
static  CHAR  szCnjPricesBaseDate [4 * SMARRAY+1] = "";
static  CHAR  szCnjPricesJobKey   [2 * SMARRAY + 1] = "";
static  CHAR  szCnjPricesJobBDKey [2 * SMARRAY + 1] = "";

static  CHAR  szBaseDate       [SMARRAY + 1] = "";
static  CHAR  szTmpBaseDate    [SMARRAY + 1] = "";


static  CHAR  szQryAreaInfo      [SIZE_OF_QRY_AREA_INFO + 1] = "";
static  CHAR  szConjAreaInfoBDK  [2 * SMARRAY + 1] = "";
static  CHAR  szConjAreaInfoSIK  [SMARRAY + 1] = "";

static  CHAR  szQryAreaTypeInfo  [SIZE_OF_QRY_AREA_TYPE_INFO + 1] = "";

static  CHAR  szQryWorkTypeInfo  [SIZE_OF_QRY_WORK_TYPE_INFO + 1] = "";



static  USHORT usKeyFlag    = 0;



//static  CHAR  szConjBPQItemKey   [SMARRAY + 1] = "";
//static  CHAR  szConjBPQBrkdwnKey [SMARRAY + 1] = "";


static  USHORT usBrkdwnCnt = 0;
//static  CHAR   szCurJobBrkdwnKey [SMARRAY + 1] = "";
//static  CHAR   szCurJobBrkdwnID  [SMARRAY + 1] = "";
//static  CHAR   szCurWorkType     [SMARRAY + 1] = "";

//static  PSZ  pszCurJobBrkdwnKey  = szCurJobBrkdwnKey;
//static  PSZ  pszCurJobBrkdwnID   = szCurJobBrkdwnID; 
//static  PSZ  pszCurWorkType      = szCurWorkType;


///* -- Adjustment queries. */
//static  CHAR  szSlcAdj         [2*SMARRAY + 1] = "";
//static  CHAR  szSlcAdjRaw      [2*SMARRAY + 1] = "";
//
//
//static  CHAR  szQryArea         [SIZE_OF_QRY_AREA + 1] = "";
//static  CHAR  szConjAreaCKey    [SMARRAY + 1] = "";
//static  CHAR  szConjAreaSIKey   [SMARRAY + 1] = "";
//
//static  CHAR  szQrySeason       [SIZE_OF_QRY_SEASON + 1] = "";
//static  CHAR  szConjSeason      [SMARRAY + 1] = "";
//
//static  CHAR  szQryWorkType     [SIZE_OF_QRY_WORKTYPE + 1] = "";
//static  CHAR  szConjWorkType    [SMARRAY + 1] = "";
//
//
//static  CHAR  szConjAdjImpDate [SMARRAY + 1] = "";
//static  CHAR  szConjAdjImpDateCap [SMARRAY + 1] = "";




/* -- The following */
static  PSZ * appsz[512];                
static  USHORT usIndex_appsz = 0;
static  USHORT usBKindex = 0;
static  USHORT usCnt_appsz = 0;


           /* Used by DbExecQuer & DbGetQueryRow functions: */
static  HHEAP   hhprJEAvg  = NULL;
static  HQRY    hqry   = NULL;
static  USHORT  usResId   = ITIRID_RPT;
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




USHORT SetTitles (HHEAP  hhpSetTtls,
                  PREPT preptitles, INT argcnt, CHAR * argvars[],
                  PSZ pszBaseDate,  PSZ pszCountyKey, PSZ pszCountyID)
   {
   USHORT us, usNumCols;
   CHAR   szNewTitle[TITLE_LEN +1] = "";
   PSZ   *ppszTitleNames = NULL;
   CHAR   szOpt[FOOTER_LEN + 1];
   PSZ    pszOpt = szOpt;


   if (preptitles == NULL)
      return (ITIRPT_INVALID_PARM);



   ItiMbQueryQueryText(hmodrJEAvg, ITIRID_RPT, ID_QRY_TITLE_NAMES,
                       szQryTitleNames, sizeof szQryTitleNames );

   /* Append the Job's key to the query; the LAST argvar is the JobKey. */
   ItiStrCat(szQryTitleNames, argvars[argcnt-1], sizeof(szQryTitleNames));

   ppszTitleNames = ItiDbGetRow1(szQryTitleNames, hhpSetTtls, hmodrJEAvg,
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


      ItiStrCpy(pszBaseDate,  ppszTitleNames[JOB_BASE_DATE], DATE_LEN);
      ItiStrCpy(pszCountyKey, ppszTitleNames[COUNTY_KEY],ITEM_KEY_LEN);
      ItiStrCpy(pszCountyID,  ppszTitleNames[COUNTY_ID], SMARRAY);

      ItiFreeStrArray (hhpSetTtls, ppszTitleNames, usNumCols);
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








USHORT SetColumnTitles (USHORT usTitleStart, USHORT usTitleStop)
   /* -- Print the column titles. */
   {
   USHORT us;

      /* -- Actual printing. */
   for (us=usTitleStart; us <= usTitleStop; us++)
      {
      ItiPrtPutTextAt (CURRENT_LN, acttl[us].usTtlCol,
                       LEFT, (acttl[us].usTtlColWth | REL_LEFT),
                       acttl[us].szTitle);

      ItiPrtDrawSeparatorLn ((CURRENT_LN + 1),
                             acttl[us].usTtlCol,
                             acttl[us].usTtlCol+acttl[us].usTtlColWth);

      }

   ItiPrtIncCurrentLn (); //line feed past titles
   ItiPrtIncCurrentLn (); //line feed past underlining

   return 0;
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






USHORT MakeList(HMODULE hmodML,            HHEAP hhpML,
                        PSZ  pszQry,       USHORT usQryID,
                        BOOL bQuotedKey,   BOOL bQuotedID,
                        PUSHORT pusKeyCnt, PPP_KEYSIDS pppkisML)
/* -- This function builds a Key&ID list from the given query.  */
/* -- Assumes query column 0 is the KEY and column 1 is the ID. */
   {
   USHORT  us, usLen, usRes;
   HQRY    hqryML   = NULL;
   USHORT  usColsML = 0;
   USHORT  usErrML  = 0;
   PSZ   * ppszML   = NULL;
   PSZ     pszIDCol, pszKeyCol;
   CHAR    szTmp[2 * SMARRAY] = "";


   if ((pszQry == NULL) || (pppkisML == NULL) || (pusKeyCnt == NULL))
      return ITIRPTUT_PARM_ERROR;

   /* -- Find out how many keys (rows) we have. */
   *pusKeyCnt = ItiDbGetQueryCount(pszQry, &usColsML, &usErrML);
   usRes = ItiRptUtEstablishKeyIDList (hhpML, (*pusKeyCnt + 1),
                                       sizeof(P_KEYID), pppkisML);

   /* -- Do the database query. */
   hqryML = ItiDbExecQuery (pszQry, hhpML, hmodML, ITIRID_RPT,
                            usQryID, &usColsML, &usErrML);
   if (hqryML == NULL)
      {
      ItiRptUtErrorMsgBox (NULL, " MakeList query failed.");
      SETQERRMSG(pszQry);
      return ITIRPTUT_ERROR;
      }
   else
      {/* -- Build a list of category keys. */
      us = 0;
      while( ItiDbGetQueryRow (hqryML, &ppszML, &usErrML) )
         {
         if ((ppszML == NULL) || (*ppszML == NULL))
            break;

         pszKeyCol = (ppszML[0] != NULL) ? ppszML[0] : " 0 ";
         pszIDCol  = (ppszML[1] != NULL) ? ppszML[1] : " N/A ";
         ItiRptUtAddToKeyIDList(hhpML, pszIDCol, pszKeyCol,
                                us, *pppkisML, *pusKeyCnt);
         if (bQuotedID)
            {
            ItiStrCpy(szTmp, " '", sizeof szTmp);
            ItiStrCat(szTmp, (*pppkisML)[us]->pszID, sizeof szTmp);
            ItiStrCat(szTmp, "' ", sizeof szTmp);
            usLen = ItiStrLen(szTmp) +1;
            ItiStrCpy((*pppkisML)[us]->pszID, szTmp, usLen);
            }

         if (bQuotedKey)
            {
            ItiStrCpy(szTmp, " '", sizeof szTmp);
            ItiStrCat(szTmp, (*pppkisML)[us]->szKey, sizeof szTmp);
            ItiStrCat(szTmp, "' ", sizeof szTmp);
            usLen = ItiStrLen(szTmp) +1;
            ItiStrCpy((*pppkisML)[us]->szKey, szTmp, usLen);
            }

         us++;   /* the variable us keeps count of the query row number */

         ItiFreeStrArray (hhpML, ppszML, usColsML);
         }
      }/* end if (hqry... else clause */


   return usRes;

   }/* END OF FUNCTION MakeList */




USHORT DescriptionSection (PSZ pszJobBrkdwnID, PSZ *ppsz)
   {
   USHORT usLn;

   if (ppsz != NULL)
      {
//      usLn = N + ItiStrLen(ppsz[DESC]) / acttl[RPT_TTL_2].usDtaColWth;
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
                       (LEFT|WRAP), (USE_RT_EDGE | REL_LEFT),
                       ppsz[JOB_ITEM_DESC]);
      ItiPrtIncCurrentLn();
      usLn = ItiPrtIncCurrentLn();

      }/* end of if's then clause */
   else
      {
      }

   return usLn;
   }/* End of Function DescriptionSection */






USHORT Percentiles (HHEAP hhpPcts,                               //In
                    PSZ pszJobKey,      PSZ pszJobItemKey,       //In
                    PSZ pszStdItemKey,  PSZ pszBaseDate,         //In
                    PSZ pszCurQuantity,                          //In
                    PSZ pszAreaTypeKey, PSZ pszItemImportDate,   //Out
                    PUSHORT pusQtyLevel)                           //Out
   {
   USHORT usColM = 48;
   USHORT us, usNumCols;
   ULONG  ulQty, ul5, ul25, ul50, ul75, ul95;
   USHORT usX, usL, usU;
   USHORT usLabel, usQtyLen;
   CHAR   szL[32] = "";
   CHAR   szU[32] = "";
   CHAR   szLabel[16] = "qty    ";
   CHAR   szQtyFmt[QTY_LEN + 4] = "";
   CHAR   szLowerBound[QTY_LEN + 4] = "";
   CHAR   szUpperBound[QTY_LEN + 4] = "";
   CHAR   szLowerPct[8] = "";
   CHAR   szUpperPct[8] = "";
   CHAR   szQtyLevel[8] = "";
   CHAR   szQuantityLevel[SMARRAY + 1] = "";
   PSZ  * ppszPcts = NULL;


   ItiFmtFormatString (pszCurQuantity, szQtyFmt, sizeof szQtyFmt,
                       "Number,,...", &usQtyLen);

   usLabel = ItiStrLen(szLabel);
   usX = (usQtyLen > usLabel) ? usQtyLen : usLabel;


   ItiStrCpy(szQry, szQryPcts, sizeof szQry);
   ItiStrCat(szQry, pszBaseDate, sizeof szQry);
   ItiStrCat(szQry, szConjPcts, sizeof szQry);
   ItiStrCat(szQry, pszStdItemKey, sizeof szQry);
   ItiStrCat(szQry, ") ", sizeof szQry);
   ItiStrCat(szQry, szConjPcts, sizeof szQry);
   ItiStrCat(szQry, pszStdItemKey, sizeof szQry);



   ppszPcts = ItiDbGetRow1 (szQry, hhpPcts, hmodrJEAvg, ITIRID_RPT,
                        ID_QRY_PCTS, &usNumCols);
   if (ppszPcts != NULL)
      {
      /* -- Save the area type key. */
      if (pszAreaTypeKey != NULL)
         {
         ItiStrCpy (pszAreaTypeKey, ppszPcts[PCTS_AREA_KEY], ITEM_KEY_LEN);
         }
      else
         {
         ItiStrCpy(szErrorMsg,
                   "Notice: No AreaTypeKey in PEMETHAverage table for StandardItemKey ",
                   sizeof szErrorMsg);
         ItiStrCat(szErrorMsg, pszStdItemKey, sizeof szErrorMsg);
         ItiRptUtErrorMsgBox (NULL, szErrorMsg);
         }


      /* -- Second, determine the bounding %iles. */
      ItiStrToULONG (pszCurQuantity, &ulQty);

      ItiStrToULONG (ppszPcts[PCTS_5],   &ul5);
      ItiStrToULONG (ppszPcts[PCTS_25], &ul25);
      ItiStrToULONG (ppszPcts[PCTS_50], &ul50);
      ItiStrToULONG (ppszPcts[PCTS_75], &ul75);
      ItiStrToULONG (ppszPcts[PCTS_95], &ul95);

      if (ulQty < ul5)
         {
         ItiStrCpy (szLowerBound, "0.000", sizeof szLowerBound);
         ItiFmtFormatString (ppszPcts[PCTS_5], szUpperBound, sizeof szUpperBound,
                             "Number,,...", &usU);
         ItiStrCpy (szLowerPct, " 0", sizeof szLowerPct);
         ItiStrCpy (szUpperPct, " 5", sizeof szUpperPct);
         ItiStrCpy (szQtyLevel, "2", sizeof szQtyLevel);
         *pusQtyLevel = 2;
         }
      else if ((ulQty >= ul5) && (ulQty < ul25))
         {
         ItiFmtFormatString (ppszPcts[PCTS_5],  szLowerBound, sizeof szLowerBound,
                             "Number,,...", &usL);
         ItiFmtFormatString (ppszPcts[PCTS_25], szUpperBound, sizeof szUpperBound,
                             "Number,,...", &usU);
         ItiStrCpy (szLowerPct, " 5", sizeof szLowerPct);
         ItiStrCpy (szUpperPct, "25", sizeof szUpperPct);
         ItiStrCpy (szQtyLevel, "3", sizeof szQtyLevel);
         *pusQtyLevel = 3;
         }
      else if ((ulQty >= ul25) && (ulQty < ul50))
         {
         ItiFmtFormatString (ppszPcts[PCTS_25], szLowerBound, sizeof szLowerBound,
                             "Number,,...", &usL);
         ItiFmtFormatString (ppszPcts[PCTS_50], szUpperBound, sizeof szUpperBound,
                             "Number,,...", &usU);
         ItiStrCpy (szLowerPct, "25", sizeof szLowerPct);
         ItiStrCpy (szUpperPct, "50", sizeof szUpperPct);
         ItiStrCpy (szQtyLevel, "4", sizeof szQtyLevel);
         *pusQtyLevel = 4;
         }
      else if ((ulQty >= ul50) && (ulQty < ul75))
         {
         ItiFmtFormatString (ppszPcts[PCTS_50], szLowerBound, sizeof szLowerBound,
                             "Number,,...", &usL);
         ItiFmtFormatString (ppszPcts[PCTS_75], szUpperBound, sizeof szUpperBound,
                             "Number,,...", &usU);
         ItiStrCpy (szLowerPct, "50", sizeof szLowerPct);
         ItiStrCpy (szUpperPct, "75", sizeof szUpperPct);
         ItiStrCpy (szQtyLevel, "5", sizeof szQtyLevel);
         *pusQtyLevel = 5;
         }
      else if ((ulQty >= ul75) && (ulQty < ul95))
         {
         ItiFmtFormatString (ppszPcts[PCTS_75], szLowerBound, sizeof szLowerBound,
                             "Number,,...", &usL);
         ItiFmtFormatString (ppszPcts[PCTS_95], szUpperBound, sizeof szUpperBound,
                             "Number,,...", &usU);
         ItiStrCpy (szLowerPct, "75", sizeof szLowerPct);
         ItiStrCpy (szUpperPct, "95", sizeof szUpperPct);
         ItiStrCpy (szQtyLevel, "6", sizeof szQtyLevel);
         *pusQtyLevel = 6;
         }
      else /* (ulQty >= ul95) */
         {
         ItiFmtFormatString (ppszPcts[PCTS_95], szLowerBound, sizeof szLowerBound,
                             "Number,,...", &usL);
         szUpperBound[0] = '\0';
         szUpperPct[0]   = '\0';
         ItiStrCpy (szLowerPct, "95", sizeof szLowerPct);
         ItiStrCpy (szQtyLevel, "7", sizeof szQtyLevel);
         *pusQtyLevel = 7;
         }

      /* -- Format the determined quantity level. */
      ItiFmtFormatString (szQtyLevel, szQuantityLevel,sizeof szQuantityLevel,
                          "QuantityLevel,f", &us);



      /* -- Bounding %iles. */
      ItiStrCpy(szL, "(", sizeof szL);
      ItiStrCat(szL, szLowerPct, sizeof szL); 
      ItiStrCat(szL, acttl[RPT_TTL_11].szTitle, sizeof szL);
      ItiStrCat(szL, " <= ", sizeof szL);
      usL = ItiStrLen(szL);

      ItiPrtPutTextAt (CURRENT_LN, (usColM-usX-usL),
                       LEFT, (usL | REL_LEFT),
                       szL);

      ItiPrtPutTextAt (CURRENT_LN, (usColM-usLabel),
                       LEFT, (usLabel | REL_LEFT),
                       szLabel);

      if (szUpperPct[0] != '\0')
         {
         ItiStrCpy(szU, " < ", sizeof szU);
         ItiStrCat(szU, szUpperPct, sizeof szU);
         ItiStrCat(szU, acttl[RPT_TTL_11].szTitle, sizeof szU);
         ItiStrCat(szU, ")", sizeof szU);
         }
      else
         ItiStrCpy(szU, ")", sizeof szU);

      usU = ItiStrLen(szU);

      ItiPrtPutTextAt (CURRENT_LN, (usColM),
                       LEFT, (usU | REL_LEFT),
                       szU);
      ItiPrtIncCurrentLn();




      /* -- Bounding %ile values. */
      ItiStrCpy(szL, "(", sizeof szL);
      ItiStrCat(szL, szLowerBound, sizeof szL);  
      ItiStrCat(szL, " <= ", sizeof szL);
      usL = ItiStrLen(szL);

      ItiPrtPutTextAt (CURRENT_LN, (usColM-usX-usL),
                       LEFT, (usL | REL_LEFT),
                       szL);
      ItiPrtPutTextAt (CURRENT_LN, (usColM-usQtyLen),
                       LEFT, (usQtyLen | REL_LEFT),
                       szQtyFmt);


      if (szUpperBound[0] != '\0')
         {
         ItiStrCpy(szU, " < ", sizeof szU);
         ItiStrCat(szU, szUpperBound, sizeof szU); 
         ItiStrCat(szU, ")", sizeof szU);
         }
      else
         ItiStrCpy(szU, ")", sizeof szU);

      usU = ItiStrLen(szU);

      ItiPrtPutTextAt (CURRENT_LN, (usColM),
                       LEFT, (usU | REL_LEFT),
                       szU);
      ItiPrtIncCurrentLn();


      ItiPrtIncCurrentLn();

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_6].usTtlCol,
                       LEFT, (acttl[RPT_TTL_6].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_6].szTitle);

      /* -- Quantity Level */
      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1, LEFT, USE_RT_EDGE,
                       szQuantityLevel);

      ItiFreeStrArray (hhpPcts, ppszPcts, usNumCols);
      }
   else
      {
      SETQERRMSG(szQry);
      us = ITIRPTUT_ERROR;
      }


   ItiPrtIncCurrentLn();

   return 0;
   }/* End of Function Percentiles */





USHORT AreaWorkInfo (HHEAP hhpAWI,
                     PSZ pszJobKey, PSZ pszCurAreaTypeKey,
                     PSZ pszCurWorkTypeCode)
   {
   PSZ   *ppszInfo = NULL;
   USHORT usColCnt;


   /* === Area info. */
   ItiStrCpy(szQry, szQryAreaInfo, sizeof szQry);
   ItiStrCat(szQry, pszJobKey, sizeof szQry);
   
   ppszInfo = ItiDbGetRow1 (szQry, hhpAWI, hmodrJEAvg, ITIRID_RPT,
                        ID_QRY_AREA_INFO, &usColCnt);
   if (ppszInfo != NULL)
      {
      /* -- Area info line: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_7].usTtlCol,
                       LEFT, (acttl[RPT_TTL_7].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_7].szTitle);
   
      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (acttl[RPT_TTL_7].usDtaColWth | REL_LEFT),
                       ppszInfo[AREA_INFO_ID]);

      ItiFreeStrArray (hhpAWI, ppszInfo, usColCnt);
      }


   /* === Area Type info. */
   if ((pszCurAreaTypeKey != NULL) && (*pszCurAreaTypeKey != '\0'))
      {
      ItiStrCpy(szQry, szQryAreaTypeInfo, sizeof szQry);
      ItiStrCat(szQry, pszCurAreaTypeKey, sizeof szQry);
      
      ppszInfo = ItiDbGetRow1 (szQry, hhpAWI, hmodrJEAvg, ITIRID_RPT,
                           ID_QRY_AREA_TYPE_INFO, &usColCnt);
      if (ppszInfo != NULL)
         {
         ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                          LEFT, (acttl[RPT_TTL_8].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_8].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                          LEFT, (acttl[RPT_TTL_8].usDtaColWth | REL_LEFT),
                          ppszInfo[AREA_TYPE_INFO_ID]);
         ItiPrtIncCurrentLn();

         ItiFreeStrArray (hhpAWI, ppszInfo, usColCnt);
         }
      else
         {
         ItiPrtIncCurrentLn();
         }
      }
   else /* no area type key was given, so end the "Area is..." line. */
      ItiPrtIncCurrentLn();



   /* === Work type info. */
   if ((pszCurWorkTypeCode != NULL) && (*pszCurWorkTypeCode != '\0'))
      {
      ItiStrCpy(szQry, szQryWorkTypeInfo, sizeof szQry);
      ItiStrCat(szQry, pszCurWorkTypeCode, sizeof szQry);
      
      ppszInfo = ItiDbGetRow1 (szQry, hhpAWI, hmodrJEAvg, ITIRID_RPT,
                           ID_QRY_WORK_TYPE_INFO, &usColCnt);
      if (ppszInfo != NULL)
         {
         /* -- WorkType info line: */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_9].usTtlCol,
                          LEFT, (acttl[RPT_TTL_9].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_9].szTitle);
      
         ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                          LEFT, (acttl[RPT_TTL_9].usDtaColWth | REL_LEFT),
                          ppszInfo[WORK_TYPE_INFO_ID]);

         ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                          LEFT, (acttl[RPT_TTL_10].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_10].szTitle);
   
         ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                          LEFT, (acttl[RPT_TTL_10].usDtaColWth | REL_LEFT),
                          ppszInfo[WORK_TYPE_INFO_DESC]);
         ItiPrtIncCurrentLn();

         ItiFreeStrArray (hhpAWI, ppszInfo, usColCnt);
         }
      else
         {
         ItiPrtIncCurrentLn();
         }
      }

   ItiPrtIncCurrentLn (); //blank line

   return 0;
   }/* End of Function AreaWorkInfo */






USHORT PriceSectionFor (HHEAP hhpPrice,
                        PSZ pszJob_Key,    PSZ pszStdItm_Key,
                        PSZ pszBrkdwn_Key, PSZ pszBaseDate,
                        USHORT usQtyLevel)
   {
   USHORT us   = 0;
   USHORT usErr, usNumCols;
   USHORT usBreakLnCnt = 1;
   HQRY   hqryPrc = NULL;
   CHAR   szDontCare[] = "1";
   PSZ    pszEnv  = NULL;
   PSZ   *ppszPrc = NULL;
   PSZ   *ppszBestMatch = NULL;


   if ((pszJob_Key == NULL) || (pszJob_Key[0] == '\0'))
      {
      DosScanEnv(ITEMS_NO_PEMETH_DATA, &pszEnv);
      if ((pszEnv != NULL) && ((pszEnv[0] == 'Y') || (pszEnv[0] == 'y')) )
         {
         ItiPrtIncCurrentLn (); //blank line
         ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
         ItiPrtIncCurrentLn (); //line feed past separator line
         }
      return ITIRPTUT_PARM_ERROR;
      }

   /* -- Build the query. */
   ItiStrCpy(szQry, szQryPrices,   sizeof szQry);
   ItiStrCat(szQry, pszStdItm_Key, sizeof szQry);

   ItiStrCat(szQry, szCnjPricesJobKey, sizeof szQry);
   ItiStrCat(szQry, pszJob_Key,      sizeof szQry);

   ItiStrCat(szQry, szCnjPricesJobBDKey, sizeof szQry);
   ItiStrCat(szQry, pszBrkdwn_Key,      sizeof szQry);

   ItiStrCat(szQry, szCnjPricesBaseDate, sizeof szQry);
   ItiStrCat(szQry, pszBaseDate, sizeof szQry);

   ItiStrCat(szQry, szPriceOrder, sizeof szQry);


   /* -- Do the database query. */
   hqryPrc = ItiDbExecQuery (szQry, hhpPrice, hmodrJEAvg, ITIRID_RPT,
                          ID_QRY_PRICES, &usNumCols, &usErr);
   if (hqryPrc == NULL)
      {
      SETQERRMSG(szQry);
      }
   else
      {
      if (ItiPrtQueryRemainNumDisplayLns() > 5)
         SetColumnTitles(RPT_TTL_12, RPT_TTL_15);

      /* -- Determine the best match value. */
      ppszBestMatch = GetBestAverageKey (hhpPrice,           usQtyLevel, 
                                         szDontCare,    szDontCare,
                                         pszStdItm_Key, pszBaseDate);
      usBreakLnCnt = 0;

      while( ItiDbGetQueryRow (hqryPrc, &ppszPrc, &usErr) )  
         {
         /* ------------------------------------------------------- *\
          * If we are on a new page...                              *
         \* ------------------------------------------------------- */
         if (ItiPrtKeepNLns(6) == PG_WAS_INC)
            {
            usCurrentPg = ItiPrtQueryCurrentPgNum();
            ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");

            /* continuation marker */
            ItiPrtPutTextAt (CURRENT_LN, 0, LEFT, 7, "(cont.)");

            ItiPrtPutTextAt (CURRENT_LN, 9,
                       LEFT, (acttl[RPT_TTL_1].usDtaColWth | REL_LEFT),
                       ppszPrc[STD_ITEM_NUM]);
            ItiPrtIncCurrentLn (); //line feed
            ItiPrtIncCurrentLn (); //blank line


            /* -- Reset the starting spacer line count. */
            usBreakLnCnt = 1;
         
            /* -- Set the column titles for new pg. */
            SetColumnTitles(RPT_TTL_12, RPT_TTL_15);
            }
         else
            usBreakLnCnt++;


         /* -- Write out the data. */
         if ( (ppszBestMatch != NULL)
            &&(ItiStrCmp(ppszBestMatch[4], ppszPrc[WRK_TYPE]) == 0)
            &&(ItiStrCmp(ppszBestMatch[2], ppszPrc[AREA_KEY]) == 0)   )
//            &&(ItiStrCmp(ppszBestMatch[3], ppszPrc[QTY_LEVEL]) == 0))
            {
            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_13].usTtlCol,
                             LEFT, (acttl[RPT_TTL_13].usTtlColWth | REL_LEFT),
                             acttl[RPT_TTL_13].szTitle);
            }

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usDtaCol,
                          LEFT, (acttl[RPT_TTL_12].usDtaColWth | REL_LEFT),
                          ppszPrc[QTY_LEVEL]);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_13].usDtaCol,
                          LEFT, (acttl[RPT_TTL_13].usDtaColWth | REL_LEFT),
                          ppszPrc[WRK_TYPE]);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_14].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_14].usDtaColWth | REL_LEFT),
                          ppszPrc[AREA]);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_15].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_15].usDtaColWth | REL_LEFT),
                          ppszPrc[WT_AVG_PRICE]);

         ItiPrtIncCurrentLn (); //line feed

         ItiFreeStrArray (hhpPrice, ppszPrc, usNumCols);


         if (usBreakLnCnt == BLANK_LINE_INTERVAL)
            {
            ItiPrtIncCurrentLn (); //line feed
            usBreakLnCnt = 0;
            }

         }/* end of while */

      }/* end of else clause, hqryPrc ==... */


   ItiPrtIncCurrentLn (); //blank line
   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   ItiPrtIncCurrentLn (); //line feed

   return (usErr);
   }/* End of Function PriceSectionFor */  





USHORT ItemsFor (PSZ pszJobKey, PSZ pszJobBrkdwnKey, PSZ pszJobBrkdwnID,
                 PSZ pszJobBrkdwnWorkType)
   {                      
   USHORT usLn, us, usCnt, usQuantityLevel;
   USHORT usNumCols, usErr;
   PSZ * ppszItems = NULL;
   HQRY  hqryItems;
   HHEAP hhprItems;



   if ((pszJobKey == NULL) || (*pszJobKey == '\0')
       || (pszJobBrkdwnKey == NULL) || (*pszJobBrkdwnKey == '\0'))
      return 13;


   hhprItems = ItiMemCreateHeap (MIN_HEAP_SIZE);


   /* -- Build the query. */
   ItiStrCpy(szQry, szQryJobItem,  sizeof szQry);
   ItiStrCat(szQry, pszJobKey,     sizeof szQry);
   ItiStrCat(szQry, szConjJobItem, sizeof szQry);
   ItiStrCat(szQry, pszJobBrkdwnKey, sizeof szQry);

   hqryItems = ItiDbExecQuery (szQry, hhprItems, hmodrJEAvg, ITIRID_RPT,
                          ID_QRY_JOB_ITEM, &usNumCols, &usErr);
   if (hqryItems == NULL)
      {
      SETQERRMSG("Query failed for PEMETHAvg. JobItem in a JobBreakdown.");
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
      while( ItiDbGetQueryRow (hqryItems, &ppszItems, &usErr) )  
         {
         usCnt++;
         us = DescriptionSection (pszJobBrkdwnID, ppszItems);

         if (us != ITIRPTUT_ERROR)
            {
            Percentiles (hhprItems,
                         pszJobKey,
                         ppszItems[JOB_ITEM_KEY],
                         ppszItems[JOB_ITEM_SIKEY],
                         szCurBaseDate,
                         ppszItems[JOB_ITEM_QTY],
                         &szCurAreaTypeKey[0],
                         &szCurItemImportDate[0],
                         &usQuantityLevel);

            AreaWorkInfo (hhprItems, pszJobKey,
                          szCurAreaTypeKey, pszJobBrkdwnWorkType);

            PriceSectionFor (hhprItems,
                             pszJobKey,
                             ppszItems[JOB_ITEM_SIKEY],
                             pszJobBrkdwnKey,
                             szCurBaseDate,
                             usQuantityLevel);
            }


         ItiFreeStrArray (hhprItems, ppszItems, usNumCols);
         }/* End of while loop */

      if (usCnt == 0)
         {
         SETQERRMSG("Empty Query Results for PEMETHAvg. JobItem in a JobBreakdown.");
         SETQERRMSG(szQry);
         
         usCurrentPg = ItiPrtQueryCurrentPgNum();
         ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");

         ItiPrtPutTextAt (CURRENT_LN + 3, 0,
                          CENTER, USE_RT_EDGE,
                          "-- Report not generated --");
         ItiPrtPutTextAt (CURRENT_LN + 5, 0,
                          CENTER, USE_RT_EDGE,
                          "No data found that fits search criteria.");

         usLn = ITIRPTUT_ERROR;
         }

      }/* End of else clause */


   ItiMemDestroyHeap (hhprItems);

   return us;
   }/* End of Function ItemsFor */






USHORT PrintProc (HWND hwnd, MPARAM mp1, MPARAM mp2)
   {
   USHORT us    = 0;
//   BOOL   bChecked = FALSE;
//   HFILE  hAltFile;
//   HHEAP  hhprLocal = NULL;
//   CHAR   szJobKey        [SMARRAY] = "";
//   CHAR   szOptSwStr   [SMARRAY] = "";
//   CHAR   szCmdLn      [2*SMARRAY +1] = "";
//   CHAR   szJobKeyFileName[CCHMAXPATH + 1] = "";
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
//                             Job_Cat_List,
//                             cJobKey,
//                             szJobKeyFileName, sizeof szJobKeyFileName);
//
//      if ((us == ITIRPTUT_PARM_ERROR)|| (us == ITIRPTUT_ERROR))
//         ItiRptUtErrorMsgBox (hwnd,
//                "ERROR: Failed to get selected keys in "__FILE__);
//      else
//         {/* -- Prep the CmdLn to use the key file. */
//         ItiStrCat (szCmdLn, ITIRPT_OPTSW_KEYFILE, sizeof szCmdLn);
//         ItiStrCat (szCmdLn, szJobKeyFileName, sizeof szCmdLn);
//         }/* end of if ((us... */
//
//      }/* end of if (bChecked... then clause */
//   else
//      {/* Do ALL of the keys. */
//      hhprLocal = ItiMemCreateHeap (MIN_HEAP_SIZE);
//
//      ItiRptUtUniqueFile (szJobKeyFileName, sizeof szJobKeyFileName, TRUE);
//
//      /* -- Build the cost sheet key list query. */
//      ItiMbQueryQueryText(hmodrJEAvg, ITIRID_RPT, ID_QRY_KEYS,
//                          szQryKeys, sizeof szQryKeys );
//
//
//      /* -- Do the database query. */
//      hqry = ItiDbExecQuery (szQryKeys, hhprLocal, hmodrJEAvg, ITIRID_RPT,
//                             ID_QRY_KEYS, &usNumCols, &usErr);
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
//         ItiRptUtStartKeyFile (&hAltFile, szJobKeyFileName, sizeof szJobKeyFileName);
//
//         while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )
//            {
//            ItiRptUtWriteToKeyFile (hAltFile, ppsz[JOB_KEY], NULL);
//            ItiFreeStrArray (hhprLocal, ppsz, usNumCols);
//            }/* End of while... */
//
//         ItiRptUtEndKeyFile (hAltFile);
//
//         ItiStrCat (szCmdLn, ITIRPT_OPTSW_KEYFILE, sizeof szCmdLn);
//         ItiStrCat (szCmdLn, szJobKeyFileName, sizeof szCmdLn);
//         }/* else clause */
//
//      }/* end of if (bChecked... else clause */
//
//
//   us = ItiRptUtExecRptCmdLn (hwnd, szCmdLn, RPT_RJEAVG_SESSION);

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
   ItiStrCpy (pszBuffer, "JobEst:(PEMETH Average)", usMaxSize);   
   *pusWindowID = rJEAvg_RptDlgBox;                       
   }/* END OF FUNCTION */




BOOL EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID, 
                                     USHORT usActiveWindowID) 
   {                                                          
   return FALSE;                                           
   }


MRESULT EXPORT ItiDllrJEAvgRptDlgProc (HWND     hwnd,     
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
         us = ItiRptUtInitDlgBox (hwnd, RPT_RJEAVG_SESSION, 0, NULL);
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
   PSZ     pszJobKey = szJobKey;

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
   hhprJEAvg = ItiMemCreateHeap (MIN_HEAP_SIZE);


   /* ------------------------------------------------------------------- *\
    * Initialize the strings.                                             *
   \* ------------------------------------------------------------------- */
   ItiStrCpy(szCurJobItemKey, " 0 ", sizeof(szCurJobItemKey));

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
   ItiRptUtLoadLabels (hmodrJEAvg, &usNumOfTitles, &acttl[0]);

   /* -- Set the report's titles for this DLL. */
   SetTitles(hhprJEAvg, prept, argcnt, argvars,
             &szCurBaseDate[0], &szCurCountyKey[0], &szCurCountyID[0]);



   


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



   ItiMbQueryQueryText(hmodrJEAvg, ITIRID_RPT, ID_QRY_TITLE_NAMES,
                       szQryTitleNames, sizeof szQryTitleNames );

   ItiMbQueryQueryText(hmodrJEAvg, ITIRID_RPT, ID_QRY_JOB_BRKDWN,
                       szQryJobBrkdwn, sizeof szQryJobBrkdwn );

   ItiMbQueryQueryText(hmodrJEAvg, ITIRID_RPT, ID_QRY_JOB_ITEM,
                       szQryJobItem, sizeof szQryJobItem );

   ItiMbQueryQueryText(hmodrJEAvg, ITIRID_RPT, ID_CONJ_JOB_ITEM,
                       szConjJobItem, sizeof szConjJobItem );

   ItiMbQueryQueryText(hmodrJEAvg, ITIRID_RPT, ID_QRY_PCTS,
                       szQryPcts, sizeof szQryPcts );

   ItiMbQueryQueryText(hmodrJEAvg, ITIRID_RPT, ID_CONJ_PCTS,
                       szConjPcts, sizeof szConjPcts );



   ItiMbQueryQueryText(hmodrJEAvg, ITIRID_RPT, ID_QRY_AREA_INFO,
                       szQryAreaInfo, sizeof szQryAreaInfo );

   ItiMbQueryQueryText(hmodrJEAvg, ITIRID_RPT, ID_QRY_AREA_TYPE_INFO,
                       szQryAreaTypeInfo, sizeof szQryAreaTypeInfo );

   ItiMbQueryQueryText(hmodrJEAvg, ITIRID_RPT, ID_CONJ_AREA_INFO_BDK,
                       szConjAreaInfoBDK, sizeof szConjAreaInfoBDK );

   ItiMbQueryQueryText(hmodrJEAvg, ITIRID_RPT, ID_CONJ_AREA_INFO_SIK,
                       szConjAreaInfoSIK, sizeof szConjAreaInfoSIK );

   ItiMbQueryQueryText(hmodrJEAvg, ITIRID_RPT, ID_QRY_WORK_TYPE_INFO,
                       szQryWorkTypeInfo, sizeof szQryWorkTypeInfo );



   ItiMbQueryQueryText(hmodrJEAvg, ITIRID_RPT, ID_QRY_PRICES,
                       szQryPrices, sizeof szQryPrices);

   ItiMbQueryQueryText(hmodrJEAvg, ITIRID_RPT, ID_CONJ_PRICES_JOB_KEY,
                       szCnjPricesJobKey, sizeof szCnjPricesJobKey);
                     
   ItiMbQueryQueryText(hmodrJEAvg, ITIRID_RPT, ID_CONJ_PRICES_JOB_BD_KEY,
                       szCnjPricesJobBDKey, sizeof szCnjPricesJobBDKey);
                     
   ItiMbQueryQueryText(hmodrJEAvg, ITIRID_RPT, ID_QRY_PRICES_BASEDATE,
                       szCnjPricesBaseDate, sizeof szCnjPricesBaseDate);

   ItiMbQueryQueryText(hmodrJEAvg, ITIRID_RPT, ID_QRY_PRICES_ORDER,
                       szPriceOrder, sizeof szPriceOrder);





   /* -------------------------------------------------------------------- *\
    * -- For the one JobKey...                                             *
   \* -------------------------------------------------------------------- */
   ItiRptUtInitKeyList(argcnt, argvars); /*  */
   while (ItiRptUtGetNextKey(pszJobKey,sizeof(szJobKey)) != ITIRPTUT_NO_MORE_KEYS)
      {
      /* -- Get JobBreakdowns */
      ItiStrCpy(szQryBDKeys, szQryJobBrkdwn, sizeof szQry);
      ItiStrCat(szQryBDKeys, pszJobKey, sizeof szQry);

      usCnt_appsz = ItiDbGetQueryCount(szQryBDKeys, &usNumColsBDKeys, &usErr);

      /* -- Do the database query. */
      hqryBDKeys = ItiDbExecQuery (szQryBDKeys, hhprJEAvg, hmodrJEAvg, ITIRID_RPT,
                                   ID_QRY_JOB_BRKDWN, &usNumColsBDKeys, &usErr);
      if (hqryBDKeys == NULL)
         {
         SETQERRMSG(szQryBDKeys);
         break;
         }                     
      else /* -- Make a list of the breakdown ids, keys, work-types. */
         {
         usIndex_appsz = 0;  
         while( ItiDbGetQueryRow (hqryBDKeys, &ppszBDKeys, &usErr) )
            {
            appsz[usIndex_appsz] = ItiStrMakePPSZ (hhprJEAvg, 3,
                                            ppszBDKeys[JOB_BRKDWN_KEY],
                                            ppszBDKeys[JOB_BRKDWN_ID],
                                            ppszBDKeys[JOB_BRKDWN_WORK_TYPE]);

            ItiFreeStrArray (hhprJEAvg, ppszBDKeys, usNumColsBDKeys);
            usIndex_appsz++;
            }/* end while */


         /* -- Now, with each breakdown in the list... */
         usBKindex = 0;
         while (usBKindex < usIndex_appsz)
            {
            ItemsFor (pszJobKey,
                      appsz[usBKindex][JOB_BRKDWN_KEY],
                      appsz[usBKindex][JOB_BRKDWN_ID],
                      appsz[usBKindex][JOB_BRKDWN_WORK_TYPE]);

            /* -- Increment the index to the next breakdown. */
            usBKindex++;
            }/* end while usBKindex */

         }/* end else */

      }/* End of WHILE GetNextKey */



   /* ------------------------------------------------------------------- *\
    * Finish up and quit.                                                 *
   \* ------------------------------------------------------------------- */
   ItiPrtEndReport();

   DBGMSG("Exit ***DLL function ItiRptDLLPrintReport");
   return 0;

   }/* END OF FUNCTION ItiRptDLLPrintReport */





