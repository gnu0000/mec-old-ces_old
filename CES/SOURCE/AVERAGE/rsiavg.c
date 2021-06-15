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


/*  CES rSIAvg Report DLL Source Code
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


#include    "rindllav.h"


#include "rSIAvg.h"                                       



/* ======================================================================= *\
 *                                         MANIFEST CONSTANT DEFINITIONS   *
\* ======================================================================= */
#define  BLANK_LINE_INTERVAL    4
#define  PRT_Q_NAME             "PEMETH StdItem Avgs"

#define  QUERY_ARRAY_SIZE       1023

#define  SIZE_OF_QUERY_RPT_DESC QUERY_ARRAY_SIZE

#define  NO_KEY   13

#define SETQERRMSG ItiStrCpy (szErrorMsg,                                    \
                              "ERROR: Query error for ",                     \
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
static  CHAR  szCompileDateTime[] = __DATE__ " " __TIME__ ;
static CHAR szDllVersion[] = "1.1a0 rSIAvg.dll";

static  CHAR  szCurrentID   [SMARRAY + 1] = "";
static  CHAR  szOrderingStr [LGARRAY + 1] = "";

static  CHAR  szQueryListing [QUERY_ARRAY_SIZE + 1];
static  CHAR  szQueryListingConj [QUERY_ARRAY_SIZE + 1];
static  CHAR  szQueryPrices  [QUERY_ARRAY_SIZE + 1];
static  CHAR  szPriceOrder   [2 * SMARRAY + 1] = "";
static  CHAR  szPricesBaseDate [4 * SMARRAY+1] = "";
static  CHAR  szListingOrder [2 * SMARRAY + 1] = "";
static  CHAR  szQueryNumber  [QUERY_ARRAY_SIZE + 1];
static  CHAR  szQuery        [QUERY_ARRAY_SIZE + 1];
static  CHAR  szKey          [SMARRAY + 1];

static  CHAR  szBaseDate    [SMARRAY + 1] = "";
static  CHAR  szTmpBaseDate [SMARRAY + 1] = "";


static  USHORT usKeyFlag    = 0;

static  CHAR   szFailedDLL [CCHMAXPATH + 1];
static  CHAR   szErrorMsg [1024] = "";

static  USHORT usNumOfTitles;


           /* Page formatting location variables: */
static   USHORT  usLn  = 1;
static   USHORT  usCol = 0;
static   USHORT  usCurrentPg = 0;
static   LONG    lColWidthPgUnits = 0L;
static   BOOL    bTitlesSet  = FALSE;
static   BOOL    bDataFound  = FALSE;

           /* Used by DbExecQuer & DbGetQueryRow functions: */
static   HHEAP   hheap  = NULL;
static   HQRY    hqry   = NULL;
static   USHORT  usResId   = ITIRID_RPT;
static   USHORT  usId      = ID_QUERY_LISTING;
static   USHORT  usNumCols = 0;
static   USHORT  usErr     = 0;
static   PSZ   * ppsz   = NULL;

           /* Title strings for this module. */
static   REPTITLES rept;
static   PREPT     prept = &rept;





typedef struct _ColTitles
           {
           CHAR    szTitle [RPT_TTL_LEN+1];
           USHORT  usTtlCol;
           USHORT  usTtlColWth;
           USHORT  usDtaCol;   
           USHORT  usDtaColWth;   
           } COLTITLES; /* cttl */

static  COLTITLES  acttl[RPT_TTL_NUM + 1];
                



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

   /* get the number of titles. */
   usErr = ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_RPT_TTL,
                               szBuff, sizeof szBuff );
   ItiStrToUSHORT(szBuff, &usNumOfTitles);

   /* get the titles themselves. */
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






USHORT SummarySectionFor  (PSZ pszStdItemKey)
   {
   USHORT us;
   PSZ    pszEnv = NULL;
   PSZ   *ppszSum= NULL;


   if (pszStdItemKey == NULL)
      {
      return NO_KEY;
      }

   /* -- Build the query. */
/* // */    ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
/* // */                      szQueryListing, szBaseDate, szQueryListingConj);
//        Note: if szbasedate used then in the met file the last line
//              of ID_QUERY_LISTING must be uncommented.

//   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
//                     szQueryListing, " ", szQueryListingConj);

   ItiStrCat (szQuery, pszStdItemKey,  sizeof szQuery);
   ItiStrCat (szQuery, szListingOrder, sizeof szQuery);


   ppszSum = ItiDbGetRow1 (szQuery, hheap, hmodModule, ITIRID_RPT,
                        ID_QUERY_LISTING, &usNumCols);

   /* -- Prepare the page. */
   if ((ItiPrtKeepNLns(11) == PG_WAS_INC)
       || (ItiPrtQueryCurrentPgNum() != usCurrentPg ))
         {
         usCurrentPg = ItiPrtQueryCurrentPgNum();
         ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
         }


   if (ppszSum == NULL) /* then no pemeth data exists for this StdItem key. */
      {
      /* -- This section will include selected items that 
         -- did not have PEMETH data in the database.     */
      DosScanEnv(ITEMS_NO_PEMETH_DATA, &pszEnv);
      if ((pszEnv != NULL) && ((pszEnv[0] == 'Y') || (pszEnv[0] == 'y')) )
         {
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_1].usTtlCol,
                          LEFT, (acttl[RPT_TTL_1].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_1].szTitle);

         ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                           szQueryNumber, " ", pszStdItemKey);

         ppszSum = ItiDbGetRow1 (szQuery, hheap, hmodModule, ITIRID_RPT,
                              ID_QUERY_NUMBER, &usNumCols);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_1].usDtaCol,
                          LEFT, (acttl[RPT_TTL_1].usDtaColWth | REL_LEFT),
                          ppszSum[STD_ITEM_NUM]);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_16].usTtlCol,
                          LEFT, (acttl[RPT_TTL_16].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_16].szTitle);
         ItiPrtIncCurrentLn();
         }

      if ((pszEnv != NULL) && ((pszEnv[0] == 'T') || (pszEnv[0] == 't')) )
         {
         ItiStrCpy (szErrorMsg, " TRACE MSG: No PEMETH Average data for key: ", sizeof szErrorMsg);
         ItiStrCat (szErrorMsg, pszStdItemKey, sizeof szErrorMsg);
         ItiRptUtErrorMsgBox (NULL, szErrorMsg);
         }

      pszStdItemKey[0] = '\0';
      return 13;
      }

//   if (usNumCols != NUM_COLS_LISTING)
//      {
//      DosBeep(1400,70);
//      DosBeep(1600,70);
//      DosBeep(1300,70);
//      SETQERRMSG
//      ItiStrCpy (szErrorMsg,
//                 "ERROR: Incorrect number of columns for SummarySection "__FILE__ ,
//                 sizeof szErrorMsg);         
//      ItiRptUtErrorMsgBox(NULL, szErrorMsg);
//
//      ItiPrtPutTextAt (CURRENT_LN, 5,
//                       LEFT, USE_RT_EDGE,
//                       szErrorMsg);
//      ItiPrtIncCurrentLn();
//      return 13;
//      }


//   if ((ItiPrtKeepNLns(11) == PG_WAS_INC)
//       || (ItiPrtQueryCurrentPgNum() != usCurrentPg ))
//         {
//         usCurrentPg = ItiPrtQueryCurrentPgNum();
//         ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
//         }

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_1].usTtlCol,
                    LEFT, (acttl[RPT_TTL_1].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_1].szTitle);

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_1].usDtaCol,
                    LEFT, (acttl[RPT_TTL_1].usDtaColWth | REL_LEFT),
                    ppszSum[STD_ITEM_NUM]);

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_2].usTtlCol,
                    LEFT, (acttl[RPT_TTL_2].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_2].szTitle);

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_2].usDtaCol,
                    LEFT, (acttl[RPT_TTL_2].usDtaColWth | REL_LEFT),
                    ppszSum[DESC]);
   ItiPrtIncCurrentLn();
   ItiPrtIncCurrentLn();


   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_3].usTtlCol,
                    LEFT, (acttl[RPT_TTL_3].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_3].szTitle);

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_3].usDtaCol,
                    LEFT, (acttl[RPT_TTL_3].usDtaColWth | REL_LEFT),
                    ppszSum[IMPORT_DATE]);
   ItiPrtIncCurrentLn();


   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_4].usTtlCol,
                    LEFT, (acttl[RPT_TTL_4].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_4].szTitle);

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_4].usDtaCol,
                    LEFT, (acttl[RPT_TTL_4].usDtaColWth | REL_LEFT),
                    ppszSum[AREA_ID]);

   us = ItiPrtQueryCurrentColNum() + 2;
   ItiPrtPutTextAt (CURRENT_LN, us,
                    LEFT, (acttl[RPT_TTL_10].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_10].szTitle);
   ItiPrtIncCurrentLn();
   ItiPrtIncCurrentLn();


   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_11].usTtlCol,
                    LEFT, (acttl[RPT_TTL_11].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_11].szTitle);
   ItiPrtIncCurrentLn();


   /* -- Very Low Range: */
   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_5].usTtlCol,
                    LEFT, (acttl[RPT_TTL_5].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_5].szTitle);

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_5].usDtaCol,
                    RIGHT, (acttl[RPT_TTL_5].usDtaColWth | REL_LEFT),
                    ppszSum[PCTL_5]);

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_9].usTtlCol,
                    LEFT, (acttl[RPT_TTL_9].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_9].szTitle);

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_9].usDtaCol,
                    RIGHT, (acttl[RPT_TTL_9].usDtaColWth | REL_LEFT),
                    ppszSum[PCTL_25]);
   ItiPrtIncCurrentLn();


   /* -- Low Range: */
   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_6].usTtlCol,
                    LEFT, (acttl[RPT_TTL_6].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_6].szTitle);

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_6].usDtaCol,
                    RIGHT, (acttl[RPT_TTL_6].usDtaColWth | REL_LEFT),
                    ppszSum[PCTL_25]);

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_9].usTtlCol,
                    LEFT, (acttl[RPT_TTL_9].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_9].szTitle);

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_9].usDtaCol,
                    RIGHT, (acttl[RPT_TTL_9].usDtaColWth | REL_LEFT),
                    ppszSum[PCTL_50]);
   ItiPrtIncCurrentLn();


   /* -- High Range: */
   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_7].usTtlCol,
                    LEFT, (acttl[RPT_TTL_7].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_7].szTitle);

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_7].usDtaCol,
                    RIGHT, (acttl[RPT_TTL_7].usDtaColWth | REL_LEFT),
                    ppszSum[PCTL_50]);

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_9].usTtlCol,
                    LEFT, (acttl[RPT_TTL_9].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_9].szTitle);

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_9].usDtaCol,
                    RIGHT, (acttl[RPT_TTL_9].usDtaColWth | REL_LEFT),
                    ppszSum[PCTL_75]);
   ItiPrtIncCurrentLn();


   /* -- Very High Range: */
   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_8].usTtlCol,
                    LEFT, (acttl[RPT_TTL_8].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_8].szTitle);

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_8].usDtaCol,
                    RIGHT, (acttl[RPT_TTL_8].usDtaColWth | REL_LEFT),
                    ppszSum[PCTL_75]);

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_9].usTtlCol,
                    LEFT, (acttl[RPT_TTL_9].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_9].szTitle);

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_9].usDtaCol,
                    RIGHT, (acttl[RPT_TTL_9].usDtaColWth | REL_LEFT),
                    ppszSum[PCTL_95]);
   ItiPrtIncCurrentLn();
   ItiPrtIncCurrentLn(); //Blank line


   ItiFreeStrArray (hheap, ppszSum, usNumCols);

   bDataFound = TRUE;

   return 0;
   }/* End of Function SummarySectionFor */   







USHORT PriceSectionFor (PSZ pszKey)
   {
   USHORT us   = 0;
   USHORT usErr, usNumCols;
   USHORT usBreakLnCnt = 1;
   PSZ    pszEnv  = NULL;
   PSZ   *ppszPrc = NULL;
   HQRY   hqryPrc = NULL;


   if ((pszKey == NULL) || (pszKey[0] == '\0'))
      {
      DosScanEnv(ITEMS_NO_PEMETH_DATA, &pszEnv);
      if ((pszEnv != NULL) && ((pszEnv[0] == 'Y') || (pszEnv[0] == 'y')) )
         {
         ItiPrtIncCurrentLn (); //blank line
         ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
         ItiPrtIncCurrentLn (); //line feed past separator line
         }
      return NO_KEY;
      }

   /* -- Build the query. */
/* // */    ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
/* // */                      szQueryPrices, pszKey, szPricesBaseDate);

//   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
//                    szQueryPrices, pszKey, " ");

/* // */   ItiStrCat(szQuery, szBaseDate, sizeof szQuery);
   ItiStrCat(szQuery, szPriceOrder, sizeof szQuery);


   /* -- Do the database query. */
   hqryPrc = ItiDbExecQuery (szQuery, hheap, hmodModule, ITIRID_RPT,
                          ID_QUERY_PRICES, &usNumCols, &usErr);
   if (hqryPrc == NULL)
      {
      SETQERRMSG
      }
   else
      {
      if (ItiPrtQueryRemainNumDisplayLns() > 5)
         SetColumnTitles(RPT_TTL_12, RPT_TTL_15);

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

         ItiFreeStrArray (hheap, ppszPrc, usNumCols);


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






USHORT PrintProc (HWND hwnd, MPARAM mp1, MPARAM mp2)
   {
   USHORT us    = 0;
   CHAR   szKey      [SMARRAY] = "";
   CHAR   szOptSwStr [SMARRAY] = "";
   CHAR   szCmdLn [2*SMARRAY +1] = "";
   CHAR   szFName [SMARRAY] = "";
   BOOL   bChecked = FALSE;
   HFILE  hfile;


   /* -- reset the command line. */
   ItiStrCpy (szCmdLn, RPT_DLL_NAME, sizeof(szCmdLn) );


   bChecked = (USHORT) (ULONG) WinSendDlgItemMsg (hwnd, 
                               DID_SELECTED, BM_QUERYCHECK, 0, 0);
   if (bChecked)
      {
      /* -- Build cmdln string to do selected code tables. */

      us = ItiRptUtBuildSelectedKeysFile(hwnd,
                             StandardItemCatL,
                             cStandardItemKey,
                             szFName, sizeof szFName);

      if ((us == ITIRPTUT_PARM_ERROR)|| (us == ITIRPTUT_ERROR))
         {
         ItiRptUtErrorMsgBox (hwnd,
                "ERROR: Failed to get selected keys in "__FILE__);
         return (13);
         }
      else
         {/* -- Prep the CmdLn to use the key file. */
         ItiStrCat (szCmdLn, ITIRPT_OPTSW_KEYFILE, sizeof szCmdLn);
         ItiStrCat (szCmdLn, szFName, sizeof szCmdLn);
         }/* end of if ((us... */

      }/* end of if (bChecked... */
   else
      {/* -- Use ALL of the keys. */
      ItiRptUtStartKeyFile (&hfile, szFName, sizeof szFName);

      /* -- Build the query. */
      ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                        "select StandardItemKey "," from StandardItem ", " ");

      /* -- Do the database query to get ALL of the keys. */
      hqry = ItiDbExecQuery (szQuery, hheap, hmodModule, ITIRID_RPT,
                             ID_QUERY_NUMBER, &usNumCols, &usErr);
      if (hqry == NULL)
         {
         ItiRptUtErrorMsgBox (hwnd,
                "ERROR: Failed to get ALL of the keys in "__FILE__);
         }
      else
         {
         while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
            {
            ItiRptUtWriteToKeyFile (hfile, ppsz[0], NULL);
            }
         }

      ItiRptUtEndKeyFile (hfile);

      ItiStrCat (szCmdLn, ITIRPT_OPTSW_KEYFILE, sizeof szCmdLn);
      ItiStrCat (szCmdLn, szFName, sizeof szCmdLn);
      }


   us = ItiRptUtExecRptCmdLn (hwnd, szCmdLn, RPT_AVG_SESSION);

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
   ItiStrCpy (pszBuffer, "StdItm: PEMETH Avg.", usMaxSize);   
   *pusWindowID = rSIAvg_RptDlgBox;                       
   }/* END OF FUNCTION */




BOOL EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID, 
                                     USHORT usActiveWindowID) 
   {
   if ((usTargetWindowID == rSIAvg_RptDlgBox)                     
       && ( (usActiveWindowID == StandardItemCatL)
          ||(usActiveWindowID == StandardItemCatS)) )

      return TRUE;                                            
   else                                                       
      return FALSE;                                           
   }/* END OF FUNCTION */



MRESULT EXPORT ItiDllrSIAvgRptDlgProc (HWND     hwnd,     
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
         us = ItiRptUtInitDlgBox (hwnd, RPT_AVG_SESSION, 0, NULL);
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
                  }
               return 0;



            case DID_CANCEL:
               WinDismissDlg (hwnd, FALSE);
               return 0;

            }/* end of switch(SHORT1... */

         break;


      default:
         us = ItiRptUtChkDlgMsg (hwnd, usMessage, mp1, mp2);
         break;
      }

   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);

   }/* END OF FUNCTION ItiDllrSIAvgRptDlgProc */








USHORT EXPORT ItiRptDLLPrintReport
                 (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[])
   {
   PSZ     pszKey = szKey;

           /* General return code variable: */
   USHORT  us = 0;



   /* ------------------------------------------------------------------- *\
    *  Create a memory heap.                                              *
   \* ------------------------------------------------------------------- */
   hheap = ItiMemCreateHeap (MIN_HEAP_SIZE);


   /* ------------------------------------------------------------------- *\
    * Initialize the strings.                                             *
   \* ------------------------------------------------------------------- */
   ItiStrCpy(szCurrentID,"TABLE_ID",sizeof(szCurrentID));

   /* -- Check for key file. */
   ItiRptUtGetOption (ITIRPTUT_FILE_SWITCH,
                      szTmpBaseDate, sizeof szTmpBaseDate,
                      argcnt, argvars);
   if (ItiStrLen(szTmpBaseDate) < 4 )
      {
      ItiRptUtErrorMsgBox(NULL,
         "REPORT ERROR: No /K option was given to " __FILE__ );
      return (13);
      }
   szTmpBaseDate[0] = '\0';


   ItiRptUtGetOption (OPT_SW_BASEDATE, szTmpBaseDate, sizeof szTmpBaseDate,
                      argcnt, argvars);
   if (ItiStrLen(szTmpBaseDate) > 3 )
      {
      szBaseDate[0] = '\"';
      ItiStrCat(szBaseDate, szTmpBaseDate, sizeof szBaseDate);
      /* -- The % symbol is used by the LIKE clause to avoid the
         -- problem of the time part of the basedate searching.  */
      ItiStrCat(szBaseDate, "\%\" ", sizeof szBaseDate);
      }
   else
      {
      /* -- Error, MUST have a basedate! */
      ItiRptUtErrorMsgBox(NULL,"REPORT ERROR: No BaseDate was given to " __FILE__ );
      return (13);
      }


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




   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_LISTING,
                       szQueryListing, sizeof szQueryListing);

   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_LISTING_CONJ,
                       szQueryListingConj, sizeof szQueryListingConj);

   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_LISTING_ORDER,
                       szListingOrder, sizeof szListingOrder);
                     


   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_PRICES,
                       szQueryPrices, sizeof szQueryPrices);
                     
   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_PRICES_BASEDATE,
                       szPricesBaseDate, sizeof szPricesBaseDate);

   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_PRICES_ORDER,
                       szPriceOrder, sizeof szPriceOrder);



   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_NUMBER,
                       szQueryNumber, sizeof szQueryNumber);


   /* -------------------------------------------------------------------- *\
    * -- For each StandardItemKey...                                       *
   \* -------------------------------------------------------------------- */
   ItiRptUtInitKeyList(argcnt, argvars); /* Will look for the /K switch */
   while ( (usKeyFlag = ItiRptUtGetNextKey(pszKey,sizeof(szKey)) )
                                               != ITIRPTUT_NO_MORE_KEYS)
      {
      us = SummarySectionFor(szKey);

      if (us == 0)
         PriceSectionFor(szKey);

      }/* End of WHILE GetNextKey */


   if (!bDataFound)
      {
      ItiPrtPutTextAt (CURRENT_LN+3, 0,
                       CENTER, USE_RT_EDGE,
                       "(No PEMETH Average Data found.)");
      }

   /* ------------------------------------------------------------------- *\
    * Finish up and quit.                                                 *
   \* ------------------------------------------------------------------- */
   ItiPrtEndReport();

   DBGMSG("Exit ***DLL function ItiRptDLLPrintReport");
   return 0;

   }/* END OF FUNCTION ItiRptDLLPrintReport */





