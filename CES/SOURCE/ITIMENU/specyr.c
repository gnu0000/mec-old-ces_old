/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1995 by Info Tech, Inc.  All Rights Reserved.         |
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


/*
 * SpecYr.c
 * Timothy Blake
 * Copyright (C) 1995 Info Tech, Inc.
 *
 * This module processes the select SpecYr menu choice.  It contains the
 * functions needed to update the menu, and activate SpecYrs.
 */

#define INCL_WIN
#define INCL_DOS
#include "..\include\iti.h"
#include "..\include\itimenu.h"
#include "..\include\winid.h"
#include "..\include\itiwin.h"
#include "..\include\itiglob.h"
#include "..\include\itierror.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itiutil.h"
#include "..\include\itiperm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SpecYr.h"
#include "init.h"



static USHORT ausWindowID   [MAX_YEARS];


USHORT SpecYrErr (PSZ psz, USHORT uId)
   {
   char  szTmp [STRLEN];

   sprintf (szTmp, "SpecYr.c : %s %d", psz, uId);
   ItiErrWriteDebugMessage (psz);
   WinMessageBox (HWND_DESKTOP, HWND_DESKTOP, szTmp,
                  "SpecYr selection error", 0, MB_OK | MB_APPLMODAL | MB_MOVEABLE);
   return 1;
   }


BOOL InitOpenSpecYrMenu (HWND hwndOpenSpecYrMenu, PSZ  pszAppName)
   {
   USHORT   i;
   HQRY     hqry;
   USHORT   usState, usCols;
   PSZ      *ppszData;
   CHAR szQuery[] = SPYRQUERY ;

   hqry = ItiDbExecQuery (szQuery,hheapGlobal,0,0,0,&usCols,&usState);
   if (hqry == NULL)
      return FALSE;

   usNumberOfSpecYrs = 0;
   for (i=0; i < MAX_YEARS && ItiDbGetQueryRow(hqry,&ppszData,&usState); i++)
      {
      usSpecYrIndex = i;
      if ( (ppszData) && (ppszData[0] != NULL)
           && ( 0 != ItiStrCmp(ppszData[0], "1900") 
           && ( ppszData[0][0] != '0') ))
         {
         ItiMenuInsertMenuItem (hwndOpenSpecYrMenu, ppszData[0],         
                                IDM_SPECYR_FIRST + i);
         }

      /* -- free query results data */
      ItiFreeStrArray (hheapGlobal, ppszData, usCols);
      } /* end of for... */
                       
   usNumberOfSpecYrs = i;

   return TRUE;
   }



BOOL EXPORT ItiMenuInitOpenSpecYrMenuPopup (HWND hwndOpenSpecYrMenu)
   {
   return TRUE;
   }




/*
 * ItiMenuSetSpecYr opens the window specified by usCommand.  usCommand
 * is SHORT1FROMMP (mp1) of the WM_COMMAND message.
 *
 * Parameters: hwndAppClient  A handle to the application's client window.
 *
 *             usCommand      SHORT1FROMMP (mp1) from the WM_COMMAND 
 *                            message. 
 *
 * Return Value: A handle to the client area of opened SpecYr window, 
 * or NULL on error.
 *
 * Comments: Possible errors include:
 *          usCommand out of range (IDM_SPECYR_FIRST through
 *          IDM_SPECYR_LAST)
 *
 *          Not enough memory to create window.
 */

BOOL EXPORT ItiMenuSetSpecYr (USHORT usCommand)
   {
   CHAR  szMsg[256] = "";
   PGLOBALS pglobSpecYr = NULL;
   HHEAP hhpLocSetSY = NULL;

   if (usCommand < IDM_SPECYR_FIRST ||
       usCommand > (USHORT) IDM_SPECYR_LAST + usNumberOfSpecYrs)
      {
      return FALSE;
      }

   usCommand -= IDM_SPECYR_FIRST;

   hhpLocSetSY = ItiMemCreateHeap (1024);
   pglobSpecYr = ItiGlobQueryGlobalsPointer();

   /* -- Set the current SpecYear selection here. */
   sprintf (szMsg,
      "IF EXISTS (select UserKey from CurrentSpecYear where UserKey = %s ) "
           "UPDATE CurrentSpecYear SET SpecYear = %s where UserKey = %s "
       " ELSE INSERT INTO CurrentSpecYear (UserKey, SpecYear) VALUES ( %s, %s ) ",
       pglobSpecYr->pszUserKey,
       aszSpecYr[usCommand],
       pglobSpecYr->pszUserKey,
       pglobSpecYr->pszUserKey,
       aszSpecYr[usCommand] );

   if (ItiDbExecSQLStatement (hhpLocSetSY, szMsg))
      {
      WinMessageBox (HWND_DESKTOP, pglobSpecYr->hwndAppFrame,
                     "Could not change current SpecYear.",
                     "Change Spec Year", 0, MB_OK | MB_MOVEABLE);
      if (hhpLocSetSY != NULL)
         ItiMemDestroyHeap (hhpLocSetSY);
      return FALSE;
      }

   pglobSpecYr->pszCurrentSpecYear = aszSpecYr[usCommand];


   sprintf (szMsg, "MESSAGE: The SpecYear is now set to %s. ", aszSpecYr[usCommand]);
   ItiErrWriteDebugMessage (szMsg);

   ItiDbUpdateBuffers("MajorItem");
   ItiDbUpdateBuffers("StandardItem");
   ItiDbUpdateBuffers("Job");
   ItiDbUpdateBuffers("Project");
   ItiDbUpdateBuffers("HistoricalProject");
   ItiDbUpdateBuffers("HistoricalProposal");

   if (hhpLocSetSY != NULL)
      ItiMemDestroyHeap (hhpLocSetSY);

   return TRUE;
   } /* End Of Function ItiMenuSetSpecYr */


