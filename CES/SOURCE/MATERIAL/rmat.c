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
 *  Timothy Blake (tlb)
 *  July 1991
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
#include "..\include\itiglob.h"
#include    "..\include\itimbase.h"
#include    "..\include\itidbase.h"
#include    "..\include\itifmt.h"
#include    "..\include\colid.h"
#include    "..\include\winid.h"
#include    "..\include\itiwin.h"
#include    "..\include\dialog.h"
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


#include    "rinidll.h"


#include "rMat.h"                                       



/* ======================================================================= *\
 *                                         MANIFEST CONSTANT DEFINITIONS   *
\* ======================================================================= */
#define  BLANK_LINE_INTERVAL    4
#define  PRT_Q_NAME             "Rpt: Materials" 

#define  QUERY_ARRAY_SIZE       640

#define  SIZE_OF_QUERY_RPT_DESC QUERY_ARRAY_SIZE


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
static CHAR szDllVersion[] = "1.1a0 rmat.dll";

static  USHORT  usCurrentPg = 0;

static  CHAR  szCurrentID   [SMARRAY + 1] = "";
static  CHAR  szOrderingStr [LGARRAY + 1] = "";
static  BOOL  bInclZonePrices = FALSE;
static  BOOL  bInclFacilities = FALSE;
static  BOOL  bUseEnglishPrices = TRUE;

static  CHAR  szQueryRptDesc [SIZE_OF_QUERY + 1] = "";

static  CHAR  szBaseQuery   [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szQuery       [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  sz2ndQuery    [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szQueryKeys   [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szQueryPrices [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szQueryFacil  [QUERY_ARRAY_SIZE + 1] = "";

static  CHAR  szWhereConj   [SIZE_OF_CONJ + 1] = "";
static  CHAR  szKey         [SMARRAY + 1];
static  CHAR  szPricesConj  [ QUERY_ARRAY_SIZE + 1] = "";


static  CHAR  szFailedDLL [CCHMAXPATH + 1] = "";
static  CHAR  szErrorMsg  [1024] = "";

static  USHORT usNumOfTitles;
static  INT    iBreakLnCnt = 0;      /* line counter for materials */


           /* Used by DbExecQuer & DbGetQueryRow functions: */
static  HHEAP   hheap  = NULL;
static  HQRY    hqry   = NULL;
static  USHORT  usResId   = ITIRID_RPT;
static  USHORT  usId      = ID_QUERY_LISTING;
static  USHORT  usNumCols = 0;
static  USHORT  usErr     = 0;
static  PSZ   * ppsz   = NULL;





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

   switch (szOrderingStr[10])
      {
      case 'u':
      case 'U':
         ItiStrCpy(preptitles->szSubTitle,
                   "Listed in Unit Order",
                   sizeof(preptitles->szSubTitle));
         break;

      case 'z':
      case 'Z':
         ItiStrCpy(preptitles->szSubTitle,
                   "Listed in Zone-Type Order",
                   sizeof(preptitles->szSubTitle));
         break;


      default:
         ItiStrCpy(preptitles->szSubTitle,
                   SUB_TITLE,sizeof(preptitles->szSubTitle));
      break;
      }/* end of switch */



   /* -- Set the subsubtitle text to be empty for this report. */
   preptitles->szSubSubTitle[0] = '\0';


   ItiStrCpy(preptitles->szFooter,  FOOTER,   sizeof(preptitles->szFooter)  );

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



   if (usTitleStart == RPT_TTL_7)
      {
      ItiPrtPutTextAt (CURRENT_LN, acttl[usTitleStart].usTtlCol,
                       LEFT, (acttl[usTitleStart].usTtlColWth | REL_LEFT),
                       acttl[usTitleStart].szTitle);
      ItiPrtIncCurrentLn (); //line feed past title
      return 0;
      }



      /* -- Actual printing. */
   for (us=usTitleStart; us <= usTitleStop; us++)
      {
      if (us == RPT_TTL_6) /* The "Unit Price" title */
         {
         ItiPrtPutTextAt (CURRENT_LN, acttl[us].usTtlCol,
                          RIGHT, (acttl[us].usTtlColWth | REL_LEFT),
                          acttl[us].szTitle);
         }
      else
         {
         ItiPrtPutTextAt (CURRENT_LN, acttl[us].usTtlCol,
                          LEFT, (acttl[us].usTtlColWth | REL_LEFT),
                          acttl[us].szTitle);
         }

      if (us >= RPT_TTL_5)
         ItiPrtDrawSeparatorLn ((CURRENT_LN + 1),
                             acttl[us].usTtlCol,
                             acttl[us].usTtlCol+acttl[us].usTtlColWth);
      }

   ItiPrtIncCurrentLn (); //line feed past titles

   if (usTitleStop == RPT_TTL_4)
      ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);

   ItiPrtIncCurrentLn (); //line feed past underlining

   return 0;
   }/* END OF FUNCTION SetColumnTitles */







USHORT WriteRowData (PSZ *ppsz, PSZ pszKey)
   {
   USHORT us   = 0;
   USHORT usLn = 0;
   USHORT usErr;
   USHORT usBreakLnCnt = 0;
   BOOL   bFirstTime = TRUE;
   CHAR   szLabel[31]= "";

   HQRY    hqry2   = NULL;
   USHORT  usNumCols2 = 0;
   USHORT  usErr2     = 0;
   PSZ    *ppsz2 = NULL;


   /* ---------------------------------------------------------------- *\
    * If we are on a new page...                                       *
   \* ---------------------------------------------------------------- */
   usLn = 7 + (ItiStrLen(ppsz[DESC]) / acttl[RPT_TTL_4].usDtaColWth);
   if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
      {
      usCurrentPg = ItiPrtQueryCurrentPgNum();

      /* -- Set the Header text. */
      ItiPrtChangeHeaderLn(ITIPRT_TITLE, NULL);

      /* -- Reset the starting spacer line count. */
      iBreakLnCnt = 0;
   
      /* -- Set the column titles for new pg.     */
      SetColumnTitles(RPT_TTL_1, RPT_TTL_4);
      }


   /* -- Insert blank line at regular intervals for readablity. */
   if ((!bInclZonePrices) && (!bInclFacilities))
      {
      if ((iBreakLnCnt == BLANK_LINE_INTERVAL)   )
         {
         ItiPrtIncCurrentLn();
         iBreakLnCnt = 1;
         }
      else
         iBreakLnCnt++;
      }


   for (us=RPT_TTL_1; us <= RPT_TTL_4; us++)
      {
      if (us < RPT_TTL_4)
         {
         usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[us].usDtaCol,
                               LEFT,
                               (acttl[us].usDtaColWth | REL_LEFT),
                               ppsz[us-1]);
         }
      else
         {
         usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[us].usDtaCol,
                               (LEFT | WRAP | LNINC),
                               (acttl[us].usDtaColWth | REL_LEFT),
                               ppsz[us-1]);
         }/* end if */
      }/* end for */



   /* -- Chapter 9 addition to this report. -------------------------- */
   if (bInclZonePrices)
      {
      /* -- Build the query. */
      ItiRptUtPrepQuery(sz2ndQuery, (QUERY_ARRAY_SIZE + 1),
                        szQueryPrices, pszKey, "  ");

      /* -- Unit type exclusion clause. */
      if (bUseEnglishPrices)
         ItiStrCat(sz2ndQuery, " AND (MP.UnitType = 0 ", sizeof sz2ndQuery);
      else
         ItiStrCat(sz2ndQuery, " AND (MP.UnitType = 1 ", sizeof sz2ndQuery);


      ItiStrCat(sz2ndQuery, szPricesConj, sizeof sz2ndQuery);
      ItiStrCat(sz2ndQuery, pszKey, sizeof sz2ndQuery);
      ItiStrCat(sz2ndQuery, " ) ) ) ", sizeof sz2ndQuery);
      ItiStrCat(sz2ndQuery, " order by ZoneID ", sizeof sz2ndQuery);

      /* -- Do the database query. */
      hqry2 = ItiDbExecQuery (sz2ndQuery, hheap, hmodModule, ITIRID_RPT,
                             ID_QUERY_PRICES, &usNumCols2, &usErr2);
      if (hqry2 == NULL)
         {
         SETQERRMSG
         }
      else
         {
//         if (ItiPrtQueryRemainNumDisplayLns() > 5)
//            SetColumnTitles(RPT_TTL_5, RPT_TTL_6);

         usBreakLnCnt = 0;

         while( ItiDbGetQueryRow (hqry2, &ppsz2, &usErr2) )  
            {
            if ((bFirstTime) && (ItiPrtQueryRemainNumDisplayLns() > 5))
               {
               SetColumnTitles(RPT_TTL_5, RPT_TTL_6);
               bFirstTime = FALSE;
               }


            /* ------------------------------------------------------- *\
             * If we are on a new page...                              *
            \* ------------------------------------------------------- */
            if (ItiPrtKeepNLns(3) == PG_WAS_INC)
               {
               usCurrentPg = ItiPrtQueryCurrentPgNum();
               ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");

               /* -- Set the main column titles for new pg.     */
               SetColumnTitles(RPT_TTL_1, RPT_TTL_4);

               /* continuation marker */
               ItiPrtPutTextAt (CURRENT_LN, 0, LEFT, 7, "(cont.)");

               ItiPrtPutTextAt (CURRENT_LN, 9,
                          LEFT, (20 | REL_LEFT),
                          ppsz2[MAT_ID]);
               ItiPrtIncCurrentLn (); //line feed
               ItiPrtIncCurrentLn (); //blank line


               /* -- Reset the starting spacer line counts. */
               usBreakLnCnt = 1;
               iBreakLnCnt = 0;
   
               /* -- Set the zone\price column titles for new pg. */
               SetColumnTitles(RPT_TTL_5, RPT_TTL_6);
               }
            else
               usBreakLnCnt++;


            /* -- Write out the data. */
            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_5].usDtaCol,
                             LEFT, (acttl[RPT_TTL_5].usDtaColWth | REL_LEFT),
                             ppsz2[ZONE_ID]);

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_6].usDtaCol,
                             RIGHT, (acttl[RPT_TTL_6].usDtaColWth | REL_LEFT),
                             ppsz2[UNIT_PRICE]);


            ItiPrtIncCurrentLn (); //line feed

            ItiFreeStrArray (hheap, ppsz2, usNumCols2);


            if (usBreakLnCnt == BLANK_LINE_INTERVAL)
               {
               ItiPrtIncCurrentLn (); //line feed
               usBreakLnCnt = 0;
               }

            }/* end of while */


//         ItiPrtIncCurrentLn (); //blank line
//         ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
//         ItiPrtIncCurrentLn (); //line feed

         }/* end of else clause, hqry2 ==... */

      }/* end if bInclZonePrices */






   /* -- Chapter 10 addition to this report. -------------------------- */
   if (bInclFacilities)
      {
      /* -- Build the query. */
      ItiRptUtPrepQuery(sz2ndQuery, (QUERY_ARRAY_SIZE + 1),
                        szQueryFacil, pszKey, " order by 1 ");


      /* -- Do the database query. */
      hqry2 = ItiDbExecQuery (sz2ndQuery, hheap, hmodModule, ITIRID_RPT,
                             ID_QUERY_FACIL, &usNumCols2, &usErr2);
      if (hqry2 == NULL)
         {
         SETQERRMSG
         }
      else
         {
         bFirstTime = TRUE;
         usBreakLnCnt = 0;

         while( ItiDbGetQueryRow (hqry2, &ppsz2, &usErr2) )  
            {
            if ((bFirstTime) && (ItiPrtQueryRemainNumDisplayLns() > 3))
               {
               ItiPrtIncCurrentLn (); //blank line
               SetColumnTitles(RPT_TTL_7, RPT_TTL_7);
               bFirstTime = FALSE;
               }


            /* ------------------------------------------------------- *\
             * If we are on a new page...                              *
            \* ------------------------------------------------------- */
            if (ItiPrtKeepNLns(4) == PG_WAS_INC)
               {
               usCurrentPg = ItiPrtQueryCurrentPgNum();
               ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");

               /* -- Set the main column titles for new pg.     */
               SetColumnTitles(RPT_TTL_1, RPT_TTL_4);

               /* continuation marker */
               ItiPrtPutTextAt (CURRENT_LN, 0, LEFT, 7, "(cont.)");

               ItiPrtPutTextAt (CURRENT_LN, 9,
                          LEFT, (20 | REL_LEFT),
                          ppsz2[FMAT_ID]);
               ItiPrtIncCurrentLn (); //line feed
               ItiPrtIncCurrentLn (); //blank line


               /* -- Reset the starting spacer line counts. */
               usBreakLnCnt = 1;
               iBreakLnCnt = 0;
   
               /* -- Set the facility column title for new pg. */
               SetColumnTitles(RPT_TTL_7, RPT_TTL_7);
               }
            else
               usBreakLnCnt++;


            /* -- Write out Facilities, capacities, units/day, approved. */
            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_7].usDtaCol,
                             LEFT, (acttl[RPT_TTL_7].usDtaColWth | REL_LEFT),
                             ppsz2[FACIL_NUM]);

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_8].usDtaCol,
                             RIGHT, (acttl[RPT_TTL_8].usDtaColWth | REL_LEFT),
                             ppsz2[CAPACITY]);

//            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_9].usDtaCol,
//                             RIGHT, (acttl[RPT_TTL_9].usDtaColWth | REL_LEFT),
//                             ppsz2[MAT_UNIT]);
//
//            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_9].usTtlCol,
//                             LEFT, (acttl[RPT_TTL_9].usTtlColWth | REL_LEFT),
//                             acttl[RPT_TTL_9].szTitle);

            ItiStrCpy(szLabel, ppsz2[MAT_UNIT],          sizeof szLabel);
            ItiStrCat(szLabel, acttl[RPT_TTL_9].szTitle, sizeof szLabel);
            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_9].usDtaCol,
                             LEFT, ((acttl[RPT_TTL_9].usDtaColWth
                                    + acttl[RPT_TTL_9].usTtlColWth) | REL_LEFT),
                             szLabel);



            if (ppsz2[APPROVED][0] == '1')
               {
               ItiStrCpy(szLabel, acttl[RPT_TTL_10].szTitle, sizeof szLabel);
               }
            else
               {
               ItiStrCpy(szLabel, "NOT ",                    sizeof szLabel);
               ItiStrCat(szLabel, acttl[RPT_TTL_10].szTitle, sizeof szLabel);
               }
            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_10].usTtlCol,
                             LEFT, (acttl[RPT_TTL_10].usTtlColWth | REL_LEFT),
                             szLabel);

            ItiPrtIncCurrentLn (); //line feed


            /* -- Production ID and its translation. */
            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_11].usDtaCol,
                             RIGHT, (acttl[RPT_TTL_11].usDtaColWth | REL_LEFT),
                             ppsz2[PROD_ID]);

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usTtlCol,
                             LEFT, (acttl[RPT_TTL_12].usTtlColWth | REL_LEFT),
                             acttl[RPT_TTL_12].szTitle);

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usDtaCol,
                             LEFT, (acttl[RPT_TTL_12].usDtaColWth | REL_LEFT),
                             ppsz2[PROD_TRANS]);

            ItiPrtIncCurrentLn (); //line feed


            ItiFreeStrArray (hheap, ppsz2, usNumCols2);


            if (usBreakLnCnt == BLANK_LINE_INTERVAL)
               {
               ItiPrtIncCurrentLn (); //line feed
               usBreakLnCnt = 0;
               }

            }/* end of while */


         ItiPrtIncCurrentLn (); //blank line
//         ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
//         ItiPrtIncCurrentLn (); //line feed

         }/* end of else clause, hqry2 ==... */

      }/* end if bInclFacilities */


   if ((bInclFacilities) || (bInclZonePrices))
      {
      ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
      ItiPrtIncCurrentLn (); //line feed
      }

   return (usErr);
   }/* End of Function WriteRowData */






USHORT ConvertOrderingStr (PSZ pszOrderingStr)
   /* -- Converts OrderingStr from ColIDs into ListingQuery column value. */
   {
   switch (pszOrderingStr[0])
      {
      case 'u':
      case 'U':
         ItiStrCpy(pszOrderingStr, " order by Unit ", LGARRAY);
         break;

      case 'z':
      case 'Z':
         ItiStrCpy(pszOrderingStr, " order by ZoneTypeID ", LGARRAY);
         break;

      case 'm':
      case 'M':
      default:
         ItiStrCpy(pszOrderingStr, " order by MaterialID ", LGARRAY);
         break;
      }/* end switch */

   return (0);
   }/* End of Function ConvertOrderingStr */







USHORT PrintProc (HWND hwnd, MPARAM mp1, MPARAM mp2)
   {
   USHORT us    = 0;
   USHORT usMaterial = 0;
   USHORT usUnit = 0;
   USHORT usZoneType = 0;
   CHAR   szKey      [SMARRAY+1] = "";
   CHAR   szKeyStr   [SMARRAY+1] = "";
   CHAR   szOptSwStr [SMARRAY+1] = "";
   CHAR   szCmdLn    [3*SMARRAY+1] = RPT_DLL_NAME " ";
   CHAR   szFName    [2*SMARRAY+1] = "";
   BOOL   bChecked = FALSE;
   HFILE  hfile;
   HHEAP  hheapLoc;
   PGLOBALS pglobLoc;


   hheapLoc = ItiMemCreateHeap (MIN_HEAP_SIZE);
   if (NULL == hheapLoc)
      {
      return 13;
      }

   pglobLoc = ItiGlobQueryGlobalsPointer();

   if (pglobLoc->bUseMetric)
      ItiStrCat(szCmdLn, RMAT_OPTSW_CURUT_M  RMAT_CURUT_M_CHAR " ",
                sizeof(szCmdLn) );
   else
      ItiStrCat(szCmdLn, RMAT_OPTSW_CURUT_E  RMAT_CURUT_E_CHAR " ",
                sizeof(szCmdLn) );



   /* -- See if Zone-Prices (Chap 9-82) are to be included. */
   bChecked = (USHORT) (ULONG) WinSendDlgItemMsg (hwnd, 
                               DID_INCL_ZP, BM_QUERYCHECK, 0, 0);
   if (bChecked)
      {
      ItiStrCat(szCmdLn, RMAT_OPTSW_INCL_ZP  RMAT_INCL_ZP_CHAR " ",
                sizeof(szCmdLn) );
      }
   


   /* -- See if Facilities (Chap 10-62) are to be included. */
   bChecked = (USHORT) (ULONG) WinSendDlgItemMsg (hwnd, 
                               DID_INCL_FC, BM_QUERYCHECK, 0, 0);
   if (bChecked)
      {
      ItiStrCat(szCmdLn, RMAT_OPTSW_INCL_FC  RMAT_INCL_FC_CHAR " ",
                sizeof(szCmdLn) );
      }
   



   /* -- Now check for selected or all keys. */
   bChecked = (USHORT) (ULONG) WinSendDlgItemMsg (hwnd, 
                               DID_SELECTED, BM_QUERYCHECK, 0, 0);
   if (bChecked)
      {
      /* -- Build cmdln string to do selected rows. */
      us = ItiRptUtBuildSelectedKeysFile(hwnd,
                             MaterialCatL,
                             cMaterialKey,
                             szFName, sizeof szFName);

      if ((us == ITIRPTUT_PARM_ERROR)|| (us == ITIRPTUT_ERROR))
         {
         WinSendDlgItemMsg(hwnd, DID_SELECTED, BM_SETCHECK, UNCHECKED, 0L);
         return (0);
         }
      else
         {
         /* -- Prep the CmdLn to use the key file. */
         ItiStrCat (szCmdLn, ITIRPT_OPTSW_KEYFILE, sizeof szCmdLn);
         ItiStrCat (szCmdLn, szFName, sizeof szCmdLn);

         if((BOOL)(ULONG)WinSendDlgItemMsg(hwnd,DID_MATERIAL,BM_QUERYCHECK,0,0)
             ||(BOOL)(ULONG)WinSendDlgItemMsg(hwnd,DID_ZONETYPE,BM_QUERYCHECK,0,0)
             ||(BOOL)(ULONG)WinSendDlgItemMsg(hwnd,DID_UNIT,BM_QUERYCHECK,0,0) )
            {
            WinSendDlgItemMsg (hwnd, DID_MATERIAL, BM_SETCHECK, UNCHECKED, 0);
            WinSendDlgItemMsg (hwnd, DID_UNIT,     BM_SETCHECK, UNCHECKED, 0);
            WinSendDlgItemMsg (hwnd, DID_ZONETYPE, BM_SETCHECK, UNCHECKED, 0);

            ItiRptUtErrorMsgBox (hwnd,
               DEV_MSG "Ordering is NOT applied to selected materials.");
            }
         }/* end of if ((us... */

      }/* end of if (bChecked... */
   else
      {/* -- Use ALL of the keys. */
      ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_KEYS,
                          szQueryKeys, SIZE_OF_QUERY);


      if (1 == (USHORT)(ULONG)WinSendDlgItemMsg (hwnd, 
                                     DID_ZONETYPE, BM_QUERYCHECK, 0, 0))
         {
         ItiStrCat(szQueryKeys, " order by ZoneTypeID ",sizeof(szQueryKeys));
         ItiStrCat(szCmdLn, " /O z ", sizeof szCmdLn);

         }
      else if (1 == (USHORT)(ULONG)WinSendDlgItemMsg (hwnd, 
                                     DID_UNIT, BM_QUERYCHECK, 0, 0))
         {
         ItiStrCat(szQueryKeys, " order by Unit ", sizeof(szQueryKeys));
         ItiStrCat(szCmdLn, " /O u ", sizeof szCmdLn);
         }
      else
         {
         ItiStrCat(szQueryKeys, " order by MaterialID ",sizeof(szQueryKeys));
         ItiStrCat(szCmdLn, " /O m ", sizeof szCmdLn);
         }


      /* -- Do the database query to get ALL of the keys. */
      hqry = ItiDbExecQuery (szQueryKeys, hheapLoc, hmodModule, ITIRID_RPT,
                             ID_QUERY_KEYS,
                             &usNumCols, &usErr);
      if (hqry == NULL)
         {
         ItiRptUtErrorMsgBox (hwnd,
                "ERROR: Failed to get ALL of the keys in "__FILE__);
         }
      else
         {
         ItiRptUtStartKeyFile (&hfile, szFName, sizeof szFName);

         while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
            {
            ItiRptUtWriteToKeyFile (hfile, ppsz[0], NULL);
            }

         ItiRptUtEndKeyFile (hfile);

         ItiStrCat (szCmdLn, ITIRPT_OPTSW_KEYFILE, sizeof szCmdLn);
         ItiStrCat (szCmdLn, szFName, sizeof szCmdLn);
         }

      }


   us = ItiRptUtExecRptCmdLn (hwnd, szCmdLn, RPT_SESSION);


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
   ItiStrCpy (pszBuffer, "Mater~ials Listing", usMaxSize);   
   *pusWindowID = RMAT_RptDlgBox;                       
   }/* END OF FUNCTION */







BOOL EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID, 
                                     USHORT usActiveWindowID) 
   {                                                          
   if ((usTargetWindowID == RMAT_RptDlgBox)
      && (usActiveWindowID == MaterialCatS))
      return TRUE;                                            
   else                                                       
      return FALSE;                                           
   }/* END OF FUNCTION */




MRESULT EXPORT ItiDllRMATRptDlgProc (HWND     hwnd,     
                                    USHORT   usMessage,
                                    MPARAM   mp1,
                                    MPARAM   mp2)
   {
   USHORT us = 0;


   if (0 < (USHORT)(ULONG)WinSendDlgItemMsg (hwnd, 
                                        DID_SELECTED, BM_QUERYCHECK, 0, 0))
      {
      WinSendDlgItemMsg (hwnd, DID_MATERIAL, BM_SETCHECK, UNCHECKED, 0);
      WinSendDlgItemMsg (hwnd, DID_UNIT,     BM_SETCHECK, UNCHECKED, 0);
      WinSendDlgItemMsg (hwnd, DID_ZONETYPE, BM_SETCHECK, UNCHECKED, 0);

//      ItiRptUtErrorMsgBox (hwnd,
//         "Ordering is NOT applied to selected materials.");
      }


   switch (usMessage)
      {
      case WM_INITDLG:
         us = ItiRptUtInitDlgBox (hwnd, RPT_SESSION, 0, NULL);

         /* -- Set default checked values. */
         WinSendDlgItemMsg (hwnd, DID_MATERIAL,
                            BM_SETCHECK, CHECKED, 0);
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

   }/* END OF FUNCTION ItiDllMapRptDlgProc */








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
   BOOL    bTitlesSet  = FALSE;

           /* Title strings for this module. */
   REPTITLES rept;
   PREPT     prept = &rept;




   /* ------------------------------------------------------------------- *\
    *  Create a memory heap.                                              *
   \* ------------------------------------------------------------------- */
   hheap = ItiMemCreateHeap (MIN_HEAP_SIZE);



   /* ------------------------------------------------------------------- *\
    * Initialize the strings.                                             *
   \* ------------------------------------------------------------------- */
   ItiStrCpy(szCurrentID,"TABLE_ID",sizeof(szCurrentID));


                                        
   /* -- Check which unit prices to use in the report. */
   ItiRptUtGetOption (RMAT_CURUT_M_SWITCH, szOrderingStr, LGARRAY,
                      argcnt, argvars);

   if (szOrderingStr[0] == RMAT_CURUT_M_SWITCH)
      bUseEnglishPrices = FALSE;

   /* -- Reset string to empty. */
   szOrderingStr[0] = '\0';

   ItiRptUtGetOption (RMAT_CURUT_E_SWITCH, szOrderingStr, LGARRAY,
                      argcnt, argvars);

   if (szOrderingStr[0] == RMAT_CURUT_E_SWITCH)
      bUseEnglishPrices = TRUE;

   /* -- Reset string to empty. */
   szOrderingStr[0] = '\0';

     
     

   /* -- Check if zone price info is to be included in the report. */
   ItiRptUtGetOption (RMAT_INCL_ZP_SWITCH, szOrderingStr, LGARRAY,
                      argcnt, argvars);

   if (szOrderingStr[0] == RMAT_INCL_ZP_SWITCH)
      bInclZonePrices = TRUE;
   else
      bInclZonePrices = FALSE;

   /* -- Reset string to empty. */
   szOrderingStr[0] = '\0';


   /* -- Check if facility info is to be included in the report. */
   ItiRptUtGetOption (RMAT_INCL_FC_SWITCH, szOrderingStr, LGARRAY,
                      argcnt, argvars);

   if (szOrderingStr[0] == RMAT_INCL_FC_SWITCH)
      bInclFacilities = TRUE;
   else
      bInclFacilities = FALSE;

   /* -- Reset string to empty. */
   szOrderingStr[0] = '\0';



   /* -- Get any query ordering information, BEFORE setting rpt titles. */
   ItiRptUtGetOption (ITIRPTUT_ORDERING_SWITCH, szOrderingStr, LGARRAY,
                      argcnt, argvars);

   ConvertOrderingStr (szOrderingStr);


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
                       szBaseQuery, SIZE_OF_QUERY);

   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_WHERE_CONJUNCTION,
                       szWhereConj, SIZE_OF_CONJ);

   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_PRICES,
                       szQueryPrices, SIZE_OF_QUERY);

   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_PRICES_CONJ,
                       szPricesConj,  QUERY_ARRAY_SIZE);

   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_KEYS,
                       szQueryKeys, SIZE_OF_QUERY);

   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_FACIL,
                       szQueryFacil, SIZE_OF_QUERY);
                     


   usCurrentPg = ItiPrtQueryCurrentPgNum();
   if (usCurrentPg == 1)
      {
      ItiPrtChangeHeaderLn(ITIPRT_TITLE, NULL);
      SetColumnTitles(RPT_TTL_1, RPT_TTL_4);
      }



   ItiRptUtInitKeyList(argcnt, argvars); /*  */
   while (ItiRptUtGetNextKey(pszKey, sizeof(szKey)) != ITIRPTUT_NO_MORE_KEYS)
      {
      if (*pszKey != '\0')
         {
         /* -- Prepare the query with the current key. */
         ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                           szBaseQuery, szWhereConj, pszKey);
         }
      else
         {
         ItiStrCpy(szQuery, szBaseQuery, QUERY_ARRAY_SIZE);
         }

      ItiStrCat(szQuery, szOrderingStr, QUERY_ARRAY_SIZE);


      /* -- Do the database query. */
      hqry = ItiDbExecQuery (szQuery, hheap, hmodModule, ITIRID_RPT,
                             ID_QUERY_LISTING, &usNumCols, &usErr);
   
   
      /* ---------------------------------------------------------------- *\
       * Process the query results for each row.                          *
      \* ---------------------------------------------------------------- */
      if (hqry == NULL)
         {
         ItiStrCpy (szErrorMsg,
                    "ERROR: Query failed for ",
                    sizeof szErrorMsg);

         ItiStrCat (szErrorMsg, TITLE, sizeof szErrorMsg);
         ItiStrCat (szErrorMsg,", Error Code: ", sizeof szErrorMsg);

         ItiStrUSHORTToString
            (usErr, &szErrorMsg[ItiStrLen(szErrorMsg)],
             sizeof szErrorMsg);

         ItiStrCat (szErrorMsg,".  Query: ", sizeof szErrorMsg);
         ItiStrCat (szErrorMsg, szQuery, sizeof szErrorMsg);

         ItiRptUtSetError (hab, szErrorMsg);
         ItiPrtEndReport();
         break;
         }
      else
         {
         while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )
            {
            WriteRowData (ppsz, pszKey);
            ItiFreeStrArray (hheap, ppsz, usNumCols);
            }/* End of WHILE GetQueryRow for Listing query. */
         }

      }/* End of WHILE GetNextKey */
   





   /* ------------------------------------------------------------------- *\
    * Finish up and quit.                                                 *
   \* ------------------------------------------------------------------- */
   ItiPrtEndReport();

   DBGMSG("Exit ***DLL function ItiRptDLLPrintReport");
   return 0;

   }/* END OF FUNCTION ItiRptDLLPrintReport */
