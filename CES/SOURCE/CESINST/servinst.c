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
 * ServInst.c
 *
 * (C) 1992 Info Tech Inc.
 */


#define INCL_VIO
//#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <process.h>
#include <direct.h>
#include <io.h>
#include <share.h>
#include "..\include\iti.h"
#include "ebs.h"
#include "scr.h"
#include "kbd.h"
#include "util.h"
#include "stuff.h"
#include "install.h"
#include "getinfo.h"
#include "exec.h"
#include "dbqry.h"
#include "error.h"
#include "database.h"
#include "arg2.h"
#include "servinst.h"

BYTE  bcBackground[2]  = {'\xB1', 0x07};
BYTE  bcOriginal [2] = {' ', 0x07};

char sz [512];


BOOL bVerbose = FALSE;

#define FIRST_FILE   "BAMSSERV.ZIP"


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

   pgw = CreateGnuWin (68, 18);

   PaintText (pgw,  2, 34, 2, 1, "BAMS/CES Server Install Program");
   PaintText (pgw,  3, 34, 2, 1, "נננננננננננננננננננננננננננננננ");
   PaintText (pgw,  5,  2, 0, 0, "This installation program will create the BAMS/CES database on");
   PaintText (pgw,  6,  2, 0, 0, "your server machine.  This program MUST be run on the machine");
   PaintText (pgw,  7,  2, 0, 0, "that runs SQL Server.  If this is not possible, contact Info");
   PaintText (pgw,  8,  2, 0, 0, "Tech BAMS/CES Technical Support.");
   PaintText (pgw, 15, 34, 2, 1, "Press <Enter> to continue installation or <ESC> to exit.");
#ifdef PRERELEASE
   PaintText (pgw, 18, 34, 2, 1, "Release "VERSION_STRING" "PRERELEASE);
#else
   PaintText (pgw, 18, 34, 2, 1, "Release "VERSION_STRING);
#endif

   c = GetKey ("\x1B\x0D");
   DestroyGnuWin (pgw);
   return (c != '\x1B');
   }



USHORT ExitNotice (void)
   {
   PGNUWIN pgw;
   int     c;

   pgw = CreateGnuWin (70, 18);

   PaintText (pgw,  2, 35, 2, 1, "BAMS/CES Server Installation Program");
   PaintText (pgw,  3, 35, 2, 1, "ננננננננננננננננננננננננננננננננננננ");
   PaintText (pgw,  5,  2, 0, 0, "The installation is now complete.");
   PaintText (pgw, 15, 35, 2, 1, "Press <Enter> to exit installation.");

   c = GetKey ("\x1B\x0D");
   DestroyGnuWin (pgw);
   return (c != '\x1B');
   }

void clr (PGNUWIN pgw)
   {
   ClearWin (pgw);
   PaintText (pgw,  2, 30, 2, 1, "BAMS/CES Server Installation");
   PaintText (pgw,  3, 30, 2, 1, "----------------------------");
   }



/*
 *
 * 1> read bid.exe file
 * 2> configure bidder ID and name
 * 3> confirm info
 * 4> write bid.exe file
 *
 */

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

   sprintf (szTemp, "%c:\\BAMS\\srvunzip.log", cDestDrive);
   pfLog = fopen (szTemp, "wt");

   iRet = UnzipFile (pgw, cSrcDrive, cDestDrive, pszDestPath,
                     FIRST_FILE, pfLog);

   if (0 != iRet)
      {
      clr (pgw);
      PaintText (pgw,  5,  30, 2, 0, "The uncompression program encountered a problem.");
      sprintf (szTemp, "Look at the file %c:\\BAMS\\SRVUNZIP.LOG for more information.", cSrcDrive);
      PaintText (pgw,  6,  30, 2, 0, szTemp);
      PaintText (pgw,  7,  30, 2, 0, "and try installing again.");
      PaintText (pgw,  9,  30, 2, 0, "Press Enter to exit.");
      GetKey ("\x1B\x0D");
      DestroyGnuWin (pgw);

      Quit (1);
      }

   fclose (pfLog);
   DestroyGnuWin (pgw);
   return TRUE;
   }

#define CREATE_ONLY  0x0001
#define UPDATE_ONLY  0x0002
#define BOTH         0x0003
#define VTEST        0x0004


#define NUM_FILES 19
#define LAST_STEP NUM_FILES

static struct
   {
   USHORT   usDots;     /* whether or not to print dots */
   USHORT   usFlags;    /* install flags */
   PSZ      pszFile;    /* name of file to execute */
   PSZ      pszMess1;   /* first line to display */
   PSZ      pszMess2;   /* second line to display */
   }
   aFiles [NUM_FILES] =
   {
/* 1*/{  10, CREATE_ONLY,         "acreate.sql",  "Creating the databases.............",        "This can take over fifteen minutes.",      },
/* 2*/{   0, BOTH,                "useds.sql",    "Making production data types...",  "",      },
/* 3*/{   0, UPDATE_ONLY | VTEST, "testver.sql",  "Checking database version.............",     "",      },
/* 4*/{   3, CREATE_ONLY,         "maketype.sql", "Making production data types.............",  "",      },
/* 5*/{ 115, CREATE_ONLY,         "pmaketab.sql", "Making production tables.............",      "",      },
/* 6*/{   0, CREATE_ONLY,         "pmakevie.sql", "Making production tables.............",      "",      },
/* 7*/{  58, CREATE_ONLY,         "pmakeidx.sql", "Making production indicies.............",    "",      },
/* 8*/{ 138, CREATE_ONLY,         "pinsert.sql",  "Inserting production metadata.............", "",      },
/* 9*/{   5, UPDATE_ONLY,         "pupdate.sql",  "Updating production metadata.............",  "",      },
/*10*/{  19, UPDATE_ONLY,         "920826.sql",   "Fixing Code Tables.............",            "This can take over fifteen minutes.",      },
/*11*/{   5, BOTH,                "pfixkeys.sql", "Fixing keys.............",                   "",      },
/*12*/{   0, BOTH,                "useimp.sql",   "Making import data types.............",      "",      },
/*13*/{   3, CREATE_ONLY,         "maketype.sql", "Making import data types.............",      "",      },
/*14*/{   5, UPDATE_ONLY,         "idrop.sql",    "Deleting old import database.............",  "",      },
/*15*/{  60, BOTH,                "imaketab.sql", "Making import tables.............",          "",      },
/*16*/{   0, BOTH,                "imakevie.sql", "Making import tables...............",        "",      },
/*17*/{   0, BOTH,                "imakeidx.sql", "Making import indicies.............",        "",      },
/*18*/{  75, BOTH,                "iinsert.sql",  "Inserting import metadata.............",     "",      },
/*19*/{   2, BOTH,                "dbcc.sql",     "Checking database consistency.............", "",      },
   };



BOOL CreateDatabase (FILE   *pfLog, 
                     PSZ    pszLog, 
                     PSZ    pszDestPath,
                     PSZ    pszDBPath,
                     PSZ    pszDataDevice, 
                     PSZ    pszDataFile, 
                     PSZ    pszLogDevice, 
                     PSZ    pszLogFile, 
                     USHORT usProdDBSize, 
                     USHORT usProdPct, 
                     USHORT usImpDBSize, 
                     USHORT usImpPct,
                     USHORT usStartStep,
                     BOOL   bUpdate)
   {
   PGNUWIN  pgw;
   char     szTemp [256];
   char     szFile [256];
   FILE     *pfIn;
   USHORT   i, x;
   USHORT   usMask;

   if (bUpdate)
      usMask = UPDATE_ONLY;
   else
      usMask = CREATE_ONLY;

   MakePath (pszDBPath);

   pgw = CreateGnuWin (60, 12);
   clr (pgw);
   PaintText (pgw,  5,  30, 2, 0, aFiles [0].pszMess1);
   PaintText (pgw,  6,  30, 2, 0, aFiles [0].pszMess2);
   PaintText (pgw,  8,  30, 2, 0, "Please wait....");

   sprintf (szTemp, "%s\\install\\%s", pszDestPath, aFiles [0].pszFile);
   if (usMask & aFiles [0].usFlags)
      CreateSQL (szTemp, pszDBPath, 
                 pszDataDevice, pszDataFile, pszLogDevice, pszLogFile, 
                 usProdDBSize, usProdPct, usImpDBSize, usImpPct);

   for (i=usStartStep - 1; i < NUM_FILES; i++)
      {
      clr (pgw);

      if (!(usMask & aFiles [i].usFlags))
         continue;

      PaintText (pgw,  5,  30, 2, 0, aFiles [i].pszMess1);
      PaintText (pgw,  6,  30, 2, 0, aFiles [i].pszMess2);
      PaintText (pgw,  8,  30, 2, 0, "Please wait....");

      x = strlen (aFiles [i].pszMess1) / 2 + 20;
      VioSetCurPos (YWinPos(pgw) + 5, XWinPos(pgw)+x, (HVIO)NULL);

      sprintf (szFile, "%s\\INSTALL\\%s", pszDestPath, aFiles [i].pszFile);
      pfIn = _fsopen (szFile, "rt", SH_DENYWR);
      if (pfIn == NULL)
         {
         sprintf (szTemp, "The file %s could not be opened.", szFile);
         InstError (pgw, FALSE, szTemp, 
                    "The database installation failed.  Contact Info Tech",
                    "BAMS/CES Tecnical Support.");
         DestroyGnuWin (pgw);
         return FALSE;
         }
      if (SQLExec (szFile, pfIn, pszLog, pfLog, 0, pfLog, aFiles [i].usDots))
         {
      // SEPT 95 POSSIBLE ERROR HERE
      //   if (VERSION & aFiles [i].usFlags)
      //      {
      //      /* wrong database version */
      //      InstError (pgw, FALSE, "The version of your database is unknown.",
      //               "The database cannot be converted.  The installation ",
      //               "failed.  Contact Info Tech BAMS/CES Tecnical Support.");
      //      }
      //   else
      //      {
      //      /* sql exec error */
      //      sprintf (szTemp, "%s.  The database installation", szFile);
      //      InstError (pgw, FALSE, "An error was encountered executing the file",
      //               szTemp,
      //               "failed.  Contact Info Tech BAMS/CES Tecnical Support.");
      //      }
      //   DestroyGnuWin (pgw);
      //   return FALSE;
         }
      fclose (pfIn);
      }

   DestroyGnuWin (pgw);
   return TRUE;
   }





BOOL IsUpdate (void)
   {
   PGNUWIN pgw;
   int     c;

   pgw = CreateGnuWin (60, 14);

   PaintText (pgw,  2, 30, 2, 1, "New Install or Update?");
   PaintText (pgw,  3, 30, 2, 1, "----------------------");
   PaintText (pgw,  5,  2, 0, 0, "Is this a new installation, or have you sucessfully");
   PaintText (pgw,  6,  2, 0, 0, "installed a previous version of the BAMS/CES");
   PaintText (pgw,  7,  2, 0, 0, "database?  If you had errors with a previous");
   PaintText (pgw,  8,  2, 0, 0, "installation, select NEW.");

   PaintText (pgw, 11, 30, 2, 1, "Type 'N' for NEW, or 'U' for UPDATE:");

   VioSetCurPos (YWinPos(pgw) + 11, XWinPos(pgw)+48, (HVIO)NULL);
   ShowCursor (TRUE);
   c = GetKey ("NU\x1b");
   ShowCursor (FALSE);

   DestroyGnuWin (pgw);
   if (c == '\x1b')
      Quit (1);

   return (c == 'U');
   }


static void Usage (void)
   {
   printf ("\n");
   printf ("ServInst Release "VERSION_STRING"  Copyright (C) 1992 Info Tech, Inc.\n");
   printf ("\n");
   printf ("Usage: servinst [-nocopy] [-nocheck] [-server name] [-update] [-noverbose]\n");
   printf ("                [-sa name] [-password p]\n");
   printf ("Advanced Options:\n");
   printf ("                [-datadev name] [-datafile name] [-startstep n]\n");
   printf ("                [-logdev name] [-logfile name]\n");
   printf ("Where:\n");
   printf ("   -nocopy     Do not copy files from install disk.\n");
   printf ("   -nocheck    Do not check for free space on database device drive.  \n");
   printf ("               This is normally only used in conjunction with the \n");
   printf ("               -server option.\n");
   printf ("   -update     Tells SERVINST that this is an update, not an install.\n");
   printf ("   -noverbose  Do not log every line sent to the SQL Server.\n");
   printf ("   -server n   Specify the server name.  Replace \"n\" with the name \n");
   printf ("               of the remote server.  Use this option only if you need\n");
   printf ("               to remotely install the server.  You may also have to \n");
   printf ("               specify the -nocheck option.\n");
   printf ("   -password n Set the SA password to \"n\".\n");
   printf ("   -sa n       Set the SA username to to \"n\".\n");
   printf ("\n");
   exit (1);
   }


_cdecl main (int argc, char *argv[])
   {
   char cSrcDrive;
   char cDestDrive;
   char szDBPath [256];
   char szDestPath [256];
   char szTemp [256];
   char szLog [256];
   USHORT usStartStep = 1;
   PSZ  pszServer = NULL, pszSA, pszPassword;
   PSZ  pszDataDevice, pszDataFile, pszLogDevice, pszLogFile;
   PSZ  pszStartStep;
   BOOL bInfo = FALSE;
   BOOL bPathOK = FALSE;
   USHORT u, usProdDBSize, usProdPct, usImpDBSize, usImpPct;
   FILE *pfLog;
   BOOL bNoCopy, bNoCheck, bUpdate;

   BuildArgBlk ("^server% ^nocopy ^nocheck ^datadev% ^datafile% ^logdev%"
                "^logfile% ^password% ^sa% ^startstep% ^update ^noverbose ");
   if (FillArgBlk (argv))
      Usage ();
   bNoCopy = IsArg ("nocopy") == 1;
   bNoCheck = IsArg ("nocheck") == 1;
   bUpdate = IsArg ("update") == 1;
   bVerbose = IsArg ("noverbose") == 0;

   pszServer      = GetArg ("server", 0);
   pszPassword    = GetArg ("password", 0);
   pszSA          = GetArg ("sa", 0);
   pszDataDevice  = GetArg ("datadev", 0);
   pszDataFile    = GetArg ("datafile", 0);
   pszLogDevice   = GetArg ("logdev", 0);
   pszLogFile     = GetArg ("logfile", 0);
   pszStartStep   = GetArg ("startstep", 0);
   if (pszStartStep)
      {
      usStartStep = atoi (pszStartStep);
      if (usStartStep <= 0)
         usStartStep = 1;
      if (usStartStep > LAST_STEP)
         {
         printf ("Nothing to do!\n");
         return 0;
         }
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
      if (!bUpdate)
         bUpdate = IsUpdate ();

      if (!SourceDrive (&cSrcDrive, FIRST_FILE))
         Quit (1);
      if (!DestDrive (&cDestDrive))
         Quit (1);

      while (!bPathOK)
         {
         if (!DestPath (&cDestDrive, szDestPath))
            Quit (1);

         if (!bUpdate && !GetDBPath (cDestDrive, szDestPath, szDBPath, bNoCheck))
            Quit (1);

         if (!Confirm (cSrcDrive, cDestDrive, szDestPath, bUpdate ? NULL : szDBPath))
            break;

         if (CheckCreatePath (&cDestDrive, szDestPath))
            bInfo = bPathOK = TRUE;
         }
      if (bPathOK)
         {
         bPathOK = bUpdate;
         while (!bPathOK)
            {
            GetProdDatabaseSize (&usProdDBSize, &usProdPct, szDBPath [0], bNoCheck);
            GetImpDatabaseSize (&usImpDBSize, &usImpPct, usProdDBSize, szDBPath [0], bNoCheck);
            bPathOK = ConfirmInfo (usProdDBSize, usProdPct, usImpDBSize, usImpPct);
            }
         }
      }

   sprintf (szTemp, "%c:%s\\INSTALL", cDestDrive, szDestPath);
   MakePath (szTemp);

   /* try an initialize the database module */
   sprintf (szLog, "%c:%s\\servinst.log", cDestDrive, szDestPath);
   if ((pfLog = DbInit (szLog, pszServer, "master", pszSA, pszPassword)) == NULL)
      {
      VioScrollDn (0, 0, 0xFFFF, 0xFFFF, 0xFFFF, bcOriginal, 0);
      VioSetCurPos (0, 0, (HVIO)NULL);
      ShowCursor (TRUE);

      printf ("The SQL Server must be running in order for the install program\n"
              "to run.  Start the SQL Server and run the install program again.\n");

      Quit (1);
      }

   if (!bNoCopy && !CopyFiles (cSrcDrive, cDestDrive, szDestPath))
      Quit (1);

   sprintf (szTemp, "%c:%s", cDestDrive, szDestPath);
   if (!CreateDatabase (pfLog, szLog, szTemp, szDBPath, 
                        pszDataDevice, pszDataFile, pszLogDevice, pszLogFile, 
                        usProdDBSize, usProdPct, usImpDBSize, usImpPct,
                        usStartStep, bUpdate))
      Quit (1);

   ExitNotice ();
   Quit (0);
   return 0;
   }



