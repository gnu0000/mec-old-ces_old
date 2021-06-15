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
 * database.c
 *
 * (C) 1992 Info Tech Inc.
 */


#define INCL_VIO
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <process.h>
#include <share.h>
#include "ebs.h"
#include "scr.h"
#include "kbd.h"
#include "util.h"
#include "stuff.h"
#include "install.h"
#include "getinfo.h"
#include "error.h"





BOOL GetDBPath (CHAR cDestDrive, PSZ pszDestPath, PSZ pszDBPath, BOOL bNoCheck)
   {
   PGNUWIN  pgw;
   BOOL     bFound;
   USHORT   usLen;

   pgw = CreateGnuWin (62, 15);

   sprintf (pszDBPath, "%c:%s\\DATA", cDestDrive, pszDestPath);
   bFound = FALSE;
   while (!bFound)
      {
      ClearWin (pgw);
      PaintText (pgw,  2, 30, 2, 1, "DATABASE FILES PATH");
      PaintText (pgw,  3, 30, 2, 1, "-------------------");
      PaintText (pgw,  5,  2, 0, 0, "Please enter the drive letter and path of the location you");
      PaintText (pgw,  6,  2, 0, 0, "would like the database files to be stored.  Refer to the ");
      PaintText (pgw,  7,  2, 0, 0, "SQL Server System Adminstrator's Guide for information on");
      PaintText (pgw,  8,  2, 0, 0, "the best location for the database files.  You may press");
      PaintText (pgw,  9,  2, 0, 0, "Escape to exit.");
      PaintText (pgw, 11,  2, 0, 1, "Path:");

      ShowCursor (TRUE);
      if (!EditCell (pszDBPath, XWinPos(pgw)+8, YWinPos(pgw) + 11, 53, 2, "\x0d", ""))
         Quit (1);
      ShowCursor (FALSE);

      strlwr (pszDBPath);
      usLen = strlen (pszDBPath);
      while (usLen > 3 && pszDBPath [usLen-1] == '\\')
         usLen--;
      pszDBPath [usLen] = '\0';

      if (pszDBPath [0] >= 'a' && pszDBPath [0]<= 'z' 
          && pszDBPath [1] == ':' && pszDBPath [2] == '\\')
         {
         /* they entered a full path! */
         if (!bNoCheck && !DriveExists (*pszDBPath))
            {
            InstError (pgw, TRUE, 
               "The drive you specified does not exist.", "", "");
            }
         else if (!bNoCheck && (GetDriveFreeSpace (*pszDBPath) / 1048567UL) < 20)
            {
            InstError (pgw, TRUE,
               "The drive you specified has less than 20 MB of free disk",
               "space The production database and import databse need a",
               "minimum of 20 MB of disk space.");
            }
         else 
            {
            bFound = TRUE;
            }
         }
      else
         {
         InstError (pgw, FALSE,
            "You need to specify a fully qualified path, including a drive",
            "letter.  The path must also start from the root directory of", 
            "the drive.");
         }
      }
   
   DestroyGnuWin (pgw);
   return TRUE;
   }





BOOL GetProdDatabaseSize (PUSHORT pusDBSize, PUSHORT pusDBPct, CHAR cDBDrive, BOOL bNoCheck)
   {
   PGNUWIN  pgw;
   PSZ      pszData;
   USHORT   i;
   BOOL     bDone;
   USHORT   usFreeMB, usMinPct, usMaxPct;
   char     szTemp [256];

   pgw = CreateGnuWin (60, 15);
   bDone = FALSE;
   pszData = "";
   usFreeMB = (USHORT) (GetDriveFreeSpace (cDBDrive) / 1048567UL);

   while (!bDone)
      {
      ClearWin (pgw);
      PaintText (pgw,  2, 30, 2, 1, "Production Database Size");
      PaintText (pgw,  3, 30, 2, 1, "------------------------");
      PaintText (pgw,  5,  2, 0, 0, "For information on what size database you may need,");
      PaintText (pgw,  6,  2, 0, 0, "consult the BAMS/CES Detailed Design Document,");
      PaintText (pgw,  7,  2, 0, 0, "Appendix B.  The minimum database size is 10 MB.  The");
      PaintText (pgw,  8,  2, 0, 0, "maximum depends on the available disk space on your");
      PaintText (pgw,  9,  2, 0, 0, "computer.");
      PaintText (pgw, 11,  2, 0, 1, "What size production database would you like:");
      PaintText (pgw, 13, 30, 2, 1, "Press <Enter> to accept value, <Esc> to exit.");

      if (!GetString (&pszData, pszData, YWinPos(pgw) + 11, XWinPos(pgw)+48, 5))
         Quit (1);

      i = atoi (pszData);
      if (i < 10)
         {
         InstError (pgw, FALSE, 
                    "The database size you have chosen is too small.  The",
                    "minimum database size is 10MB.",
                    "");
         }
      else if (!bNoCheck && i > usFreeMB)
         {
         sprintf (szTemp, "The destination drive has only %u MB of free", usFreeMB);

         InstError (pgw, FALSE, szTemp, "disk space.", "");
         }
      else
         {
         bDone = TRUE;
         }
      }

   *pusDBSize = i;

   usMaxPct = 50;
   usMinPct = 10;

   bDone = FALSE;
   while (!bDone)
      {
      ClearWin (pgw);
      PaintText (pgw,  2, 30, 2, 1, "Production Database Log Size");
      PaintText (pgw,  3, 30, 2, 1, "----------------------------");
      sprintf (szTemp, "You have chosen to make a %uMB production database.", *pusDBSize);
      PaintText (pgw,  5,  2, 0, 0, szTemp);
      PaintText (pgw,  6,  2, 0, 0, "What percent of this would you like to dedicate for");
      PaintText (pgw,  7,  2, 0, 0, "the system logs.  For more information on system");
      PaintText (pgw,  8,  2, 0, 0, "logs, see the SQL Server System Administrator's");
      PaintText (pgw,  9,  2, 0, 0, "Guide.");
      PaintText (pgw, 11,  2, 0, 1, "Data base log size (in percent of database size):");
      PaintText (pgw, 13, 30, 2, 1, "Press <Enter> to accept value, <Esc> to exit.");

      if (!GetString (&pszData, "35%", YWinPos(pgw) + 11, XWinPos(pgw)+53, 4))
         Quit (1);

      i = atoi (pszData);
      if (i < usMinPct)
         {
         sprintf (szTemp, "minimum percent to use is %u%% (%uMB).", usMinPct,
                  (USHORT) ((ULONG) usMinPct * (ULONG) *pusDBSize / 100UL));
         InstError (pgw, FALSE, 
                    "The database log size you have chosen is too small.  The",
                    szTemp,
                    "");
         }
      else if (i > usMaxPct)
         {
         sprintf (szTemp, "maximum percent to use is %u%% (%uMB).", usMaxPct,
                  (USHORT) ((ULONG) usMaxPct * (ULONG) *pusDBSize / 100UL));
         InstError (pgw, FALSE, 
                    "The database log size you have chosen is too large.  The",
                    szTemp,
                    "");
         }
      else
         {
         bDone = TRUE;
         }
      }

   *pusDBPct = i;
   DestroyGnuWin (pgw);
   return TRUE;
   }




BOOL GetImpDatabaseSize (PUSHORT pusDBSize, PUSHORT pusDBPct, USHORT usProdDBSize, CHAR cDBDrive, BOOL bNoCheck)
   {
   PGNUWIN  pgw;
   PSZ      pszData;
   USHORT   i;
   BOOL     bDone;
   USHORT   usFreeMB, usMinPct, usMaxPct;
   char     szTemp [256];

   pgw = CreateGnuWin (60, 17);
   bDone = FALSE;
   pszData = "";
   usFreeMB = (USHORT) (GetDriveFreeSpace (cDBDrive) / 1048567UL) - usProdDBSize;

   while (!bDone)
      {
      ClearWin (pgw);
      PaintText (pgw,  2, 30, 2, 1, "Import Database Size");
      PaintText (pgw,  3, 30, 2, 1, "--------------------");
      PaintText (pgw,  5,  2, 0, 0, "The import database is used when you pass data from");
      PaintText (pgw,  6,  2, 0, 0, "your mainframe to the PC.  We have found that an");
      PaintText (pgw,  7,  2, 0, 0, "import database of 40MB is sufficient for most state's");
      PaintText (pgw,  8,  2, 0, 0, "needs.");
      PaintText (pgw, 10,  2, 0, 1, "What size import database would you like:");
      PaintText (pgw, 15, 30, 2, 1, "Press <Enter> to accept value, <Esc> to exit.");

      if (!GetString (&pszData, "40", YWinPos(pgw) + 10, XWinPos(pgw)+44, 5))
         Quit (1);

      i = atoi (pszData);
      if (i < 10)
         {
         InstError (pgw, FALSE, 
                    "The database size you have chosen is too small.  The",
                    "minimum database size is 10MB.",
                    "");
         }
      else if (!bNoCheck && i > usFreeMB)
         {
         sprintf (szTemp, "The destination drive has only %u MB of free", usFreeMB);

         InstError (pgw, FALSE, szTemp, "Disk Space.", "");
         }
      else
         {
         bDone = TRUE;
         }
      }

   *pusDBSize = i;
   usMaxPct = 90;
   usMinPct = 40;

   bDone = FALSE;
   while (!bDone)
      {
      ClearWin (pgw);
      PaintText (pgw,  2, 30, 2, 1, "Production Database Log Size");
      PaintText (pgw,  3, 30, 2, 1, "----------------------------");
      sprintf (szTemp, "You have chosen to make a %uMB production database.", *pusDBSize);
      PaintText (pgw,  5,  2, 0, 0, szTemp);
      PaintText (pgw,  6,  2, 0, 0, "What percent of this would you like to dedicate for");
      PaintText (pgw,  7,  2, 0, 0, "the system logs.  For more information on system");
      PaintText (pgw,  8,  2, 0, 0, "logs, see the SQL Server System Administrator's ");
      PaintText (pgw,  9,  2, 0, 0, "Guide.");
      PaintText (pgw, 11,  2, 0, 1, "Data base log size (in percent of database size):");
      PaintText (pgw, 13, 30, 2, 1, "Press <Enter> to accept value, <Esc> to exit.");

      if (!GetString (&pszData, "80%", YWinPos(pgw) + 11, XWinPos(pgw)+53, 4))
         Quit (1);

      i = atoi (pszData);
      if (i < usMinPct)
         {
         sprintf (szTemp, "minimum percent to use is %u%% (%uMB).", usMinPct,
                  (USHORT) ((ULONG) usMinPct * (ULONG) *pusDBSize / 100UL));
         InstError (pgw, FALSE, 
                    "The database log size you have chosen is too small.  The",
                    szTemp,
                    "");
         }
      else if (i > usMaxPct)
         {
         sprintf (szTemp, "maximum percent to use is %u%% (%uMB).", usMaxPct,
                  (USHORT) ((ULONG) usMaxPct * (ULONG) *pusDBSize / 100UL));
         InstError (pgw, FALSE, 
                    "The database log size you have chosen is too large.  The",
                    szTemp,
                    "");
         }
      else
         {
         bDone = TRUE;
         }
      }

   *pusDBPct = i;
   DestroyGnuWin (pgw);
   return TRUE;
   }

BOOL ConfirmInfo (USHORT usProdDB, USHORT usProdPct, USHORT usImpDB, USHORT usImpPct)
   {
   PGNUWIN  pgw;
   int      c;

   pgw = CreateGnuWin (60, 14);

   PaintText (pgw,  2, 30, 2, 1, "CONFIRMATION");
   PaintText (pgw,  3, 30, 2, 1, "------------");
   PaintText (pgw,  5,  2, 0, 0, "You have entered the following parameters:");
   sprintf (sz, "Production Database Size: %3u MB", usProdDB);
   PaintText (pgw,  7,  2, 0, 0, sz);
   sprintf (sz, "     Production Log Size: %3u MB (%2u%%)", 
            usProdPct * usProdDB / 100, usProdPct);
   PaintText (pgw,  8,  2, 0, 0, sz);
   sprintf (sz, "    Import Database Size: %3u MB", usImpDB);
   PaintText (pgw,  9,  2, 0, 0, sz);
   sprintf (sz, "         Import Log Size: %3u MB (%2u%%)", 
            usImpPct * usImpDB / 100, usImpPct);
   PaintText (pgw, 10,  2, 0, 0, sz);

   PaintText (pgw, 12, 30, 2, 1, "Are they correct ? (Y or N)");

   c = GetKey ("YN\x1b");
   DestroyGnuWin (pgw);
   return (c == 'Y');
   }


static PSZ apszCreate [] =
   {  
   ";\n",
   ";BAMS/CES Installation Script\n",
   ";Release 1.0a\n",
   ";Mark Hampton\n",
   ";\n",
   ";Revision History:\n",
   ";\n",
   ";6/10/1992: Added creation of dump devices\n",
   ";\n",
   ";6/9/1992: Added messages displaying progress.\n",
   ";\n",
   ";6/8/1992: Initial creation.  mdh\n",
   ";\n",
   "; Create the data device\n",
   ";\n",
   "\"SELECT \"Creating data device...\"\n",
   "+DECLARE @DeviceNumber int\n",
   "+SELECT @DeviceNumber=(SELECT MAX (low / 16777216) + 1 FROM sysdevices)\n",
   "+DISK INIT NAME='%DataDevice',\n",
   "+PHYSNAME='%Path\\%DataFile',\n",
   "+VDEVNO=@DeviceNumber,\n",
   "SIZE=%DataPages\n",
   ";\n",
   "; Create the log device\n",
   ";\n",
   "\"SELECT \"Creating log device...\"\n",
   "+DECLARE @DeviceNumber int\n",
   "+SELECT @DeviceNumber=(SELECT MAX (low / 16777216) + 1 FROM sysdevices)\n",
   "+DISK INIT NAME='%LogDevice',\n",
   "+PHYSNAME='%Path\\%LogFile',\n",
   "+VDEVNO=@DeviceNumber,\n",
   "SIZE=%LogPages\n",
   ";\n",
   "; Create the production database.\n",
   ";\n",
   "\"SELECT \"Creating production database...\"\n",
   "CREATE DATABASE DSShell ON %DataDevice=%ProdData, %LogDevice=%ProdLog\n",
   "sp_logdevice DSShell, %LogDevice\n",
   ";\n",
   "; Create the import database.\n",
   ";\n",
   "\"SELECT \"Creating import database...\"\n",
   "CREATE DATABASE DSShellImport ON %DataDevice=%ImpData, %LogDevice=%ImpLog\n",
   "sp_logdevice DSShellImport, %LogDevice\n",
   ";\n",
   "; Create dump devices\n",
   ";\n",
   "\"SELECT \"Creating dump devices...\"\n",
   "sp_addumpdevice 'disk',BamsDumper,'%Path\\bamsdump.dev',2\n",
   "sp_addumpdevice 'disk',ImportDumper,'%Path\\impdump.dev',2\n",
   "sp_addumpdevice 'disk',MasterDumper,'%Path\\mstrdump.dev',2\n",
   "\n",
   NULL
   };


BOOL CreateSQL (PSZ    pszFile,
                PSZ    pszPath,
                PSZ    pszDataDevice, 
                PSZ    pszDataFile, 
                PSZ    pszLogDevice, 
                PSZ    pszLogFile, 
                USHORT usProdDBSize, 
                USHORT usProdPct, 
                USHORT usImpDBSize, 
                USHORT usImpPct)
   {
   USHORT i;
   FILE   *pf;
   char   szTemp [256];
   char   szDataPages [32];
   char   szLogPages [32];
   char   szProdData [32];
   char   szProdLog  [32];
   char   szImpData  [32];
   char   szImpLog   [32];
   PSZ    apszTokens [11];
   PSZ    apszValues [11];
   ULONG  ulDataPages;
   ULONG  ulLogPages;
   USHORT usImpLog;
   USHORT usProdLog;

   pf = fopen (pszFile, "wt");
   if (pf == NULL)
      return FALSE;

   apszTokens [0]  = "DataPages";
   apszTokens [1]  = "LogPages";
   apszTokens [2]  = "ProdData";
   apszTokens [3]  = "ProdLog";
   apszTokens [4]  = "ImpData";
   apszTokens [5]  = "ImpLog";
   apszTokens [6]  = "Path";
   apszTokens [7]  = "DataDevice";
   apszTokens [8]  = "DataFile";
   apszTokens [9]  = "LogDevice";
   apszTokens [10] = "LogFile";

   apszValues [0]  = szDataPages;
   apszValues [1]  = szLogPages;
   apszValues [2]  = szProdData;
   apszValues [3]  = szProdLog;
   apszValues [4]  = szImpData;
   apszValues [5]  = szImpLog;
   apszValues [6]  = pszPath;
   apszValues [7]  = pszDataDevice ? pszDataDevice : "BamsData";
   apszValues [8]  = pszDataFile   ? pszDataFile   : "BamsData";
   apszValues [9]  = pszLogDevice  ? pszLogDevice  : "BamsLog";
   apszValues [10] = pszLogFile    ? pszLogFile    : "BamsLog";
   
   usProdLog = usProdDBSize * usProdPct / 100;
   usImpLog = usImpDBSize * usImpPct / 100;
   ulDataPages = (ULONG) (usProdDBSize - usProdLog + usImpDBSize - usImpLog) * 
                 512UL;
   ulLogPages = (ULONG) (usImpLog + usProdLog) * 512UL;

   sprintf (szDataPages, "%lu", ulDataPages);
   sprintf (szLogPages, "%lu", ulLogPages);
   sprintf (szProdData, "%u", usProdDBSize - usProdLog);
   sprintf (szProdLog, "%u", usProdLog);
   sprintf (szImpData, "%u", usImpDBSize - usImpLog);
   sprintf (szImpLog, "%u", usImpLog);

   for (i=0; apszCreate [i]; i++)
      {
      ReplaceParams (szTemp, apszCreate [i], sizeof szTemp, 
                     apszTokens, apszValues, 11);
      fputs (szTemp, pf);
      }
   fclose (pf);
   return TRUE;
   }


