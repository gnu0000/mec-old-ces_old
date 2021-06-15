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

#include    "rmap.h"  



/* ======================================================================= *\
 *                                         MANIFEST CONSTANT DEFINITIONS   *
\* ======================================================================= */
#define  BLANK_LINE_INTERVAL    4
#define  PRT_Q_NAME             "Map Listings"

#define  QUERY_ARRAY_SIZE       1024
         /* size of query plus criteria of " AND Key = XXXXXXXX" */

#define  SIZE_OF_QUERY_RPT_DESC QUERY_ARRAY_SIZE



/* ======================================================================= *\
 *                                             MODULE'S GLOBAL VARIABLES   *
\* ======================================================================= */
static  CHAR  szCompileDateTime[] = __DATE__ " " __TIME__ ;
static CHAR szDllVersion[] = "1.1a0 rmap.dll";

static  CHAR  szCurrentID   [SMARRAY + 1];

static  CHAR  szQueryRptDesc [SIZE_OF_QUERY + 1];

static  CHAR  szBaseQuery   [QUERY_ARRAY_SIZE + 1];
static  CHAR  szQuery       [QUERY_ARRAY_SIZE + 1];
static  CHAR  szWhereConj   [SIZE_OF_CONJ + 1];
static  CHAR  szKey         [SMARRAY + 1];

static  CHAR    szFailedDLL [CCHMAXPATH + 1];
static  CHAR    szErrorMsg  [1024] = "";

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
      ItiStrCpy(acttl[us].szTitle, ItiStrTok(NULL,", "),
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







USHORT WriteRowData (PSZ *ppsz)
   {
   USHORT us   = 0;
   USHORT usLn = 0;
   USHORT usErr;
   INT    i    = 1;


   while (ppsz[DESC][i] != '\0')
      {
      if (ppsz[DESC][i] == '\n')
         usLn++;
      i++;
      }
   ItiPrtKeepNLns (usLn);



   for (us=1; us <= NUM_ATTRIB_COLS; us++)
      {
      if (us < NUM_ATTRIB_COLS)
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
         }


      if (usErr != 0)
         break;
      }

   return (usErr);
   }/* End of Function WriteRowData */









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
   ItiStrCpy (pszBuffer, "~Map Listing", usMaxSize);
   *pusWindowID = rMap_RptDlgBox;
   }




BOOL EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID,
                                     USHORT usActiveWindowID)
   {
   if ((usTargetWindowID == rMap_RptDlgBox)
      && (usActiveWindowID == MapCatS))
      return TRUE;
   else
      return FALSE;
   }






MRESULT EXPORT ItiDllMapRptDlgProc (HWND     hwnd,
                                    USHORT   usMessage,
                                    MPARAM   mp1,
                                    MPARAM   mp2)
   {
   USHORT us = 0;
   CHAR   szKeyStr [SMARRAY+1] = "";
   CHAR   szCmdLn  [LGARRAY+1] = RPT_DLL_NAME " ";
   CHAR   szFName  [SMARRAY+1] = "";
   BOOL   bChecked = FALSE;


   szKeyStr[0] = '\0';

   switch (usMessage)
      {
      case WM_INITDLG:
         us = ItiRptUtInitDlgBox (hwnd, RPT_SESSION, 0, NULL);
         break;


      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               bChecked = (USHORT) (ULONG) WinSendDlgItemMsg (hwnd, 
                                           DID_SELECTED, BM_QUERYCHECK, 0, 0);
               if (bChecked)
                  {
                  /* -- Build cmdln string to do selected rows. */

                  us = ItiRptUtBuildSelectedKeysFile(hwnd,
                                         MapCatL,
                                         cMapKey,
                                         szFName, sizeof szFName);

                  if ((us == ITIRPTUT_PARM_ERROR)|| (us == ITIRPTUT_ERROR))
                     {
                     WinSendDlgItemMsg(hwnd, DID_SELECTED, BM_SETCHECK, UNCHECKED, 0L);
                     return (0);
                     }
                  else
                     {/* -- Prep the CmdLn to use the key file. */
                     ItiStrCat (szCmdLn, ITIRPT_OPTSW_KEYFILE, sizeof szCmdLn);
                     ItiStrCat (szCmdLn, szFName, sizeof szCmdLn);
                     }/* end of if ((us... */

                  }/* end of if (bChecked... */


               us = ItiRptUtExecRptCmdLn (hwnd, szCmdLn, RPT_SESSION);


               WinDismissDlg (hwnd, TRUE);
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
   USHORT  usCurrentPg = 0;
   POINTL  ptl    = {0L,0L};
   POINTL  ptlVal = {0L,0L};
   LONG    lColWidthPgUnits = 0L;
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


      ItiStrCat(szQuery, " order by MapID ", QUERY_ARRAY_SIZE);


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


         /* -- Insert blank line at regular intervals for readablity. */
         if ( iBreakLnCnt == BLANK_LINE_INTERVAL)
            {
            ItiPrtIncCurrentLn();
            iBreakLnCnt = 1;
            }
         else
            iBreakLnCnt++;


         }/* End of WHILE GetQueryRow for Listing query. */


      }/* End of WHILE GetNextKey */
   





   /* ------------------------------------------------------------------- *\
    * Finish up and quit.                                                 *
   \* ------------------------------------------------------------------- */
   ItiPrtEndReport();

   DBGMSG("Exit ***DLL function ItiRptDLLPrintReport");
   return 0;

   }/* END OF FUNCTION ItiRptDLLPrintReport */
