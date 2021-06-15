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
 * GetInfo.c
 *
 * (C) 1992 Info Tech Inc.
 * This file is part if the EBS module
 */


#define INCL_VIO
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <process.h>
#include <direct.h>
#include <io.h>
#include "ebs.h"
#include "scr.h"
#include "kbd.h"
#include "util.h"
#include "stuff.h"
#include "getinfo.h"
#include "install.h"
#include "error.h"







BOOL SourceDrive (PCHAR pc, PSZ pszTestFile)
   {
   PGNUWIN pgw;
   BOOL    bFound;

   pgw = CreateGnuWin (60, 11);

   *pc = 'A';
   bFound = FALSE;
   while (!bFound && *pc != '\x1b')
      {
      ClearWin (pgw);
      PaintText (pgw,  2, 30, 2, 1, "INSTALLATION SOURCE DRIVE");
      PaintText (pgw,  3, 30, 2, 1, "-------------------------");
      PaintText (pgw,  5,  2, 0, 0, "Please enter the drive letter of the installation");
      PaintText (pgw,  6,  2, 0, 0, "disk (A or B), or <ESC> to abort installation.");
      PaintText (pgw,  8, 30, 2, 1, "-> <-");

      VioSetCurPos (YWinPos(pgw) + 8, XWinPos(pgw)+30, (HVIO)NULL);
      ShowCursor (TRUE);
      *pc = (char)GetKey ("ABCDEFGHIJKLMNOPQRSTUVWXYZ\x1B");
      ShowCursor (FALSE);

      if (*pc != '\x1b')
         {
         if (pszTestFile)
            {
            bFound = CheckForFile (pszTestFile, *pc);

            if (!bFound)
               {
               ClearWin (pgw);
               PaintText (pgw,  2, 30, 2, 1, "ERROR");
               PaintText (pgw,  3, 30, 2, 1, "-----");
               PaintText (pgw,  5,  2, 0, 0, "The drive you specified does not contain the needed");
               PaintText (pgw,  6,  2, 0, 0, "installation files.  Make sure the drive contains");
               PaintText (pgw,  7,  2, 0, 0, "Setup disk 1.");
               PaintText (pgw,  9, 30, 2, 1, "Press Enter to try again, or Escape to exit.");

               if ('\x1b' == GetKey ("\x1B\x0D"))
                  Quit (1);
               }
            }
         else
            bFound = TRUE;
         }
      }

   DestroyGnuWin (pgw);
   return (*pc != '\x1B');
   }


BOOL DestDrive (PCHAR pc)
   {
   PGNUWIN pgw;
   BOOL    bFound;

   pgw = CreateGnuWin (60, 11);

   *pc = 'A';
   bFound = FALSE;
   while (!bFound && *pc != '\x1b')
      {
      ClearWin (pgw);
      PaintText (pgw,  2, 30, 2, 1, "INSTALLATION DESTINATION DRIVE");
      PaintText (pgw,  3, 30, 2, 1, "------------------------------");
      PaintText (pgw,  5,  2, 0, 0, "Please enter the drive letter of the hard disk to");
      PaintText (pgw,  6,  2, 0, 0, "install to, or <ESC> to abort installation.");
      PaintText (pgw,  8, 30, 2, 1, "-> <-");

      VioSetCurPos (YWinPos(pgw) + 8, XWinPos(pgw)+30, (HVIO)NULL);
      ShowCursor (TRUE);
      *pc = (char)GetKey ("ABCDEFGHIJKLMNOPQRSTUVWXYZ\x1B");
      ShowCursor (FALSE);
      if (*pc != '\x1b')
         {
         bFound = DriveExists (*pc);

         if (!bFound)
            {
            ClearWin (pgw);
            PaintText (pgw,  2, 30, 2, 1, "ERROR");
            PaintText (pgw,  3, 30, 2, 1, "-----");
            PaintText (pgw,  5,  2, 0, 0, "The drive you specified does not exist.");
            PaintText (pgw,  9, 30, 2, 1, "Press Enter to try again, or Escape to exit.");

            if ('\x1b' == GetKey ("\x1B\x0D"))
               Quit (1);
            }
         }
      }
   
   DestroyGnuWin (pgw);
   return (*pc != '\x1B');
   }


BOOL DestPath (PCHAR pcDestDrv, PCHAR pszDestPath)
   {
   strcpy (pszDestPath, "\\BAMS");
   return TRUE;
   }



BOOL Confirm (CHAR   cSrcDrive, 
              CHAR   cDestDrv, 
              PSZ    pszDestPath, 
              PSZ    pszDBPath)
   {
   PGNUWIN pgw;
   int     c;

   pgw = CreateGnuWin (60, 14);

   PaintText (pgw,  2, 30, 2, 1, "CONFIRMATION");
   PaintText (pgw,  3, 30, 2, 1, "------------");
   PaintText (pgw,  5,  2, 0, 0, "You have entered the following parameters:");
   sprintf (sz, "     Source Drive : %c", cSrcDrive);
   PaintText (pgw,  7,  2, 0, 0, sz);
   sprintf (sz, "Destination Drive : %c", cDestDrv);
   PaintText (pgw,  8,  2, 0, 0, sz);
   if (pszDBPath)
      {
      sprintf (sz, "    Database Path : %s", pszDBPath);
      PaintText (pgw,  9,  2, 0, 0, sz);
      }

   PaintText (pgw, 11, 30, 2, 1, "Are they correct ? (Y or N)");

   c = GetKey ("YN\x1b");
   DestroyGnuWin (pgw);
   return (c == 'Y');
   }


BOOL CheckCreatePath (PCHAR pcDestDrv, PSZ pszDestPath)
   {
   PGNUWIN  pgw;
   int      c;
   char     szFullPath [256];
   char     szOldPath [256];
   char     szTemp [256];
   BOOL     bExists;

   sprintf (szFullPath, "%c:%s", *pcDestDrv, pszDestPath);

   pgw = CreateGnuWin (60, 12);

   _getdcwd (*pcDestDrv - '@', szOldPath, sizeof szOldPath);
   bExists = !chdir (szFullPath);
   if (bExists)
      {
      PaintText (pgw,  2, 30, 2, 1, "Destination Path");
      PaintText (pgw,  3, 30, 2, 1, "----------------");
      PaintText (pgw,  5,  2, 0, 0, "the path:");
      PaintText (pgw,  6,  2, 0, 1, szFullPath);
      PaintText (pgw,  7,  2, 0, 0, "already exists");
      PaintText (pgw,  9,  2, 0, 1, "Are you sure you want to use it? (Y or N)");
      chdir (szOldPath);
      }
   else
      {
      PaintText (pgw,  2, 30, 2, 1, "CREATE PATH");
      PaintText (pgw,  3, 30, 2, 1, "-----------");
      PaintText (pgw,  5,  2, 0, 0, "the path:");
      PaintText (pgw,  6,  2, 0, 1, szFullPath);
      PaintText (pgw,  7,  2, 0, 0, "does not exist");
      PaintText (pgw,  9,  2, 0, 1, "Would you like it created ? (Y or N)");
      }

   c = GetKey ("YN\x1b");
   DestroyGnuWin (pgw);

   if (c != 'Y')
      return 0;

   if (bExists || MakePath (szFullPath))
      {
      /* go ahead and make some other directories */
      sprintf (szTemp, "%s\\TEMP", szFullPath);
      MakePath (szTemp);

      sprintf (szTemp, "%s\\EXE", szFullPath);
      MakePath (szTemp);

      return 1;
      }

   pgw = CreateGnuWin (60, 12);

   PaintText (pgw,  2, 30, 2, 1, "CREATE PATH ERROR");
   PaintText (pgw,  3, 30, 2, 1, "-----------------");
   PaintText (pgw,  5,  2, 0, 0, "the path:");
   PaintText (pgw,  6,  5, 0, 1, szFullPath);
   PaintText (pgw,  7,  2, 0, 0, "could not be created");
   PaintText (pgw,  9,  2, 0, 1, "press <Enter> to continue");

   GetKey ("\x1B\x0D");
   DestroyGnuWin (pgw);

   return 0;
   }



BOOL UnzipFile (PGNUWIN pgw,
                CHAR    cSrcDrive, 
                CHAR    cDestDrive, 
                PSZ     pszDestPath, 
                PSZ     pszFile,
                FILE    *pfLog)
   {
   char     *argv [6];
   int      iRet;
   int      iOldStdOut;
   int      iOldStdErr;
   char     szTemp [256];

   /* spawn pkunzip */
   sprintf (szTemp, "%c:pkunzip.exe", cSrcDrive);
   argv [0] = strdup (szTemp);
   sprintf (szTemp, "-d");
   argv [1] = strdup (szTemp);
   sprintf (szTemp, "-o");
   argv [2] = strdup (szTemp);
   sprintf (szTemp, "%c:%s", cSrcDrive, pszFile);
   argv [3] = strdup (szTemp);
   sprintf (szTemp, "%c:\\", cDestDrive);
   argv [4] = strdup (szTemp);
   argv [5] = NULL;

   if (pfLog != NULL)
      {
      iOldStdOut = dup (1);
      iOldStdErr = dup (2);
      dup2 (fileno (pfLog), 1);
      dup2 (fileno (pfLog), 2);
      }

   iRet = spawnv (P_WAIT, argv [0], argv);
   if (pfLog != NULL)
      {
      dup2 (iOldStdOut, 1);
      dup2 (iOldStdErr, 2);
      }

   if (-1 == iRet)
      {
      InstError (pgw, FALSE, 
                 "The uncompression program could not be started",
                 strerror (errno),
                 "Please check the source drive and try installing again.");
      DestroyGnuWin (pgw);
      Quit (1);
      }

   return iRet;
   }


