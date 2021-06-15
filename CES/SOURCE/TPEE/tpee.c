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
 * tpee.c
 * Mark Hampton 
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This program acts like the unix utility tee, except that 
 * a pipe is read rather than stdin.  By default, the pipe
 * \pipe\tpee is read.
 */

#define INCL_DOS
#include <os2.h>
#include <stdio.h>
#include "arg.h"
#include "errorcod.h"

#define ARG_PIPE   0
#define ARG_NO_FILE     1
#define NUM_ARGS        2

ARGBLK args [NUM_ARGS] = 
   {
   { "p",      "\\pipe\\tpee",   0 },
   { "nofile", NULL, 0 }
   };



static void usage (void)
   {
   fprintf (stderr, "usage: tpee [-p pipename] { -nofile | outfile }\n");
   }

int _cdecl main (int argc, char *argv [])
   {
   USHORT   usError;
   HFILE    hfPipe;
   HFILE    hfOutFile;
   USHORT   usAction;
   USHORT   usBytesRead;
   USHORT   usBytesWritten;
   char     szBuf [1];
   int i;

#ifdef DEBUG
   for (i=0; i<argc; i++)
      fprintf (stderr, "Argv[%d]=\"%s\"\n", i, argv[i]);
#endif

   if (argc == 1)
      {
      usage ();
      getchar ();
      return 1;
      }

   i = ProcessParams (argv + 1, args, NUM_ARGS, &usError);

   if (usError)
      {
      usage ();
      getchar ();
      return 1;
      }

   usError = DosOpen (args [ARG_PIPE].pszParam, &hfPipe, &usAction,
                      0, 0, FILE_OPEN, 
                      OPEN_ACCESS_READONLY | OPEN_SHARE_DENYNONE, 0);
   
   if (usError)
      {
      fprintf (stderr, "Could not open file %s: %s\n", 
               args [ARG_PIPE].pszParam, ErrorCodeText (usError));
      getchar ();
      return usError;
      }

   if (args [ARG_NO_FILE].uiCount)
      {
      args [ARG_NO_FILE].pszParam = "nul";
      }
   else
      {
      args [ARG_NO_FILE].pszParam = argv [i];
      }

   usError = DosOpen (args [ARG_NO_FILE].pszParam, &hfOutFile, &usAction,
                      0, 0, FILE_CREATE | FILE_TRUNCATE, 
                      OPEN_ACCESS_WRITEONLY | OPEN_SHARE_DENYWRITE, 0);

   if (usError)
      {
      fprintf (stderr, "Could not open file %s: %s\n", 
               args [ARG_NO_FILE].pszParam, ErrorCodeText (usError));
      getchar ();
      return usError;
      }

   while (DosRead (hfPipe, szBuf, sizeof szBuf, &usBytesRead) == 0 &&
          usBytesRead != 0)
      {
      DosWrite (1, szBuf, usBytesRead, &usBytesWritten);
      DosWrite (hfOutFile, szBuf, usBytesRead, &usBytesWritten);
      }

   return 0;
   }

