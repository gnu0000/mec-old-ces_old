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
 * JobPath.c
 * (C) 1991 AASHTO
 *
 * This module calculates the job schedule
 *
 * Craig
 *
 */


#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itiutil.h"
#include "..\include\itimap.h"
#include "..\include\itifmt.h"
#include "..\include\winid.h"
#include "..\include\colid.h"
#include "..\include\itiglob.h"
#include "..\include\dialog.h"
#include "bamsces.h"
#include "job.h"
#include "menu.h"
#include "dialog.h"
#include "jobpath.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*--- these are really defined in the format module ---*/
#define SS   1 
#define SF   2
#define FS   3
#define FF   4

#define MEMORY_ERROR    1
#define DATABASE_ERROR  2
#define PATH_ERROR      3
#define NO_MORE_LINKS   4

#define MAX_LINKS       20
#define MAX_JOBS        200
#define MAX_LEVELS      999



typedef struct jinode   *PJINODE;
typedef struct pjnode   *PPJNODE;
typedef struct pjlink   *PPJLINK;

typedef struct
   {
   USHORT   uSuccessor;     // 2
   USHORT   uLink;          // 2
   USHORT   uLag;           // 2
   USHORT   uDelay;         // 2 8
   } LINK;
typedef LINK *PLINK;



typedef struct jinode
   {
   PSZ      pszJIK;         // 4
   USHORT   uDuration;      // 2
   USHORT   uDaysTillStart; // 2
   USHORT   uSlack;         // 2
   USHORT   uLinks;         // 2
   PLINK    plink;          // 4
   CHAR     cFlag;          // 1 18
   } JINODE;

typedef struct pjlink
   {
   PPJNODE ppjJobLink;
   USHORT  usLevel;
   USHORT  usLink;
   USHORT  usLag;
   LONG    lDelay;
   } PJLINK;

typedef struct pjnode
   {
   PSZ     pszJobKey;
   LONG    lWorkBeginDate;
   LONG    lWorkEndDate;
   USHORT  usPriority;
   USHORT  usDuration;
   USHORT  usSlack;
   USHORT  usFwdLinks;
   USHORT  usBwdLinks;
   PPJLINK ppjBackward;
   PPJLINK ppjForward;
   BOOL    bDateCalced;
   } PJNODE;


#define PutInPath(jin)        ((jin).cFlag != 0x0002)
#define RemoveFromPath(jin)   ((jin).cFlag &= ~0x0002)
#define IsInPath(jin)         (!!((jin).cFlag & 0x0002))

extern WORKDAYS  WorkDays;


PJINODE BuildScheduleTree (HHEAP hheap, PSZ pszJobKey)
   {
   char     szTmp[300] = "";
   PSZ      *ppszTmp;
   PJINODE  pjin = NULL;
   USHORT   uAllocSize = 0;
   USHORT   uNodes = 0;
   USHORT   uCols, uErr, uPred, uSucc, i;
   HQRY     hqry;

   /*--- First we must build the job item nodes ---*/

   sprintf (szTmp, " SELECT JobItemKey, Duration"
                   " FROM JobItem "
                   " WHERE  JobKey = %s", pszJobKey);

   if (!(hqry = ItiDbExecQuery(szTmp, hheap, 0, 0, 0, &uCols, &uErr)))
      return NULL;

   while (ItiDbGetQueryRow(hqry, &ppszTmp, &uErr))
      {
      uNodes++;
      if (uNodes+1 >= uAllocSize)
         {
         uAllocSize += 16;
         pjin = (PJINODE) ItiMemRealloc (hheap, pjin,
                                           uAllocSize * sizeof (JINODE), 0);
         }
      pjin[uNodes-1].pszJIK             = ItiStrDup (hheap, ppszTmp[0]);
      pjin[uNodes-1].uDuration          = atoi   (ppszTmp[1]);
      pjin[uNodes-1].uDaysTillStart     = 0;
      pjin[uNodes-1].uSlack             = 0;
      pjin[uNodes-1].uLinks             = 0;
      pjin[uNodes-1].plink              = NULL;
      pjin[uNodes-1].cFlag              = '\0';

      ItiFreeStrArray (hheap, ppszTmp, uCols);
      }

   if (uNodes)
      pjin[uNodes].pszJIK = NULL;  /*--- terminator ---*/
   else
      return NULL;

   /*--- Now we build the links between the nodes ---*/
   sprintf (szTmp, " SELECT PredecessorJobItemKey,"
                          " SuccessorJobItemKey,"
                          " Link, Lag, Delay"
                   " FROM JobItemDependency",
                   " WHERE JobKey = %s", pszJobKey);

   if (!(hqry = ItiDbExecQuery(szTmp, hheap, 0, 0, 0, &uCols, &uErr)))
      return NULL;

   while (ItiDbGetQueryRow(hqry, &ppszTmp, &uErr))
      {
      uPred = uSucc = 0xFFFF;
      for (i=0; i<uNodes; i++)
         {                              
         if (!strcmp (pjin[i].pszJIK, ppszTmp[0]))
            uPred = i;                  
         if (!strcmp (pjin[i].pszJIK, ppszTmp[1]))
            uSucc = i;
         }
      if (uPred == 0xFFFF || uSucc == 0xFFFF) /*--- bad link ---*/
         continue;

      i = pjin[uPred].uLinks++;
      pjin[uPred].plink = (PLINK) ItiMemRealloc (hheap, pjin[uPred].plink,
                                                 i * sizeof (LINK), 0);
      pjin[uPred].plink[i].uSuccessor = uSucc;
      pjin[uPred].plink[i].uLink      = atoi (ppszTmp[2]);
      pjin[uPred].plink[i].uLag       = atoi (ppszTmp[3]);
      pjin[uPred].plink[i].uDelay     = atoi (ppszTmp[4]);
      }
   return pjin;
   }







/*
 * returns min time so far
 * 
 */
USHORT GetCriticalPath (PJINODE pjin,
                        USHORT uNode,
                        USHORT uCurrentDay,
                        USHORT uPrevLink)
   {
   PLINK  plink;
   USHORT i,
          uTmp,
          uStartDay,
          uLen = 0;

   if (IsInPath(pjin[uNode]))
      {
      /*--- Loop detected in path scheduling ---*/
      WinMessageBox (HWND_DESKTOP, WinQueryActiveWindow (HWND_DESKTOP, 0), 
                     "Loop in Schedule found. "
                     " Job Schedule cannot be calculated with looped dependencies.",
                     pglobals->pszAppName, 0, MB_OK | MB_ICONHAND);
      return 0;
      }

   PutInPath (pjin[uNode]);

   uStartDay = uCurrentDay;

   if (uPrevLink == SF || uPrevLink == FF)
      uStartDay = max (0, (INT)uCurrentDay - (INT)pjin[uNode].uDuration);

   pjin[uNode].uDaysTillStart = max (pjin[uNode].uDaysTillStart, uStartDay);

   /*--- end of a chain ---*/
   if (!pjin[uNode].uLinks)
      return uStartDay + pjin[uNode].uDuration;

   for (i=0; i<pjin[uNode].uLinks; i++)
      {
      plink = &(pjin[uNode].plink[i]);

      uTmp = uStartDay + plink->uLag;

      if (plink->uLink == FS || plink->uLink == FF)
         uTmp += pjin[uNode].uDuration;

      uLen = max (uLen,
                  GetCriticalPath (pjin, pjin[uNode].plink[i].uSuccessor,
                  uTmp, plink->uLink));
      }
   RemoveFromPath (pjin[uNode]);
   return uLen;
   }





void GetSlack (PJINODE pjin, USHORT uProjDuration)
   {
   PLINK  plink;
   USHORT uNode, i;
   USHORT uSlack, uSuccReady, uReady;


   for (uNode=0; pjin && pjin[uNode].pszJIK; uNode++)
      {
      pjin[uNode].uSlack = 0xFFFF;

      if (!pjin[uNode].uLinks)
         {
         pjin[uNode].uSlack = uProjDuration -
                              pjin[uNode].uDaysTillStart -
                              pjin[uNode].uDuration;
         continue;
         }

      for (i=0; i<pjin[uNode].uLinks; i++)
         {
         plink = &(pjin[uNode].plink[i]);

         uSuccReady = pjin[plink->uSuccessor].uDaysTillStart;

         if (plink->uLink == SF || plink->uLink == FF)
            uSuccReady += pjin[plink->uSuccessor].uDuration;

         uReady = pjin[uNode].uDaysTillStart + plink->uLag;

         if (plink->uLink == FS || plink->uLink == FF)
            uReady += pjin[uNode].uDuration;

         uSlack = uSuccReady - uReady;

         pjin[uNode].uSlack = min (pjin[uNode].uSlack, uSlack);
         }
      }
   }



USHORT GetJobDuration (HHEAP hheap, PSZ pszJobKey)
   {
   char     szTmp[250] = "";
   PJINODE  pjin;
   USHORT   i, j, uLen = 0;
   HHEAP    hheapLocal;


   if (!(hheapLocal = ItiMemCreateHeap (0x8000)))
      return 0;

   if (!(pjin = BuildScheduleTree (hheapLocal, pszJobKey)))
      return 0;

   /*--- first, find the items without predecessors ---*/
   for (i=0; pjin && pjin[i].pszJIK; i++)
      for (j=0; j<pjin[i].uLinks; j++)
         pjin[pjin[i].plink[j].uSuccessor].cFlag |= 0x0001;

   /*--- get the longest path starting from all start points ---*/
   for (i=0; pjin && pjin[i].pszJIK; i++)
      if (!pjin[i].cFlag)
         uLen = max (uLen, GetCriticalPath (pjin, i, 0, 0));

   /*--- resolve slack here ---*/
   GetSlack (pjin, uLen);

   /*--- update job table to reflect job duration ---*/
   sprintf (szTmp, " UPDATE Job"
                   " SET Duration = %d"
                   " WHERE JobKey = %s", uLen, pszJobKey);

   ItiDbExecSQLStatement (hheapLocal, szTmp);

   /*--- update job items to set the slack and DaysBeforeStart fields ---*/
   for (i=0; pjin && pjin[i].pszJIK; i++)
      {
      sprintf (szTmp, " UPDATE JobItem"
                      " SET Slack = %d,"
                      " DaysElapsedBeforeStart = %d"
                      " WHERE JobKey = %s"
                      " AND JobItemKey = %s",
                      pjin[i].uSlack, pjin[i].uDaysTillStart,
                      pszJobKey, pjin[i].pszJIK);

      ItiDbExecSQLStatement (hheapLocal, szTmp);
      }

   ItiDbUpdateBuffers ("Job");
   ItiDbUpdateBuffers ("JobItem");
   ItiMemDestroyHeap (hheapLocal);
   return uLen;
   }





void DoCalculateJobSchedule (HWND hwnd)
   {
   HWND  hwndModeless;
   HHEAP hheapWnd, hheapTmp;
   PSZ   pszJobKey;

   hwndModeless = WinLoadDlg (HWND_DESKTOP, hwnd, WinDefDlgProc, 
                              0, CalculateJobSchedule, NULL);
   if (!hwndModeless)
      {
      WinMessageBox (HWND_DESKTOP, hwnd, 
                     "Not enough memory to calculate job schedule."
                     "  Try the command again after closing some applications.",
                     pglobals->pszAppName, 0, MB_OK | MB_ICONHAND);
      return;
      }
                                 
   WinUpdateWindow (hwndModeless);
   ItiWndSetHourGlass (TRUE);

   if (!(pszJobKey = (PSZ)   QW (hwnd, ITIWND_DATA, 0, 0, cJobKey)) ||
       !(hheapWnd  = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0))       ||
       !(hheapTmp  = ItiMemCreateHeap (0x8000)))
      {
      ItiWndSetHourGlass (FALSE);
      WinDestroyWindow (hwndModeless);
      ItiMemDestroyHeap (hheapTmp);
      ItiMemFree (hheapWnd, pszJobKey);
      return;
      }

   GetJobDuration (hheapTmp, pszJobKey);

//
//  For now, duration of zero may be valid
//
//   if (!GetJobDuration (hheapTmp, pszJobKey))
//      {
//      WinMessageBox (HWND_DESKTOP, hwnd, 
//                     "Not enough memory to calculate job schedule."
//                     "  Try the command again after closing some applications.",
//                     pglobals->pszAppName, 0, MB_OK | MB_ICONHAND);
//      ItiWndSetHourGlass (FALSE);
//      WinDestroyWindow (hwndModeless);
//      ItiMemDestroyHeap (hheapTmp);
//      ItiMemFree (hheapWnd, pszJobKey);
//      return;
//      }

   ItiWndSetHourGlass (FALSE);
   WinDestroyWindow (hwndModeless);
   if (NULL != hheapTmp)
      ItiMemDestroyHeap (hheapTmp);
   if (NULL != pszJobKey)
      ItiMemFree (hheapWnd, pszJobKey);
   }


void DisplayError (HWND   hwnd,
                   USHORT usError)
   {
   CHAR  szTmp[500] = "";

   switch (usError)
      {
      case MEMORY_ERROR:
         sprintf (szTmp,
                  "Not enough memory to calculate program schedule. "
                  "Try the command again after closing some applications.");
         break;
      case DATABASE_ERROR:
         sprintf (szTmp,
                  "Database error while calculating program schedule. "
                  "See the log for more information.");
         break;
      case PATH_ERROR:
         sprintf (szTmp,
                  "Path error while calculating program schedule. "
                  "A loop condition exists. ");
         break;
      default:
         sprintf (szTmp,
                  "Unable to calculate program schedule. ");
      }

   WinMessageBox (HWND_DESKTOP, hwnd,
                  szTmp, 
                  pglobals->pszAppName, 0, MB_OK | MB_ICONHAND);
   return;
   }

/*
 * Initialize the start and end job nodes of the program. The start
 * node is placed in node[0]. The end node is placed in node[1]
*/
void InitializeStartEndNodes (PPJNODE ppjJob)
   {
   ppjJob[0].pszJobKey = NULL;

   ppjJob[0].lWorkBeginDate =
             ppjJob[0].lWorkEndDate = 0;

   ppjJob[0].usPriority =
             ppjJob[0].usPriority =
             ppjJob[0].usDuration =   
             ppjJob[0].usSlack    = 0;

   ppjJob[0].usFwdLinks  = 0;
   ppjJob[0].usBwdLinks  = 0;
   ppjJob[0].ppjBackward = NULL;  
   ppjJob[0].ppjForward  = NULL;
   ppjJob[0].bDateCalced = FALSE;

   ppjJob[1].pszJobKey = NULL;

   ppjJob[1].lWorkBeginDate =
             ppjJob[1].lWorkEndDate = 0;

   ppjJob[1].usPriority =
             ppjJob[1].usPriority =
             ppjJob[1].usDuration =   
             ppjJob[1].usSlack    = 0;

   ppjJob[1].usFwdLinks  = 0;
   ppjJob[1].usBwdLinks  = 0;
   ppjJob[1].ppjBackward = NULL;  
   ppjJob[1].ppjForward  = NULL;
   ppjJob[1].bDateCalced = FALSE;

   return;
   }

/*
 * Trace through the job path to see if the passed job is already
 * in the path.
*/
USHORT ValidateProgramTree (PPJNODE ppjJob,
                            PSZ     pszJobKey)
   {
   USHORT  i, uErr;
  
   for (i = 1 ; i < ppjJob->usFwdLinks ; i++)
      {
      if (!ItiStrCmp (pszJobKey, ppjJob->ppjForward[i].ppjJobLink->pszJobKey))
         return PATH_ERROR;

      uErr = ValidateProgramTree (ppjJob->ppjForward[i].ppjJobLink, pszJobKey);

      if (uErr)
         return PATH_ERROR;
      i++;
      }

   return 0;
   }

/* This routine will add the link between two jobs. Input to
 * this routine is the 'back' job, the 'front' job, the type
 * of link (ie SS, SF, FS, FF), the lag between jobs, and the level
 * in the path.
*/
USHORT AddJobLinks (HHEAP   hheap,
                    PPJNODE ppjBack,
                    PPJNODE ppjFront,
                    USHORT  usLink,
                    USHORT  usLag,
                    USHORT  usLevel)
   {
   USHORT i, j, usReturn;
   CHAR   szJobKey[10] = "";
   BOOL   bFwdLinkFound, bBwdLinkFound;

   bFwdLinkFound = bBwdLinkFound = FALSE;

   /* if the forward link already exists from another path,
    * update the level only in the path.
   */
   for (j = 0 ; j < ppjBack->usFwdLinks ; j++)
      {
      if (ppjBack->ppjForward[j].ppjJobLink->pszJobKey)
         {
         if (!ItiStrCmp (ppjBack->ppjForward[j].ppjJobLink->pszJobKey,
                         ppjFront->pszJobKey))
            {
            ppjBack->ppjForward[j].usLevel  = usLevel;
            bFwdLinkFound = TRUE;
            }
         }
      }

   /* if the backward link already exists from another path,
    * update the level only in the path.
   */
   for (j = 0 ; j < ppjFront->usBwdLinks ; j++)
      {
      if (ppjFront->ppjBackward[j].ppjJobLink->pszJobKey)
         {
         if (!ItiStrCmp (ppjFront->ppjBackward[j].ppjJobLink->pszJobKey,
                         ppjBack->pszJobKey))
            {
            ppjFront->ppjBackward[j].usLevel  = usLevel;
            bBwdLinkFound = TRUE;
            }
         }
      }

   /* Add the forward and backward links between the Jobs */
   if (!bFwdLinkFound)
      {
      ppjBack->usFwdLinks++;
      i = ppjBack->usFwdLinks - 1;
      ppjBack->ppjForward[i].ppjJobLink = ppjFront;
      ppjBack->ppjForward[i].usLink     = usLink;
      ppjBack->ppjForward[i].usLag      = usLag;
      ppjBack->ppjForward[i].usLevel    = usLevel;
      ppjBack->ppjForward[i].lDelay     = 0;
      }

   if (!bBwdLinkFound)
      {
      ppjFront->usBwdLinks++;
      i = ppjFront->usBwdLinks - 1;
      ppjFront->ppjBackward[i].ppjJobLink = ppjBack;
      ppjFront->ppjBackward[i].usLink     = usLink;
      ppjFront->ppjBackward[i].usLag      = usLag;
      ppjFront->ppjBackward[i].usLevel    = usLevel;
      ppjFront->ppjBackward[i].lDelay     = 0;
      }

   /* Validate the new link to check for loops. */
   usReturn = 0;
   ItiStrCpy (szJobKey, ppjBack->pszJobKey, sizeof szJobKey);

   if (ppjBack->usFwdLinks > 1)
      usReturn = ValidateProgramTree (ppjBack, szJobKey);

   return usReturn;
   }

/* Find the node passed at the current level in the path.
 * return TRUE if the job exists at this level.
*/
BOOL FindJobKeyToSearch (PPJNODE ppjJob,
                         USHORT  usEntry,
                         USHORT  usLevel,
                         PSZ     pszJobKey)
   {
   BOOL    bFound;
   USHORT  i, j;

   i = ppjJob[usEntry].usFwdLinks;
   j = 0;
   bFound    = FALSE;

   if (usLevel == 0)
      {
      ItiStrCpy (pszJobKey, ppjJob[usEntry].pszJobKey, 11);
      return TRUE;
      }

   while (!bFound && i)
      {
      if (ppjJob[usEntry].ppjForward[j].usLevel == usLevel)
         {
         bFound = TRUE;
         ItiStrCpy (pszJobKey,
                    ppjJob[usEntry].ppjForward[j].ppjJobLink->pszJobKey, 11);
         }
      j++;
      i--;
      }

   return bFound;
   }


LONG AdjustForWorkingDays (LONG   lDate,
                           int    usDuration,
                           PSZ    pszCalendar)
   {
   int     i, j, iDay;
   LONG    lYearBegin, lReturn;
   USHORT  usBaseYear, usYear;
   CHAR    szDate[12] = "";
   PSZ     pszToken;

   usBaseYear = 1995;

   ItiStrCpy (szDate, ItiFmtJulianToCalendar (lDate), sizeof szDate);

   pszToken = strtok (szDate, "/");
   pszToken = strtok (NULL, "/");
   pszToken = strtok (NULL, "/");
   usYear   = atoi (pszToken);

   sprintf (szDate, "1/1/%d", usYear);

   lYearBegin = ItiFmtCalendarToJulian (szDate);

   pWorkDays = &WorkDays;

   while (usYear > usBaseYear)
      {
      pWorkDays++;
      usYear --;
      }

   lReturn = lDate;

   iDay = (USHORT) (lDate - lYearBegin);
   if (iDay >= 59 && !pWorkDays->bLeapYear)
      iDay++;

   i    = ((usDuration < 0) ? +1 : -1);

   while (!pWorkDays->bWorkDay[iDay])
      {
      lReturn -= i;
      iDay    -= i;
      if (iDay == 59 && !pWorkDays->bLeapYear)
         iDay    -= i;
      if (iDay == 367)
         {
         iDay = 1;
         pWorkDays++;
         }
      if (iDay == -1)
         {
         iDay = 366;
         pWorkDays--;
         }
      }

   j = (usDuration ? usDuration += i : 0);

   while (j != 0)
      {
      if (pWorkDays->bWorkDay[iDay])
         j += i;

      lReturn -= i;
      iDay    -= i;
      if (iDay == 59 && !pWorkDays->bLeapYear)
         iDay    -= i;
      if (iDay == 367)
         {
         iDay = 1;
         pWorkDays++;
         }
      if (iDay == -1)
         {
         iDay = 366;
         pWorkDays--;
         }
      }

   while (!pWorkDays->bWorkDay[iDay])
      {
      lReturn -= i;
      iDay    -= i;
      if (iDay == 59 && !pWorkDays->bLeapYear)
         iDay    -= i;
      if (iDay == 367)
         {
         iDay = 1;
         pWorkDays++;
         }
      if (iDay == -1)
         {
         iDay = 366;
         pWorkDays--;
         }
      }

   return lReturn;
   }


/*
 * Compute the start and end dates for each job in the program.
 * This routine is passed Jobs at the beginning of the path
 * (ie no predecessors), and traverses the path setting each date
 * via a recursive call to this routine.
*/
USHORT ComputeBeginEndDates (PPJNODE ppjJob)
   {
   USHORT i, j, uErr;
   LONG   lPredBegin, lPredEnd, lSuccBegin, lSuccEnd, lDelay, lCurrent;

   /* Get the work begin and end dates for the predecessor job. */
   lPredBegin = AdjustForWorkingDays (ppjJob->lWorkBeginDate,
                                      0, NULL);

   lPredEnd   = AdjustForWorkingDays (lPredBegin,
                                      (int)ppjJob->usDuration,
                                      NULL);

   /* Set the delay date for the ProgramJobDependency Table */
   for (i = 1 ; i < ppjJob->usFwdLinks ; i++)
      {
      if (ppjJob->ppjForward[i].usLink == FF ||
          ppjJob->ppjForward[i].usLink == FS)
         ppjJob->ppjForward[i].lDelay = lPredEnd;
      else
         ppjJob->ppjForward[i].lDelay = lPredBegin;
      }

   /* If a job only has one backward link it is the beginning of the path.
    * If this is the case set the dates and traverse the path.
   */
   if (ppjJob->usBwdLinks == 1)
      {
      ppjJob->lWorkBeginDate = lPredBegin;
      ppjJob->lWorkEndDate   = lPredEnd;

      for (i = 1 ; i < ppjJob->usFwdLinks ; i++)
         {
         uErr = ComputeBeginEndDates (ppjJob->ppjForward[i].ppjJobLink);
         if (uErr)
            return uErr;
         }
      ppjJob->bDateCalced = TRUE;
      return 0;
      }

   /*
    * get the current begin date for the job.
   */
   lCurrent = AdjustForWorkingDays (ppjJob->lWorkBeginDate,
                                    0, NULL);

   /* For each Backward link for this job calculate the start and
    * end date for this job based on the type of link.
   */
   for (i = 1 ; i < ppjJob->usBwdLinks ; i++)
      {
      lPredBegin = ppjJob->ppjBackward[i].ppjJobLink->lWorkBeginDate;
      lPredEnd   = ppjJob->ppjBackward[i].ppjJobLink->lWorkEndDate;

      lSuccEnd = lSuccBegin = 0;

      switch (ppjJob->ppjBackward[i].usLink)
         {
         case SS: /* Start-to-Start */

            lSuccBegin = AdjustForWorkingDays
                        (lPredBegin + ppjJob->ppjBackward[i].usLag,
                         0, NULL);

            lSuccEnd   = AdjustForWorkingDays
                        (lSuccBegin, (int)ppjJob->usDuration, NULL);

            lDelay     = lPredBegin;
            break;
     
         case SF: /* Start-to-Finish */

            lSuccEnd   = AdjustForWorkingDays
                        (lPredBegin + ppjJob->ppjBackward[i].usLag - 1,
                         0, NULL);

            lSuccBegin = AdjustForWorkingDays
                        (lSuccEnd, (-1 * (int)ppjJob->usDuration), NULL);
            lDelay     = lPredBegin;
            break;
     
         case FS: /* Finish-to-Start */

            lSuccBegin = AdjustForWorkingDays
                        (lPredEnd + ppjJob->ppjBackward[i].usLag + 1,
                         0, NULL);

            lSuccEnd   = AdjustForWorkingDays
                        (lSuccBegin, ppjJob->usDuration, NULL);
                        
            lDelay     = lPredEnd;
            break;
     
         case FF: /* Finish-to-Finish */

            lSuccEnd   = AdjustForWorkingDays
                        (lPredEnd + ppjJob->ppjBackward[i].usLag,
                         0, NULL);

            lSuccBegin = AdjustForWorkingDays
                        (lSuccEnd, (-1 * (int)ppjJob->usDuration), NULL);

            lDelay     = lPredEnd;
            break;
         }

      /* if this is the first time the job was found or the current
       * begin date is less than the calculated date update the
       * current date with the calculated date.
      */

      if (!ppjJob->bDateCalced || lCurrent < lSuccBegin)
         {
         ppjJob->lWorkBeginDate = lSuccBegin;
         ppjJob->lWorkEndDate   = lSuccEnd;
         ppjJob->ppjBackward[i].lDelay = lDelay;
         ppjJob->ppjBackward[i].ppjJobLink->ppjForward[i].lDelay = lDelay;
         }

      ppjJob->bDateCalced = TRUE;

      /*
       * Continue along the path.
      */
      for (j = 1 ; j < ppjJob->usFwdLinks ; j++)
         {
         uErr = ComputeBeginEndDates (ppjJob->ppjForward[j].ppjJobLink);
         if (uErr)
            return uErr;
         }
      }

   return 0;
   }


USHORT CalculateCriticalPath (PPJNODE ppjJob,
                              LONG    lMaxEndDate)
   {
   USHORT  i, j, uErr, usMinSlack;
   LONG    lMaxLocal, lCurrent;
   PPJNODE ppjLocal;

   lMaxLocal = lMaxEndDate;
   uErr = 0;
   usMinSlack = 32767;

   for (i = 1 ; i < ppjJob->usFwdLinks ; i++)
      {
      if (ppjJob->ppjForward[i].ppjJobLink->usSlack < usMinSlack)
         usMinSlack = ppjJob->ppjForward[i].ppjJobLink->usSlack;
      }

   if (usMinSlack == 32767)
      usMinSlack = 0;

//   if (ppjJob->usFwdLinks > 1)
//      ppjJob->usSlack = usMinSlack;

   for (i = 0 ; i < ppjJob->usBwdLinks ; i++)
      {
      ppjJob->ppjBackward[i].ppjJobLink->usSlack = (USHORT)                             
              (lMaxLocal - ppjJob->ppjBackward[i].ppjJobLink->lWorkEndDate);
      ppjJob->ppjBackward[i].ppjJobLink->usSlack += usMinSlack;                             
      }

   for (i = 0 ; i < ppjJob->usBwdLinks ; i++)
      {
      ppjLocal  = ppjJob->ppjBackward[i].ppjJobLink;
      lCurrent = lMaxLocal;
      lMaxLocal = 0;

      for (j = 0 ; j < ppjLocal->usBwdLinks ; j++)
         {
         switch (ppjLocal->ppjBackward[j].usLink)
            {
            case SS:
               lCurrent = ppjLocal->ppjBackward[j].ppjJobLink->lWorkBeginDate
                        + ppjLocal->ppjBackward[j].usLag;
               break;           
            case SF:
               lCurrent = ppjLocal->ppjBackward[j].ppjJobLink->lWorkBeginDate
                        + ppjLocal->ppjBackward[j].usLag
                        - ppjLocal->usDuration;
               break;           
            case FS:
               lCurrent = ppjLocal->ppjBackward[j].ppjJobLink->lWorkEndDate
                        + ppjLocal->ppjBackward[j].usLag;
               break;           
            case FF:
               lCurrent = ppjLocal->ppjBackward[j].ppjJobLink->lWorkEndDate
                        + ppjLocal->ppjBackward[j].usLag
                        - ppjLocal->usDuration;
               break;           
            default:
               lCurrent = 0;
            } 
         if (lCurrent > lMaxLocal)
            lMaxLocal = lCurrent;
         }
      uErr = CalculateCriticalPath (ppjLocal, lMaxLocal);
      }

   return uErr;
   }


USHORT OrderProgramSchedule (HHEAP  hheap,
                             PPJNODE ppjJob,
                             USHORT  usLevel)
   {
   CHAR    szTmp[250] = "";
   CHAR    szJobKey[10] = "";
   CHAR    szSuccessorJobKey[10] = "";
   USHORT  uCols, uErr, i, j, k, usLink, usLag, usReturn;
   PSZ     *ppszTmp;
   HQRY    hqry;
   HHEAP   hheapLocal;
   BOOL    bFound;

   i = 2;
   usReturn = 0;

   if (!(hheapLocal = ItiMemCreateHeap (1000)))
      {
      return MEMORY_ERROR;
      }

   while (ppjJob[i].pszJobKey)
      {
      bFound = FindJobKeyToSearch (ppjJob, i, usLevel, szJobKey);
         
      if (bFound)
         {
         sprintf(szTmp,"SELECT SuccessorJobKey, Link, Lag "  
                       "FROM   ProgramJobDependency "                       
                       "WHERE  PredecessorJobKey = %s "
                       "ORDER BY Delay"
                       , szJobKey);
     
         if (!(hqry = ItiDbExecQuery(szTmp, hheapLocal, 0, 0, 0, &uCols, &uErr)))
            {
            ItiMemDestroyHeap (hheapLocal);
            return DATABASE_ERROR;
            }
     
         while (ItiDbGetQueryRow(hqry, &ppszTmp, &uErr))
            {

            ItiStrCpy (szSuccessorJobKey, ppszTmp[0], sizeof szSuccessorJobKey);
     
            usLink   = atoi (ppszTmp[1]);  
            usLag    = atoi (ppszTmp[2]);
     
            k = 2;
            bFound = FALSE;
            while (!bFound)
               {
               if (!ItiStrCmp (ppjJob[k].pszJobKey, szJobKey))
                  bFound = TRUE;
               else
                  k++;
               }
     
            j = 2; 
            while (ppjJob[j].pszJobKey)
               {
               if (!ItiStrCmp (ppjJob[j].pszJobKey, szSuccessorJobKey))
                  {
                  usReturn = AddJobLinks (hheap, &ppjJob[k],
                                          &ppjJob[j],
                                          usLink, usLag, (usLevel + 1));
                  if (usReturn)
                     return usReturn;
                  }
               j++;
               }
            }
         }
      i++;
      }

   if (usReturn == 0)
      usReturn = NO_MORE_LINKS;

   ItiMemDestroyHeap (hheapLocal);
   return usReturn;
   }



USHORT UpdateProgramJobs (HHEAP   hheap,
                          PSZ     pszProgramKey,
                          PPJNODE ppjJob)
   {
   USHORT  i, j;
   CHAR    szTmp[400] = "";
   CHAR    szProgramEndDate[12] = "";
   CHAR    szWorkBeginDate[12] = "";
   CHAR    szWorkEndDate[12] = "";
   CHAR    szDelay[12];
   LONG    lJulDateMax, lTemp;

   i = 2;
   lJulDateMax = 0;

   while (ppjJob[i].pszJobKey)
      {
      ItiStrCpy (szWorkBeginDate,
                 ItiFmtJulianToCalendar (ppjJob[i].lWorkBeginDate),
                 sizeof szWorkBeginDate);
      ItiStrCpy (szWorkEndDate,
                 ItiFmtJulianToCalendar (ppjJob[i].lWorkEndDate),
                 sizeof szWorkEndDate);

      sprintf (szTmp,  " UPDATE ProgramJob "
                       " SET WorkBeginDate = '%s', "
                       " WorkEndDate = '%s', "
                       " Slack = %d "
                       " WHERE JobKey = %s "
                       " AND ProgramKey = %s "
                       , szWorkBeginDate
                       , szWorkEndDate
                       , ppjJob[i].usSlack
                       , ppjJob[i].pszJobKey
                       , pszProgramKey);

      if (ItiDbExecSQLStatement (hheap, szTmp))
         return DATABASE_ERROR;

      lTemp = ppjJob[i].lWorkEndDate;
      if (lTemp > lJulDateMax)
         lJulDateMax = lTemp;

      for (j = 1 ; j < ppjJob[i].usFwdLinks ; j++)
         {
         ItiStrCpy (szDelay,
                    ItiFmtJulianToCalendar (ppjJob[i].ppjForward[j].lDelay),
                    sizeof szDelay);

         sprintf (szTmp, "UPDATE ProgramJobDependency "
                         " SET Delay = '%s' "
                         " WHERE PredecessorJobKey = %s "
                         " AND SuccessorJobKey = %s "
                         , szDelay
                         , ppjJob[i].pszJobKey
                         , ppjJob[i].ppjForward[j].ppjJobLink->pszJobKey);
     
         if (ItiDbExecSQLStatement (hheap, szTmp))
            return DATABASE_ERROR;
         }

      i++;
      }


   ItiStrCpy (szProgramEndDate,
              ItiFmtJulianToCalendar (lJulDateMax),
              sizeof szProgramEndDate);

   sprintf (szTmp, "UPDATE Program "
                    " SET EndDate = '%s' "
                    " WHERE ProgramKey = %s "
                    , szProgramEndDate
                    , pszProgramKey);

   if (ItiDbExecSQLStatement (hheap, szTmp))
      return DATABASE_ERROR;

   return 0;

   }



void DoCalculateProgramSchedule (HWND hwnd)
   {
   HWND      hwndModeless;
   HHEAP     hheapWnd, hheapJob, hheapLink;
   PSZ       pszProgramKey, *ppszTmp;
   CHAR      szTmp [500] = "";
   USHORT    uCols, uErr, i, j, usLevel;
   HQRY      hqry;
   PPJNODE   ppjJob;
   LONG      lMaxEndDate;

   hwndModeless = WinLoadDlg (HWND_DESKTOP, hwnd, WinDefDlgProc, 
                              0, CalculateProgramSchedule, NULL);
   
   if (hwndModeless == NULL)
      {
      DisplayError (hwnd, MEMORY_ERROR);
      return;
      }
                                 
   WinUpdateWindow (hwndModeless);
   ItiWndSetHourGlass (TRUE);

   if (!(pszProgramKey = (PSZ)   QW (hwnd, ITIWND_DATA, 0, 0, cProgramKey)) ||
       !(hheapWnd  = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0)) ||
       !(hheapLink = ItiMemCreateHeap (MAX_HEAP_SIZE)) ||
       !(hheapJob  = ItiMemCreateHeap (MAX_HEAP_SIZE)))
      {
      DisplayError (hwnd, MEMORY_ERROR);
      ItiWndSetHourGlass (FALSE);
      return;
      }

   sprintf(szTmp,"SELECT JobKey, " 
                 "CONVERT(varchar(26),WorkBeginDate, 101), "
                 "CONVERT(varchar(26),WorkEndDate, 101), "
                 "Priority "
                 "FROM ProgramJob "
                 "WHERE ProgramKey = %s "
                 "ORDER BY Priority, WorkBeginDate", pszProgramKey);

   if (!(hqry = ItiDbExecQuery(szTmp, hheapJob, 0, 0, 0, &uCols, &uErr)))
      {
      ItiMemDestroyHeap (hheapJob);
      ItiMemDestroyHeap (hheapLink);
      DisplayError (hwnd, DATABASE_ERROR);
      ItiWndSetHourGlass (FALSE);
      return;
      }

   i = 3;
   ppjJob   = (PPJNODE) ItiMemAlloc (hheapJob, MAX_JOBS * sizeof (PJNODE), 0);

   if (!ppjJob)
      {
      ItiMemDestroyHeap (hheapJob);
      ItiMemDestroyHeap (hheapLink);
      DisplayError (hwnd, MEMORY_ERROR);
      ItiWndSetHourGlass (FALSE);
      return;
      }

   InitializeStartEndNodes (ppjJob);
                                                                       
   ppjJob[0].ppjForward = (PPJLINK)
                          ItiMemAlloc (hheapLink, MAX_JOBS * sizeof (PJLINK), 0);
                                                                       
   ppjJob[1].ppjBackward = (PPJLINK)
                          ItiMemAlloc (hheapLink, MAX_JOBS * sizeof (PJLINK), 0);

   while (ItiDbGetQueryRow(hqry, &ppszTmp, &uErr))
      {
      j = i - 1;
      ppjJob[j].pszJobKey = ItiStrDup (hheapJob, ppszTmp[0]);

      ppjJob[j].lWorkBeginDate = ItiFmtCalendarToJulian (ppszTmp[1]);
      ppjJob[j].lWorkEndDate   = ItiFmtCalendarToJulian (ppszTmp[2]);

      ppjJob[j].usPriority  = atoi (ppszTmp [3]);
      ppjJob[j].usDuration  = GetJobDuration (hheapJob, ppszTmp[0]);
      if (!ppjJob[j].usDuration)
         ppjJob[j].usDuration  = 1;

      ppjJob[j].usSlack     = 0;
      ppjJob[j].usFwdLinks  = 0;
      ppjJob[j].usBwdLinks  = 0;
      ppjJob[j].bDateCalced = FALSE;
                                                                       
      ppjJob[j].ppjForward = (PPJLINK)
                          ItiMemAlloc (hheapLink, MAX_LINKS * sizeof (PJLINK), 0);
                                                                       
      ppjJob[j].ppjBackward = (PPJLINK)
                          ItiMemAlloc (hheapLink, MAX_LINKS * sizeof (PJLINK), 0);

      AddJobLinks (hheapLink, &ppjJob[0], &ppjJob[j], FS, 0, 0);
      AddJobLinks (hheapLink, &ppjJob[j], &ppjJob[1], FS, 0, MAX_LEVELS);
                               
      ItiFreeStrArray (hheapJob, ppszTmp, uCols);
      i++;

      ppjJob[i-1].pszJobKey = NULL;
      }

   usLevel = 0;
   uErr    = 0;

   while (!uErr)
      {
      uErr = OrderProgramSchedule (hheapLink, ppjJob, usLevel);
      usLevel++;
      }

   if (uErr == NO_MORE_LINKS)
      uErr = 0;

   if (uErr)
      {
      ItiMemFree (hheapJob, ppjJob);
      ItiMemDestroyHeap (hheapJob);
      ItiMemDestroyHeap (hheapLink);
      DisplayError (hwnd, uErr);
      ItiWndSetHourGlass (FALSE);
      WinDestroyWindow (hwndModeless);
      return;
      }

   i = 2;
   while (ppjJob[i].pszJobKey && !uErr)
      {
      if (ppjJob[i].usBwdLinks == 1)
         uErr = ComputeBeginEndDates(&ppjJob[i]);
      i++;
      }

   if (uErr)
      {
      DisplayError (hwnd, uErr);
      WinDestroyWindow (hwndModeless);
      ItiMemDestroyHeap (hheapJob);
      ItiMemDestroyHeap (hheapLink);
      ItiWndSetHourGlass (FALSE);
      return;
      }

   lMaxEndDate = 0;
   for (i = 0 ; i < ppjJob[1].usBwdLinks ; i++)
      {
      if (ppjJob[1].ppjBackward[i].ppjJobLink->lWorkEndDate > lMaxEndDate)
         lMaxEndDate = ppjJob[1].ppjBackward[i].ppjJobLink->lWorkEndDate;
      }

   uErr = CalculateCriticalPath (&ppjJob[1], lMaxEndDate);

   if (uErr)
      {
      DisplayError (hwnd, uErr);
      WinDestroyWindow (hwndModeless);
      ItiMemDestroyHeap (hheapJob);
      ItiMemDestroyHeap (hheapLink);
      ItiWndSetHourGlass (FALSE);
      return;
      }

   uErr = UpdateProgramJobs (hheapJob, pszProgramKey, ppjJob);

   if (uErr)
      {
      DisplayError (hwnd, uErr);
      WinDestroyWindow (hwndModeless);
      ItiMemDestroyHeap (hheapJob);
      ItiMemDestroyHeap (hheapLink);
      ItiWndSetHourGlass (FALSE);
      return;
      }

   ItiDbUpdateBuffers ("Program");
   ItiDbUpdateBuffers ("ProgramJob");

   ItiWndSetHourGlass (FALSE);
   WinDestroyWindow (hwndModeless);

   if (hheapJob)
      ItiMemDestroyHeap (hheapJob);
   if (hheapLink)
      ItiMemDestroyHeap (hheapLink);
   if (pszProgramKey)
      ItiMemFree (hheapWnd, pszProgramKey);

   return;
   }




