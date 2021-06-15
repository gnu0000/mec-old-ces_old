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


/*  CES Area Types Listing Report DLL Source Code
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


#include    "rindllar.h"


#include "rAreas.h"                                       



/* ======================================================================= *\
 *                                         MANIFEST CONSTANT DEFINITIONS   *
\* ======================================================================= */
#define  BLANK_LINE_INTERVAL    3
#define  PRT_Q_NAME             "Area Type List"

#define  QUERY_ARRAY_SIZE       640

#define  SIZE_OF_QUERY_RPT_DESC QUERY_ARRAY_SIZE



/* ======================================================================= *\
 *                                             MODULE'S GLOBAL VARIABLES   *
\* ======================================================================= */
static  CHAR  szCompileDateTime[] = __DATE__ " " __TIME__ ;
static CHAR szDllVersion[] = "1.1a0 raAreas.dll";

static  CHAR  szCurrentID   [SMARRAY + 1] = "";
static  CHAR  szOrderingStr [LGARRAY + 1] = "";

static  CHAR  szQueryDesc    [SIZE_OF_QUERY + 1] = "";
static  CHAR  szQueryListing [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szQuery        [QUERY_ARRAY_SIZE + 1] = "";
static  CHAR  szKey          [SMARRAY + 1] = "";
static  CHAR  szAreaTypeID   [32] = "";


static  CHAR   szFailedDLL [CCHMAXPATH + 1];
static  CHAR   szErrorMsg [1024] = "";

static  USHORT usNumOfTitles;
static  USHORT usCurrentPg = 0;


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









USHORT SummarySectionFor (PSZ pszKey, PSZ pszAreaTypeID)
   {
   USHORT usLn, us;
   PSZ * ppsz = NULL;


   if ((pszKey == NULL) || (*pszKey == '\0'))
      return 13;

   /* -- Build the query. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryDesc, " ", pszKey);


   ppsz = ItiDbGetRow1 (szQuery, hheap, hmodModule, ITIRID_RPT,
                        ID_QUERY_DESC, &usNumCols);

   if (ppsz != NULL)
      {
      usLn = 7 + ItiStrLen(ppsz[DESC]) / acttl[RPT_TTL_2].usDtaColWth;
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
                       ppsz[ID]);
      ItiStrCpy(pszAreaTypeID, ppsz[ID], sizeof szAreaTypeID);

      us = acttl[RPT_TTL_1].usDtaCol + ItiStrLen(ppsz[ID]);  // + 1;

      ItiPrtPutTextAt (CURRENT_LN, us,
                       LEFT, (acttl[RPT_TTL_2].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_2].szTitle);

      ItiPrtPutTextAt (CURRENT_LN, (us + acttl[RPT_TTL_2].usTtlColWth),
                       (LEFT|WRAP),
                       (2 | FROM_RT_EDGE),
                       //(acttl[RPT_TTL_2].usDtaColWth | REL_LEFT),
                       ppsz[DESC]);
//      ItiPrtIncCurrentLn();

      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }
   else
      return 13;

   return 0;
   }/* End of Function SummarySectionFor */




USHORT ACListSectionFor (PSZ pszAreaTypeID, PSZ pszKey)
/* -- Area/County listing function for a given areatype key. */
   {
   USHORT usLn;
   BOOL   bNewAreaID = TRUE;
   BOOL   bLabeled   = FALSE;
   CHAR   szCurrentAreaID[32] = "";


   if ((pszKey == NULL) || (*pszKey == '\0') || (pszAreaTypeID == NULL))
      return 13;

   /* -- Build the query. */
   ItiRptUtPrepQuery(szQuery, (QUERY_ARRAY_SIZE + 1),
                     szQueryListing, pszKey, " order by 1, 4 ");


   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hheap, hmodModule, ITIRID_RPT,
                          ID_QUERY_LISTING, &usNumCols, &usErr);
   
   /* -- Process the query results for each row. */
   if (hqry == NULL)
      {
      ItiStrCpy (szErrorMsg,                                    
                 "ERROR: Query failed for ",                    
                 sizeof szErrorMsg);                            
                                                                
      ItiStrCat (szErrorMsg, TITLE, sizeof szErrorMsg);         
      ItiStrCat (szErrorMsg,", Error Code: ",sizeof szErrorMsg);
                                                                
      ItiStrUSHORTToString                                      
         (usErr, &szErrorMsg[ItiStrLen(szErrorMsg)],            
          sizeof szErrorMsg);                                   
                                                                
      ItiStrCat (szErrorMsg,".  Query: ", sizeof szErrorMsg);   
      ItiStrCat (szErrorMsg, szQuery, sizeof szErrorMsg);       
                                                                
      ItiRptUtErrorMsgBox (NULL, szErrorMsg);
      }
   else
      {
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )  
         {
            /* -- Check for new page. */
            usLn = 5+ItiStrLen(ppsz[AREA_DESC])/acttl[RPT_TTL_4].usDtaColWth;
            if (ItiPrtKeepNLns(usLn) == PG_WAS_INC)
               {
               usCurrentPg = ItiPrtQueryCurrentPgNum();
               ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");

               ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_7].usTtlCol,
                       LEFT, (acttl[RPT_TTL_7].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_7].szTitle);

               ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_7].usDtaCol,
                       LEFT, (acttl[RPT_TTL_7].usDtaColWth | REL_LEFT),
                       pszAreaTypeID);

//               if (bLabeled)
//                  {
//                  ItiPrtPutTextAt(CURRENT_LN, (ItiPrtQueryCurrentColNum() +3),
//                       LEFT, (acttl[RPT_TTL_3].usTtlColWth | REL_LEFT),
//                       acttl[RPT_TTL_3].szTitle);
//                  ItiPrtPutTextAt(CURRENT_LN, (ItiPrtQueryCurrentColNum() +2),
//                       LEFT, (acttl[RPT_TTL_3].usDtaColWth | REL_LEFT),
//                       ppsz[AREA_ID]);
//                  }

               ItiPrtIncCurrentLn (); //line feed
               ItiPrtIncCurrentLn (); //line feed

               bLabeled = FALSE;
               }

         /* -- Writeout the AreaID subsection. */
         if (ItiStrCmp(szCurrentAreaID, ppsz[AREA_ID]) != 0)
            {
            ItiPrtIncCurrentLn (); //line feed

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_3].usTtlCol,
                       LEFT, (acttl[RPT_TTL_3].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_3].szTitle);

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_3].usDtaCol,
                             LEFT,
                             (acttl[RPT_TTL_3].usDtaColWth | REL_LEFT),
                             ppsz[AREA_ID]);

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_4].usTtlCol,
                       LEFT, (acttl[RPT_TTL_4].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_4].szTitle);

            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_4].usDtaCol,
                             LEFT,
                             (acttl[RPT_TTL_4].usDtaColWth | REL_LEFT),
                             ppsz[AREA_DESC]);

            ItiPrtIncCurrentLn (); //line feed
            ItiPrtIncCurrentLn (); //blank line
            ItiStrCpy(szCurrentAreaID, ppsz[AREA_ID], sizeof szCurrentAreaID);
            bLabeled = FALSE;

            }/* end of if (ItiStrCmp(szCur... then clause */



         /* -- Writeout the County subsection for the current AreaID. */
         if (!bLabeled)
            {
            ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_5].usTtlCol,
                       LEFT, (acttl[RPT_TTL_5].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_5].szTitle);
            bLabeled = TRUE;
            }

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_5].usDtaCol,
                       LEFT,
                       (acttl[RPT_TTL_5].usDtaColWth | REL_LEFT),
                       ppsz[COUNTY_ID]);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_6].usTtlCol,
                       LEFT, (acttl[RPT_TTL_6].usTtlColWth | REL_LEFT),
                       acttl[RPT_TTL_6].szTitle);

         ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_6].usDtaCol,
                       LEFT,
                       (acttl[RPT_TTL_6].usDtaColWth | REL_LEFT),
                       ppsz[COUNTY_DESC]);

         ItiPrtIncCurrentLn (); //line feed

         ItiFreeStrArray (hheap, ppsz, usNumCols);
         }/* End of WHILE GetQueryRow */

      }/* else clause */
   


   /* -- Finishout this section. */
   ItiPrtIncCurrentLn (); //blank line 
   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   ItiPrtIncCurrentLn (); //line feed

   }/* End of Function ACListSectionFor */






USHORT PrintProc (HWND hwnd, MPARAM mp1, MPARAM mp2)
   {
   USHORT us    = 0;
   CHAR   szKey      [SMARRAY] = "";
   CHAR   szOptSwStr [SMARRAY] = "";
   CHAR   szCmdLn [5*SMARRAY +1] = "";
   CHAR   szFName [4*SMARRAY +1] = "";
   BOOL   bChecked = FALSE;


   /* -- reset the command line. */
   ItiStrCpy (szCmdLn, RPT_DLL_NAME, sizeof(szCmdLn) );


   bChecked = (USHORT) (ULONG) WinSendDlgItemMsg (hwnd, 
                               DID_SELECTED, BM_QUERYCHECK, 0L, 0L);
   if (bChecked)
      {
      /* -- Build cmdln string to do selected code tables. */

      us = ItiRptUtBuildSelectedKeysFile(hwnd,
                             AreaTypeCatL,
                             cAreaTypeKey,
                             szFName, sizeof szFName);

      if ((us == ITIRPTUT_PARM_ERROR)|| (us == ITIRPTUT_ERROR))
         {
         ItiRptUtErrorMsgBox (hwnd,
                "ERROR: Failed to get selected keys in "__FILE__);
         return 13;
         }
      }/* end of if (bChecked... then clause */

   else
      {/* Use a query to get all of the keys. */
      ItiStrCpy(szFName,
                " \""ITIRPTUT_KEY_QUERY_SW_STR
                "select AreaTypeKey from AreaType "
                "where Deleted is NULL"
                " order by AreaTypeID"
                " \" ",
                sizeof szFName);
      }/* end of if (bChecked... else clause */



   /* -- Prep the CmdLn to use the key file. */
   ItiStrCat (szCmdLn, ITIRPT_OPTSW_KEYFILE, sizeof szCmdLn);
   ItiStrCat (szCmdLn, szFName, sizeof szCmdLn);

   us = ItiRptUtExecRptCmdLn (hwnd, szCmdLn, RPT_AREAS_SESSION);

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
   ItiStrCpy (pszBuffer, "Area Listing", usMaxSize);   
   *pusWindowID = rAreas_RptDlgBox;
   }/* END OF FUNCTION */



BOOL EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID, 
                                     USHORT usActiveWindowID) 
   {                                                          
   if ((usTargetWindowID == rAreas_RptDlgBox)                     
       && (   (usActiveWindowID == AreaTypeCatL)
           || (usActiveWindowID == AreaTypeCatS)) )
      return TRUE;                                            
   else                                                       
      return FALSE;                                           
   }/* END OF FUNCTION */




MRESULT EXPORT ItiDllrAreasRptDlgProc (HWND     hwnd,     
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
         us = ItiRptUtInitDlgBox (hwnd, RPT_AREAS_SESSION, 0, NULL);
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

               return 0L;



            case DID_CANCEL:
               WinDismissDlg (hwnd, FALSE);
               return 0L;

            }/* end of switch(SHORT1... */

         break;


      default:
         us = ItiRptUtChkDlgMsg (hwnd, usMessage, mp1, mp2);
         break;
      }/* end of switch (usMessage... */

   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);

   }/* END OF FUNCTION ItiDllrAreasRptDlgProc */








USHORT EXPORT ItiRptDLLPrintReport
                 (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[])
   {
   PSZ     pszKey = szKey;

           /* General return code variable: */
   USHORT  us = 0;




   /* ------------------------------------------------------------------- *\
    *  Create a memory heap.                                              *
   \* ------------------------------------------------------------------- */
   hheap = ItiMemCreateHeap (MIN_HEAP_SIZE);


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



   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_DESC,
                       szQueryDesc, sizeof szQueryDesc );

   ItiMbQueryQueryText(hmodModule, ITIRID_RPT, ID_QUERY_LISTING,
                       szQueryListing, sizeof szQueryListing );


   /* -------------------------------------------------------------------- *\
    * -- For each AreaTypeKey...                                           *
   \* -------------------------------------------------------------------- */
   ItiRptUtInitKeyList(argcnt, argvars); /*  */
   while (ItiRptUtGetNextKey(pszKey,sizeof(szKey)) != ITIRPTUT_NO_MORE_KEYS)
      {
      us = SummarySectionFor(pszKey, szAreaTypeID);

      if (us == 0)
         ACListSectionFor(szAreaTypeID, pszKey);

      }/* End of WHILE GetNextKey */



   /* ------------------------------------------------------------------- *\
    * Finish up and quit.                                                 *
   \* ------------------------------------------------------------------- */
   ItiPrtEndReport();

   DBGMSG("Exit ***DLL function ItiRptDLLPrintReport");
   return 0;

   }/* END OF FUNCTION ItiRptDLLPrintReport */





