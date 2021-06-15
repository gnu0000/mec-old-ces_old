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


#include "rFund.h"                                       



/* ======================================================================= *\
 *                                         MANIFEST CONSTANT DEFINITIONS   *
\* ======================================================================= */
#define  PRT_Q_NAME             "Fund Listing"

#define  QUERY_ARRAY_SIZE       1023

#define  SIZE_OF_QUERY_RPT_DESC QUERY_ARRAY_SIZE



/* ======================================================================= *\
 *                                             MODULE'S GLOBAL VARIABLES   *
\* ======================================================================= */
static  CHAR  szCompileDateTime[] = __DATE__ " " __TIME__ ;
static CHAR szDllVersion[] = "1.1a0 rfund.dll";

static  CHAR  szCurrentID   [SMARRAY + 1] = "";
static  CHAR  szOrderingStr [LGARRAY + 1] = "";

static  CHAR  szQueryRptDesc [SIZE_OF_QUERY + 1];

static  CHAR  szBaseQuery   [QUERY_ARRAY_SIZE + 1];
static  CHAR  szQuery       [QUERY_ARRAY_SIZE + 1];
static  CHAR  szWhereConj   [SIZE_OF_CONJ + 1];
static  CHAR  szOrConj      [SIZE_OF_CONJ + 1];
static  CHAR  szKey         [SMARRAY + 1];

static  USHORT  usCurrentPg = 0;


static  CHAR    szFailedDLL [CCHMAXPATH + 1];
static  CHAR    szErrorMsg [1024] = "";

static  USHORT usNumOfTitles;






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
      if (us != usNumOfTitles)
         {
         usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[us].usTtlCol,
                                  LEFT, (acttl[us].usTtlColWth | REL_LEFT),
                                  acttl[us].szTitle);
         }
      else
         {
         ItiPrtIncCurrentLn (); //line feed
         usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[us].usTtlCol,
                                  LEFT, (acttl[us].usTtlColWth | REL_LEFT),
                                  acttl[us].szTitle);
         }

      if (usErr != 0)
         break;
      }
   ItiPrtIncCurrentLn (); //line feed

   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   ItiPrtIncCurrentLn (); //line feed

   return usErr;
   }/* END OF FUNCTION SetColumnTitles */







USHORT WriteRowData (PSZ *ppsz)
   {
   USHORT us   = 0;
   USHORT usLn = 0;
   USHORT usErr;




   usLn = (ItiStrLen(ppsz[DESC]) / acttl[RPT_TTL_3].usDtaColWth) + 2;


   if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
      {
      /* ---------------------------------------------------------------- *\
       * If we are on a new page...                                       *
      \* ---------------------------------------------------------------- */
      if (ItiPrtQueryCurrentPgNum() != usCurrentPg )
         {
         /* -- Set the Header text. */
         ItiPrtChangeHeaderLn(ITIPRT_TITLE, NULL);

         usCurrentPg = ItiPrtQueryCurrentPgNum();
   
         /* -- Set the column titles for new pg.     */
         SetColumnTitles(usNumOfTitles);
         }
      }


   usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_1].usDtaCol,
                         LEFT,
                         (acttl[RPT_TTL_1].usDtaColWth | REL_LEFT),
                         ppsz[ID]);

   usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_2].usDtaCol,
                         LEFT,
                         (acttl[RPT_TTL_2].usDtaColWth | REL_LEFT),
                         ppsz[NAME]);

   ItiPrtIncCurrentLn();

   usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_3].usDtaCol,
                         (LEFT | WRAP | LNINC),
                         (acttl[RPT_TTL_3].usDtaColWth | REL_LEFT),
                         ppsz[DESC]);

   ItiPrtIncCurrentLn();

   return (usErr);
   }/* End of Function WriteRowData */





USHORT ConvertOrderingStr (PSZ pszOrderingStr)
   /* -- Converts OrderingStr from ColIDs into ListingQuery column value. */
   {
   switch (pszOrderingStr[0])
      {
      case 'x':
      case 'X':
      default:
         ItiStrCpy(pszOrderingStr, " order by 1 ", LGARRAY);
         break;
      }/* end switch */

   return (0);
   }/* End of Function ConvertOrderingStr */





USHORT PrintProc (HWND hwnd, MPARAM mp1, MPARAM mp2)
   {
   USHORT us;
   CHAR   szCmdLn  [LGARRAY+1] = RPT_DLL_NAME " ";
   CHAR   szKeyStr [SMARRAY+1] = "";
   CHAR   szFName  [SMARRAY+1] = "";
   BOOL   bChecked;



   bChecked = (USHORT) (ULONG) WinSendDlgItemMsg (hwnd, 
                               DID_SELECTED, BM_QUERYCHECK, 0, 0);
   if (bChecked)
      {
      /* -- Build cmdln string to do selected rows. */

      us = ItiRptUtBuildSelectedKeysFile(hwnd,
                             FundCatL,
                             cFundKey,
                             szFName, sizeof szFName);

      if ((us == ITIRPTUT_PARM_ERROR)|| (us == ITIRPTUT_ERROR))
         ItiRptUtErrorMsgBox (hwnd,
                "ERROR: Failed to get selected keys in "__FILE__);
      else
         {/* -- Prep the CmdLn to use the key file. */
         ItiStrCat (szCmdLn, ITIRPT_OPTSW_KEYFILE, sizeof szCmdLn);
         ItiStrCat (szCmdLn, szFName, sizeof szCmdLn);
         }/* end of if ((us... */

      }/* end of if (bChecked... */



//   /* -- Get the key. */
//   WinQueryDlgItemText (hwnd, DID_KEY,
//                        sizeof szKeyStr, szKeyStr);
//
//   if (ItiStrLen(szKeyStr) == 0)
//      {
//      ItiStrCpy (szErrorMsg,
//                 "ERROR: No KEY returned from ",
//                 sizeof szErrorMsg);
//
//      ItiStrCat (szErrorMsg, TITLE, sizeof szErrorMsg);
//      ItiStrCat (szErrorMsg, " dialog box query.", sizeof szErrorMsg);
//
//      ItiRptUtErrorMsgBox (hwnd, szErrorMsg);
//      }
//
//   ItiStrCat (szCmdLn, szKeyStr, sizeof szCmdLn);


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
   ItiStrCpy (pszBuffer, "~Fund Listing", usMaxSize);   
   *pusWindowID = rFund_RptDlgBox;                       
   }/* END OF FUNCTION */



BOOL EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID, 
                                     USHORT usActiveWindowID) 
   {                                                          
   if ( (usTargetWindowID == rFund_RptDlgBox)
      && ( (usActiveWindowID == FundCatL)
         ||(usActiveWindowID == FundCatS)) )
      return TRUE;                                            
   else                                                       
      return FALSE;                                           
   }/* END OF FUNCTION */



MRESULT EXPORT ItiDllrFundRptDlgProc (HWND     hwnd,     
                                      USHORT   usMessage,
                                      MPARAM   mp1,
                                      MPARAM   mp2)
   {
   USHORT us = 0;


   switch (usMessage)
      {
      case WM_INITDLG:
         us = ItiRptUtInitDlgBox (hwnd, RPT_SESSION, 0, NULL);
         break;


      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               WinSetPointer (HWND_DESKTOP,
                           WinQuerySysPointer (HWND_DESKTOP, SPTR_WAIT, 0));

               us = PrintProc(hwnd, mp1, mp2);

               WinSetPointer (HWND_DESKTOP,
                           WinQuerySysPointer (HWND_DESKTOP, SPTR_ARROW, 0));

               WinDismissDlg (hwnd, TRUE);
               return (0L);



            case DID_CANCEL:
               WinDismissDlg (hwnd, FALSE);
               return (0L);

            case DID_HELP:
               return (0L);

            }/* end of switch(SHORT1... */
         break;


      default:
         us = ItiRptUtChkDlgMsg (hwnd, usMessage, mp1, mp2);
         break;
      }

   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);

   }/* END OF FUNCTION ItiDllrFundRptDlgProc */








USHORT EXPORT ItiRptDLLPrintReport
                 (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[])
   {
   PSZ     pszKey = szKey;

           /* General return code variable: */
   USHORT  us = 0;

           /* Page formatting location variables: */
   USHORT  usLn  = 1;
   USHORT  usCol = 0;
   INT     iBreakLnCnt = 0;      /* line counter */
   BOOL    bTitlesSet  = FALSE;

           /* Used by DbExecQuer & DbGetQueryRow functions: */
   HHEAP   hheap  = NULL;
   HQRY    hqry   = NULL;
   USHORT  usResId   = ITIRID_RPT;
   USHORT  usId      = ID_QUERY_LISTING;
   USHORT  usNumCols = 0;
   USHORT  usErr     = 0;
   PSZ   * ppsz   = NULL;

           /* Title strings for this module. */
   REPTITLES rept;
   PREPT     prept = &rept;



   /* ------------------------------------------------------------------- *\
    *  Create a memory heap.                                              *
   \* ------------------------------------------------------------------- */
   hheap = ItiMemCreateHeap (0);


   /* ------------------------------------------------------------------- *\
    * Initialize the strings.                                             *
   \* ------------------------------------------------------------------- */
   ItiStrCpy(szCurrentID,"TABLE_ID",sizeof(szCurrentID));

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
                       szBaseQuery, sizeof szBaseQuery );

   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_WHERE_CONJUNCTION,
                       szWhereConj, SIZE_OF_CONJ);

   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_WHERE_OR_CONJ,
                       szOrConj, SIZE_OF_CONJ);


   us = 1;
   ItiRptUtInitKeyList(argcnt, argvars); /*  */
   while (ItiRptUtGetNextKey(pszKey, sizeof(szKey)) != ITIRPTUT_NO_MORE_KEYS)
      {
      if ((*pszKey != '\0') && (us == 1)) 
         {/* Do only once */
         ItiStrCat(szBaseQuery, szWhereConj, QUERY_ARRAY_SIZE);
         ItiStrCat(szBaseQuery, pszKey, QUERY_ARRAY_SIZE);
         }

      if ((*pszKey != '\0') && (us != 1))
         {
         ItiStrCat(szBaseQuery, szOrConj, QUERY_ARRAY_SIZE);
         ItiStrCat(szBaseQuery, pszKey, QUERY_ARRAY_SIZE);
         }

      us++;
      }/* End of WHILE GetNextKey */

   ItiStrCpy(szQuery, szBaseQuery, QUERY_ARRAY_SIZE);
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
      }
   else
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )
      {
      /* ---------------------------------------------------------------- *\
       * If we are on a new page...                                       *
      \* ---------------------------------------------------------------- */
      if (ItiPrtQueryCurrentPgNum() != usCurrentPg )
         {
         /* -- Set the Header text. */
         ItiPrtChangeHeaderLn(ITIPRT_TITLE, NULL);

         usCurrentPg = ItiPrtQueryCurrentPgNum();
   
         /* -- Reset the starting spacer line count. */
         iBreakLnCnt = 1;
   
         /* -- Set the column titles for new pg.     */
         SetColumnTitles(usNumOfTitles);
         }

      WriteRowData (ppsz);

      ItiFreeStrArray (hheap, ppsz, usNumCols);

      }/* End of WHILE GetQueryRow for Listing query. */





   /* ------------------------------------------------------------------- *\
    * Finish up and quit.                                                 *
   \* ------------------------------------------------------------------- */
   ItiPrtEndReport();

   DBGMSG("Exit ***DLL function ItiRptDLLPrintReport");
   return 0;

   }/* END OF FUNCTION ItiRptDLLPrintReport */




