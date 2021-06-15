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


#include    "rindlcst.h"


#define RCSTSHT
#include "rCstSh.h"                                       



/* ======================================================================= *\
 *                                         MANIFEST CONSTANT DEFINITIONS   *
\* ======================================================================= */
#define  BLANK_LINE_INTERVAL    4
#define  PRT_Q_NAME             "CstSht Listing"

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
static  HMODULE hmodrCstSh;
static  CHAR  szCompileDateTime[] = __DATE__ " " __TIME__ ;
static CHAR szDllVersion[] = "1.1a0 rcstsh.dll";

static  CHAR  szKeyFileName [CCHMAXPATH + 1] = "";

static  CHAR  szFailedDLL [CCHMAXPATH + 1] = "";
static  CHAR  szErrorMsg  [1023] = "";
static  CHAR  szOptSw [SMARRAY + 1] = "";

static  USHORT usNumOfTitles = 0;
static  USHORT usLabelsLoaded = 13;
static  USHORT usCurrentPg = 0;
static  BOOL   bUseNewPgPerCstSht = FALSE;
static  BOOL   bFirstCstSht = TRUE;
static  BOOL   bQryStrInit = FALSE;

static  CHAR  szCurrentID   [SMARRAY + 1] = "";
static  PSZ   pszCurrentID = szCurrentID;
static  CHAR  szOrderingStr [SMARRAY + 1] = "";

/* -- Variables for the BrkDwn Section. */
static  CHAR  szMaterialPct    [SMARRAY + 1] = "";
static  CHAR  szEquipmentPct   [SMARRAY + 1] = "";
static  CHAR  szLaborPct       [SMARRAY + 1] = "";
static  CHAR  szOverHeadPct    [SMARRAY + 1] = "";
static  CHAR  szOverHeadPctRaw [SMARRAY + 1] = "";

static  CHAR  szEquipmentTtl [SMARRAY + 1] = " 0.0 ";
static  CHAR  szLaborTtl     [SMARRAY + 1] = " 0.0 ";
static  CHAR  szMaterialTtl  [SMARRAY + 1] = " 0.0 ";

static  CHAR  szLbrTtl [SMARRAY] = "";
static  CHAR  szEqpTtl [SMARRAY] = "";


/* -- Queries */
static  CHAR  szBaseQuery   [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szQuery       [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szKey         [SMARRAY + 1] = "";

static  CHAR  szQueryCstShtKeys   [3 * SMARRAY] = "";

static  CHAR  szQueryListing      [SIZE_OF_QUERY_LISTING + 1] = "";
static  CHAR  szQueryMat          [SIZE_OF_QUERY_MAT + 1] = "";
static  CHAR  szQueryMatSum       [SIZE_OF_QUERY_MAT_SUM + 1] = "";
static  CHAR  szQueryMatTtl       [SIZE_OF_QUERY_MAT_TTL + 1] = "";

static  CHAR  szQueryCrews        [SIZE_OF_QUERY_CREWS + 1] = "";
static  CHAR  szQueryCrewsEqp     [SIZE_OF_QUERY_CREWS_EQP + 1] = "";
static  CHAR  szQueryCrewsEqpConj [SIZE_OF_QUERY_CREWS_EQP_CONJ + 1] = "";
static  CHAR  szQueryCrewsEqpTtl  [SIZE_OF_QUERY_CREWS_EQP_TTL + 1] = "";
static  CHAR  szQueryCrewsLbr     [SIZE_OF_QUERY_CREWS_LBR + 1] = "";
static  CHAR  szQueryCrewsLbrTtl  [SIZE_OF_QUERY_CREWS_LBR_TTL + 1] = "";

static  CHAR  szQueryCstShtProd   [SIZE_OF_QUERY_CSTSHT_PROD + 1] = "";

static  CHAR  szQueryEqpTtl       [SIZE_OF_QUERY_EQP_TTL + 1] = "";
static  CHAR  szQueryLbrTtl       [SIZE_OF_QUERY_LBR_TTL + 1] = "";


/* -- The following is used to indicate which report sections to print. */
#define SECTIONS_ALL            0
#define SECTIONS_SUMMARY_ONLY   1

static  USHORT  usRetOptSw = SECTIONS_ALL;


           /* Used by DbExecQuer & DbGetQueryRow functions: */
static  HHEAP   hhprCstSh  = NULL;
static  HHEAP   hhpBrkDwnSect = NULL;
static  HHEAP   hhpMatSect = NULL;
static  HHEAP   hhpCrewSect = NULL;
static  HHEAP   hhpSetID = NULL;

static  HQRY    hqry   = NULL;
static  USHORT  usResId   = ITIRID_RPT;
static  USHORT  usId      = ID_QUERY_LISTING;
static  USHORT  usNumCols = 0;
static  USHORT  usErr     = 0;
static  PSZ   * ppsz   = NULL;

           /* Title strings for this module. */
static  REPTITLES rept;
static  PREPT     preptCS = &rept;

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







USHORT SetColumnTitles (USHORT usSection, PSZ pszID)
   /* -- Print the column titles. */
   {
   USHORT us;
   USHORT usFirst;
   USHORT usLast;
   CHAR   szSubHeader[80] = "Cost Sheet ";


   /* Subsection title header line. */
   ItiStrCat(szSubHeader, pszID, sizeof szSubHeader);
   ItiStrCat(szSubHeader,
             acttl[(SUBHEADER_INX + usSection)].szTitle, sizeof szSubHeader);
   ItiPrtPutTextAt(CURRENT_LN, 0, CENTER, USE_RT_EDGE, szSubHeader);
   ItiPrtIncCurrentLn();

   switch (usSection)
      {
      case BRKDWN:
         usLast = SUBHEADER_INX + usSection + BRKDWN_OFF;
         break;

      case MATERIAL:
         usLast = SUBHEADER_INX + usSection + MATERIAL_OFF;
         break;

      case CREW:
         usLast = SUBHEADER_INX + usSection + CREW_OFF;
         break;

      default:
         return 13;
         break;
      }

   usFirst = SUBHEADER_INX + usSection + 1;

   for (us=usFirst; us<=usLast; us++)
      {
      ItiPrtPutTextAt (CURRENT_LN, acttl[us].usTtlCol,
                       LEFT, (acttl[us].usTtlColWth | REL_LEFT),
                       acttl[us].szTitle);

      if((us == RPT_TTL_23) || /* Extension        */
         (us == RPT_TTL_27) || /* Crew Description */
         (us == RPT_TTL_31))   /* Daily work       */
         {
         ItiPrtIncCurrentLn (); //line feed
         }

      }/* end for loop */




   ItiPrtIncCurrentLn (); //line feed

   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   ItiPrtIncCurrentLn (); //line feed

   return usErr;
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


USHORT SetCurrentID (PSZ pszKey, PSZ * ppszID)
   {
   static USHORT usNumberOfCols = 0;
   static PSZ * ppsz = NULL;
   static CHAR  szLocQuery  [QUERY_ARRAY_SIZE + 1] = "";

   if ((pszKey == NULL) || (*pszKey == '\0'))
      return 13;

   /* ------------------------------------------------------------------- *\
    *  Create a memory heap.                                              *
   \* ------------------------------------------------------------------- */
   if (NULL == hhpSetID)
      hhpSetID = ItiMemCreateHeap (4096);


   /* -- Build the query. */
   ItiRptUtPrepQuery(szLocQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryListing, " ", pszKey);

   ppsz = ItiDbGetRow1 (szLocQuery, hhpSetID, hmodrCstSh, ITIRID_RPT,
                        ID_QUERY_LISTING, &usNumberOfCols);
   if (ppsz != NULL)
      {
      /* -- Now save the percentages: */
      ItiStrCpy(szMaterialPct , ppsz[MAT_OVRHD_PCT], sizeof szMaterialPct );
      ItiStrCpy(szEquipmentPct, ppsz[LBR_OVRHD_PCT], sizeof szEquipmentPct);
      ItiStrCpy(szLaborPct    , ppsz[EQP_OVRHD_PCT], sizeof szLaborPct    );
      ItiStrCpy(szOverHeadPct , ppsz[MARKUP_PCT],     sizeof szOverHeadPct );
      ItiStrCpy(szOverHeadPctRaw,ppsz[MARKUP_PCT_RAW],sizeof szOverHeadPctRaw);

      ItiStrCpy((*ppszID), ppsz[CSTSHT_ID], sizeof szCurrentID);
      ItiMemDestroyHeap (hhpSetID);
      hhpSetID = NULL;
      return 0;
      }

   ItiMemDestroyHeap (hhpSetID);
   hhpSetID = NULL;
   return 13;
   }/* End of SetCurrentID */



USHORT SummarySectionFor (PSZ pszKey, PSZ * ppszID, BOOL bPrint)
   {
   USHORT usLn, us;
   PSZ * ppsz = NULL;


   if ((pszKey == NULL) || (*pszKey == '\0'))
      return 13;


   /* ------------------------------------------------------------------- *\
    *  Create a memory heap, if needed.                                   *
   \* ------------------------------------------------------------------- */
   if (NULL == hhprCstSh)
      hhprCstSh = ItiMemCreateHeap (MAX_HEAP_SIZE);


   /* --  New page for new cost sheet. */
   if ((bUseNewPgPerCstSht) && (!bFirstCstSht) && (bPrint))
      {
      ItiPrtNewPg();
      }


   /* -- Build the query. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryListing, " ", pszKey);


   ppsz = ItiDbGetRow1 (szQuery, hhprCstSh, hmodrCstSh, ITIRID_RPT,
                        ID_QUERY_LISTING, &usNumCols);


   if (ppsz != NULL)
      {
      bFirstCstSht = FALSE; /* reset for subsequent cost sheets. */
      if (bPrint)
         {
         usLn = 5 + ItiStrLen(ppsz[DESC]) / acttl[RPT_TTL_2].usDtaColWth;
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
                          ppsz[CSTSHT_ID]);
         }

      ItiStrCpy((*ppszID), ppsz[CSTSHT_ID], sizeof szCurrentID);
   
      if (bPrint)
         {
         us = acttl[RPT_TTL_1].usDtaCol + ItiStrLen(ppsz[CSTSHT_ID]);
   
         ItiPrtPutTextAt (CURRENT_LN, us,
                          LEFT, (acttl[RPT_TTL_2].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_2].szTitle);
   
         ItiPrtPutTextAt (CURRENT_LN, (us + acttl[RPT_TTL_2].usTtlColWth),
                          (LEFT|WRAP),
                          (3 | FROM_RT_EDGE),
                          //(acttl[RPT_TTL_2].usDtaColWth | REL_LEFT),
                          ppsz[DESC]);

          /* Added unit system for metric release. */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_3].usTtlCol,
                          LEFT, (22 | REL_LEFT),
                          "Measurement system:");

         if ((*ppsz[CS_UNIT_SYS]) != 'N')
            ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                             LEFT, (10 | REL_LEFT),
                             ppsz[CS_UNIT_SYS]);
         else
            ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                             LEFT, (40 | REL_LEFT),
                             "Both (Materials will not be listed.)");
         ItiPrtIncCurrentLn();


         /* Produtivity assumptions: */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_3].usTtlCol,
                          LEFT, (acttl[RPT_TTL_3].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_3].szTitle);
   
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_3].usDtaCol,
                          LEFT, (acttl[RPT_TTL_3].usDtaColWth | REL_LEFT),
                          ppsz[PROD_RATE]);
   
         ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                          LEFT, (acttl[RPT_TTL_4].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_4].szTitle);
         ItiPrtIncCurrentLn();


         /* Default hours/day */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_5].usTtlCol,
                          LEFT, (acttl[RPT_TTL_5].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_5].szTitle);
   
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_5].usDtaCol,
                          LEFT, (acttl[RPT_TTL_5].usDtaColWth | REL_LEFT),
                          ppsz[DEF_HRS_PERDAY]);
   
         ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                          LEFT, (acttl[RPT_TTL_6].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_6].szTitle);
         ItiPrtIncCurrentLn();
   
         /* Production granularity */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_7].usTtlCol,
                          LEFT, (acttl[RPT_TTL_7].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_7].szTitle);
   
         ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                          LEFT, (acttl[RPT_TTL_7].usDtaColWth | REL_LEFT),
                          ppsz[PROD_GRAN]);
         ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL,
                          LEFT, (3 | REL_LEFT),
                          "s.");
         ItiPrtIncCurrentLn();
         }

      /* -- Now save the percentages: */
      ItiStrCpy(szMaterialPct , ppsz[MAT_OVRHD_PCT], sizeof szMaterialPct );
      ItiStrCpy(szEquipmentPct, ppsz[LBR_OVRHD_PCT], sizeof szEquipmentPct);
      ItiStrCpy(szLaborPct    , ppsz[EQP_OVRHD_PCT], sizeof szLaborPct    );
      ItiStrCpy(szOverHeadPct , ppsz[MARKUP_PCT],     sizeof szOverHeadPct );
      ItiStrCpy(szOverHeadPctRaw,ppsz[MARKUP_PCT_RAW],sizeof szOverHeadPctRaw);

      if (bPrint)
         {
         /* Draw separator line. */
         ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
         ItiPrtIncCurrentLn (); //line feed
         }

      ItiFreeStrArray (hhprCstSh, ppsz, usNumCols);
      }
   else
      return 13;

   return usRetOptSw;
   }/* End of Function SummarySectionFor */







USHORT MatSectionFor (PSZ pszKey, PSZ pszID)
   {
   USHORT usLn, usStart, usStop;
   BOOL   bNewID   = TRUE;
   BOOL   bLabeled;
   CHAR   szTmp[SMARRAY] = "";
   HQRY    hqry   = NULL;
   USHORT  usResId   = ITIRID_RPT;
   USHORT  usId      = ID_QUERY_LISTING;
   USHORT  usNumCols = 0;
   USHORT  usErr     = 0;
   PSZ   * ppsz   = NULL;


   if ((pszKey == NULL) || (*pszKey == '\0') || (pszID == NULL))
      return 13;

   /* ------------------------------------------------------------------- *\
    *  Create a memory heap.                                              *
   \* ------------------------------------------------------------------- */
   if (NULL == hhpMatSect)
      hhpMatSect = ItiMemCreateHeap (MAX_HEAP_SIZE);


   /* -- Build the query. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryMat, pszKey, " order by 1 ");

   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hhpMatSect, hmodrCstSh, ITIRID_RPT,
                          ID_QUERY_MAT, &usNumCols, &usErr);
   
   /* -- Process the query results for each row. */
   if (hqry == NULL)
      {
      SETQERRMSG;
      }
   else
      {
      bLabeled = FALSE;
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
         {
         szTmp[0] = '\0';

         usLn = ItiStrLen(ppsz[MAT_DESC]) + 1;
         usLn = 8 + (usLn / acttl[RPT_TTL_24].usDtaColWth);
         if (!bLabeled)
            {
            if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
               {
               ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
               }
            SetColumnTitles(MATERIAL, pszID);
            bLabeled = TRUE;
            }
         else
            {
            if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
               {
               ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
               SetColumnTitles(MATERIAL, pszID);
               }
            }/* end if(!bLab... else clause */


         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_19].usDtaCol,
                          LEFT, (acttl[RPT_TTL_19].usDtaColWth | REL_LEFT),
                          ppsz[MAT_ID]);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_20].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_20].usDtaColWth | REL_LEFT),
                          ppsz[QUANTITY]);

         ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 2,
                          LEFT, (2 | REL_LEFT),
                          "@ ");

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_21].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_21].usDtaColWth | REL_LEFT),
                          ppsz[UNIT_PRICE]);


         /* -- "/ Unit = " */
//         ItiStrCpy(szTmp, "/ Unit ", sizeof szTmp);
         ItiStrCpy(szTmp, "/ ", sizeof szTmp);
         ItiStrCat(szTmp, ppsz[MAT_UNIT], sizeof szTmp);
         ItiStrCat(szTmp, " =", sizeof szTmp);
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_22].usTtlCol,
                          LEFT, (acttl[RPT_TTL_22].usDtaColWth | REL_LEFT),
                          szTmp);


         /* -- Quantity * Price = Extension */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_23].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_23].usDtaColWth | REL_LEFT),
                          ppsz[EXT]);
         ItiPrtIncCurrentLn(); //blank line




         /* -- Description */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_24].usDtaCol,
                          (LEFT|WRAP),
                          (acttl[RPT_TTL_24].usDtaColWth | REL_LEFT),
                          ppsz[MAT_DESC]);
//         ItiPrtIncCurrentLn(); //blank line


         ItiFreeStrArray (hhpMatSect, ppsz, usNumCols);
         }/* End of WHILE GetQueryRow */


      /* -- Finishout this section, iff we had any data. */
      if (bLabeled)
         {
         /* -- Do the materials totals: */
         ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                           szQueryMatSum, " ", pszKey);

         ppsz = ItiDbGetRow1 (szQuery, hhpMatSect, hmodrCstSh, ITIRID_RPT,
                              ID_QUERY_MAT_SUM, &usNumCols);

         if (ppsz != NULL)
            {
            usStart = (acttl[RPT_TTL_23].usDtaCol 
                       + acttl[RPT_TTL_23].usDtaColWth)
                       - ItiStrLen(ppsz[MAT_SUM_FMT]) - 1;
            usStop = ItiStrLen(ppsz[MAT_SUM_FMT]) + usStart;

            ItiPrtDrawSeparatorLn (CURRENT_LN,
                                   usStart, usStop);
            ItiPrtIncCurrentLn (); //line feed


            ItiPrtPutTextAt(CURRENT_LN,
                            (usStart - (acttl[RPT_TTL_35].usTtlColWth + 1)),
                            LEFT, (acttl[RPT_TTL_35].usTtlColWth | REL_LEFT),
                            acttl[RPT_TTL_35].szTitle);

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_23].usDtaCol,
                             RIGHT, (acttl[RPT_TTL_23].usDtaColWth | REL_LEFT),
                             ppsz[MAT_SUM_FMT]);
            ItiPrtIncCurrentLn(); //blank line

            ItiFreeStrArray (hhpMatSect, ppsz, usNumCols);
            }


         //ItiPrtIncCurrentLn (); //blank line 
         ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
         ItiPrtIncCurrentLn (); //line feed
         }

      }/* else clause */
   

   ItiMemDestroyHeap (hhpMatSect);
   hhpMatSect = NULL;
   return 0;
   }/* End of Function MatSectionFor */









USHORT BrkDwnSectionFor (PSZ pszKey, PSZ pszID, BOOL bDoPrinting, PSZ pszUnitPriceRaw)
   {
   USHORT us, usStart, usStop, usStartLn, usStopLn, usTxtBegin;
   BOOL   bNewID   = TRUE;
   BOOL   bLabeled = FALSE;
   CHAR   szUnitCost[SMARRAY] = " ";
   CHAR   szRes[SMARRAY] = " ";
   CHAR   szTtlRaw[SMARRAY] = " ";
   CHAR   szMrkRaw[SMARRAY] = " ";
   CHAR   szLbrTtlRaw[SMARRAY] = "";
   CHAR   szEqpTtlRaw[SMARRAY] = "";
   CHAR   szTmp[LGARRAY] = "";
   HQRY    hqry   = NULL;
   USHORT  usResId   = ITIRID_RPT;
   USHORT  usId      = ID_QUERY_LISTING;
   USHORT  usNumCols = 0;
   USHORT  usErr     = 0;
   PSZ   * ppsz   = NULL;


   if ((pszKey == NULL) || (*pszKey == '\0') || (pszID == NULL))
      return 13;


   /* ------------------------------------------------------------------- *\
    *  Create a memory heap.                                              *
   \* ------------------------------------------------------------------- */
   if (NULL == hhpBrkDwnSect)
      hhpBrkDwnSect = ItiMemCreateHeap (MAX_HEAP_SIZE);


   if (bDoPrinting)
      {
      if (ItiPrtKeepNLns(11) == PG_WAS_INC)
         {
         ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
         }
      SetColumnTitles(BRKDWN, pszID);
      }


   /* === Materials Line: ===================================== */

   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryMatSum, " ", pszKey);

   ppsz = ItiDbGetRow1 (szQuery, hhpBrkDwnSect, hmodrCstSh, ITIRID_RPT,
                        ID_QUERY_MAT_SUM, &usNumCols);
   if ((ppsz != NULL)
       && (ppsz[MAT_SUM][0] != '?'))
      {
      ItiStrCat(szUnitCost,    ppsz[MAT_SUM], sizeof szUnitCost);
      ItiStrCat(szUnitCost,    " ",           sizeof szUnitCost);

      ItiFreeStrArray (hhpBrkDwnSect, ppsz, usNumCols);


      /* -- Now use the szUnitCost in the totals query. */
      ItiStrCpy(szQuery, "select ", sizeof szQuery);
      ItiStrCat(szQuery, szUnitCost, sizeof szQuery);
      ItiStrCat(szQuery, ", ", sizeof szQuery);
      ItiStrCat(szQuery, szUnitCost, sizeof szQuery);
      ItiStrCat(szQuery, " * MaterialsOverheadPct, ", sizeof szQuery);
      ItiStrCat(szQuery, "MaterialsOverheadPct, ", sizeof szQuery);
      ItiStrCat(szQuery, szUnitCost, sizeof szQuery);
      ItiStrCat(szQuery, " + (", sizeof szQuery);
      ItiStrCat(szQuery, szUnitCost, sizeof szQuery);
      ItiStrCat(szQuery, " * MaterialsOverheadPct), ", sizeof szQuery);

         /* Now do the raw, ie unformatted total column. */
      ItiStrCat(szQuery, szUnitCost, sizeof szQuery);   
      ItiStrCat(szQuery, " + (", sizeof szQuery);
      ItiStrCat(szQuery, szUnitCost, sizeof szQuery);
      ItiStrCat(szQuery, " * MaterialsOverheadPct) ", sizeof szQuery);

      ItiStrCat(szQuery, szQueryMatTtl, sizeof szQuery);
      ItiStrCat(szQuery, pszKey, sizeof szQuery);


      ppsz = ItiDbGetRow1 (szQuery, hhpBrkDwnSect, hmodrCstSh, ITIRID_RPT,
                           ID_QUERY_MAT_TTL, &usNumCols);
      if (ppsz != NULL)
         {
         if (bDoPrinting)
            {
            ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_8].usTtlCol,
                            LEFT, (acttl[RPT_TTL_8].usTtlColWth | REL_LEFT),
                            acttl[RPT_TTL_8].szTitle);

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_14].usDtaCol,
                             RIGHT, (acttl[RPT_TTL_14].usDtaColWth | REL_LEFT),
                             ppsz[MAT_TOTAL]);

            ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 2,
                            LEFT, (2 | REL_LEFT),
                            "+ ");

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_15].usDtaCol,
                             RIGHT, (acttl[RPT_TTL_15].usDtaColWth | REL_LEFT),
                             ppsz[MAT_OVRHD]);

            ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                            LEFT, (1 | REL_LEFT),
                            "(");

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_16].usDtaCol,
                             RIGHT, (acttl[RPT_TTL_16].usDtaColWth | REL_LEFT),
                             ppsz[MAT_OH_PCT]);

            ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                            LEFT, (3 | REL_LEFT),
                            ") =");

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_17].usDtaCol,
                             RIGHT, (acttl[RPT_TTL_17].usDtaColWth | REL_LEFT),
                             ppsz[MAT_EXT]);
            ItiPrtIncCurrentLn(); //line feed
            }/* end of bDoPrinting */

         ItiStrCpy(szMaterialTtl, ppsz[MAT_EXT_RAW], sizeof szMaterialTtl);
         ItiFreeStrArray (hhpBrkDwnSect, ppsz, usNumCols);
         }
      else
         {
         ItiRptUtErrorMsgBox
            (NULL,"\n\t Failed CostSheet Breakdown's Material query.");
         SETQERRMSG;
         ItiStrCpy(szMaterialTtl, " 0.00 ", sizeof szMaterialTtl);
         }



      }/* end of if (ppsz... then clause */
   else
      {/* No materials were in the MaterialUsed table. */
      if (bDoPrinting)
         {
         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_8].usTtlCol,
                         LEFT, (acttl[RPT_TTL_8].usTtlColWth | REL_LEFT),
                         acttl[RPT_TTL_8].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_14].usDtaCol,
                          CENTER, (15 | FROM_RT_EDGE),
                          "(None listed in the database.)");
         ItiPrtIncCurrentLn(); //line feed
         }
      }

//   ItiPrtIncCurrentLn(); //line feed




   /* === Equipment Line: ===================================== */

   /* -- (1) Equipment unit sub total. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryCrewsEqpTtl, " ", pszKey);

   ppsz = ItiDbGetRow1 (szQuery, hhpBrkDwnSect, hmodrCstSh, ITIRID_RPT,
                           ID_QUERY_CREWS_EQP_TTL, &usNumCols);
   if (ppsz != NULL)
      {
      ItiStrCpy(szEqpTtlRaw, ppsz[EQP_TTL_RAW], sizeof szEqpTtlRaw);

      ItiFreeStrArray (hhpBrkDwnSect, ppsz, usNumCols);
      }

   /* -- (2) Determine equip unitSubTotal / (unit/day) ie CrewUnitCost. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryCstShtProd, "  ", pszKey);

   ppsz = ItiDbGetRow1 (szQuery, hhpBrkDwnSect, hmodrCstSh, ITIRID_RPT,
                           ID_QUERY_CSTSHT_PROD, &usNumCols);
   if (ppsz != NULL)
      {
      ItiRptUtMath(ID_EXP_4, sizeof szEqpTtl, szEqpTtl,
                   szEqpTtlRaw, " / ", ppsz[CSTSHT_PROD_RATE_RAW]);
      szUnitCost[0] = ' ';
      szUnitCost[1] = '\0';
      ItiStrCat(szUnitCost, szEqpTtl, sizeof szUnitCost);
      ItiStrCat(szUnitCost, " ",      sizeof szUnitCost);

      ItiFreeStrArray (hhpBrkDwnSect, ppsz, usNumCols);
      }


   /* -- (3) Now use the szUnitCost in the totals query. */
   ItiStrCpy(szQuery, "select ", sizeof szQuery);
   ItiStrCat(szQuery, szUnitCost, sizeof szQuery);
   ItiStrCat(szQuery, ", ", sizeof szQuery);
   ItiStrCat(szQuery, szUnitCost, sizeof szQuery);
   ItiStrCat(szQuery, " * EquipmentOverheadPct, ", sizeof szQuery);
   ItiStrCat(szQuery, "EquipmentOverheadPct, ", sizeof szQuery);
   ItiStrCat(szQuery, szUnitCost, sizeof szQuery);
   ItiStrCat(szQuery, " + (", sizeof szQuery);
   ItiStrCat(szQuery, szUnitCost, sizeof szQuery);
   ItiStrCat(szQuery, " * EquipmentOverheadPct), ", sizeof szQuery);

      /* Now do the raw, ie unformatted total column. */
   ItiStrCat(szQuery, szUnitCost, sizeof szQuery);   
   ItiStrCat(szQuery, " + (", sizeof szQuery);
   ItiStrCat(szQuery, szUnitCost, sizeof szQuery);
   ItiStrCat(szQuery, " * EquipmentOverheadPct) ", sizeof szQuery);

   ItiStrCat(szQuery, szQueryEqpTtl, sizeof szQuery);
   ItiStrCat(szQuery, pszKey, sizeof szQuery);


   ppsz = ItiDbGetRow1 (szQuery, hhpBrkDwnSect, hmodrCstSh, ITIRID_RPT,
                        ID_QUERY_EQP_TTL, &usNumCols);
   if (ppsz != NULL)
      {
      if (bDoPrinting)
         {
         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_10].usTtlCol,
                         LEFT, (acttl[RPT_TTL_10].usTtlColWth | REL_LEFT),
                         acttl[RPT_TTL_10].szTitle);
   
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_14].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_14].usDtaColWth | REL_LEFT),
                          ppsz[EQP_TOTAL]);
   
         ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 2,
                         LEFT, (2 | REL_LEFT),
                         "+ ");
   
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_15].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_15].usDtaColWth | REL_LEFT),
                          ppsz[EQP_OVRHD]);
   
         ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                         LEFT, (1 | REL_LEFT),
                         "(");
   
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_16].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_16].usDtaColWth | REL_LEFT),
                          ppsz[EQP_OH_PCT]);
   
         ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                         LEFT, (3 | REL_LEFT),
                         ") =");
   
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_17].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_17].usDtaColWth | REL_LEFT),
                          ppsz[EQP_EXT]);
         ItiPrtIncCurrentLn(); //line feed
         }/* end of DoPrinting */

      ItiStrCpy(szEquipmentTtl, ppsz[EQP_EXT_RAW], sizeof szEquipmentTtl);
      ItiFreeStrArray (hhpBrkDwnSect, ppsz, usNumCols);
      }
   else
      {
      ItiRptUtErrorMsgBox
         (NULL,"\n\t Failed CostSheet Breakdown's Equipment query.");
      SETQERRMSG;
      ItiStrCpy(szEquipmentTtl, " 0.00 ", sizeof szEquipmentTtl);
      }
//   ItiPrtIncCurrentLn(); //line feed





   /* === Labor Line: ========================================= */

   /* -- (1) Labor unit sub total. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryCrewsLbrTtl, " ", pszKey);

   ppsz = ItiDbGetRow1 (szQuery, hhpBrkDwnSect, hmodrCstSh, ITIRID_RPT,
                           ID_QUERY_CREWS_LBR_TTL, &usNumCols);
   if (ppsz != NULL)
      {
      ItiStrCpy(szLbrTtlRaw, ppsz[LBR_TTL_RAW], sizeof szLbrTtlRaw);

      ItiFreeStrArray (hhpBrkDwnSect, ppsz, usNumCols);
      }

   /* -- (2) Determine equip unitSubTotal / (unit/day) ie CrewUnitCost. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryCstShtProd, "  ", pszKey);

   ppsz = ItiDbGetRow1 (szQuery, hhpBrkDwnSect, hmodrCstSh, ITIRID_RPT,
                           ID_QUERY_CSTSHT_PROD, &usNumCols);
   if (ppsz != NULL)
      {
      ItiRptUtMath(ID_EXP_4, sizeof szLbrTtl, szLbrTtl,
                   szLbrTtlRaw, " / ", ppsz[CSTSHT_PROD_RATE_RAW]);
      szUnitCost[0] = ' ';
      szUnitCost[1] = '\0';
      ItiStrCat(szUnitCost, szLbrTtl, sizeof szUnitCost);
      ItiStrCat(szUnitCost, " ",      sizeof szUnitCost);

      ItiFreeStrArray (hhpBrkDwnSect, ppsz, usNumCols);
      }


   /* -- (3) Now use the szUnitCost in the totals query. */
   ItiStrCpy(szQuery, "select ", sizeof szQuery);
   ItiStrCat(szQuery, szUnitCost, sizeof szQuery);
   ItiStrCat(szQuery, ", ", sizeof szQuery);
   ItiStrCat(szQuery, szUnitCost, sizeof szQuery);
   ItiStrCat(szQuery, " * LaborOverheadPct, ", sizeof szQuery);
   ItiStrCat(szQuery, "LaborOverheadPct, ", sizeof szQuery);
   ItiStrCat(szQuery, szUnitCost, sizeof szQuery);
   ItiStrCat(szQuery, " + (", sizeof szQuery);
   ItiStrCat(szQuery, szUnitCost, sizeof szQuery);
   ItiStrCat(szQuery, " * LaborOverheadPct), ", sizeof szQuery);

      /* Now do the raw, ie unformatted total column. */
   ItiStrCat(szQuery, szUnitCost, sizeof szQuery);   
   ItiStrCat(szQuery, " + (", sizeof szQuery);
   ItiStrCat(szQuery, szUnitCost, sizeof szQuery);
   ItiStrCat(szQuery, " * LaborOverheadPct) ", sizeof szQuery);

   ItiStrCat(szQuery, szQueryLbrTtl, sizeof szQuery);
   ItiStrCat(szQuery, pszKey, sizeof szQuery);


   ppsz = ItiDbGetRow1 (szQuery, hhpBrkDwnSect, hmodrCstSh, ITIRID_RPT,
                        ID_QUERY_LBR_TTL, &usNumCols);
   if (ppsz != NULL)
      {
      if (bDoPrinting)
         {
         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_9].usTtlCol,
                         LEFT, (acttl[RPT_TTL_9].usTtlColWth | REL_LEFT),
                         acttl[RPT_TTL_9].szTitle);
   
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_14].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_14].usDtaColWth | REL_LEFT),
                          ppsz[LBR_TOTAL]);
   
         ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 2,
                         LEFT, (2 | REL_LEFT),
                         "+ ");
   
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_15].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_15].usDtaColWth | REL_LEFT),
                          ppsz[LBR_OVRHD]);
   
         ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                         LEFT, (1 | REL_LEFT),
                         "(");
   
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_16].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_16].usDtaColWth | REL_LEFT),
                          ppsz[LBR_OH_PCT]);
   
         ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                         LEFT, (3 | REL_LEFT),
                         ") =");
   
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_17].usDtaCol,
                          RIGHT, (acttl[RPT_TTL_17].usDtaColWth | REL_LEFT),
                          ppsz[LBR_EXT]);
         ItiPrtIncCurrentLn(); //line feed
         }/* end of DoPrinting */

      ItiStrCpy(szLaborTtl, ppsz[LBR_EXT_RAW], sizeof szLaborTtl);
      ItiFreeStrArray (hhpBrkDwnSect, ppsz, usNumCols);
      }
   else
      {
      ItiRptUtErrorMsgBox
         (NULL,"\n\t Failed CostSheet Breakdown's Labor query.");
      SETQERRMSG;
      ItiStrCpy(szLaborTtl, " 0.00 ", sizeof szLaborTtl);
      }
//   ItiPrtIncCurrentLn(); //line feed  





   /* === Total, MarkUp, and UnitPrice: ======================= */

   /* -- Sum the totals: */
   ItiStrCpy(szTmp, szMaterialTtl, sizeof szTmp);
   ItiStrCat(szTmp, " + ", sizeof szTmp);
   ItiStrCat(szTmp, szEquipmentTtl, sizeof szTmp);
   ItiRptUtMath(ID_EXP_4DC, sizeof szRes, szRes,
                szTmp, " + ", szLaborTtl);

   ItiRptUtMath(ID_EXP_4, sizeof szTtlRaw, szTtlRaw,
                szTmp, " + ", szLaborTtl);


   if (bDoPrinting)
      {
      us = ItiStrLen(szRes);
      usStart = (acttl[RPT_TTL_36].usDtaCol 
                 + acttl[RPT_TTL_36].usDtaColWth)
                 - us - 1;
      usStop = us + usStart;

      usStartLn = usStart;
      usStopLn = usStop;

      ItiPrtDrawSeparatorLn (CURRENT_LN, usStartLn, usStopLn);
      ItiPrtIncCurrentLn (); //line feed

      usStart = (usStart - acttl[RPT_TTL_36].usTtlColWth);

      ItiPrtPutTextAt(CURRENT_LN,
                      usStart,
                      LEFT, (acttl[RPT_TTL_36].usTtlColWth | REL_LEFT),
                      acttl[RPT_TTL_36].szTitle);
   
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_36].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_36].usDtaColWth | REL_LEFT),
                       szRes);
      ItiPrtIncCurrentLn(); //blank line
      szRes[0] = '\0';
      }

   /* -- Mark-up: */
   ItiStrCpy(szTmp, " (", sizeof szTmp);
   ItiStrCat(szTmp, szMaterialTtl, sizeof szTmp);
   ItiStrCat(szTmp, " + ", sizeof szTmp);
   ItiStrCat(szTmp, szEquipmentTtl, sizeof szTmp);
   ItiStrCat(szTmp, " + ", sizeof szTmp);
   ItiStrCat(szTmp, szLaborTtl, sizeof szTmp);
   ItiStrCat(szTmp, ") ", sizeof szTmp);
   ItiRptUtMath(ID_EXP_4DC, sizeof szRes, szRes,
                szTmp, " * ", szOverHeadPctRaw);

   ItiRptUtMath(ID_EXP_4, sizeof szMrkRaw, szMrkRaw,
                szTmp, " * ", szOverHeadPctRaw);

   if (bDoPrinting)
      {
      ItiStrCpy(szTmp, "+ ", sizeof szTmp);
      ItiStrCat(szTmp, szOverHeadPct, sizeof szTmp);
      ItiStrCat(szTmp, acttl[RPT_TTL_37].szTitle, sizeof szTmp);
   
      us = ItiStrLen(szTmp);
      usTxtBegin = usStartLn - us;
   
      ItiPrtPutTextAt(CURRENT_LN, usTxtBegin, LEFT,
                      (usTxtBegin + us),
                      szTmp);
   
      ItiPrtPutTextAt (CURRENT_LN,
                       acttl[RPT_TTL_37].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_37].usDtaColWth | REL_LEFT),
                       szRes);
      ItiPrtIncCurrentLn(); //blank line
   
      ItiPrtDrawSeparatorLn (CURRENT_LN, usStartLn, usStopLn);
      ItiPrtIncCurrentLn (); //line feed
      }


   /* -- Unit Price = Total + Markup (RAW) */
   ItiRptUtMath(ID_EXP_4, sizeof szRes, szRes,
                szTtlRaw, " + ", szMrkRaw);

   /* == Set raw UnitPrice return value. */
   us = ItiStrLen(pszUnitPriceRaw);
   ItiStrCpy(pszUnitPriceRaw, szRes, us);

   /* -- Unit Price = Total + Markup */
   ItiRptUtMath(ID_EXP_4DC, sizeof szRes, szRes,
                szTtlRaw, " + ", szMrkRaw);

   if (bDoPrinting)
      {
      usTxtBegin = usStartLn - acttl[RPT_TTL_38].usTtlColWth;
   
      ItiPrtPutTextAt(CURRENT_LN, usTxtBegin, LEFT,
                      (usTxtBegin + acttl[RPT_TTL_38].usTtlColWth),
                      acttl[RPT_TTL_38].szTitle);
   
      ItiPrtPutTextAt (CURRENT_LN,
                       acttl[RPT_TTL_38].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_38].usDtaColWth | REL_LEFT),
                       szRes);
   
      ItiPrtIncCurrentLn (); //blank line 
      ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
      ItiPrtIncCurrentLn (); //line feed
      }
   
   ItiMemDestroyHeap (hhpBrkDwnSect);
   hhpBrkDwnSect = NULL;
   return 0;
   }/* End of Function BrkDwnSectionFor */





USHORT CrewSectionFor (PSZ pszKey, PSZ pszID)
   {
   USHORT usLn;
   BOOL   bNewID   = TRUE;
   BOOL   bLabeled;
   CHAR   szLbrTtlRaw[SMARRAY] = "";
   CHAR   szEqpTtlRaw[SMARRAY] = "";

           /* Used by DbGetQueryRow function: */
   HQRY    hqryLoc   = NULL;
   USHORT  usResIdLoc   = ITIRID_RPT;
   USHORT  usNumColsLoc = 0;
   USHORT  usErrLoc  = 0;
   PSZ   * ppszLoc   = NULL;

   HQRY    hqry   = NULL;
   USHORT  usResId   = ITIRID_RPT;
   USHORT  usId      = ID_QUERY_LISTING;
   USHORT  usNumCols = 0;
   USHORT  usErr     = 0;
   PSZ   * ppsz   = NULL;


   if ((pszKey == NULL) || (*pszKey == '\0') || (pszID == NULL))
      return 13;

   /* ------------------------------------------------------------------- *\
    *  Create a memory heap.                                              *
   \* ------------------------------------------------------------------- */
   if (NULL == hhpCrewSect)
      hhpCrewSect = ItiMemCreateHeap (MAX_HEAP_SIZE);


   /* -- Build the query. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryCrews, pszKey, " order by 1 ");

   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hhpCrewSect, hmodrCstSh, ITIRID_RPT,
                          ID_QUERY_CREWS, &usNumCols, &usErr);
   
   /* -- Process the query results for each row. */
   if (hqry == NULL)
      {
      SETQERRMSG;
      }
   else
      {
      bLabeled = FALSE;
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
         {
//         usLn = ItiStrLen(ppsz[CREWS_DESC]) + 1;
//         usLn = 12 + (usLn / acttl[RPT_TTL_27].usDtaColWth);

         usLn = 12 + ItiPrtWrappedLnCnt (ppsz[CREWS_DESC],    
                                         acttl[RPT_TTL_27].usDtaColWth,
                                         FALSE);  // 5 Aug 
         if (!bLabeled)
            {
            if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
               {
               ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
               }
            SetColumnTitles(CREW, pszID);
            bLabeled = TRUE;
            }
         else
            {
            if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
               {
               ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
               SetColumnTitles(CREW, pszID);
               }
            }/* end if(!bLab... else clause */


         /* -- Now, with each CrewKey... */ 
         /* -- ID and description text. */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_26].usDtaCol,
                       LEFT, (acttl[RPT_TTL_26].usDtaColWth | REL_LEFT),
                       ppsz[CREWS_ID]);

         /* -- Crew description */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_27].usDtaCol,
                       (LEFT|WRAP),
                       (acttl[RPT_TTL_27].usDtaColWth | REL_LEFT),
                       ppsz[CREWS_DESC]);



         /* -- The crew's equipment: */
         ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                           szQueryCrewsEqp, pszKey, szQueryCrewsEqpConj);
         ItiStrCat(szQuery, ppsz[CREWS_KEY], sizeof szQuery);

         hqryLoc = ItiDbExecQuery (szQuery, hhpCrewSect, hmodrCstSh, ITIRID_RPT,
                                ID_QUERY_CREWS_EQP, &usNumColsLoc, &usErrLoc);
         
         /* -- Process the query results for each row. */
         if (hqryLoc == NULL)
            {
            SETQERRMSG;
            }
         else
            {
            while( ItiDbGetQueryRow (hqryLoc, &ppszLoc, &usErrLoc) )  
               {
               usLn = ItiStrLen(ppszLoc[EQP_DESC]) + 2;
               usLn = 6 + (usLn / acttl[RPT_TTL_32].usDtaColWth);
               if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
                  {
                  ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
                  SetColumnTitles(CREW, pszID);
                  }


               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_28].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_28].usDtaColWth | REL_LEFT),
                            ppszLoc[EQP_QUAN]);

               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_29].usDtaCol,
                            LEFT, (acttl[RPT_TTL_29].usDtaColWth | REL_LEFT),
                            ppszLoc[EQP_ID]);

               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_30].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_30].usDtaColWth | REL_LEFT),
                            ppszLoc[EQP_RATE]);

               ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL,  // + 1,
                               LEFT, (6 | REL_LEFT),
                               "/hr *");

               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_31].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_31].usDtaColWth | REL_LEFT),
                            ppszLoc[EQP_HRS]);

               ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                               LEFT, (9 | REL_LEFT),
                               " hr/day =");

               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_33].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_33].usDtaColWth | REL_LEFT),
                            ppszLoc[EQP_DAILY_CST]);
               ItiPrtIncCurrentLn (); //line feed

               /* -- Equipment description */
               ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_32].usDtaCol,
                             (LEFT|WRAP),
                             (acttl[RPT_TTL_32].usDtaColWth | REL_LEFT),
                             ppszLoc[EQP_DESC]);

               ItiFreeStrArray (hhpCrewSect, ppszLoc, usNumColsLoc);
               }
            }/* end of if (hqryLoc... else clause */



         /* -- Now the crew's labor: */
         ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                           szQueryCrewsLbr, pszKey, szQueryCrewsEqpConj);
         ItiStrCat(szQuery, ppsz[CREWS_KEY], sizeof szQuery);

         hqryLoc = ItiDbExecQuery (szQuery, hhpCrewSect, hmodrCstSh, ITIRID_RPT,
                                ID_QUERY_CREWS_LBR, &usNumColsLoc, &usErrLoc);
         
         /* -- Process the query results for each row. */
         if (hqryLoc == NULL)
            {
            SETQERRMSG;
            }
         else
            {
            while( ItiDbGetQueryRow (hqryLoc, &ppszLoc, &usErrLoc) )  
               {
               usLn = ItiStrLen(ppszLoc[LBR_DESC]) + 2;
               usLn = 6 + (usLn / acttl[RPT_TTL_32].usDtaColWth);
               if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
                  {
                  ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
                  SetColumnTitles(CREW, pszID);
                  }


               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_28].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_28].usDtaColWth | REL_LEFT),
                            ppszLoc[LBR_QUAN]);

               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_29].usDtaCol,
                            LEFT, (acttl[RPT_TTL_29].usDtaColWth | REL_LEFT),
                            ppszLoc[LBR_ID]);

               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_30].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_30].usDtaColWth | REL_LEFT),
                            ppszLoc[LBR_RATE]);

               ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL,  // + 1,
                               LEFT, (6 | REL_LEFT),
                               "/hr *");

               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_31].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_31].usDtaColWth | REL_LEFT),
                            ppszLoc[LBR_HRS]);

               ItiPrtPutTextAt(CURRENT_LN, CURRENT_COL + 1,
                               LEFT, (9 | REL_LEFT),
                               " hr/day =");

               ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_34].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_34].usDtaColWth | REL_LEFT),
                            ppszLoc[LBR_DAILY_CST]);
               ItiPrtIncCurrentLn (); //line feed

               /* -- Equipment description */
               ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_32].usDtaCol,
                             (LEFT|WRAP),
                             (acttl[RPT_TTL_32].usDtaColWth | REL_LEFT),
                             ppszLoc[LBR_DESC]);

               ItiFreeStrArray (hhpCrewSect, ppszLoc, usNumColsLoc);
               }
            }/* end of if (hqryLoc... else clause */







         /* -- Almost finally, the totals of equipment and labor: */
         if (ItiPrtKeepNLns(6) == PG_WAS_INC)
            {
            ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
            SetColumnTitles(CREW, pszID);
            }

         ItiPrtDrawSeparatorLn(CURRENT_LN,
                               acttl[RPT_TTL_33].usDtaCol+1,
                               acttl[RPT_TTL_33].usDtaCol+acttl[RPT_TTL_33].usDtaColWth+1);
         ItiPrtDrawSeparatorLn(CURRENT_LN,
                               acttl[RPT_TTL_34].usDtaCol+1,
                               acttl[RPT_TTL_34].usDtaCol+acttl[RPT_TTL_34].usDtaColWth+1);
         ItiPrtIncCurrentLn (); //line feed
         ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_33].usDtaCol - 6,
                         LEFT, (8 | REL_LEFT), "Total: ");

         /* -- Equipment sub total. */
         ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                           szQueryCrewsEqpTtl, pszKey, szQueryCrewsEqpConj);

         ItiStrCat(szQuery, ppsz[CREWS_KEY], sizeof szQuery);

         ppszLoc = ItiDbGetRow1 (szQuery, hhpCrewSect, hmodrCstSh, ITIRID_RPT,
                                 ID_QUERY_CREWS_EQP_TTL, &usNumColsLoc);
         if (ppszLoc != NULL)
            {
            ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_33].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_33].usDtaColWth | REL_LEFT),
                            ppszLoc[EQP_TTL]);
            ItiStrCpy(szEqpTtlRaw, ppszLoc[EQP_TTL_RAW], sizeof szEqpTtlRaw);

            ItiFreeStrArray (hhpCrewSect, ppszLoc, usNumColsLoc);
            }

         /* -- Labor sub total. */
         ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                           szQueryCrewsLbrTtl, pszKey, szQueryCrewsEqpConj);
                                                       /*    ^^ CORRECT ^^ */
         ItiStrCat(szQuery, ppsz[CREWS_KEY], sizeof szQuery);

         ppszLoc = ItiDbGetRow1 (szQuery, hhpCrewSect, hmodrCstSh, ITIRID_RPT,
                                 ID_QUERY_CREWS_LBR_TTL, &usNumColsLoc);
         if (ppszLoc != NULL)
            {
            ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_34].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_34].usDtaColWth | REL_LEFT),
                            ppszLoc[LBR_TTL]);
            ItiStrCpy(szLbrTtlRaw, ppszLoc[LBR_TTL_RAW], sizeof szLbrTtlRaw);

            ItiFreeStrArray (hhpCrewSect, ppszLoc, usNumColsLoc);
            }

         ItiPrtIncCurrentLn (); //line feed


         /* unit/day =  sub total. */
         ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                           szQueryCstShtProd, "  ", pszKey);

         ppszLoc = ItiDbGetRow1 (szQuery, hhpCrewSect, hmodrCstSh, ITIRID_RPT,
                                 ID_QUERY_CSTSHT_PROD, &usNumColsLoc);
         if (ppszLoc != NULL)
            {
            ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_39].usDtaCol - 2,
                            LEFT, (2 | REL_LEFT),
                            "/ ");

            ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_39].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_39].usDtaColWth | REL_LEFT),
                            ppszLoc[CSTSHT_PROD_RATE]);

            ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_39].usTtlCol,
                            LEFT, (acttl[RPT_TTL_39].usTtlColWth | REL_LEFT),
                            acttl[RPT_TTL_39].szTitle);


            /* -- EqpTtl unit/day */
            ItiRptUtMath(ID_EXP_4DC, sizeof szEqpTtl, szEqpTtl,
                         szEqpTtlRaw, " / ", ppszLoc[CSTSHT_PROD_RATE_RAW]);

            ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_40].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_40].usDtaColWth | REL_LEFT),
                            szEqpTtl);


            /* -- LbrTtl unit/day */
            ItiRptUtMath(ID_EXP_4DC, sizeof szLbrTtl, szLbrTtl,
                         szLbrTtlRaw, " / ", ppszLoc[CSTSHT_PROD_RATE_RAW]);

            ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_41].usDtaCol,
                            RIGHT, USE_RT_EDGE,
//                            RIGHT, (acttl[RPT_TTL_41].usDtaColWth | REL_LEFT),
                            szLbrTtl);

            ItiPrtIncCurrentLn (); //line feed


            ItiPrtDrawSeparatorLn(CURRENT_LN,
                      acttl[RPT_TTL_33].usDtaCol+1,
                      acttl[RPT_TTL_33].usDtaCol+acttl[RPT_TTL_33].usDtaColWth+1);
            ItiPrtDrawSeparatorLn(CURRENT_LN,
                      acttl[RPT_TTL_34].usDtaCol+1,
                      acttl[RPT_TTL_34].usDtaCol+acttl[RPT_TTL_34].usDtaColWth+1);
            ItiPrtIncCurrentLn (); //line feed

            ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_40].usTtlCol,
                            LEFT, (acttl[RPT_TTL_40].usTtlColWth | REL_LEFT),
                            acttl[RPT_TTL_40].szTitle);

            /* -- Repeats according to the CES design document, Chap 9 pg 66 */
            ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_40].usDtaCol,
                            RIGHT, (acttl[RPT_TTL_40].usDtaColWth | REL_LEFT),
                            szEqpTtl);

            ItiPrtPutTextAt(CURRENT_LN, acttl[RPT_TTL_41].usDtaCol,
                            RIGHT, USE_RT_EDGE,
//                            (acttl[RPT_TTL_41].usDtaColWth | REL_LEFT),
                            szLbrTtl);

            ItiPrtIncCurrentLn (); //line feed
            ItiPrtIncCurrentLn (); //blank line 

            ItiFreeStrArray (hhpCrewSect, ppszLoc, usNumColsLoc);
            }


         ItiFreeStrArray (hhpCrewSect, ppsz, usNumCols);
         }/* End of WHILE GetQueryRow */

      }/* end if (hqry... else clause */

   
   ItiMemDestroyHeap (hhpCrewSect);
   hhpCrewSect = NULL;
   return 0;

   }/* End of Function CrewSectionFor */













USHORT PrintProc (HWND hwnd, MPARAM mp1, MPARAM mp2)
   {
   USHORT us    = 0;
   CHAR   szKey      [SMARRAY] = "";
   CHAR   szOptSwStr [SMARRAY] = "";
   CHAR   szCmdLn [2*SMARRAY +1] = "";
   BOOL   bChecked = FALSE;
   BOOL   bCheckedSumm = FALSE;
   HFILE  hAltFile;
   HHEAP  hhprLocal = NULL;


   /* -- reset the command line. */
   ItiStrCpy (szCmdLn, RPT_DLL_NAME, sizeof(szCmdLn) );


   
   /* -- Check if we are to use only the summary section, ie. short listing. */
   bCheckedSumm = (USHORT) (ULONG) WinSendDlgItemMsg (hwnd, 
                               DID_SUMMONLY, BM_QUERYCHECK, 0, 0);
   if (bCheckedSumm)
      {
      ItiStrCat (szCmdLn, OPT_SW_SUMMONLY_STR, sizeof szCmdLn);
      }


   /* -- Check if each cost sheet is to be on its own page. */
   bChecked = (USHORT) (ULONG) WinSendDlgItemMsg (hwnd, 
                               DID_NEWPG, BM_QUERYCHECK, 0, 0);
   if (bChecked && !bCheckedSumm)
      {             /* Don't use new page if only summaries are used. */
      ItiStrCat (szCmdLn, ITIRPT_OPTSW_NEWPG_STR, sizeof szCmdLn);
      }


   bChecked = (USHORT) (ULONG) WinSendDlgItemMsg (hwnd, 
                               DID_SELECTED, BM_QUERYCHECK, 0, 0);
   if (bChecked)
      {
      /* -- Build cmdln string to do selected cost sheet keys. */

      us = ItiRptUtBuildSelectedKeysFile(hwnd,
                             CostSheetCatL,
                             cCostSheetKey,
                             szKeyFileName, sizeof szKeyFileName);

      if ((us == ITIRPTUT_PARM_ERROR)|| (us == ITIRPTUT_ERROR))
         ItiRptUtErrorMsgBox (hwnd,
                "ERROR: Failed to get selected keys in "__FILE__);
      else
         {/* -- Prep the CmdLn to use the key file. */
         ItiStrCat (szCmdLn, ITIRPT_OPTSW_KEYFILE, sizeof szCmdLn);
         ItiStrCat (szCmdLn, szKeyFileName, sizeof szCmdLn);
         }/* end of if ((us... */

      }/* end of if (bChecked... */
   else
      {/* Do ALL of the cost sheet catalogs. */
      hhprLocal = ItiMemCreateHeap (MAX_HEAP_SIZE);

      ItiRptUtUniqueFile (szKeyFileName, sizeof szKeyFileName, TRUE);

      /* -- Build the cost sheet key list query. */
      ItiMbQueryQueryText(hmodrCstSh, ITIRID_RPT, ID_QUERY_CSTSHT_KEYS,
                          szQueryCstShtKeys, sizeof szQueryCstShtKeys );


      /* -- Do the database query. */
      hqry = ItiDbExecQuery (szQueryCstShtKeys, hhprLocal, hmodrCstSh, ITIRID_RPT,
                             ID_QUERY_CSTSHT_KEYS, &usNumCols, &usErr);
      
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
            ItiRptUtWriteToKeyFile (hAltFile, ppsz[CST_SHT_KEY], NULL);
            ItiFreeStrArray (hhprLocal, ppsz, usNumCols);
            }/* End of while... */

         ItiRptUtEndKeyFile (hAltFile);

         ItiStrCat (szCmdLn, ITIRPT_OPTSW_KEYFILE, sizeof szCmdLn);
         ItiStrCat (szCmdLn, szKeyFileName, sizeof szCmdLn);
         }/* else clause */

      }/* end of else clause */


   us = ItiRptUtExecRptCmdLn (hwnd, szCmdLn, RPT_CSTSHT_SESSION);

   return (us);

   }/* End of PrintProc Function */




BOOL InitQueryStrings (void)
   {
   if (TRUE == bQryStrInit)
      return bQryStrInit;

   if (0 != ItiMbQueryQueryText(hmodrCstSh, ITIRID_RPT, ID_QUERY_CSTSHT_KEYS,
                                szQueryCstShtKeys, sizeof szQueryCstShtKeys) )
      return FALSE;

   if (0 != ItiMbQueryQueryText(hmodrCstSh, ITIRID_RPT, ID_QUERY_LISTING,
                                szQueryListing, sizeof szQueryListing) )
      return FALSE;


   if (0 != ItiMbQueryQueryText(hmodrCstSh, ITIRID_RPT, ID_QUERY_MAT,
                                szQueryMat, sizeof szQueryMat) )
      return FALSE;

   if (0 != ItiMbQueryQueryText(hmodrCstSh, ITIRID_RPT, ID_QUERY_MAT_SUM,
                                szQueryMatSum, sizeof szQueryMatSum) )
      return FALSE;

   if (0 != ItiMbQueryQueryText(hmodrCstSh, ITIRID_RPT, ID_QUERY_MAT_TTL,
                                szQueryMatTtl, sizeof szQueryMatTtl) )
      return FALSE;


   if (0 != ItiMbQueryQueryText(hmodrCstSh, ITIRID_RPT, ID_QUERY_CREWS,
                                szQueryCrews, sizeof szQueryCrews) )
      return FALSE;

   if (0 != ItiMbQueryQueryText(hmodrCstSh, ITIRID_RPT, ID_QUERY_CREWS_EQP,
                                szQueryCrewsEqp, sizeof szQueryCrewsEqp) )
      return FALSE;

   if (0 != ItiMbQueryQueryText(hmodrCstSh, ITIRID_RPT, ID_QUERY_CREWS_EQP_CONJ,
                                szQueryCrewsEqpConj, sizeof szQueryCrewsEqpConj))
      return FALSE;

   if (0 != ItiMbQueryQueryText(hmodrCstSh, ITIRID_RPT, ID_QUERY_CREWS_EQP_TTL,
                                szQueryCrewsEqpTtl, sizeof szQueryCrewsEqpTtl))
      return FALSE;


   if (0 != ItiMbQueryQueryText(hmodrCstSh, ITIRID_RPT, ID_QUERY_CREWS_LBR,
                                szQueryCrewsLbr, sizeof szQueryCrewsLbr) )
      return FALSE;

   if (0 != ItiMbQueryQueryText(hmodrCstSh, ITIRID_RPT, ID_QUERY_CREWS_LBR_TTL,
                                szQueryCrewsLbrTtl, sizeof szQueryCrewsLbrTtl) )
      return FALSE;


   if (0 != ItiMbQueryQueryText(hmodrCstSh, ITIRID_RPT, ID_QUERY_CSTSHT_PROD,
                                szQueryCstShtProd, sizeof szQueryCstShtProd) )
      return FALSE;


   if (0 != ItiMbQueryQueryText(hmodrCstSh, ITIRID_RPT, ID_QUERY_EQP_TTL,
                                szQueryEqpTtl, sizeof szQueryEqpTtl) )
      return FALSE;

   if (0 != ItiMbQueryQueryText(hmodrCstSh, ITIRID_RPT, ID_QUERY_LBR_TTL,
                                szQueryLbrTtl, sizeof szQueryLbrTtl) )
      return FALSE;

   bQryStrInit = TRUE;
   return bQryStrInit;

   }/* End of InitQueryStrings */





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
   ItiStrCpy (pszBuffer, "Cost Sheet Catalog Listing", usMaxSize);   
   *pusWindowID = rCstSh_RptDlgBox;                       
   }/* END OF FUNCTION */




BOOL EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID, 
                                     USHORT usActiveWindowID) 
   {                                                          
   if ((usTargetWindowID == rCstSh_RptDlgBox)                     
       && (usActiveWindowID == CostSheetCatS))                  
      return TRUE;                                            
   else                                                       
      return FALSE;                                           
   }/* END OF FUNCTION */





MRESULT EXPORT ItiDllrCstShRptDlgProc (HWND     hwnd,     
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
         us = ItiRptUtInitDlgBox (hwnd, RPT_CSTSHT_SESSION, 0, NULL);
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

   }/* END OF FUNCTION ItiDllrCstShRptDlgProc */





USHORT EXPORT CostSheetBrkDwnSections (PSZ pszCstShtKey, BOOL bSetID)
   {
   USHORT us = 0;
   CHAR   szCurID[SMARRAY + 1] = "";
   CHAR   szUnitPrice[SMARRAY + 1] = "                          ";
   PSZ    pszCurID = szCurID;

   /* --- */
   if (hmodrCstSh != hmodrCS)
      hmodrCstSh = hmodrCS;

   /* -- Get the report's titles for the rCstSh DLL. */
   if (0 != usLabelsLoaded)
      usLabelsLoaded = ItiRptUtLoadLabels (hmodrCstSh, &usNumOfTitles, &acttl[0]);

   if (FALSE == InitQueryStrings())
      ItiRptUtErrorMsgBox(NULL," *** CostSheetBrkDwnSections query string init failed.");

   if (TRUE == bSetID)
      us = SetCurrentID (pszCstShtKey, &pszCurID);

   if (us == 0)
      {
      BrkDwnSectionFor (pszCstShtKey, szCurID, TRUE, szUnitPrice);
      MatSectionFor    (pszCstShtKey, szCurID);
      CrewSectionFor   (pszCstShtKey, szCurID);
      return us;
      }

   return 13;

   }/* END OF FUNCTION CostSheetBrkDwnSections */



USHORT EXPORT CostSheetUnitPrice (PSZ pszCstSheetKey,
                                  PSZ pszCSUnitPrice, USHORT usLen)
   {
   static CHAR szRawUnitPrice[SMARRAY+1] = "                             ";

   if (NULL == pszCstSheetKey)
      return 13;

   /* --- */
   if (hmodrCstSh != hmodrCS)
      hmodrCstSh = hmodrCS;

   /* -- Get the report's titles for the rCstSh DLL. */
   if (0 != usLabelsLoaded)
      usLabelsLoaded = ItiRptUtLoadLabels (hmodrCstSh, &usNumOfTitles, &acttl[0]);

   if (FALSE == InitQueryStrings())
      ItiRptUtErrorMsgBox(NULL," *** CostSheetUnitPrice query string init failed.");

   /* -- Need to get and set the overhead pcts,     without printing. */
   SummarySectionFor(pszCstSheetKey, &pszCurrentID, FALSE);

   BrkDwnSectionFor (pszCstSheetKey, " ", FALSE, szRawUnitPrice);
                  /* No ID needed (" ") and do NOT (false) print anything */

   if (NULL != pszCSUnitPrice)
      ItiStrCpy (pszCSUnitPrice, szRawUnitPrice, usLen);   
   else
      return 13;

   return 0;
   }



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


   hmodrCstSh = hmodrCS;

   /* ------------------------------------------------------------------- *\
    *  Create a memory heap.                                              *
   \* ------------------------------------------------------------------- */
   if (NULL == hhprCstSh)
      hhprCstSh = ItiMemCreateHeap (MAX_HEAP_SIZE);


   /* ------------------------------------------------------------------- *\
    * Initialize the strings.                                             *
   \* ------------------------------------------------------------------- */
   ItiStrCpy(szCurrentID,"TABLE_ID",sizeof(szCurrentID));

   /* -- Get any query ordering information, BEFORE setting rpt titles. */
   ItiRptUtGetOption (ITIRPTUT_ORDERING_SWITCH, szOrderingStr, LGARRAY,
                      argcnt, argvars);
   ConvertOrderingStr (szOrderingStr);


   /* -- Get user option to print only the cost sheet summary section. */
   ItiRptUtGetOption (OPT_SW_SUMMONLY_CHR, szOptSw, sizeof szOptSw,
                      argcnt, argvars);
   if (szOptSw[0] == '\0') 
      usRetOptSw = SECTIONS_ALL;
   else
      usRetOptSw = SECTIONS_SUMMARY_ONLY;



   /* -- Get user option to use a new page for each cost sheet. */
   ItiRptUtGetOption (ITIRPT_OPTSW_NEWPG_CHR, szOptSw, sizeof szOptSw,
                      argcnt, argvars);
   if (szOptSw[0] == '\0') 
      bUseNewPgPerCstSht = FALSE;
   else
      bUseNewPgPerCstSht = TRUE;



   /* -- Get the report's titles for this DLL. */
   usLabelsLoaded = ItiRptUtLoadLabels (hmodrCstSh, &usNumOfTitles, &acttl[0]);

   /* -- Set the report's titles for this DLL. */
   SetTitles(preptCS, argcnt, argvars);

   /* ------------------------------------------------------------------- *\
    * Do any run-time formatting then start the report.                   *
   \* ------------------------------------------------------------------- */
   us =  ItiPrtLoadDLLInfo(hab, hmq, preptCS, argcnt, argvars);
   if (us != 0 )
      {
      DBGMSG("FAILED to ItiPrtDLLLoadInfo.");
      return (ITIPRT_NOT_INITIALIZED);
      }

   if (FALSE == InitQueryStrings())
      {
      ItiRptUtErrorMsgBox (NULL, " *** rCstSht query string init failed.");
      return 13;
      }



   ItiPrtBeginReport (PRT_Q_NAME);


   /* -------------------------------------------------------------------- *\
    * -- For each CostSheetKey...                                                   *
   \* -------------------------------------------------------------------- */
   ItiRptUtInitKeyList(argcnt, argvars); /*  */
   while (ItiRptUtGetNextKey(pszKey,sizeof(szKey)) != ITIRPTUT_NO_MORE_KEYS)
      {                                                 
      us = SummarySectionFor(pszKey, &pszCurrentID, TRUE);

      if (us == 0)
         CostSheetBrkDwnSections (pszKey, FALSE);

      }/* End of WHILE GetNextKey */



   /* ------------------------------------------------------------------- *\
    * Finish up and quit.                                                 *
   \* ------------------------------------------------------------------- */
   ItiPrtEndReport();

   DBGMSG("Exit ***DLL function ItiRptDLLPrintReport");
   return 0;

   }/* END OF FUNCTION ItiRptDLLPrintReport */

