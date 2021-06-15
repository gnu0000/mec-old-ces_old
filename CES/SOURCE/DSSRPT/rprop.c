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


#include    "rindllal.h"


#include "rProp.h"                                       



/* ======================================================================= *\
 *                                         MANIFEST CONSTANT DEFINITIONS   *
\* ======================================================================= */
#define  BLANK_LINE_INTERVAL    4

#define  PRT_Q_NAME             "Proposal List"

#define  QUERY_ARRAY_SIZE       1400

#define  SIZE_OF_QUERY_RPT_DESC QUERY_ARRAY_SIZE


#define  RANK        1
#define  MAJR        2
#define  ITEM        3


#define  EXEC_ERR      ItiStrCpy (szErrorMsg,                          \
                                  "ERROR: Query failed for ",          \
                                  sizeof szErrorMsg);                  \
            ItiStrCat (szErrorMsg, TITLE, sizeof szErrorMsg);          \
            ItiStrCat (szErrorMsg,", Error Code: ",sizeof szErrorMsg); \
            ItiStrUSHORTToString                                       \
               (usErr, &szErrorMsg[ItiStrLen(szErrorMsg)],             \
                sizeof szErrorMsg);                                    \
            ItiStrCat (szErrorMsg,".  Query: ", sizeof szErrorMsg);    \
            ItiStrCat (szErrorMsg, szQuery, sizeof szErrorMsg);        \
            ItiRptUtErrorMsgBox (NULL, szErrorMsg)                     \





/* ======================================================================= *\
 *                                             MODULE'S GLOBAL VARIABLES   *
\* ======================================================================= */
static  CHAR  szCompileDateTime[] = __DATE__ " " __TIME__ ;
static CHAR szDllVersion[] = "1.1a0 rprop.dll";

static  CHAR  szID   [SMARRAY + 1] = "";
static  CHAR  szOrderingStr [SMARRAY + 1] = "";
static  CHAR  szFlagStr     [8] = "";
static  CHAR  szProposalKey [SMARRAY + 1] = "";

static  CHAR  szFailedDLL [CCHMAXPATH + 1] = "";
static  CHAR  szErrorMsg  [1023] = "";

static  USHORT usNumOfTitles;
static  USHORT usCurrentPg = 0;



           /* Used by DbExecQuer & DbGetQueryRow functions: */
static  HHEAP   hhprProp  = NULL;
static  HQRY    hqry   = NULL;
static  USHORT  usResId   = ITIRID_RPT;
static  USHORT  usNumCols = 0;
static  USHORT  usErr     = 0;
static  PSZ   * ppsz   = NULL;

           /* Title strings for this module. */
static  REPTITLES rept;
static  PREPT     prept = &rept;

static  INT   iDesc = ITM_LONG_DESC;
           /* default to use long descriptions. */

static  BOOL  bProposalAwarded = FALSE;


static  CHAR  szBaseQuery    [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szQuery        [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szKey          [SMARRAY + 1] = "";

static  CHAR  szQueryBidKeys [SIZE_OF_QUERY_BIDKEYS + 1] = "";

static  CHAR  szQueryListing [SIZE_OF_QUERY_LISTING + 1] = "";

static  CHAR  szQueryListingProjs [SIZE_OF_QUERY_LISTING + 1] = "";

static  CHAR  szQueryRank      [SIZE_OF_QUERY_RANK + 1] = "";
static  CHAR  szQueryRankOrder [SIZE_OF_CONJ + 1] = "";

static  CHAR  szQueryMjr       [SIZE_OF_QUERY_MJR + 1] = "";
static  CHAR  szQueryMjrOrder  [SIZE_OF_CONJ + 1] = "";
static  CHAR  szQueryMjrTtl    [SIZE_OF_QUERY_MJR_TTL + 1] = "";
static  CHAR  szQueryMjrTtlPct [SIZE_OF_QUERY_MJR_TTL_PCT + 1] = "";




//static  CHAR  szQueryCat     [SIZE_OF_QUERY_CAT + 1] = "";
//static  CHAR  szQueryCatConj [SIZE_OF_CONJ + 1] = "";


static  CHAR  szQueryPItmKeys [SIZE_OF_QUERY_PITM_KEYS + 1] = "";

static  CHAR  szQueryItems      [SIZE_OF_QUERY_ITEMS + 1] = "";
static  CHAR  szQueryItemsConj  [SIZE_OF_CONJ + 1] = "";
static  CHAR  szQueryItemsOrder [SIZE_OF_CONJ + 1] = "";





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







USHORT SetColumnTitles (USHORT usSubSectionID, PSZ pszID)
   /* -- Print the item list column titles. */
   {
   USHORT us;
   CHAR szTxt[SMARRAY] = "Proposal ";


   ItiStrCat(szTxt, pszID, sizeof szTxt);
   ItiPrtPutTextAt (CURRENT_LN, 0,
                    CENTER, USE_RT_EDGE,
                    szTxt);
   ItiPrtIncCurrentLn (); //line feed


   switch (usSubSectionID)
      {
      case RANK:
         for (us = RPT_TTL_35; us <= RPT_TTL_39; us++)
            {
            ItiPrtPutTextAt (CURRENT_LN, acttl[us].usTtlCol,
                             LEFT, (acttl[us].usTtlColWth | REL_LEFT),
                             acttl[us].szTitle);
            }
         ItiPrtIncCurrentLn (); //line feed

         for (us = RPT_TTL_40; us <= RPT_TTL_42; us++)
            {
            ItiPrtPutTextAt (CURRENT_LN, acttl[us].usTtlCol,
                             LEFT, (acttl[us].usTtlColWth | REL_LEFT),
                             acttl[us].szTitle);
            }
         break;


      case MAJR:
         for (us = RPT_TTL_43; us <= RPT_TTL_51; us++)
            {
            ItiPrtPutTextAt (CURRENT_LN, acttl[us].usTtlCol,
                             LEFT, (acttl[us].usTtlColWth | REL_LEFT),
                             acttl[us].szTitle);
            }
         ItiPrtIncCurrentLn (); //line feed

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_52].usTtlCol,
                          LEFT, (acttl[RPT_TTL_52].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_52].szTitle);
         break;


      case ITEM:
         for (us = RPT_TTL_53; us <= RPT_TTL_57; us++)
            {
            ItiPrtPutTextAt (CURRENT_LN, acttl[us].usTtlCol,
                             LEFT, (acttl[us].usTtlColWth | REL_LEFT),
                             acttl[us].szTitle);
            }
         ItiPrtIncCurrentLn (); //line feed

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_58].usTtlCol,
                          LEFT, (acttl[RPT_TTL_58].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_58].szTitle);
         break;


      default:
         break;
      }/* end of switch */


   ItiPrtIncCurrentLn (); //line feed
   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   ItiPrtIncCurrentLn (); //line feed

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





USHORT SummarySectionFor (PSZ pszKey, PSZ pszID)
   {
   USHORT us;
   USHORT usNumColsLoc = 0;
   PSZ  * ppsz = NULL;
   PSZ  * ppszLoc = NULL;
   BOOL   bAwarded = FALSE;
   BOOL   bLowBid  = FALSE;
   BOOL   bAwardToLowBid = FALSE;
   HQRY   hqryLoc  = NULL;
   CHAR   szTmp[36] = "";


   if ((pszKey == NULL) || (*pszKey == '\0'))
      return 13;



   /* -- Build the bid keys query. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryBidKeys, " ", pszKey);

   ppsz = ItiDbGetRow1 (szQuery, hhprProp, hmodrProp, ITIRID_RPT,
                        ID_QUERY_BIDKEYS, &usNumCols);

   if (ppsz != NULL)
      {
      if ((ppsz[AWARDED_KEY] != NULL) && (ppsz[LOWBID_KEY] != NULL))
         {
         if (ItiStrCmp(ppsz[LOWBID_KEY], ppsz[AWARDED_KEY]) == 0)
            {
            bAwardToLowBid = TRUE;
            }
         else
            {
            bAwarded = TRUE;
            bLowBid  = TRUE;
            }
         }
      else
         {
         if (ppsz[AWARDED_KEY] != NULL)
            bAwarded = TRUE;

         if (ppsz[LOWBID_KEY] != NULL)
            bLowBid  = TRUE;
         }

      /* -- We're now done with this query, so... */
      ItiFreeStrArray (hhprProp, ppsz, usNumCols);
      }
   else
      {
      ItiStrCpy(szErrorMsg,
                "RPT ERROR: Failed to get Bid keys for ProposalKey: ",
                sizeof szErrorMsg);
      ItiStrCat(szErrorMsg, pszKey, sizeof szErrorMsg);

      ItiRptUtErrorMsgBox(NULL, szErrorMsg);
      }







   /* -- Build the query. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryListing, " ", pszKey);


   ppsz = ItiDbGetRow1 (szQuery, hhprProp, hmodrProp, ITIRID_RPT,
                        ID_QUERY_LISTING, &usNumCols);

   if (ppsz != NULL)
      {
      if (ItiPrtQueryCurrentPgNum() > 1 )
         {
         ItiPrtNewPg();
         /* -- This section is large enough to always start on a new page. */
         }
      usCurrentPg = ItiPrtQueryCurrentPgNum();
      ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");



   /* -- Proposal: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_1].usTtlCol,
                       LEFT, (acttl[RPT_TTL_1].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_1].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_1].usDtaCol,
                       LEFT, (acttl[RPT_TTL_1].usDtaColWth | REL_LEFT),
                       ppsz[PROPID]);
      ItiStrCpy(pszID, ppsz[PROPID], sizeof szID);


   /* -- Project Number: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_2].usTtlCol,
                       LEFT, (acttl[RPT_TTL_2].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_2].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_2].usDtaCol,
                       LEFT, (acttl[RPT_TTL_2].usDtaColWth | REL_LEFT),
                       ppsz[PROJ_NUM]);
      ItiPrtIncCurrentLn();


   /* -- Description: */
      ItiPrtPutTextAt (CURRENT_LN, (acttl[RPT_TTL_3].usDtaCol),
                       (LEFT|WRAP),
                       //(2 | FROM_RT_EDGE),
                       (acttl[RPT_TTL_3].usDtaColWth | REL_LEFT),
                       ppsz[DESC]);
      ItiPrtIncCurrentLn();


   /* -- BAMS Project list: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_4].usTtlCol,
                       LEFT, (acttl[RPT_TTL_4].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_4].szTitle);


   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryListingProjs, " ", pszKey);

   /* -- Do the database query. */
   hqryLoc = ItiDbExecQuery (szQuery, hhprProp, hmodrProp, ITIRID_RPT,
                             ID_QUERY_LISTING_PROJS, &usNumColsLoc, &usErr);
   if (hqryLoc == NULL)
      {
      EXEC_ERR;
      }
   else
      {
      while (ItiDbGetQueryRow(hqryLoc, &ppszLoc, &usErr) )
         {
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_4].usDtaCol,
                          LEFT, (acttl[RPT_TTL_4].usDtaColWth | REL_LEFT),
                          ppszLoc[PROJ_CTRL_NUM]);
         ItiPrtIncCurrentLn();

         ItiFreeStrArray (hhprProp, ppszLoc, usNumColsLoc);
         }
      }

      ItiPrtIncCurrentLn();


   /* -- Location: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_5].usTtlCol,
                       LEFT, (acttl[RPT_TTL_5].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_5].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_5].usDtaCol,
                       LEFT, (acttl[RPT_TTL_5].usDtaColWth | REL_LEFT),
                       ppsz[LOC]);
      ItiPrtIncCurrentLn();

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_6].usDtaCol,
                       (LEFT|WRAP),
                       //(2 | FROM_RT_EDGE),
                       (acttl[RPT_TTL_3].usDtaColWth | REL_LEFT),
                       ppsz[LOC_DESC]);

      ItiPrtIncCurrentLn();


   /* -- District: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_7].usTtlCol,
                       LEFT, (acttl[RPT_TTL_7].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_7].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_7].usDtaCol,
                       LEFT, (acttl[RPT_TTL_7].usDtaColWth | REL_LEFT),
                       ppsz[DIST]);
      ItiPrtIncCurrentLn();



   /* -- County: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_8].usTtlCol,
                       LEFT, (acttl[RPT_TTL_8].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_8].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_8].usDtaCol,
                       LEFT, (acttl[RPT_TTL_8].usDtaColWth | REL_LEFT),
                       ppsz[COUNTY_ID]);

      ItiPrtPutTextAt (CURRENT_LN,  CURRENT_COL+1,
                       LEFT, (acttl[RPT_TTL_9].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_9].szTitle);

      ItiPrtPutTextAt (CURRENT_LN,  CURRENT_COL+1,
                       LEFT, (acttl[RPT_TTL_9].usDtaColWth | REL_LEFT),
                       ppsz[COUNTY_NAME]);
      ItiPrtIncCurrentLn();
      ItiPrtIncCurrentLn(); //blank line



   /* -- Status: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_10].usTtlCol,
                       LEFT, (acttl[RPT_TTL_10].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_10].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_10].usDtaCol,
                       LEFT, (acttl[RPT_TTL_10].usDtaColWth | REL_LEFT),
                       ppsz[STATUS_ID]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                       LEFT, (acttl[RPT_TTL_11].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_11].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                       LEFT, (acttl[RPT_TTL_11].usDtaColWth | REL_LEFT),
                       ppsz[STATUS_TRANS]);
      ItiPrtIncCurrentLn();



   /* -- Pct% complete: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usDtaCol,
                       LEFT, (acttl[RPT_TTL_12].usDtaColWth | REL_LEFT),
                       ppsz[PERCENT]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+2,
                       LEFT, (acttl[RPT_TTL_13].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_13].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                       LEFT, (acttl[RPT_TTL_13].usDtaColWth | REL_LEFT),
                       ppsz[IMP_DATE]);
      ItiPrtIncCurrentLn();


   /* -- Work completed, and it is not blank: */
      if (ItiStrLen(ppsz[COMP_DATE]) > 5)
         {
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_14].usTtlCol,
                          LEFT, (acttl[RPT_TTL_14].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_14].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_14].usDtaCol,
                          LEFT, (acttl[RPT_TTL_14].usDtaColWth | REL_LEFT),
                          ppsz[COMP_DATE]);
         ItiPrtIncCurrentLn();
         }

      ItiPrtIncCurrentLn(); // blank line


   /* -- Work Type: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_15].usTtlCol,
                       LEFT, (acttl[RPT_TTL_15].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_15].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_15].usDtaCol,
                       LEFT, (acttl[RPT_TTL_15].usDtaColWth | REL_LEFT),
                       ppsz[WRK_TYP]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                       LEFT, (acttl[RPT_TTL_16].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_16].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                       LEFT, (acttl[RPT_TTL_16].usDtaColWth | REL_LEFT),
                       ppsz[WRK_TRANS]);
      ItiPrtIncCurrentLn();



   /* -- Road Type: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_17].usTtlCol,
                       LEFT, (acttl[RPT_TTL_17].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_17].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_17].usDtaCol,
                       LEFT, (acttl[RPT_TTL_17].usDtaColWth | REL_LEFT),
                       ppsz[ROAD_TYP]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                       LEFT, (acttl[RPT_TTL_16].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_16].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                       LEFT, (acttl[RPT_TTL_16].usDtaColWth | REL_LEFT),
                       ppsz[ROAD_TRANS]);
      ItiPrtIncCurrentLn();


   /* -- Spec Year: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_18].usTtlCol,
                       LEFT, (acttl[RPT_TTL_18].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_18].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_18].usDtaCol,
                       LEFT, (acttl[RPT_TTL_18].usDtaColWth | REL_LEFT),
                       ppsz[SPEC_YR]);
      ItiPrtIncCurrentLn();
      ItiPrtIncCurrentLn(); //blank line




   /* -- Proposal Length: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_19].usTtlCol,
                       LEFT, (acttl[RPT_TTL_19].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_19].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_19].usDtaCol,
                       LEFT, (acttl[RPT_TTL_19].usDtaColWth | REL_LEFT),
                       ppsz[LENGTH]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                       LEFT, (acttl[RPT_TTL_22].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_22].szTitle);
      ItiPrtIncCurrentLn();




   /* -- Avg Pvmt Width: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_20].usTtlCol,
                       LEFT, (acttl[RPT_TTL_20].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_20].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_20].usDtaCol,
                       LEFT, (acttl[RPT_TTL_20].usDtaColWth | REL_LEFT),
                       ppsz[WIDTH]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                       LEFT, (acttl[RPT_TTL_23].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_23].szTitle);
      ItiPrtIncCurrentLn();




   /* -- Avg Pvmt Depth: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_21].usTtlCol,
                       LEFT, (acttl[RPT_TTL_21].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_21].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_21].usDtaCol,
                       LEFT, (acttl[RPT_TTL_21].usDtaColWth | REL_LEFT),
                       ppsz[DEPTH]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                       LEFT, (acttl[RPT_TTL_24].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_24].szTitle);
      ItiPrtIncCurrentLn();
      ItiPrtIncCurrentLn(); //blank line




   /* -- Letting Date: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_25].usTtlCol,
                       LEFT, (acttl[RPT_TTL_25].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_25].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_25].usDtaCol,
                       LEFT, (acttl[RPT_TTL_25].usDtaColWth | REL_LEFT),
                       ppsz[LETT_DATE]);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_26].usTtlCol,
                       LEFT, (acttl[RPT_TTL_26].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_26].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_26].usDtaCol,
                       LEFT, (acttl[RPT_TTL_26].usDtaColWth | REL_LEFT),
                       ppsz[BID_CNT]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+2,
                       LEFT, (acttl[RPT_TTL_27].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_27].szTitle);
      ItiPrtIncCurrentLn();



   /* -- Rejected info line: */
      if (ppsz[REJ_FLAG][0] == 'T')
         {
         bProposalAwarded = FALSE;
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_28].usTtlCol,
                          LEFT, (acttl[RPT_TTL_28].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_28].szTitle);

         ItiStrCpy(szTmp, ppsz[REJ_ID], sizeof szTmp);
         ItiStrCat(szTmp, " - ", sizeof szTmp);
         ItiStrCat(szTmp, ppsz[REJ_TRANS], (sizeof(szTmp) - 2) );
         ItiStrCat(szTmp, ")", sizeof szTmp);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_28].usDtaCol,
                          LEFT, USE_RT_EDGE,
                          szTmp);
         ItiPrtIncCurrentLn();
         }
      else
         bProposalAwarded = TRUE;


      ItiPrtIncCurrentLn();





      /* -- Now do the Project Amounts subsection: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_30].usTtlCol,
                       LEFT, (acttl[RPT_TTL_30].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_30].szTitle);

      /* Determine column placement based on the longest amount string. */
      if (ItiStrLen(ppsz[AWD_TTL]) >= ItiStrLen(ppsz[LOW_TTL]) )
         us = ItiStrLen(ppsz[AWD_TTL]);
      else
         us = ItiStrLen(ppsz[LOW_TTL]);

      if (ItiStrLen(ppsz[EST_TTL]) >= us)
         us = ItiStrLen(ppsz[EST_TTL]);

      us = acttl[RPT_TTL_30].usDtaCol + us;



      if (bAwardToLowBid)
         {
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_34].usDtaCol,
//                          RIGHT, (acttl[RPT_TTL_34].usDtaColWth | REL_LEFT),
                          RIGHT, us,
                          ppsz[AWD_TTL]);
//         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_34].usTtlCol,
         ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                          LEFT, (acttl[RPT_TTL_34].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_34].szTitle);
         ItiPrtIncCurrentLn();
         }
      else
         {
         if (bAwarded) 
            {      /* Awarded */
            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_30].usDtaCol,
                             RIGHT, us,
                             ppsz[AWD_TTL]);
            ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                             LEFT, (acttl[RPT_TTL_31].usTtlColWth | REL_LEFT),
                             acttl[RPT_TTL_31].szTitle);
            ItiPrtIncCurrentLn();
            }

         if (bLowBid)
            {        /* Low Bidder */
            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_30].usDtaCol,
                             RIGHT, us,
                             ppsz[LOW_TTL]);
            ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                             LEFT, (acttl[RPT_TTL_32].usTtlColWth | REL_LEFT),
                             acttl[RPT_TTL_32].szTitle);
            ItiPrtIncCurrentLn();
            }
         }/* end else clause  if (bAwardToLowBid... */





      /* Estimated */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_30].usDtaCol,
                       RIGHT, us,
                       ppsz[EST_TTL]);
      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                       LEFT, (acttl[RPT_TTL_33].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_33].szTitle);
      ItiPrtIncCurrentLn();


      ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
      ItiPrtIncCurrentLn (); //line feed


      /* -- We're now done with this query, so... */
      ItiFreeStrArray (hhprProp, ppsz, usNumCols);

      }/* end if (ppsz !=... / _LISTING */
   else
      return 13;

   return 0;
   }/* End of Function SummarySectionFor */




USHORT RankSection (PSZ pszRankKey, PSZ pszID)
   {
   BOOL   bFirst = TRUE;
   BOOL   bHaveData = FALSE;
   BOOL   bEngEstShown = FALSE;
   PSZ  * ppszLoc = NULL;
   LONG   lBidTotalMinusEngEst = 0L;
   USHORT usLn, usTmp, usRank, usRankCnt;
   CHAR   szRank[16] = "";
   CHAR   szTmp[16]  = "";


   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryRank, pszRankKey, szQueryRankOrder);

   usRankCnt = ItiDbGetQueryCount(szQuery, &usNumCols, &usErr);

   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hhprProp, hmodrProp, ITIRID_RPT,
                          ID_QUERY_RANK, &usNumCols, &usErr);
   if (hqry == NULL)
      {
      EXEC_ERR;
      }
   else
      {
      usLn = 7;
      usRank = 1;
      while (ItiDbGetQueryRow(hqry, &ppszLoc, &usErr) )
         {
         /* -- Check for new page. */
         if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
            {
            usCurrentPg = ItiPrtQueryCurrentPgNum();
            SetColumnTitles(RANK, pszID);  
            }
         else if (bFirst)
            {
            SetColumnTitles(RANK, pszID);
            usLn = 3;
            bFirst = FALSE;
            bHaveData = TRUE;
            }

         szTmp[0] = '\0';
         ItiStrUSHORTToString(usRank, szRank, sizeof szRank);

         /* -- Check if the current row's "Bid Total" is greater *\
          * -- than the Engineer's Estimate; ifso then writeout  *
         \* -- the EngEst before writing the current row.        */
         ItiStrToLONG (ppszLoc[RANK_BTEE], &lBidTotalMinusEngEst);
         if ((lBidTotalMinusEngEst >= 0) && (!bEngEstShown))
            {
            bEngEstShown = TRUE;
            usTmp = usRank - 1;
            ItiStrUSHORTToString(usTmp, szTmp, sizeof szTmp);
            ItiStrCat(szTmp, ".5 ", sizeof szTmp);

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_35].usDtaCol + 1,
                       CENTER, (acttl[RPT_TTL_35].usDtaColWth | REL_LEFT),
                       szTmp);

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_36].usDtaCol,
                       LEFT, (acttl[RPT_TTL_36].usDtaColWth | REL_LEFT),
                       "Engineer's Estimate");

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_37].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_37].usDtaColWth | REL_LEFT),
                       ppszLoc[RANK_ENG_EST]);

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_38].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_38].usDtaColWth | REL_LEFT),
                       "100.0%");

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_39].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_39].usDtaColWth | REL_LEFT),
                       ppszLoc[RANK_ENGLOW]);
            ItiPrtIncCurrentLn();

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_35].usDtaCol,
                       LEFT, (acttl[RPT_TTL_35].usDtaColWth | REL_LEFT),
                       "Est");

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_40].usDtaCol,
                       LEFT, (acttl[RPT_TTL_40].usDtaColWth | REL_LEFT),
                       "This is the estimated proposal cost.");
            ItiPrtIncCurrentLn();
            }



         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_35].usDtaCol + 1,
                    LEFT, (acttl[RPT_TTL_35].usDtaColWth | REL_LEFT),
                    szRank);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_36].usDtaCol,
                    LEFT, (acttl[RPT_TTL_36].usDtaColWth | REL_LEFT),
                    ppszLoc[RANK_V_ID]);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_37].usDtaCol,
                    RIGHT, (acttl[RPT_TTL_37].usDtaColWth | REL_LEFT),
                    ppszLoc[RANK_BID_TTL]);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_38].usDtaCol,
                    RIGHT, (acttl[RPT_TTL_38].usDtaColWth | REL_LEFT),
                    ppszLoc[RANK_ENG_PCT]);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_39].usDtaCol,
                    RIGHT, (acttl[RPT_TTL_39].usDtaColWth | REL_LEFT),
                    ppszLoc[RANK_LOW_PCT]);
         ItiPrtIncCurrentLn();

         if (ppszLoc[RANK_AWDD][0] == '0')
            {
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_35].usDtaCol,
                    LEFT, (acttl[RPT_TTL_35].usDtaColWth | REL_LEFT),
                    "Awd");
            }

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_40].usDtaCol,
                    LEFT, (acttl[RPT_TTL_40].usDtaColWth | REL_LEFT),
                    ppszLoc[RANK_NAME]);
         ItiPrtIncCurrentLn();




         /* -- If we are on the LAST row and still have not *\
         \* -- written the EngEst then do so now.           */
         if ((usRank == usRankCnt) && (!bEngEstShown))
            {
            /* NOTE: This code block is(had better be!) identical to *
             *  the code block for the Engineer's Estimate above.    */
            bEngEstShown = TRUE;
            usTmp = usRank - 1;
            ItiStrUSHORTToString(usTmp, szTmp, sizeof szTmp);
            ItiStrCat(szTmp, ".5 ", sizeof szTmp);

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_35].usDtaCol + 1,
                       RIGHT, (acttl[RPT_TTL_35].usDtaColWth | REL_LEFT),
                       szTmp);

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_36].usDtaCol,
                       LEFT, (acttl[RPT_TTL_36].usDtaColWth | REL_LEFT),
                       "Engineer's Estimate");

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_37].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_37].usDtaColWth | REL_LEFT),
                       ppszLoc[RANK_ENG_EST]);

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_38].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_38].usDtaColWth | REL_LEFT),
                       "100.0%");

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_39].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_39].usDtaColWth | REL_LEFT),
                       ppszLoc[RANK_ENGLOW]);
            ItiPrtIncCurrentLn();

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_35].usDtaCol,
                       LEFT, (acttl[RPT_TTL_35].usDtaColWth | REL_LEFT),
                       "Est");

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_40].usDtaCol,
                       LEFT, (acttl[RPT_TTL_40].usDtaColWth | REL_LEFT),
                       "This is the estimated proposal cost.");
            ItiPrtIncCurrentLn();
            }

//         ItiPrtIncCurrentLn (); //blank line 

         ItiFreeStrArray (hhprProp, ppszLoc, usNumCols);

         /* -- Increment the new current Ranking for the next row. */
         usRank++;

         }/* end of while (ItiDbGetQuery... */

      }/* end if (hqry... */

   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   ItiPrtIncCurrentLn (); //line feed

   return 0;
   }/* END OF FUNCTION RankSection */




MajorItemSection (PSZ pszProposalKey, PSZ pszID)
   {
   USHORT usLen;
   BOOL   bFirst = TRUE;
   BOOL   bHaveData = FALSE;
   PSZ  * ppszLst = NULL;
   CHAR   szTtl[80] = "Total: ";


   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryMjr, pszProposalKey, szQueryMjrOrder);


   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hhprProp, hmodrProp, ITIRID_RPT,
                          ID_QUERY_MJR, &usNumCols, &usErr);
   if (hqry == NULL)
      {
      EXEC_ERR;
      }
   else
      {
      while (ItiDbGetQueryRow(hqry, &ppszLst, &usErr) )
         {
         /* -- Check for new page. */
         if (ItiPrtKeepNLns(7) == PG_WAS_INC)
            {
            usCurrentPg = ItiPrtQueryCurrentPgNum();
            SetColumnTitles(MAJR, pszID);  
            bFirst = FALSE;
            bHaveData = TRUE;
            }
         else if (bFirst)
            {
            SetColumnTitles(MAJR, pszID);  
            bFirst = FALSE;
            bHaveData = TRUE;
            }

         /* -- List the major items: */
         /* -- Item number: */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_43].usDtaCol,
                       LEFT, (acttl[RPT_TTL_43].usDtaColWth | REL_LEFT),
                       ppszLst[MJR_ID]);

         /* -- Quantity: */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_44].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_44].usDtaColWth | REL_LEFT),
                       ppszLst[MJR_QTY]);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_45].usTtlCol,
                       LEFT, (2 | REL_LEFT),
                       acttl[RPT_TTL_45].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_46].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_46].usDtaColWth | REL_LEFT),
                       ppszLst[MJR_PRC]);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_47].usTtlCol,
                       LEFT, (acttl[RPT_TTL_47].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_47].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_48].usDtaCol,
                       LEFT, (acttl[RPT_TTL_48].usDtaColWth | REL_LEFT),
                       ppszLst[MJR_UNIT]);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_49].usTtlCol,
                       LEFT, (acttl[RPT_TTL_49].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_49].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_50].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_50].usDtaColWth | REL_LEFT),
                       ppszLst[MJR_EXT]);

         if (bProposalAwarded)
            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_51].usDtaCol,
                             RIGHT, (acttl[RPT_TTL_51].usDtaColWth | REL_LEFT),
                             ppszLst[MJR_AWD_PCT]);
         else
            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_51].usDtaCol,
                             RIGHT, (acttl[RPT_TTL_51].usDtaColWth | REL_LEFT),
                             ppszLst[MJR_REJ_PCT]);

         ItiPrtIncCurrentLn (); //line feed

         /* -- Do the Description: */
         ItiPrtPutTextAt (CURRENT_LN, (acttl[RPT_TTL_52].usDtaCol),
                          (LEFT | WRAP),
                          (acttl[RPT_TTL_52].usDtaColWth | REL_LEFT),
                          ppszLst[MJR_DESC]);

//         ItiPrtIncCurrentLn(); //blank line

         }/* end while... */

		//BSR#921018-4102 Omit Total section if no major items in list.
		if (bHaveData == FALSE)
			return 0;

      /* -- Now do the "Total:" line. */
      ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                        szQueryMjrTtl, " ", pszProposalKey);

      ppszLst = ItiDbGetRow1 (szQuery, hhprProp, hmodrProp, ITIRID_RPT,
                           ID_QUERY_MJR_TTL, &usNumCols);
      if (ppszLst != NULL)
         {
         ItiStrCat(szTtl, ppszLst[MJR_TTL], sizeof szTtl);
         }
      else
         {
         ItiStrCat(szTtl, " $*.** ", sizeof szTtl);
         }


      /* Prep for separator line above the total. */
      usLen = ItiStrLen(szTtl) - 5;


      ItiStrCat(szTtl, " ", sizeof szTtl);

      ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                        szQueryMjrTtlPct, " ", pszProposalKey);

      ppszLst = ItiDbGetRow1 (szQuery, hhprProp, hmodrProp, ITIRID_RPT,
                           ID_QUERY_MJR_TTL_PCT, &usNumCols);
      if (ppszLst != NULL)
         {
         if (bProposalAwarded)
            ItiStrCat(szTtl, ppszLst[MJR_TTL_PCT_AWD], sizeof szTtl);
         else
            ItiStrCat(szTtl, ppszLst[MJR_TTL_PCT_REJ], sizeof szTtl);
         }
      else
         {
         ItiStrCat(szTtl, " *.*%", sizeof szTtl);
         }

      /* -- Figure out where to draw separator lines. */
      ItiPrtDrawSeparatorLn (CURRENT_LN,
                             acttl[RPT_TTL_51].usDtaCol - usLen,
                             acttl[RPT_TTL_51].usDtaCol - 2);

      ItiPrtDrawSeparatorLn (CURRENT_LN,
                             acttl[RPT_TTL_51].usDtaCol, USE_RT_EDGE);
      ItiPrtIncCurrentLn (); //line feed

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_48].usDtaCol,
                 RIGHT, USE_RT_EDGE,
                 szTtl);

      ItiPrtIncCurrentLn (); //line feed
      ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
      ItiPrtIncCurrentLn (); //line feed

      }/* end else... */


   return 0;
   }



USHORT PropoItemList (PSZ pszProposalKey, PSZ pszID)
/* -- This function design requires at least TWO database connections. */
   {
   USHORT usLn = 6;
   USHORT usR;
   USHORT usL;
   BOOL   bFirst = TRUE;
   BOOL   bHaveData = FALSE;
   PSZ  * ppszLst = NULL;
   USHORT usExtNumCols = 0;

   CHAR   szTmpAmt[SMARRAY] = "0";



   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryItems, pszProposalKey, szQueryItemsOrder);


   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hhprProp, hmodrProp, ITIRID_RPT,
                          ID_QUERY_ITEMS, &usNumCols, &usErr);
   if (hqry == NULL)
      {
      EXEC_ERR;
      }
   else
      {
      while (ItiDbGetQueryRow(hqry, &ppszLst, &usErr) )
         {
         /* -- Check for new page. */
         usLn = 4 + (ItiStrLen(ppszLst[iDesc]) / acttl[RPT_TTL_58].usDtaColWth);
         if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
            {
            usCurrentPg = ItiPrtQueryCurrentPgNum();
            SetColumnTitles(ITEM, pszID);
            bFirst = FALSE;
            bHaveData = TRUE;
            }
         else if (bFirst)
            {
            SetColumnTitles(ITEM, pszID);
            bFirst = FALSE;
            bHaveData = TRUE;
            }

         /* -- Item number: */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_53].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_53].usDtaColWth | REL_LEFT),
                       ppszLst[ITM_NUM]);

         /* -- Quantity: */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_54].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_54].usDtaColWth | REL_LEFT),
                       ppszLst[ITM_QTY]);


         /* -- Awd Unit: */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_56].usTtlCol,
                       LEFT, (2 | REL_LEFT),
                       acttl[RPT_TTL_56].szTitle);
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_56].usDtaCol,
                       LEFT, (acttl[RPT_TTL_56].usDtaColWth | REL_LEFT),
                       ppszLst[ITM_UNIT]);
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_60].usTtlCol,
                       LEFT, (acttl[RPT_TTL_60].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_60].szTitle);
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_59].usTtlCol,
                       LEFT, (acttl[RPT_TTL_59].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_59].szTitle);


         /* -- Low Unit: */
         ItiPrtPutTextAt (CURRENT_LN + 1, acttl[RPT_TTL_56].usTtlCol,
                       LEFT, (2 | REL_LEFT),
                       acttl[RPT_TTL_56].szTitle);
         ItiPrtPutTextAt (CURRENT_LN + 1, acttl[RPT_TTL_56].usDtaCol,
                       LEFT, (acttl[RPT_TTL_56].usDtaColWth | REL_LEFT),
                       ppszLst[ITM_UNIT]);
         ItiPrtPutTextAt (CURRENT_LN + 1, acttl[RPT_TTL_61].usTtlCol,
                       LEFT, (acttl[RPT_TTL_61].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_61].szTitle);
         ItiPrtPutTextAt (CURRENT_LN + 1, acttl[RPT_TTL_59].usTtlCol,
                       LEFT, (acttl[RPT_TTL_59].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_59].szTitle);


         /* -- Est Unit: */
         ItiPrtPutTextAt (CURRENT_LN + 2, acttl[RPT_TTL_56].usTtlCol,
                       LEFT, (2 | REL_LEFT),
                       acttl[RPT_TTL_56].szTitle);
         ItiPrtPutTextAt (CURRENT_LN + 2, acttl[RPT_TTL_56].usDtaCol,
                       LEFT, (acttl[RPT_TTL_56].usDtaColWth | REL_LEFT),
                       ppszLst[ITM_UNIT]);
         ItiPrtPutTextAt (CURRENT_LN + 2, acttl[RPT_TTL_62].usTtlCol,
                       LEFT, (acttl[RPT_TTL_62].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_62].szTitle);
         ItiPrtPutTextAt (CURRENT_LN + 2, acttl[RPT_TTL_59].usTtlCol,
                       LEFT, (acttl[RPT_TTL_59].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_59].szTitle);



         /* ==== Do Price/Extensions for the current item. */

         /* -- Awd UnitPrice and Extension: */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_55].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_55].usDtaColWth | REL_LEFT),
                       ppszLst[ITM_AWD_PRC]);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_57].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_57].usDtaColWth | REL_LEFT),
                       ppszLst[ITM_AWD_EXT]);

         /* -- StrikeThru if not part of the low cost alt. for the bidder. */
         if (ppszLst[ITM_AWD_LCA][0] == 'F')
            {
            usR = acttl[RPT_TTL_57].usDtaCol + acttl[RPT_TTL_57].usDtaColWth;
            usL = usR - ItiStrLen(ppszLst[ITM_AWD_EXT]);
            ItiPrtDrawSeparatorLn (CURRENT_LN, usL, usR);
            }
                                       
                          


         /* -- Low UnitPrice and Extension: */
         ItiPrtPutTextAt (CURRENT_LN + 1, acttl[RPT_TTL_55].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_55].usDtaColWth | REL_LEFT),
                       ppszLst[ITM_LOW_PRC]);

         ItiPrtPutTextAt (CURRENT_LN + 1, acttl[RPT_TTL_57].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_57].usDtaColWth | REL_LEFT),
                       ppszLst[ITM_LOW_EXT]);

         /* -- StrikeThru if not part of the low cost alt. for the bidder. */
         if (ppszLst[ITM_LOW_LCA][0] == 'F')
            {
            usR = acttl[RPT_TTL_57].usDtaCol + acttl[RPT_TTL_57].usDtaColWth;
            usL = usR - ItiStrLen(ppszLst[ITM_LOW_EXT]);
            ItiPrtDrawSeparatorLn (CURRENT_LN + 1, usL, usR);
            }
                                       




            /* -- Est UnitPrice and Extension: */
         ItiPrtPutTextAt (CURRENT_LN + 2, acttl[RPT_TTL_55].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_55].usDtaColWth | REL_LEFT),
                       ppszLst[ITM_EST_PRC]);

         ItiPrtPutTextAt (CURRENT_LN + 2, acttl[RPT_TTL_57].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_57].usDtaColWth | REL_LEFT),
                       ppszLst[ITM_EST_EXT]);

                                                                 





         ItiPrtIncCurrentLn(); //current_ln   (Awd) 
         ItiPrtIncCurrentLn(); //current_ln+1 (Low)
         ItiPrtIncCurrentLn(); //current_ln+2 (Est)

         /* -- Do the Description: */
         ItiPrtPutTextAt (CURRENT_LN, (acttl[RPT_TTL_58].usDtaCol),
                          (LEFT | WRAP | LNINC),
                          (acttl[RPT_TTL_58].usDtaColWth | REL_LEFT),
                          ppszLst[iDesc]);
         ItiPrtIncCurrentLn(); //blank line


         ItiFreeStrArray (hhprProp, ppszLst, usNumCols);

         }/* end of while (ItiDbGetQuery... */


      if (bHaveData == FALSE)
         {
         ItiPrtPutTextAt (CURRENT_LN, 5,
                          CENTER, 74,
               "*** NO Items were found in the database for this Proposal.");
         ItiPrtIncCurrentLn (); //line feed
         }

      }/* end if (hqry... else clause */


   /* -- Close out this list subsection. */
   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   ItiPrtIncCurrentLn (); //line feed

   return 0;
   }/* End of Function PropoItemList */











USHORT PrintProc (HWND hwnd, MPARAM mp1, MPARAM mp2)
   {
   USHORT us = ITIRPTUT_ERROR;
   CHAR   szKey      [SMARRAY] = "";
   CHAR   szProposalKeyStr [SMARRAY] = "";
   CHAR   szCmdLn [4*SMARRAY +1] = "";
   BOOL   bChecked = FALSE;


   /* -- reset the command line. */
   ItiStrCpy (szCmdLn, RPT_DLL_NAME, sizeof(szCmdLn) );


   bChecked = (USHORT) (ULONG) WinSendDlgItemMsg (hwnd, 
                               DID_SELECTED, BM_QUERYCHECK, 0, 0);
   if (bChecked)
      {
      /* -- Add the option switch to use the item's short descriptions. */
      ItiStrCat (szCmdLn, OPT_SW_SHORT_DESC_STR, sizeof szCmdLn);

      }/* end of if (bChecked... */



   /* -- Get the Proposal key. */
   WinQueryDlgItemText (hwnd, DID_KEY,
                        sizeof szProposalKeyStr, szProposalKeyStr);

   if (ItiStrLen(szProposalKeyStr) == 0)
      {
      ItiStrCpy (szErrorMsg,
                 "ERROR: No Proposal KEY returned from ",
                 sizeof szErrorMsg);
      ItiStrCat (szErrorMsg, TITLE, sizeof szErrorMsg);
      ItiStrCat (szErrorMsg, " dialog box query.", sizeof szErrorMsg);

      ItiRptUtErrorMsgBox (hwnd, szErrorMsg);
      return (13);
      }
   else
      {
      ItiStrCat (szCmdLn, szProposalKeyStr, sizeof szCmdLn);
      }



   us = ItiRptUtExecRptCmdLn (hwnd, szCmdLn, RPT_PROP_SESSION);

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
   ItiStrCpy (pszBuffer, "Proposal Listing", usMaxSize);   
   *pusWindowID = rProp_RptDlgBox;                       
   }/* END OF FUNCTION */


BOOL EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID, 
                                     USHORT usActiveWindowID) 
   {                                                          
   if ((usTargetWindowID == rProp_RptDlgBox)                     
       && (usActiveWindowID == ProposalS))                  
      return TRUE;                                            
   else                                                       
      return FALSE;                                           
   }/* END OF FUNCTION */



MRESULT EXPORT ItiDllrPropRptDlgProc (HWND     hwnd,     
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
         us = ItiRptUtInitDlgBox (hwnd, RPT_PROJ_SESSION, 0, NULL);
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
   hhprProp = ItiMemCreateHeap (MIN_HEAP_SIZE);


   /* ------------------------------------------------------------------- *\
    * Initialize the strings.                                             *
   \* ------------------------------------------------------------------- */
   ItiStrCpy(szID,"TABLE_ID",sizeof(szID));

   /* -- Get any query ordering information, BEFORE setting rpt titles. */
   ItiRptUtGetOption (ITIRPTUT_ORDERING_SWITCH, szOrderingStr, LGARRAY,
                      argcnt, argvars);
   ConvertOrderingStr (szOrderingStr);


   ItiRptUtGetOption (OPT_SW_SHORT_DESC_CHR, szFlagStr, sizeof szFlagStr,
                      argcnt, argvars);
   if (szFlagStr[0] != '\0') 
      iDesc = ITM_SHORT_DESC;
   else
      iDesc = ITM_LONG_DESC;



   /* -- Get the report's titles for this DLL. */
   ItiRptUtLoadLabels (hmodrProp, &usNumOfTitles, &acttl[0]);

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



   ItiMbQueryQueryText(hmodrProp, ITIRID_RPT, ID_QUERY_LISTING,
                       szQueryListing, sizeof szQueryListing );

   ItiMbQueryQueryText(hmodrProp, ITIRID_RPT, ID_QUERY_LISTING_PROJS,
                       szQueryListingProjs, sizeof szQueryListingProjs );


   ItiMbQueryQueryText(hmodrProp, ITIRID_RPT, ID_QUERY_BIDKEYS,
                       szQueryBidKeys, sizeof szQueryBidKeys );


   ItiMbQueryQueryText(hmodrProp, ITIRID_RPT, ID_QUERY_PITM_KEYS,
                       szQueryPItmKeys, sizeof szQueryPItmKeys );

   ItiMbQueryQueryText(hmodrProp, ITIRID_RPT, ID_QUERY_RANK,
                       szQueryRank, sizeof szQueryRank );

   ItiMbQueryQueryText(hmodrProp, ITIRID_RPT, ID_QUERY_RANK_ORDER,
                       szQueryRankOrder, sizeof szQueryRankOrder );


   ItiMbQueryQueryText(hmodrProp, ITIRID_RPT, ID_QUERY_MJR,
                       szQueryMjr, sizeof szQueryMjr );

   ItiMbQueryQueryText(hmodrProp, ITIRID_RPT, ID_QUERY_MJR_ORDER,
                       szQueryMjrOrder, sizeof szQueryMjrOrder );


   ItiMbQueryQueryText(hmodrProp, ITIRID_RPT, ID_QUERY_MJR_TTL,
                       szQueryMjrTtl, sizeof szQueryMjrTtl );

   ItiMbQueryQueryText(hmodrProp, ITIRID_RPT, ID_QUERY_MJR_TTL_PCT,
                       szQueryMjrTtlPct, sizeof szQueryMjrTtlPct );


// // //

   ItiMbQueryQueryText(hmodrProp, ITIRID_RPT, ID_QUERY_ITEMS,
                       szQueryItems, sizeof szQueryItems );

   ItiMbQueryQueryText(hmodrProp, ITIRID_RPT, ID_QUERY_ITEMS_CONJ,
                       szQueryItemsConj, sizeof szQueryItemsConj );

   ItiMbQueryQueryText(hmodrProp, ITIRID_RPT, ID_QUERY_ITEMS_ORDER,
                       szQueryItemsOrder, sizeof szQueryItemsOrder );


//   ItiMbQueryQueryText(hmodrProp, ITIRID_RPT, ID_QUERY_AWD_SLCT,
//                       szQueryAwdSlct, sizeof szQueryAwdSlct );
//
//   ItiMbQueryQueryText(hmodrProp, ITIRID_RPT, ID_QUERY_AWD,
//                       szQueryAwd, sizeof szQueryAwd );
//
//   ItiMbQueryQueryText(hmodrProp, ITIRID_RPT, ID_QUERY_AWD_CONJ,
//                       szQueryAwdConj, sizeof szQueryAwdConj );
//
//
//   ItiMbQueryQueryText(hmodrProp, ITIRID_RPT, ID_QUERY_LOW_SLCT,
//                       szQueryLowSlct, sizeof szQueryLowSlct );
//
//   ItiMbQueryQueryText(hmodrProp, ITIRID_RPT, ID_QUERY_LOW,
//                       szQueryLow, sizeof szQueryLow );
//
//   ItiMbQueryQueryText(hmodrProp, ITIRID_RPT, ID_QUERY_LOW_CONJ,
//                       szQueryLowConj, sizeof szQueryLowConj );
//
//
//   ItiMbQueryQueryText(hmodrProp, ITIRID_RPT, ID_QUERY_EST_SLCT,
//                       szQueryEstSlct, sizeof szQueryEstSlct );
//
//   ItiMbQueryQueryText(hmodrProp, ITIRID_RPT, ID_QUERY_EST,
//                       szQueryEst, sizeof szQueryEst );
//
//   ItiMbQueryQueryText(hmodrProp, ITIRID_RPT, ID_QUERY_EST_CONJ,
//                       szQueryEstConj, sizeof szQueryEstConj );


   

   /* -------------------------------------------------------------------- *\
    * -- For each Key...                                                   *
   \* -------------------------------------------------------------------- */
   ItiRptUtInitKeyList(argcnt, argvars); /*  */
   while (ItiRptUtGetNextKey(pszKey,sizeof(szKey)) != ITIRPTUT_NO_MORE_KEYS)
      {
      us = SummarySectionFor(pszKey, szID);

      if (us == 0)
         {
         us = RankSection (pszKey, szID);
         us = MajorItemSection (pszKey, szID);
         us = PropoItemList (pszKey, szID);
         }

      }/* End of WHILE GetNextKey */



   /* ------------------------------------------------------------------- *\
    * Finish up and quit.                                                 *
   \* ------------------------------------------------------------------- */
   ItiPrtEndReport();

   DBGMSG("Exit ***DLL function ItiRptDLLPrintReport");
   return 0;

   }/* END OF FUNCTION ItiRptDLLPrintReport */





