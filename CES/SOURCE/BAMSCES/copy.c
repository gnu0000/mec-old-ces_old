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
 * copy.c
 * Mark Hampton
 * (C) 1991 AASHTO
 *
 * This module handles the copy job dialog box.
 */


#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\winid.h"
#include "..\include\itiutil.h"
#include "..\include\itiglob.h"
#include "..\include\itiperm.h"
#include "..\include\colid.h"
#include "..\include\dialog.h"
#include "bamsces.h"
#include "dialog.h"
#include "copy.h"
#include "..\include\tableid.h"
#include <stdio.h>


static BOOL CopyTable (HHEAP  hheap,
                       HWND   hwnd,
                       PPSZ   apszTokens,
                       PPSZ   apszValues,
                       USHORT usTable)
   {
   char szTemp [2048];
   char szQuery [2048];

   szQuery [0] = '\0';
   ItiMbQueryQueryText (0, ITIRID_COPY, usTable, szQuery, sizeof szQuery);
   ItiStrReplaceParams (szTemp, szQuery, sizeof szTemp,
                        apszTokens, apszValues, 100);
   ItiWndPrepQuery (szQuery, szTemp, hwnd, ITI_ADD);
   if (szQuery [0] == '\0')
      return FALSE;

   return 0 == ItiDbExecSQLStatement (hheap, szQuery);
   }


static BOOL CopyTheSucker (HHEAP hheap,
                           HWND  hwnd,
                           PSZ   pszOldJobKey,
                           PSZ   pszNewJobKey,
                           PSZ   pszNewJobID)
   {
   PSZ apszTokens [4];
   PSZ apszValues [4];

   apszTokens [0] = "JobKey";
   apszValues [0] = pszNewJobKey;
   apszTokens [1] = "OldJobKey";
   apszValues [1] = pszOldJobKey;
   apszTokens [2] = "JobID";
   apszValues [2] = pszNewJobID;
   apszTokens [3] = NULL;
   apszValues [3] = NULL;

   if (!CopyTable (hheap, hwnd, apszTokens, apszValues, tJob))
      return FALSE;

   if (!CopyTable (hheap, hwnd, apszTokens, apszValues, tJobUser))
      return FALSE;

   if (!CopyTable (hheap, hwnd, apszTokens, apszValues, tJobFund))
      return FALSE;

   if (!CopyTable (hheap, hwnd, apszTokens, apszValues, tJobAlternateGroup))
      return FALSE;

   if (!CopyTable (hheap, hwnd, apszTokens, apszValues, tJobAlternate))
      return FALSE;

   if (!CopyTable (hheap, hwnd, apszTokens, apszValues, tJobBreakdown))
      return FALSE;

   if (!CopyTable (hheap, hwnd, apszTokens, apszValues, tJobBreakdownFund))
      return FALSE;

   if (!CopyTable (hheap, hwnd, apszTokens, apszValues, tJobItem))
      return FALSE;

   if (!CopyTable (hheap, hwnd, apszTokens, apszValues, tJobItemDependency))
      return FALSE;

   if (!CopyTable (hheap, hwnd, apszTokens, apszValues, tJobCostBasedEstimate))
      return FALSE;

   if (!CopyTable (hheap, hwnd, apszTokens, apszValues, tJobCostSheet))
      return FALSE;

   if (!CopyTable (hheap, hwnd, apszTokens, apszValues, tJobCrewUsed))
      return FALSE;

   if (!CopyTable (hheap, hwnd, apszTokens, apszValues, tJobCrewEquipmentUsed))
      return FALSE;

   if (!CopyTable (hheap, hwnd, apszTokens, apszValues, tJobCrewLaborUsed))
      return FALSE;

   if (!CopyTable (hheap, hwnd, apszTokens, apszValues, tJobMaterialUsed))
      return FALSE;

   if (!CopyTable (hheap, hwnd, apszTokens, apszValues, tJobZone))
      return FALSE;

    if (!CopyTable (hheap, hwnd, apszTokens, apszValues, tJobBreakdownProfile))
       return FALSE;
 
    if (!CopyTable (hheap, hwnd, apszTokens, apszValues, tJobBreakdownMajorItem))
       return FALSE;


   return TRUE;
   }






BOOL DoCopyJob (HWND hwnd)
   {
   USHORT i;

   i = (UM) ItiWndBuildWindow (WinWindowFromID (hwnd,OpenJobDL), CopyJobD);

   return i == 1;
   }



MRESULT EXPORT CopyJobDProc (HWND   hwnd,
                             USHORT usMessage,
                             MPARAM mp1,
                             MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               {
               HHEAP hheap;
               PSZ   pszOldJobKey;
               PSZ   pszNewJobKey;
               char  szNewID [20];

               hheap = QW (hwnd, ITIWND_HEAP, 0, 0, 0);
               pszOldJobKey = QW (hwnd, ITIWND_DATA, 0, 0, cJobKey);
               pszNewJobKey = ItiDbGetKey (hheap, "Job", 1);
               if (pszNewJobKey == NULL)
                  {
                  WinMessageBox (HWND_DESKTOP, hwnd, 
                                 "Unable to get a new key for the new job.",
                                 pglobals->pszAppName, 0, 
                                 MB_MOVEABLE | MB_OK | MB_ICONHAND);
                  return FALSE;
                  }
               ItiWndSetHourGlass (TRUE);
               WinQueryDlgItemText (hwnd, DID_JOBID, sizeof szNewID, szNewID);
               CopyTheSucker (hheap, hwnd, pszOldJobKey, pszNewJobKey, szNewID);
               ItiMemFree (hheap, pszOldJobKey);
               ItiMemFree (hheap, pszNewJobKey);
               ItiWndSetHourGlass (FALSE);
               }
               break;
            }
         break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }

