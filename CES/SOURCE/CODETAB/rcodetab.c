/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1992 by Info Tech, Inc.  All Rights Reserved.         |
|                                                                           |
|       This program module is part of DS/Shell (PC), Info Tech's           |
|       database interfaces for OS/2, a proprietary product of              |
|       Info Tech, Inc., no part of which may be reproduced or              |
|       transmitted in any form or by any means, electronic,                |
|       mechanical, or otherwise, including photocopying and recording      |
|       or in connection with any information storage or retrieval          |
|       system, without permission in writing from Info Tech, Inc.          |
|                                                                           |
+--------------------------------------------------------------------------*/


#define     INCL_BASE
#define     INCL_DOS
#define     INCL_DEV
#define     INCL_GPI
#define     INCL_WIN
#include    "..\include\iti.h"     


#include    "..\include\itibase.h"
#include    "..\include\itiutil.h"
#include    "..\include\itierror.h"     
#include    "..\include\itidbase.h"
#include    "..\include\itimbase.h"
#include    "..\include\itifmt.h"
#include    "..\include\colid.h"
#include    "..\include\winid.h"
#include    "..\include\itiwin.h"

#include    "..\include\itirpt.h"
#include    "..\include\itirptdg.h"
#include    "rcodetab.h"
#include    "dialog.h"
#include    "rinidll.h"


/*
 * The following are support modules that are always used by a report DLL.
 */
#if !defined (INCL_ITIPRT)
#include "..\include\itiprt.h"
#endif

#if !defined (INCL_RPTUTIL)
#include "..\include\itirptut.h"
#endif



/* ======================================================================= *\
 *                                         MANIFEST CONSTANT DEFINITIONS   *
\* ======================================================================= */
#define  BLANK_LINE_INTERVAL    5

#define  PRT_Q_NAME             "Code Tables"

#define  DLL_NAME_PARM          1
#define  CRITERIA_PARM          2
#define  QUERY_ARRAY_SIZE       (2 * SIZE_OF_QUERY)




/* ======================================================================= *\
 *                                             MODULE'S GLOBAL VARIABLES   *
\* ======================================================================= */
static  CHAR  szCompileDateTime[] = __DATE__ " " __TIME__ ;
static  CHAR  szDllVersion[] = "1.1a0 rCodeTab.dll";

static  CHAR  szCurrentID [SMARRAY];

static  CHAR  szBaseQuery [SIZE_OF_QUERY + 1];
static  CHAR  szQuery     [QUERY_ARRAY_SIZE + 1];
static  CHAR  szWhereConj [SIZE_OF_CONJ + 1];
static  CHAR  szKey       [SMARRAY + 1];


static  CHAR szFailedDLL [CCHMAXPATH + 1];
static  CHAR szErrorMsg  [1024] = "";
               
static  BOOL bColTitlesSet;
 


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
   }




USHORT SetColumnTitles (BOOL bUseUnderline)
   {
   ItiPrtPutTextAt (0, 15, LEFT, 23, "Code");
   ItiPrtPutTextAt (0, 26, (LEFT | LNINC), 66, "Translation");

   if (bUseUnderline == TRUE)  
      {
      ItiPrtDrawSeparatorLn (0, 15, 19);
      ItiPrtDrawSeparatorLn (0, 26, 37);
      ItiPrtIncCurrentLn ();
      }

   bColTitlesSet = TRUE;

   return (0);
   }/* END OF FUNCTION SetColumnTitles */



USHORT PrintProc (HWND hwnd, MPARAM mp1, MPARAM mp2, PSZ pszCmdLn, USHORT usLen)
   {
   USHORT us    = 0;
   BOOL   bChecked;
   CHAR   szKeyStr   [SMARRAY+1] = "";
   CHAR   szOptSwStr [SMARRAY+1] = "";
   CHAR   szFName    [SMARRAY+1] = "";



   bChecked = (USHORT) (ULONG) WinSendDlgItemMsg (hwnd, 
                               DID_SELECTED, BM_QUERYCHECK, 0L, 0L);
   if (bChecked)
      {
      /* -- Build cmdln string to do selected rows. */
      us = ItiRptUtBuildSelectedKeysFile(hwnd,
                                         CodeTableL,
                                         cCodeTableKey,
                                         szFName, sizeof szFName);

      if ((us == ITIRPTUT_PARM_ERROR) || (us == ITIRPTUT_ERROR))
         {
         WinSendDlgItemMsg(hwnd, DID_SELECTED, BM_SETCHECK, UNCHECKED, 0L);
         return (ITIRPTUT_PARM_ERROR);
         }
      else
         {/* -- Prep the CmdLn to use the key file. */
         ItiStrCat (pszCmdLn, ITIRPT_OPTSW_KEYFILE, usLen);
         ItiStrCat (pszCmdLn, szFName, usLen);
         }/* end of if ((us... */

      }/* end of if (bChecked... */


   us = ItiRptUtExecRptCmdLn (hwnd, pszCmdLn, RCODETAB_SESSION);

   return (us);

   }/* End of PrintProc Function */









/* ======================================================================= *\
 *                                                    EXPORTED FUNCTIONS   *
\* ======================================================================= */
/* -- ItiDllQueryVersion returns the version number of this DLL. */
USHORT EXPORT ItiDllQueryVersion (VOID)
   {
   return (CODETABL_DLL_VERSION);
   }


/*
 * ItiDllQueryCompatibility returns TRUE if this DLL is compatable
 * with the caller's version numer, otherwise FALSE is returned.
 */

BOOL EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion)
   {
   if ( ItiVerCmp(szDllVersion) )
      return (TRUE);
   return (FALSE);
   }


VOID EXPORT ItiDllQueryMenuName (PSZ     pszBuffer,
                                 USHORT  usMaxSize,
                                 PUSHORT pusWindowID)
   {
   ItiStrCpy (pszBuffer, "~Code Table Listing", usMaxSize);
   *pusWindowID = rCodeTab_RptDlgBox;
   }




BOOL EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID,
                                     USHORT usActiveWindowID)
   {
   if ((usTargetWindowID == rCodeTab_RptDlgBox)
      && ((usActiveWindowID == CodeTableL)
         ||(usActiveWindowID == CodeTableS)   ))
      return (TRUE);
   else
      return (FALSE);
   }








MRESULT EXPORT ItiDllCodeTabRptDlgProc (HWND     hwnd,
                                        USHORT   usMessage,
                                        MPARAM   mp1,
                                        MPARAM   mp2)
   {
   USHORT us = 0;
   CHAR   szCmdLn [LGARRAY+1] = RPT_DLL_NAME " ";


   switch (usMessage)
      {
      case WM_INITDLG:
         us = ItiRptUtInitDlgBox (hwnd, RCODETAB_SESSION, 0, NULL);
         break;


      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_ALL:
               /* -- Print all is the default so no command line options
                *    nor any key values need to be appended.
                */
               break;

            case DID_SELECTED:
               break;

            case DID_PRINT:
               WinSetPointer (HWND_DESKTOP,
                           WinQuerySysPointer (HWND_DESKTOP, SPTR_WAIT, 0));

               us = PrintProc(hwnd, mp1, mp2, szCmdLn, sizeof szCmdLn);

               WinSetPointer (HWND_DESKTOP,
                           WinQuerySysPointer (HWND_DESKTOP, SPTR_ARROW, 0));

               if (us == 0)
                  WinDismissDlg (hwnd, TRUE);

//               return (0L);
               break;


            case DID_CANCEL:
               WinDismissDlg (hwnd, FALSE);
//               return 0;
               break;

            }/* end of switch(SHORT1... */

         break;


      default:
         us = ItiRptUtChkDlgMsg (hwnd, usMessage, mp1, mp2);
         break;

      }/* end of switch (usMessage */

   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);

   }/* END OF FUNCTION ItiDllCodeTabRptDlgProc */













USHORT EXPORT ItiRptDLLPrintReport
                 (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[])
   {
   PSZ     pszKey = szKey;

           /* General return code variable: */
   USHORT  us = 0;

           /* Page formatting location variables: */
   USHORT  usLn  = 1;
   USHORT  usCol = 0;
   USHORT  usCurrentPg = 0;
   LONG    lColWidthPgUnits = 0L;
   INT     iBreakLnCnt = 0;      /* line counter */
   BOOL    bTitlesSet  = FALSE;

           /* Used by DbExecQuer & DbGetQueryRow functions: */
   HHEAP   hheap  = NULL;
   HQRY    hqry   = NULL;
   USHORT  usResId   = ITIRID_RPT;
   USHORT  usId      = 0;
   USHORT  usNumCols = 0;
   USHORT  usErr     = 0;
   PSZ   * ppsz   = NULL;

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



   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_CODE_TABLE_LISTING,
                       szBaseQuery, SIZE_OF_QUERY);

   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_WHERE_CONJUNCTION,
                       szWhereConj, SIZE_OF_CONJ);


   ItiRptUtInitKeyList(argcnt, argvars); /*  */
   while (ItiRptUtGetNextKey(pszKey, sizeof(szKey)) != ITIRPTUT_NO_MORE_KEYS)
      {
      if (*pszKey != '\0')
         {
         if (*pszKey == ERROR_KEY_CHAR)
            {
            break;
            }

         /* -- Prepare the query with the current key. */
         ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                           szBaseQuery, szWhereConj, pszKey);
         }
      else
         {
         ItiStrCpy(szQuery, szBaseQuery, QUERY_ARRAY_SIZE);
         }


      ItiStrCat(szQuery, " order by CodeTableID,UnitType,CodeValueID ", QUERY_ARRAY_SIZE);


      /* -- Do the database query. */
      hqry = ItiDbExecQuery (szQuery, hheap, hmodModule, ITIRID_RPT,
                             ID_QUERY_CODE_TABLE_LISTING, &usNumCols, &usErr);
   
   
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
         usCurrentPg = ItiPrtQueryCurrentPgNum(); /* init the page number.  */
         /* -- Set the Header. */
         ItiPrtChangeHeaderLn(ITIPRT_TITLE, NULL);

         iBreakLnCnt = 1;/* data line counter variable, used for ln spacing.*/
      
         while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )
            {
   
            /* ============================================================== *\
             *  The code between the doubled dashed lines is report specific. *
            \* -------------------------------------------------------------- */
   
            /*  Check if this row is from next ID.                            */
            if (ItiStrCmp(szCurrentID, ppsz[CODETAB]) != 0 )
               {
               /* ----------------------------------------------------------- *\
                * Insert 2 spacer lines.                                      *
               \* ----------------------------------------------------------- */
               usCurrentPg = ItiPrtSkipLns(2);
                                
      
               /* ----------------------------------------------------------- *\
                * Bump down table desc. if too close to bottom of page.       *
               \* ----------------------------------------------------------- */
               if (ItiPrtKeepNLns(5) == PG_WAS_INC)
                  {
                  /* -- Set the Header. */
                  ItiPrtChangeHeaderLn(ITIPRT_TITLE, NULL);
                  }
   
   
               /* ------------------------------------------------------------- *\
                * Setup the Code-Table-ID and Description.                      *
               \* ------------------------------------------------------------- */
               ItiPrtPutTextAt (0, 0, LEFT, 7, "Table: ");
               
               ItiPrtPutTextAt (0, (ItiPrtQueryCurrentColNum() + 1),
                                NONE, 15, ppsz[CODETAB]);
   
               ItiPrtPutTextAt (0, (ItiPrtQueryCurrentColNum() + 1),
                                NONE, 18, " - ");
   
               ItiPrtPutTextAt (0, (ItiPrtQueryCurrentColNum() + 1),
                                NONE, USE_RT_EDGE, ppsz[DESC]);
               ItiPrtIncCurrentLn(); // line feed
   
               ItiPrtIncCurrentLn(); // blank line
               iBreakLnCnt= 1;  // reset the count for the new table.
      
      
               /* ------------------------------------------------------------- *\
                * Set the Code-Value-ID and Translation column titles.          *
               \* ------------------------------------------------------------- */
               if (ItiStrCmp("----", ppsz[CODEVAL]) == 0 )
                  SetColumnTitles(FALSE);
               else
                  SetColumnTitles(TRUE);

               bTitlesSet = TRUE;
      
               /* ------------------------------------------------------------- *\
                * Set the current Table ID.                                     *
               \* ------------------------------------------------------------- */
               ItiStrCpy(szCurrentID, ppsz[CODETAB],
                         sizeof(szCurrentID) );
      
               } /* END of if this row is from next ID. */
            else
               bTitlesSet = FALSE;
   
   
            /* ---------------------------------------------------------------- *\
             * If we are on a new page...                                       *
            \* ---------------------------------------------------------------- */
            if (ItiPrtQueryCurrentPgNum() != usCurrentPg )
               {
               /* -- Set the Page Header. */
               ItiPrtChangeHeaderLn(ITIPRT_TITLE, NULL);

               usCurrentPg = ItiPrtQueryCurrentPgNum();
      
               /* -- Reset the starting spacer line count. */
               iBreakLnCnt = 1;
      
               /* -- Set the Code-Value-ID and Trans column titles for new pg. */
               if (!bTitlesSet)
                  {
                  if (ItiStrCmp("----", ppsz[CODEVAL]) == 0 )
                     SetColumnTitles(FALSE);
                  else
                     SetColumnTitles(TRUE);
                  }
               }
      
            /* ---------------------------------------------------------------- *\
             * Set the column's data into the current page line.                *
            \* ---------------------------------------------------------------- */
            if (   (ItiStrCmp("----", ppsz[CODEVAL]) == 0 )
                && (bColTitlesSet == FALSE) )
               {
               ItiPrtIncCurrentLn(); // skip line
               iBreakLnCnt = 1; /* -- Reset the starting spacer line count. */
               }

            bColTitlesSet = FALSE;

            /* -- Set the Code-Value-ID, string(8). */
            ItiPrtPutTextAt (0, 15, LEFT, 23, ppsz[CODEVAL]);

            /* -- Set the unit system, string(1). */
            // ItiPrtPutTextAt (0, 23, LEFT, 24, ppsz[UNTSYS]);

            /* -- Set the Translation, string(40). */
            ItiPrtPutTextAt (0, 26, LEFT, 66, ppsz[TRANS]);
            ItiPrtIncCurrentLn();
      
            /* -- Insert blank line at regular intervals for readablity. */
            if ( iBreakLnCnt == BLANK_LINE_INTERVAL)
               {
               ItiPrtIncCurrentLn();
               iBreakLnCnt = 1;
               }
            else
               iBreakLnCnt++;
   

            ItiFreeStrArray (hheap, ppsz, usNumCols);

            /* ============================================================== */
            }/* End of WHILE GetNextRow */

         }/* End of if(hqry...) else clause */

      }/* End of WHILE GetNextKey */
   





   /* ------------------------------------------------------------------- *\
    * Finish up and quit.                                                 *
   \* ------------------------------------------------------------------- */
   ItiPrtEndReport();

   DBGMSG("Exit ***DLL function ItiRptDLLPrintReport");
   return 0;

   }/* END OF FUNCTION ItiRptDLLPrintReport */
