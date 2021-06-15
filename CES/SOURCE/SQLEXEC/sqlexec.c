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
 * sqlexec
 *
 * usage: sqlexec [fn1 [fn2...]]
 * 
 * This program will read one line from the input file, and execute it
 * on the SQL server.  If no file name is given, then stdin is used. 
 * Blank lines ignored.  
 *
 * A log of lines sent to the SQL server, as well as messages and errors
 * is placed in the file sqlexec.log.
 *
 * The following characters have special meaning when in the first 
 * column of a line:
 *
 *  Char    Meaning
 *  ----    ---------------------------------------------------------
 *    ;     This line is a comment.  The line will not be sent to the
 *          SQL server.
 *
 *    *     Ignore any errors this query may cause.
 *
 *    "     Print the results of this query to stdout.  Each column
 *          returned is separated by a tab character.  If the digits
 *          1 - 9 follow this character, then only that column is
 *          printed.
 *
 *    #     Same as the '*' and '"' combined: print and ignore errors.
 *
 *    +     The query continues on the next line.  The last line of the 
 *          query should not have a '+' as the first character.  If
 *          you want to use any other special characters, place them
 *          only on the LAST line, the one without the '+'.
 *
 * 11/18/92:
 *
 *    ? : ! Conditional execution.  The line marked with the '?' is
 *          executed.  If the query returns a non-zero row, or more
 *          than one row, queries marked with the ':' (true condition)
 *          are executed, otherwise the queries marked with the '!' (false
 *          condition) are executed.  If you need to use '*', '"', or '#',
 *          place the ':' or '!' first on the line.  If an error occurs
 *          executing the '?' query, and the '*' or '#' options are not
 *          present, neither of the other queries are executed; otherwise,
 *          the '!' query is executed.  The result of the '?' is saved
 *          until a new '?' query is executed, so you may have as many
 *          ':' and '!' queries as you need.  The order of the ':' and
 *          '!' queries is not important.
 *
 * KNOWN BUGS:
 * 
 */



#define INCL_DOSMISC
#include "..\include\iti.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "sqlexec.h"
#include "exec.h"
#include "dbqry.h"
#include "arg2.h"

BOOL bVerbose = FALSE;

BOOL bNoHardError = FALSE;

PSZ pszAppName = "SQLExec";

void Copyright ()
   {
#ifdef PRERELEASE
   fprintf (stderr,
            "\nSQLExec Release %s %s Copyright (C) "COPYRIGHT" Info Tech, Inc.\n\n",
           VERSION_STRING, PRERELEASE);
#else
   fprintf (stderr,
            "\nSQLExec Release %s      Copyright (C) "COPYRIGHT" Info Tech, Inc.\n\n",
           VERSION_STRING);
#endif
   }


static int Usage (void);

int _cdecl main (int argc, char *argv [])
   {
   FILE  *pf, *pfLog;
   int   i, usLine;
   PSZ   pszLine, pszFile, pszLog;
   PSZ   pszServer, pszDatabase, pszSA, pszPassword;

   Copyright ();

   BuildArgBlk ("*^server% *^password% ^sa% *^database% *^noharderror *^verbose *^help ^? *^line% *^log% ");
   if (FillArgBlk (argv))
      {
      printf ("%s\n", GetArgErr ());
      return Usage ();
      }

   pszFile = GetArg (NULL, 0);
   if (IsArg ("help") || IsArg ("?"))
      return Usage ();

   pszServer = GetArg ("server", 0);
   if (!pszServer && DosScanEnv ("SERVER", &pszServer))
      pszServer = "";

   pszDatabase = GetArg ("database", 0);
   if (!pszDatabase && DosScanEnv ("DATABASE", &pszDatabase))
      pszDatabase = "DSShell";

   pszSA = GetArg ("sa", 0);
   if (!pszSA && DosScanEnv ("USERNAME", &pszSA))
      pszSA = "sa";

   pszPassword = GetArg ("password", 0);
   if (!pszPassword && DosScanEnv ("PASSWORD", &pszPassword))
      pszPassword = "";

   bVerbose = IsArg ("verbose") == 1;
   bNoHardError = IsArg ("noharderror") == 1;

   pszLog = GetArg ("log", 0);
   if (pszLog == NULL)
      pszLog = "sqlexec.log";

   pszLine = GetArg ("line", 0);
   if (pszLine)
      usLine = atoi (pszLine);
   else
      usLine = 0;

   if ((pfLog = DbInit (pszLog, pszServer, pszDatabase, pszSA, pszPassword)) == NULL)
      {
      fprintf (stderr, "Could not initialize the database module!\n");
      return 1;
      }

   if (pszFile)
      {
      pf = fopen (pszFile, "rt");
      if (pf == NULL)
         {
         fprintf (stderr, "Could not open file %s.\n", pszFile);
         fprintf (pfLog, "Could not open file %s.\n", pszFile);
         return 1;
         }
      }
   else
      {
      pf = stdin;
      pszFile = "<stdin>";
      }

   fprintf (pfLog, "Execing file %s\n", pszFile);
   i = SQLExec (pszFile, pf, pszLog, pfLog, usLine, stdout, 0);

   fclose (pf);

   DbDeInit ();
   return i;
   }


static int Usage (void)
   {
   printf ("usage: sqlexec [-verbose] [-server s] [-sa s] [-password s] [-help] [-?]\n"
           "               [-database s] [-line n] [-log s] [-noharderror] [file]\n");
   printf ("\n");
   printf ("This program will read one line from the input file, and execute it\n");
   printf ("on the SQL server.  If no file name is given, then stdin is used. \n");
   printf ("Blank lines ignored.  \n");
   printf ("Available options:\n");
   printf ("\n");
   printf ("   -verbose     Print out more status messages.\n");
   printf ("   -server s    Use the server named \"s\".\n");
   printf ("   -sa s        Use the user named \"s\".\n");
   printf ("   -password s  Use the password \"s\".\n");
   printf ("   -help        Display this help.\n");
   printf ("   -?           Display this help.\n");
   printf ("   -database s  Use the database named \"s\".\n");
   printf ("   -line n      Start executing at line n.\n");
   printf ("   -log s       Store the log in the file named \"s\".\n");
   printf ("   -noharderror Don't display hard error screens.\n");
   printf ("\n");
   printf ("A log of lines sent to the SQL server, as well as messages and errors\n");
   printf ("is placed in the file sqlexec.log.\n");
   printf ("\n");
   printf ("The following characters have special meaning when in the first \n");
   printf ("column of a line:\n");
   printf ("\n");
   printf (" Char    Meaning\n");
   printf (" ----    ---------------------------------------------------------\n");
   printf ("   ;     This line is a comment.  The line will not be sent to the\n");
   printf ("         SQL server.\n");
   printf ("\n");
   printf ("   *     Ignore any errors this query may cause.\n");
   printf ("\n");
   printf ("   \"     Print the results of this query to stdout.  Each column\n");
   printf ("         returned is separated by a tab character.  If the digits\n");
   printf ("         1 - 9 follow this character, then only that column is\n");
   printf ("         printed.\n");
   printf ("\n");
   printf ("   #     Same as the '*' and '\"' combined: print and ignore errors.\n");
   printf ("\n");
   printf ("   +     The query continues on the next line.  The last line of the \n");
   printf ("         query should not have a '+' as the first character.  If\n");
   printf ("         you want to use any other special characters, place them\n");
   printf ("         only on the LAST line, the one without the '+'.\n");
   printf ("\n");
   printf ("   ? : ! Conditional execution.  The line marked with the '?' is\n");
   printf ("         executed.  If the query returns a non-zero row, or more\n");
   printf ("         than one row, queries marked with the ':' (true condition)\n");
   printf ("         are executed, otherwise the queries marked with the '!' (false\n");
   printf ("         condition) are executed.  If you need to use '*', '\"', or '#',\n");
   printf ("         place the ':' or '!' first on the line.  If an error occurs\n");
   printf ("         executing the '?' query, and the '*' or '#' options are not\n");
   printf ("         present, neither of the other queries are executed; otherwise,\n");
   printf ("         the '!' query is executed.  The result of the '?' is saved\n");
   printf ("         until a new '?' query is executed, so you may have as many\n");
   printf ("         ':' and '!' queries as you need.  The order of the ':' and\n");
   printf ("         '!' queries is not important.\n");
   printf ("\n");

   return 0;
   }

