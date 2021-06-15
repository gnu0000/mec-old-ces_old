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


/*
 * Print.c
 * Mark Hampton
 *
 * This module processes the Print menu choice.  It contains the
 * functions needed to update the menu, and activate print dialog boxes.
 */

#define INCL_WIN
#define INCL_DOS
#include "..\include\iti.h"
#include "..\include\itimenu.h"
#include "..\include\winid.h"
#include "..\include\itiwin.h"
#include "..\include\itierror.h"
#include "..\include\itibase.h"
#include "..\include\itiglob.h"
#include "..\include\itidbase.h"
#include "..\include\message.h"
#include "..\include\itiutil.h"

#include <stdio.h>
#include "init.h"

#define MAX_MODULES  50


typedef struct 
   {
   USHORT      usWindowID;
   HMODULE     hmodPrint;
   PAVAILFN    pfnAvail;
   } PRINTINFO;

typedef PRINTINFO *PPRINTINFO;

static USHORT usNumPrintCommands = 0;

static PRINTINFO pinfo [MAX_MODULES];




USHORT PrintErr (PSZ psz, USHORT uId)
   {
   char  szTmp [STRLEN];

   sprintf (szTmp, "print.c : %s %d", psz, uId);
   ItiErrWriteDebugMessage (psz);
   WinMessageBox (HWND_DESKTOP, HWND_DESKTOP, szTmp,
                  "Print Error", 0, MB_OK | MB_APPLMODAL | MB_MOVEABLE);
   return 1;
   }


static PSZ pszQuery = 
   "SELECT ReportID "
   "FROM Application, ApplicationReport, Report "
   "WHERE Application.ApplicationKey = ApplicationReport.ApplicationKey "
   "AND Report.ReportKey = ApplicationReport.ReportKey "
   "AND Application.ApplicationID = '%s' "
   "ORDER BY ReportID ";


BOOL InitPrintMenu (HWND hwndPrintMenu, PSZ  pszAppName)
   {
   char szTemp [100];
   char szTempQuery [1000];
   PFNMENUNAME pfnMenuName;
   USHORT   i;
   HQRY     hqry;
   USHORT   usState, usCols;
   PSZ      *ppszData;

   sprintf (szTempQuery, pszQuery, pszAppName);

   hqry = ItiDbExecQuery (szTempQuery, hheapGlobal, 0, 0, 0, &usCols, &usState);
   if (hqry == NULL)
      return FALSE;

   usNumPrintCommands = 0;
   for (i=0; i < MAX_MODULES && 
             ItiDbGetQueryRow (hqry, &ppszData, &usState); i++)
      {
      pinfo [i].hmodPrint = ItiDllLoadDll (ppszData [0], "0");
      if (pinfo [i].hmodPrint)
         {
         pfnMenuName = ItiDllQueryProcAddress (pinfo [i].hmodPrint, 
                                               "ITIDLLQUERYMENUNAME");
         if (pfnMenuName != NULL)
            {
            pfnMenuName (szTemp, sizeof szTemp, &(pinfo [i].usWindowID));
            ItiMenuInsertMenuItem (hwndPrintMenu, szTemp,         
                                  IDM_FILE_PRINT_FIRST + i);
            if (pinfo [i].usWindowID == 0)
               {
               ItiMenuGrayMenuItem (hwndPrintMenu, 
                                    IDM_FILE_PRINT_FIRST + i, TRUE);
               }
            pinfo [i].pfnAvail = ItiDllQueryProcAddress (pinfo [i].hmodPrint, 
                                               "ITIDLLQUERYAVAILABILITY");
            }
         else
            {
            /* could not find function in DLL.  Make a note of it. */
            sprintf (szTemp, "Print name unavilable in %s", 
                     ppszData [0]);
            ItiMenuInsertMenuItem (hwndPrintMenu, szTemp,         
                                   IDM_FILE_PRINT_FIRST + i);
            ItiMenuGrayMenuItem (hwndPrintMenu, 
                                IDM_FILE_PRINT_FIRST + i,
                                TRUE);
            /* free the module */
            ItiDllFreeDll (pinfo [i].hmodPrint);
            pinfo [i].hmodPrint = 0;
            }
         }
      else
         {
         /* could not load this DLL.  Make a note of it. */
         sprintf (szTemp, "Could not load dll %s", ppszData [0]);
         ItiMenuInsertMenuItem (hwndPrintMenu, szTemp,         
                                IDM_FILE_PRINT_FIRST + i);
         ItiMenuGrayMenuItem (hwndPrintMenu, 
                             IDM_FILE_PRINT_FIRST + i,
                             TRUE);
         }

      /* free query results data */
      ItiFreeStrArray (hheapGlobal, ppszData, usCols);
      }

   usNumPrintCommands = i;

   return TRUE;
   }



BOOL EXPORT ItiMenuInitPrintMenuPopup (HWND hwndPrintMenu)
   {
   USHORT i;
   USHORT usActive;
   HWND hwndActive;

   hwndActive = WinSendMsg (pglobals->hwndAppFrame, WM_QUERYACTIVEDOC, 0, 0);
   usActive = (UM) QW (hwndActive, ITIWND_ID, 0, 0, 0);
   for (i=0; i < usNumPrintCommands; i++)
      {
      if (pinfo [i].hmodPrint != 0 && pinfo [i].pfnAvail != NULL)
         {
         ItiMenuGrayMenuItem (hwndPrintMenu, 
                              IDM_FILE_PRINT_FIRST + i,
                              hwndActive == NULL ? TRUE :
                              !pinfo[i].pfnAvail (pinfo [i].usWindowID, usActive));
         }
      }
   return TRUE;
   }




/*
 * ItiMenuPrint opens the window specified by usCommand.  usCommand
 * is SHORT1FROMMP (mp1) of the WM_COMMAND message.
 *
 * Parameters: hwndAppClient  A handle to the application's client window.
 *
 *             usCommand      SHORT1FROMMP (mp1) from the WM_COMMAND 
 *                            message. 
 *
 * Return Value: A handle to the client area of opened catalog window, 
 * or NULL on error.
 *
 * Comments: Possible errors include:
 *          usCommand out of range (IDM_FILE_PRINT_FIST through
 *          IDM_FILE_PRINT_LAST)
 *
 *          Not enough memory to create window.
 */

void EXPORT ItiMenuPrint (HWND   hwndAppClient,
                          USHORT usCommand)
   {
   HWND hwndActive;
   HWND hwndStatic;

   if (usCommand < IDM_FILE_PRINT_FIRST ||
       usCommand > (USHORT) IDM_FILE_PRINT_LAST + usNumPrintCommands ||
       hwndAppClient == NULL)
      {
      return NULL;
      }

   usCommand -= IDM_FILE_PRINT_FIRST;

   hwndActive = WinSendMsg (pglobals->hwndAppFrame, WM_QUERYACTIVEDOC, 0, 0);
   hwndStatic = WinWindowFromID (hwndActive, FID_CLIENT);
   if (pinfo [usCommand].hmodPrint != 0)
      {
      ItiWndBuildWindow (hwndStatic, pinfo [usCommand].usWindowID);
      }
#ifdef DEBUG
   else
      {
      PrintErr ("Attempt to open non-existant catalog (Menu Offset = %u) in ItiMenuPrint",
                usCommand);
      }
#endif
   }


