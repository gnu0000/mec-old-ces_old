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
 * init.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This module defines global variables for the itimenu module.
 */

#include "..\include\iti.h"
#include "..\include\itierror.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimenu.h"
#include "..\include\itiglob.h"
#include "..\include\winid.h"
#include "..\include\itiwin.h"
#include "..\include\itiutil.h"
#include "..\include\itimbase.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "init.h"
#include "catalog.h"
#include "print.h"
#include "utility.h"
#include "specyr.h"

HMODULE hmodMe;

HHEAP hheapGlobal;

PGLOBALS pglobals;


/*
 * DllInit is called by either ITIDLLI.OBJ or ITIDLLC.OBJ.  This function
 * must return non-zero if the initialization succeded, or FALSE
 * if initialization failed.
 */

BOOL DLLINITPROC DllInit (HMODULE hmodDll)
   {
   /* call your module initialization functions here. */
   hmodMe = hmodDll;
   pglobals = ItiGlobQueryGlobalsPointer ();

   return TRUE;
   }/* End of function */



BOOL EXPORT ItiMenuInitialize (HWND hwndMainMenu)
   {
   HWND hwndMenu;
   USHORT   i;
   HQRY     hqry;
   USHORT   usState, usCols;
   PSZ      *ppszData;
   CHAR     szQry[] = SPYRQUERY ;


   if (hheapGlobal == NULL)
      hheapGlobal = ItiMemCreateHeap (ITIMENU_DEFAULT_HEAP_SIZE);

   if (hheapGlobal == NULL)
      return FALSE;


   hqry = ItiDbExecQuery (szQry, hheapGlobal, 0, 0, 0, &usCols, &usState);
   if (hqry == NULL)
      return FALSE;

   usNumberOfSpecYrs = 0;
   for (i=0; i < MAX_YEARS && 
             ItiDbGetQueryRow (hqry, &ppszData, &usState); i++)
      {
      usSpecYrIndex = i;
      if ( (ppszData) && (ppszData[0] != NULL)
           && ( 0 != ItiStrCmp(ppszData[0], "1900") 
           && ( ppszData[0][0] != '0') ))
         {
         ItiStrCpy (aszSpecYr[i], ppszData[0], ENTRYLENGTH);
         }
      else
         {
         ItiStrCpy (aszSpecYr[i], "0", ENTRYLENGTH);
         }

      /* -- free query results data */
      ItiFreeStrArray (hheapGlobal, ppszData, usCols);
      } /* end of for... */

   usNumberOfSpecYrs = i;





   hwndMenu = ItiMenuQuerySubmenu (hwndMainMenu, IDM_OPEN_CATALOG_MENU);
   if (hwndMenu != NULL)
      if (!InitOpenCatalogMenu (hwndMenu, pglobals->pszApplicationID))
         return FALSE;

   hwndMenu = ItiMenuQuerySubmenu (hwndMainMenu, IDM_PRINT_MENU);
   if (hwndMenu != NULL)
      if (!InitPrintMenu (hwndMenu, pglobals->pszApplicationID))
         return FALSE;

   hwndMenu = ItiMenuQuerySubmenu (hwndMainMenu, IDM_OPEN_SPECYEAR_MENU);
   if (hwndMenu != NULL)
      if (!InitOpenSpecYrMenu (hwndMenu, pglobals->pszApplicationID))
         return FALSE;

   ItiMbRegisterListWnd   (ShowUsersDL, ItiWndDefListWndTxtProc, hmodMe);
   ItiMbRegisterListWnd   (SendMessageDL, ItiWndDefListWndTxtProc, hmodMe);

   ItiMbRegisterDialogWnd (ShowUsersD, ShowUsersD, ShowUsersDProc, hmodMe);
   ItiMbRegisterDialogWnd (SendMessageD, SendMessageD, SendMessageDProc, hmodMe);

   return TRUE;
   }
