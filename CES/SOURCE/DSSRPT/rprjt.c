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


#include    "rindllpj.h"


#include "rPrjt.h"                                       



/* ======================================================================= *\
 *                                         MANIFEST CONSTANT DEFINITIONS   *
\* ======================================================================= */
#define  BLANK_LINE_INTERVAL    4

#define  PRT_Q_NAME             "Project List"

#define  QUERY_ARRAY_SIZE       1023

#define  SIZE_OF_QUERY_RPT_DESC QUERY_ARRAY_SIZE




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
static CHAR szDllVersion[] = "1.1a0 rprjt.dll";

static  CHAR  szID   [SMARRAY + 1] = "";
static  CHAR  szOrderingStr [SMARRAY + 1] = "";
static  CHAR  szFlagStr     [8] = "";
static  CHAR  szProposalKey [SMARRAY + 1] = "";

static  CHAR  szFailedDLL [CCHMAXPATH + 1] = "";
static  CHAR  szErrorMsg  [1023] = "";

static  USHORT usNumOfTitles;
static  USHORT usCurrentPg = 0;



           /* Used by DbExecQuer & DbGetQueryRow functions: */
static  HHEAP   hhprPrjt  = NULL;
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

static  CHAR  szBaseQuery    [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szQuery        [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szKey          [SMARRAY + 1] = "";

static  CHAR  szQueryBidKeys [SIZE_OF_QUERY_BIDKEYS + 1] = "";

static  CHAR  szQueryListing [SIZE_OF_QUERY_LISTING + 1] = "";

static  CHAR  szQueryCat     [SIZE_OF_QUERY_CAT + 1] = "";
static  CHAR  szQueryCatConj [SIZE_OF_CONJ + 1] = "";
static  CHAR  szQueryCatKeys [SIZE_OF_QUERY_CATKEYS + 1] = "";

static  CHAR  szQueryItems      [SIZE_OF_QUERY_ITEMS + 1] = "";
static  CHAR  szQueryItemsConj  [SIZE_OF_CONJ + 1] = "";
static  CHAR  szQueryItemsOrder [SIZE_OF_CONJ + 1] = "";



static  CHAR  szQueryQPE     [QUERY_ARRAY_SIZE + 1] = "";

static  CHAR  szQueryAwdSlct [2 * SIZE_OF_CONJ + 1] = "";
static  CHAR  szQueryAwdConj [SIZE_OF_CONJ + 1] = "";
static  CHAR  szQueryAwd     [SIZE_OF_QUERY_AWD + 1] = "";

static  CHAR  szQueryLowSlct [2 * SIZE_OF_CONJ + 1] = "";
static  CHAR  szQueryLowConj [SIZE_OF_CONJ + 1] = "";
static  CHAR  szQueryLow     [SIZE_OF_QUERY_LOW + 1] = "";

static  CHAR  szQueryEstSlct [2 * SIZE_OF_CONJ + 1] = "";
static  CHAR  szQueryEstConj [SIZE_OF_CONJ + 1] = "";
static  CHAR  szQueryEst     [SIZE_OF_QUERY_EST + 1] = "";






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







USHORT SetColumnTitles (VOID)
   /* -- Print the item list column titles. */
   {
   USHORT us;

      /* -- Actual printing. */
   for (us = RPT_TTL_27; us <= RPT_TTL_31; us++)
      {
      ItiPrtPutTextAt (CURRENT_LN, acttl[us].usTtlCol,
                       LEFT, (acttl[us].usTtlColWth | REL_LEFT),
                       acttl[us].szTitle);
      }
   ItiPrtIncCurrentLn (); //line feed

   us = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_32].usTtlCol,
                         LEFT, (acttl[RPT_TTL_32].usTtlColWth | REL_LEFT),
                         acttl[RPT_TTL_32].szTitle);
   ItiPrtIncCurrentLn (); //line feed


   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   ItiPrtIncCurrentLn (); //line feed

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





USHORT SummarySectionFor (PSZ pszKey, PSZ pszID)
   {
   USHORT usLn;
   PSZ  * ppsz = NULL;
   BOOL   bAwarded = FALSE;
   BOOL   bLowBid  = FALSE;
   BOOL   bAwardToLowBid = FALSE;



   if ((pszKey == NULL) || (*pszKey == '\0'))
      return 13;



   /* -- Build the bid keys query. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryBidKeys, " ", pszKey);

   ppsz = ItiDbGetRow1 (szQuery, hhprPrjt, hmodrPrjt, ITIRID_RPT,
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
      ItiFreeStrArray (hhprPrjt, ppsz, usNumCols);
      }
   else
      {
      ItiStrCpy(szErrorMsg,
                "RPT ERROR: Failed to get Bid keys for ProjectKey: ",
                sizeof szErrorMsg);
      ItiStrCat(szErrorMsg, pszKey, sizeof szErrorMsg);

      ItiRptUtErrorMsgBox(NULL, szErrorMsg);
      }







   /* -- Build the query. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryListing, " ", pszKey);


   ppsz = ItiDbGetRow1 (szQuery, hhprPrjt, hmodrPrjt, ITIRID_RPT,
                        ID_QUERY_LISTING, &usNumCols);

   if (ppsz != NULL)
      {
      usLn = 16 + ItiStrLen(ppsz[DESC]) / acttl[RPT_TTL_3].usDtaColWth;
      if ((ItiPrtKeepNLns(usLn) == PG_WAS_INC)
          || (ItiPrtQueryCurrentPgNum() != usCurrentPg ))
         {
         usCurrentPg = ItiPrtQueryCurrentPgNum();
         ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
         }

   /* -- Save the ProposalKey. */
      ItiStrCpy(szProposalKey, ppsz[PROP_KEY], sizeof szProposalKey);
 

   /* -- Project Control Number: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_1].usTtlCol,
                       LEFT, (acttl[RPT_TTL_1].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_1].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_1].usDtaCol,
                       LEFT, (acttl[RPT_TTL_1].usDtaColWth | REL_LEFT),
                       ppsz[PCN]);
      ItiStrCpy(pszID, ppsz[PCN], sizeof szID);


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


   /* -- Assigned to: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_4].usTtlCol,
                       LEFT, (acttl[RPT_TTL_4].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_4].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_4].usDtaCol,
                       LEFT, (acttl[RPT_TTL_4].usDtaColWth | REL_LEFT),
                       ppsz[PROP_ID]);
      ItiPrtIncCurrentLn();
      ItiPrtIncCurrentLn();


   /* -- Work Type: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_5].usTtlCol,
                       LEFT, (acttl[RPT_TTL_5].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_5].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_5].usDtaCol,
                       LEFT, (acttl[RPT_TTL_5].usDtaColWth | REL_LEFT),
                       ppsz[WRK_TYPE]);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_8].usTtlCol,
                       LEFT, (acttl[RPT_TTL_8].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_8].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_8].usDtaCol,
                       LEFT, (acttl[RPT_TTL_8].usDtaColWth | REL_LEFT),
                       ppsz[WRK_TRAN]);
      ItiPrtIncCurrentLn();


   /* -- Urban/Rural: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_6].usTtlCol,
                       LEFT, (acttl[RPT_TTL_6].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_6].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_6].usDtaCol,
                       LEFT, (acttl[RPT_TTL_6].usDtaColWth | REL_LEFT),
                       ppsz[CLASS]);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_8].usTtlCol,
                       LEFT, (acttl[RPT_TTL_8].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_8].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_8].usDtaCol,
                       LEFT, (acttl[RPT_TTL_8].usDtaColWth | REL_LEFT),
                       ppsz[CLASS_TRAN]);
      ItiPrtIncCurrentLn();



   /* -- District: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_7].usTtlCol,
                       LEFT, (acttl[RPT_TTL_7].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_7].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_7].usDtaCol,
                       LEFT, (acttl[RPT_TTL_7].usDtaColWth | REL_LEFT),
                       ppsz[DIST]);
      ItiPrtIncCurrentLn();



   /* -- Spec Year: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_9].usTtlCol,
                       LEFT, (acttl[RPT_TTL_9].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_9].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_9].usDtaCol,
                       LEFT, (acttl[RPT_TTL_9].usDtaColWth | REL_LEFT),
                       ppsz[SPEC_YR]);
      ItiPrtIncCurrentLn();



   /* -- Location: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_10].usTtlCol,
                       LEFT, (acttl[RPT_TTL_10].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_10].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_10].usDtaCol,
                       LEFT, (acttl[RPT_TTL_10].usDtaColWth | REL_LEFT),
                       ppsz[LOC]);
      ItiPrtIncCurrentLn();


   /* -- Length: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_11].usTtlCol,
                       LEFT, (acttl[RPT_TTL_11].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_11].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_11].usDtaCol,
                       LEFT, (acttl[RPT_TTL_11].usDtaColWth | REL_LEFT),
                       ppsz[LENGTH]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                       LEFT, (acttl[RPT_TTL_18].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_18].szTitle);

   /* -- Width: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usTtlCol,
                       LEFT, (acttl[RPT_TTL_12].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_12].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usDtaCol,
                       LEFT, (acttl[RPT_TTL_12].usDtaColWth | REL_LEFT),
                       ppsz[WIDTH]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                       LEFT, (acttl[RPT_TTL_19].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_19].szTitle);
      ItiPrtIncCurrentLn();
      ItiPrtIncCurrentLn();



      /* -- Now do the Project Amounts subsection: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_13].usTtlCol,
                       LEFT, (acttl[RPT_TTL_13].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_13].szTitle);

      if (bAwardToLowBid)
         {
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_15].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_15].usDtaColWth | REL_LEFT),
                          ppsz[AMT_AWARD]);
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_15].usTtlCol,
                          LEFT, (acttl[RPT_TTL_15].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_15].szTitle);
         ItiPrtIncCurrentLn();
         }
      else
         {
         if (bAwarded) 
            {
            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_14].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_14].usDtaColWth | REL_LEFT),
                          ppsz[AMT_AWARD]);
            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_14].usTtlCol,
                          LEFT, (acttl[RPT_TTL_14].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_14].szTitle);
            ItiPrtIncCurrentLn();
            }

         if (bLowBid)
            {
            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_16].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_16].usDtaColWth | REL_LEFT),
                          ppsz[AMT_LOW]);
            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_16].usTtlCol,
                          LEFT, (acttl[RPT_TTL_16].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_16].szTitle);
            ItiPrtIncCurrentLn();
            }
         }/* end else clause  if (bAwardToLowBid... */


      /* -- Engineer's estimate: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_17].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_17].usDtaColWth | REL_LEFT),
                       ppsz[AMT_EST]);
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_17].usTtlCol,
                       LEFT, (acttl[RPT_TTL_17].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_17].szTitle);
      ItiPrtIncCurrentLn();


      ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
      ItiPrtIncCurrentLn (); //line feed


      /* -- We're now done with this query, so... */
      ItiFreeStrArray (hhprPrjt, ppsz, usNumCols);

      }/* end if (ppsz !=... / _LISTING */
   else
      return 13;

   return 0;
   }/* End of Function SummarySectionFor */






USHORT CategoryItemList (PSZ pszCatID, PSZ pszCatProjectKey)
/* -- This function design requires at least TWO database connections. */
   {
   USHORT usLn = 12;
   BOOL   bFirst = TRUE;
   BOOL   bHaveData = FALSE;
   PSZ  * ppszLst = NULL;
   PSZ  * ppszExt = NULL;
   USHORT usExtNumCols, usLen;
   CHAR   szTotalAwdAmt[SMARRAY] = "0";
   CHAR   szTotalLowAmt[SMARRAY] = "0";
   CHAR   szTotalEstAmt[SMARRAY] = "0";
   CHAR   szAwdAmt[SMARRAY] = "0";
   CHAR   szLowAmt[SMARRAY] = "0";
   CHAR   szEstAmt[SMARRAY] = "0";

   CHAR   szTmpAmt[SMARRAY] = "0";
   CHAR   szItmQty[SMARRAY] = "";



   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryItems, pszCatProjectKey, szQueryItemsConj);

   ItiStrCat(szQuery, pszCatID, sizeof szQuery);
   ItiStrCat(szQuery, szQueryItemsOrder, sizeof szQuery);


   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hhprPrjt, hmodrPrjt, ITIRID_RPT,
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
         usLn = 12 + (ItiStrLen(ppszLst[iDesc]) / acttl[RPT_TTL_32].usDtaColWth);
         if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
            {
            usCurrentPg = ItiPrtQueryCurrentPgNum();
            SetColumnTitles();
            bFirst = FALSE;
            bHaveData = TRUE;
            }
         else if (bFirst)
            {
            SetColumnTitles();
            bFirst = FALSE;
            bHaveData = TRUE;
            }

         /* -- Item number: */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_27].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_27].usDtaColWth | REL_LEFT),
                       ppszLst[ITM_NUM]);

         /* -- Quantity: */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_28].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_28].usDtaColWth | REL_LEFT),
                       ppszLst[ITM_QTY]);


         /* -- Awd Unit: */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_30].usTtlCol,
                       LEFT, (2 | REL_LEFT),
                       acttl[RPT_TTL_30].szTitle);
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_30].usDtaCol,
                       LEFT, (acttl[RPT_TTL_30].usDtaColWth | REL_LEFT),
                       ppszLst[ITM_UNIT]);
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_34].usTtlCol,
                       LEFT, (acttl[RPT_TTL_34].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_34].szTitle);
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_33].usTtlCol,
                       LEFT, (acttl[RPT_TTL_33].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_33].szTitle);


         /* -- Low Unit: */
         ItiPrtPutTextAt (CURRENT_LN + 1, acttl[RPT_TTL_30].usTtlCol,
                       LEFT, (2 | REL_LEFT),
                       acttl[RPT_TTL_30].szTitle);
         ItiPrtPutTextAt (CURRENT_LN + 1, acttl[RPT_TTL_30].usDtaCol,
                       LEFT, (acttl[RPT_TTL_30].usDtaColWth | REL_LEFT),
                       ppszLst[ITM_UNIT]);
         ItiPrtPutTextAt (CURRENT_LN + 1, acttl[RPT_TTL_35].usTtlCol,
                       LEFT, (acttl[RPT_TTL_35].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_35].szTitle);
         ItiPrtPutTextAt (CURRENT_LN + 1, acttl[RPT_TTL_33].usTtlCol,
                       LEFT, (acttl[RPT_TTL_33].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_33].szTitle);


         /* -- Est Unit: */
         ItiPrtPutTextAt (CURRENT_LN + 2, acttl[RPT_TTL_30].usTtlCol,
                       LEFT, (2 | REL_LEFT),
                       acttl[RPT_TTL_30].szTitle);
         ItiPrtPutTextAt (CURRENT_LN + 2, acttl[RPT_TTL_30].usDtaCol,
                       LEFT, (acttl[RPT_TTL_30].usDtaColWth | REL_LEFT),
                       ppszLst[ITM_UNIT]);
         ItiPrtPutTextAt (CURRENT_LN + 2, acttl[RPT_TTL_36].usTtlCol,
                       LEFT, (acttl[RPT_TTL_36].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_36].szTitle);
         ItiPrtPutTextAt (CURRENT_LN + 2, acttl[RPT_TTL_33].usTtlCol,
                       LEFT, (acttl[RPT_TTL_33].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_33].szTitle);



         /* ==== Do Price/Extension query for the current item. */

         /* -- Build the AwdExt query. */
			ItiFmtFormatString (ppszLst[ITM_QTY], szItmQty,
				                 sizeof szItmQty, "Number,....", &usLen);
         ItiRptUtPrepQuery(szQueryQPE, (QUERY_ARRAY_SIZE + 1),
                           szQueryAwdSlct, szItmQty, szQueryAwd);
                                        
         ItiStrCat(szQueryQPE, szProposalKey,    sizeof szQueryQPE);
         ItiStrCat(szQueryQPE, szQueryAwdConj,   sizeof szQueryQPE);
         ItiStrCat(szQueryQPE, ppszLst[ITM_PROP_ITEM_KEY], sizeof szQueryQPE);
                                                                 
         ppszExt = ItiDbGetRow1 (szQueryQPE, hhprPrjt, hmodrPrjt, ITIRID_RPT,
                                 ID_QUERY_AWD, &usExtNumCols);
         if (ppszExt != NULL)                                       
            {
            /* -- Awd UnitPrice and Extension: */
            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_29].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_29].usDtaColWth | REL_LEFT),
                          ppszExt[AWD_UNIT_PRICE]);

            ItiStrCpy(szAwdAmt, ppszExt[AWD_EXT], sizeof szAwdAmt);

            ItiFreeStrArray (hhprPrjt, ppszExt, usExtNumCols);
            }

         /* -- Reformat for display. */
         ItiStrCpy(szTmpAmt, szAwdAmt, sizeof szTmpAmt);
         ItiRptUtMath (ID_EXP_2DC, sizeof szTmpAmt, szTmpAmt, 
                                   szTmpAmt, " + ", " 0 ");

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_31].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_31].usDtaColWth | REL_LEFT),
                       szTmpAmt);
                                       
                          


         /* -- Build the LowExt query. */
         ItiRptUtPrepQuery(szQueryQPE, (QUERY_ARRAY_SIZE + 1),
                           szQueryLowSlct, szItmQty, szQueryLow);
                                        
         ItiStrCat(szQueryQPE, szProposalKey,    sizeof szQueryQPE);
         ItiStrCat(szQueryQPE, szQueryLowConj,   sizeof szQueryQPE);
         ItiStrCat(szQueryQPE, ppszLst[ITM_PROP_ITEM_KEY], sizeof szQueryQPE);
                                                                 
         ppszExt = ItiDbGetRow1 (szQueryQPE, hhprPrjt, hmodrPrjt, ITIRID_RPT,
                                 ID_QUERY_LOW, &usExtNumCols);
         if (ppszExt != NULL)                                       
            {
            /* -- Low UnitPrice and Extension: */
            ItiPrtPutTextAt (CURRENT_LN + 1, acttl[RPT_TTL_29].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_29].usDtaColWth | REL_LEFT),
                          ppszExt[LOW_UNIT_PRICE]);

            ItiStrCpy(szLowAmt, ppszExt[LOW_EXT], sizeof szLowAmt);

            ItiFreeStrArray (hhprPrjt, ppszExt, usExtNumCols);
            }

         /* -- Reformat for display. */
         ItiStrCpy(szTmpAmt, szLowAmt, sizeof szTmpAmt);
         ItiRptUtMath (ID_EXP_2DC, sizeof szTmpAmt, szTmpAmt, 
                                   szTmpAmt, " + ", " 0 ");

         ItiPrtPutTextAt (CURRENT_LN + 1, acttl[RPT_TTL_31].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_31].usDtaColWth | REL_LEFT),
                       szTmpAmt);
                                                                 




         /* -- Build the EstExt query. */
         ItiRptUtPrepQuery(szQueryQPE, (QUERY_ARRAY_SIZE + 1),
                           szQueryEstSlct, szItmQty, szQueryEst);
                                        
         ItiStrCat(szQueryQPE, szProposalKey,    sizeof szQueryQPE);
         ItiStrCat(szQueryQPE, szQueryEstConj,   sizeof szQueryQPE);
         ItiStrCat(szQueryQPE, ppszLst[ITM_PROP_ITEM_KEY], sizeof szQueryQPE);
                                                                 
         ppszExt = ItiDbGetRow1 (szQueryQPE, hhprPrjt, hmodrPrjt, ITIRID_RPT,
                                 ID_QUERY_EST, &usExtNumCols);
         if (ppszExt != NULL)                                       
            {
            /* -- Est UnitPrice and Extension: */
            ItiPrtPutTextAt (CURRENT_LN + 2, acttl[RPT_TTL_29].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_29].usDtaColWth | REL_LEFT),
                          ppszExt[EST_UNIT_PRICE]);

            ItiStrCpy(szEstAmt, ppszExt[EST_EXT], sizeof szEstAmt);

            ItiFreeStrArray (hhprPrjt, ppszExt, usExtNumCols);
            }

         /* -- Reformat for display. */
         ItiStrCpy(szTmpAmt, szEstAmt, sizeof szTmpAmt);
         ItiRptUtMath (ID_EXP_2DC, sizeof szTmpAmt, szTmpAmt, 
                                   szTmpAmt, " + ", " 0 ");

         ItiPrtPutTextAt (CURRENT_LN + 2, acttl[RPT_TTL_31].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_31].usDtaColWth | REL_LEFT),
                       szTmpAmt);
                                                                 





         ItiPrtIncCurrentLn(); //current_ln   (Awd) 
         ItiPrtIncCurrentLn(); //current_ln+1 (Low)
         ItiPrtIncCurrentLn(); //current_ln+2 (Est)

         /* -- Do the Description: */
         ItiPrtPutTextAt (CURRENT_LN, (acttl[RPT_TTL_32].usDtaCol),
                          (LEFT | WRAP | LNINC),
                          (acttl[RPT_TTL_32].usDtaColWth | REL_LEFT),
                          ppszLst[iDesc]);
         ItiPrtIncCurrentLn(); //blank line




         /* ==== Update the running totals. */
         ItiRptUtMath (ID_EXP_4, sizeof szTotalAwdAmt, szTotalAwdAmt, 
                                 szTotalAwdAmt, " + ", szAwdAmt);
         ItiStrCpy(szAwdAmt, "0", sizeof szAwdAmt);

         ItiRptUtMath (ID_EXP_4, sizeof szTotalLowAmt, szTotalLowAmt, 
                                 szTotalLowAmt, " + ", szLowAmt);
         ItiStrCpy(szLowAmt, "0", sizeof szLowAmt);

         ItiRptUtMath (ID_EXP_4, sizeof szTotalEstAmt, szTotalEstAmt, 
                                 szTotalEstAmt, " + ", szEstAmt);
         ItiStrCpy(szEstAmt, "0", sizeof szEstAmt);


         ItiFreeStrArray (hhprPrjt, ppszLst, usNumCols);

         }/* end of while (ItiDbGetQuery... */


      if (bHaveData)
         {
         /* ====================== Do the category items total subsection. */
         ItiPrtDrawSeparatorLn (CURRENT_LN,
                                acttl[RPT_TTL_31].usDtaCol - 1,
                                (5 | FROM_RT_EDGE));
         ItiPrtIncCurrentLn (); //line feed

         /* -- Category Total label: */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_37].usTtlCol,
                          LEFT, (acttl[RPT_TTL_37].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_37].szTitle);


         /* -- Awarded total line: */
         ItiRptUtMath (ID_EXP_2DC, sizeof szTotalAwdAmt, szTotalAwdAmt, 
                                          szTotalAwdAmt, " + ", "0");

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_34].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_34].usDtaColWth | REL_LEFT),
                          szTotalAwdAmt);
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_34].usTtlCol,
                          LEFT, (acttl[RPT_TTL_34].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_34].szTitle);
         ItiPrtIncCurrentLn (); //line feed


         /* -- LowBid total line: */
         ItiRptUtMath (ID_EXP_2DC, sizeof szTotalLowAmt, szTotalLowAmt, 
                                          szTotalLowAmt, " + ", "0");

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_35].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_35].usDtaColWth | REL_LEFT),
                          szTotalLowAmt);
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_35].usTtlCol,
                          LEFT, (acttl[RPT_TTL_35].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_35].szTitle);
         ItiPrtIncCurrentLn (); //line feed


         /* -- Estimate total line: */
         ItiRptUtMath (ID_EXP_2DC, sizeof szTotalEstAmt, szTotalEstAmt, 
                                          szTotalEstAmt, " + ", "0");

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_36].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_36].usDtaColWth | REL_LEFT),
                          szTotalEstAmt);
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_36].usTtlCol,
                          LEFT, (acttl[RPT_TTL_36].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_36].szTitle);
         ItiPrtIncCurrentLn (); //line feed
         }/* end if bHaveData */
      else
         {
         ItiPrtPutTextAt (CURRENT_LN, 5,
                          CENTER, 74,
                          "** NO Items were found in the database for this category.");
         ItiPrtIncCurrentLn (); //line feed

         }

      }/* end if (hqry... else clause */


   /* -- Close out this list subsection. */
   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   ItiPrtIncCurrentLn (); //line feed

   return 0;
   }/* End of Function CategoryItemList */





USHORT CategorySectionsFor (PSZ pszKey, PSZ pszID)
   {
   USHORT us, usLn, usCatCnt, usLen;
   CHAR   szTmp[SMARRAY] = "";
   PSZ  * ppszLoc = NULL;
   PP_KEYSIDS ppkisCatKeys = NULL;
   HQRY   hqrySect = NULL;
	

   if ((pszKey == NULL) || (*pszKey == '\0'))
      return 13;


   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryCatKeys, " ", pszKey);

   /* -- Find out how many categories we have. */
   usCatCnt = ItiDbGetQueryCount(szQuery, &usNumCols, &usErr);
   ItiRptUtEstablishKeyIDList (hhprPrjt, (usCatCnt+1),
                               sizeof(P_KEYID), &ppkisCatKeys);

   /* -- Do the database query. */
   hqrySect = ItiDbExecQuery (szQuery, hhprPrjt, hmodrPrjt, ITIRID_RPT,
                          ID_QUERY_CATKEYS, &usNumCols, &usErr);
   if (hqrySect == NULL)
      {
      EXEC_ERR;
      }
   else
      {/* -- Build a list of category keys. */
      us = 0;
      while( ItiDbGetQueryRow (hqrySect, &ppszLoc, &usErr) )
         {
         if (ppszLoc == NULL || *ppszLoc == NULL)
            break;

         ItiRptUtAddToKeyIDList(hhprPrjt, ppszLoc[CAT_CN], ppszLoc[CAT_PKEY],
                                us, ppkisCatKeys, usCatCnt);

         ItiStrCpy(szTmp, "'", sizeof szTmp);
         ItiStrCat(szTmp, ppkisCatKeys[us]->pszID, sizeof szTmp);
         ItiStrCat(szTmp, "'", sizeof szTmp);
         usLen = ItiStrLen(ppkisCatKeys[us]->pszID) + 3;
         ItiStrCpy(ppkisCatKeys[us]->pszID, szTmp, usLen);


         us++;   /* the variable us keeps count of the query row number */
         ItiFreeStrArray (hhprPrjt, ppszLoc, usNumCols);
         }
      }/* end if (hqrySect... else clause */




//START LOOP
   for (us = 0; us < usCatCnt; us++)
      {
      /* -- Build the query. */
      ItiRptUtPrepQuery(szBaseQuery, (QUERY_ARRAY_SIZE + 1),
                        szQueryCat, " ", pszKey);

      ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                        szBaseQuery, szQueryCatConj, ppkisCatKeys[us]->pszID);

      ItiStrCat(szQuery, " order by 2 ", sizeof szQuery);

      ppszLoc = ItiDbGetRow1 (szQuery, hhprPrjt, hmodrPrjt, ITIRID_RPT,
                           ID_QUERY_CAT, &usNumCols);

      if (ppszLoc != NULL)
         {
         usLn = 12 + ItiStrLen(ppszLoc[CAT_DESC]) / acttl[RPT_TTL_21].usDtaColWth;
         if ((ItiPrtKeepNLns(usLn) == PG_WAS_INC)
             || (ItiPrtQueryCurrentPgNum() != usCurrentPg ))
            {
            usCurrentPg = ItiPrtQueryCurrentPgNum();
            ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
            }

         /* -- Category Number: */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_20].usTtlCol,
                          LEFT, (acttl[RPT_TTL_20].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_20].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_20].usDtaCol,
                          LEFT, (acttl[RPT_TTL_20].usDtaColWth | REL_LEFT),
                          ppszLoc[CAT_NUM]);
         ItiStrCpy(pszID, ppszLoc[CAT_NUM], sizeof szID);


         /* -- Description: */
         ItiPrtPutTextAt (CURRENT_LN, (acttl[RPT_TTL_21].usDtaCol),
                          (LEFT|WRAP),
                          (3 | FROM_RT_EDGE),
                          //  (acttl[RPT_TTL_21].usDtaColWth | REL_LEFT),
                          ppszLoc[CAT_DESC]);
         ItiPrtIncCurrentLn();


         /* -- Length: */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_11].usTtlCol,
                          LEFT, (acttl[RPT_TTL_11].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_11].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_11].usDtaCol,
                          LEFT, (acttl[RPT_TTL_11].usDtaColWth | REL_LEFT),
                          ppszLoc[CAT_LEN]);

         ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                          LEFT, (acttl[RPT_TTL_18].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_18].szTitle);

         /* -- Width: */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usTtlCol,
                          LEFT, (acttl[RPT_TTL_12].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_12].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usDtaCol,
                          LEFT, (acttl[RPT_TTL_12].usDtaColWth | REL_LEFT),
                          ppszLoc[CAT_WTH]);

         ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                          LEFT, (acttl[RPT_TTL_19].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_19].szTitle);
         ItiPrtIncCurrentLn();
         ItiPrtIncCurrentLn();



         /* -- If Structure ID is not blank then write structure info. */
         if ((ppszLoc[CAT_SID] != NULL) && (ppszLoc[CAT_SID][0] != '\0'))
            {
            /* -- Struct ID: */
            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_22].usTtlCol,
                             LEFT, (acttl[RPT_TTL_22].usTtlColWth | REL_LEFT),
                             acttl[RPT_TTL_22].szTitle);

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_22].usDtaCol,
                             LEFT, (acttl[RPT_TTL_22].usDtaColWth | REL_LEFT),
                             ppszLoc[CAT_SID]);
            ItiPrtIncCurrentLn();

            /* -- Struct Length: */
            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_23].usTtlCol,
                             LEFT, (acttl[RPT_TTL_23].usTtlColWth | REL_LEFT),
                             acttl[RPT_TTL_23].szTitle);

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_23].usDtaCol,
                             LEFT, (acttl[RPT_TTL_23].usDtaColWth | REL_LEFT),
                             ppszLoc[CAT_SLEN]);

            ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                             LEFT, (acttl[RPT_TTL_19].usTtlColWth | REL_LEFT),
                             acttl[RPT_TTL_19].szTitle);
            ItiPrtIncCurrentLn();

            /* -- Struct Width: */
            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_24].usTtlCol,
                             LEFT, (acttl[RPT_TTL_24].usTtlColWth | REL_LEFT),
                             acttl[RPT_TTL_24].szTitle);

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_24].usDtaCol,
                             LEFT, (acttl[RPT_TTL_24].usDtaColWth | REL_LEFT),
                             ppszLoc[CAT_SWTH]);

            ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                             LEFT, (acttl[RPT_TTL_19].usTtlColWth | REL_LEFT),
                             acttl[RPT_TTL_19].szTitle);
            ItiPrtIncCurrentLn();


            /* -- Struct Type: */
            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_25].usTtlCol,
                             LEFT, (acttl[RPT_TTL_25].usTtlColWth | REL_LEFT),
                             acttl[RPT_TTL_25].szTitle);

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_25].usDtaCol,
                             LEFT, (acttl[RPT_TTL_25].usDtaColWth | REL_LEFT),
                             ppszLoc[CAT_STYP]);

            ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                             LEFT, (acttl[RPT_TTL_26].usTtlColWth | REL_LEFT),
                             acttl[RPT_TTL_26].szTitle);

            ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                             LEFT, (acttl[RPT_TTL_26].usDtaColWth | REL_LEFT),
                             ppszLoc[CAT_STYPE_TRANS]);
            ItiPrtIncCurrentLn();



            }/* End of if ((ppszLoc[CAT_SID... */



         ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
         ItiPrtIncCurrentLn (); //line feed


         /* -- We're now done with this query, so... */
         ItiFreeStrArray (hhprPrjt, ppszLoc, usNumCols);


         /* -- Now do the Item listing for the current project category. */
         CategoryItemList(ppkisCatKeys[us]->pszID,  /* 'CategoryNumber'  */
                          ppkisCatKeys[us]->szKey); /*  ProjectKey       */

         }/* end if (ppszLoc !=... / _CAT */
      else
         {
         //return 13;
         }

      }/* end for loop */
//END LOOP


   ItiRptUtDestroyKeyIDList (hhprPrjt, &ppkisCatKeys, usCatCnt);

   return 0;
   }/* End of Function CategorySectionsFor */












USHORT PrintProc (HWND hwnd, MPARAM mp1, MPARAM mp2)
   {
   USHORT us = ITIRPTUT_ERROR;
   CHAR   szKey      [SMARRAY] = "";
   CHAR   szProjectKeyStr [SMARRAY] = "";
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



   /* -- Get the Project key. */
   WinQueryDlgItemText (hwnd, DID_KEY,
                        sizeof szProjectKeyStr, szProjectKeyStr);

   if (ItiStrLen(szProjectKeyStr) == 0)
      {
      ItiStrCpy (szErrorMsg,
                 "ERROR: No Project KEY returned from ",
                 sizeof szErrorMsg);
      ItiStrCat (szErrorMsg, TITLE, sizeof szErrorMsg);
      ItiStrCat (szErrorMsg, " dialog box query.", sizeof szErrorMsg);

      ItiRptUtErrorMsgBox (hwnd, szErrorMsg);
      return (13);
      }
   else
      {
      ItiStrCat (szCmdLn, szProjectKeyStr, sizeof szCmdLn);
      }



   us = ItiRptUtExecRptCmdLn (hwnd, szCmdLn, RPT_PROJ_SESSION);

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
   ItiStrCpy (pszBuffer, "Project Listing", usMaxSize);   
   *pusWindowID = rPrjt_RptDlgBox;                       
   }/* END OF FUNCTION */


BOOL EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID, 
                                     USHORT usActiveWindowID) 
   {                                                          
   if ((usTargetWindowID == rPrjt_RptDlgBox)                     
       && (usActiveWindowID == ProjectS))                  
      return TRUE;                                            
   else                                                       
      return FALSE;                                           
   }/* END OF FUNCTION */



MRESULT EXPORT ItiDllrPrjtRptDlgProc (HWND     hwnd,     
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
   hhprPrjt = ItiMemCreateHeap (MIN_HEAP_SIZE);


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
   ItiRptUtLoadLabels (hmodrPrjt, &usNumOfTitles, &acttl[0]);

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



   ItiMbQueryQueryText(hmodrPrjt, ITIRID_RPT, ID_QUERY_BIDKEYS,
                       szQueryBidKeys, sizeof szQueryBidKeys );

   ItiMbQueryQueryText(hmodrPrjt, ITIRID_RPT, ID_QUERY_LISTING,
                       szQueryListing, sizeof szQueryListing );


   ItiMbQueryQueryText(hmodrPrjt, ITIRID_RPT, ID_QUERY_CATKEYS,
                       szQueryCatKeys, sizeof szQueryCatKeys );

   ItiMbQueryQueryText(hmodrPrjt, ITIRID_RPT, ID_QUERY_CAT,
                       szQueryCat, sizeof szQueryCat );

   ItiMbQueryQueryText(hmodrPrjt, ITIRID_RPT, ID_QUERY_CAT_CONJ,
                       szQueryCatConj, sizeof szQueryCatConj );


   ItiMbQueryQueryText(hmodrPrjt, ITIRID_RPT, ID_QUERY_ITEMS,
                       szQueryItems, sizeof szQueryItems );

   ItiMbQueryQueryText(hmodrPrjt, ITIRID_RPT, ID_QUERY_ITEMS_CONJ,
                       szQueryItemsConj, sizeof szQueryItemsConj );

   ItiMbQueryQueryText(hmodrPrjt, ITIRID_RPT, ID_QUERY_ITEMS_ORDER,
                       szQueryItemsOrder, sizeof szQueryItemsOrder );


   ItiMbQueryQueryText(hmodrPrjt, ITIRID_RPT, ID_QUERY_AWD_SLCT,
                       szQueryAwdSlct, sizeof szQueryAwdSlct );

   ItiMbQueryQueryText(hmodrPrjt, ITIRID_RPT, ID_QUERY_AWD,
                       szQueryAwd, sizeof szQueryAwd );

   ItiMbQueryQueryText(hmodrPrjt, ITIRID_RPT, ID_QUERY_AWD_CONJ,
                       szQueryAwdConj, sizeof szQueryAwdConj );


   ItiMbQueryQueryText(hmodrPrjt, ITIRID_RPT, ID_QUERY_LOW_SLCT,
                       szQueryLowSlct, sizeof szQueryLowSlct );

   ItiMbQueryQueryText(hmodrPrjt, ITIRID_RPT, ID_QUERY_LOW,
                       szQueryLow, sizeof szQueryLow );

   ItiMbQueryQueryText(hmodrPrjt, ITIRID_RPT, ID_QUERY_LOW_CONJ,
                       szQueryLowConj, sizeof szQueryLowConj );


   ItiMbQueryQueryText(hmodrPrjt, ITIRID_RPT, ID_QUERY_EST_SLCT,
                       szQueryEstSlct, sizeof szQueryEstSlct );

   ItiMbQueryQueryText(hmodrPrjt, ITIRID_RPT, ID_QUERY_EST,
                       szQueryEst, sizeof szQueryEst );

   ItiMbQueryQueryText(hmodrPrjt, ITIRID_RPT, ID_QUERY_EST_CONJ,
                       szQueryEstConj, sizeof szQueryEstConj );


   

   /* -------------------------------------------------------------------- *\
    * -- For each Key...                                                   *
   \* -------------------------------------------------------------------- */
   ItiRptUtInitKeyList(argcnt, argvars); /*  */
   while (ItiRptUtGetNextKey(pszKey,sizeof(szKey)) != ITIRPTUT_NO_MORE_KEYS)
      {
      us = SummarySectionFor(pszKey, szID);

      if (us == 0)
         us = CategorySectionsFor (pszKey, szID);

      }/* End of WHILE GetNextKey */



   /* ------------------------------------------------------------------- *\
    * Finish up and quit.                                                 *
   \* ------------------------------------------------------------------- */
   ItiPrtEndReport();

   DBGMSG("Exit ***DLL function ItiRptDLLPrintReport");
   return 0;

   }/* END OF FUNCTION ItiRptDLLPrintReport */





