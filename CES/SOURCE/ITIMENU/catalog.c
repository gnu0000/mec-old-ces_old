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
 * Catalog.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This module processes the Open Catalog menu choice.  It contains the
 * functions needed to update the menu, and activate catalogs.
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
#include "catalog.h"
#include "init.h"


#define MAX_MODULES  50

static USHORT usNumCatalogCommands = 0;

static HMODULE hmodCatalogs [MAX_MODULES];
static USHORT ausWindowID   [MAX_MODULES];

static USHORT usUserCatIndex = 0;


USHORT CatErr (PSZ psz, USHORT uId)
   {
   char  szTmp [STRLEN];

   sprintf (szTmp, "catalog.c : %s %d", psz, uId);
   ItiErrWriteDebugMessage (psz);
   WinMessageBox (HWND_DESKTOP, HWND_DESKTOP, szTmp,
                  "Catalog Error", 0, MB_OK | MB_APPLMODAL | MB_MOVEABLE);
   return 1;
   }


static PSZ pszQuery = 
   "SELECT CatalogID "
   "FROM Application, ApplicationCatalog, Catalog "
   "WHERE Application.ApplicationKey = ApplicationCatalog.ApplicationKey "
   "AND Catalog.CatalogKey = ApplicationCatalog.CatalogKey "
   "AND Application.ApplicationID = '%s' "
   "ORDER BY CatalogID ";


BOOL InitOpenCatalogMenu (HWND hwndOpenCatalogMenu, PSZ  pszAppName)
   {
   char szTemp [100];
   char szTempQuery [1000];
   PFNMENUNAME pfnMenuName;
   USHORT   i;
   HQRY     hqry;
   USHORT   usState, usCols;
   PSZ      *ppszData;
   PSZ      psz;

   if ((psz = getenv ("JOE")) &&
       strcmp (psz, "098ecg mpj-047d-034z-2,23-48u3-cpu845ylkjfs,mv0974-2 ") == 0)
      {
      WinQueryWindowText (pglobals->hwndAppFrame, sizeof szTempQuery, szTempQuery);
      strcat (szTempQuery, " - Database: ");
      psz = getenv ("DATABASE");
      if (!psz)
         psz = "DSShell!";
      strcat (szTempQuery, psz);
      WinSetWindowText (pglobals->hwndAppFrame, szTempQuery);
      }

   sprintf (szTempQuery, pszQuery, pszAppName);

   hqry = ItiDbExecQuery (szTempQuery, hheapGlobal, 0, 0, 0, &usCols, &usState);
   if (hqry == NULL)
      return FALSE;

   usNumCatalogCommands = 0;
   for (i=0; i < MAX_MODULES && 
             ItiDbGetQueryRow (hqry, &ppszData, &usState); i++)
      {
//      hmodCatalogs [i] = ItiDllLoadDll (ppszData [0], VERSION_STRING);
      hmodCatalogs [i] = ItiDllLoadDll (ppszData [0], "1");
      if (hmodCatalogs [i])
         {
         pfnMenuName = ItiDllQueryProcAddress (hmodCatalogs [i], 
                                               "ITIDLLQUERYMENUNAME");
         if (pfnMenuName != NULL)
            {
            pfnMenuName (szTemp, sizeof szTemp, &(ausWindowID [i]));
            if (ausWindowID [i] == UserCatS)
               usUserCatIndex = i;
            ItiMenuInsertMenuItem (hwndOpenCatalogMenu, szTemp,         
                                  IDM_FILE_OPEN_CATALOG_FIRST + i);
            if (ausWindowID [i] == 0)
               {
               ItiMenuGrayMenuItem (hwndOpenCatalogMenu, 
                                    IDM_FILE_OPEN_CATALOG_FIRST + i, TRUE);
               }
            }
         else
            {
            /* could not find function in DLL.  Make a note of it. */
            sprintf (szTemp, "Catalog name unavilable in %s", 
                     ppszData [0]);
            ItiMenuInsertMenuItem (hwndOpenCatalogMenu, szTemp,         
                                   IDM_FILE_OPEN_CATALOG_FIRST + i);
            ItiMenuGrayMenuItem (hwndOpenCatalogMenu, 
                                IDM_FILE_OPEN_CATALOG_FIRST + i,
                                TRUE);
            /* free the module */
            ItiDllFreeDll (hmodCatalogs [i]);
            hmodCatalogs [i] = 0;
            }
         }
      else
         {
         /* could not load this DLL.  Make a note of it. */
         sprintf (szTemp, "Could not load dll %s", ppszData [0]);
         ItiMenuInsertMenuItem (hwndOpenCatalogMenu, szTemp,         
                                IDM_FILE_OPEN_CATALOG_FIRST + i);
         ItiMenuGrayMenuItem (hwndOpenCatalogMenu, 
                             IDM_FILE_OPEN_CATALOG_FIRST + i,
                             TRUE);
         }

      /* free query results data */
      ItiFreeStrArray (hheapGlobal, ppszData, usCols);
      }

   usNumCatalogCommands = i;

   return TRUE;
   }



BOOL EXPORT ItiMenuInitOpenCatalogMenuPopup (HWND hwndOpenCatalogMenu)
   {
   if (usUserCatIndex)
      ItiMenuGrayMenuItem (hwndOpenCatalogMenu,
                           IDM_FILE_OPEN_CATALOG_FIRST + usUserCatIndex,
                           !ItiPermQueryUserCatAvail ());
   return TRUE;
   }




/*
 * ItiMenuOpenCatalog opens the window specified by usCommand.  usCommand
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
 *          usCommand out of range (IDM_FILE_OPEN_CATALOG_FIST through
 *          IDM_FILE_OPEN_CATALOG_LAST)
 *
 *          Not enough memory to create window.
 */

HWND EXPORT ItiMenuOpenCatalog (HWND   hwndAppClient,
                                USHORT usCommand)
   {
   HWND hwndClient;
   
   if (usCommand < IDM_FILE_OPEN_CATALOG_FIRST ||
       usCommand > (USHORT) IDM_FILE_OPEN_CATALOG_LAST + usNumCatalogCommands ||
       hwndAppClient == NULL)
      {
      return NULL;
      }

   usCommand -= IDM_FILE_OPEN_CATALOG_FIRST;

   if (hmodCatalogs [usCommand] != 0)
      {
      hwndClient = ItiWndBuildWindow (hwndAppClient, ausWindowID [usCommand]);
      if (hwndClient == NULL)
         CatErr ("Unable to create window, Id=", ausWindowID [usCommand]);
      }
#ifdef DEBUG
   else
      {
      char szTemp [100];

      sprintf (szTemp, "Attempt to open non-existant catalog (Menu Offset = %u) in ItiMenuOpenCatalog",
               usCommand);
      ItiErrWriteDebugMessage (szTemp);
      }
#endif

   return hwndClient;
   }


