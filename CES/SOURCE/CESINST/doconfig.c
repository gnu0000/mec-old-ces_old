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
 * DoConfig.c
 *
 * (C) 1992 Info Tech Inc.
 * This file modifies config.sys for BAMS/CES
 */


#define INCL_VIO
#define INCL_DOS
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <share.h>
#include <errno.h>
#include <process.h>
#include <direct.h>
#include <io.h>
#include <fcntl.h>
#include "ebs.h"
#include "scr.h"
#include "kbd.h"
#include "util.h"
#include "stuff.h"
#include "install.h"
#include "doconfig.h"
#include "error.h"


#define NUM_STRINGS  10

PSZ apszFixes [NUM_STRINGS][4] =
   {
      { "LIBPATH=",     "LIBPATH=",      ".;",                       NULL, },
      { "SETSERVER=",   "SET SERVER=",   "SERVER",                   "SERVER", },
      { "SETDATABASE=", "SET DATABASE=", "DSShell",                  NULL, },
      { "SETHOSTNAME=", "SET HOSTNAME=", "Client",                   "CLIENT", },
      { "SETUSERNAME=", "SET USERNAME=", NULL,                       NULL, },
      { "SETPASSWORD=", "SET PASSWORD=", NULL,                       NULL, },
      { "SETTMP=",      "SET TMP=",      NULL,                       NULL, },
      { "SETTEMP=",     "SET TEMP=",     NULL,                       NULL, },
      { "SETSTATE=",    "SET STATE=",    "The State of Florida",     "STATE",      },
      { "SETAGENCY=",   "SET AGENCY=",   "Department of Transportation", "AGENCY"  },
   };



PSZ apszMessages [NUM_STRINGS] [6] =
   {
      { NULL, NULL, NULL, NULL, NULL, NULL, },
      {   
      "DATABASE SERVER NAME",
      "--------------------",
      "Please enter the machine name of the database server.",
      "If you are setting up a stand-alone workstation, leave",
      "this field blank.",
      "Server Name:",
      },
      { NULL, NULL, NULL, NULL, NULL, NULL, },
      {
      "HOST NAME",
      "---------",
      "Please enter the name of this machine.  This name will show",
      "up in the Show Users dialog box.  If you are installing a",
      "stand-alone workstation, you may leave this name blank.",
      "Host Name:",
      },
      { NULL, NULL, NULL, NULL, NULL, NULL, },
      { NULL, NULL, NULL, NULL, NULL, NULL, },
      { NULL, NULL, NULL, NULL, NULL, NULL, },
      { NULL, NULL, NULL, NULL, NULL, NULL, },
      {
      "STATE NAME",
      "----------",
      "Please enter your state's name as you would like it to",
      "appear on reports.  Reports print out in mixed case.",
      "",
      "State Name:",
      },
      {
      "AGENCY NAME",
      "-----------",
      "Please enter your state agency's name as you would like it",
      "to appear on reports.  Reports print out in mixed case.",
      "",
      "Agency Name:",
      },
   };









void SetInstEnviron (USHORT usIndex, PSZ psz)
   {
   apszFixes [usIndex][2] = psz;
   }

BOOL CheckLibPath (PSZ pszLibPath)
   {
   PSZ pszStart, pszEnd;

   pszStart = strchr (pszLibPath, '=');
   while (pszStart)
      {
      pszStart++;
      pszEnd = strchr (pszStart, ';');
      if (pszEnd == NULL)
         pszEnd = strchr (pszStart, '\n');
      if (pszEnd == NULL)
         pszEnd = strchr (pszStart, '\0');
      if (pszEnd == pszStart)
         return FALSE;
      if (pszEnd && strncmp (pszStart, ".", pszEnd - pszStart) == 0)
         return TRUE;
      if (pszEnd && *pszEnd)
         pszStart = pszEnd;
      else
         pszStart = NULL;
      }   
   return FALSE;
   }



void FixConfigSys (FILE *pfIn, FILE *pfOut)
   {
   char szLine [1024];
   char szCopy [1024];
   PSZ  pszOld;
   USHORT   i, j;
   BOOL bFound [NUM_STRINGS];
   BOOL bWritten;

   for (i=0; i < NUM_STRINGS; i++)
      bFound [i] = FALSE;

   while (fgets (szLine, sizeof szLine, pfIn))
      {
      strcpy (szCopy, szLine);
      ExtractString (szCopy, " \t\n\b");
      bWritten = FALSE;
      for (i=0; i < NUM_STRINGS && !bWritten; i++)
         {
         j = strlen (apszFixes [i] [0]);
         if (0 == strnicmp (szCopy, apszFixes [i] [0], j))
            {
            /* a match */
            bFound [i] = TRUE;
            bWritten = TRUE;
            if (apszFixes [i] [2] && szCopy [j-1] &&
                strcmp (szCopy + j, apszFixes [i] [2]) == 0)
               {
               /* we don't have to change it */
               bWritten = FALSE;
               }
            else if (apszFixes [i] [2])
               {
               if (i == TEMP || i == TMP)
                  bWritten = FALSE;  /* don't change temp */
               else if (i == LIBPATH && CheckLibPath (szLine))
                  bWritten = FALSE;  /* . is already in libpath */
               else
                  {
                  fputs (apszFixes [i] [1], pfOut);
                  fputs (apszFixes [i] [2], pfOut);
                  if (i == LIBPATH)
                     {
                     /* find the = */
                     pszOld = strchr (szLine, '=');
                     if (pszOld != NULL)
                        {
                        pszOld++;
                        fputs (pszOld, pfOut);
                        }
                     }
                  else
                     fputc ('\n', pfOut);
                  }
               }
            }
         }
      if (!bWritten)
         fputs (szLine, pfOut);
      }

   for (i=0; i < NUM_STRINGS; i++)
      if (!bFound [i] && apszFixes [i] [2])
         {
         fputs (apszFixes [i] [1], pfOut);
         fputs (apszFixes [i] [2], pfOut);
         fputc ('\n', pfOut);
         }
   }



void BackupConfigSys (char cDestDrive,
                      PSZ pszDestPath,
                      PSZ pszFileName)
   {
   char     szCopy [256];
   BOOL     bDone;
   USHORT   i, j;

   for (bDone = FALSE, i = 0; i <= 999 && !bDone; i++)
      {
      sprintf (szCopy, "%c:%s\\config.%.3d", cDestDrive, pszDestPath, i);
      j = open (szCopy, O_RDONLY);
      if (j != -1)
         close (j);
      else
         bDone = TRUE;
      }

   DosCopy (pszFileName, szCopy, 0, 0);

   for (bDone = FALSE, i = 0; i <= 999 && !bDone; i++)
      {
      sprintf (szCopy, "%c:\\config.%.3d", (char) *pszFileName, i);
      j = open (szCopy, O_RDONLY);
      if (j != -1)
         close (j);
      else
         bDone = TRUE;
      }

   DosCopy (pszFileName, szCopy, 0, 0);
   }



BOOL ModifyConfigSys (char cDestDrive,
                      PSZ pszDestPath)
   {
   PGNUWIN     pgw;
   FILE  *pfIn, *pfOut;
   char  szFileName [256];
   char  szNewName [256];
   char  szCopy [256];
   char  szTemp [256];
   SEL   selGlobal, selLocal;
   PGINFOSEG pgis;

   pgw = CreateGnuWin (60, 12);
   clr (pgw);
   PaintText (pgw,  5,  30, 2, 1, "Modifing CONFIG.SYS.");
   PaintText (pgw,  7,  30, 2, 1, "Please Wait");

   /* figure out the file name */
   DosGetInfoSeg (&selGlobal, &selLocal);
   pgis = MAKEP (selGlobal, 0);
   sprintf (szFileName, "%c:\\config.sys", (char) (pgis->bootdrive - 1) + 'a');
   sprintf (szNewName, "%c:\\@iticnf@.sys", (char) (pgis->bootdrive - 1) + 'a');

   /* make a bunch of copies */
   BackupConfigSys (cDestDrive, pszDestPath, szFileName);

   /* update config.sys */
   pfIn = _fsopen (szFileName, "rt", SH_DENYRW);
   if (pfIn == NULL)
      {
      sprintf (szTemp, "Could not open %s: %s", szFileName, strerror (errno));
      InstError (pgw, FALSE, szTemp,
                 "You will have to make the changes to CONFIG.SYS manually.",
                 NULL);
      DestroyGnuWin (pgw);
      return FALSE;
      }

   pfOut = _fsopen (szNewName, "wt", SH_DENYRW);
   if (pfIn == NULL)
      {
      sprintf (szTemp, "Could not open %s: %s", szNewName, strerror (errno));
      InstError (pgw, FALSE, szTemp,
                 "You will have to make the changes to CONFIG.SYS manually.",
                 NULL);
      DestroyGnuWin (pgw);
      fclose (pfIn);
      return FALSE;
      }

   sprintf (szTemp, "%c:%s\\TEMP", cDestDrive, pszDestPath);
   apszFixes [TEMP] [2] = szTemp;
   apszFixes [TMP] [2] = szTemp;
   FixConfigSys (pfIn, pfOut);

   fclose (pfIn);
   fclose (pfOut);

   if (unlink (szFileName))
      {
      sprintf (szTemp, "Could not delete %s.  You will have to", szFileName);
      sprintf (szCopy, "rename %s to %s before BAMS/CES can", szNewName, szFileName);
      InstError (pgw, FALSE, szTemp, szCopy, "run.");
      return FALSE;
      }

   if (rename (szNewName, szFileName))
      {
      sprintf (szTemp, "DANGER! Could not rename %s to %s.", szNewName, szFileName);
      sprintf (szCopy, "%s to %s.", szNewName, szFileName);
      InstError (pgw, FALSE, szTemp,
         "Your machine will not be able to reboot until yo rename",
         szCopy);
      DestroyGnuWin (pgw);
      return FALSE;
      }

   /* make a copy of the new config.sys */
   sprintf (szCopy, "%c:%s\\config.bam", cDestDrive, pszDestPath);
   DosCopy (szFileName, szCopy, DCPY_EXISTING, 0);

   DestroyGnuWin (pgw);
   return TRUE;
   }




BOOL GetConfigString (USHORT usID)
   {
   PGNUWIN  pgw;
   BOOL     bFound;
   PSZ      pszData;

   if (apszFixes [usID] [3] && (pszData = getenv (apszFixes [usID] [3])))
      {
      apszFixes [usID] [2] = pszData;
      /* make sure that second time through they get what they typed */
      apszFixes [usID] [3] = NULL;
      }
   
   pgw = CreateGnuWin (62, 15);
   bFound = FALSE;
   while (!bFound)
      {
      ClearWin (pgw);
      PaintText (pgw,  2, 30, 2, 1, apszMessages [usID] [0]);
      PaintText (pgw,  3, 30, 2, 1, apszMessages [usID] [1]);
      PaintText (pgw,  5,  2, 0, 0, apszMessages [usID] [2]);
      PaintText (pgw,  6,  2, 0, 0, apszMessages [usID] [3]);
      PaintText (pgw,  7,  2, 0, 0, apszMessages [usID] [4]);
      PaintText (pgw, 11,  2, 0, 1, apszMessages [usID] [5]);

      if (!GetString (&pszData, apszFixes [usID] [2], YWinPos(pgw) + 11,
                      XWinPos(pgw) + strlen (apszMessages [usID] [5]) + 3, 56))
         Quit (1);

      bFound = TRUE;
      apszFixes [usID] [2] = strdup (pszData);
      }
   
   DestroyGnuWin (pgw);
   return TRUE;
   }





BOOL ConfirmConfigStrings (void)
   {
   PGNUWIN  pgw;
   int      c;

   pgw = CreateGnuWin (60, 15);

   PaintText (pgw,  2, 30, 2, 1, "CONFIRMATION");
   PaintText (pgw,  3, 30, 2, 1, "------------");
   PaintText (pgw,  5,  2, 0, 0, "You have entered the following parameters:");
   sprintf (sz, "Server Name: %s", apszFixes [SERVER] [2]);
   PaintText (pgw,  7,  2, 0, 0, sz);
   sprintf (sz, "  Host Name: %s", apszFixes [HOSTNAME] [2]);
   PaintText (pgw,  8,  2, 0, 0, sz);
   sprintf (sz, " State Name: %s", apszFixes [STATENAME] [2]);
   PaintText (pgw,  9,  2, 0, 0, sz);
   sprintf (sz, "Agency Name: %s", apszFixes [AGENCYNAME] [2]);
   PaintText (pgw, 10,  2, 0, 0, sz);

   PaintText (pgw, 12, 30, 2, 1, "Are they correct ? (Y or N)");

   c = GetKey ("YN\x1b");
   DestroyGnuWin (pgw);
   return (c == 'Y');
   }
