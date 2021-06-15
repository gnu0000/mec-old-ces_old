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


#define INCL_DOSFILEMGR
#include "..\include\iti.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <share.h>
#include "dumpdll.h"

#ifndef EXEONLY
#include "..\include\itiglob.h"
#include "window.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\itibase.h"
#include "..\include\itimbase.h"
#include "..\include\itidbase.h"
#include "..\include\itiutil.h"
#include "..\include\colid.h"
#include "..\include\itierror.h"
#include "..\include\dialog.h"
#include "..\include\itirpt.h"

#endif



/* Global variables */

static FILEFINDBUF findbuf;

static ULONG ulNumFiles  = 0;
static ULONG ulFileTotal = 0;
static ULONG ulFileAlloc = 0;
static ULONG ulTotalCRC  = 0;

static BOOL bColOne = TRUE;


char *szMakeFileName (char *szResult, char *szDir, char *szName)
   {
   int i;

   strcpy (szResult, szDir);

   /* If necessary, add a trailing \. */
   i = strlen(szResult)-1;
   if (szResult[i] != '\\'
    && szResult[i] != '/'
    && szResult[i] != ':')
      strcat (szResult, "\\");

   /* Finally, add the name */
   strcat (szResult, szName);

   return szResult;
   }


#define   MASK   0xA0000001


/*
 * Do not modify unless you are really sure you know what 
 * you are doing!
 *
 * This function blatently stolen from EBS.
 */

ULONG CRC32 (ULONG ulCRC, CHAR c)
   {
   USHORT i;

   for (i=0; i<8; i++)
      {
      if((ulCRC ^ c) & 1)
         ulCRC = (ulCRC>>1) ^ MASK;
      else
         ulCRC>>=1;
      c>>=1;
      }
   return ulCRC;
   }


ULONG GetFileCRC (PSZ pszFileName)
   {
   FILE *pf;
   int c;
   ULONG ulCRC = 0;

   if (NULL == (pf = _fsopen (pszFileName, "rb", SH_DENYWR)))
      return -1;

   while ((c = getc (pf)) != EOF)
      ulCRC = CRC32 (ulCRC, (char) c);

   fclose (pf);
   return ulCRC;
   }


void PrintFile (FILEFINDBUF *pFileBuf, char *szSearchDir)
   {
   ULONG ulDateTime, ulCRC;
   char  szBuffer [256];
   USHORT i;

   ulNumFiles++;
   ulFileTotal += pFileBuf->cbFile;
   ulFileAlloc += pFileBuf->cbFileAlloc;

   ulDateTime = (((ULONG) pFileBuf->fdateLastWrite.year+1980  ) << 20) |
                (((ULONG) pFileBuf->fdateLastWrite.month      ) << 16) |
                (((ULONG) pFileBuf->fdateLastWrite.day        ) << 10) |
                (((ULONG) pFileBuf->ftimeLastWrite.hours      ) <<  5) |
                 ((ULONG) pFileBuf->ftimeLastWrite.minutes / 2);

   szMakeFileName (szBuffer, szSearchDir, pFileBuf->achName);
   ulCRC = GetFileCRC (szBuffer);
   ulTotalCRC += ulCRC;
   sprintf (szBuffer, "%p %p %5.5lx %-12.12s", ulDateTime, ulCRC, pFileBuf->cbFile, pFileBuf->achName);
   
   for (i=0; szBuffer [i]; i++)
      szBuffer [i] = (char) tolower (szBuffer [i]);

#ifdef EXEONLY
   fputs (szBuffer, stdout);
   if (bColOne)
      fputc (' ', stdout);
   else
      fputc ('\n', stdout);
#else
   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL + (bColOne ? 0 : 2),
                    LEFT, USE_RT_EDGE, szBuffer);
   if (!bColOne)
      ItiPrtIncCurrentLn ();

#endif

   bColOne = !bColOne;
   }










BOOL bMatchWildCard (char *szString, char *szWildCard)
   {
   switch (*szWildCard)
      {
      case '\0':
         return '\0' == *szString;
         break;

      case '?':
         return '\0' != *szString &&
                bMatchWildCard (szString+1, szWildCard+1);
         break;

      case '*':
         if ('\0' == szWildCard[1])
            return TRUE;
         while ('\0' != *szString)
            {
            if (bMatchWildCard (szString, szWildCard+1))
               return TRUE;
            szString++;
            }
         return bMatchWildCard (szString, szWildCard+1);
         break;

      default:
         if ('\0' == *szString)
            return FALSE;
         return (toupper(*szString) == toupper(*szWildCard)) &&
                bMatchWildCard (szString+1, szWildCard+1);
         break;
      }

   return TRUE;
   }









void SearchDir (char *szSearchDir)
   {
   HDIR        hdir;
   USHORT      usSearchCount;
   char        szFileName [CCHMAXPATHCOMP];
   USHORT      usResult;
   char        szBuffer [256];

   usSearchCount = 1;
   hdir = HDIR_CREATE;

   sprintf (szBuffer, "\nDate:Time CRC-32    Size  File Name    Date:Time CRC-32    Size  File Name    \n");
   fputs (szBuffer, stdout);
   sprintf (szBuffer, "--------- --------- ----- ------------ --------- --------- ----- ------------ \n");
   fputs (szBuffer, stdout);



   usResult = DosFindFirst(szMakeFileName (szFileName, szSearchDir, "*.*"),
                           &hdir, FILE_NORMAL | FILE_DIRECTORY,
                           &findbuf, sizeof(findbuf),
                           &usSearchCount, 0L);

   while (0 == usResult && 0 != usSearchCount)
      {
      if (0 == (findbuf.attrFile & FILE_DIRECTORY))
         {
         if (bMatchWildCard (findbuf.achName, "*.EXE") ||
             bMatchWildCard (findbuf.achName, "*.DLL"))
            PrintFile (&findbuf, szSearchDir);
         }
      usSearchCount = 1;
      usResult = DosFindNext(hdir, &findbuf, sizeof(findbuf), &usSearchCount);
      }
   DosFindClose (hdir);
   }

void DumpFiles (void)
   {
   char szTemp [256];

   SearchDir (".");

   sprintf (szTemp, "%.3d: %p %p", (USHORT) ulNumFiles, ulTotalCRC, ulFileTotal);

#ifdef EXEONLY
   if (bColOne)
      fputc ('\n', stdout);
   fputc ('\n', stdout);
   fputs (szTemp, stdout);
   fputc ('\n', stdout);
#else
   if (bColOne)
      ItiPrtIncCurrentLn ();
   ItiPrtIncCurrentLn ();
   ItiPrtPutTextAt (CURRENT_LN, CURRENT_COL, LEFT, USE_RT_EDGE, szTemp);
   ItiPrtIncCurrentLn ();
#endif
   }



#ifdef EXEONLY

void main (int argc, char *argv [])
   {
   int i;

   fprintf (stderr, "Copyright (C) "COPYRIGHT" Info Tech, Inc.\nDumpDll Release "VERSION_STRING"\n");
   fflush (stderr);
   if (argc > 1)
      for (i=1; i < argc; i++)
         {
         ulNumFiles = 0;
         ulTotalCRC = 0;
         ulFileTotal = 0;
         ulFileAlloc = 0;
         if (bColOne)
            fputc ('\n', stdout);
         bColOne = TRUE;
         printf ("Directory %s:\n", argv [i]);
         SearchDir (argv [i]);

         printf ("\n%.3d: %p %p\n", (USHORT) ulNumFiles, ulTotalCRC, ulFileTotal);
         }
   else
      DumpFiles ();
   }

#endif
