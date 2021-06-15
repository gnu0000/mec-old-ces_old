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


/*  CES Crew Catalog Listing Report DLL Source Code
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


#include    "rindlcre.h"


#include "rCrew.h"                                       



/* ======================================================================= *\
 *                                         MANIFEST CONSTANT DEFINITIONS   *
\* ======================================================================= */
#define  BLANK_LINE_INTERVAL    4
#define  PRT_Q_NAME             "Crew List Rpt."

#define  QUERY_ARRAY_SIZE       512


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
static CHAR szDllVersion[] = "1.1a0 rcrew.dll";

static  CHAR  szCurrentID   [SMARRAY + 1] = "";
static  CHAR  szOrderingStr [SMARRAY + 1] = "";

static  CHAR  szKeyFileName [CCHMAXPATH + 1] = "";
static  CHAR    szFailedDLL [CCHMAXPATH + 1] = "";
static  CHAR    szErrorMsg [1024] = "";

static  USHORT usNumOfTitles;
static  USHORT usCurrentPg = 0;


static  CHAR  szBaseQuery   [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szQuery       [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szKey         [SMARRAY + 1] = "";

/* Queries */
static  CHAR  szQueryListing [SIZE_OF_QUERY_LISTING + 1] = "";
static  CHAR  szQueryEqp     [SIZE_OF_QUERY_EQP + 1] = "";
static  CHAR  szQueryLbr     [SIZE_OF_QUERY_LBR + 1] = "";
static  CHAR  szQueryAll     [SIZE_OF_QUERY_ALL + 1] = "";



           /* Used by DbExecQuer & DbGetQueryRow functions: */
static  HHEAP   hhprCrew  = NULL;
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

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_3].usTtlCol,
                    LEFT, (acttl[RPT_TTL_3].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_3].szTitle);

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_4].usTtlCol,
                    LEFT, (acttl[RPT_TTL_4].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_4].szTitle);

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_6].usTtlCol,
                    LEFT, (acttl[RPT_TTL_6].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_6].szTitle);

   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_7].usTtlCol,
                    LEFT, (acttl[RPT_TTL_7].usTtlColWth | REL_LEFT),
                    acttl[RPT_TTL_7].szTitle);


   ItiPrtIncCurrentLn (); //line feed

   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   ItiPrtIncCurrentLn (); //line feed

   return 0;
   }/* END OF FUNCTION SetColumnTitles */






//         {
//         usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[us].usDtaCol,
//                               (LEFT | WRAP | LNINC),
//                               (acttl[us].usDtaColWth | REL_LEFT),
//                               ppsz[us-1]);
//         }



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
   USHORT usLn, us;
   PSZ * ppsz = NULL;


   if ((pszKey == NULL) || (*pszKey == '\0'))
      return 13;

   /* -- Build the query. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryListing, " ", pszKey);


   ppsz = ItiDbGetRow1 (szQuery, hhprCrew, hmodrCrew, ITIRID_RPT,
                        ID_QUERY_LISTING, &usNumCols);

   if (ppsz != NULL)
      {
      usLn = 7 + ItiStrLen(ppsz[DESC]) / acttl[RPT_TTL_2].usDtaColWth;
      if ((ItiPrtKeepNLns(usLn) == PG_WAS_INC)
          || (ItiPrtQueryCurrentPgNum() != usCurrentPg ))
         {
         usCurrentPg = ItiPrtQueryCurrentPgNum();
         ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
         }


      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_1].usDtaCol,
                       LEFT, (acttl[RPT_TTL_1].usDtaColWth | REL_LEFT),
                       ppsz[ID]);
      ItiStrCpy(pszID, ppsz[ID], sizeof szCurrentID);

      us = acttl[RPT_TTL_1].usDtaCol + ItiStrLen(ppsz[ID]);

      ItiPrtPutTextAt (CURRENT_LN, us,
                       LEFT, (acttl[RPT_TTL_2].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_2].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, (us + acttl[RPT_TTL_2].usTtlColWth),
                       (LEFT|WRAP),
                       (4 | FROM_RT_EDGE),
                       //(acttl[RPT_TTL_2].usDtaColWth | REL_LEFT),
                       ppsz[DESC]);
      ItiPrtIncCurrentLn(); //blank line


      ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
      ItiPrtIncCurrentLn (); //line feed

      ItiFreeStrArray (hhprCrew, ppsz, usNumCols);
      }
   else
      return 13;

   return 0;
   }/* End of Function SummarySectionFor */





USHORT SubSectionFor (PSZ pszID, PSZ pszKey)
   {
   USHORT usLnEqp, usLnLbr, usLnStart, usLnEnd, usLnTmp;
   BOOL    bDoEqp,  bDoLbr;
   BOOL   bLabeled = FALSE;

   HQRY    hqryEqp   = NULL;
   USHORT  usNumColsEqp = 0;
   USHORT  usErrEqp     = 0;
   PSZ   * ppszEqp   = NULL;

   HQRY    hqryLbr   = NULL;
   USHORT  usNumColsLbr = 0;
   USHORT  usErrLbr     = 0;
   PSZ   * ppszLbr   = NULL;

   CHAR  szQueryEQUIP [SIZE_OF_QUERY_EQP + 32] = "";
   CHAR  szQueryLABOR [SIZE_OF_QUERY_LBR + 32] = "";

   


   if ((pszKey == NULL) || (*pszKey == '\0') || (pszID == NULL))
      return 13;


   /* -- Build the queries. */
   ItiRptUtPrepQuery(szQueryLABOR, (QUERY_ARRAY_SIZE + 1),
                     szQueryLbr, pszKey, " order by 2 ");

   ItiRptUtPrepQuery(szQueryEQUIP, (QUERY_ARRAY_SIZE + 1),
                     szQueryEqp, pszKey, " order by 2 ");


   /* -- Do the database queries. */
   hqryEqp = ItiDbExecQuery (szQueryEQUIP, hhprCrew, hmodrCrew, ITIRID_RPT,
                          ID_QUERY_EQP, &usNumColsEqp, &usErrEqp);
   if (hqryEqp == NULL)
      {
      ItiStrCpy (szQuery, szQueryEQUIP, sizeof szQuery);
      SETQERRMSG;
      bDoEqp = FALSE;
      }
   else
      bDoEqp = ItiDbGetQueryRow (hqryEqp, &ppszEqp, &usErrEqp);

   
   hqryLbr = ItiDbExecQuery (szQueryLABOR, hhprCrew, hmodrCrew, ITIRID_RPT,
                          ID_QUERY_LBR, &usNumColsLbr, &usErrLbr);
   if (hqryLbr == NULL)
      {
      ItiStrCpy (szQuery, szQueryLABOR, sizeof szQuery);
      SETQERRMSG;
      bDoLbr = FALSE;
      }
   else
      bDoLbr = ItiDbGetQueryRow (hqryLbr, &ppszLbr, &usErrLbr);


   

   /* -- Process the query results for each row. */
   while (bDoEqp || bDoLbr)
      {
      /* -- Determine description lengths. */
      if (bDoEqp)
         {
//        usLnEqp=ItiStrLen(ppszEqp[EQP_DESC])/acttl[RPT_TTL_5].usDtaColWth+2;
         /* -- need to figure out how many lines using word breaking. ^ FIX ^
            -- But for now just add 2 to the above line to cover all cases  */
         usLnEqp = ItiPrtWrappedLnCnt (ppszEqp[EQP_DESC],
                                       acttl[RPT_TTL_5].usDtaColWth, FALSE);
         }
      else
         usLnEqp = 1;

      if (bDoLbr)
         {
//        usLnLbr=ItiStrLen(ppszLbr[LBR_DESC])/acttl[RPT_TTL_8].usDtaColWth+2;
         /* need to figure out how many lines we are using; word breaking. */
         usLnLbr = ItiPrtWrappedLnCnt (ppszLbr[LBR_DESC],
                                       acttl[RPT_TTL_8].usDtaColWth, FALSE);
         }
      else
         usLnLbr = 1;


      usLnStart = ItiPrtQueryCurrentLnNum();
      usLnTmp = (usLnEqp > usLnLbr) ? usLnEqp : usLnLbr;
      usLnEnd = usLnStart + usLnTmp;

      /* -- Check for new page. */
      if ((ItiPrtKeepNLns( usLnEnd - usLnStart + 3 ) == PG_WAS_INC)
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


      /* -- Reset the line values. */
      usLnStart = ItiPrtQueryCurrentLnNum();
      usLnTmp = (usLnEqp > usLnLbr) ? usLnEqp : usLnLbr;
      usLnEnd = usLnStart + usLnTmp;


      /* -- Write out the data. */
      if (bDoEqp)
         {
         ItiPrtPutTextAt(usLnStart, acttl[RPT_TTL_3].usDtaCol,
                         RIGHT, (acttl[RPT_TTL_3].usDtaColWth | REL_LEFT),
                         ppszEqp[EQP_CNT]);

         ItiPrtPutTextAt(usLnStart, acttl[RPT_TTL_4].usDtaCol,
                         LEFT, (acttl[RPT_TTL_4].usDtaColWth | REL_LEFT),
                         ppszEqp[EQP_ID]);

         ItiPrtPutTextAt(usLnStart, acttl[RPT_TTL_5].usDtaCol,
                         (LEFT|WRAP),
                         (acttl[RPT_TTL_5].usDtaColWth | REL_LEFT),
                         ppszEqp[EQP_DESC]);

         ItiFreeStrArray (hhprCrew, ppszEqp, usNumColsEqp);
         bDoEqp = ItiDbGetQueryRow (hqryEqp, &ppszEqp, &usErrEqp);
         }


      if (bDoLbr)
         {
         ItiPrtMoveTo(usLnStart, acttl[RPT_TTL_6].usDtaCol);

         ItiPrtPutTextAt (usLnStart, acttl[RPT_TTL_6].usDtaCol,
                    RIGHT, (acttl[RPT_TTL_6].usDtaColWth | REL_LEFT),
                    ppszLbr[LBR_CNT]);

         ItiPrtPutTextAt (usLnStart, acttl[RPT_TTL_7].usDtaCol,
                    LEFT, (acttl[RPT_TTL_7].usDtaColWth | REL_LEFT),
                    ppszLbr[LBR_ID]);

         ItiPrtPutTextAt (usLnStart, acttl[RPT_TTL_8].usDtaCol,
                       (LEFT|WRAP),
                       (acttl[RPT_TTL_8].usDtaColWth | REL_LEFT),
                       ppszLbr[LBR_DESC]);

         ItiFreeStrArray (hhprCrew, ppszLbr, usNumColsLbr);
         bDoLbr = ItiDbGetQueryRow (hqryLbr, &ppszLbr, &usErrLbr);
         }

      ItiPrtMoveTo(usLnEnd, 0);


      }/* End of WHILE (bDo... */

   


   /* -- Finishout this section. */
//   ItiPrtIncCurrentLn (); //blank line 
   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   ItiPrtIncCurrentLn (); //line feed

   }/* End of Function SubSectionFor */






USHORT PrintProc (HWND hwnd, MPARAM mp1, MPARAM mp2)
   {
   USHORT us    = 0;
   CHAR   szKey      [SMARRAY] = "";
   CHAR   szOptSwStr [SMARRAY] = "";
   CHAR   szCmdLn [2*SMARRAY +1] = "";
   BOOL   bChecked = FALSE;
   HFILE  hAltFile;
   HHEAP  hhprLocal = NULL;



   /* -- reset the command line. */
   ItiStrCpy (szCmdLn, RPT_DLL_NAME, sizeof(szCmdLn) );


   bChecked = (USHORT) (ULONG) WinSendDlgItemMsg (hwnd, 
                               DID_SELECTED, BM_QUERYCHECK, 0, 0);
   if (bChecked)
      {
      /* -- Build cmdln string to do selected crews. */
      us = ItiRptUtBuildSelectedKeysFile(hwnd,
                             CrewCatL,
                             cCrewKey,
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
      {/* Do ALL of the crews. */
      hhprLocal = ItiMemCreateHeap (MIN_HEAP_SIZE);

      ItiRptUtUniqueFile (szKeyFileName, sizeof szKeyFileName, TRUE);

      /* -- Build the cost sheet key list query. */
      ItiMbQueryQueryText(hmodrCrew, ITIRID_RPT, ID_QUERY_ALL,
                          szQueryAll, sizeof szQueryAll );


      /* -- Do the database query. */
      hqry = ItiDbExecQuery (szQueryAll, hhprLocal, hmodrCrew, ITIRID_RPT,
                             ID_QUERY_ALL, &usNumCols, &usErr);
      
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
            ItiRptUtWriteToKeyFile (hAltFile, ppsz[CREW_KEY], NULL);
            ItiFreeStrArray (hhprLocal, ppsz, usNumCols);
            }/* End of while... */

         ItiRptUtEndKeyFile (hAltFile);

         ItiStrCat (szCmdLn, ITIRPT_OPTSW_KEYFILE, sizeof szCmdLn);
         ItiStrCat (szCmdLn, szKeyFileName, sizeof szCmdLn);
         }/* else clause */

      }/* end of else clause */


   us = ItiRptUtExecRptCmdLn (hwnd, szCmdLn, RPT_CREW_SESSION);

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
   ItiStrCpy (pszBuffer, "Crew Catalog Listing", usMaxSize);   
   *pusWindowID = rCrew_RptDlgBox;                       
   }/* END OF FUNCTION */



BOOL EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID, 
                                     USHORT usActiveWindowID) 
   {                                                          
   if ((usTargetWindowID == rCrew_RptDlgBox)                     
       && (usActiveWindowID == CrewCatS))                  
      return TRUE;                                            
   else                                                       
      return FALSE;                                           
   }/* END OF FUNCTION */




MRESULT EXPORT ItiDllrCrewRptDlgProc (HWND     hwnd,     
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
         us = ItiRptUtInitDlgBox (hwnd, RPT_CREW_SESSION, 0, NULL);
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
   hhprCrew = ItiMemCreateHeap (MIN_HEAP_SIZE);


   /* ------------------------------------------------------------------- *\
    * Initialize the strings.                                             *
   \* ------------------------------------------------------------------- */
   ItiStrCpy(szCurrentID,"TABLE_ID",sizeof(szCurrentID));

   /* -- Get any query ordering information, BEFORE setting rpt titles. */
//   ItiRptUtGetOption (ITIRPTUT_ORDERING_SWITCH, szOrderingStr, LGARRAY,
//                      argcnt, argvars);
//   ConvertOrderingStr (szOrderingStr);


//   ItiRptUtGetOption (OPT_SW_INCL_XYZ_RPT, szSuppRpt, sizeof szSuppRpt,
//                      argcnt, argvars);
//   if (szSuppRpt[0] == '\0') 
//      bPrtRpt = FALSE;
//   else
//      bPrtRpt = TRUE;



   /* -- Get the report's titles for this DLL. */
   ItiRptUtLoadLabels (hmodrCrew, &usNumOfTitles, &acttl[0]);

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




   ItiMbQueryQueryText(hmodrCrew, ITIRID_RPT, ID_QUERY_LISTING,
                       szQueryListing, sizeof szQueryListing );


   ItiMbQueryQueryText(hmodrCrew, ITIRID_RPT, ID_QUERY_EQP,
                       szQueryEqp, sizeof szQueryEqp );

   ItiMbQueryQueryText(hmodrCrew, ITIRID_RPT, ID_QUERY_LBR,
                       szQueryLbr, sizeof szQueryLbr );

   ItiMbQueryQueryText(hmodrCrew, ITIRID_RPT, ID_QUERY_ALL,
                       szQueryAll, sizeof szQueryAll );




   /* -------------------------------------------------------------------- *\
    * -- For each Key...                                                   *
   \* -------------------------------------------------------------------- */
   ItiRptUtInitKeyList(argcnt, argvars); /*  */
   while (ItiRptUtGetNextKey(pszKey,sizeof(szKey)) != ITIRPTUT_NO_MORE_KEYS)
      {
      us = SummarySectionFor(pszKey, szCurrentID);

      if (us == 0)
         {
         SubSectionFor(szCurrentID, pszKey);
         }


      }/* End of WHILE GetNextKey */



   /* ------------------------------------------------------------------- *\
    * Finish up and quit.                                                 *
   \* ------------------------------------------------------------------- */
   ItiPrtEndReport();

   DBGMSG("Exit ***DLL function ItiRptDLLPrintReport");
   return 0;

   }/* END OF FUNCTION ItiRptDLLPrintReport */





