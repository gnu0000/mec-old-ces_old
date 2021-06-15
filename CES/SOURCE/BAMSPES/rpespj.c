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


#include    "rindlpj.h"


#include "rPESpj.h"                                       



/* ======================================================================= *\
 *                                         MANIFEST CONSTANT DEFINITIONS   *
\* ======================================================================= */
#define  BLANK_LINE_INTERVAL    4
#define  PRT_Q_NAME             "PES Prjt Lst"

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
static CHAR szDllVersion[] = "1.1a0 rPespj.dll";

static  CHAR  szCurrentPCN  [SMARRAY + 1] = "";
static  CHAR  szOrderingStr [SMARRAY + 1] = "";
static  CHAR  szOptSw [SMARRAY + 1] = "";

static  CHAR  szFailedDLL [CCHMAXPATH + 1] = "";
static  CHAR  szErrorMsg  [1024] = "";

static  USHORT usNumOfTitles;
static  USHORT usCurrentPg = 0;
static  BOOL  bUseNewPgPerPCN;

static  CHAR  szQuery       [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szQueryLoc    [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szQueryChsAlt [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szKey         [SMARRAY + 1] = "";

static  CHAR  szQueryKeys    [2 * SMARRAY + 1] = "";
static  CHAR  szQueryListing [SIZE_OF_QUERY_LISTING + 1] = "";
static  CHAR  szQueryEstAmt  [SIZE_OF_QUERY_EST_AMT + 1] = "";
static  CHAR  szQueryCatgs   [SIZE_OF_QUERY_CATGS + 1] = "";


static  CHAR  szQryCatItmTtl    [SIZE_OF_QRY_CAT_ITM_TTL + 1] = ""; 
static  CHAR  szQryCatItm       [SIZE_OF_QRY_CAT_ITM + 1] = ""; 
static  CHAR  szQryCatItmC1     [SIZE_OF_QRY_CAT_ITM_C1 + 1] = ""; 
static  CHAR  szQryCatItmC2     [SIZE_OF_QRY_CAT_ITM_C2 + 1] = ""; 
//static  CHAR  szQryCatItmC3     [SIZE_OF_QRY_CAT_ITM_C3 + 1] = ""; 
//static  CHAR  szQryCatItmC4     [SIZE_OF_QRY_CAT_ITM_C4 + 1] = ""; 
//static  CHAR  szQryCatItmAlt    [SIZE_OF_QRY_CAT_ITM_ALT + 1] = ""; 
//static  CHAR  szQryCatItmNonAlt [SIZE_OF_QRY_CAT_ITM_NONALT + 1] = ""; 

static  CHAR  szQryCatItmChos    [SIZE_OF_QRY_CAT_ITM_CHOS + 1] = "";
static  CHAR  szQryCatItmChosCat [SMARRAY + 1] = "";
static  CHAR  szQryCatItmChosSeq [SMARRAY + 1] = "";

           /* Used by DbExecQuer & DbGetQueryRow functions: */
static  HHEAP   hhprPESpj  = NULL;
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







USHORT SetColumnTitles (PSZ pszCategoryID)
   {
   USHORT us;
   CHAR   szTtlLn[64] = "";


   ItiStrCpy(szTtlLn, acttl[RPT_TTL_9].szTitle, sizeof szTtlLn);
   ItiStrCat(szTtlLn, szCurrentPCN, sizeof szTtlLn);
   ItiStrCat(szTtlLn, ", ", sizeof szTtlLn);
   ItiStrCat(szTtlLn, acttl[RPT_TTL_10].szTitle, sizeof szTtlLn);
   ItiStrCat(szTtlLn, pszCategoryID, sizeof szTtlLn);
   ItiStrCat(szTtlLn, acttl[RPT_TTL_11].szTitle, sizeof szTtlLn);

   ItiPrtPutTextAt (CURRENT_LN, 0, CENTER, USE_RT_EDGE, szTtlLn);
   ItiPrtIncCurrentLn (); //line feed


   for (us = RPT_TTL_12; us <= RPT_TTL_18; us++)
      {
      ItiPrtPutTextAt (CURRENT_LN, acttl[us].usTtlCol,
                       LEFT, (acttl[us].usTtlColWth | REL_LEFT),
                       acttl[us].szTitle);
      }
   ItiPrtIncCurrentLn (); //line feed

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_19].usTtlCol,
                    LEFT, (acttl[RPT_TTL_19].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_19].szTitle);
   ItiPrtIncCurrentLn (); //line feed


   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
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








USHORT SummarySectionFor (PSZ pszKey, PSZ pszCurrentPCN)
   {                         /* In */    /* Out */
   USHORT usLn, us;
   PSZ * ppsz = NULL;
   PSZ pszM;


   if ((pszKey == NULL) || (*pszKey == '\0'))
      return 13;

   /* -- remove trailing blanks from key. */
   us = ItiStrLen(pszKey);
   pszM = &pszKey[us];
   while ((*(--pszM) == '\r')||(*pszM == '\n')||(*pszM == ' '))
      *pszM = '\0';

   /* -- Build the query. */
   ItiStrCpy(szQuery, szQueryListing, (sizeof szQuery)-2);
   ItiStrCat(szQuery, pszKey,         (sizeof szQuery)-2);
   us = ItiStrLen(szQuery);
   if ((us+1) < sizeof szQuery)
      {
      szQuery[us] = '\x27';
      szQuery[us+1] = '\0';
      }


   ppsz = ItiDbGetRow1 (szQuery, hhprPESpj, hmodrPESpj, ITIRID_RPT,
                        ID_QUERY_LISTING, &usNumCols);
   if (ppsz != NULL)
      {
      usLn = 9 + ItiStrLen(ppsz[DESC]) / acttl[RPT_TTL_2].usDtaColWth;
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
                       ppsz[PCN]);
      ItiStrCpy(pszCurrentPCN, ppsz[PCN], sizeof szCurrentPCN);
      /* Note: pszCurrentPCN is known to be the global szCurrentPCN string. */

      us = acttl[RPT_TTL_1].usDtaCol + ItiStrLen(ppsz[PCN]);

      ItiPrtPutTextAt (CURRENT_LN, us,
                       LEFT, (acttl[RPT_TTL_2].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_2].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, (us + acttl[RPT_TTL_2].usTtlColWth),
                       (LEFT|WRAP),
                       (2 | FROM_RT_EDGE),
                       //(acttl[RPT_TTL_2].usDtaColWth | REL_LEFT),
                       ppsz[DESC]);
      ItiPrtIncCurrentLn(); //blank line


      /* -- Work Type */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_3].usTtlCol,
                       LEFT, (acttl[RPT_TTL_3].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_3].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_3].usDtaCol,
                       LEFT, (acttl[RPT_TTL_3].usDtaColWth | REL_LEFT),
                       ppsz[WRK_ID]);

      /* -- WorkType Translation */
      us = acttl[RPT_TTL_3].usDtaCol + ItiStrLen(ppsz[WRK_ID]);

      ItiPrtPutTextAt (CURRENT_LN, us,
                       LEFT, (acttl[RPT_TTL_4].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_4].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                       LEFT, (acttl[RPT_TTL_4].usDtaColWth | REL_LEFT),
                       ppsz[WRK_TRNS]);
      ItiPrtIncCurrentLn(); //line feed


      /* -- District */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_5].usTtlCol,
                       LEFT, (acttl[RPT_TTL_5].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_5].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_5].usDtaCol,
                       LEFT, (acttl[RPT_TTL_5].usDtaColWth | REL_LEFT),
                       ppsz[DIST_ID]);
      ItiPrtIncCurrentLn(); //line feed


      /* -- Spec Year */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_6].usTtlCol,
                       LEFT, (acttl[RPT_TTL_6].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_6].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_6].usDtaCol,
                       LEFT, (acttl[RPT_TTL_6].usDtaColWth | REL_LEFT),
                       ppsz[YEAR]);
      ItiPrtIncCurrentLn(); //line feed


      /* -- Location */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_7].usTtlCol,
                       LEFT, (acttl[RPT_TTL_7].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_7].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_7].usDtaCol,
                       LEFT, (acttl[RPT_TTL_7].usDtaColWth | REL_LEFT),
                       ppsz[LOC]);
      ItiPrtIncCurrentLn(); //line feed
      ItiPrtIncCurrentLn(); //blank line

      ItiFreeStrArray (hhprPESpj, ppsz, usNumCols);




      /* -- Now compute the Estimate: */
      /* -- Build the query. */
      ItiStrCpy(szQuery, szQueryEstAmt, (sizeof szQuery)-2);
      ItiStrCat(szQuery, pszKey,        (sizeof szQuery)-2);
      us = ItiStrLen(szQuery);
      if ((us+1) < sizeof szQuery)
         {
         szQuery[us] = '\x27';
         szQuery[us+1] = '\0';
         }


      ppsz = ItiDbGetRow1 (szQuery, hhprPESpj, hmodrPESpj, ITIRID_RPT,
                           ID_QUERY_EST_AMT, &usNumCols);
      if (ppsz != NULL)
         {
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_8].usTtlCol,
                          LEFT, (acttl[RPT_TTL_8].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_8].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_8].usDtaCol,
                          LEFT, (acttl[RPT_TTL_8].usDtaColWth | REL_LEFT),
                          ppsz[EST_AMT]);
         ItiPrtIncCurrentLn(); //line feed

         ItiFreeStrArray (hhprPESpj, ppsz, usNumCols);
         }/* end of if EstAmt qurey then clause. */
      else
         {
         SETQERRMSG;
         return 13;
         }


      ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
      ItiPrtIncCurrentLn (); //line feed
      }
   else
      {
      SETQERRMSG;
      return 13;
      }

   return 0;
   }/* End of Function SummarySectionFor */







USHORT CategoryItemList(PSZ pszCategoryID, PSZ pszCurrentPCN, PULONG pulCatTotal)
   {
   USHORT us, usLn;
   HQRY    hqryLoc   = NULL;
   USHORT  usNumColsLoc = 0;
   USHORT  usErrLoc     = 0;
   PSZ   * ppszLoc   = NULL;
   ULONG   ulExtRaw;

   /* Chosen-Alternate */
   CHAR    szQueryChsAlt[512] = "";
   HQRY    hqryChsAlt   = NULL;
   USHORT  usNumColsChsAlt = 0;
   USHORT  usErrChsAlt     = 0;
   PSZ   * ppszChsAlt   = NULL;




   /* -- Build the Category Items query. */
   ItiStrCpy(szQueryLoc, szQryCatItm,   sizeof szQueryLoc);
   ItiStrCat(szQueryLoc, szQryCatItmC1, sizeof szQueryLoc);
   ItiStrCat(szQueryLoc, pszCurrentPCN,  sizeof szQueryLoc);
   ItiStrCat(szQueryLoc, "' ",          sizeof szQueryLoc);
   ItiStrCat(szQueryLoc, szQryCatItmC2, sizeof szQueryLoc);
   ItiStrCat(szQueryLoc, pszCategoryID, sizeof szQueryLoc);
   ItiStrCat(szQueryLoc, "' order by 1 ", sizeof szQueryLoc);
    
   /* -- Do the database query. */
   hqryLoc = ItiDbExecQuery (szQueryLoc, hhprPESpj, hmodrPESpj, ITIRID_RPT,
                          ID_QRY_CAT_ITM, &usNumColsLoc, &usErrLoc);
   
   /* -- Process the query results for each row. */
   if (hqryLoc == NULL)
      {
      SETQERRMSG;
      }
   else
      {
      *pulCatTotal = 0L;
      usLn = 6; /* First time thru, column titles and close total lines. */
      while( ItiDbGetQueryRow (hqryLoc, &ppszLoc, &usErrLoc) )  
         {
         /* -- Check for new page. */
         if ((ItiPrtKeepNLns(usLn) == PG_WAS_INC)
             || (ItiPrtQueryCurrentPgNum() != usCurrentPg ))
            {
            usCurrentPg = ItiPrtQueryCurrentPgNum();
            ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
            SetColumnTitles(pszCategoryID);
            }

         /* -- Display Item info. */
         /* Sequence Number: */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usDtaCol,
                          LEFT, (acttl[RPT_TTL_12].usDtaColWth | REL_LEFT),
                          ppszLoc[SEQ]);

         /* Item Number: */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_13].usDtaCol,
                          LEFT, (acttl[RPT_TTL_13].usDtaColWth | REL_LEFT),
                          ppszLoc[ITEM]);

         /* Quantity: */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_14].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_14].usDtaColWth | REL_LEFT),
                          ppszLoc[QUAN]);

         /* "@" */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_20].usTtlCol,
                          LEFT, (acttl[RPT_TTL_20].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_20].szTitle);

         /* Price: */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_15].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_15].usDtaColWth | REL_LEFT),
                          ppszLoc[PRICE]);
         /* "/" */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_16].usTtlCol,
                          LEFT, (acttl[RPT_TTL_16].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_16].szTitle);

         /* Unit: */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_17].usDtaCol,
                          LEFT, (acttl[RPT_TTL_17].usDtaColWth | REL_LEFT),
                          ppszLoc[UNIT]);

         /* "=" */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_21].usTtlCol,
                          LEFT, (acttl[RPT_TTL_21].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_21].szTitle);

         /* Extention: */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_18].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_18].usDtaColWth | REL_LEFT),
                          ppszLoc[EXT]);
         

         /* -- Need to modify non-chosen alternate item w/strike thru */
         ItiStrCpy(szQueryChsAlt, szQryCatItmChos, sizeof szQueryChsAlt);
         ItiStrCat(szQueryChsAlt, pszCurrentPCN, sizeof szQueryChsAlt);
         ItiStrCat(szQueryChsAlt, "' ", sizeof szQueryChsAlt);
         ItiStrCat(szQueryChsAlt, szQryCatItmChosCat, sizeof szQueryChsAlt);
         ItiStrCat(szQueryChsAlt, pszCategoryID, sizeof szQueryChsAlt);
         ItiStrCat(szQueryChsAlt, "' ", sizeof szQueryChsAlt);
         ItiStrCat(szQueryChsAlt, szQryCatItmChosSeq, sizeof szQueryChsAlt);
         ItiStrCat(szQueryChsAlt, ppszLoc[SEQ], sizeof szQueryChsAlt);
         ItiStrCat(szQueryChsAlt, "' ", sizeof szQueryChsAlt);

//       ItiRptUtErrorMsgBox (NULL, szQueryChsAlt);

         ppszChsAlt = ItiDbGetRow1 (szQueryChsAlt, hhprPESpj, hmodrPESpj,
                         ITIRID_RPT, ID_QRY_CAT_ITM_CHOS, &usNumColsChsAlt);

         if ((ppszChsAlt == NULL) ||
             ((ppszChsAlt != NULL) && (ppszChsAlt[0] == NULL)))
           {
            /* Then draw a strike-out line thru the extension    *
             * because we did NOT get a chosen alt. key for this *
             * item, so it must be a NON-Chosen alternate.       */
           us = ItiStrLen(ppszLoc[EXT]);
           ItiPrtDrawSeparatorLn(CURRENT_LN,
              (acttl[RPT_TTL_18].usDtaCol
               + acttl[RPT_TTL_18].usDtaColWth - us),
               acttl[RPT_TTL_18].usDtaCol + acttl[RPT_TTL_18].usDtaColWth -1);
           }
         else
            {/* Since it is a chosen-alt. add its extension *
              * to the category's running total.            */
            ItiStrToULONG(ppszLoc[EXT_RAW], &ulExtRaw);
            *pulCatTotal = *pulCatTotal + ulExtRaw;
            }

         if (ppszChsAlt != NULL)
            ItiFreeStrArray (hhprPESpj, ppszChsAlt, usNumColsChsAlt);



         ItiPrtIncCurrentLn(); //line feed


         /* Short Description: */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_19].usDtaCol,
                          LEFT, (acttl[RPT_TTL_19].usDtaColWth | REL_LEFT),
                          ppszLoc[ITM_DESC]);
         us = ItiPrtIncCurrentLn(); //line feed

         ItiFreeStrArray (hhprPESpj, ppszLoc, usNumColsLoc);
         }/* end of while */
      }/* end of else if (hqryLoc... */

   return us;
   }/* End of Function CategoryItemList */

        





USHORT CategoryItemTotal(PSZ pszCategoryID, PSZ pszCurPCN, ULONG ulCatTotal)
   {
   USHORT  us;
   HQRY    hqryLoc   = NULL;
   USHORT  usNumColsLoc = 0;
   USHORT  usErrLoc     = 0;
   PSZ   * ppszLoc   = NULL;
   CHAR    szTmp[SMARRAY +1] = "";
   CHAR    szCatTtlRaw[SMARRAY +1] = "";
   CHAR    szCatTtlFmt[SMARRAY +1] = "";




   ItiPrtDrawSeparatorLn(CURRENT_LN, acttl[RPT_TTL_18].usDtaCol,
                 acttl[RPT_TTL_18].usDtaCol + acttl[RPT_TTL_18].usDtaColWth);
   ItiPrtIncCurrentLn (); //line feed

   ItiStrCpy(szTmp, acttl[RPT_TTL_10].szTitle, sizeof szTmp);
   ItiStrCat(szTmp, pszCategoryID, sizeof szTmp);
   ItiStrCat(szTmp, acttl[RPT_TTL_22].szTitle, sizeof szTmp);
   us = ItiStrLen(szTmp);

   ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_18].usDtaCol - us, LEFT,
                   (acttl[RPT_TTL_18].usDtaCol | REL_LEFT),
                   szTmp);


//   /* -- Build the Category Item Total query. */
//   ItiStrCpy(szQueryLoc, szQryCatItmTtl, sizeof szQueryLoc);
//   ItiStrCat(szQueryLoc, pszCurPCN,      sizeof szQueryLoc);
//   ItiStrCat(szQueryLoc, "' ",           sizeof szQueryLoc);
//   ItiStrCat(szQueryLoc, szQryCatItmC2,  sizeof szQueryLoc);
//   ItiStrCat(szQueryLoc, pszCategoryID,  sizeof szQueryLoc);
//   ItiStrCat(szQueryLoc, "' ", sizeof szQueryLoc);
//
//   ppszLoc = ItiDbGetRow1 (szQueryLoc, hhprPESpj, hmodrPESpj, ITIRID_RPT,
//                        ID_QRY_CAT_ITM_TTL, &usNumColsLoc);
//   if (ppszLoc != NULL)
//      {
//      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_18].usDtaCol,
//                       RIGHT, (acttl[RPT_TTL_18].usDtaColWth | REL_LEFT),
//                       ppszLoc[CAT_ITM_TTL]);
//          
//      ItiFreeStrArray (hhprPESpj, ppszLoc, usNumColsLoc);
//      }/* end of if */

   ItiStrULONGToString(ulCatTotal, szCatTtlRaw, sizeof szCatTtlRaw);
   ItiFmtFormatString (szCatTtlRaw,  &szCatTtlFmt[0], sizeof szCatTtlFmt,
                       "Number,$,..", &us);

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_18].usDtaCol,
                    RIGHT, (acttl[RPT_TTL_18].usDtaColWth | REL_LEFT),
                    szCatTtlFmt);


   us = ItiPrtIncCurrentLn(); //line feed

   return us;
   }/* End of Function CategoryItemTotal */






USHORT Categories (PSZ pszKey, PSZ pszCurPCN)
   {
   USHORT usLn;
   ULONG  ulCategoryTotal;
   BOOL   bNewID   = TRUE;
   BOOL   bLabeled = FALSE;


   if ((pszKey == NULL) || (*pszKey == '\0') || (pszCurPCN == NULL))
      return 13;

   /* -- Build the query. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryCatgs, pszKey, "' order by 1 ");

    
   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hhprPESpj, hmodrPESpj, ITIRID_RPT,
                          ID_QUERY_CATGS, &usNumCols, &usErr);
   
   /* -- Process the query results for each row. */
   if (hqry == NULL)
      {
      SETQERRMSG;
      }
   else
      {
      ulCategoryTotal = 0L;
      usLn = 9; /* First time thru, column titles and close total lines. */
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
         {
         /* -- Check for new page. */
         if ((ItiPrtKeepNLns(usLn) == PG_WAS_INC)
             || (ItiPrtQueryCurrentPgNum() != usCurrentPg ))
            {
            usCurrentPg = ItiPrtQueryCurrentPgNum();
            ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
            }


         SetColumnTitles(ppsz[CATEGORY_ID]);

         /* -- Generate item list with current row's CategoryID. */
         CategoryItemList(ppsz[CATEGORY_ID], pszCurPCN, &ulCategoryTotal);


         /* -- Section total. */
         CategoryItemTotal(ppsz[CATEGORY_ID], pszCurPCN, ulCategoryTotal);


         /* -- Finishout this section. */
//         ItiPrtIncCurrentLn (); //blank line 
         ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
         ItiPrtIncCurrentLn (); //line feed

         ItiFreeStrArray (hhprPESpj, ppsz, usNumCols);
         }/* End of WHILE GetQueryRow */

      }/* else clause */
   
   }/* End of Function Categories */






USHORT PrintProc (HWND hwnd, MPARAM mp1, MPARAM mp2)
   {
   USHORT us    = 0;
   BOOL   bChecked = FALSE;
   HFILE  hAltFile;
   HHEAP  hhprLocal = NULL;
   CHAR   szKey        [SMARRAY] = "";
   CHAR   szOptSwStr   [SMARRAY] = "";
   CHAR   szCmdLn      [2*SMARRAY +1] = "";
   CHAR   szKeyFileName[CCHMAXPATH + 1] = "";



   /* -- reset the command line. */
   ItiStrCpy (szCmdLn, RPT_DLL_NAME, sizeof(szCmdLn) );



   /* -- Check if each Project is to start on its own page. */
   bChecked = (USHORT) (ULONG) WinSendDlgItemMsg (hwnd, 
                               DID_NEWPG, BM_QUERYCHECK, 0, 0);
   if (bChecked)
      {
      ItiStrCat (szCmdLn, ITIRPT_OPTSW_NEWPG_STR, sizeof szCmdLn);
      }

   


   bChecked = (USHORT) (ULONG) WinSendDlgItemMsg (hwnd, 
                               DID_SELECTED, BM_QUERYCHECK, 0, 0);
   if (bChecked)
      {
      /* -- Build cmdln string to do those selected. */
      us = ItiRptUtBuildSelectedKeysFile(hwnd,
                             ProjectsForJobL,
                             cProjectControlNumber,
                             szKeyFileName, sizeof szKeyFileName);

      if ((us == ITIRPTUT_PARM_ERROR)|| (us == ITIRPTUT_ERROR))
         ItiRptUtErrorMsgBox (hwnd,
                "ERROR: Failed to get selected keys in "__FILE__);
      else
         {/* -- Prep the CmdLn to use the key file. */
         ItiStrCat (szCmdLn, ITIRPT_OPTSW_KEYFILE, sizeof szCmdLn);
         ItiStrCat (szCmdLn, szKeyFileName, sizeof szCmdLn);
         }/* end of if ((us... */

      }/* end of if (bChecked... then clause */
   else
      {/* Do ALL of the keys. */
      hhprLocal = ItiMemCreateHeap (MIN_HEAP_SIZE);

      ItiRptUtUniqueFile (szKeyFileName, sizeof szKeyFileName, TRUE);

      /* -- Build the cost sheet key list query. */
      ItiMbQueryQueryText(hmodrPESpj, ITIRID_RPT, ID_QUERY_PCN,
                          szQueryKeys, sizeof szQueryKeys );


      /* -- Do the database query. */
      hqry = ItiDbExecQuery (szQueryKeys, hhprLocal, hmodrPESpj, ITIRID_RPT,
                             ID_QUERY_PCN, &usNumCols, &usErr);
      
      /* ---------------------------------------------------------------- *\
       * Process the query results for each row.                          *
      \* ---------------------------------------------------------------- */
      if (hqry == NULL)
         {
         SETQERRMSG;
         }                     
      else
         {
         ItiRptUtStartKeyFile (&hAltFile, szKeyFileName, sizeof szKeyFileName);

         while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )
            {
            ItiRptUtWriteToKeyFile (hAltFile, ppsz[PCN], NULL);
            ItiFreeStrArray (hhprLocal, ppsz, usNumCols);
            }/* End of while... */

         ItiRptUtEndKeyFile (hAltFile);

         ItiStrCat (szCmdLn, ITIRPT_OPTSW_KEYFILE, sizeof szCmdLn);
         ItiStrCat (szCmdLn, szKeyFileName, sizeof szCmdLn);
         }/* else clause */

      }/* end of if (bChecked... else clause */


   us = ItiRptUtExecRptCmdLn (hwnd, szCmdLn, RPT_RPESPJ_SESSION);

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
   ItiStrCpy (pszBuffer, "Project Listing (PES)", usMaxSize);   
   *pusWindowID = rPESpj_RptDlgBox;                       
   }/* END OF FUNCTION */




BOOL EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID, 
                                     USHORT usActiveWindowID) 
   {                                                          
   if ((usTargetWindowID == rPESpj_RptDlgBox)                     
       && (   (usActiveWindowID == ProjectsForJobS)
           || (usActiveWindowID == ProjectCategoryS)
           || (usActiveWindowID == ProjectItemS)
           || (usActiveWindowID == ProjectsForJobL)
           || (usActiveWindowID == ProjectCategoryL)
           || (usActiveWindowID == ProjectItemL1)
           || (usActiveWindowID == ProjectItemL2)      ))
      return TRUE;                                            
   else                                                       
      return FALSE;                                           
   }/* END OF FUNCTION */



MRESULT EXPORT ItiDllrPESpjRptDlgProc (HWND     hwnd,     
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
         us = ItiRptUtInitDlgBox (hwnd, RPT_RPESPJ_SESSION, 0, NULL);
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
   hhprPESpj = ItiMemCreateHeap (MIN_HEAP_SIZE);


   /* ------------------------------------------------------------------- *\
    * Initialize the strings.                                             *
   \* ------------------------------------------------------------------- */
   ItiStrCpy(szCurrentPCN,"--PCN--",sizeof(szCurrentPCN));

   /* -- Get any query ordering information, BEFORE setting rpt titles. */
   ItiRptUtGetOption (ITIRPTUT_ORDERING_SWITCH, szOrderingStr, LGARRAY,
                      argcnt, argvars);
   ConvertOrderingStr (szOrderingStr);


   /* -- Get user option to use a new page for each cost sheet. */
   ItiRptUtGetOption (ITIRPT_OPTSW_NEWPG_CHR, szOptSw, sizeof szOptSw,
                      argcnt, argvars);
   if (szOptSw[0] == '\0') 
      bUseNewPgPerPCN = FALSE;
   else
      bUseNewPgPerPCN = TRUE;




   /* -- Get the report's titles for this DLL. */
   ItiRptUtLoadLabels (hmodrPESpj, &usNumOfTitles, &acttl[0]);

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



   ItiMbQueryQueryText(hmodrPESpj, ITIRID_RPT, ID_QUERY_PCN,
                       szQueryKeys, sizeof szQueryKeys );

   ItiMbQueryQueryText(hmodrPESpj, ITIRID_RPT, ID_QUERY_LISTING,
                       szQueryListing, sizeof szQueryListing );

   ItiMbQueryQueryText(hmodrPESpj, ITIRID_RPT, ID_QUERY_EST_AMT,
                       szQueryEstAmt, sizeof szQueryEstAmt );


   ItiMbQueryQueryText(hmodrPESpj, ITIRID_RPT, ID_QUERY_CATGS,
                       szQueryCatgs, sizeof szQueryCatgs );


   ItiMbQueryQueryText(hmodrPESpj, ITIRID_RPT, ID_QRY_CAT_ITM_TTL,
                       szQryCatItmTtl, sizeof szQryCatItmTtl );

   ItiMbQueryQueryText(hmodrPESpj, ITIRID_RPT, ID_QRY_CAT_ITM,
                       szQryCatItm, sizeof szQryCatItm );

   ItiMbQueryQueryText(hmodrPESpj, ITIRID_RPT, ID_QRY_CAT_ITM_C1,
                       szQryCatItmC1, sizeof szQryCatItmC1 );

   ItiMbQueryQueryText(hmodrPESpj, ITIRID_RPT, ID_QRY_CAT_ITM_C2,
                       szQryCatItmC2, sizeof szQryCatItmC2 );

//   ItiMbQueryQueryText(hmodrPESpj, ITIRID_RPT, ID_QRY_CAT_ITM_C3,
//                       szQryCatItmC3, sizeof szQryCatItmC3 );
//
//   ItiMbQueryQueryText(hmodrPESpj, ITIRID_RPT, ID_QRY_CAT_ITM_C4,
//                       szQryCatItmC4, sizeof szQryCatItmC4 );
//
//   ItiMbQueryQueryText(hmodrPESpj, ITIRID_RPT, ID_QRY_CAT_ITM_ALT,
//                       szQryCatItmAlt, sizeof szQryCatItmAlt );
//
//   ItiMbQueryQueryText(hmodrPESpj, ITIRID_RPT, ID_QRY_CAT_ITM_NONALT,
//                       szQryCatItmNonAlt, sizeof szQryCatItmNonAlt );

   ItiMbQueryQueryText(hmodrPESpj, ITIRID_RPT, ID_QRY_CAT_ITM_CHOS,
                       szQryCatItmChos, sizeof szQryCatItmChos );

   ItiMbQueryQueryText(hmodrPESpj, ITIRID_RPT, ID_QRY_CAT_ITM_CHOS_CAT,
                       szQryCatItmChosCat, sizeof szQryCatItmChosCat );

   ItiMbQueryQueryText(hmodrPESpj, ITIRID_RPT, ID_QRY_CAT_ITM_CHOS_SEQ,
                       szQryCatItmChosSeq, sizeof szQryCatItmChosSeq );


   /* -------------------------------------------------------------------- *\
    * -- For each Key...                                                   *
   \* -------------------------------------------------------------------- */
   ItiRptUtInitKeyList(argcnt, argvars); /*  */
   while (ItiRptUtGetNextKey(pszKey,sizeof(szKey)) != ITIRPTUT_NO_MORE_KEYS)
      {
      us = SummarySectionFor(pszKey, &szCurrentPCN[0]);
                   /* The current PCN value for the given key is now set. */
      if (us == 0)
         us = Categories(pszKey, szCurrentPCN);


      /* --  New page for new project. */
      if (bUseNewPgPerPCN)
         {
         ItiPrtNewPg();
         }

      }/* End of WHILE GetNextKey */



   /* ------------------------------------------------------------------- *\
    * Finish up and quit.                                                 *
   \* ------------------------------------------------------------------- */
   ItiPrtEndReport();

   DBGMSG("Exit ***DLL function ItiRptDLLPrintReport");
   return 0;

   }/* END OF FUNCTION ItiRptDLLPrintReport */





