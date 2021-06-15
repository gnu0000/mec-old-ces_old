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
 * Export.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 * This file is part of DS/Shell.
 *
 * This module exports files from the BAMS/PC database.
 */

#define INCL_DOSMISC
#include "..\include\iti.h"
#include "..\include\itiutil.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itifmt.h"
#include "..\include\itierror.h"
#include "..\include\itiglob.h"
#include <stdio.h>
#include <share.h>
#include <string.h>
#include "..\include\itiimp.h"
#include "export.h"

/* Global variables */
PGLOBALS pglobals;

static void Usage (void);
static BOOL InitExport (HHEAP hheap);

static BOOL ExportFiles (HHEAP   hheap, 
                         PSZ     pszFileName, 
                         PSZ     pszTableName,
                         PSZ     pszWhere);


int _cdecl main (int argc, char *argv [])
   {
   HHEAP    hheap;
   int      i;
   PSZ      pszWhere;
   USHORT   usLen, j;

#ifdef PRERELEASE
   ItiErrWriteDebugMessage ("DS/Shell Export Release "VERSION_STRING" "PRERELEASE);
   puts ("DS/Shell Export Release "VERSION_STRING" "PRERELEASE);
#else
   ItiErrWriteDebugMessage ("DS/Shell Export Release "VERSION_STRING);
   puts ("DS/Shell Export Release "VERSION_STRING);
#endif
   puts ("Copyright (c) "COPYRIGHT" Info Tech, Inc.");
   printf ("Log file is: %s.  (Note this in case of error)\n",
           ItiErrQueryLogFileName ());

   for (i=0; i < argc; i++)
      ItiErrWriteDebugMessage (argv [i]);

   if (argc < 3)
      {
      Usage ();
      return 1;
      }

   hheap = ItiMemCreateHeap (0);
   if (hheap == NULL)
      {
      printf ("Could not create a heap!\n");
      return 1;
      }

   if (!InitExport (hheap))
      {
      printf ("Could not initialize the export program!\n");
      return 4;
      }

   pszWhere = ItiMemAlloc (hheap, 1, 0);
   if (pszWhere == NULL)
      {
      printf ("Could not allocate from heap!\n");
      return 5;
      }
   *pszWhere = '\0';
   usLen = 0;
   for (i=3; i < argc; i++)
      {
      j = strlen (argv [i]);
      pszWhere = ItiMemRealloc (hheap, pszWhere, usLen + j + 2, 0);
      if (pszWhere == NULL)
         {
         printf ("Could not reallocate from heap.\n");
         return 6;
         }
      usLen += j + 1;
      ItiStrCat (pszWhere, argv [i], usLen + 1);
      ItiStrCat (pszWhere, " ", usLen + 1);
      }

/* csp - BSR 930154 - 12oct94 Initialize format module */
   if (!ItiFmtInitialize ())
      {
      printf ("Could not initialize the format program!\n");
      return 7;                                                                
      }

   ExportFiles (hheap, argv [1], argv [2], pszWhere);

   ItiMemDestroyHeap (hheap);
   ItiDbTerminate ();
   return 0;
   }



static BOOL InitExport (HHEAP hheap)
   {
   pglobals = ItiGlobInitGlobals (NULL, NULL, NULL, NULL, "Export", "Export", VERSION_STRING);
   if (pglobals == NULL)
      {
      printf ("Could not initialize the globals module!\n");
      return FALSE;
      }
   pglobals->usVersionMajor = VERSION_MAJOR;
   pglobals->usVersionMinor = VERSION_MINOR;
   pglobals->pszVersionString = VERSION_STRING;
   pglobals->ucVersionChar = VERSION_CHAR;

   if (!ItiFmtInitialize ())
      {
      printf ("Could not initialize the format module!\n");
      return FALSE;
      }

   if (!ItiDbInit (0, 2))
      {
      printf ("Could not initialize the database module!\n");
      return FALSE;
      }

   return TRUE;
   }

static void Usage (void)
   {
   printf ("usage: \nexport filename table-name where-clause\n");
   ItiErrWriteDebugMessage ("bad parameters!");
   }





static BOOL ExportFiles (HHEAP   hheap, 
                         PSZ     pszFileName, 
                         PSZ     pszTableName,
                         PSZ     pszWhere)
   {
   char     szQuery [512];
   FILE     *pf;
   USHORT   usNumCols;
   PSZ      *ppsz;
   PFNEXP   pfnexp;
   BOOL     bOK = FALSE;
   HMODULE  hmodDll;

   pf = _fsopen (pszFileName, "wt", SH_DENYWR);

   if (pf == NULL)
      {
      sprintf (szQuery, 
               "Could not open file %s: %s",
               pszFileName, 
               _strerror (NULL));
      puts (szQuery);
      ItiErrWriteDebugMessage (szQuery);
      return FALSE;
      }

   ItiCSVWriteLine (pf, "DS/Shell Import File", "1.00", NULL);

   sprintf (szQuery, 
            " SELECT EI.DllName, EI.FunctionName"
            " FROM ExportInfo EI, Tables T"
            " WHERE EI.TableKey=T.TableKey"
            " AND T.TableName='%s'",
            pszTableName);

   ppsz = ItiDbGetRow1 (szQuery, hheap, 0, 0, 0, &usNumCols);
   if (ppsz == NULL || ppsz [0] == NULL || *ppsz [0] == '\0')
      {
      sprintf (szQuery, 
               "Could not get metadata for table %s.",
               pszTableName);

      puts (szQuery);
      ItiErrWriteDebugMessage (szQuery);
      fclose (pf);
      return FALSE;
      }

   hmodDll = ItiDllLoadDll (ppsz [0], "1");
   pfnexp = (PFNEXP) ItiDllQueryProcAddress (hmodDll, ppsz [1]);
   if (hmodDll == 0 || pfnexp == NULL)
      {
      sprintf (szQuery, 
               "Could not load function %s in dll %s.",
               ppsz [1], ppsz [0]);

      puts (szQuery);
      ItiErrWriteDebugMessage (szQuery);
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      fclose (pf);
      return FALSE;
      }

   bOK = pfnexp (hheap, pf, pszWhere);

   ItiFreeStrArray (hheap, ppsz, usNumCols);
   ItiDllFreeDll (hmodDll);
   fclose (pf);

   return bOK;
   }

