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


#include    "rinidll.h"


#include "rVend.h"                                       



/* ======================================================================= *\
 *                                         MANIFEST CONSTANT DEFINITIONS   *
\* ======================================================================= */
#define  BLANK_LINE_INTERVAL    4
#define  RPT_SESSION            TITLE
#define  PRT_Q_NAME             "Vendor Listing"

#define  QUERY_ARRAY_SIZE       1023


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
                   ItiRptUtErrorMsgBox (NULL, szErrorMsg)



/* ======================================================================= *\
 *                                             MODULE'S GLOBAL VARIABLES   *
\* ======================================================================= */
static  CHAR  szCompileDateTime[] = __DATE__ " " __TIME__ ;
static CHAR szDllVersion[] = "1.1a0 rVend.dll";

static  CHAR  szID          [SMARRAY + 1] = "";
static  CHAR  szCurrentID   [SMARRAY + 1] = "";
static  CHAR  szOrderingStr [SMARRAY + 1] = "";

static  BOOL  bInclFacilities = FALSE;
static  CHAR  szOptSw  [16] = "";

static  CHAR  szQueryDesc [SIZE_OF_QUERY + 1] = "";
static  CHAR  szQueryAddr [SIZE_OF_QUERY + 1] = "";
static  CHAR  szQueryWork [SIZE_OF_QUERY + 1] = "";

static  CHAR  szQueryFacilNum [SIZE_OF_QUERY_FACIL_NUM + 1] = "";
static  CHAR  szQueryFacil    [SIZE_OF_QUERY_FACIL + 1] = "";
static  CHAR  szQueryFacilConj[SIZE_OF_QUERY_FACIL_CONJ + 1] = "";

static  CHAR  szBaseQuery   [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szQuery       [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szKey         [SMARRAY + 1] = "";

static  CHAR  sz2ndQuery    [QUERY_ARRAY_SIZE + 1] = "";


static  CHAR    szFailedDLL [CCHMAXPATH + 1] = "";
static  CHAR    szErrorMsg [1024] = "";

static  USHORT usNumOfTitles;
static  USHORT usCurrentPg = 0;
static  BOOL   bNeedColumnTitles = FALSE;

static  CHAR    szFName [CCHMAXPATH + 1] = "";

           /* Used by DbExecQuer & DbGetQueryRow functions: */
static  HHEAP   hhprVend  = NULL;
static  HQRY    hqry      = NULL;
static  USHORT  usResId   = ITIRID_RPT;
static  USHORT  usId      = ID_QUERY_DESC;
static  USHORT  usNumCols = 0;
static  USHORT  usErr     = 0;
static  PSZ   * ppsz      = NULL;

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
   usErr = ItiMbQueryQueryText(hmodrVend, ITIRID_RPT, ID_RPT_TTL,
                               szBuff, sizeof szBuff );
   ItiStrToUSHORT(szBuff, &usNumOfTitles);

   /* get the titles themselves. */
   for (us=1; us <= usNumOfTitles; us++)
      {
      usErr = ItiMbQueryQueryText(hmodrVend, ITIRID_RPT, us,
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





USHORT SetColumnTitles (VOID)
   {
   USHORT us;
   USHORT usErr;

      /* -- Page column title printing. */
   for (us=1; us <= 6; us++)
      {
      usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[us].usTtlCol,
                               LEFT, (acttl[us].usTtlColWth | REL_LEFT),
                               acttl[us].szTitle);
      if (usErr != 0)
         break;

      if ((us == 3) || (us == 5))
         ItiPrtIncCurrentLn (); //line feed
      }

   ItiPrtIncCurrentLn (); //line feed

   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   ItiPrtIncCurrentLn (); //line feed

   return usErr;
   }/* END OF FUNCTION SetColumnTitles */







USHORT SummarySectionFor (PSZ pszKey, PSZ pszID)
   {
   USHORT usLn, us, usA, usB, usC;
   PSZ * ppsz = NULL;


   if ((pszKey == NULL) || (*pszKey == '\0'))
      return 13;

   /* -- Build the query. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryDesc, " ", pszKey);


   ppsz = ItiDbGetRow1 (szQuery, hhprVend, hmodrVend, ITIRID_RPT,
                        ID_QUERY_DESC, &usNumCols);

   if (ppsz != NULL)
      {
      usLn = 3 + ItiStrLen(ppsz[NAME]) / acttl[RPT_TTL_4].usDtaColWth;
      if ((ItiPrtKeepNLns(usLn) == PG_WAS_INC)
          || (ItiPrtQueryCurrentPgNum() != usCurrentPg ))
         {
         usCurrentPg = ItiPrtQueryCurrentPgNum();
         ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
         SetColumnTitles();
         }
      else
         {
         if (bNeedColumnTitles) 
            {
            SetColumnTitles();
            bNeedColumnTitles = FALSE;
            }
         }

      if ((ItiStrLen(ppsz[NAME]) % acttl[RPT_TTL_4].usDtaColWth) != 0)
         us = 1;
      else
         us = 0;

      /* -- save the line# of the last line of the Name field. */
      usLn = (usLn - 3) + us;

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_1].usDtaCol,
                       LEFT, (acttl[RPT_TTL_1].usDtaColWth | REL_LEFT),
                       ppsz[ID]);
      ItiStrCpy(pszID, ppsz[ID], sizeof szID);


      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_2].usDtaCol,
                       LEFT, (acttl[RPT_TTL_2].usDtaColWth | REL_LEFT),
                       ppsz[SHRT_NAME]);


      /* -- Determine placement of CodeValue translations. */
      usA = ItiStrLen(ppsz[TYPE]);
      usB = ItiStrLen(ppsz[CERT]);
      usC = ItiStrLen(ppsz[DBEC]);
      us = (usA > usB) ? usA : usB;
      us = ((us > usC)  ? us  : usC) + acttl[RPT_TTL_3].usDtaCol;

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_3].usDtaCol,
                       LEFT, (acttl[RPT_TTL_3].usDtaColWth | REL_LEFT),
                       ppsz[TYPE]);
      ItiPrtPutTextAt (CURRENT_LN, us,
                       LEFT, (acttl[RPT_TTL_7].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_7].szTitle);
      ItiPrtPutTextAt (CURRENT_LN, (us + acttl[RPT_TTL_7].usTtlColWth),
                       LEFT, (FROM_RT_EDGE | 2),
                       ppsz[X_TYPE]);


      ItiPrtPutTextAt (CURRENT_LN + 1, acttl[RPT_TTL_5].usDtaCol,
                       LEFT, (acttl[RPT_TTL_5].usDtaColWth | REL_LEFT),
                       ppsz[CERT]);
      ItiPrtPutTextAt (CURRENT_LN + 1, us,
                       LEFT, (acttl[RPT_TTL_7].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_7].szTitle);
      ItiPrtPutTextAt (CURRENT_LN + 1, (us + acttl[RPT_TTL_7].usTtlColWth),
                       LEFT, (FROM_RT_EDGE | 2),
                       ppsz[X_CERT]);


      ItiPrtPutTextAt (CURRENT_LN + 2, acttl[RPT_TTL_6].usDtaCol,
                       LEFT, (acttl[RPT_TTL_6].usDtaColWth | REL_LEFT),
                       ppsz[DBEC]);
      ItiPrtPutTextAt (CURRENT_LN + 2, us,
                       LEFT, (acttl[RPT_TTL_7].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_7].szTitle);
      ItiPrtPutTextAt (CURRENT_LN + 2, (us + acttl[RPT_TTL_7].usTtlColWth),
                       LEFT, (FROM_RT_EDGE | 2),
                       ppsz[X_DBEC]);



      ItiPrtIncCurrentLn();

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_4].usDtaCol,
                       (LEFT|WRAP), (acttl[RPT_TTL_4].usDtaColWth | REL_LEFT),
                       ppsz[NAME]);

      /* -- line spacing, accounts for tha variable length Name field. */
      // note: ...TextAt's WRAP option always leaves current ln after text.
      if (usLn == 1)
         {
         ItiPrtIncCurrentLn();
         ItiPrtIncCurrentLn();
         }
      else
         ItiPrtIncCurrentLn();


      ItiFreeStrArray (hhprVend, ppsz, usNumCols);
      }
   else
      return 13;

   return 0;
   }/* End of Function SummarySectionFor */





USHORT AddrSectFor (PSZ pszID, PSZ pszKey)
/* -- Address listing function for a given key. */
   {
   USHORT usLn, us, usWideLn, usColWidth;
   CHAR   szAddr[30] = "";
   PSZ    pszAddr;
   BOOL   bDone = FALSE;
   BOOL   bNeedLabel;


   if ((pszKey == NULL) || (*pszKey == '\0') || (pszID == NULL))
      return 13;

   /* -- Build the query. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryAddr, pszKey," order by AddressSequenceNumber ");


   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hhprVend, hmodrVend, ITIRID_RPT,
                          ID_QUERY_ADDR, &usNumCols, &usErr);
   
   /* -- Process the query results for each row. */
   if (hqry == NULL)
      {
      SETQERRMSG;
      }
   else
      {
      bNeedLabel = TRUE;
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
         {
         bDone = TRUE;
         /* -- Check for new page. */
         usLn = 5 + ItiStrLen(ppsz[ADDR])/acttl[RPT_TTL_8].usDtaColWth;
         if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
            {
            usCurrentPg = ItiPrtQueryCurrentPgNum();
            ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
            SetColumnTitles();

            ItiPrtPutTextAt (CURRENT_LN, 0,
                             LEFT, (8 | REL_LEFT),
                             "(cont.) ");

            ItiPrtPutTextAt (CURRENT_LN,
                    (CURRENT_COL + 1),
                    LEFT, (acttl[RPT_TTL_1].usDtaColWth | REL_LEFT),
                    pszID);

            ItiPrtIncCurrentLn (); //line feed
            ItiPrtIncCurrentLn (); //line feed
            }/* end of if (ItiPrtKeep... */

         if (bNeedLabel)
            {/* -- Write out the Address label. */
            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_8].usTtlCol,
                             LEFT, (acttl[RPT_TTL_8].usTtlColWth | REL_LEFT),
                             acttl[RPT_TTL_8].szTitle);
            bNeedLabel = FALSE;
            }

         /* -- Longitude: */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_9].usTtlCol,
                          LEFT, (acttl[RPT_TTL_9].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_9].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_9].usDtaCol,
                          LEFT,
                          (acttl[RPT_TTL_9].usDtaColWth | REL_LEFT),
                          ppsz[LNG]);

         /* -- Latitude: */
         ItiPrtPutTextAt ((CURRENT_LN+1), acttl[RPT_TTL_10].usTtlCol,
                          LEFT, (acttl[RPT_TTL_10].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_10].szTitle);

         ItiPrtPutTextAt ((CURRENT_LN+1), acttl[RPT_TTL_10].usDtaCol,
                          LEFT,
                          (acttl[RPT_TTL_10].usDtaColWth | REL_LEFT),
                          ppsz[LAT]);
         usWideLn = ItiPrtQueryCurrentLnNum() + 2;


         /* -- Now parse and place the address text, assumes that the
          * -- text feilds are CSV. */
         usColWidth = (acttl[RPT_TTL_8].usDtaColWth | REL_LEFT);
         us = 1;
         do {
            ItiCsvGetField (us, ppsz[ADDR], szAddr, sizeof szAddr);
            pszAddr = &szAddr[0];
            if (*pszAddr == '\0')
               break;

            while (*pszAddr == ' ')  // Skip over leading blanks.
               pszAddr++;
            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_8].usDtaCol,
                             LEFT, usColWidth, pszAddr);

            if (ItiPrtIncCurrentLn() >= usWideLn)
               usColWidth = USE_RT_EDGE;
               /* Widen the address space now that we are past Long/Lat. */
            us++;
            }while (szAddr[0] != '\0');


         /* -- Concat and print the City, State, Zip */
         ItiStrCpy(szAddr, ppsz[CITY], sizeof szAddr);
         ItiStrCat(szAddr, ", ",       sizeof szAddr);
         ItiStrCat(szAddr, ppsz[STATE],sizeof szAddr);
         ItiStrCat(szAddr, "   ",      sizeof szAddr);
         ItiStrCat(szAddr, ppsz[ZIP], sizeof szAddr);
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_8].usDtaCol,
                          LEFT,
                          (acttl[RPT_TTL_8].usDtaColWth | REL_LEFT),
                          szAddr);
         ItiPrtIncCurrentLn (); //line feed

         /* -- Now print the phone number. */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_8].usDtaCol,
                          LEFT,
                          (acttl[RPT_TTL_8].usDtaColWth | REL_LEFT),
                          ppsz[PHONE]);
         ItiPrtIncCurrentLn (); //line feed
         ItiPrtIncCurrentLn (); //blank line 

         ItiFreeStrArray (hhprVend, ppsz, usNumCols);
         }/* End of WHILE GetQueryRow */


      if (!bDone)
         /* We did NOT write any addresses, indicate such. */
         {
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_8].usTtlCol,
                          LEFT, (acttl[RPT_TTL_8].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_8].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_8].usDtaCol,
                          LEFT,
                          (acttl[RPT_TTL_8].usDtaColWth | REL_LEFT),
                          "None are in the database.");
         ItiPrtIncCurrentLn (); //line feed
         ItiPrtIncCurrentLn (); //line feed
         }

      }/* else clause */

   }/* End of Function AddrSectFor */








USHORT WorkSectFor (PSZ pszID, PSZ pszKey)
/* -- Work Classes listing function for a given key. */
   {
   USHORT usLn = 1;
   BOOL   bDone = FALSE;
   BOOL   bNeedLabel;


   if ((pszKey == NULL) || (*pszKey == '\0') || (pszID == NULL))
      return 13;

   /* -- Build the query. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryWork, pszKey," order by 1 ");


   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hhprVend, hmodrVend, ITIRID_RPT,
                          ID_QUERY_WORK, &usNumCols, &usErr);
   
   /* -- Process the query results for each row. */
   if (hqry == NULL)
      {
      SETQERRMSG;
      }
   else
      {
      bNeedLabel = TRUE;
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
         {
         bDone = TRUE;
         /* -- Check for new page. */
         if (ItiPrtKeepNLns(3) == PG_WAS_INC)
            {
            usCurrentPg = ItiPrtQueryCurrentPgNum();
            ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
            SetColumnTitles();
            usLn = 1;

            ItiPrtPutTextAt (CURRENT_LN, 0,
                             LEFT, (8 | REL_LEFT),
                             "(cont.) ");

            ItiPrtPutTextAt (CURRENT_LN, (CURRENT_COL + 1),
                    LEFT, (acttl[RPT_TTL_1].usDtaColWth | REL_LEFT),
                    pszID);

            ItiPrtIncCurrentLn (); //line feed
            ItiPrtIncCurrentLn (); //line feed
            }/* end of if (ItiPrtKeep... */

         if (bNeedLabel)
            {/* -- Write out the label. */
            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_11].usTtlCol,
                             LEFT, (acttl[RPT_TTL_11].usTtlColWth | REL_LEFT),
                             acttl[RPT_TTL_11].szTitle);
            bNeedLabel = FALSE;
            }


         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_11].usDtaCol,
                          LEFT,
                          (acttl[RPT_TTL_11].usDtaColWth | REL_LEFT),
                          ppsz[ID]);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usTtlCol,
                          LEFT,
                          (acttl[RPT_TTL_12].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_12].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usDtaCol,
                          LEFT,
                          (acttl[RPT_TTL_12].usDtaColWth | REL_LEFT),
                          ppsz[TRANS]);
         ItiPrtIncCurrentLn (); //line feed


         if (BLANK_LINE_INTERVAL == usLn)
            {
            ItiPrtIncCurrentLn (); //line feed
            usLn = 1;
            }
         else
            usLn++;


         ItiFreeStrArray (hhprVend, ppsz, usNumCols);
         }/* End of WHILE GetQueryRow */


      if (!bDone)
         { /* We did NOT write any work classes, indicate such. */
         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_11].usTtlCol,
                          LEFT, (acttl[RPT_TTL_11].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_11].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_8].usDtaCol,
                          LEFT,
                          (acttl[RPT_TTL_8].usDtaColWth | REL_LEFT),
                          "None are in the database.");
         ItiPrtIncCurrentLn (); //line feed
         ItiPrtIncCurrentLn (); //line feed
         }
      else
         if (1 != usLn)
            ItiPrtIncCurrentLn (); //line feed


      }/* else clause */

   }/* End of Function WorkSectFor */





USHORT ListFacilities (PSZ pszVendorID, PSZ pszVendorKey)
   {
   HQRY    hqry2   = NULL;
   USHORT  usNumCols2 = 0;
   USHORT  usErr2     = 0;
   PSZ    *ppsz2 = NULL;
   BOOL    bFirstTime;
   USHORT  usBreakLnCnt;
   USHORT  usCnt;
   USHORT  usFacilityCnt;
   PP_KEYSIDS ppkis = NULL;
   KEY_ID  stcKeyID;


   /* == Chapter 10 addition to this report. ========================== */

   ItiRptUtPrepQuery(sz2ndQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryFacilNum, " ", pszVendorKey);

   /* -- Get the number of facilities for this vendor key. */
   usFacilityCnt = ItiDbGetQueryCount(sz2ndQuery, &usNumCols2, &usErr2);

   /* -- Build a list of those FacilityNumbers. */
   ItiRptUtEstablishKeyIDList (hhprVend, usFacilityCnt + 1,
                               sizeof(stcKeyID), &ppkis);

   hqry2 = ItiDbExecQuery (sz2ndQuery, hhprVend, hmodrVend, ITIRID_RPT,
                          ID_QUERY_FACIL_NUM, &usNumCols2, &usErr2);
   if (hqry2 == NULL)
      {
      SETQERRMSG;
      }
   else
      {
      usCnt = 0;
      while( ItiDbGetQueryRow (hqry2, &ppsz2, &usErr2) )  
         {
         ItiRptUtAddToKeyIDList(hhprVend,
                                ppsz2[FACIL_NUMBER], ppsz2[VENDOR_KEY],
                                usCnt, ppkis, usFacilityCnt);
         usCnt++;
         ItiFreeStrArray (hhprVend, ppsz2, usNumCols2);
         }/* end of while */

      }/* end of if (hqry2... else clause */


   /* -- Get facility and material information for each facility number. */
   usCnt = 0;
   while((usCnt < usFacilityCnt)
         &&(ppkis[usCnt] != NULL)
         &&(ppkis[usCnt]->pszID != NULL)
         &&(ppkis[usCnt]->pszID[0] != '\0'))
      {
      ItiRptUtPrepQuery(sz2ndQuery, (QUERY_ARRAY_SIZE + 1),
                        szQueryFacil, pszVendorKey, szQueryFacilConj);

      ItiStrCat(sz2ndQuery, ppkis[usCnt]->pszID, sizeof sz2ndQuery);
      ItiStrCat(sz2ndQuery, "' ", sizeof sz2ndQuery);

      /* -- Do the database query. */
      hqry2 = ItiDbExecQuery (sz2ndQuery, hhprVend, hmodrVend, ITIRID_RPT,
                          ID_QUERY_FACIL, &usNumCols2, &usErr2);
      if (hqry2 == NULL)
         {
         SETQERRMSG;
         }
      else
         {
         bFirstTime = TRUE;
         usBreakLnCnt = 0;

         while( ItiDbGetQueryRow (hqry2, &ppsz2, &usErr2) )  
            {
            if (bFirstTime)
               {
               bFirstTime = FALSE;

               if (ItiPrtKeepNLns(8) == PG_WAS_INC)
                  {
                  usCurrentPg = ItiPrtQueryCurrentPgNum();
                  ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");

                  /* -- Continuation marker */
                  ItiPrtPutTextAt (CURRENT_LN, 0, LEFT, 7, "(cont.)");

                  ItiPrtPutTextAt (CURRENT_LN, 9,
                             LEFT, (20 | REL_LEFT),
                             pszVendorID);
                  ItiPrtIncCurrentLn (); //line feed
                  ItiPrtIncCurrentLn (); //blank line
                  }
               else
                  {
                  ItiPrtIncCurrentLn (); //blank line
                  }


               /* Facility number. */
               ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_13].usTtlCol, LEFT,
                                (acttl[RPT_TTL_13].usTtlColWth | REL_LEFT),
                                acttl[RPT_TTL_13].szTitle);

               ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_13].usDtaCol, LEFT,
                                (acttl[RPT_TTL_13].usDtaColWth | REL_LEFT),
                                ppkis[usCnt]->pszID);


               /* Date opened. */
               ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL +1, LEFT,
                                (acttl[RPT_TTL_14].usTtlColWth | REL_LEFT),
                                acttl[RPT_TTL_14].szTitle);

               ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL +1, LEFT,
                                (acttl[RPT_TTL_14].usDtaColWth | REL_LEFT),
                                ppsz2[DATE_OPENED]);


               /* Date closed. */
               ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL +1, LEFT,
                                (acttl[RPT_TTL_15].usTtlColWth | REL_LEFT),
                                acttl[RPT_TTL_15].szTitle);

               ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL +1, LEFT,
                                (acttl[RPT_TTL_15].usDtaColWth | REL_LEFT),
                                ppsz2[DATE_CLOSED]);

               ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL +1, LEFT,
                                (acttl[RPT_TTL_16].usTtlColWth | REL_LEFT),
                                acttl[RPT_TTL_16].szTitle);


               /* Portability status. */
               ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_9].usDtaCol, RIGHT,
                                (USE_RT_EDGE),
                                ppsz2[PORT_FLAG]);
               ItiPrtIncCurrentLn (); //line feed

               /* Location. */
               ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_26].usTtlCol, LEFT,
                                (acttl[RPT_TTL_26].usTtlColWth | REL_LEFT),
                                acttl[RPT_TTL_26].szTitle);

               ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL +1, LEFT,
                                (acttl[RPT_TTL_17].usDtaColWth | REL_LEFT),
                                ppsz2[LONGITUDE]);

               ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL +1, LEFT,
                                (acttl[RPT_TTL_17].usTtlColWth | REL_LEFT),
                                acttl[RPT_TTL_17].szTitle);

               ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL +2, LEFT,
                                (acttl[RPT_TTL_18].usDtaColWth | REL_LEFT),
                                ppsz2[LATITUDE]);

               ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL +1, LEFT,
                                (acttl[RPT_TTL_18].usTtlColWth | REL_LEFT),
                                acttl[RPT_TTL_18].szTitle);
               ItiPrtIncCurrentLn (); //line feed


               /* in xxxxx county. */
               ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_19].usTtlCol, LEFT,
                                (acttl[RPT_TTL_19].usTtlColWth | REL_LEFT),
                                acttl[RPT_TTL_19].szTitle);

               ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1, LEFT,
                                (acttl[RPT_TTL_19].usDtaColWth | REL_LEFT),
                                ppsz2[COUNTY_NAME]);

               ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL +1, LEFT,
                                (acttl[RPT_TTL_20].usTtlColWth | REL_LEFT),
                                acttl[RPT_TTL_20].szTitle);
               ItiPrtIncCurrentLn (); //line feed
               ItiPrtIncCurrentLn (); //blank feed

               ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_21].usTtlCol, LEFT,
                                (acttl[RPT_TTL_21].usTtlColWth | REL_LEFT),
                                acttl[RPT_TTL_21].szTitle);
               ItiPrtIncCurrentLn (); //line feed

               }/* end of if (bFirstTime... */



            if (ItiPrtKeepNLns(2) == PG_WAS_INC)
               {
               usCurrentPg = ItiPrtQueryCurrentPgNum();
               ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");

               /* continuation marker */
               ItiPrtPutTextAt (CURRENT_LN, 0, LEFT, 7, "(cont.)");

               ItiPrtPutTextAt (CURRENT_LN, 9,
                                LEFT, (20 | REL_LEFT),
                                pszVendorID);

               ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                          LEFT, (20 | REL_LEFT),
                          ", Materials from: ");

               ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                          LEFT, (20 | REL_LEFT),
                          ppkis[usCnt]->pszID);
               ItiPrtIncCurrentLn (); //line feed
               ItiPrtIncCurrentLn (); //blank line

               /* -- Reset the starting spacer line counts. */
               usBreakLnCnt = 1;
               }
            else
               usBreakLnCnt++;


            /* Material ID. */
            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_21].usDtaCol, LEFT,
                             (acttl[RPT_TTL_21].usDtaColWth | REL_LEFT),
                             ppsz2[MAT_ID]);

            /* capacity. */
            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_22].usDtaCol, RIGHT,
                             (acttl[RPT_TTL_22].usDtaColWth | REL_LEFT),
                             ppsz2[CAPACITY]);

            /* material units. */
            ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL +2, LEFT,
                             (acttl[RPT_TTL_23].usDtaColWth | REL_LEFT),
                             ppsz2[MAT_UNITS]);

            ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL +1, LEFT,
                             (acttl[RPT_TTL_23].usTtlColWth | REL_LEFT),
                             acttl[RPT_TTL_23].szTitle);


            /* -- Production ID and its translation. */
            ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL +1,
                             LEFT, (acttl[RPT_TTL_24].usTtlColWth | REL_LEFT),
                             acttl[RPT_TTL_24].szTitle);

            ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL +1,
                             LEFT, (acttl[RPT_TTL_24].usDtaColWth | REL_LEFT),
                             ppsz2[PROD_METH]);

            ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                             LEFT, (acttl[RPT_TTL_25].usTtlColWth | REL_LEFT),
                             acttl[RPT_TTL_25].szTitle);

            ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                             LEFT, (7 | FROM_RT_EDGE),
                             ppsz2[PROD_DESC]);

            ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                             LEFT, (2 | REL_LEFT),
                             ") ");

            /* State approved flag. */
            ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                             RIGHT, USE_RT_EDGE,
                             ppsz2[APPROVED]);

            ItiPrtIncCurrentLn (); //line feed


            ItiFreeStrArray (hhprVend, ppsz2, usNumCols2);


//            if (usBreakLnCnt == BLANK_LINE_INTERVAL)
//               {
//               ItiPrtIncCurrentLn (); //line feed
//               usBreakLnCnt = 0;
//               }
//
            }/* end of while( ItiDbGetQueryRow... */

         }/* end of else clause, hqry2 ==... */

      usCnt++;
      bNeedColumnTitles = TRUE;

      }/* end of while((usCnt < usFacilityCnt... */


   ItiRptUtDestroyKeyIDList (hhprVend, &ppkis, usFacilityCnt);


   return 0;

   }/* End of Function ListFacilities */









USHORT PrintProc (HWND hwnd, MPARAM mp1, MPARAM mp2)
   {
   USHORT us;
   CHAR   szKey      [SMARRAY] = "";
   CHAR   szOptSwStr [SMARRAY] = "";
   CHAR   szCmdLn [5*SMARRAY +1] = "";
   CHAR   szFName [4*SMARRAY +1] = "";
   BOOL   bChecked = FALSE;


   /* -- reset the command line. */
   ItiStrCpy (szCmdLn, RPT_DLL_NAME, sizeof(szCmdLn) );


   /* -- Check if the user wants to include facilities. */
   bChecked = (USHORT)(ULONG)WinSendDlgItemMsg (hwnd, DID_INCL_FACIL,
                                                BM_QUERYCHECK, 0L, 0L);
   if (bChecked)
      {
      /* -- Add the InclFacilities option switch to the cmdln string. */
      ItiStrCat (szCmdLn,
                 OPTSW_INCL_FACIL   OPTSW_INCL_FACIL_VAL_STR,
                 sizeof szCmdLn);
      }



   ItiStrCat (szCmdLn, ITIRPT_OPTSW_KEYFILE, sizeof szCmdLn);


   bChecked = (USHORT)(ULONG)WinSendDlgItemMsg (hwnd, DID_SELECTED,
                                                BM_QUERYCHECK, 0L, 0L);
   if (bChecked)
      {
      /* -- Build cmdln string to do selected keys. */
      us = ItiRptUtBuildSelectedKeysFile(hwnd,
                             VendorCatL,
                             cVendorKey,
                             szFName, sizeof szFName);

      if ((us == ITIRPTUT_PARM_ERROR)|| (us == ITIRPTUT_ERROR))
         {
         ItiRptUtErrorMsgBox (hwnd,
                "ERROR: Failed to get selected keys in "__FILE__);
         return (us);
         }
      else
         {
         /* -- File was generated so the CmdLn will use the key file. */
         }/* end of if ((us... */

      }/* end of if (bChecked... then clause */
   else
      {/* -- Prep the CmdLn to use all of the vendor keys. */
      ItiStrCpy(szFName,
                " \"" ITIRPTUT_KEY_QUERY_SW_STR
                "select VendorKey from Vendor "
                "where Deleted is NULL"
                " order by VendorID"
                " \" ",
                sizeof szFName);
      }/* end of if (bChecked... else clause */

   ItiStrCat (szCmdLn, szFName, sizeof szCmdLn);

   us = ItiRptUtExecRptCmdLn (hwnd, szCmdLn, RPT_VEND_SESSION);

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
   ItiStrCpy (pszBuffer, "~Vendor Listing", usMaxSize);   
   *pusWindowID = rVend_RptDlgBox;                       
   }/* END OF FUNCTION */




BOOL EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID, 
                                     USHORT usActiveWindowID) 
   {                                                          
   if ((usTargetWindowID == rVend_RptDlgBox)
      && ((usActiveWindowID == VendorCatS)
          || (usActiveWindowID == VendorCatL)) )
      return TRUE;                                            
   else                                                       
      return FALSE;                                           
   }/* END OF FUNCTION */



MRESULT EXPORT ItiDllrVendRptDlgProc (HWND     hwnd,     
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
         us = ItiRptUtInitDlgBox (hwnd, RPT_SESSION, 0, NULL);
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
               else
                  {
                  WinSendDlgItemMsg (hwnd, DID_SELECTED,
                                     BM_SETCHECK, UNCHECKED, 0L);
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
   hhprVend = ItiMemCreateHeap (MIN_HEAP_SIZE);


   /* ------------------------------------------------------------------- *\
    * Initialize the strings.                                             *
   \* ------------------------------------------------------------------- */
   ItiStrCpy(szCurrentID,"TABLE_ID",sizeof(szCurrentID));


   ItiRptUtGetOption (OPTSW_INCL_FACIL_CHR, szOptSw, sizeof szOptSw,
                      argcnt, argvars);
   if (szOptSw[0] == OPTSW_INCL_FACIL_VAL_CHR )
      bInclFacilities = TRUE;
   else
      bInclFacilities = FALSE;



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



   ItiMbQueryQueryText(hmodrVend, ITIRID_RPT, ID_QUERY_DESC,
                       szQueryDesc, sizeof szQueryDesc );

   ItiMbQueryQueryText(hmodrVend, ITIRID_RPT, ID_QUERY_ADDR,
                       szQueryAddr, sizeof szQueryAddr );

   ItiMbQueryQueryText(hmodrVend, ITIRID_RPT, ID_QUERY_WORK,
                       szQueryWork, sizeof szQueryWork );


   ItiMbQueryQueryText(hmodrVend, ITIRID_RPT, ID_QUERY_FACIL_NUM,
                       szQueryFacilNum, sizeof szQueryFacilNum );

   ItiMbQueryQueryText(hmodrVend, ITIRID_RPT, ID_QUERY_FACIL,
                       szQueryFacil, sizeof szQueryFacil );

   ItiMbQueryQueryText(hmodrVend, ITIRID_RPT, ID_QUERY_FACIL_CONJ,
                       szQueryFacilConj, sizeof szQueryFacilConj );





   /* -------------------------------------------------------------------- *\
    * -- For each VendorKey...                                           *
   \* -------------------------------------------------------------------- */
   ItiRptUtInitKeyList(argcnt, argvars); /*  */
   while (ItiRptUtGetNextKey(pszKey,sizeof(szKey)) != ITIRPTUT_NO_MORE_KEYS)
      {
      us = SummarySectionFor(pszKey, szID);

      if (us == 0)
         {
         AddrSectFor(szID, pszKey);
         WorkSectFor(szID, pszKey);
         if (bInclFacilities)
            {
            ListFacilities(szID, pszKey);
            }

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





