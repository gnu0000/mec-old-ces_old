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
 * CesInst.c
 *
 * (C) 1992 Info Tech Inc.
 * This file is part if the EBS module
 */


#define INCL_VIO
#define INCL_WINPROGRAMLIST
//#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <process.h>
#include <direct.h>
#include <io.h>
#include "..\include\iti.h"
#include "ebs.h"
#include "scr.h"
#include "kbd.h"
#include "util.h"
#include "stuff.h"
#include "install.h"
#include "getinfo.h"
#include "doconfig.h"
#include "error.h"
#include "arg2.h"
#include "cesinst.h"

BYTE  bcBackground[2]  = {'\xB1', 0x07};
BYTE  bcOriginal [2] = {' ', 0x07};

char sz [512];


#define FIRST_FILE   "BAMSEXE.ZIP"
#define SECOND_FILE  "BAMSDLL.ZIP"



void Quit (USHORT i)
   {
   VioScrollDn (0, 0, 0xFFFF, 0xFFFF, 0xFFFF, bcOriginal, 0);
   VioSetCurPos (0, 0, (HVIO)NULL);
   ShowCursor (TRUE);
   exit (0);
   }


USHORT Notice (void)
   {
   PGNUWIN pgw;
   int     c;

   pgw = CreateGnuWin (70, 18);

   PaintText (pgw,  2, 35, 2, 1, " BAMS/CES  Update Installation ");
   PaintText (pgw,  3, 35, 2, 1, "נננננננננננננננננננננננננננננננ");
   PaintText (pgw,  5,  2, 0, 0, "This installation program will update BAMS/CES on ");
   PaintText (pgw,  6,  2, 0, 0, "your hard disk, for use on a server or a workstation.");
   PaintText (pgw, 15, 35, 2, 1, "Press <Enter> to continue or <ESC> to exit installation.");
#ifdef PRERELEASE
   PaintText (pgw, 18, 34, 2, 1, "Release "VERSION_STRING" "PRERELEASE);
#else
   PaintText (pgw, 18, 34, 2, 1, "Release "VERSION_STRING);
#endif
   c = GetKey ("\x1B\x0D");
   DestroyGnuWin (pgw);
   return (c != '\x1B');
   }


void clr (PGNUWIN pgw)
   {
   ClearWin (pgw);
   PaintText (pgw,  2, 30, 2, 1, "BAMS/CES Update Installation");
   PaintText (pgw,  3, 30, 2, 1, "----------------------------");
   }







USHORT CopyFiles (CHAR cSrcDrive, CHAR cDestDrive, PSZ pszDestPath)
   {
   PGNUWIN  pgw;
   FILE     *pfLog;
   char     szTemp [256];
   int      iRet;

   pgw = CreateGnuWin (60, 12);
   clr (pgw);

   /*------ uncompress the file ------*/
   PaintText (pgw,  5,  30, 2, 0, "Uncompressing Files...");
   PaintText (pgw,  7,  30, 2, 0, "Please Wait");

   sprintf (szTemp, "%c:\\BAMS\\unzip.log", cDestDrive);
   pfLog = fopen (szTemp, "wt");

   iRet = UnzipFile (pgw, cSrcDrive, cDestDrive, pszDestPath, 
                     FIRST_FILE, pfLog);

   if (0 != iRet)
      {
      clr (pgw);
      PaintText (pgw,  5,  30, 2, 0, "The uncompression program encountered a problem.");
      sprintf (szTemp, "See the file %c:\\BAMS\\UNZIP.LOG", cSrcDrive);
      PaintText (pgw,  6,  30, 2, 0, szTemp);
      PaintText (pgw,  7,  30, 2, 0, "for more information and try installing again.");
      PaintText (pgw,  9,  30, 2, 0, "Press Enter to exit.");
      GetKey ("\x1B\x0D");
      DestroyGnuWin (pgw);

      Quit (1);
      }

   iRet = CheckForFile (SECOND_FILE, cSrcDrive);
   while (iRet == 0)
      {
      clr (pgw);
      sprintf (szTemp, "Insert Setup Disk 2 in drive %c:", cSrcDrive);
      PaintText (pgw,  5,  30, 2, 0, szTemp);
      PaintText (pgw,  7,  30, 2, 0, "Press Enter to continue, or Escape to exit.");
      if ('\x1b' == GetKey ("\x1B\x0D"))
         Quit (1);

      iRet = CheckForFile (SECOND_FILE, cSrcDrive);
      if (!iRet)
         Beep ();
      }

   clr (pgw);
   PaintText (pgw,  5,  30, 2, 0, "Uncompressing Files...");
   PaintText (pgw,  7,  30, 2, 0, "Please Wait");

   iRet = UnzipFile (pgw, cSrcDrive, cDestDrive, pszDestPath, 
                     SECOND_FILE, pfLog);

   if (0 != iRet)
      {
      clr (pgw);
      PaintText (pgw,  5,  30, 2, 0, "The uncompression program encountered a problem.");
      sprintf (szTemp, "See the file %c:\\BAMS\\UNZIP.LOG", cSrcDrive);
      PaintText (pgw,  6,  30, 2, 0, szTemp);
      PaintText (pgw,  7,  30, 2, 0, "for more information and try installing again.");
      PaintText (pgw,  9,  30, 2, 0, "Press Enter to exit.");
      GetKey ("\x1B\x0D");
      DestroyGnuWin (pgw);

      Quit (1);
      }

   fclose (pfLog);
   DestroyGnuWin (pgw);
   return TRUE;
   }


void CreateGroups (CHAR cDestDrive, PSZ pszDestPath)
   {
   PGNUWIN     pgw;
   HPROGRAM    hp;
   PROGDETAILS progde;
   CHAR        szProgPath [256];
   CHAR        szDir [256];

   pgw = CreateGnuWin (60, 12);
   clr (pgw);

   PaintText (pgw,  5,  30, 2, 1, "Creating program group BAMS and program icons.");
   PaintText (pgw,  7,  30, 2, 1, "Please Wait");

   hp = PrfCreateGroup (HINI_USERPROFILE, "BAMS", SHE_VISIBLE | SHE_UNPROTECTED);
   if (hp == NULL)
      {
      InstError (pgw, FALSE, 
                 "The program group BAMS could not be created.  You will have",
                 "to manually create the program group, and add the programs",
                 "to it.");
      DestroyGnuWin (pgw);
      return;
      }

   sprintf (szDir, "%c:%s\\EXE", cDestDrive, pszDestPath);
   progde.Length = sizeof progde;
   progde.progt.progc     = PROG_PM;
   progde.progt.fbVisible = SHE_VISIBLE | SHE_UNPROTECTED;
   progde.pszExecutable = szProgPath;
   progde.pszParameters = "";
   progde.pszStartupDir = szDir;
   progde.pszIcon = "";
   progde.pszEnvironment = "\0";
   progde.swpInitial.x = 0;
   progde.swpInitial.y = 0;
   progde.swpInitial.cx = 0;
   progde.swpInitial.cy = 0;
   progde.swpInitial.hwnd = NULL;
   progde.swpInitial.hwndInsertBehind = NULL;
   progde.swpInitial.fs = 0;
   
   sprintf (szProgPath, "%s\\BAMSCES.EXE", szDir);
   progde.pszTitle = "BAMS/CES";
   PrfAddProgram (HINI_USERPROFILE, &progde, hp);

   sprintf (szProgPath, "%s\\BAMSPES.EXE", szDir);
   progde.pszTitle = "BAMS/PES";
   PrfAddProgram (HINI_USERPROFILE, &progde, hp);

   sprintf (szProgPath, "%s\\BAMSDSS.EXE", szDir);
   progde.pszTitle = "BAMS/DSS";
   PrfAddProgram (HINI_USERPROFILE, &progde, hp);

   sprintf (szProgPath, "%s\\DSSHELL.EXE", szDir);
   progde.pszTitle = "DS/Shell";
   PrfAddProgram (HINI_USERPROFILE, &progde, hp);

   DestroyGnuWin (pgw);
   }




USHORT ExitNotice (void)
   {
   PGNUWIN pgw;
   int     c;

   pgw = CreateGnuWin (70, 18);

   PaintText (pgw,  2, 35, 2, 1, "   BAMS/CES  Update Installation   ");
   PaintText (pgw,  3, 35, 2, 1, "נננננננננננננננננננננננננננננננננננ");
   PaintText (pgw,  5,  2, 0, 0, "The update is now complete.  ");
   PaintText (pgw, 15, 35, 2, 1, "Press <Enter> to exit.");

   c = GetKey ("\x1B\x0D");
   DestroyGnuWin (pgw);
   return (c != '\x1B');
   }



static void Usage (void)
   {
   printf ("\n");
   printf ("CesInst Release "VERSION_STRING".  Copyright (C) 1992 Info Tech, Inc.\n");
   printf ("\n");
   printf ("Usage: cesinst [-nocopy] [-nogroup] [-noconfig] [-sa n] [-password n]\n");
   printf ("Where:\n");
   printf ("   -nocopy     Do not copy files from install disk.\n");
   printf ("   -nogroup    Do not create program group and icons.\n");
   printf ("   -noconfig   Do not modify config.sys.\n");
   printf ("   -password n Set the SA password to \"n\".\n");
   printf ("   -sa n       Set the SA username to to \"n\".\n");
   printf ("\n");
   exit (1);
   }



_cdecl main (int argc, char *argv[])
   {
   char cSrcDrive;
   char cDestDrive;
   char szDestPath [256];
   BOOL bInfo = FALSE;
   BOOL bPathOK = FALSE;
   BOOL bNoCopy, bNoConfig, bNoGroups;
   USHORT u;

   BuildArgBlk ("^nocopy ^noconfig ^nogroup ^sa% ^password% ");
   if (FillArgBlk (argv))
      Usage ();
   bNoCopy   = IsArg ("nocopy");
   bNoConfig = IsArg ("noconfig");
   bNoGroups = IsArg ("nogroup");
   SetInstEnviron (PASSWORD, GetArg ("password", 0));
   SetInstEnviron (USERNAME, GetArg ("sa", 0));

   if (bNoCopy && bNoConfig && bNoGroups)
      {
      printf ("Nothing to do!\n");
      exit (1);
      }

   u = sizeof bcOriginal;
   VioReadCellStr ((PCH) &bcOriginal, &u, 0, 0, 0);
   bcOriginal [0] = ' ';
   InitScreenMetrics ();
   ShowCursor (FALSE);

   /*--- cls ---*/
   VioScrollDn (0, 0, 0xFFFF, 0xFFFF, 0xFFFF, bcBackground , 0);
   if (!Notice ())
      Quit (1);

   while (!bInfo)
      {
      if (!SourceDrive (&cSrcDrive, FIRST_FILE))
         Quit (1);
      if (!DestDrive (&cDestDrive))
         Quit (1);

      while (!bPathOK)
         {
         if (!DestPath (&cDestDrive, szDestPath))
            Quit (1);

         if (!Confirm (cSrcDrive, cDestDrive, szDestPath, NULL))
            break;

         if (CheckCreatePath (&cDestDrive, szDestPath))
            bInfo = bPathOK = TRUE;
        }

      if (bPathOK)
         {
         bPathOK = bNoConfig;
         while (!bPathOK)
            {
            if (!GetConfigString (SERVER))
               Quit (1);
   
            if (!GetConfigString (HOSTNAME))
               Quit (1);
   
            if (!GetConfigString (STATENAME))
               Quit (1);
   
            if (!GetConfigString (AGENCYNAME))
               Quit (1);
   
            bPathOK = ConfirmConfigStrings ();
            }
         }
      }

   if (!bNoCopy && !CopyFiles (cSrcDrive, cDestDrive, szDestPath))
      Quit (1);

     if (!bNoGroups)
        CreateGroups (cDestDrive, szDestPath);

     if (!bNoConfig)
        ModifyConfigSys (cDestDrive, szDestPath);

   ExitNotice ();
   Quit (0);
   return 0;
   }



