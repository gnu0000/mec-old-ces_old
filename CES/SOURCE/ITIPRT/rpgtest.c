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
#include    "rpgtest.h"


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

#define  PRT_Q_NAME             "PrtPg Test"

#define  DLL_NAME_PARM          1
#define  CRITERIA_PARM          2
#define  QUERY_ARRAY_SIZE       (2 * SIZE_OF_QUERY)




/* ======================================================================= *\
 *                                             MODULE'S GLOBAL VARIABLES   *
\* ======================================================================= */
static  CHAR  szCompileDateTime[] = __DATE__ " " __TIME__ ;
static CHAR szDllVersion[] = "1.1a0 rpgtest.dll";

static  CHAR  szCurrentID [SMARRAY];

static  CHAR  szBaseQuery [SIZE_OF_QUERY + 1];
static  CHAR  szQuery     [QUERY_ARRAY_SIZE + 1];
static  CHAR  szWhereConj [SIZE_OF_CONJ + 1];
static  CHAR  szKey       [SMARRAY + 1];


static  CHAR szFailedDLL [CCHMAXPATH + 1];
static  CHAR szErrorMsg  [1024] = "";                                         
                
           /* Title strings for this module. */
static  REPTITLES rept;
static  PREPT     prept = &rept;



/*
 * hmodModule is the handle to this DLL
 */

HMODULE hmodModule;



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




USHORT SetColumnTitles (VOID)
   {
   return (0);
   }/* END OF FUNCTION SetColumnTitles */



USHORT PrintProc (HWND hwnd, MPARAM mp1, MPARAM mp2, PSZ pszCmdLn, USHORT usLen)
   {
   USHORT us    = 0;

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
   DBGMSG(" *** In DLL version check function.");

   if ( ItiVerCmp(szDllVersion) )
      return (TRUE);
   return (FALSE);
   }


VOID EXPORT ItiDllQueryMenuName (PSZ     pszBuffer,
                                 USHORT  usMaxSize,
                                 PUSHORT pusWindowID)
   {
   ItiStrCpy (pszBuffer, "~Page Test", usMaxSize);
   *pusWindowID = rCodeTab_RptDlgBox;
   }




BOOL EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID,
                                     USHORT usActiveWindowID)
   {
      return (FALSE);
   }








MRESULT EXPORT ItiDllCodeTabRptDlgProc (HWND     hwnd,
                                        USHORT   usMessage,
                                        MPARAM   mp1,
                                        MPARAM   mp2)
   {
   USHORT us = 0;

   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);

   }/* END OF FUNCTION ItiDllCodeTabRptDlgProc */








static BOOL bAlreadyRegistered = FALSE;

/* the variable below tells the linker not to link with the 
   C run time library.  This is only need for the following case: if
   you don't use the C run time library, and you have no static variables. */
/* int _acrtused = 0; */



/*
 * DllInit is called by either ITIDLLI.OBJ or ITIDLLC.OBJ.  This functino
 * must return non-zero if the initialization succeded, or FALSE
 * if initialization failed.
 */

BOOL DLLINITPROC DllInit (HMODULE hmodDll)
   {
   /* module initialization functions. */

   hmodModule = hmodDll;
   return TRUE;
   }






BOOL EXPORT ItiDllInitDll (VOID)
   {
   return TRUE;
   }








USHORT EXPORT ItiRptDLLPrintReport
                 (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[])
   {
   PSZ     pszKey = szKey;

           /* General return code variable: */
   USHORT  usNum, us = 0;

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
   USHORT  col, row;
   PSZ   * ppsz   = NULL;
   CHAR    szDest[5] = "";


   /* ------------------------------------------------------------------- *\
    *  Create a memory heap.                                              *
   \* ------------------------------------------------------------------- */
   hheap = ItiMemCreateHeap (MIN_HEAP_SIZE);



   /* ------------------------------------------------------------------- *\
    * Initialize the strings.                                             *
   \* ------------------------------------------------------------------- */
   ItiStrCpy(szCurrentID,"TABLE_ID",sizeof(szCurrentID));


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



   /* -- Set the Header. */
   ItiPrtChangeHeaderLn(ITIPRT_TITLE, NULL);


   usCurrentPg = ItiPrtQueryCurrentPgNum(); /* init the page number.  */
   usNum = ItiPrtQueryTotalNumDisplayLns();

   for (row=0; row<usNum; row++)
      {
      for (col=0; col<80; col++)
         {
         us = col % 10;

         if (us == 0)
            ItiPrtPutTextAt (CURRENT_LN, col,
                             LEFT, USE_RT_EDGE, "0");
         else
            {
            ItiStrUSHORTToString (us, szDest, 2);

            ItiPrtPutTextAt (CURRENT_LN, col,
                             LEFT, USE_RT_EDGE, szDest);
            }
         }

      if (row < (usNum-1))
         ItiPrtIncCurrentLn(); // line feed
      }


                                
      
               
   
      


   /* ------------------------------------------------------------------- *\
    * Finish up and quit.                                                 *
   \* ------------------------------------------------------------------- */
   ItiPrtEndReport();

   DBGMSG("Exit ***DLL function ItiRptDLLPrintReport");
   return 0;

   }/* END OF FUNCTION ItiRptDLLPrintReport */

