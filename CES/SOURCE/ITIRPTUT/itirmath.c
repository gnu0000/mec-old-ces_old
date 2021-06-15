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


/* ItiRMath.exe
 * 7 Oct 1991
 *
 * This module is used to circumvent the current limitation of the
 * ItiDBase module that one may not get a second database connection
 * while a query is still pending.  Often times for a report numbers
 * from a result row need to be added to a running total before
 * proceeding and getting the next row; recall that we currently have
 * SQL do all the arithmitic to maintain the same level of percision.
 *
 * The command line syntax is:
 *
 *  param#1    "Query"       The SQL query expression; ie. "select 2 + 2"
 *  param#2    szName        Temporary file name for shared memory.
 *  param#3    usFormat      Result format.
 *  param#4    szPath&Name   Path and file name for shared memory,
 *                           (same name as above).
 */


#define     INCL_ERRORS 
#define     INCL_BASE
#define     INCL_WIN
#define     INCL_DOS
#define     INCL_DEV
#define     INCL_GPI


#include    "..\include\iti.h"     

#include    "..\include\itibase.h"
#include    "..\include\itiutil.h"
#include    "..\include\itierror.h"     
#include    "..\include\itidbase.h"
#include    "..\include\itimbase.h"
#include    "..\include\itifmt.h"
#include    "..\include\itiglob.h"
#include    "..\include\itiwin.h"
#include    "..\include\colid.h"


#include    "..\include\itirpt.h"


#if !defined (INCL_RPTUTIL)
#include "..\include\itirptut.h"
#endif



/* -- argv[] parameter ordering (Do NOT change!). */
#define  EXPR_QUERY  1
#define  F_NAME      2
#define  EXPR_FMT    3
#define  MEM_NAME    4


             
/* ======================================================================= *\
 *                                             MODULE'S GLOBAL VARIABLES   *
\* ======================================================================= */
static CHAR    szCompileDateTime[] = __DATE__ " " __TIME__ ;
static INT     iLevel = 0;
static HAB     hab;
static HMQ     hmq;
static HMODULE hmod;

 /* Used by DbExecQuer & DbGetQueryRow functions: */
static  HQRY    hqry   = NULL;
static  USHORT  usResId   = ITIRID_RPT;
static  USHORT  usId      = 0;
static  USHORT  usNumCols = 0;
static  USHORT  usErr     = 0;
static  PSZ   * ppsz   = NULL;

static USHORT (EXPORT *pfnRptFunc) (HAB, HMQ, INT, CHAR **);
static BOOL   (EXPORT *fnVersion)  (USHORT);





/*
 *  OptionSwitch
 *     Processes a command line option switch.
 */
USHORT OptionSwitch (PSZ pszOptSwitch)
   {

   switch(pszOptSwitch[1])
      {
      case 'h':
      case 'H':
      case '?':
         /* Usage. */
         return 0;
         break;

      default:
         break;
      }
   return 0;
   }








/* ======================================================================= *\
\* ======================================================================= */
USHORT _cdecl main (int argc, char * argv[])
   {
   USHORT us = 0;
   USHORT usRowCnt = 0;
   USHORT usFmtNum = 0;
   INT    i = 0;
   CHAR   szErrorMsg [128] = "";
   HHEAP  hhpUt = NULL;
   SEL    selResultMem;
   PSZ    pszResultMem  = NULL;
   PSZ    pszProblemChr = NULL;


   if (argc < 5) 
      return ITIRPT_ERR;



#ifdef DEBUG
   ItiRptUtErrorMsgBox(NULL,"Parameters received by the ItiRMath module: ");

   i = EXPR_QUERY; /* argv[EXPR_QUERY] should be the the math expression string. */
   while (i < argc) /* -- Check for and process any relevent options. */
      {
      if (i != EXPR_FMT)
         ItiRptUtErrorMsgBox(NULL,argv[i]);
      else
         {
         ItiStrCpy(szErrorMsg," SQL math result format:  ",sizeof szErrorMsg);

         switch(argv[EXPR_FMT][3])
            {
            case '0':
               ItiStrCat (szErrorMsg,STR_EXP, sizeof szErrorMsg);
               break;

            case '1':
               ItiStrCat (szErrorMsg,STR_EXP_2, sizeof szErrorMsg);
               break;

            case '2':
               ItiStrCat (szErrorMsg,STR_EXP_2D, sizeof szErrorMsg);
               break;

            case '3':
               ItiStrCat (szErrorMsg,STR_EXP_2DC, sizeof szErrorMsg);
               break;

            case '4':
               ItiStrCat (szErrorMsg,STR_EXP_10, sizeof szErrorMsg);
               break;

            case '5':
               ItiStrCat (szErrorMsg,STR_KEY, sizeof szErrorMsg);
               break;

            case '6':
               ItiStrCat (szErrorMsg,STR_EXP_3DC, sizeof szErrorMsg);
               break;

            case '7':
               ItiStrCat (szErrorMsg,STR_EXP_4DC, sizeof szErrorMsg);
               break;


            default:
               ItiStrCat (szErrorMsg, argv[i], sizeof szErrorMsg);
               break;
            }

         ItiRptUtErrorMsgBox(NULL,szErrorMsg);
         szErrorMsg[0] = '\0';
         }

      if ((argv[i][0] == '/')||(argv[i][0] == '-'))
         {
         us = OptionSwitch(argv[i]);
         }

      i++; /* increment loop counter */
      } /* end of while (i... */
#endif


   /* -- Check that the given query was not built with an error result. */
   pszProblemChr = ItiStrChr (argv[EXPR_QUERY], '?');
   if (pszProblemChr != NULL)
      {
      ItiRptUtErrorMsgBox(NULL, "*** Query expression ERROR in ItiRMath: ");
      *pszProblemChr = '0';
      ItiRptUtErrorMsgBox(NULL,argv[EXPR_QUERY]);
      }




   hab = WinInitialize(0);
   if (hab == NULL)           
      {
      ItiRptUtErrorMsgBox(NULL,"*ERR: ItiRMath function:  FAILED to get hab.");
      return (ITIRPT_ERR);
      }

   hmq = WinCreateMsgQueue(hab, DEFAULT_QUEUE_SIZE);
   if (hmq == NULL)           
      {
      ItiRptUtErrorMsgBox(NULL,"*ERR: ItiRMath function:  FAILED to get hmq.");
      return (ITIRPT_ERR);
      }

   if (!ItiFmtInitialize ())
      {
      ItiRptUtErrorMsgBox(NULL,"*ERR: ItiRMath could not initialize the format module!\n");
      return (ITIRPT_ERR);
      }

   if (!ItiDbInit (0, 2))
      {
      ItiRptUtErrorMsgBox(NULL,"*ERR: ItiRMath could not initialize the database module!\n");
      return (ITIRPT_ERR);
      }



   us = DosGetShrSeg(argv[MEM_NAME], &selResultMem);
   if (us != 0)
      {
      ItiRptUtErrorMsgBox(NULL,"*ERR: ItiRMath could not get the shared memory selector!\n");
      return (ITIRPT_ERR);
      }
   else
      {
      pszResultMem = MAKEP(selResultMem, 0);
      }




   /* -- Create a memory heap. */
   hhpUt = ItiMemCreateHeap (MIN_HEAP_SIZE);

   ItiStrToUSHORT(argv[EXPR_FMT], &usFmtNum);

   /* -- Do the query. */
   hqry = ItiDbExecQuery (argv[EXPR_QUERY], hhpUt, hmod, ITIRID_RPT,
                          usFmtNum, &usNumCols, &usErr);
                    /* [EXPR_QUERY] is the expression string "select 2 + 2" */
                    /* [EXPR_FMT] is the format */
   if (hqry == NULL)
      {
      ItiStrCpy(szErrorMsg,"ERROR: Failed to exec ItiRptUtMath query:\n\r   ",
                 sizeof szErrorMsg);
      ItiStrCat (szErrorMsg, argv[EXPR_QUERY], sizeof szErrorMsg);
      ItiRptUtErrorMsgBox(NULL, szErrorMsg);
      ItiMemDestroyHeap (hhpUt);
      return (ITIRPT_ERR);
      }
   else
      {
      /* -- Write the result data for the job key; should be only 1 row. */
      usRowCnt = 0;
      while( ItiDbGetQueryRow (hqry, &ppsz, &usErr) )
         {
         usRowCnt++;
         if (usRowCnt == 1)
            {
            ItiStrCpy (&pszResultMem[1], ppsz[0], (RESULT_SIZE-1));
            ItiStrCpy (szErrorMsg," ItiRMath module result: ", sizeof szErrorMsg);
            ItiStrCat (szErrorMsg, ppsz[0], sizeof szErrorMsg);
            ItiRptUtErrorMsgBox(NULL, szErrorMsg);
            }
         else
            {
            ItiRptUtErrorMsgBox(NULL,"PANIC: Multiple ItiRMath result rows.");
            }

         ItiFreeStrArray (hhpUt, ppsz, usNumCols);
         }

      }/* end else hqry == null */

   ItiMemDestroyHeap (hhpUt);


   /* -- Now let the user process know that we are done. */
   pszResultMem[0] = ' ';


   WinDestroyMsgQueue(hmq);
   WinTerminate(hab);

   return (us);
   }/* END OF main */

