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
 * DoImport.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This module processes the "File Import..." menu choice.  
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
#include "..\include\itimbase.h"
#include "..\include\opendlg.h"
#include "..\include\itiutil.h"
#include "..\include\dssimp.h"
#include <stdio.h>
#include "init.h"
#include "doimport.h"
#include <process.h>


USHORT EXPORT ItiMenuDoExport (HWND hwnd)
   {
   DLF      dlf;
   USHORT   usError;
   HFILE    hf;
   char     szWhere [4096];
   char     szTable [64];

   dlf.rgbAction        = DLG_SAVEDLG | DLG_NOOPEN | DLG_HELP;
   dlf.rgbFlags         = BITATTRDRIVE | BITATTRDIR;
   dlf.phFile           = &hf;
   dlf.pszExt           = "\\*.csv";
   dlf.pszAppName       = pglobals->pszAppName;
   dlf.pszTitle         = "Export";
   dlf.pszInstructions  = "Type the name of the file to export in the file name entry field.";
   ItiStrCpy (dlf.szFileName, "\\*.csv", sizeof dlf.szFileName);
   ItiStrCpy (dlf.szOpenFile, "", sizeof dlf.szOpenFile);
   ItiStrCpy (dlf.szLastWild, "\\*.csv", sizeof dlf.szLastWild);
   ItiStrCpy (dlf.szLastFile, "", sizeof dlf.szLastFile);

   usError = DlgFile (hwnd, &dlf);
   if (usError == TDF_OLDSAVE || usError == TDF_NEWSAVE)
      {
      if (!ItiExpGetWhereClause (szWhere, sizeof szWhere, szTable, sizeof szTable))
         return 0;
      spawnl (P_NOWAIT, "export.exe", "export.exe", dlf.szFileName,
              szTable, szWhere, NULL);
      }
   return 0;
   }


USHORT EXPORT ItiMenuDoImport (HWND hwnd)
   {
   DLF      dlf;
   USHORT   usError;
   HFILE    hf;

   dlf.rgbAction        = DLG_OPENDLG | DLG_NOOPEN | DLG_HELP;
   dlf.rgbFlags         = BITATTRDRIVE | BITATTRDIR;
   dlf.phFile           = &hf;
   dlf.pszExt           = "\\*.csv";
   dlf.pszAppName       = pglobals->pszAppName;
   dlf.pszTitle         = "Import";
   dlf.pszInstructions  = "Select the file you want to import, and press OK.  Press Cancel to abort.";
   ItiStrCpy (dlf.szFileName, "\\*.csv", sizeof dlf.szFileName);
   ItiStrCpy (dlf.szOpenFile, "", sizeof dlf.szOpenFile);
   ItiStrCpy (dlf.szLastWild, "\\*.csv", sizeof dlf.szLastWild);
   ItiStrCpy (dlf.szLastFile, "", sizeof dlf.szLastFile);

   usError = DlgFile (hwnd, &dlf);
   if (usError == TDF_OLDOPEN)
      {
      spawnl (P_NOWAIT, "import.exe", "import.exe", dlf.szFileName, NULL);
      }
   return 0;
   }


USHORT EXPORT ItiMenuDaDoRunRunRun (HWND hwnd)
   {
   DLF      dlf;
   USHORT   usError;
   HFILE    hf;

   dlf.rgbAction        = DLG_OPENDLG | DLG_NOOPEN | DLG_HELP;
   dlf.rgbFlags         = BITATTRDRIVE | BITATTRDIR;
   dlf.phFile           = &hf;
   dlf.pszExt           = "\\*.exe";
   dlf.pszAppName       = pglobals->pszAppName;
   dlf.pszTitle         = "Run";
   dlf.pszInstructions  = "Select the file you want to import, and press OK.  Press Cancel to abort.";
   ItiStrCpy (dlf.szFileName, "\\*.exe", sizeof dlf.szFileName);
   ItiStrCpy (dlf.szOpenFile, "", sizeof dlf.szOpenFile);
   ItiStrCpy (dlf.szLastWild, "\\*.exe", sizeof dlf.szLastWild);
   ItiStrCpy (dlf.szLastFile, "", sizeof dlf.szLastFile);

   usError = DlgFile (hwnd, &dlf);
   if (usError == TDF_OLDOPEN)
      {
      spawnl (P_NOWAIT, dlf.szFileName, NULL);
      }
   return 0;
   }
