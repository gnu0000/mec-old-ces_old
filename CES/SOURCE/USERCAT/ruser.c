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


/*  CES User Listing Report DLL Source Code
 *
 *  Timothy Blake (tlb)
 *  Copyright (C) 1991, Info Tech, Inc.
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


#include "rUser.h"                                       



/* ======================================================================= *\
 *                                         MANIFEST CONSTANT DEFINITIONS   *
\* ======================================================================= */
#define  BLANK_LINE_INTERVAL    4
#define  PRT_Q_NAME             "User List"

#define  QUERY_ARRAY_SIZE       512


#define  SIZE_OF_QUERY_RPT_DESC QUERY_ARRAY_SIZE



/* ======================================================================= *\
 *                                             MODULE'S GLOBAL VARIABLES   *
\* ======================================================================= */
static  CHAR  szCompileDateTime[] = __DATE__ " " __TIME__ ;
static CHAR szDllVersion[] = "1.1a0 ruser.dll";

static  CHAR  szCurrentID   [SMARRAY + 1] = "";
static  CHAR  szOrderingStr [LGARRAY + 1] = "";

static  CHAR  szQueryRptDesc [SIZE_OF_QUERY + 1];

static  CHAR  szBaseQuery   [QUERY_ARRAY_SIZE + 1];
static  CHAR  szAppQuery    [SIZE_OF_QUERY_APP + 1];
static  CHAR  szCatQuery    [SIZE_OF_QUERY_CAT + 1];
static  CHAR  szQuery       [QUERY_ARRAY_SIZE + 1];
static  CHAR  szConj        [SIZE_OF_CONJ + 1];
static  CHAR  szKey         [SMARRAY + 1];

static  CHAR  szFailedDLL [CCHMAXPATH + 1];
static  CHAR  szErrorMsg [1024] = "";

static  USHORT usButton = 1;

static  USHORT usNumOfTitles;
static  USHORT usCurrentPg  = 0;
static  USHORT usBreakLnCnt = 0;


           /* Used by DbExecQuer & DbGetQueryRow functions: */
// static  HHEAP   hheap  = NULL;
static  HQRY    hqry   = NULL;
static  USHORT  usResId   = ITIRID_RPT;
static  USHORT  usId      = ID_QUERY_LISTING;
static  USHORT  usNumCols = 0;
static  USHORT  usErr     = 0;
static  PSZ   * ppsz   = NULL;

static  CHAR  szKeyFileName[32] = "";
static  PSZ   apszAlt[2] = { "/K", szKeyFileName };


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





USHORT SetColumnTitles (USHORT usTitleStart, USHORT usTitleStop)
   /* -- Print the column titles. */
   {
   USHORT us;
   USHORT usErr;

      /* -- Actual printing. */
   for (us=usTitleStart; us <= usTitleStop; us++)
      {
      usErr = ItiPrtPutTextAt (CURRENT_LN, acttl[us].usTtlCol,
                               LEFT, (acttl[us].usTtlColWth | REL_LEFT),
                               acttl[us].szTitle);
      if (us == RPT_TTL_3)
         ItiPrtIncCurrentLn (); //line feed

      if (usErr != 0)
         break;
      }
   ItiPrtIncCurrentLn (); //line feed

   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   ItiPrtIncCurrentLn (); //line feed

   return usErr;
   }/* END OF FUNCTION SetColumnTitles */







USHORT WriteUserIDSection (PSZ pszKeyVal)
   {
   USHORT us    = 0;
   USHORT usCnt = 0;
   USHORT usRC  = 0;
   USHORT usLn = 5;
   HHEAP  hhpWUID;

   /* ------------------------------------------------------------------- *\
    *  Create a memory heap.                                              *
   \* ------------------------------------------------------------------- */
   hhpWUID = ItiMemCreateHeap (MAX_HEAP_SIZE);


   if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
      {
      /* -- Set the Header. */
      ItiPrtChangeHeaderLn(ITIPRT_TITLE, NULL);
      }
   

   /* ---------------------------------------------------------------- *\
    * If we are on a new page...                                       *
   \* ---------------------------------------------------------------- */
   if (ItiPrtQueryCurrentPgNum() != usCurrentPg )
      {
      usCurrentPg = ItiPrtQueryCurrentPgNum();
   
      /* -- Reset the starting spacer line count. */
      usBreakLnCnt = 1;
   
      /* -- Set the column titles for new pg.     */
      SetColumnTitles(RPT_TTL_1, RPT_TTL_4);
      }



   /* -- Prepare the query with the current key. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szBaseQuery, " ", pszKeyVal);

   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hhpWUID, hmodModule, ITIRID_RPT,
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

      ItiRptUtErrorMsgBox (NULL, szErrorMsg);
      }
   else
      {
      usCnt = 1;
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )
         {
         if (usCnt > 1)
            {
            ItiRptUtErrorMsgBox (NULL, "ERROR: Multiple users w/ same key.");
            }
         else
            for (us=1; us <= NUM_ATTRIB_COLS_LST; us++)
               {
               usRC = ItiPrtPutTextAt (CURRENT_LN, acttl[us].usDtaCol,
                                       LEFT,
                                       (acttl[us].usDtaColWth | REL_LEFT),
                                       ppsz[us-1]);
               if (us == RPT_TTL_3)/*TYPE*/
                  ItiPrtIncCurrentLn (); //line feed
               }/* end of for */
         ItiPrtIncCurrentLn (); //line feed
         usCnt++;
         }/* end of while */

      }/* end of else clause */

   ItiMemDestroyHeap (hhpWUID);

   return (usErr);
   }/* End of Function WriteUserIDSection */







USHORT WriteApplicationsSection (PSZ pszKeyVal)
   {
   USHORT us;
   USHORT usCnt = 0;
   USHORT usRC  = 0;
   USHORT usLn = 5;
   HHEAP  hhpWAS;


   /* ------------------------------------------------------------------- *\
    *  Create a memory heap.                                              *
   \* ------------------------------------------------------------------- */
   hhpWAS = ItiMemCreateHeap (MAX_HEAP_SIZE);


   /* -- Prepare the query with the current key. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szAppQuery, " ", pszKeyVal);

   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hhpWAS, hmodModule, ITIRID_RPT,
                          ID_QUERY_APP, &usNumCols, &usErr);
   
   
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

      ItiRptUtErrorMsgBox (NULL, szErrorMsg);
      }
   else
      {
      us = 0;
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )
         {
         usLn = 3 + (ItiStrLen(ppsz[DESC]) / acttl[DESC].usDtaColWth);
         if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
            {
            /* -- Set the Header. */
            ItiPrtChangeHeaderLn(ITIPRT_TITLE, NULL);

            usCurrentPg = ItiPrtQueryCurrentPgNum();
         
            /* -- Reset the starting spacer line count. */
            usBreakLnCnt = 1;
         
            /* -- Set the column titles for new pg.     */
            SetColumnTitles(RPT_TTL_1, RPT_TTL_4);
      
            usRC = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_5].usTtlCol,
                                    LEFT,
                                    (acttl[RPT_TTL_5].usTtlColWth | REL_LEFT),
                                    acttl[RPT_TTL_5].szTitle);
            }
         else
            {
            if (us == 0)
               usRC = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_5].usTtlCol,
                                    LEFT,
                                    (acttl[RPT_TTL_5].usTtlColWth | REL_LEFT),
                                    acttl[RPT_TTL_5].szTitle);
            us++;
            }
   


         usRC = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_5].usDtaCol,
                                 (LEFT | WRAP | LNINC),
                                 (acttl[RPT_TTL_5].usDtaColWth | REL_LEFT),
                                 ppsz[ID]);

         usRC = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_6].usDtaCol,
                                 (LEFT | WRAP | LNINC),
                                 (acttl[RPT_TTL_6].usDtaColWth | REL_LEFT),
                                 ppsz[DESC]);
         }/* end of while */

      ItiPrtIncCurrentLn (); //blank line 

      }/* end of else clause */

   ItiMemDestroyHeap (hhpWAS);

   return (usErr);
   }/* End of Function WriteApplicationsSection */







USHORT WriteCatalogsSection (PSZ pszKeyVal)
   {
   USHORT us;
   USHORT usCnt = 0;
   USHORT usRC  = 0;
   USHORT usLn  = 4;
   HHEAP  hhpWCS;


   /* ------------------------------------------------------------------- *\
    *  Create a memory heap.                                              *
   \* ------------------------------------------------------------------- */
   hhpWCS = ItiMemCreateHeap (MAX_HEAP_SIZE);


   /* -- Prepare the query with the current key. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szCatQuery, " ", pszKeyVal);

   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hhpWCS, hmodModule, ITIRID_RPT,
                          ID_QUERY_CAT, &usNumCols, &usErr);
   
   
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

      ItiRptUtErrorMsgBox (NULL, szErrorMsg);
      }
   else
      {
      us = 0;
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )
         {
         if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
            {
            /* -- Set the Header. */
            ItiPrtChangeHeaderLn(ITIPRT_TITLE, NULL);

            usCurrentPg = ItiPrtQueryCurrentPgNum();
         
            /* -- Reset the starting spacer line count. */
            usBreakLnCnt = 1;
         
            /* -- Set the column titles for new pg.     */
            SetColumnTitles(RPT_TTL_1, RPT_TTL_4);
      
            usRC = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_7].usTtlCol,
                                    LEFT,
                                    (acttl[RPT_TTL_7].usTtlColWth | REL_LEFT),
                                    acttl[RPT_TTL_7].szTitle);
            }
         else
            {
            if (us == 0)
               usRC = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_7].usTtlCol,
                                    LEFT,
                                    (acttl[RPT_TTL_7].usTtlColWth | REL_LEFT),
                                    acttl[RPT_TTL_7].szTitle);
            us++;
            }
   
      
         usRC = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_7].usDtaCol,
                                 LEFT,
                                 (acttl[RPT_TTL_7].usDtaColWth | REL_LEFT),
                                 ppsz[ID]);

         usRC = ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_8].usDtaCol,
                                 LEFT,
                                 (acttl[RPT_TTL_8].usDtaColWth | REL_LEFT),
                                 ppsz[PERM]);
         ItiPrtIncCurrentLn (); //line feed
         }/* end of while */

      ItiPrtIncCurrentLn (); //blank line 

      }/* end of else clause */


   ItiMemDestroyHeap (hhpWCS);

   return (usErr);
   }/* End of Function WriteCatalogsSection */






/* -- Converts OrderingStr from ColIDs into ListingQuery column value. */

USHORT ConvertOrderingStr (PSZ pszOrderingStr)
   {
   switch (pszOrderingStr[0])
      {
      case ORDER_SW_VAL_NAME:
         ItiStrCpy(pszOrderingStr, " order by User.Name ", LGARRAY);
         break;

      case ORDER_SW_VAL_USERID:
      default:
         ItiStrCpy(pszOrderingStr, " order by User.UserID ", LGARRAY);
         break;
      }/* end switch */

   return (0);
   }/* End of Function ConvertOrderingStr */







USHORT PrintProc (HWND hwnd, MPARAM mp1, MPARAM mp2)
   {
   USHORT us    = 0;
   CHAR   szKey      [SMARRAY] = "";
   CHAR   szOptSwStr [SMARRAY] = "";
   CHAR   szCmdLn [2*SMARRAY +1] = "";
   BOOL   bChecked = FALSE;
   HFILE  hAltFile;
   HHEAP  hhp = NULL;


   /* ------------------------------------------------------------------- *\
    *  Create a memory heap.                                              *
   \* ------------------------------------------------------------------- */
   hhp = ItiMemCreateHeap (MAX_HEAP_SIZE);


   /* -- reset the command line. */
   ItiStrCpy (szCmdLn, RPT_DLL_NAME, sizeof(szCmdLn) );


   bChecked = (USHORT) (ULONG) WinSendDlgItemMsg (hwnd, 
                               DID_SELECTED, BM_QUERYCHECK, 0, 0);
   if (bChecked)
      {
      /* -- Build cmdln string to do selected users. */
      us = ItiRptUtBuildSelectedKeysFile(hwnd,
                             UserCatL,
                             cUserKey,
                             szKeyFileName, sizeof szKeyFileName);

      if ((us == ITIRPTUT_PARM_ERROR)|| (us == ITIRPTUT_ERROR))
         ItiRptUtErrorMsgBox (hwnd,
                "ERROR: Failed to get selected keys in "__FILE__);
      else
         {
         }/* end of if ((us... */

      }/* end of if then (bChecked... */
   else
      {/* build a key file that contains ALL valid user keys. */
      ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_KEYS,
                          szQuery, sizeof szQuery );

      /* -- Order the keys here. */
      szOrderingStr[1] = '\0';
      if ((BOOL)(ULONG)WinSendDlgItemMsg(hwnd,DID_NAME,BM_QUERYCHECK,0,0))
         szOrderingStr[0] = ORDER_SW_VAL_NAME;
      else
         szOrderingStr[0] = ORDER_SW_VAL_USERID;

      ConvertOrderingStr (szOrderingStr);
      ItiStrCat (szQuery, szOrderingStr, sizeof szQuery);


      /* -- Do the database query. */
      hqry = ItiDbExecQuery (szQuery, hhp, hmodModule, ITIRID_RPT,
                             ID_QUERY_KEYS, &usNumCols, &usErr);
   
   
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
         ItiRptUtErrorMsgBox (hwnd, szErrorMsg);
         }
      else
         {
         ItiRptUtStartKeyFile (&hAltFile, szKeyFileName, sizeof szKeyFileName);

         while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )
            {
            ItiRptUtWriteToKeyFile (hAltFile, ppsz[KEY], NULL);
            ItiFreeStrArray (hhp, ppsz, usNumCols);
            }/* End of WHILE GetQueryRow for Listing query. */

         ItiRptUtEndKeyFile (hAltFile);
         }

      }/* end of if (bChecked... else clause */



   /* -- Prep the CmdLn to use the key file. */
   ItiStrCat (szCmdLn, ITIRPT_OPTSW_KEYFILE, sizeof szCmdLn);
   ItiStrCat (szCmdLn, szKeyFileName, sizeof szCmdLn);

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
   ItiStrCpy (pszBuffer, "~User Listing", usMaxSize);   
   *pusWindowID = rUser_RptDlgBox;                       
   }/* END OF FUNCTION */



BOOL EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID, 
                                     USHORT usActiveWindowID) 
   {                                                          
   if ((usTargetWindowID == rUser_RptDlgBox)
      && (usActiveWindowID == UserCatS))                  
      return TRUE;                                            
   else                                                       
      return FALSE;                                           
   }/* END OF FUNCTION */



MRESULT EXPORT ItiDllrUserRptDlgProc (HWND     hwnd,     
                                    USHORT   usMessage,
                                    MPARAM   mp1,
                                    MPARAM   mp2)
   {
   USHORT us = 0;
   USHORT usAlt = 0;
   CHAR   szCmdLn  [LGARRAY+1] = RPT_DLL_NAME " ";
   CHAR   szKeyStr [SMARRAY+1] = "";

   szKeyStr[0] = '\0';

   switch (usMessage)
      {
      case WM_INITDLG:
         us = ItiRptUtInitDlgBox (hwnd, RPT_SESSION, 0, NULL);

         /* -- Set default button state. */
         WinSendDlgItemMsg (hwnd, DID_USERID, BM_SETCHECK, CHECKED, 0);
         break;


      case WM_CONTROL:
         if ((USHORT) SHORT2FROMMP(mp1) != BN_CLICKED)
            break;

         switch ((USHORT) SHORT1FROMMP (mp1))
            {
            case (DID_USERID):
            case (DID_NAME):
               us = (USHORT)(ULONG)WinSendDlgItemMsg (hwnd, 
                                             DID_USERID,
                                             BM_QUERYCHECK, 0L, 0L);
               usAlt = (USHORT)(ULONG)WinSendDlgItemMsg (hwnd, 
                                             DID_NAME,
                                             BM_QUERYCHECK, 0L, 0L);
               if ((us == 1) || (usAlt == 1))
                  {
                  WinSendDlgItemMsg(hwnd,DID_SELECTED,BM_SETCHECK,UNCHECKED,0);
                  WinSetDlgItemText(hwnd,DID_SELECTED+1,  "  Order by:  ");
                  if (us == 1)
                     usButton = 1; /* UserID pushed */
                  else
                     usButton = 2; /* Name pushed */
                  }

               break;


            case (DID_SELECTED):
               us = (USHORT)(ULONG)WinSendDlgItemMsg (hwnd, DID_SELECTED,
                                                      BM_QUERYCHECK, 0L, 0L);
               if (us == 1) /* selected so... */
                  {                                                      
                  WinSetDlgItemText(hwnd, DID_SELECTED+1, "Default Order");

                  us = (USHORT)(ULONG)WinSendDlgItemMsg (hwnd, 
                                             DID_USERID,
                                             BM_QUERYCHECK, 0L, 0L);
                  if (us == 1)
                     usButton = 1; /* UserID was set so remember it. */
                  else
                     usButton = 2; /* Name was set so remember it. */

                  WinSendDlgItemMsg (hwnd, DID_USERID, BM_SETCHECK, UNCHECKED, 0);
                  WinSendDlgItemMsg (hwnd, DID_NAME,   BM_SETCHECK, UNCHECKED, 0);
                  }
               else
                  {
                  WinSetDlgItemText(hwnd, DID_SELECTED+1, "  Order by:  ");
                  if (usButton == 2)
                     WinSendDlgItemMsg (hwnd, DID_NAME, BM_SETCHECK, CHECKED, 0);
                  else
                     WinSendDlgItemMsg (hwnd, DID_USERID, BM_SETCHECK, CHECKED, 0);
                  }
               break;
            }/* end of switch */
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
   USHORT  usCurrentPg = 0;
   POINTL  ptl    = {0L,0L};
   POINTL  ptlVal = {0L,0L};
   LONG    lColWidthPgUnits = 0L;
   INT     iBreakLnCnt = 0;      /* line counter */
   BOOL    bTitlesSet  = FALSE;

           /* Title strings for this module. */
   REPTITLES rept;
   PREPT     prept = &rept;



   /* ------------------------------------------------------------------- *\
    *  Create a memory heap.                                              *
   \* ------------------------------------------------------------------- */
   // hheap = ItiMemCreateHeap (MAX_HEAP_SIZE);


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

   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_APP,
                       szAppQuery, sizeof szAppQuery );

   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_CAT,
                       szCatQuery, sizeof szCatQuery );

   /* -- Set the Header. */
   ItiPrtChangeHeaderLn(ITIPRT_TITLE, NULL);

   ItiRptUtInitKeyList(argcnt, argvars); /*  */
   while (ItiRptUtGetNextKey(pszKey, sizeof(szKey)) != ITIRPTUT_NO_MORE_KEYS)
      {
      if (*pszKey != '\0')
         {
         WriteUserIDSection(pszKey);
         WriteApplicationsSection(pszKey);
         WriteCatalogsSection(pszKey);
         }
      else
         {
         /* -- This report requires a key file, so if no key... */
         break;
         }

      }/* End of WHILE GetNextKey */
   


   /* ------------------------------------------------------------------- *\
    * Finish up and quit.                                                 *
   \* ------------------------------------------------------------------- */
   ItiPrtEndReport();

   DBGMSG("Exit ***DLL function ItiRptDLLPrintReport");
   return 0;

   }/* END OF FUNCTION ItiRptDLLPrintReport */





