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


/*
 * dprof.c
 *
 * Mark Hampton
 * Timothy Blake
 *
 * This is the dss parametric profile analysis model.
 */


#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"
#include "..\include\itimbase.h"
#include "..\include\itidbase.h"
#include "..\include\colid.h"
#include "..\include\itiglob.h"
#include "..\include\itirptdg.h"

#include "..\include\itirpt.h"
#include <string.h>
#include <stdio.h>
#include <share.h>
#include "dprof.h"

#include "..\include\itiimp.h"








/* ======================================================================= *\
 *                                             MODULE'S GLOBAL VARIABLES   *
\* ======================================================================= */

/*
 * hmod is the handle to this DLL
 */

HMODULE hmod = -1;
PGLOBALS pglobals = NULL;

static  BOOL bAlreadyRegistered = FALSE;

static  CHAR     szCompileDateTime[] = __DATE__ " " __TIME__ ;
static CHAR szDllVersion[] = "1.1a0 dprof.dll";

static  CHAR     szErrorMsg  [1020] = "";
static  COLTTLS  acttl[7 + 1];

static  USHORT   usNumOfTitles;
static  USHORT   usCurrentPg = 0;
static  USHORT   usErr = 0;
static  USHORT   usCount = 0;




static USHORT SetTitles (PREPT preptitles, INT argcnt, CHAR * argvars[])
   {
   USHORT us = 1;
   CHAR szOpt[FOOTER_LEN + 1];
   PSZ  pszOpt = szOpt;

   if (preptitles == NULL)
      return ITIRPT_INVALID_PARM;

   ItiStrCpy (preptitles->szTitle,
              TITLE,
              sizeof preptitles->szTitle);

   //ItiStrCpy (preptitles->szSubTitle, "", sizeof preptitles->szSubTitle);
   preptitles->szSubTitle [0] = '\0';
   preptitles->szSubTitle [1] = '\0';

   //ItiStrCpy (preptitles->szFooter, "", sizeof preptitles->szFooter);
   preptitles->szFooter [0] = '\0';
   preptitles->szFooter [1] = '\0';

   /* -- Set the subsubtitle text to be empty for this report. */
   preptitles->szSubSubTitle [0] = '\0';
   preptitles->szSubSubTitle [1] = '\0';

   /* -- Check for use of footer override option. */
   ItiRptUtGetOption (ITIRPTUT_FOOTER_SWITCH,
                      preptitles->szFooter, sizeof preptitles->szFooter,
                      argcnt, argvars);


   return 0;
   }





static USHORT SetColumnTitles (void)
   {
   USHORT us;

   for (us = 1; us <= 5; us++)
      {
      ItiPrtPutTextAt (CURRENT_LN, acttl[us].usTtlCol,
                       LEFT, (acttl[us].usTtlColWth | REL_LEFT),
                       acttl[us].szTitle);

      if (us == RPT_TTL_4)
         ItiPrtIncCurrentLn ();
      }

   ItiPrtIncCurrentLn ();
   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   ItiPrtIncCurrentLn ();

   return 0;
   }



static PSZ GetKeyList (HHEAP hheap, PSZ pszWhere, PSZ pszClause)
   {
   PSZ pszStart, pszEnd;
   char c;

   pszStart = strstr (pszWhere, pszClause);
   if (pszStart)
      {
      while (*pszStart && *pszStart != '(')
         pszStart++;

      for (pszEnd = pszStart; *pszEnd && *pszEnd != ')'; pszEnd++)
         ;
      c = *pszEnd;
      *pszEnd = '\0';
      pszStart = ItiStrDup (hheap, pszStart);
      *pszEnd = c;
      return pszStart;
      }
   return NULL;
   }


static PSZ GetProposalTotal (HHEAP hheap, PSZ pszWhere)
   {
   PSZ      pszTotal, psz, pszEnd = NULL;
//   CHAR     c;
   BOOL     bFirst;
   CHAR     szQuery [4000] = "";
   INT      i = 0;

   /* get the total for the proposals */
   ItiMbQueryQueryText (hmod, ITIRID_RPT, GET_PROPOSAL_TOTAL,
                        szQuery, sizeof szQuery);

   psz = pszWhere;
   // bFirst = TRUE;
   bFirst = FALSE;

//   while (psz && (psz = strstr (psz, "HP.")))
//      {
//      if (bFirst)
//         {
//         ItiStrCat (szQuery, " WHERE ", sizeof szQuery);
//         bFirst = FALSE;
//         }
//      else
//         {
//         ItiStrCat (szQuery, " AND ", sizeof szQuery);
//         }
//      if (pszEnd = strstr (psz, "AND"))
//         {
//         c = *pszEnd;
//         *pszEnd = '\0';
//         }
//
//      ItiStrCat (szQuery, psz, sizeof szQuery);
//
//      if (pszEnd)
//         *pszEnd = c;
//      psz = pszEnd;
//      }

   i = ItiStrLen(szQuery);

   if (NULL != pszWhere)
      ItiStrCat (szQuery, pszWhere, (sizeof szQuery) - i );

   i = ItiStrLen(szQuery);
   ItiStrCat (szQuery, " ) ", (sizeof szQuery) - i );

   pszTotal = ItiDbGetRow1Col1 (szQuery, hheap, 0, 0, 0);
   if (pszTotal != NULL)
      ItiExtract (pszTotal, ",");
   return pszTotal;
   }



static USHORT MajorItemList (HHEAP  hheap,
                             FILE   *pf,
                             PSZ    pszProfileID,
                             PSZ    pszWhere)
   {
   USHORT   usLn;
   HQRY     hqry;
   USHORT   usNumCols;
   PSZ   *ppsz, pszDesc, pszSpYr, pszPropTotal, apszTokens[2], apszValues[2];
   CHAR  szTemp [2040] = "";
   CHAR  szQuery [6000] = "";
   CHAR  szMajItmID [15] = "";
   BOOL  bFree = TRUE;

   pszPropTotal = GetProposalTotal (hheap, pszWhere);
   if (ItiStrIsBlank (pszPropTotal))
      {
      pszPropTotal = " 1.0 ";
      bFree = FALSE;
      // return -1;
      }

   apszTokens [0] = "ProposalTotal";
   apszValues [0] = pszPropTotal;
   apszTokens [1] = "Where";

   if (NULL != pszWhere)
      apszValues [1] = pszWhere;
   else
      apszValues [1] = "  ";


   /* get the major item listing query */
   ItiMbQueryQueryText (hmod, ITIRID_RPT, GET_MAJORITEM_PERCENTS,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery,
                        apszTokens, apszValues, 2);

   /* -- Do the database query. */
   hqry = ItiDbExecQuery (szQuery, hheap, hmod, ITIRID_RPT,
                          GET_MAJORITEM_PERCENTS, &usNumCols, &usErr);

   /* -- Process the query results for each row. */
   if (hqry == NULL)
      {
      return -1;
      }

   ItiCSVWriteLine (pf, "SignificantMajorItem", "ProfileKey",
                    "MajorItemKey", "PercentofValue", "PercentofValueLock",
                    NULL);

   szTemp [0] = '\0';
   usLn = 6; /* First time thru, column titles and close total lines. */
   while (ItiDbGetQueryRow (hqry, &ppsz, &usErr))
      {
      /* -- Check for new page. */
      if (ItiPrtKeepNLns (usLn) == PG_WAS_INC ||
          ItiPrtQueryCurrentPgNum() != usCurrentPg)
         {
         usCurrentPg = ItiPrtQueryCurrentPgNum();
         ItiPrtChangeHeaderLn (ITIPRT_NEWPG, "");
         SetColumnTitles ();
         }

      /* -- Display info. */
      /* Major Item ID: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_1].usDtaCol,
                       LEFT, (acttl[RPT_TTL_1].usDtaColWth | REL_LEFT),
                       ppsz[0]);

      /* Proposal Total: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_2].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_2].usDtaColWth | REL_LEFT),
                       ppsz[1]);

      /* Major Item Total: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_3].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_3].usDtaColWth | REL_LEFT),
                       ppsz[2]);

      /* % of Value: */
      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_4].usDtaCol,
                       RIGHT, (acttl[RPT_TTL_4].usDtaColWth | REL_LEFT),
                       ppsz[3]);

      ItiPrtIncCurrentLn();

      /* Description: */
      /* -- the following is done to circumvent an MS-SQL bug. */
      ItiStrCpy (szTemp,
                 "select Description from MajorItem where MajorItemKey = ",
                 sizeof szTemp);
      ItiStrCat (szTemp, ppsz[4], sizeof szTemp); /* Append HPMI.MajorItemKey */
      pszDesc = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);
      if (pszDesc == NULL)
         pszDesc = "-Blank-";

      ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_5].usDtaCol,
                       LEFT, (acttl[RPT_TTL_5].usDtaColWth | REL_LEFT),
                       pszDesc);
                       //  ppsz[4]);  //MS-SQL bug

      ItiPrtIncCurrentLn();


      /* -- Build "MajorItemKey" ie. MajorItemID & SpecYear */
      ItiStrCpy (szTemp,
                 "select SpecYear from MajorItem where MajorItemKey = ",
                 sizeof szTemp);
      ItiStrCat (szTemp, ppsz[4], sizeof szTemp); /* Append HPMI.MajorItemKey */
      pszSpYr = ItiDbGetRow1Col1 (szTemp, hheap, 0, 0, 0);
      if ((pszSpYr == NULL) || ((pszSpYr != NULL) && (pszSpYr[0] == '\0')) )
         pszSpYr = "NULL";

      ItiStrCpy (szMajItmID, ppsz[0], sizeof szMajItmID);
      ItiStrCat (szMajItmID, " ", sizeof szMajItmID);
      ItiStrCat (szMajItmID, pszSpYr, sizeof szMajItmID);  // SpecYear

      ItiCSVWriteLine (pf, "SignificantMajorItem",
                       pszProfileID,
                    //   ppsz [0], /* major item id */
                       szMajItmID, /* major item id and spec year */
                       ppsz [5], /* percent of value */
                       "N",    /* percent of value lock {format: Flag,N,Y} */
                       NULL);

      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }/* end of while */

   /* get the major item total query */
   ItiMbQueryQueryText (hmod, ITIRID_RPT, GET_MAJORITEM_TOTAL,
                        szTemp, sizeof szTemp);
   ItiStrReplaceParams (szQuery, szTemp, sizeof szQuery,
                        apszTokens, apszValues, 2);

   /* -- Do the database query. */
   ppsz = ItiDbGetRow1 (szQuery, hheap, hmod, ITIRID_RPT,
                        GET_MAJORITEM_TOTAL, &usNumCols);

   /* -- Do Totals line here. */
   ItiPrtIncCurrentLn (); //blank line
   ItiPrtPutTextAt (CURRENT_LN, acttl[6].usTtlCol,
                    LEFT, (acttl[6].usTtlColWth | REL_LEFT),
                    acttl[6].szTitle);

   /* Proposal Total: */
   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_2].usDtaCol,
                    RIGHT, (acttl[RPT_TTL_2].usDtaColWth | REL_LEFT),
                    ppsz [0]);

   /* Major Item Total: */
   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_3].usDtaCol,
                    RIGHT, (acttl[RPT_TTL_3].usDtaColWth | REL_LEFT),
                    ppsz [1]);

   /* % of Value: */
   ItiPrtPutTextAt (CURRENT_LN, acttl[RPT_TTL_4].usDtaCol,
                    RIGHT, (acttl[RPT_TTL_4].usDtaColWth | REL_LEFT),
                    ppsz [2]);
   ItiPrtIncCurrentLn ();

   ItiFreeStrArray (hheap, ppsz, usNumCols);

   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   ItiPrtIncCurrentLn ();

   if (bFree  && (NULL != pszPropTotal))
      ItiMemFree (hheap, pszPropTotal);

   return usErr;
   }







/***************************************************************************
 * Analysis
 **************************************************************************/



static void CreateProfile (HHEAP  hheap,
                           PSZ    pszProfileID,
                           PSZ    pszDescription,
                           PSZ    pszWhere,
                           BOOL   bAuto)
   {
   CHAR   szFileName [256] = "";
   CHAR szXlatedWhere [1000] = ""; 
   USHORT usLn = 10;
   FILE   *pf;

   sprintf (szFileName, "dpro%.4x.csv", usCount);
   pf = _fsopen (szFileName, "wt", SH_DENYWR);
   if (pf == NULL)
      return;

   fprintf (pf, ";created by dprof model, compiled on %s\n", 
            szCompileDateTime);
   fprintf (pf, ";Model run on %.2d/%.2d/%.4d %.2d:%.2d%cm.\n",
            (USHORT) pglobals->pgis->month,
            (USHORT) pglobals->pgis->day,
            (USHORT) pglobals->pgis->year,
            (USHORT) pglobals->pgis->hour % 12,
            (USHORT) pglobals->pgis->minutes,
            pglobals->pgis->hour > 12 ? (char) 'p' : (char) 'a');

   ItiCSVWriteLine (pf, "DS/Shell Import File", "1.00", NULL);
   ItiCSVWriteLine (pf, "ParametricProfile", "ProfileID", "Description", NULL);
   ItiCSVWriteLine (pf, "ParametricProfile", pszProfileID, pszDescription, NULL);

   /* -- Check for new page. */
   if ((ItiPrtKeepNLns(usLn) == PG_WAS_INC)
       || (ItiPrtQueryCurrentPgNum() != usCurrentPg ))
      {
      usCurrentPg = ItiPrtQueryCurrentPgNum();
      ItiPrtChangeHeaderLn(ITIPRT_NEWPG, "");
      }

   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL, LEFT, 8 | REL_LEFT, "Profile:");
   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1, LEFT, 16 | REL_LEFT,
                    pszProfileID);
   if (bAuto)
      ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1, LEFT, 50 | REL_LEFT,
                       "(Automatically generated by work type)");
   ItiPrtIncCurrentLn ();

   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 9, LEFT | WRAP,
                    1 | FROM_RT_EDGE, pszDescription);
   ItiPrtIncCurrentLn ();

   /* -- Translate selection clause key values to IDs here. */
   ItiDbXlateKeys (hheap, pszWhere, szXlatedWhere, sizeof szXlatedWhere);

   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL, LEFT, 17 | REL_LEFT, "Selection Clause:");
   //ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1, LEFT | WRAP,
   //                 1 | FROM_RT_EDGE, pszWhere);
   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1, LEFT | WRAP, 
                      1 | FROM_RT_EDGE, szXlatedWhere);

   ItiPrtIncCurrentLn ();
   ItiPrtIncCurrentLn ();

   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL, LEFT, 17 | REL_LEFT, "Output file name:");
   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + 1, LEFT | WRAP,
                    1 | FROM_RT_EDGE, szFileName);
   ItiPrtIncCurrentLn ();

   ItiPrtDrawSeparatorLn (CURRENT_LN, 0, USE_RT_EDGE);
   ItiPrtIncCurrentLn ();

   SetColumnTitles();

   MajorItemList (hheap, pf, pszProfileID, pszWhere);
   fclose (pf);
   usCount++;
   return;
   }

/***************************************************************************
 * Main loop
 **************************************************************************/

static void GetInfo (PSZ      pszFile,
                     BOOL     bMakeWorkType,
                     PSZ      pszProfileID,
                     USHORT   usProfileID,
                     PSZ      pszDescription,
                     USHORT   usDescription,
                     PSZ      pszWhere,
                     USHORT   usWhere)
   {
   FILE     *pf;
   USHORT   i;

   pszProfileID [0] = '\0';
   pszDescription [0] = '\0';
   pszWhere [0] = '\0';

   if (pszFile && *pszFile)
      {
      pf = _fsopen (pszFile, "rt", SH_DENYWR);
      if (pf == NULL)
         return;
      fgets (pszProfileID, usProfileID, pf);
      i = ItiStrLen (pszProfileID) - 1;
      if (pszProfileID [i] == '\n')
         pszProfileID [i] = ' ';
      else if (bMakeWorkType)
         ItiStrCat (pszProfileID, " ", usProfileID);

      fgets (pszDescription, usDescription, pf);
      i = ItiStrLen (pszDescription) - 1;
      if (pszDescription [i] == '\n')
         pszDescription [i] = ' ';
      else if (bMakeWorkType)
         ItiStrCat (pszDescription, " ", usDescription);

      fgets (pszWhere, usWhere, pf);
      i = ItiStrLen (pszWhere) - 1;
      if (pszWhere [i] == '\n')
         pszWhere [i] = ' ';

      fclose (pf);

      if (ItiStrIsBlank (pszProfileID))
         ItiStrCpy (pszProfileID, "Work Type ", usProfileID);
      if (ItiStrIsBlank (pszDescription))
         ItiStrCpy (pszDescription, "System generated profile for Work Type ",
                    usDescription);
      }
   else if (!bMakeWorkType)
      {
      sprintf (pszProfileID, "%.4d%.2d%.2d-%.2d%.2d%.2d",
               (USHORT) pglobals->pgis->year,
               (USHORT) pglobals->pgis->month,
               (USHORT) pglobals->pgis->day,
               (USHORT) pglobals->pgis->hour,
               (USHORT) pglobals->pgis->minutes,
               (USHORT) pglobals->pgis->seconds);

      sprintf (pszDescription,
               "System generated profile.  Created on "
               "%.2d/%.2d/%.4d %.2d:%.2d%cm.",
               (USHORT) pglobals->pgis->month,
               (USHORT) pglobals->pgis->day,
               (USHORT) pglobals->pgis->year,
               (USHORT) pglobals->pgis->hour % 12,
               (USHORT) pglobals->pgis->minutes,
               pglobals->pgis->hour > 12 ? (char) 'p' : (char) 'a');
      }
   else
      {
      ItiStrCpy (pszProfileID, "Work Type ", usProfileID);
      ItiStrCpy (pszDescription, "System generated profile for Work Type ",
                 usDescription);
      }
   }



static void ParametricProfileAnalysis (HHEAP hheap,
                                       BOOL  bCreateWorkTypes,
                                       PSZ   pszInfoFile)
   {
   char     szProfileID [17], szDescription [257], szWhere [2040];
   char     szProfileID2 [17], szDescription2 [257], szWhere2 [2040];
   USHORT   i, j;
   USHORT   usLn = 10;
   char     szQuery [1020] = "";
   HQRY     hqry;
   PSZ      *ppsz;

   GetInfo (pszInfoFile, bCreateWorkTypes, szProfileID, sizeof szProfileID,
            szDescription, sizeof szDescription, szWhere, sizeof szWhere);

   if (bCreateWorkTypes)
      {
      i = ItiStrLen (szProfileID);
      if (i >= 11)
         szProfileID [10] = '\0';

      ItiMbQueryQueryText (hmod, ITIRID_RPT, LIST_WORK_TYPES,
                           szQuery, sizeof szQuery);
      hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &i, &j);
      if (hqry == NULL)
         {
         return;
         }
      while (ItiDbGetQueryRow (hqry, &ppsz, &j))
         {
         ItiStrCpy (szProfileID2, szProfileID, sizeof szProfileID2);
         ItiStrCat (szProfileID2, ppsz [0], sizeof szProfileID2);

         ItiStrCpy (szDescription2, szDescription, sizeof szDescription2);
         ItiStrCat (szDescription2, ppsz [0], sizeof szDescription2);

         ItiStrCpy (szWhere2, szWhere, sizeof szWhere2);
         ItiStrCat (szWhere2, " AND WorkType = ", sizeof szWhere2);
         ItiStrCat (szWhere2, ppsz [1], sizeof szWhere2);

         CreateProfile (hheap, szProfileID2, szDescription2, szWhere2, TRUE);

         ItiFreeStrArray (hheap, ppsz, i);
         }
      }
   else
      {
      CreateProfile (hheap, szProfileID, szDescription, szWhere, FALSE);
      }
   }



/* ======================================================================= *\
 *                                                    EXPORTED FUNCTIONS   *
\* ======================================================================= */
/* -- ItiDllQueryVersion returns the version number of this DLL. */
USHORT EXPORT ItiDllQueryVersion (VOID)
   {
   return DLL_VERSION;
   }


/*
 * ItiDllQueryCompatibility returns TRUE if this DLL is compatable
 * with the caller's version numer, otherwise FALSE is returned.
 */

BOOL EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion)

   {
   if ( ItiVerCmp(szDllVersion) )
      return TRUE;
   return FALSE;
   }





USHORT EXPORT ItiRptDLLPrintReport (HAB      hab,
                                    HMQ      hmq,
                                    int      iArgCount,
                                    CHAR     *ppszArgs [])
   {
   int         i = 0;
   HHEAP       hheap;
   REPTITLES   rept;
   BOOL        bCreateWorkTypes = FALSE;
   char        szKeyFile [256];

   /* ------------------------------------------------------------------- *\
    *  Create a heap.                                                     *
   \* ------------------------------------------------------------------- */
   hheap = ItiMemCreateHeap (MIN_HEAP_SIZE);


   /* ------------------------------------------------------------------- *\
    * Do any run-time formatting then start the report.                   *
   \* ------------------------------------------------------------------- */

   /* -- Get the report's column titles for this DLL. */
   ItiRptUtLoadLabels (hmod, &usNumOfTitles, &acttl[0]);

   SetTitles (&rept, iArgCount, ppszArgs);

   i =  ItiPrtLoadDLLInfo (hab, hmq, &rept, iArgCount, ppszArgs);
   if (i != 0 )
      {
      DBGMSG("FAILED to ItiPrtDLLLoadInfo.");
      return ITIPRT_NOT_INITIALIZED;
      }

   ItiPrtBeginReport ("Parametric Profile Analysis");

   szKeyFile [0] = '\0';
   for (i=0; i < iArgCount; i++)
      {
      bCreateWorkTypes |= ItiStrICmp (ppszArgs [i], "-worktype") == 0;
      if (ItiStrICmp (ppszArgs [i], "-modelinfo") == 0 && i+1 < iArgCount)
         ItiStrCpy (szKeyFile, ppszArgs [i+1], sizeof szKeyFile);
      }

   ParametricProfileAnalysis (hheap, bCreateWorkTypes, szKeyFile);

   /* ------------------------------------------------------------------- *\
    * Finish up and quit.                                                 *
   \* ------------------------------------------------------------------- */
   ItiPrtEndReport ();

   return 0;

   }/* END OF FUNCTION ItiRptDLLPrintReport */








/*
 * DllInit is called by either ITIDLLI.OBJ or ITIDLLC.OBJ.  This functino
 * must return non-zero if the initialization succeded, or FALSE
 * if initialization failed.
 */



BOOL DLLINITPROC DllInit (HMODULE hmodDll)
   {
   /* module initialization functions. */
   hmod = hmodDll;
   return TRUE;
   }





BOOL EXPORT ItiDllInitDll (VOID)
   {
   if (bAlreadyRegistered)
      return TRUE;

   pglobals = ItiGlobQueryGlobalsPointer ();
   bAlreadyRegistered = TRUE;
   return TRUE;
   }



