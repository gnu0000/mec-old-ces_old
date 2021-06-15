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
#include    "..\include\itimenu.h"

#include    "..\include\itiglob.h"

#include    "..\include\dialog.h"
#include    "dialog.h"

#include    "..\include\itirptdg.h"
#include    "..\include\itirpt.h"

#include   <limits.h>

/*
 * The following are support modules that are always used by a report DLL.
 */
#if !defined (INCL_ITIPRT)
#include "..\include\itiprt.h"
#endif

#if !defined (INCL_RPTUTIL)
#include "..\include\itirptut.h"
#endif

#include    "rinidef.h"

#include "rDefUP.h"                                       



/* ======================================================================= *\
 *                                         MANIFEST CONSTANT DEFINITIONS   *
\* ======================================================================= */
#define  BLANK_LINE_INTERVAL    4
#define  PRT_Q_NAME             "DfltUntPrices"

#define  MAX_CMDLNS             8
          /* The maximum number of expected report dlls in the list box. */
#define  MAX_LEN_CMDLN          128

#define  QUERY_ARRAY_SIZE       1023

#define  SIZE_OF_QUERY_RPT_DESC QUERY_ARRAY_SIZE
#define  NO_KEY  13


#define LEN_STRUCT_STR            18
typedef struct _Lnk
   {
   CHAR   szID   [LEN_STRUCT_STR];
   CHAR   szKey  [LEN_STRUCT_STR];
   USHORT usKey;
   CHAR   szLink [LEN_STRUCT_STR];
   CHAR   szLag  [LEN_STRUCT_STR];
   CHAR   szDesc [LGARRAY + 1];
   } LNK; /* lnk */


typedef LNK   *PLNK;     /* plnk   */
typedef PLNK  *PPLNK;    /* pplnk  */
typedef PPLNK *PPPLNK;   /* ppplnk */



typedef struct _ColTitles
           {
           CHAR    szTitle [RPT_TTL_LEN+1];
           USHORT  usTtlCol;
           USHORT  usTtlColWth;
           USHORT  usDtaCol;   
           USHORT  usDtaColWth;   
           } COLTITLES; /* cttl */




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
                   ItiRptUtErrorMsgBox (NULL, szErrorMsg);








/* ======================================================================= *\
 *                                             MODULE'S GLOBAL VARIABLES   *
\* ======================================================================= */
static  CHAR  szCompileDateTime[] = RPT_DLL_NAME " " __DATE__ " " __TIME__ ;

static  CHAR   szSubTitle[SMARRAY] = "";
static  USHORT usCurrentPg = 0;

static  CHAR  szCurrentID   [SMARRAY + 1] = "";
static  CHAR  szOrderingStr [LGARRAY + 1] = "";

static  CHAR  szQueryRptDesc [SIZE_OF_QUERY + 1] = "";

static  CHAR  szBaseQuery          [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szBaseQuerySumSec    [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szBaseQueryDefSumSec [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szBaseQueryDeflt     [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szBaseQueryPrdLst    [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szBaseQuerySucLst    [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szBaseQueryPctCst    [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szBaseQuerySumPct    [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szBaseQueryQCF       [QUERY_ARRAY_SIZE + 1] = "";

static  CHAR  szPrdOrderClause [SMARRAY+1] = "";
static  CHAR  szSucOrderClause [SMARRAY+1] = "";


static  CHAR  szQuery          [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szConjSpecYear   [SIZE_OF_CONJ + 1]     = "";
static  CHAR  szConjStdItemKey [SIZE_OF_CONJ + 1]   = "";

static  CHAR  szKey  [SMARRAY] = "";
static  CHAR  szUnit [SMARRAY] = "";

static  CHAR  szDate     [SMARRAY + 1] = "";
static  CHAR  szTmpDate  [SMARRAY + 1] = "";
static  CHAR  szSpecYear [SMARRAY + 1] = "";
static  CHAR  szBaseDate [SMARRAY + 1] = "";

static  CHAR  szOnly     [SMARRAY + 1] = "";
static  BOOL  bUseSelectedOnly = FALSE;
static  BOOL  bProceed = FALSE;

static  CHAR  szDfltPrc[8] = "";
static  CHAR  szBuffer   [4*SMARRAY+1] = "";
static  BOOL  bInclDfltPrices = FALSE;

static  CHAR  szCmdLn [LGARRAY+1] = "";

static  HPOINTER hptrDefault = NULL;
static  HPOINTER hptrWait    = NULL;

static  CHAR    szFailedDLL [CCHMAXPATH + 1] = "";
static  CHAR    szErrorMsg [512] = "";

static  USHORT usNumOfTitles = 0;

static  HAB ghab = NULL;

           /* Used by DbExecQuer & DbGetQueryRow functions: */
static  HHEAP   hheap  = NULL;
static  HQRY    hqry   = NULL;
static  USHORT  usResId   = ITIRID_RPT;
static  USHORT  usId      = ID_QUERY_LISTING;
static  USHORT  usNumCols = 0;
static  USHORT  usErr     = 0;
static  PSZ   * ppsz   = NULL;
         

static  USHORT usArgCnt = 2;
static  CHAR  szKeyFileName[CCHMAXPATH + 1] = "";
static  PSZ   apszAlt[2] = { "/K", szKeyFileName };
static  PSZ * pAltArgVar = &apszAlt[0];


static  PPLNK  ppPred = NULL;
static  PPLNK  ppSucc = NULL;


static  COLTITLES  acttl[RPT_TTL_NUM + 1];
        
        
        /* -- Used for supplemental reprots: */
static  CHAR    szSuppRpt[8] = "";
static  USHORT  (EXPORT *pDllFnRpt) (HAB, HMQ, INT, CHAR **);
static  HMODULE hmodRpt = 0;
static  BOOL    bPrtAvgRpt = FALSE;
static  BOOL    bPrtRegRpt = FALSE;
static  BOOL    bPrtCstRpt = FALSE;



USHORT EstablishArray (USHORT usNumArrayElements, USHORT usElementSize,
                       PVOID *pppStruct)
   {
   USHORT us;

   /* -- Recover memory from any previous call. */
   if (*pppStruct != NULL)
      ItiMemFree (hheap, *pppStruct);

   us = (usNumArrayElements + 1) * usElementSize;

   /* -- Get space for pointer array. */
   *pppStruct = ItiMemAlloc (hheap, us, MEM_ZERO_INIT);

   if (pppStruct == NULL)
      return (ITIRPT_ERR);

   return(0);
   }/* End of function */






USHORT AddToLNKList (PSZ  *ppsz,  USHORT usNth,
                     PPLNK pplnk, USHORT usArrayElementCount)

   /* Returns 1 if fund was added to the list, 0 if not or if error. */
   {
   USHORT usSize;
   USHORT us = 0;
   CHAR   szTmp[3] = "";


   if (ppsz == NULL)
      {
      DBGMSG("AddToLNKList parameter error.");
      return (0);
      }

   if (usNth > usArrayElementCount)
      {
      DBGMSG("AddToLNKList error; array element count exceeded.");
      return (0);
      }


   /* -- Get space for the structure. */
   usSize = sizeof(LNK);
   pplnk[usNth] = ItiMemAlloc(hheap, usSize, MEM_ZERO_INIT);
   if (pplnk[usNth] == NULL)
      {
      DBGMSG("ItiMem error in the AddToLNKList function of rStdIt DLL");
      return (0);
      }


   /* -- Get the text. */
   ItiStrCpy(pplnk[usNth]->szID,   ppsz[STD_ITEM_NUM], LEN_STRUCT_STR);
   ItiStrCpy(pplnk[usNth]->szKey,  ppsz[ITEM_KEY],     LEN_STRUCT_STR);
   ItiStrCpy(pplnk[usNth]->szLink, ppsz[LINK],         LEN_STRUCT_STR);
   ItiStrCpy(pplnk[usNth]->szLag,  ppsz[LAG],          LEN_STRUCT_STR);
   ItiStrCpy(pplnk[usNth]->szDesc, ppsz[SHORT_DESC],   LGARRAY+1);


   ItiStrToUSHORT(ppsz[ITEM_KEY], &(pplnk[usNth]->usKey));

   return (1);

   }/* END OF FUNCTION AddToLNKList */


 







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









USHORT LoadColumnTitles (VOID)
   {
   CHAR   szBuff [RPT_TTL_LEN+1] = "";
   USHORT us;
   USHORT usErr;


   /* -- Prep from meta files. */

   /* get the number of titles using ID_RPT_TTL! */
   usErr = ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_RPT_TTL,
                               szBuff, sizeof szBuff );
   ItiStrToUSHORT(szBuff, &usNumOfTitles);

   /* get the titles themselves, must start at 1. */
   for (us=1; us <= usNumOfTitles; us++)
      {
      usErr = ItiMbQueryQueryText(hmodModule, ITIRID_RPT, us,
                                  szBuff, sizeof szBuff );
      if (usErr != 0)
         break;

      /* Get the title column location. */
      ItiStrToUSHORT (ItiStrTok(szBuff, ", "), &acttl[us].usTtlCol);

      /* Get the title column width. */
      ItiStrToUSHORT (ItiStrTok(NULL,   ", "), &acttl[us].usTtlColWth);

      /* Get the data column location. */
      ItiStrToUSHORT (ItiStrTok(NULL, ", "), &acttl[us].usDtaCol);

      /* Get the data column width. */
      ItiStrToUSHORT (ItiStrTok(NULL,   ", "), &acttl[us].usDtaColWth);

      /* Get the column title. */
      ItiStrCpy(acttl[us].szTitle, ItiStrTok(NULL,","),
                sizeof acttl[us].szTitle);
      }

   return usErr;
   }/* END OF FUNCTION LoadColumnTitles */





USHORT SetColumnTitles (USHORT usFirstTitle, USHORT usLastTitle)
   /* -- Print the column titles. */
   {
   USHORT us;
   USHORT usErr;

      /* -- Actual printing. */
   for (us=usFirstTitle; us <= usLastTitle; us++)
      {
      if (acttl[us].usTtlColWth != 0)
         usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[us].usTtlCol,
                                  LEFT, (acttl[us].usTtlColWth | REL_LEFT),
                                  acttl[us].szTitle);

      /* Add line feeds and underscores when appropriate. */
      switch(us)
         {
         case RPT_TTL_3:  /* Short Description/  */
         case RPT_TTL_7:  /* Start at %Cmpl      */
            ItiPrtIncCurrentLn (); //line feed
            break;


         case RPT_TTL_8:  /* Long Description    */
            ItiPrtIncCurrentLn (); //line feed
            ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
            ItiPrtIncCurrentLn (); //line feed
            break;

         case RPT_TTL_14: /* Lag (for Successor) last one. */
            ItiPrtIncCurrentLn (); //line feed
            /* -- Underline Predecessors/ Link-Type Lag line. */
            ItiPrtDrawSeparatorLn (CURRENT_LN,
                                   acttl[RPT_TTL_9].usTtlCol,
                                   (acttl[RPT_TTL_11].usTtlCol
                                   +acttl[RPT_TTL_11].usTtlColWth) );

            /* -- Underline Successors/ Link-Type Lag line. */
            ItiPrtDrawSeparatorLn (CURRENT_LN,
                                   acttl[RPT_TTL_12].usTtlCol,
                                   (acttl[RPT_TTL_14].usTtlCol
                                   +acttl[RPT_TTL_14].usTtlColWth) );
            ItiPrtIncCurrentLn (); //line feed
            break;

         default:
            break;

         }/* end of switch */

      }/* end of for */


   return usErr;
   }/* END OF FUNCTION SetColumnTitles */







USHORT ConvertOrderingStr (PSZ pszOrderingStr)
   /* -- Converts OrderingStr from ColIDs into ListingQuery column value. */
   {
   switch (pszOrderingStr[0])
      {
      case ORDER_BY_ITEMNUM:
      default:
         ItiStrCat(pszOrderingStr, " order by StandardItemNumber ", LGARRAY);
         break;
      }/* end switch */

   return (0);
   }/* End of Function ConvertOrderingStr */




USHORT WriteDefaultUnitPrice (PSZ pszStdItemKey, PSZ pszUnit)
   {
   USHORT us    = 0;
   USHORT usCol = 0;
   USHORT usLen = 0;
   CHAR   szUnitStr[SMARRAY] = "";

   if ((pszStdItemKey == NULL) || (pszUnit == NULL))
      return (NO_KEY);


   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szBaseQueryDeflt, szConjStdItemKey, pszStdItemKey);

   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hheap, hmodModule, ITIRID_RPT,
                          ID_QUERY_DEFLT, &usNumCols, &usErr);
   
   /* ---------------------------------------------------------------- *\
    * Process the query results for each row.                          *
   \* ---------------------------------------------------------------- */
   if (hqry == NULL)
      {
      SETQERRMSG
      ItiPrtEndReport();
      }
   else
      { /* -- should only get ONE row back. */
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
         {
         us = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_26].usTtlCol,
                               LEFT,
                               (acttl[RPT_TTL_26].usTtlColWth | REL_LEFT),
                               acttl[RPT_TTL_26].szTitle);
      
         us = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_26].usDtaCol,
                               LEFT,
                               (acttl[RPT_TTL_26].usDtaColWth | REL_LEFT),
                               ppsz[UNIT_PRICE]);
      
         usCol = ItiPrtQueryCurrentColNum();
         szUnitStr[0] = ' ' ;
         if (ItiStrICmp(pszUnit, "EACH") != 0)
            {
            szUnitStr[1] = '/';
            szUnitStr[2] = ' ';
            szUnitStr[3] = '\0';
            }
         else
            szUnitStr[1] = '\0';

         ItiStrCat (szUnitStr, pszUnit, sizeof szUnitStr);
         usLen = ItiStrLen (szUnitStr);
         us = ItiPrtPutTextAt (CURRENT_LN, usCol,
                               LEFT,
                               ((usLen+1) | REL_LEFT),
                               szUnitStr);

         ItiPrtIncCurrentLn (); //line feed
         ItiPrtIncCurrentLn (); //blank line 


         ItiFreeStrArray (hheap, ppsz, usNumCols);

         }/* End of WHILE GetQueryRow for Listing query. */

      }/* else clause */

   return (us);
   }/* End of Function WriteDefaultUnitPrice */






USHORT WriteRowDataForSummarySection (PSZ *ppsz)
   {
   USHORT us   = 0;
   USHORT usLn = 0;
   USHORT usErr;

   /* -- Note that LONG_DESC+1 == RPT_TTL_8;  Refer to the met file.
    */
   usLn = (ItiStrLen(ppsz[LONG_DESC])/acttl[LONG_DESC+1].usDtaColWth) + 7;
   if (bInclDfltPrices)
      usLn = usLn + 4; /* space to write default unit prices. */

   if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
      {
      /* -- Set the Header's subtitle text. */
      ItiStrCpy(szSubTitle, SUB_TITLE, sizeof szSubTitle);
      ItiStrCat(szSubTitle, ppsz[SPEC_YEAR], sizeof szSubTitle);
      ItiPrtChangeHeaderLn(ITIPRT_SUBTITLE, szSubTitle);

      usCurrentPg = ItiPrtQueryCurrentPgNum();

      SetColumnTitles (RPT_TTL_2, RPT_TTL_8); 
      }

                   /* -- Only do the 1st eight in this function. */
   for (us=1; us <= RPT_TTL_8; us++)
      {
      switch(us)
         {
         case RPT_TTL_1:  /* SpecYear      not printed here. */
         case RPT_TTL_9:  /* Predecessors/ not printed here. */
         case RPT_TTL_10: /* Link-Type     not printed here. */
         case RPT_TTL_11: /* Lag           not printed here. */
         case RPT_TTL_12: /* Successors/   not printed here. */
         case RPT_TTL_13: /* Link-Type     not printed here. */
         case RPT_TTL_14: /* Lag           not printed here. */
            break;

         case RPT_TTL_6:                      /* Daily Prod. (NO line feed) */   
            usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[us].usDtaCol,
                                  RIGHT,
                                  (acttl[us].usDtaColWth | REL_LEFT),
                                  ppsz[us-1]);
            break;

         case RPT_TTL_7:                    /* Start at %Cmpl (w/line feed) */
            usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[us].usDtaCol,
                                  RIGHT,
                                  (acttl[us].usDtaColWth | REL_LEFT),
                                  ppsz[us-1]);
            ItiPrtIncCurrentLn (); //line feed
            break;


         case RPT_TTL_3:                              /* Short Description/ */
            usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[us].usDtaCol,
                                  LEFT,
                                  (acttl[us].usDtaColWth | REL_LEFT),
                                  ppsz[us-1]);
            ItiPrtIncCurrentLn (); //line feed
            break;


         case RPT_TTL_8:                               /* Long Description  */
            usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[us].usDtaCol,
                                  (LEFT | WRAP | LNINC),
                                  (acttl[us].usDtaColWth | REL_LEFT),
                                  ppsz[us-1]);
            ItiPrtIncCurrentLn (); // blank line 
            break;


         case RPT_TTL_5:
            usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[us].usDtaCol,
                                  LEFT,
                                  (acttl[us].usDtaColWth | REL_LEFT),
                                  ppsz[us-1]);
            break;
                  

         default:
            /* NO line feeds for these lines. */
            usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[us].usDtaCol,
                                  LEFT,
                                  (acttl[us].usDtaColWth | REL_LEFT),
                                  ppsz[us-1]);
            break;
         }/* end of switch */

      }/* end of for (us... */

   return (usErr);
   }/* End of Function WriteRowDataForSummarySection */





USHORT SummarySectionFor  (PSZ pszStdItemKey, PSZ pszUnit, USHORT pszUnitLen)
   {

   if ((pszStdItemKey == NULL) || (pszUnit == NULL))
      {
      return NO_KEY;
      }


   /* -- Build the query. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szBaseQuerySumSec, szConjStdItemKey, pszStdItemKey);


   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hheap, hmodModule, ITIRID_RPT,
                          ID_QUERY_LISTING, &usNumCols, &usErr);
   
   /* ---------------------------------------------------------------- *\
    * Process the query results for each row.                          *
   \* ---------------------------------------------------------------- */
   if (hqry == NULL)
      {
      SETQERRMSG
//      ItiPrtEndReport();
      }
   else
      { /* -- should only get ONE row back. */
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
         {
         /* ----------------------------------------------------------- *\
          * If we are on a new page...                                  *
         \* ----------------------------------------------------------- */
         if (ItiPrtQueryCurrentPgNum() != usCurrentPg )
            {
            /* -- Set the Header's subtitle text. */
            ItiStrCpy(szSubTitle, SUB_TITLE, sizeof szSubTitle);
            ItiStrCat(szSubTitle, ppsz[SPEC_YEAR], sizeof szSubTitle);
            ItiPrtChangeHeaderLn(ITIPRT_SUBTITLE, szSubTitle);
      
            /* -- Set the column titles for new pg.     */
            SetColumnTitles(RPT_TTL_2, RPT_TTL_8);
   
            usCurrentPg = ItiPrtQueryCurrentPgNum();
            }
   
         WriteRowDataForSummarySection (ppsz);

         /* -- Make a copy of the Unit code value for this item. */
         ItiStrCpy(pszUnit, ppsz[CODE_VALUE_ID], pszUnitLen);

         ItiFreeStrArray (hheap, ppsz, usNumCols);
   
         }/* End of WHILE GetQueryRow for Listing query. */


//      if (bInclDfltPrices)
//         {
//         WriteDefaultUnitPrice(pszStdItemKey, pszUnit);
//         }

      }/* else clause */


   return 0;
   }/* End of Function SummarySectionFor */







USHORT SummarySectionForDefaultPrices  (PSZ pszStdItemKey, PSZ pszUnit, USHORT pszUnitLen)
   {
   USHORT us = 0;
   USHORT usNumCols;
   PSZ *ppsz;

   if ((pszStdItemKey == NULL) || (pszUnit == NULL))
      {
      return NO_KEY;
      }


   /* -- Build the query. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szBaseQueryDefSumSec, szConjStdItemKey, pszStdItemKey);

   ppsz = ItiDbGetRow1 (szQuery, hheap, hmodModule, ITIRID_RPT,
                        ID_QUERY_DEF_LISTING, &usNumCols);
   
   /* ---------------------------------------------------------------- *\
    * Process the query results for each row.                          *
   \* ---------------------------------------------------------------- */
   if (ppsz != NULL)
      {
      /* ----------------------------------------------------------- *\
       * If we are on a new page...                                  *
      \* ----------------------------------------------------------- */
      if (ItiPrtQueryCurrentPgNum() != usCurrentPg )
         {
         /* -- Set the Header's subtitle text. */
         ItiStrCpy(szSubTitle, SUB_TITLE, sizeof szSubTitle);
         ItiStrCat(szSubTitle, ppsz[SPEC_YEAR], sizeof szSubTitle);
         ItiPrtChangeHeaderLn(ITIPRT_SUBTITLE, szSubTitle);
      
         /* -- Set the column titles for new pg.     */
         SetColumnTitles(RPT_TTL_2, RPT_TTL_8);
   
         usCurrentPg = ItiPrtQueryCurrentPgNum();
         }
   
      WriteRowDataForSummarySection (ppsz);

      WriteDefaultUnitPrice(pszStdItemKey, ppsz[CODE_VALUE_ID]);

      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }
   else
      { /* The given std. item key does not have a default price. */
      /* -- Build the query. */
      ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                        szBaseQuerySumSec, szConjStdItemKey, pszStdItemKey);

      /* -- Do the database query. */
      ppsz = ItiDbGetRow1 (szQuery, hheap, hmodModule, ITIRID_RPT,
                           ID_QUERY_LISTING, &usNumCols);
      
      /* ---------------------------------------------------------------- *\
       * Process the query results for each row.                          *
      \* ---------------------------------------------------------------- */
      if (ppsz != NULL)
         {
         /* ----------------------------------------------------------- *\
          * If we are on a new page...                                  *
         \* ----------------------------------------------------------- */
         if (ItiPrtQueryCurrentPgNum() != usCurrentPg )
            {
            /* -- Set the Header's subtitle text. */
            ItiStrCpy(szSubTitle, SUB_TITLE, sizeof szSubTitle);
            ItiStrCat(szSubTitle, ppsz[SPEC_YEAR], sizeof szSubTitle);
            ItiPrtChangeHeaderLn(ITIPRT_SUBTITLE, szSubTitle);
   
            /* -- Set the column titles for new pg.     */
            SetColumnTitles(RPT_TTL_2, RPT_TTL_8);
   
            usCurrentPg = ItiPrtQueryCurrentPgNum();
            }
   
         WriteRowDataForSummarySection (ppsz);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_26].usTtlCol,
                         LEFT,
                         USE_RT_EDGE,
                         "This Item does NOT have a default unit price.");

         ItiPrtIncCurrentLn (); //line feed
         ItiPrtIncCurrentLn (); //blank line

         ItiFreeStrArray (hheap, ppsz, usNumCols);
         }
      else
         return 13;

      }/* else clause */


   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   ItiPrtIncCurrentLn (); //line feed

   return 0;
   }/* End of Function SummarySectionForDefaultPrices */











USHORT PredSuccSectionFor (PSZ pszStdItemKey)
   {
   USHORT usNumPred = 0; /* total count */
   USHORT usNumSucc = 0;
   USHORT usPred = 0;    /* running count */
   USHORT usSucc = 0;
   USHORT us, usP, usS, usLnCnt;
   BOOL   bSecTtlSet;
   BOOL   bBlankLn  = FALSE;



   if (pszStdItemKey == NULL)
      {
      return NO_KEY;
      }


   /* ---------------------------------------------------------------- *\
    * -- Build the predecessor list.                                   *
   \* ---------------------------------------------------------------- */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szBaseQueryPrdLst, pszStdItemKey, szPrdOrderClause);


   usNumPred = ItiDbGetQueryCount(szQuery, &usNumCols, &usErr);

   if (usNumPred > 0)
      {
      /* -- Do the database query. */
      hqry = ItiDbExecQuery (szQuery, hheap, hmodModule, ITIRID_RPT,
                             ID_QUERY_PRED, &usNumCols, &usErr);
      
      /* -- Process the query results for each row. */
      if (hqry == NULL)
         {
         SETQERRMSG
//         ItiPrtEndReport();
         }
      else
         {
         EstablishArray (usNumPred+1, sizeof(LNK), &(PVOID)ppPred);
         usPred = 0;
         while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
            {
            usPred = AddToLNKList(ppsz, usPred, ppPred, usNumPred) + usPred;
            ItiFreeStrArray (hheap, ppsz, usNumCols);
      
            }/* End of WHILE GetQueryRow for Predecessor list query. */
         }/* else clause */
      }/* end if usNumPred... */
   


   /* ---------------------------------------------------------------- *\
    * -- Build the Successor list.                                     *
   \* ---------------------------------------------------------------- */
//   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_SUCC,
//                       szBaseQuerySucLst, sizeof szBaseQuerySucLst );
//
//   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_ORDER_SUCC,
//                       szSucOrderClause, sizeof szSucOrderClause);
//
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szBaseQuerySucLst,  pszStdItemKey, szSucOrderClause);


   usNumSucc = ItiDbGetQueryCount(szQuery, &usNumCols, &usErr);

   if (usNumSucc > 0)
      {
      /* -- Do the database query. */
      hqry = ItiDbExecQuery (szQuery, hheap, hmodModule, ITIRID_RPT,
                             ID_QUERY_SUCC, &usNumCols, &usErr);
      
      /* -- Process the query results for each row. */
      if (hqry == NULL)
         {
         SETQERRMSG
//         ItiPrtEndReport();
         }
      else
         {
         EstablishArray (usNumSucc+1, sizeof(LNK), &(PVOID)ppSucc);
         usSucc = 0;
         while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
            {
            usSucc = AddToLNKList(ppsz, usSucc, ppSucc, usNumSucc) + usSucc;
            ItiFreeStrArray (hheap, ppsz, usNumCols);
      
            }/* End of WHILE GetQueryRow for Successor list query. */
         }/* else clause */
      }/* end if usNumSucc... */



   /* -- If no Preds or Succs, then don't print anything.
    * -- Ref. Detailed Design, chapter 3, page 56.
    */
   if ((usNumPred == 0) && (usNumSucc == 0))
      {
//      ItiPrtIncCurrentLn (); //line feed
      return 0;
      }


   /* ---------------------------------------------------------------- *\
    * -- Write out the two lists.                                      *
   \* ---------------------------------------------------------------- */
   bSecTtlSet = FALSE;
   bBlankLn   = FALSE;
   usP = 0;
   usS = 0;
   usLnCnt = 0;
   while ((usP < usPred) || (usS < usSucc))
      {
      bBlankLn = TRUE;

      /* -- Check for new page. */
      us = ItiPrtKeepNLns(4);
      if (us == PG_WAS_INC)
         {
         usCurrentPg = ItiPrtQueryCurrentPgNum();
         SetColumnTitles (RPT_TTL_2, RPT_TTL_8);
         usErr = ItiPrtPutTextAt(CURRENT_LN,0,LEFT,(7 | REL_LEFT),"(cont.)");
         SetColumnTitles (RPT_TTL_9, RPT_TTL_14);
         usLnCnt = 0;
         }
      else if (!bSecTtlSet)
         {
         /* -- Set initial column titles for the Pred\Succ subsection. */
         SetColumnTitles (RPT_TTL_9, RPT_TTL_14);
         bSecTtlSet = TRUE;
         }

                                                  /* -- PREDECESSOR COLUMN. */ 
      if (usP < usPred)
         {
         /* NO line feeds for these lines. */
         usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_9].usDtaCol,
                               LEFT,
                               (acttl[RPT_TTL_9].usDtaColWth | REL_LEFT),
                               ppPred[usP]->szID);
         usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_10].usDtaCol,
                               LEFT,
                               (acttl[RPT_TTL_10].usDtaColWth | REL_LEFT),
                               ppPred[usP]->szLink);
         usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_11].usDtaCol,
                               RIGHT,
                               (acttl[RPT_TTL_11].usDtaColWth | REL_LEFT),
                               ppPred[usP]->szLag);

         usErr = ItiPrtPutTextAt ((ItiPrtQueryCurrentLnNum() +1),
                               acttl[RPT_TTL_15].usDtaCol,
                               LEFT,
                               (acttl[RPT_TTL_15].usDtaColWth | REL_LEFT),
                               ppPred[usP]->szDesc);
         usP++;
         }/* end of then */


                                                    /* -- SUCCESSOR COLUMN. */
      if (usS < usSucc)
         {
         /* NO line feeds for these lines. */
         usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usDtaCol,
                               LEFT,
                               (acttl[RPT_TTL_12].usDtaColWth | REL_LEFT),
                               ppSucc[usS]->szID);
         usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_13].usDtaCol,
                               LEFT,
                               (acttl[RPT_TTL_13].usDtaColWth | REL_LEFT),
                               ppSucc[usS]->szLink);
         usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_14].usDtaCol,
                               RIGHT,
                               (acttl[RPT_TTL_14].usDtaColWth | REL_LEFT),
                               ppSucc[usS]->szLag);
         usErr = ItiPrtPutTextAt ((ItiPrtQueryCurrentLnNum() +1),
                               acttl[RPT_TTL_16].usDtaCol,
                               LEFT,
                               (acttl[RPT_TTL_16].usDtaColWth | REL_LEFT),
                               ppSucc[usS]->szDesc);
         usS++;
         }


      ItiPrtIncCurrentLn (); //line feed, link\lag line
      ItiPrtIncCurrentLn (); //line feed, short desc. line
      usLnCnt++;

      if (usLnCnt == BLANK_LINE_INTERVAL)
         {
         ItiPrtIncCurrentLn (); //blank line
         usLnCnt = 0;
         }

      }/* end of while usP... */

   if (bBlankLn)
      ItiPrtIncCurrentLn (); //blank line

   return 0;
   }/* End of Function PredSuccSectionFor */







USHORT YieldSectionFor (PSZ pszStdItemKey, PSZ pszUnit)
   {
   USHORT us, usErr;
   CHAR szTextBuff[30] = "$1.00 ";
   CHAR szTextTail[30] = "";
   BOOL bDoSection= TRUE;
   BOOL bDoSumPct = FALSE;
   BOOL bBlankLn  = FALSE;


//   ItiPrtIncCurrentLn (); //blank line 

   /* ---------------------------------------------------------------- *\
    * -- PctCst section.                                               *
   \* ---------------------------------------------------------------- */
//   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_PCT_MAJ,
//                       szBaseQueryPctCst, sizeof szBaseQueryPctCst );

   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szBaseQueryPctCst, pszStdItemKey, " ");

   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hheap, hmodModule, ITIRID_RPT,
                          ID_QUERY_PCT_MAJ, &usNumCols, &usErr);
   
   /* -- Process the query results for each row. */
   if (hqry == NULL)
      {
      SETQERRMSG 
//      ItiPrtEndReport();
      }
   else
      {
      bDoSection = TRUE;
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
         {
         if (bDoSection)
            {
            /* -- Check for new page at section init. */
            if (ItiPrtKeepNLns(6) == PG_WAS_INC)
               {
               usCurrentPg = ItiPrtQueryCurrentPgNum();
               SetColumnTitles (RPT_TTL_2, RPT_TTL_8);
               usErr = ItiPrtPutTextAt(CURRENT_LN,0,LEFT,(7 | REL_LEFT),"(cont.)");
               }

            ItiStrCat(szTextBuff, acttl[RPT_TTL_25].szTitle, sizeof szTextBuff);
            usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_9].usDtaCol,
                                  LEFT,
                                  (acttl[RPT_TTL_17].usDtaCol - 1),
                                  szTextBuff);
            bDoSection = FALSE;
            bDoSumPct = TRUE;
            }

         /* -- Check for new page. */
         if (ItiPrtKeepNLns(3) == PG_WAS_INC)
            {
            usCurrentPg = ItiPrtQueryCurrentPgNum();
            SetColumnTitles (RPT_TTL_2, RPT_TTL_8);
            usErr = ItiPrtPutTextAt(CURRENT_LN,0,LEFT,(7 | REL_LEFT),"(cont.)");
            }

         usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_17].usDtaCol,
                              RIGHT,
                              ((acttl[RPT_TTL_17].usDtaColWth +1) | REL_LEFT),
                              ppsz[PCT_OF_COST]);

         usErr = ItiPrtPutTextAt (CURRENT_LN,
                                  acttl[RPT_TTL_24].usTtlCol,
                                  LEFT, (3 | REL_LEFT), "of ");

         usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_18].usDtaCol,
                               LEFT,
                               (acttl[RPT_TTL_18].usDtaColWth | REL_LEFT),
                               ppsz[MAJ_ITEM_ID]);

         usErr = ItiPrtPutTextAt (CURRENT_LN,
                 (acttl[RPT_TTL_18].usDtaCol + acttl[RPT_TTL_18].usDtaColWth),
                               LEFT, (3 | REL_LEFT), " - ");

         usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_19].usDtaCol,
                               LEFT,
                               (acttl[RPT_TTL_19].usDtaColWth | REL_LEFT),
                               ppsz[DESC]);

         ItiPrtIncCurrentLn (); //line feed
         ItiFreeStrArray (hheap, ppsz, usNumCols);
         }/* End of WHILE GetQueryRow */

      }/* else clause */
   



   /* ---------------------------------------------------------------- *\
    * -- SumPct section.                                               *
   \* ---------------------------------------------------------------- */
//   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_SUM_PCT,
//                       szBaseQuerySumPct, sizeof szBaseQuerySumPct);
//
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szBaseQuerySumPct, pszStdItemKey, " ");

   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hheap, hmodModule, ITIRID_RPT,
                          ID_QUERY_SUM_PCT, &usNumCols, &usErr);
   
   /* -- Process the query results for each row. */
   if (hqry == NULL)
      {
      SETQERRMSG 
      ItiPrtEndReport();
      }
   else
      {
      us = 1;
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
         {
         if ((us == 1) && bDoSumPct )
            {
            ItiPrtDrawSeparatorLn (CURRENT_LN, acttl[RPT_TTL_17].usDtaCol,
              (acttl[RPT_TTL_17].usDtaCol + acttl[RPT_TTL_17].usDtaColWth));
            ItiPrtIncCurrentLn (); //line feed

            usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_17].usDtaCol,
                              RIGHT,
                              ((acttl[RPT_TTL_17].usDtaColWth +1) | REL_LEFT),
                              ppsz[SUM_PCT]);

            usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_24].usTtlCol,
                                  LEFT,
                                  (acttl[RPT_TTL_24].usTtlColWth | REL_LEFT),
                                  acttl[RPT_TTL_24].szTitle);

            ItiPrtIncCurrentLn (); //line feed
            ItiPrtIncCurrentLn (); //blank line 
            }

         us++;
         ItiFreeStrArray (hheap, ppsz, usNumCols);

         }/* End of WHILE GetQueryRow */

      }/* else clause */
   








   /* ---------------------------------------------------------------- *\
    * -- Quantity Conversion Factor section.                           *
   \* ---------------------------------------------------------------- */
//   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_MAJ_QCF,
//                       szBaseQueryQCF, sizeof szBaseQueryQCF);

   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szBaseQueryQCF, pszStdItemKey, " ");

   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hheap, hmodModule, ITIRID_RPT,
                          ID_QUERY_MAJ_QCF, &usNumCols, &usErr);
   
   /* -- Process the query results for each row. */
   if (hqry == NULL)
      {
      SETQERRMSG 
      ItiPrtEndReport();
      }
   else
      {
      bDoSection = TRUE;
      bBlankLn = FALSE;
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
         {
         if (bDoSection)
            {
            /* -- Check for new page. */
            if (ItiPrtKeepNLns(3) == PG_WAS_INC)
               {
               usCurrentPg = ItiPrtQueryCurrentPgNum();
               SetColumnTitles (RPT_TTL_2, RPT_TTL_8);
               usErr = ItiPrtPutTextAt(CURRENT_LN, 0, LEFT,
                                       (7 | REL_LEFT), "(cont.)");
               }

            szTextBuff[0] = '1';
            szTextBuff[1] = ' ';
            szTextBuff[2] = '\0';
            ItiStrCat(szTextBuff, pszUnit, sizeof szTextBuff);
            ItiStrCat(szTextBuff, " ", sizeof szTextBuff);
            ItiStrCat(szTextBuff,
                      acttl[RPT_TTL_25].szTitle, sizeof szTextBuff);
            usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_9].usDtaCol,
                                  LEFT,
                                  (acttl[RPT_TTL_20].usDtaCol - 1),
                                  szTextBuff);
            bDoSection = FALSE;
            bBlankLn = TRUE;
            }


         usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_20].usDtaCol,
                               RIGHT,
                               (acttl[RPT_TTL_20].usDtaColWth | REL_LEFT),
                               ppsz[QCF]);

         usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_21].usDtaCol,
//                              RIGHT,
                               LEFT,
                               (acttl[RPT_TTL_21].usDtaColWth | REL_LEFT),
                               ppsz[CVID]);

         usErr = ItiPrtPutTextAt (CURRENT_LN,
                              (CURRENT_COL+1),
                               LEFT, (4 | REL_LEFT), " of ");

         usErr = ItiPrtPutTextAt (CURRENT_LN,
                              (CURRENT_COL + 1),//acttl[RPT_TTL_22].usDtaCol,
                               LEFT,
                               (acttl[RPT_TTL_22].usDtaColWth | REL_LEFT),
                               ppsz[MAJID]);

         usErr = ItiPrtPutTextAt (CURRENT_LN,
                              (CURRENT_COL + 1),
             // (acttl[RPT_TTL_22].usDtaCol + acttl[RPT_TTL_22].usDtaColWth),
                               LEFT, (3 | REL_LEFT), " - ");

         usErr = ItiPrtPutTextAt (CURRENT_LN,
                              (CURRENT_COL + 1),//acttl[RPT_TTL_23].usDtaCol,
                               LEFT,
                               (acttl[RPT_TTL_23].usDtaColWth | REL_LEFT),
                               ppsz[IDDESC]);

         ItiPrtIncCurrentLn (); //line feed
         ItiFreeStrArray (hheap, ppsz, usNumCols);
         }/* End of WHILE GetQueryRow */

      if (bBlankLn)
         ItiPrtIncCurrentLn (); //blank line

      }/* else clause */
   


   /* -- Finishout this section. */
   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   ItiPrtIncCurrentLn (); //line feed

   return 0;
   }/* End of Function YieldSectionFor */





USHORT BuildSpecYrKeyFile (PSZ pszSpecYr)
/* -- This function builds a file listing all of the standard item keys
 * -- for a given spec year.
 */
   {
   HFILE hAltFile;

   ItiRptUtUniqueFile (szKeyFileName, sizeof szKeyFileName, TRUE);

   /* -- Build the key list query. */
   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_ITEM_KEYS,
                       szBaseQuery, sizeof szBaseQuery );

   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szBaseQuery, " ", pszSpecYr);


   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hheap, hmodModule, ITIRID_RPT,
                          ID_QUERY_ITEM_KEYS, &usNumCols, &usErr);
   
   /* ---------------------------------------------------------------- *\
    * Process the query results for each row.                          *
   \* ---------------------------------------------------------------- */
   if (hqry == NULL)
      {
      SETQERRMSG 
      }                     
   else
      {
      ItiRptUtStartKeyFile (&hAltFile, szKeyFileName, sizeof szKeyFileName);

      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )
         {
         ItiRptUtWriteToKeyFile (hAltFile, ppsz[KEY_STD_ITEM], NULL);
         ItiFreeStrArray (hheap, ppsz, usNumCols);
         }/* End of WHILE GetQueryRow for Listing query. */

      ItiRptUtEndKeyFile (hAltFile);
      }/* else clause */


   return 0;
   }/* End of Function BuildSpecYrKeyFile */






USHORT PrintProc (HWND hwnd, MPARAM mp1, MPARAM mp2, USHORT usCaller_ID)
   {
//   SHORT  sIdx;
   USHORT us;
//   USHORT usRow;
//   USHORT usTxtLen;
   HHEAP  hhp  = NULL;
   CHAR   szKeyStr   [SMARRAY+1] = "";
   CHAR   szFName    [SMARRAY+1] = "";
   PSZ    pszBuffer = &szBuffer[0];
   CHAR   szOptSwInclDflt [SMARRAY+1] = "";
   CHAR   szOptSwSpecYr   [SMARRAY+1] = "";
   CHAR   szOptSwBaseDate [SMARRAY+1] = "";
   CHAR   szOptSwSlctFile [4*SMARRAY+1] = "";
   CHAR   aszCmdLns[MAX_CMDLNS+1][MAX_LEN_CMDLN+1];


   hhp = ItiMemCreateHeap (MIN_HEAP_SIZE);

   /* -- Init the cmdlns. */
   for(us=0; us<=MAX_CMDLNS; us++)
      aszCmdLns[us][0] = '\0';


   /* -- Check if we are to do selected items (if so a keyfile is built) *
    * -- or all items for a selected spec year.                          */
   bUseSelectedOnly = (1 == (USHORT)(ULONG)WinSendDlgItemMsg (hwnd, 
                                 DID_SELECTED,
                                 BM_QUERYCHECK, 0L, 0L));
   if (bUseSelectedOnly)
      {
      WinSetDlgItemText(hwnd, DID_DATEEDIT, "Selected");


      if ((usCaller_ID == DefaultUnitPriceS)
           || (usCaller_ID == DefaultUnitPriceCatL)
           || (usCaller_ID == DefaultUnitPriceCatS) )
         {
         us = ItiRptUtBuildSelectedKeysFile(hwnd,
                                            DefaultUnitPriceCatL,
                                            cStandardItemKey,
                                            szFName, sizeof szFName);
         }

      if ((us == ITIRPTUT_PARM_ERROR)|| (us == ITIRPTUT_ERROR))
         {
         WinSendDlgItemMsg(hwnd, DID_SELECTED+DID_STOFFSET,
                           BM_SETCHECK, UNCHECKED, 0L);

         szErrorMsg[0] = DEV_MSG_CHAR;
         szErrorMsg[1] = '\0';
         ItiStrCat (szErrorMsg, "Error selecting keys, action aborted.",
                    sizeof szErrorMsg);
         ItiRptUtErrorMsgBox (hwnd, szErrorMsg);
         ItiMemDestroyHeap(hhp);
         return (ITIRPTUT_PARM_ERROR);
         }
      else
         {/* -- Make the file spec option. */
         ItiStrCpy(szOptSwSlctFile,
                   ITIRPT_OPTSW_KEYFILE, sizeof szOptSwSlctFile);
         ItiStrCat(szOptSwSlctFile,
                   szFName,              sizeof szOptSwSlctFile);

         us = 0;
         ItiStrCat(aszCmdLns[us], szOptSwSlctFile, sizeof aszCmdLns[us]);
         }

      }
   else
      {
      us = 0;

      ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_DEF_ITEM_KEYS,
                          szBaseQuery, sizeof szBaseQuery );

      ItiStrCpy(szQuery, " \"" ITIRPTUT_KEY_QUERY_SW_STR,(QUERY_ARRAY_SIZE + 1));
      ItiStrCat(szQuery, szBaseQuery, (QUERY_ARRAY_SIZE + 1));

      ItiStrCat(aszCmdLns[us], ITIRPT_OPTSW_KEYFILE,
                sizeof aszCmdLns[us]);
      
      ItiStrCat(aszCmdLns[us], szQuery, sizeof aszCmdLns[us]);
      ItiStrCat(aszCmdLns[us], " \"", sizeof aszCmdLns[us]);

      }/* end of if (bUseSelectedOnly... */




   /* ------------------------------------------------------- *\
   \* -- [4] Exec the cmd lns.                                */
   us = 0;
   while(us < MAX_CMDLNS) 
      {
      if (aszCmdLns[us][0] != '\0')
         ItiRptUtExecRptCmdLn (hwnd, aszCmdLns[us], aszCmdLns[us]);
      us++;
      }



   ItiRptUtCloseDlgBox (hwnd, NULL);

   ItiMemDestroyHeap(hhp);
   return (us > 0) ? 0 : 1;

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

//   if (usCallerVersion <= DLL_VERSION)
      return (TRUE);
//   return (FALSE);
   }








VOID EXPORT ItiDllQueryMenuName (PSZ     pszBuffer,   
                                 USHORT  usMaxSize,   
                                 PUSHORT pusWindowID) 
   {                                                  
   ItiStrCpy (pszBuffer, "~Default Unit Prices", usMaxSize);   
   *pusWindowID = rStdIt_RptDlgBox;                       
   }/* END OF FUNCTION */



BOOL EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID, 
                                     USHORT usActiveWindowID) 
   {                                                          
   if ((usTargetWindowID == rStdIt_RptDlgBox)
       && ( (usActiveWindowID == StandardItemCatL)
          ||(usActiveWindowID == StandardItemCatS)
          ||(usActiveWindowID == DefaultUnitPriceCatL)   
          ||(usActiveWindowID == DefaultUnitPriceCatS) 
          ||(usActiveWindowID == DefaultUnitPriceS)     ))    

      return TRUE;                                            
   else                                                       
      return FALSE;                                           
   }/* END OF FUNCTION */




MRESULT EXPORT ItiDllrDefUPRptDlgProc (HWND   hwnd,     
                                       USHORT usMessage,
                                       MPARAM mp1,
                                       MPARAM mp2)
   {
   static USHORT usCallerID;
   USHORT us = 0;
   HWND   hwndCaller = NULL;

   switch (usMessage)
      {
      case WM_INITDLG:
         usCallerID = 0;
         bUseSelectedOnly = FALSE;
         us = ItiRptUtInitDlgBox (hwnd, RPT_SESSION, 0, NULL);
         WinSetDlgItemText(hwnd, DID_DATEEDIT, "Loading");
         break;


      case WM_INITQUERY:
         us = ItiRptUtChkDlgMsg (hwnd, usMessage, mp1, mp2);

         hwndCaller = (HWND)(ULONG)WinSendMsg (hwnd, WM_ITIWNDQUERY,
                                           MPFROMSHORT(ITIWND_OWNERWND), 0L);
         usCallerID = (USHORT)(ULONG)WinSendMsg (hwndCaller, WM_ITIWNDQUERY,
                                                 MPFROMSHORT(ITIWND_ID), 0L);
         break;


      case WM_MOUSEMOVE:
         us = ItiRptUtChkDlgMsg (hwnd, usMessage, mp1, mp2);

         if (1 == (USHORT)(ULONG)WinSendDlgItemMsg (hwnd, 
                                             DID_SELECTED+DID_STOFFSET,
                                             BM_QUERYCHECK, 0L, 0L) )
            {
            WinSetDlgItemText(hwnd, DID_DATEEDIT, "Selected");
            }
         else
            WinSetDlgItemText(hwnd, DID_DATEEDIT, szDate);

         WinSetDlgItemText(hwnd, DID_BASEEDIT, szBaseDate);
         break;



      case WM_QUERYDONE:
         us = ItiRptUtChkDlgMsg (hwnd, usMessage, mp1, mp2);
         szDate[0] = '\0';
         WinSetDlgItemText(hwnd, DID_DATEEDIT, szDate);
         WinSetDlgItemText(hwnd, DID_BASEEDIT, szBaseDate);
         break;



      case WM_CONTROL:
         if ((USHORT) SHORT2FROMMP(mp1) != BN_CLICKED)
            break;

         switch ((USHORT) SHORT1FROMMP (mp1))
            {
            /* -- Show on screen whether selected or spec year. */
            case (DID_SELECTED+DID_STOFFSET):
               us = (USHORT)(ULONG)WinSendDlgItemMsg (hwnd, 
                                             DID_SELECTED+DID_STOFFSET,
                                             BM_QUERYCHECK, 0L, 0L);
               bUseSelectedOnly = (us == 1);
               if (bUseSelectedOnly)
                  {
                  /* Save any text in the dropbox. */
                  us =  WinQueryDlgItemText(hwnd, DID_DATEEDIT,
                                            sizeof szDate, szDate);
                  WinSetDlgItemText(hwnd, DID_DATEEDIT, "Selected");
                  }
               else
                  { /* -- Erase the combobox's display. */
                  szDate[0] = '\0';
                  WinSetDlgItemText(hwnd, DID_DATEEDIT, szDate);
                  }
               break;

                                                        
            case (DID_DATEEDIT):
               us = WinQueryDlgItemText(hwnd, DID_DATEEDIT,
                                        sizeof szDate, szDate);
               break;

                                                        
            case (DID_BASEEDIT):
               us = WinQueryDlgItemText(hwnd, DID_BASEEDIT,
                                        sizeof szBaseDate, szBaseDate);
               break;


            default:
               if (bUseSelectedOnly)
                  {
                  WinSetDlgItemText(hwnd, DID_DATEEDIT, "Selected");
                  }
               else
                  {
                  us = WinQueryDlgItemText(hwnd, DID_DATEEDIT,
                                         sizeof szDate, szDate);
                  us = WinQueryDlgItemText(hwnd, DID_BASEEDIT,
                                         sizeof szBaseDate, szBaseDate);
                  }
            } /* end of switch */

         return 0L;
         break;




      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_PRINT:
               WinSetPointer (HWND_DESKTOP,
                           WinQuerySysPointer (HWND_DESKTOP, SPTR_WAIT, 0));

               ItiRptUtCloseDlgBox (hwnd, NULL);

               us = PrintProc(hwnd, mp1, mp2, usCallerID);

               WinSetPointer (HWND_DESKTOP,
                           WinQuerySysPointer (HWND_DESKTOP, SPTR_ARROW, 0));

               if (us == 0)
                  WinDismissDlg (hwnd, TRUE);
               return 0L;
               break;


            case DID_CANCEL:
               WinDismissDlg (hwnd, FALSE);
               return 0L;
               break;


            case DID_HELP:
               break;

            }/* end of switch(SHORT1... */

         break;


      default:
         us = ItiRptUtChkDlgMsg (hwnd, usMessage, mp1, mp2);
      }


   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);

   }/* END OF FUNCTION ItiDllrDefUPRptDlgProc */








USHORT EXPORT ItiRptDLLPrintReport
                 (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[])
   {
   PSZ     pszKey = szKey;
   USHORT  usKeyFlag = 0;

           /* General return code variable: */
   USHORT  us = 0;

           /* Page formatting location variables: */
   USHORT  usLn  = 1;
   USHORT  usCol = 0;
   LONG    lColWidthPgUnits = 0L;
   INT     iBreakLnCnt = 0;      /* line counter */
   BOOL    bTitlesSet  = FALSE;
//   BOOL    bDoFullRpt;


           /* Title strings for this module. */
   REPTITLES rept;
   PREPT     prept = &rept;



   ghab = hab;

   /* ------------------------------------------------------------------- *\
    *  Create a memory heap.                                              *
   \* ------------------------------------------------------------------- */
   hheap = ItiMemCreateHeap (MIN_HEAP_SIZE);


   /* ------------------------------------------------------------------- *\
    * Initialize the strings and options.                                 *
   \* ------------------------------------------------------------------- */
   ItiStrCpy(szCurrentID,"TABLE_ID",sizeof(szCurrentID));


   /* -- Get any query ordering information, BEFORE setting rpt titles. */
   ItiRptUtGetOption (ITIRPTUT_ORDERING_SWITCH, szOrderingStr, LGARRAY,
                      argcnt, argvars);
   ConvertOrderingStr (szOrderingStr);


   ItiRptUtGetOption (OPT_SW_SPEC_YR, szSpecYear, sizeof szSpecYear,
                      argcnt, argvars);

//   ItiRptUtGetOption (OPT_SW_BASEDATE, szBaseDate, sizeof szBaseDate,
//                      argcnt, argvars);


//   ItiRptUtGetOption (OPT_SW_SUMMARY_ONLY, szOnly, sizeof szOnly,
//                      argcnt, argvars);
//   if (szOnly[0] == '\0')
//      /* ...then do full report (default case) */
//      bDoFullRpt = TRUE;
//   else
//      bDoFullRpt = FALSE;
//
//
//   ItiRptUtGetOption (OPT_SW_INCL_DFLT_PRICES, szDfltPrc, sizeof szDfltPrc,
//                      argcnt, argvars);
//   if (szDfltPrc[0] == '\0') 
//      bInclDfltPrices = FALSE;
//   else
//      {
      bInclDfltPrices = TRUE;
//      }


   /* -- Get the report's titles for this DLL. */
   LoadColumnTitles ();

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



   /* -------------------------------------------------------------------- *\
    * -- Load the base queries.                                            *
   \* -------------------------------------------------------------------- */
   /* -- Summary section. */
   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_LISTING,
                       szBaseQuerySumSec, sizeof szBaseQuerySumSec);

   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_DEF_LISTING,
                       szBaseQueryDefSumSec, sizeof szBaseQueryDefSumSec);

   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_CONJ_STDITEMKEY,
                       szConjStdItemKey, sizeof szConjStdItemKey);


   /* -- Predecessor list. */
   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_PRED,
                       szBaseQueryPrdLst, sizeof szBaseQueryPrdLst);

   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_ORDER_PRED,
                       szPrdOrderClause, sizeof szPrdOrderClause);


   /* -- Successor list. */
   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_SUCC,
                       szBaseQuerySucLst, sizeof szBaseQuerySucLst);

   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_ORDER_SUCC,
                       szSucOrderClause, sizeof szSucOrderClause);


   /* -- Percent Cost. */
   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_PCT_MAJ,
                       szBaseQueryPctCst, sizeof szBaseQueryPctCst);


   /* -- Sum Percent. */
   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_SUM_PCT,
                       szBaseQuerySumPct, sizeof szBaseQuerySumPct);


   /* -- Quantity Conversion Factor. */
   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_MAJ_QCF,
                       szBaseQueryQCF, sizeof szBaseQueryQCF);


   /* -- Default unit prices. */
   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_DEFLT,
                       szBaseQueryDeflt, sizeof szBaseQueryDeflt);




   /* -- Prepare the subtitle with the Spec Year, AFTER setting rpt titles. */
   if (szSpecYear[0] != '\0')
      {
      /* -- Alter the header's subtitle. */
      ItiStrCpy(szSubTitle, SUB_TITLE,  sizeof szSubTitle);
      ItiStrCat(szSubTitle, szSpecYear, sizeof szSubTitle);
      ItiPrtChangeHeaderLn (ITIPRT_SUBTITLE,   szSubTitle);

      /* -- Build an all keys file for the spec year. */
      BuildSpecYrKeyFile(szSpecYear);
      usArgCnt = 2;
      pAltArgVar = apszAlt;
      }
   else
      {
      usArgCnt   = argcnt;
      pAltArgVar = (PSZ *)argvars;
      }


   /* -------------------------------------------------------------------- *\
    * -- For each StandardItemKey...                                       *
   \* -------------------------------------------------------------------- */
   ItiRptUtInitKeyList(usArgCnt, (CHAR **)pAltArgVar); /*  */
   while ( (usKeyFlag = ItiRptUtGetNextKey(pszKey,sizeof(szKey)) )
                                               != ITIRPTUT_NO_MORE_KEYS)
      {
      us = SummarySectionForDefaultPrices(szKey, szUnit, sizeof szUnit);


      }/* End of WHILE GetNextKey */


   /* ------------------------------------------------------------------- *\
    * Finish up and quit the Standard Item Listing Base Report.           *
   \* ------------------------------------------------------------------- */
   ItiPrtEndReport();


   DBGMSG("Exit ***DLL function StdItem ItiRptDLLPrintReport");

   return 0;

   }/* END OF FUNCTION ItiRptDLLPrintReport */
  
   
  
   
   
  


