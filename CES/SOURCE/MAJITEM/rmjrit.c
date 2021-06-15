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


/*  CES Major Item Listing Report DLL Source Code
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


#include    "rindllmj.h"


#include "rMjrIt.h"                                       



/* ======================================================================= *\
 *                                         MANIFEST CONSTANT DEFINITIONS   *
\* ======================================================================= */
#define  BLANK_LINE_INTERVAL    3
#define  PRT_Q_NAME             "Major Item List"

#define  QUERY_ARRAY_SIZE       840

#define  SIZE_OF_QUERY_RPT_DESC QUERY_ARRAY_SIZE


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
static CHAR szDllVersion[] = "1.1a0 rmjrit.dll";

static  CHAR  szCurrentID   [SMARRAY + 1] = "";
static  CHAR  szOrderingStr [LGARRAY + 1] = "";

static  CHAR  szQueryDesc [SIZE_OF_QUERY + 1] = "";
static  CHAR  szQueryListing   [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szQuery       [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szQueryLoc    [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szKey         [SMARRAY + 1] = "";
static  CHAR  szMjrID       [8] = "";


static  CHAR  szQryBaseDate [SIZE_OF_QRY_BASEDATE + 1] = "";
static  CHAR  szBaseDate [SMARRAY + 1] = "";



static  CHAR  szQryQvar    [SIZE_OF_QRY_QVAR + 1] = "";
static  CHAR  szCnjQvar    [3 * SMARRAY + 1] = "";

static  CHAR  szQryPriceRegrs [SIZE_OF_QRY_PRICE_REGRS + 1] = "";
static  CHAR  szCnjPriceRegrs [2 * SMARRAY + 1] = "";

static  CHAR  szQryQtyRegrs [SIZE_OF_QRY_QTY_REGRS + 1] = "";
static  CHAR  szCnjQtyRegrs [2 * SMARRAY + 1] = "";



static  CHAR  szQryQvAdj   [SIZE_OF_QRY_QV_ADJ + 1] = "";
static  CHAR  szQryQestAdj [SIZE_OF_QRY_QV_ADJ + 1] = "";

static  CHAR  szCnjQvAdjBaseDate [2 * SMARRAY + 1] = "";
static  CHAR  szCnjQvAdj         [3 * SMARRAY + 1] = "";


static  CHAR   szFailedDLL [CCHMAXPATH + 1];
static  CHAR   szErrorMsg [1024] = "";

static  USHORT usNumOfTitles;
static  USHORT usCurrentPg = 0;


           /* Used by DbExecQuer & DbGetQueryRow functions: */
static  HHEAP   hheap  = NULL;
static  HQRY    hqry   = NULL;
static  USHORT  usNumCols = 0;
static  USHORT  usErr     = 0;
static  PSZ   * ppsz   = NULL;

static  HQRY    hqryLoc   = NULL;
static  USHORT  usNumColsLoc = 0;
static  USHORT  usErrLoc     = 0;
static  PSZ   * ppszLoc   = NULL;


           /* Title strings for this module. */
static  REPTITLES rept;
static  PREPT     prept = &rept;





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









USHORT SummarySectionFor (PSZ pszKey, PSZ pszMjrID)
   {
   USHORT usLn;
   CHAR szUnitType[15] = "";
   CHAR szSpecYear[8] = "";
   PSZ * ppsz = NULL;


   if ((pszKey == NULL) || (*pszKey == '\0'))
      return 13;

   /* -- Build the query. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryDesc, pszKey, " order by 1 ");


   ppsz = ItiDbGetRow1 (szQuery, hheap, hmodModule, ITIRID_RPT,
                        ID_QUERY_DESC, &usNumCols);

   if (ppsz != NULL)
      {
      usLn = 7 + ItiStrLen(ppsz[DESC]) / acttl[RPT_TTL_2].usDtaColWth;
      if ((ItiPrtKeepNLns(usLn) == PG_WAS_INC)
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
                       ppsz[ID]);
      ItiStrCpy(pszMjrID, ppsz[ID], sizeof szMjrID);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_2].usTtlCol,
                       LEFT, (acttl[RPT_TTL_2].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_2].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_2].usDtaCol,
                       (LEFT|WRAP), (acttl[RPT_TTL_2].usDtaColWth | REL_LEFT),
                       ppsz[DESC]);
    //  ItiPrtIncCurrentLn();

      /* -- SpecYear and unit type info. */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_37].usTtlCol,
                       LEFT, (acttl[RPT_TTL_37].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_37].szTitle);

      if (*ppsz[UNITTYPE] == '\0')
         ItiStrCpy(szUnitType, "Both ", sizeof szUnitType);
      else if (*ppsz[UNITTYPE] == '0')
         ItiStrCpy(szUnitType, "English", sizeof szUnitType);
      else
         ItiStrCpy(szUnitType, "Metric", sizeof szUnitType);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_37].usDtaCol,
                       (LEFT|WRAP), (acttl[RPT_TTL_37].usDtaColWth | REL_LEFT),
                       szUnitType);
      // ItiPrtIncCurrentLn();

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_36].usTtlCol,
                       LEFT, (acttl[RPT_TTL_36].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_36].szTitle);

      if (*ppsz[SPECYR] != '\0')
         ItiStrCpy(szSpecYear, ppsz[SPECYR], sizeof szSpecYear);
      else
         ItiStrCpy(szSpecYear, "All ", sizeof szSpecYear);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_36].usDtaCol,
                       LEFT, (acttl[RPT_TTL_36].usDtaColWth | REL_LEFT),
                       szSpecYear);

      //  
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_3].usTtlCol,
                       LEFT, (acttl[RPT_TTL_3].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_3].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_3].usDtaCol,
                       LEFT, (acttl[RPT_TTL_3].usDtaColWth | REL_LEFT),
                       ppsz[UNIT]);

      ItiPrtPutTextAt (CURRENT_LN,
                       (CURRENT_COL + 1), // acttl[RPT_TTL_4].usTtlCol,
                       LEFT, (acttl[RPT_TTL_4].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_4].szTitle);

      ItiPrtPutTextAt (CURRENT_LN,
                       (CURRENT_COL + 1), // acttl[RPT_TTL_4].usDtaCol,
                       LEFT, (acttl[RPT_TTL_4].usDtaColWth | REL_LEFT),
                       ppsz[TRANS]);
      ItiPrtIncCurrentLn();
      ItiPrtIncCurrentLn();

      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }
   else
      {
      SETQERRMSG(szQuery);
      return 13;
      }

   return 0;
   }/* End of Function SummarySectionFor */







USHORT ItemListSectionFor (PSZ pszMjrID, PSZ pszKey)
   {
   BOOL bLabeled = FALSE;

   if ((pszKey == NULL) || (*pszKey == '\0') || (pszMjrID == NULL))
      return 13;

   /* -- Build the query. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryListing, pszKey, " order by 1 ");


   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hheap, hmodModule, ITIRID_RPT,
                          ID_QUERY_LISTING, &usNumCols, &usErr);
   
   /* -- Process the query results for each row. */
   if (hqry == NULL)
      {
      SETQERRMSG(szQuery);
      }
   else
      {
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
         {
         /* -- Check for new page. */
         if (ItiPrtKeepNLns(7) == PG_WAS_INC)
            {
            usCurrentPg = ItiPrtQueryCurrentPgNum();
            ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usTtlCol,
                    LEFT, (acttl[RPT_TTL_12].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_12].szTitle);

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usDtaCol,
                    LEFT, (acttl[RPT_TTL_12].usDtaColWth | REL_LEFT),
                    pszMjrID);
            ItiPrtIncCurrentLn (); //line feed
            }
//         else
//            {
            if (!bLabeled)
               {
               ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_5].usTtlCol,
                          LEFT, (acttl[RPT_TTL_5].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_5].szTitle);
               bLabeled = TRUE;
               }
//            }

         /* -- 1st standard item line. */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_5].usDtaCol,
//                       LEFT,
                       RIGHT,
                       (acttl[RPT_TTL_5].usDtaColWth | REL_LEFT),
                       ppsz[STD_NUM]);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_6].usTtlCol,
                       LEFT, (acttl[RPT_TTL_6].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_6].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_6].usDtaCol,
                       LEFT,
                       (acttl[RPT_TTL_6].usDtaColWth | REL_LEFT),
                       ppsz[DESC]);
         ItiPrtIncCurrentLn (); //line feed


         /* -- 2nd standard item line. */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_7].usTtlCol,
                       LEFT, (acttl[RPT_TTL_7].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_7].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_7].usDtaCol,
                       LEFT,
                       (acttl[RPT_TTL_7].usDtaColWth | REL_LEFT),
                       ppsz[UNIT]);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_8].usTtlCol,
                       LEFT, (acttl[RPT_TTL_8].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_8].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_8].usDtaCol,
                       RIGHT,
                       (acttl[RPT_TTL_8].usDtaColWth | REL_LEFT),
                       ppsz[QCF]);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_9].usDtaCol,
                       LEFT,
                       (acttl[RPT_TTL_9].usDtaColWth | REL_LEFT),
                       ppsz[COMM]);

         ItiPrtPutTextAt (CURRENT_LN,
                       (CURRENT_COL + 1), // acttl[RPT_TTL_9].usTtlCol,
                       LEFT, (acttl[RPT_TTL_9].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_9].szTitle);
         ItiPrtIncCurrentLn (); //line feed


         /* -- 3rd standard item line. */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_10].usTtlCol,
                       LEFT, (acttl[RPT_TTL_10].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_10].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_10].usDtaCol,
                       LEFT,
                       (acttl[RPT_TTL_10].usDtaColWth | REL_LEFT),
                       ppsz[PCT]);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_11].usTtlCol,
                       LEFT, (acttl[RPT_TTL_11].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_11].szTitle);

         ItiPrtIncCurrentLn (); //line feed
         ItiPrtIncCurrentLn (); //blank line 
         ItiFreeStrArray (hheap, ppsz, usNumCols);
         }/* End of WHILE GetQueryRow */

      }/* else clause */
   

   }/* End of Function ItemListSectionFor */






USHORT PriceEstimationSectionFor (PSZ pszMjrID, PSZ pszKey)
   {
   BOOL   bLabeled = FALSE;
   BOOL   bHaveData = FALSE;

   PSZ    pszBaseDate;
   USHORT usYear  = 0;
   USHORT usMonth = 0;
   USHORT usDay   = 0;


   if ((pszKey == NULL) || (*pszKey == '\0') || (pszMjrID == NULL))
      return 13;


   /* -- Build the query to get the most recent basedate. */
   ItiStrCpy(szQuery, szQryBaseDate, sizeof szQuery);
   ItiStrCat(szQuery, pszKey,        sizeof szQuery);
   ItiStrCat(szQuery, " order by 1 DESC ", sizeof szQuery);


   pszBaseDate = ItiDbGetRow1Col1 (szQuery, hheap, hmodModule,
                                   ITIRID_RPT, ID_QRY_BASEDATE);
   if ((pszBaseDate != NULL) && (*pszBaseDate != '\0')
       && (pszBaseDate[0] != 'N') && (pszBaseDate[1] != 'A'))
      {
      ItiStrCpy(szBaseDate, pszBaseDate, sizeof szBaseDate);

      pszBaseDate = &szBaseDate[0];

      ItiFmtParseDate(&pszBaseDate, &usYear, &usMonth, &usDay);
      if (usYear == 0)
         {
         ItiRptUtErrorMsgBox (NULL, szQuery);           // dev code
         ItiRptUtErrorMsgBox (NULL, szBaseDate);        // dev code
         ItiRptUtErrorMsgBox (NULL, pszBaseDate);// dev code
         return 1; /* -- Not a valid base date, so no data */
                   /*    exists for this major item.       */
         }
      else
         {
         /* -- We have a valid base date, so continue. */
         }
      }
   else
      {/* -- No major item regression data exists for this key, so return. */
      szBaseDate[0] = '\0';
      return 1;
      }




   /* -- Build the query to get the quantity information. */
   ItiStrCpy(szQuery, szQryPriceRegrs, sizeof szQuery);
   ItiStrCat(szQuery, pszKey,          sizeof szQuery);
   ItiStrCat(szQuery, szCnjPriceRegrs, sizeof szQuery);
   ItiStrCat(szQuery, szBaseDate,      sizeof szQuery);

   ppsz = ItiDbGetRow1(szQuery, hheap, hmodModule,
                      ITIRID_RPT, ID_QRY_PRICE_REGRS, &usNumCols);
   if (ppsz != NULL)
      {
      /* -- Check for new page. */
      if (ItiPrtKeepNLns(9) == PG_WAS_INC)
         {
         usCurrentPg = ItiPrtQueryCurrentPgNum();
         ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usTtlCol,
                 LEFT, (acttl[RPT_TTL_12].usTtlColWth | REL_LEFT),
                 acttl[RPT_TTL_12].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usDtaCol,
                 LEFT, (acttl[RPT_TTL_12].usDtaColWth | REL_LEFT),
                 pszMjrID);
         ItiPrtIncCurrentLn (); //line feed
         }


      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_13].usTtlCol,
                 LEFT, (acttl[RPT_TTL_13].usTtlColWth | REL_LEFT),
                 acttl[RPT_TTL_13].szTitle);
      ItiPrtIncCurrentLn (); //line feed
      

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_14].usTtlCol,
                 LEFT, (acttl[RPT_TTL_14].usTtlColWth | REL_LEFT),
                 acttl[RPT_TTL_14].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                 LEFT, (acttl[RPT_TTL_14].usDtaColWth | REL_LEFT),
                 ppsz[PRICE_REGRS_MED_QTY]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 2,
                 LEFT, (acttl[RPT_TTL_14].usDtaColWth | REL_LEFT),
                 ppsz[PRICE_REGRS_UNIT]);
      ItiPrtIncCurrentLn (); //line feed


      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_15].usTtlCol,
                 LEFT, (acttl[RPT_TTL_15].usTtlColWth | REL_LEFT),
                 acttl[RPT_TTL_15].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                 LEFT, (acttl[RPT_TTL_15].usDtaColWth | REL_LEFT),
                 ppsz[PRICE_REGRS_MED_QTY_PRICE]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                 LEFT, (2 | REL_LEFT),
                 "/ ");

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                 LEFT, (acttl[RPT_TTL_15].usDtaColWth | REL_LEFT),
                 ppsz[PRICE_REGRS_UNIT]);
      ItiPrtIncCurrentLn (); //line feed



      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_16].usTtlCol,
                 LEFT, (acttl[RPT_TTL_16].usTtlColWth | REL_LEFT),
                 acttl[RPT_TTL_16].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                 LEFT, (acttl[RPT_TTL_16].usDtaColWth | REL_LEFT),
                 ppsz[PRICE_REGRS_QTY_ADJ]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                 LEFT, (2 | REL_LEFT),
                 ". ");
      ItiPrtIncCurrentLn (); //line feed
      ItiPrtIncCurrentLn (); //blank line

      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }
   else
      {/* -- No major item regression data exists for this key, so return. */
      return 1;
      }



   /* === Adjustments list:  */

   /* -- Do the query to get the QualitativeVariables for this MajorItem. */
   ItiStrCpy(szQuery, szQryQvar,   sizeof szQuery);
   ItiStrCat(szQuery, pszKey,      sizeof szQuery);
   ItiStrCat(szQuery, szCnjQvar,   sizeof szQuery);
   ItiStrCat(szQuery, szBaseDate,  sizeof szQuery);

   hqry = ItiDbExecQuery (szQuery, hheap, hmodModule, ITIRID_RPT,
                          ID_QRY_QVAR, &usNumCols, &usErr);
   
   /* -- Process the query results for each row. */
   if (hqry == NULL)
      {
      SETQERRMSG(szQuery);
      }
   else
      {
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
         {

         /* -- Query to retrieve the QualitativeValue and PriceAdjustment */
         /* --  for each QualitativeVariable for a particular MajorItem   */
         ItiStrCpy(szQueryLoc, szQryQvAdj,         sizeof szQueryLoc);
         ItiStrCat(szQueryLoc, pszKey,               sizeof szQueryLoc);
         ItiStrCat(szQueryLoc, szCnjQvAdjBaseDate, sizeof szQueryLoc);
         ItiStrCat(szQueryLoc, szBaseDate,           sizeof szQueryLoc);
         ItiStrCat(szQueryLoc, szCnjQvAdj,         sizeof szQueryLoc);
         ItiStrCat(szQueryLoc, ppsz[QVAR],           sizeof szQueryLoc);

         hqryLoc = ItiDbExecQuery (szQueryLoc, hheap, hmodModule, ITIRID_RPT,
                                ID_QRY_QV_ADJ, &usNumColsLoc, &usErrLoc);
         
         /* -- Process the query results for each row. */
         if (hqryLoc == NULL)
            {
            SETQERRMSG(szQueryLoc);
            }
         else
            {
            bHaveData = FALSE;

            while( ItiDbGetQueryRow (hqryLoc, &ppszLoc, &usErrLoc) )  
               {
               /* -- Check for new page. */
               if (ItiPrtKeepNLns(4) == PG_WAS_INC)
                  {
                  usCurrentPg = ItiPrtQueryCurrentPgNum();
                  ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");

                  ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usTtlCol,
                          LEFT, (acttl[RPT_TTL_12].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_12].szTitle);

                  ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usDtaCol,
                          LEFT, (acttl[RPT_TTL_12].usDtaColWth | REL_LEFT),
                          pszMjrID);
                  ItiPrtIncCurrentLn (); //line feed

                  ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_17].usTtlCol,
                             LEFT, (acttl[RPT_TTL_17].usTtlColWth | REL_LEFT),
                             acttl[RPT_TTL_17].szTitle);

                  ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                             LEFT, (acttl[RPT_TTL_16].usDtaColWth | REL_LEFT),
                             ppsz[QVAR]);
                  ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                             LEFT, (2 | REL_LEFT),
                             ": ");
                  ItiPrtIncCurrentLn (); //line feed

                  bHaveData = TRUE;
                  }

      
               if (!bHaveData)
                  {
                  /* -- Adjustments for ... label: */
                  ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_17].usTtlCol,
                             LEFT, (acttl[RPT_TTL_17].usTtlColWth | REL_LEFT),
                             acttl[RPT_TTL_17].szTitle);

                  ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                             LEFT, (acttl[RPT_TTL_16].usDtaColWth | REL_LEFT),
                             ppsz[QVAR]);
                  ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                             LEFT, (2 | REL_LEFT),
                             ": ");
                  ItiPrtIncCurrentLn (); //line feed

                  bHaveData = TRUE;
                  }




               ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_20].usDtaCol,
                         LEFT, (acttl[RPT_TTL_20].usDtaColWth | REL_LEFT),
                         ppszLoc[QVALUE]);
               
               if (  (ppszLoc[QV_ADJ][0] != '-')
                   &&(ppszLoc[QV_ADJ][1] != '-'))
                  {
                  /* -- INcreases */
                  ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_18].usTtlCol,
                             LEFT, (acttl[RPT_TTL_18].usTtlColWth | REL_LEFT),
                             acttl[RPT_TTL_18].szTitle);

                  ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                             LEFT, USE_RT_EDGE,
                             ppszLoc[QV_ADJ]);
                  }
               else
                  { /* -- DEcreases */
                  if (ppszLoc[QV_ADJ][0] == '-')
                     ppszLoc[QV_ADJ][0] = ' ';

                  if (ppszLoc[QV_ADJ][1] == '-')
                     ppszLoc[QV_ADJ][1] = ' ';

                  ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_19].usTtlCol,
                             LEFT, (acttl[RPT_TTL_19].usTtlColWth | REL_LEFT),
                             acttl[RPT_TTL_19].szTitle);

                  ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL,
                             LEFT, USE_RT_EDGE,
                             ppszLoc[QV_ADJ]);
                  }

               ItiPrtIncCurrentLn (); //line feed
               ItiFreeStrArray (hheap, ppszLoc, usNumColsLoc);
               }/* End of WHILE GetQueryRowLoc */


            ItiPrtIncCurrentLn (); //blank line
            }/* end else clause */

         ItiFreeStrArray (hheap, ppsz, usNumCols);
         }/* End of WHILE GetQueryRow */

      }/* else clause */
   

   }/* End of Function PriceEstimationSectionFor */









USHORT QuantityEstimationSectionFor (PSZ pszMjrID, PSZ pszKey)
   {
   BOOL   bLabeled = FALSE;
   BOOL   bHaveData = FALSE;
//   PSZ    pszBaseDate;
   USHORT usYear  = 0;
   USHORT usMonth = 0;
   USHORT usDay   = 0;


   if ((pszKey == NULL) || (*pszKey == '\0') || (pszMjrID == NULL))
      return 13;


   if ((szBaseDate[0] == '\0')
       || ((szBaseDate[0] == 'N') && (szBaseDate[1] == 'A')))
      {
      return 14;
      }


   /* -- Build the query to get the quantity information. */
   ItiStrCpy(szQuery, szQryQtyRegrs, sizeof szQuery);
   ItiStrCat(szQuery, pszKey,          sizeof szQuery);
   ItiStrCat(szQuery, szCnjQtyRegrs, sizeof szQuery);
   ItiStrCat(szQuery, szBaseDate,      sizeof szQuery);

   ppsz = ItiDbGetRow1(szQuery, hheap, hmodModule,
                      ITIRID_RPT, ID_QRY_QTY_REGRS, &usNumCols);
   if (ppsz != NULL)
      {
      /* -- Check for new page. */
      if (ItiPrtKeepNLns(18) == PG_WAS_INC)
         {
         usCurrentPg = ItiPrtQueryCurrentPgNum();
         ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usTtlCol,
                 LEFT, (acttl[RPT_TTL_12].usTtlColWth | REL_LEFT),
                 acttl[RPT_TTL_12].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usDtaCol,
                 LEFT, (acttl[RPT_TTL_12].usDtaColWth | REL_LEFT),
                 pszMjrID);
         ItiPrtIncCurrentLn (); //line feed
         }


      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_21].usTtlCol,
                 LEFT, (acttl[RPT_TTL_21].usTtlColWth | REL_LEFT),
                 acttl[RPT_TTL_21].szTitle);
      ItiPrtIncCurrentLn (); //line feed
      

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_22].usTtlCol,
                 LEFT, (acttl[RPT_TTL_22].usTtlColWth | REL_LEFT),
                 acttl[RPT_TTL_22].szTitle);
      ItiPrtIncCurrentLn (); //line feed
      ItiPrtIncCurrentLn (); //blank line


      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_23].usTtlCol,
                 LEFT, (acttl[RPT_TTL_23].usTtlColWth | REL_LEFT),
                 acttl[RPT_TTL_23].szTitle);
      ItiPrtIncCurrentLn (); //line feed

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_24].usTtlCol,
                 LEFT, (acttl[RPT_TTL_24].usTtlColWth | REL_LEFT),
                 acttl[RPT_TTL_24].szTitle);
      ItiPrtIncCurrentLn (); //line feed

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_25].usTtlCol,
                 LEFT, (acttl[RPT_TTL_25].usTtlColWth | REL_LEFT),
                 acttl[RPT_TTL_25].szTitle);
      ItiPrtIncCurrentLn (); //line feed
      ItiPrtIncCurrentLn (); //blank line


      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_23].usTtlCol,
                 LEFT, (11 | REL_LEFT),
                 acttl[RPT_TTL_23].szTitle);
      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                 LEFT, (2 | REL_LEFT),
                 " (");
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_26].usDtaCol,
                 RIGHT, (acttl[RPT_TTL_26].usDtaColWth | REL_LEFT),
                 ppsz[QTY_REGRS_BASE_COEF]);
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_26].usTtlCol,
                 LEFT, (acttl[RPT_TTL_26].usTtlColWth | REL_LEFT),
                 acttl[RPT_TTL_26].szTitle);
      ItiPrtIncCurrentLn (); //line feed


      /* -- Pavement depth */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_27].usDtaCol,
                 RIGHT, (acttl[RPT_TTL_27].usDtaColWth | REL_LEFT),
                 ppsz[QTY_REGRS_DPTH]);
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_27].usTtlCol,
                 LEFT, (acttl[RPT_TTL_27].usTtlColWth | REL_LEFT),
                 acttl[RPT_TTL_27].szTitle);
      ItiPrtIncCurrentLn (); //line feed

      /* -- Pavement width */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_28].usDtaCol,
                 RIGHT, (acttl[RPT_TTL_28].usDtaColWth | REL_LEFT),
                 ppsz[QTY_REGRS_WTH]);
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_28].usTtlCol,
                 LEFT, (acttl[RPT_TTL_28].usTtlColWth | REL_LEFT),
                 acttl[RPT_TTL_28].szTitle);
      ItiPrtIncCurrentLn (); //line feed

      /* -- Pavement depth * pavement width */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_29].usDtaCol,
                 RIGHT, (acttl[RPT_TTL_29].usDtaColWth | REL_LEFT),
                 ppsz[QTY_REGRS_XSEC]);
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_29].usTtlCol,
                 LEFT, (acttl[RPT_TTL_29].usTtlColWth | REL_LEFT),
                 acttl[RPT_TTL_29].szTitle);
      ItiPrtIncCurrentLn (); //line feed


      /* -- Structure coeff. */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_24].usTtlCol,
                 LEFT, (4 | REL_LEFT),
                 acttl[RPT_TTL_24].szTitle);
      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                 LEFT, (2 | REL_LEFT),
                 " (");

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_30].usDtaCol,
                 RIGHT, (acttl[RPT_TTL_30].usDtaColWth | REL_LEFT),
                 ppsz[QTY_REGRS_S_BASE]);
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_30].usTtlCol,
                 LEFT, (acttl[RPT_TTL_30].usTtlColWth | REL_LEFT),
                 acttl[RPT_TTL_30].szTitle);
      ItiPrtIncCurrentLn (); //line feed

      /* --  Structure length */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_31].usDtaCol,
                 RIGHT, (acttl[RPT_TTL_31].usDtaColWth | REL_LEFT),
                 ppsz[QTY_REGRS_S_LEN]);
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_31].usTtlCol,
                 LEFT, (acttl[RPT_TTL_31].usTtlColWth | REL_LEFT),
                 acttl[RPT_TTL_31].szTitle);
      ItiPrtIncCurrentLn (); //line feed

      /* -- Structure width */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_32].usDtaCol,
                 RIGHT, (acttl[RPT_TTL_32].usDtaColWth | REL_LEFT),
                 ppsz[QTY_REGRS_S_WTH]);
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_32].usTtlCol,
                 LEFT, (acttl[RPT_TTL_32].usTtlColWth | REL_LEFT),
                 acttl[RPT_TTL_32].szTitle);
      ItiPrtIncCurrentLn (); //line feed

      /* -- Structure area */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_33].usDtaCol,
                 RIGHT, (acttl[RPT_TTL_33].usDtaColWth | REL_LEFT),
                 ppsz[QTY_REGRS_S_AREA]);
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_33].usTtlCol,
                 LEFT, (acttl[RPT_TTL_33].usTtlColWth | REL_LEFT),
                 acttl[RPT_TTL_33].szTitle);
      ItiPrtIncCurrentLn (); //line feed


      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_25].usTtlCol,
                 LEFT, (acttl[RPT_TTL_25].usTtlColWth | REL_LEFT),
                 acttl[RPT_TTL_25].szTitle);
      ItiPrtIncCurrentLn (); //line feed
      ItiPrtIncCurrentLn (); //blank line

      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }
   else
      {/* -- No major item regression data exists for this key, so return. */
      return 1;
      }





   /* === Adjustments list:  */

   /* -- Do the query to get the QualitativeVariables for this MajorItem. */
   ItiStrCpy(szQuery, szQryQvar,   sizeof szQuery);
   ItiStrCat(szQuery, pszKey,      sizeof szQuery);
   ItiStrCat(szQuery, szCnjQvar,   sizeof szQuery);
   ItiStrCat(szQuery, szBaseDate,  sizeof szQuery);

   hqry = ItiDbExecQuery (szQuery, hheap, hmodModule, ITIRID_RPT,
                          ID_QRY_QVAR, &usNumCols, &usErr);
   
   /* -- Process the query results for each row. */
   if (hqry == NULL)
      {
      SETQERRMSG(szQuery);
      }
   else
      {
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
         {

         /* -- Query to retrieve the QualitativeValue and PriceAdjustment */
         /* --  for each QualitativeVariable for a particular MajorItem   */
                               /* -- vvv */
         ItiStrCpy(szQueryLoc, szQryQestAdj,         sizeof szQueryLoc);
         ItiStrCat(szQueryLoc, pszKey,               sizeof szQueryLoc);
         ItiStrCat(szQueryLoc, szCnjQvAdjBaseDate, sizeof szQueryLoc);
         ItiStrCat(szQueryLoc, szBaseDate,           sizeof szQueryLoc);
         ItiStrCat(szQueryLoc, szCnjQvAdj,         sizeof szQueryLoc);
         ItiStrCat(szQueryLoc, ppsz[QVAR],           sizeof szQueryLoc);

         hqryLoc = ItiDbExecQuery (szQueryLoc, hheap, hmodModule, ITIRID_RPT,
                                ID_QRY_QEST_ADJ, &usNumColsLoc, &usErrLoc);
         
         /* -- Process the query results for each row. */
         if (hqryLoc == NULL)
            {
            SETQERRMSG(szQueryLoc);
            }
         else
            {
            bHaveData = FALSE;

            while( ItiDbGetQueryRow (hqryLoc, &ppszLoc, &usErrLoc) )  
               {
               /* -- Check for new page. */
               if (ItiPrtKeepNLns(4) == PG_WAS_INC)
                  {
                  usCurrentPg = ItiPrtQueryCurrentPgNum();
                  ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");

                  ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usTtlCol,
                          LEFT, (acttl[RPT_TTL_12].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_12].szTitle);

                  ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usDtaCol,
                          LEFT, (acttl[RPT_TTL_12].usDtaColWth | REL_LEFT),
                          pszMjrID);
                  ItiPrtIncCurrentLn (); //line feed

                  ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_17].usTtlCol,
                             LEFT, (acttl[RPT_TTL_17].usTtlColWth | REL_LEFT),
                             acttl[RPT_TTL_17].szTitle);

                  ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                             LEFT, (acttl[RPT_TTL_16].usDtaColWth | REL_LEFT),
                             ppsz[QVAR]);
                  ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                             LEFT, (2 | REL_LEFT),
                             ": ");
                  ItiPrtIncCurrentLn (); //line feed

                  bHaveData = TRUE;
                  }

      
               if (!bHaveData)
                  {
                  /* -- Adjustments for ... label: */
                  ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_17].usTtlCol,
                             LEFT, (acttl[RPT_TTL_17].usTtlColWth | REL_LEFT),
                             acttl[RPT_TTL_17].szTitle);

                  ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                             LEFT, (acttl[RPT_TTL_16].usDtaColWth | REL_LEFT),
                             ppsz[QVAR]);
                  ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                             LEFT, (2 | REL_LEFT),
                             ": ");
                  ItiPrtIncCurrentLn (); //line feed

                  bHaveData = TRUE;
                  }




               ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_20].usDtaCol,
                         LEFT, (acttl[RPT_TTL_20].usDtaColWth | REL_LEFT),
                         ppszLoc[QVALUE]);
               
               if (  (ppszLoc[QV_ADJ][0] != '-')
                   &&(ppszLoc[QV_ADJ][1] != '-'))
                  {
                  /* -- INcreases */
                  ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_34].usTtlCol,
                             LEFT, (acttl[RPT_TTL_34].usTtlColWth | REL_LEFT),
                             acttl[RPT_TTL_34].szTitle);

                  ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                             LEFT, USE_RT_EDGE,
                             ppszLoc[QV_ADJ]);
                  }
               else
                  { /* -- DEcreases */
                  if (ppszLoc[QV_ADJ][0] == '-')
                     ppszLoc[QV_ADJ][0] = ' ';

                  if (ppszLoc[QV_ADJ][1] == '-')
                     ppszLoc[QV_ADJ][1] = ' ';

                  ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_35].usTtlCol,
                             LEFT, (acttl[RPT_TTL_35].usTtlColWth | REL_LEFT),
                             acttl[RPT_TTL_35].szTitle);

                  ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL,
                             LEFT, USE_RT_EDGE,
                             ppszLoc[QV_ADJ]);
                  }

               ItiPrtIncCurrentLn (); //line feed
               ItiFreeStrArray (hheap, ppszLoc, usNumColsLoc);
               }/* End of WHILE GetQueryRowLoc */


            ItiPrtIncCurrentLn (); //blank line
            }/* end else clause */

         ItiFreeStrArray (hheap, ppsz, usNumCols);
         }/* End of WHILE GetQueryRow */

      }/* else clause */
   

   }/* End of Function QuantityEstimationSectionFor */






USHORT PrintProc (HWND hwnd, MPARAM mp1, MPARAM mp2)
   {
   USHORT us    = 0;
   CHAR   szKey      [SMARRAY] = "";
   CHAR   szOptSwStr [SMARRAY] = "";
   CHAR   szCmdLn [5*SMARRAY +1] = "";
   CHAR   szFName [4*SMARRAY +1] = "";
   BOOL   bChecked = FALSE;


   /* -- reset the command line. */
   ItiStrCpy (szCmdLn, RPT_DLL_NAME, sizeof(szCmdLn) );


   bChecked = (USHORT) (ULONG) WinSendDlgItemMsg (hwnd, 
                               DID_SELECTED, BM_QUERYCHECK, 0, 0);
   if (bChecked)
      {
      /* -- Build cmdln string to do selected code tables. */

      us = ItiRptUtBuildSelectedKeysFile(hwnd,
                             MajorItemCatL,
                             cMajorItemKey,
                             szFName, sizeof szFName);

      if ((us == ITIRPTUT_PARM_ERROR)|| (us == ITIRPTUT_ERROR))
         {
         ItiRptUtErrorMsgBox (hwnd,
                "ERROR: Failed to get selected keys in "__FILE__);
         return 13;
         }
      }/* end of if (bChecked... then clause */

   else
      {/* Use a query to get all of the keys. */
      ItiStrCpy(szFName,
                " \""ITIRPTUT_KEY_QUERY_SW_STR
                "select MajorItemKey from MajorItem "
                "where Deleted is NULL"
                " order by MajorItemID"
                " \" ",
                sizeof szFName);
      }/* end of if (bChecked... else clause */



   /* -- Prep the CmdLn to use the key file. */
   ItiStrCat (szCmdLn, ITIRPT_OPTSW_KEYFILE, sizeof szCmdLn);
   ItiStrCat (szCmdLn, szFName, sizeof szCmdLn);

   us = ItiRptUtExecRptCmdLn (hwnd, szCmdLn, RPT_MJR_SESSION);

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
   ItiStrCpy (pszBuffer, "Major Item Listing", usMaxSize);   
   *pusWindowID = rMjrIt_RptDlgBox;                       
   }/* END OF FUNCTION */



BOOL EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID, 
                                     USHORT usActiveWindowID) 
   {                                                          
   if ((usTargetWindowID == rMjrIt_RptDlgBox)                     
       && ( (usActiveWindowID == MajorItemCatL )
          ||(usActiveWindowID == MajorItemL )
          ||(usActiveWindowID == MajorItemCatL )   
          ||(usActiveWindowID == MajorItemCatS ) 
          ||(usActiveWindowID == MajorItemL )
          ||(usActiveWindowID == MajorItemS ) ))
      return TRUE;                                            
   else                                                       
      return FALSE;                                           
   }/* END OF FUNCTION */




MRESULT EXPORT ItiDllrMjrItRptDlgProc (HWND     hwnd,     
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
         us = ItiRptUtInitDlgBox (hwnd, RPT_MJR_SESSION, 0, NULL);
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
               return 0L;



            case DID_CANCEL:
               WinDismissDlg (hwnd, FALSE);
               return 0L;

            }/* end of switch(SHORT1... */

         break;


      default:
         us = ItiRptUtChkDlgMsg (hwnd, usMessage, mp1, mp2);
         break;
      }/* end of switch (usMessage... */

   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);

   }/* END OF FUNCTION ItiDllrMjrItRptDlgProc */








USHORT EXPORT ItiRptDLLPrintReport
                 (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[])
   {
   PSZ     pszMjrItKey = szKey;

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



   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_DESC,
                       szQueryDesc, sizeof szQueryDesc );

   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_LISTING,
                       szQueryListing, sizeof szQueryListing );


   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QRY_BASEDATE,
                       szQryBaseDate, sizeof szQryBaseDate );


   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QRY_PRICE_REGRS,
                       szQryPriceRegrs, sizeof szQryPriceRegrs );

   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_CNJ_PRICE_REGRS,
                       szCnjPriceRegrs, sizeof szCnjPriceRegrs );


   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QRY_QTY_REGRS,
                       szQryQtyRegrs, sizeof szQryQtyRegrs );

   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_CNJ_QTY_REGRS,
                       szCnjQtyRegrs, sizeof szCnjQtyRegrs );



   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QRY_QVAR,
                       szQryQvar, sizeof szQryQvar );

   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_CNJ_QVAR,
                       szCnjQvar, sizeof szCnjQvar );



   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QRY_QV_ADJ,
                       szQryQvAdj, sizeof szQryQvAdj );

   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QRY_QEST_ADJ,
                       szQryQestAdj, sizeof szQryQestAdj );

   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_CNJ_QV_ADJ,
                       szCnjQvAdj, sizeof szCnjQvAdj );

   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_CNJ_QV_ADJ_BASEDATE,
                       szCnjQvAdjBaseDate, sizeof szCnjQvAdjBaseDate );



   /* -------------------------------------------------------------------- *\
    * -- For each MajorItemKey...                                          *
   \* -------------------------------------------------------------------- */
   ItiRptUtInitKeyList(argcnt, argvars); /*  */
   while (ItiRptUtGetNextKey(pszMjrItKey,sizeof(szKey)) != ITIRPTUT_NO_MORE_KEYS)
      {
      us = SummarySectionFor(pszMjrItKey, szMjrID);

      if (us == 0)
         {
         ItemListSectionFor           (szMjrID, pszMjrItKey);

         /* -- The following are the NON-optional Parametric chapter reports. */
         PriceEstimationSectionFor    (szMjrID, pszMjrItKey);
         QuantityEstimationSectionFor (szMjrID, pszMjrItKey);

         /* -- Finishout this section. */
         ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
         ItiPrtIncCurrentLn (); //line feed
         }

      }/* End of WHILE GetNextKey */



   /* ------------------------------------------------------------------- *\
    * Finish up and quit.                                                 *
   \* ------------------------------------------------------------------- */
   ItiPrtEndReport();

   DBGMSG("Exit ***DLL function ItiRptDLLPrintReport");
   return 0;

   }/* END OF FUNCTION ItiRptDLLPrintReport */





