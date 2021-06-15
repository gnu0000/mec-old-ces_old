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


#include    "rindljo.h"


#include "rPESjo.h"                                       



/* ======================================================================= *\
 *                                         MANIFEST CONSTANT DEFINITIONS   *
\* ======================================================================= */
#define  BLANK_LINE_INTERVAL    4
#define  PRT_Q_NAME             "Job/Proj Rpt"

#define  QUERY_ARRAY_SIZE       1023


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
static CHAR szDllVersion[] = "1.1a0 rPesjo.dll";

static  CHAR  szID   [SMARRAY + 1] = "";
static  CHAR  szOrderingStr [SMARRAY + 1] = "";

static  CHAR  szFailedDLL [CCHMAXPATH + 1] = "";
static  CHAR  szErrorMsg  [1024] = "";

static  USHORT usNumOfTitles;
static  USHORT usCurrentPg = 0;

static  CHAR  szBaseQuery   [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szQuery       [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szKey         [SMARRAY + 1] = "";

static  CHAR  szQueryKeys    [SIZE_OF_QUERY_KEYS + 1] = "";
static  CHAR  szQueryListing [SIZE_OF_QUERY_LISTING + 1] = "";
static  CHAR  szQueryPCN     [SIZE_OF_QUERY_PCN + 1] = "";
static  CHAR  szQueryCat     [SIZE_OF_QUERY_CAT + 1] = "";
static  CHAR  szQueryItm     [SIZE_OF_QUERY_ITM + 1] = "";


           /* Used by DbExecQuer & DbGetQueryRow functions: */
static  HHEAP   hhprPESjo  = NULL;
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






USHORT  AlterHeader (USHORT usSelect, PSZ pszNewStr)
   {
   CHAR   szNewText[TITLE_LEN +1] = "";
   USHORT us;

   for (us=0; us<TITLE_LEN; us++)
      szNewText[us] = ' ';

   switch(usSelect)
      {
      case ITIPRT_TITLE:
         us = ItiPrtChangeHeaderLn (usSelect, szNewText);

         ItiStrCpy(szNewText, TITLE_LEAD,  sizeof(szNewText) );
         ItiStrCat(szNewText, pszNewStr, sizeof(szNewText) );
      
         /* -- Remove trailing blanks. */
         us = (ItiStrLen(szNewText)) - 1;
         while ((us > 1) && (szNewText[us] == ' ') && (szNewText[us-1] == ' ') )
            {
            szNewText[us] = '\0';
            us--;
            }
      
         ItiStrCat(szNewText, TITLE_TAIL,  sizeof(szNewText) );
         us = ItiPrtChangeHeaderLn (usSelect, szNewText);
         break;

//      case ITIPRT_SUBTITLE:
//         ItiStrCpy(szNewText, "Items sorted by ",  sizeof(szNewText) );
//         ItiStrCat(szNewText, pszNewStr, sizeof(szNewText) );
//         us = ItiPrtChangeHeaderLn (usSelect, szNewText);
//         break;

      default:
         break;
      }

   return (us);
   }/* END OF FUNCTION AlterHeader */







USHORT SetColumnTitles (void)
   /* -- Print the column titles. */
   {
   USHORT us;

      /* -- Actual printing. */
   for (us=RPT_TTL_6; us <= RPT_TTL_12; us++)
      {
      ItiPrtPutTextAt (CURRENT_LN, acttl[us].usTtlCol,
                       LEFT, (acttl[us].usTtlColWth | REL_LEFT),
                       acttl[us].szTitle);
      }
   ItiPrtIncCurrentLn (); //line feed
   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_13].usTtlCol,
                    LEFT, (acttl[RPT_TTL_13].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_13].szTitle);
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








USHORT SummarySection (PSZ pszKey)
   {
   USHORT usLn, us;
   BOOL  bShowComma;
   PSZ * ppsz = NULL;
   HQRY    hqryLoc   = NULL;
   USHORT  usNumColsLoc = 0;
   USHORT  usErrLoc     = 0;
   PSZ   * ppszLoc   = NULL;


   if ((pszKey == NULL) || (*pszKey == '\0'))
      return 13;

   /* -- Build the query. */                  
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryListing, " ", pszKey);


   ppsz = ItiDbGetRow1 (szQuery, hhprPESjo, hmodrPESjo, ITIRID_RPT,
                        ID_QUERY_LISTING, &usNumCols);

   if (ppsz != NULL)
      {
      usLn = 6 + ItiStrLen(ppsz[LISTING_DESC]) / acttl[RPT_TTL_2].usDtaColWth;
      if ((ItiPrtKeepNLns(usLn) == PG_WAS_INC)
          || (ItiPrtQueryCurrentPgNum() != usCurrentPg ))
         {
         usCurrentPg = ItiPrtQueryCurrentPgNum();
         AlterHeader (ITIPRT_TITLE, ppsz[LISTING_ID]);
         ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
         }

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_1].usTtlCol,
                       LEFT, (acttl[RPT_TTL_1].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_1].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_1].usDtaCol,
                       LEFT, (acttl[RPT_TTL_1].usDtaColWth | REL_LEFT),
                       ppsz[LISTING_ID]);

      us = acttl[RPT_TTL_1].usDtaCol + ItiStrLen(ppsz[LISTING_ID]);

      ItiPrtPutTextAt (CURRENT_LN, us,
                       LEFT, (acttl[RPT_TTL_2].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_2].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, (us + acttl[RPT_TTL_2].usTtlColWth),
                       (LEFT|WRAP),
                       (2 | FROM_RT_EDGE),
                       //(acttl[RPT_TTL_2].usDtaColWth | REL_LEFT),
                       ppsz[LISTING_DESC]);
      ItiPrtIncCurrentLn(); //blank line

      ItiFreeStrArray (hhprPESjo, ppsz, usNumCols);
      }
   else
      return 13;


   /* ==== Now do the PCN query. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryPCN, pszKey, " order by 1 ");

   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hhprPESjo, hmodrPESjo, ITIRID_RPT,
                          ID_QUERY_PCN, &usNumCols, &usErr);
   
   /* -- Process the query results for each row. */
   if (hqry == NULL)
      {
      SETQERRMSG;
      }
   else
      {
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
         {
         /* -- Check for new page. */
         usLn = 3 + ItiStrLen(ppsz[PCN_DESC]) / acttl[RPT_TTL_4].usDtaColWth;
         if ((ItiPrtKeepNLns(usLn) == PG_WAS_INC)
             || (ItiPrtQueryCurrentPgNum() != usCurrentPg ))
            {
            usCurrentPg = ItiPrtQueryCurrentPgNum();
            ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
            }

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_3].usTtlCol,
                          LEFT, (acttl[RPT_TTL_3].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_3].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_3].usDtaCol,
                          LEFT, (acttl[RPT_TTL_3].usDtaColWth | REL_LEFT),
                          ppsz[PCN]);

         us = acttl[RPT_TTL_3].usDtaCol + ItiStrLen(ppsz[PCN]);

         ItiPrtPutTextAt (CURRENT_LN, us,
                          LEFT, (acttl[RPT_TTL_4].usTtlColWth | REL_LEFT),
                          acttl[RPT_TTL_4].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, (us + acttl[RPT_TTL_4].usTtlColWth),
                          (LEFT|WRAP),
                          (USE_RT_EDGE),
                          //(acttl[RPT_TTL_4].usDtaColWth | REL_LEFT),
                          ppsz[LISTING_DESC]);
//         ItiPrtIncCurrentLn(); //blank line



         /* === Now do the project's categories: */
         ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                           szQueryCat, ppsz[PCN], "' order by 1 ");

         /* -- Do the database query. */
         hqryLoc = ItiDbExecQuery (szQuery, hhprPESjo, hmodrPESjo, ITIRID_RPT,
                                ID_QUERY_CAT, &usNumColsLoc, &usErrLoc);
         
         /* -- Process the query results for each row. */
         if (hqryLoc == NULL)
            {
            SETQERRMSG;
            }
         else
            {
            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_5].usTtlCol,
                             LEFT, (acttl[RPT_TTL_5].usTtlColWth | REL_LEFT),
                             acttl[RPT_TTL_5].szTitle);

            bShowComma = FALSE;
            while( ItiDbGetQueryRow (hqryLoc, &ppszLoc, &usErrLoc) )  
               {
               /* -- Check for new page. */
               if ((ItiPrtKeepNLns(2) == PG_WAS_INC)
                   || (ItiPrtQueryCurrentPgNum() != usCurrentPg ))
                  {
                  usCurrentPg = ItiPrtQueryCurrentPgNum();
                  ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
                  ItiPrtPutTextAt (CURRENT_LN, 0,
                                   LEFT, (30 | REL_LEFT), "(cont.) Project: ");
                  ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL+1,
                                   LEFT, (16 | REL_LEFT), ppsz[PCN]);
                  ItiPrtIncCurrentLn();

                  ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_5].usTtlCol,
                             LEFT, (acttl[RPT_TTL_5].usTtlColWth | REL_LEFT),
                             acttl[RPT_TTL_5].szTitle);
                  }

               if (ItiPrtQueryCurrentColNum() >= 75)
                 {
                 ItiPrtIncCurrentLn();
                 ItiPrtMoveTo(CURRENT_LN, acttl[RPT_TTL_5].usDtaCol);
                 }

               if (bShowComma)
                  {
                  ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL,
                                   LEFT, (3 | REL_LEFT), ", ");
                  }
               else
                  bShowComma = TRUE;


               ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL,
                             LEFT, (acttl[RPT_TTL_5].usDtaColWth | REL_LEFT),
                             ppszLoc[CAT_ID]);


               ItiFreeStrArray (hhprPESjo, ppszLoc, usNumColsLoc); /* CAT */
               }/* end while CAT */
            ItiPrtIncCurrentLn(); //line feed
            ItiPrtIncCurrentLn(); //blank line

            }/* end else CAT */

         ItiFreeStrArray (hhprPESjo, ppsz, usNumCols); /* PCN */
         }/* end while PCN */
      }/* end else PCN */

   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   ItiPrtIncCurrentLn (); //line feed

   return 0;
   }/* End of Function SummarySection */





USHORT ItemsSection (PSZ pszKey)
   {
   USHORT usLn;
   BOOL   bLabeled, bFirst;
   CHAR   szCurrentID[SMARRAY + 1] = "";


   if ((pszKey == NULL) || (*pszKey == '\0'))
      return 13;

   /* -- Build the query. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryItm, pszKey, " order by 1 ");


   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hhprPESjo, hmodrPESjo, ITIRID_RPT,
                          ID_QUERY_ITM, &usNumCols, &usErr);
   
   /* -- Process the query results for each row. */
   if (hqry == NULL)
      {
      SETQERRMSG;
      }
   else
      {
      bLabeled = FALSE;
      bFirst = TRUE;
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
         {
         /* -- Check for new page. */
         usLn = 4;
         /* -- Check for new page. */
         if ((ItiPrtKeepNLns(usLn) == PG_WAS_INC)
             || (ItiPrtQueryCurrentPgNum() != usCurrentPg ))
            {
            usCurrentPg = ItiPrtQueryCurrentPgNum();
            ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
            SetColumnTitles();
            bLabeled = TRUE;
            }

         if (!bLabeled)
            {
            SetColumnTitles();
            bLabeled = TRUE;
            }

         if (ItiStrCmp(szCurrentID, ppsz[ITM_NUM]) != 0)
            {
            if (!bFirst)
               { 
               ItiPrtIncCurrentLn (); //line feed
               }
            else
               bFirst = FALSE;

            ItiStrCpy(szCurrentID, ppsz[ITM_NUM], sizeof(szCurrentID));

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_6].usDtaCol,
                             LEFT, (acttl[RPT_TTL_6].usDtaColWth | REL_LEFT),
                             ppsz[ITM_NUM]);

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_7].usDtaCol,
                             RIGHT, (acttl[RPT_TTL_7].usDtaColWth | REL_LEFT),
                             ppsz[ITM_JOB_QTY]);

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_8].usDtaCol,
                             LEFT, (acttl[RPT_TTL_8].usDtaColWth | REL_LEFT),
                             ppsz[ITM_UNIT]);

            ItiPrtPutTextAt (CURRENT_LN+1, acttl[RPT_TTL_13].usDtaCol,
                             LEFT, (acttl[RPT_TTL_13].usDtaColWth | REL_LEFT),
                             ppsz[ITM_DESC]);
            }


         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_9].usDtaCol,
                          LEFT, (acttl[RPT_TTL_9].usDtaColWth | REL_LEFT),
                          ppsz[ITM_PCN]);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_10].usDtaCol,
                          LEFT, (acttl[RPT_TTL_10].usDtaColWth | REL_LEFT),
                          ppsz[ITM_CAT]);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_11].usDtaCol,
                          LEFT, (acttl[RPT_TTL_11].usDtaColWth | REL_LEFT),
                          ppsz[ITM_SEQ]);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_12].usDtaCol,
//                          RIGHT, (acttl[RPT_TTL_12].usDtaColWth | REL_LEFT),
                          RIGHT, USE_RT_EDGE,
                          ppsz[ITM_PCN_QTY]);

         ItiPrtIncCurrentLn (); //line feed

         ItiFreeStrArray (hhprPESjo, ppsz, usNumCols); /* ITM */

         }/* end of while */

      }/* else clause */
   


   /* -- Finishout this section. */
   ItiPrtIncCurrentLn (); 
   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   ItiPrtIncCurrentLn (); //line feed

   }/* End of Function ItemsSection */






USHORT PrintProc (HWND hwnd, MPARAM mp1, MPARAM mp2)
   {
   USHORT us    = 0;
   BOOL   bChecked = FALSE;
//   HFILE  hAltFile;
   HHEAP  hhprLocal = NULL;
   CHAR   szKey        [SMARRAY] = "";
   CHAR   szOptSwStr   [SMARRAY] = "";
   CHAR   szCmdLn      [2*SMARRAY +1] = "";
//   CHAR   szKeyFileName[CCHMAXPATH + 1] = "";



   /* -- reset the command line. */
   ItiStrCpy (szCmdLn, RPT_DLL_NAME "  ", sizeof(szCmdLn) );


   /* -- Get the job key. */
   WinQueryDlgItemText (hwnd, DID_KEY, sizeof szKey, szKey);

   if (ItiStrLen(szKey) == 0)
      {
      ItiStrCpy (szErrorMsg,
                 "ERROR: No KEY returned from ",
                 sizeof szErrorMsg);

      ItiStrCat (szErrorMsg, TITLE, sizeof szErrorMsg);
      ItiStrCat (szErrorMsg, " dialog box query.", sizeof szErrorMsg);

      ItiRptUtErrorMsgBox (hwnd, szErrorMsg);
      return (ITIRPTUT_ERROR);
      }
   else
      {
      ItiStrCat (szCmdLn, szKey, sizeof szCmdLn);
      }



//   bChecked = (USHORT) (ULONG) WinSendDlgItemMsg (hwnd, 
//                               DID_SELECTED, BM_QUERYCHECK, 0, 0);
//   if (bChecked)
//      {
//      /* -- Build cmdln string to do those selected. */
//      us = ItiRptUtBuildSelectedKeysFile(hwnd,
//                             Job_Cat_List,
//                             cJobKey,
//                             szKeyFileName, sizeof szKeyFileName);
//
//      if ((us == ITIRPTUT_PARM_ERROR)|| (us == ITIRPTUT_ERROR))
//         ItiRptUtErrorMsgBox (hwnd,
//                "ERROR: Failed to get selected keys in "__FILE__);
//      else
//         {/* -- Prep the CmdLn to use the key file. */
//         ItiStrCat (szCmdLn, ITIRPT_OPTSW_KEYFILE, sizeof szCmdLn);
//         ItiStrCat (szCmdLn, szKeyFileName, sizeof szCmdLn);
//         }/* end of if ((us... */
//
//      }/* end of if (bChecked... then clause */
//   else
//      {/* Do ALL of the keys. */
//      hhprLocal = ItiMemCreateHeap (MIN_HEAP_SIZE);
//
//      ItiRptUtUniqueFile (szKeyFileName, sizeof szKeyFileName, TRUE);
//
//      /* -- Build the cost sheet key list query. */
//      ItiMbQueryQueryText(hmodrPESjo, ITIRID_RPT, ID_QUERY_KEYS,
//                          szQueryKeys, sizeof szQueryKeys );
//
//
//      /* -- Do the database query. */
//      hqry = ItiDbExecQuery (szQueryKeys, hhprLocal, hmodrPESjo, ITIRID_RPT,
//                             ID_QUERY_KEYS, &usNumCols, &usErr);
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
//         ItiRptUtStartKeyFile (&hAltFile, szKeyFileName, sizeof szKeyFileName);
//
//         while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )
//            {
//            ItiRptUtWriteToKeyFile (hAltFile, ppsz[0], NULL);
//            ItiFreeStrArray (hhprLocal, ppsz, usNumCols);
//            }/* End of while... */
//
//         ItiRptUtEndKeyFile (hAltFile);
//
//         ItiStrCat (szCmdLn, ITIRPT_OPTSW_KEYFILE, sizeof szCmdLn);
//         ItiStrCat (szCmdLn, szKeyFileName, sizeof szCmdLn);
//         }/* else clause */
//
//      }/* end of if (bChecked... else clause */


   us = ItiRptUtExecRptCmdLn (hwnd, szCmdLn, RPT_RPESJO_SESSION);

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
   ItiStrCpy (pszBuffer, "Job/Project Listing", usMaxSize);   
   *pusWindowID = rPESjo_RptDlgBox;                       
   }/* END OF FUNCTION */





BOOL EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID, 
                                     USHORT usActiveWindowID) 
   {                                                          
   if ((usTargetWindowID == rPESjo_RptDlgBox)                     
       && ((usActiveWindowID == JobS)
           || (usActiveWindowID == ProjectsForJobS)
           || (usActiveWindowID == ProjectsForJobL))  )
      return TRUE;                                            
   else                                                       
      return FALSE;                                           
   }/* END OF FUNCTION */





MRESULT EXPORT ItiDllrPESjoRptDlgProc (HWND     hwnd,     
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
         us = ItiRptUtInitDlgBox (hwnd, RPT_RPESJO_SESSION, 0, NULL);
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
   hhprPESjo = ItiMemCreateHeap (MIN_HEAP_SIZE);


   /* ------------------------------------------------------------------- *\
    * Initialize the strings.                                             *
   \* ------------------------------------------------------------------- */
//   ItiStrCpy(szCurrentID,"TABLE_ID",sizeof(szCurrentID));

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
   ItiRptUtLoadLabels (hmodrPESjo, &usNumOfTitles, &acttl[0]);

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


   ItiMbQueryQueryText(hmodrPESjo, ITIRID_RPT, ID_QUERY_KEYS,
                       szQueryKeys, sizeof szQueryKeys );

   ItiMbQueryQueryText(hmodrPESjo, ITIRID_RPT, ID_QUERY_LISTING,
                       szQueryListing, sizeof szQueryListing );

   ItiMbQueryQueryText(hmodrPESjo, ITIRID_RPT, ID_QUERY_PCN,
                       szQueryPCN, sizeof szQueryPCN );

   ItiMbQueryQueryText(hmodrPESjo, ITIRID_RPT, ID_QUERY_CAT,
                       szQueryCat, sizeof szQueryCat );

   ItiMbQueryQueryText(hmodrPESjo, ITIRID_RPT, ID_QUERY_ITM,
                       szQueryItm, sizeof szQueryItm );

                          

   /* -------------------------------------------------------------------- *\
    * -- For each Key...                                                   *
   \* -------------------------------------------------------------------- */
   ItiRptUtInitKeyList(argcnt, argvars); /*  */
   while (ItiRptUtGetNextKey(pszKey,sizeof(szKey)) != ITIRPTUT_NO_MORE_KEYS)
      {

      SummarySection (pszKey);
      ItemsSection (pszKey);

      }/* End of WHILE GetNextKey */



   /* ------------------------------------------------------------------- *\
    * Finish up and quit.                                                 *
   \* ------------------------------------------------------------------- */
   ItiPrtEndReport();

   ItiMemDestroyHeap (hhprPESjo);


   DBGMSG("Exit ***DLL function ItiRptDLLPrintReport");
   return 0;

   }/* END OF FUNCTION ItiRptDLLPrintReport */





