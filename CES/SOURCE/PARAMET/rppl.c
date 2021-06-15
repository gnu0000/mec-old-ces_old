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


#include    "rindlPPL.h"


#include "rPPL.h"                                       



/* ======================================================================= *\
 *                                         MANIFEST CONSTANT DEFINITIONS   *
\* ======================================================================= */
#define  BLANK_LINE_INTERVAL    4
#define  PRT_Q_NAME             "Paramet Prof"

#define  QUERY_ARRAY_SIZE       1023

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
static CHAR szDllVersion[] = "1.1a0 rppl.dll";

static  CHAR  szCurrentID   [SMARRAY + 1] = "";
static  CHAR  szOrderingStr [SMARRAY + 1] = "";

static  CHAR  szFailedDLL [CCHMAXPATH + 1] = "";
static  CHAR  szErrorMsg  [1024] = "";

static  USHORT usNumOfTitles;
static  USHORT usCurrentPg = 0;

static  CHAR  szBaseQuery   [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szQuery       [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szKey         [SMARRAY + 1] = "";

static  CHAR  szQueryKeys     [SIZE_OF_QUERY_KEYS + 1] = "";
static  CHAR  szQueryListing  [SIZE_OF_QUERY_LISTING + 1] = "";
static  CHAR  szQueryMjrItems [SIZE_OF_QUERY_MJR_ITEMS + 1] = "";
static  CHAR  szQueryTtl      [SIZE_OF_QUERY_TTL + 1] = "";


           /* Used by DbExecQuer & DbGetQueryRow functions: */
static  HHEAP   hheap  = NULL;
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







USHORT SetColumnTitles (void)
   /* -- Print the column titles. */
   {
   USHORT us;

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_2].usTtlCol,
                    LEFT, (acttl[RPT_TTL_2].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_2].szTitle);
   ItiPrtDrawSeparatorLn (CURRENT_LN +1,
                          acttl[RPT_TTL_3].usTtlCol,
                          (acttl[RPT_TTL_3].usTtlColWth | REL_LEFT));

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_4].usTtlCol,
                    LEFT, (acttl[RPT_TTL_4].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_4].szTitle);
   ItiPrtDrawSeparatorLn (CURRENT_LN +1,
                          acttl[RPT_TTL_4].usDtaCol,
                          (acttl[RPT_TTL_4].usDtaColWth | REL_LEFT));

   ItiPrtIncCurrentLn (); //line feed
   us = ItiPrtIncCurrentLn (); //line feed

   return us;
   }/* END OF FUNCTION SetColumnTitles */






USHORT ConvertOrderingStr (PSZ pszOrderingStr)
   /* -- Converts OrderingStr from ColIDs into ListingQuery column value. */
   {
   switch (pszOrderingStr[0])
      {
      case 'p':
      case 'P':
         ItiStrCat(pszOrderingStr, " order by 3 ", sizeof szOrderingStr);
         break;

      default:
         ItiStrCat(pszOrderingStr, " order by 1 ", sizeof szOrderingStr);
         break;
      }/* end switch */

   return (0);
   }/* End of Function ConvertOrderingStr */








USHORT SummarySectionFor (PSZ pszKey, PSZ pszCurrentID)
   {
   USHORT usLn, us;
   PSZ * ppsz = NULL;


   if ((pszKey == NULL) || (*pszKey == '\0'))
      return 13;

   /* -- Build the query. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryListing, " ", pszKey);


   ppsz = ItiDbGetRow1 (szQuery, hheap, hmodrPPL, ITIRID_RPT,
                        ID_QUERY_LISTING, &usNumCols);

   if (ppsz != NULL)
      {
      usLn = 10 + ItiStrLen(ppsz[LISTING_DESC]) / 50;
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
                       ppsz[LISTING_ID]);
      ItiStrCpy(pszCurrentID, ppsz[LISTING_ID], sizeof szCurrentID);

//      us = 2 + acttl[RPT_TTL_1].usDtaCol + ItiStrLen(ppsz[LISTING_ID]);

      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 2,
                       (LEFT|WRAP),
                       (4 | FROM_RT_EDGE),
                       //(acttl[RPT_TTL_2].usDtaColWth | REL_LEFT),
                       ppsz[LISTING_DESC]);
      us = ItiPrtIncCurrentLn();

      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }
   else
      {
      SETQERRMSG(szQuery);
      return 13;
      }

   return 0;
   }/* End of Function SummarySectionFor */





USHORT SubSectionFor (PSZ pszCurrentID, PSZ pszKey)
   {
   USHORT usLn;
   BOOL   bNewID = TRUE;
   BOOL   bLabeled   = FALSE;
   CHAR   szCurrentID[32] = "";


   if ((pszKey == NULL) || (*pszKey == '\0') || (pszCurrentID == NULL))
      return 13;

   /* -- Build the query. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryMjrItems, pszKey, szOrderingStr);


   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hheap, hmodrPPL, ITIRID_RPT,
                          ID_QUERY_MJR_ITEMS, &usNumCols, &usErr);
   
   /* -- Process the query results for each row. */
   if (hqry == NULL)
      {
      SETQERRMSG(szQuery);
      }
   else
      {
      bLabeled = FALSE;
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
         {
         /* -- Check for new page. */
         usLn = 6 + ItiStrLen(ppsz[MJR_ITEMS_DESC])/acttl[RPT_TTL_3].usDtaColWth;
         if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
            {
            usCurrentPg = ItiPrtQueryCurrentPgNum();
            ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");

            ItiPrtPutTextAt (CURRENT_LN, 0,
                    LEFT, (9 | REL_LEFT),
                    "(cont.) ");

            ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                    LEFT, (acttl[RPT_TTL_1].usDtaColWth | REL_LEFT),
                    pszCurrentID);
            ItiPrtIncCurrentLn (); //line feed


            SetColumnTitles();
            bLabeled = TRUE;
            }


         if (!bLabeled)
            {
            SetColumnTitles();
            bLabeled = TRUE;
            }



         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_2].usDtaCol,
                       LEFT,
                       (acttl[RPT_TTL_2].usDtaColWth | REL_LEFT),
                       ppsz[MJR_ITEMS_ID]);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_4].usDtaCol,
                       RIGHT,
                       (acttl[RPT_TTL_4].usDtaColWth | REL_LEFT),
                       ppsz[MJR_ITEMS_PCT]);


         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_3].usDtaCol,
                       (LEFT|WRAP),
                       (acttl[RPT_TTL_3].usDtaColWth | REL_LEFT),
                       ppsz[MJR_ITEMS_DESC]);

//         ItiPrtIncCurrentLn (); //line feed

         ItiFreeStrArray (hheap, ppsz, usNumCols);
         }/* End of WHILE GetQueryRow */

      }/* else clause */
   


   /* -- Build the totals query. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryTtl, " ", pszKey);

   ppsz = ItiDbGetRow1 (szQuery, hheap, hmodrPPL, ITIRID_RPT,
                        ID_QUERY_TTL, &usNumCols);
   if (ppsz != NULL)
      {
      if (ItiPrtKeepNLns(2) == PG_WAS_INC)
         {
         usCurrentPg = ItiPrtQueryCurrentPgNum();
         ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");

         ItiPrtPutTextAt (CURRENT_LN, 0,
                 LEFT, (9 | REL_LEFT),
                 "(cont.) ");

         ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1,
                 LEFT, (acttl[RPT_TTL_1].usDtaColWth | REL_LEFT),
                 pszCurrentID);
         ItiPrtIncCurrentLn (); //line feed

         SetColumnTitles();
         bLabeled = TRUE;
         }


      ItiPrtDrawSeparatorLn (CURRENT_LN,
                             acttl[RPT_TTL_5].usDtaCol,
                             (acttl[RPT_TTL_5].usDtaColWth | REL_LEFT));
      ItiPrtIncCurrentLn (); //line feed

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_5].usTtlCol,
                       LEFT, (acttl[RPT_TTL_5].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_5].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_5].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_5].usDtaColWth | REL_LEFT),
                       ppsz[TOTAL]);
      ItiPrtIncCurrentLn (); //line feed
      ItiPrtIncCurrentLn (); //blank line
      }
   else
      {
      SETQERRMSG("JE: PEMETH Averages SigMjrItem Total query result was empty.");
      SETQERRMSG(szQuery);
      }


   /* -- Finishout this section. */
//   ItiPrtIncCurrentLn (); //blank line 
//   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
//   ItiPrtIncCurrentLn (); //line feed

   }/* End of Function SubSectionFor */






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


   bChecked = (USHORT) (ULONG) WinSendDlgItemMsg (hwnd, 
                               DID_SELECTED, BM_QUERYCHECK, 0, 0);
   if (bChecked)
      {
      /* -- Build cmdln string to do those selected. */
      us = ItiRptUtBuildSelectedKeysFile(hwnd,
                             ParametricProfileCatL,
                             cProfileKey,
                             szKeyFileName, sizeof szKeyFileName);

      if ((us == ITIRPTUT_PARM_ERROR)|| (us == ITIRPTUT_ERROR))
         {
         WinSendDlgItemMsg(hwnd, DID_SELECTED, BM_SETCHECK, UNCHECKED, 0L);
         ItiRptUtErrorMsgBox (hwnd,
                "ERROR: Failed to get selected keys in "__FILE__);
         }
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
      ItiMbQueryQueryText(hmodrPPL, ITIRID_RPT, ID_QUERY_KEYS,
                          szQueryKeys, sizeof szQueryKeys );


      /* -- Do the database query. */
      hqry = ItiDbExecQuery (szQueryKeys, hhprLocal, hmodrPPL, ITIRID_RPT,
                             ID_QUERY_KEYS, &usNumCols, &usErr);
      
      /* ---------------------------------------------------------------- *\
       * Process the query results for each row.                          *
      \* ---------------------------------------------------------------- */
      if (hqry == NULL)
         {
         SETQERRMSG(szQueryKeys);
         }                     
      else
         {
         ItiRptUtStartKeyFile (&hAltFile, szKeyFileName, sizeof szKeyFileName);

         while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )
            {
            ItiRptUtWriteToKeyFile (hAltFile, ppsz[KEY], NULL);
            ItiFreeStrArray (hhprLocal, ppsz, usNumCols);
            }/* End of while... */

         ItiRptUtEndKeyFile (hAltFile);

         ItiStrCat (szCmdLn, ITIRPT_OPTSW_KEYFILE, sizeof szCmdLn);
         ItiStrCat (szCmdLn, szKeyFileName, sizeof szCmdLn);
         }/* else clause */

      }/* end of if (bChecked... else clause */


   us = ItiRptUtExecRptCmdLn (hwnd, szCmdLn, RPT_RPPL_SESSION);

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
   ItiStrCpy (pszBuffer, "Parametric Profile Listing", usMaxSize);   
   *pusWindowID = rPPL_RptDlgBox;                       
   }/* END OF FUNCTION */



BOOL EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID, 
                                     USHORT usActiveWindowID) 
   {                                          
   if ((usTargetWindowID == rPPL_RptDlgBox)     
       && (usActiveWindowID == ParametricProfileCatS))  
      return TRUE;   
   else              
      return FALSE;  
   }/* END OF FUNCTION */



MRESULT EXPORT ItiDllrPPLRptDlgProc (HWND   hwnd,  
                                     USHORT usMessage,
                                     MPARAM mp1,
                                     MPARAM mp2)
   {
   USHORT us = 0;
   CHAR   szCmdLn  [LGARRAY+1] = RPT_DLL_NAME " ";
   CHAR   szKeyStr [SMARRAY+1] = "";

   szKeyStr[0] = '\0';

   switch (usMessage)
      {
      case WM_INITDLG:
         us = ItiRptUtInitDlgBox (hwnd, RPT_RPPL_SESSION, 0, NULL);
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
   hheap = ItiMemCreateHeap (MIN_HEAP_SIZE);


   /* ------------------------------------------------------------------- *\
    * Initialize the strings.                                             *
   \* ------------------------------------------------------------------- */
   ItiStrCpy(szCurrentID,"TABLE_ID",sizeof(szCurrentID));

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
   ItiRptUtLoadLabels (hmodrPPL, &usNumOfTitles, &acttl[0]);

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



   ItiMbQueryQueryText(hmodrPPL, ITIRID_RPT, ID_QUERY_KEYS,
                       szQueryKeys, sizeof szQueryKeys );

   ItiMbQueryQueryText(hmodrPPL, ITIRID_RPT, ID_QUERY_LISTING,
                       szQueryListing, sizeof szQueryListing );

   ItiMbQueryQueryText(hmodrPPL, ITIRID_RPT, ID_QUERY_MJR_ITEMS,
                       szQueryMjrItems, sizeof szQueryMjrItems );

   ItiMbQueryQueryText(hmodrPPL, ITIRID_RPT, ID_QUERY_TTL,
                       szQueryTtl, sizeof szQueryTtl );


   /* -------------------------------------------------------------------- *\
    * -- For each ProfileKey...                                            *
   \* -------------------------------------------------------------------- */
   ItiRptUtInitKeyList(argcnt, argvars); /*  */
   while (ItiRptUtGetNextKey(pszKey,sizeof(szKey)) != ITIRPTUT_NO_MORE_KEYS)
      {
      us = SummarySectionFor(pszKey, szCurrentID);

      if (us != ITIRPTUT_ERROR)
         us = SubSectionFor(szCurrentID, pszKey);

      }/* End of WHILE GetNextKey */



   /* ------------------------------------------------------------------- *\
    * Finish up and quit.                                                 *
   \* ------------------------------------------------------------------- */
   ItiPrtEndReport();

   DBGMSG("Exit ***DLL function ItiRptDLLPrintReport");
   return 0;

   }/* END OF FUNCTION ItiRptDLLPrintReport */





