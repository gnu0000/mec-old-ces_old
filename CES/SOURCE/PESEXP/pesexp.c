/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1992 by AASHTO.  All Rights Reserved.                 |
|                                                                           |
|       This program module is part of BAMS/CES, a module of BAMS,          |
|       The American Association of State Highway and Transportation        |
|       Officials' (AASHTO) Information System for Managing Transportation  |
|       Programs, a proprietary product of AASHTO, no part of which may be  |
|       reproduced or transmitted in any form or by any means, electronic,  |
|       mechanical, or otherwise, including photocopying and recording      |
|       or in connection with any information storage or retrieval          |
|       system, without permission in writing from AASHTO.                  |
|                                                                           |
+--------------------------------------------------------------------------*/


/*
 * PESExp.c
 * Mark Hampton
 *
 * This module exports BAMS/PES (PC) projects to a mainframe BAMS
 * dump file.
 */

#define INCL_DOSMISC
#include "..\include\iti.h"
#include "..\include\itiutil.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itiimp.h"
#include "..\include\itifmt.h"
#include "..\include\itierror.h"
#include "..\include\itiglob.h"
#include <stdio.h>
#include <stdlib.h>
#include <share.h>
#include "PESExp.h"

#define MAX_BUFFER_SIZE 65535U
#define MIN_BUFFER_SIZE 16383U


/* Global variables */
PSZ pszBuffer = NULL;
USHORT usBufferSize = 0;
PGLOBALS pglobals;





static void Usage (void);
static BOOL InitPESExp (HHEAP hheap);
static BOOL PESExportJob (HHEAP hheap, PSZ pszFileName, PSZ pszJobKey);
HHEAP hheap;


void Copyright ()
   {
   char szTemp [256];

   putchar ('\n');
#ifdef PRERELEASE
   sprintf (szTemp, "BAMS/CES to BAMS/PES Export Release %s %s",
            VERSION_STRING, PRERELEASE);
#else
   sprintf (szTemp, "BAMS/CES to BAMS/PES Export Release %s", VERSION_STRING);
#endif

   ItiErrWriteDebugMessage (szTemp);
   puts (szTemp);
   puts ("Copyright (c) "COPYRIGHT" AASHTO.");
   printf ("Log file is: %s.  (Note this in case of error)\n",
           ItiErrQueryLogFileName ());
   }

int _cdecl main (int argc, char *argv [])
   {
   Copyright ();

   if (argc != 3)
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

   if (!InitPESExp (hheap))
      {
      printf ("Could not initialize the PESExp program!\n");
      return 4;
      }

   sprintf (pszBuffer, "Exporting to file '%s', JobKey=%s",
            argv [1], argv [2]);
   ItiErrWriteDebugMessage (pszBuffer);

   PESExportJob (hheap, argv [1], argv [2]);

   ItiMemDestroyHeap (hheap);
   ItiMemFreeSeg (pszBuffer);
   ItiDbTerminate ();
   return 0;
   }



static BOOL InitPESExp (HHEAP hheap)
   {
   pglobals = ItiGlobInitGlobals (NULL, NULL, NULL, NULL, "PESExp", "PESExp", VERSION_STRING);
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

   usBufferSize = MAX_BUFFER_SIZE;
   pszBuffer = ItiMemAllocSeg (usBufferSize, SEG_NONSHARED, MEM_ZERO_INIT);
   while (pszBuffer == NULL && usBufferSize < MIN_BUFFER_SIZE)
      {
      usBufferSize = usBufferSize - usBufferSize / 4;
      pszBuffer = ItiMemAllocSeg (usBufferSize, SEG_NONSHARED, MEM_ZERO_INIT);
      }

   if (pszBuffer == NULL)
      {
      printf ("Could not allocate enough memory for buffers!\n");
      return FALSE;
      }

   return TRUE;
   }

static void Usage (void)
   {
   printf ("\nUsage: PESExp filename JobKey,[JobKey...]\n");
   printf ("\nWhere: filename is the name of the file to create.\n");
   printf ("       JobKey is the key for the Job to export.  If more than one\n");
   printf ("       Job is to be exported, separate the keys with a comma, do\n");
   printf ("       not use spaces\n\n");
   }



/**************************************************************************
 * Export functions
 **************************************************************************/

static BOOL ExportProjectItem (HHEAP   hheap, 
                               FILE    *pfOut, 
                               PSZ     pszPCN, 
                               PSZ     pszCategory)
   {
   HQRY     hqry;
   CHAR     szQuery [2046] = "";
   CHAR     szTemp [2046] = "";
   USHORT   usNumCols, usState, usNumItems;
   PSZ      *ppsz, psz;
   BOOL     bOK;
   double   d;

   ItiMbQueryQueryText (0, ITIRID_EXPORT, PESEXP_PROJECT_ITEM, 
                        szTemp, sizeof szTemp);
   sprintf (szQuery, szTemp, pszPCN, pszCategory);

   hqry = ItiDbExecQuery (szQuery, hheap, 0, ITIRID_EXPORT, 
                          PESEXP_PROJECT_ITEM, &usNumCols, &usState);
   if (hqry == NULL)
      {
      printf ("Could not execute the query to get the project items!\n");
      return FALSE;
      }

   bOK = TRUE;
   usNumItems = 0;
   while (ItiDbGetQueryRow (hqry, &ppsz, &usState))
      {

//    6-16-93 mdh: changed this from int to double, since quanties 
//    between 0 and 1 did not get exported.
      d = strtod (ppsz [2], &psz);
      if (d == 0)
         continue;
      fprintf (pfOut, "ESTITEM1 %-4s %-13s %13s %14s\n",
               ppsz [0], ppsz [1], ppsz [2], ppsz [3]);

      /* -- Supplemental description, characters 1 to 40 */
      fprintf (pfOut, "ESTITEM2 %-40s\n", ppsz [4]);

      /* -- Supplemental description, characters 41 to 81, added JAN 96  */
            /* -- A design bug: There is no way to edit a project item's */
            /* -- supplemental description field, ppsz[4 & 5]; however, a*/
            /* -- job item's supplemental description field, ppsz[6 & 7] */
            /* -- can be edited from within CES.                         */
      if ((*ppsz[5] != '\0') && (ItiStrLen(ppsz[5]) > 0))
         {
         fprintf (pfOut, "ESTITEM3 %-40s\n", ppsz [5]);
         }
      else if ((*ppsz[6] != '\0') && (ItiStrLen(ppsz[6]) > 0))
         {
         fprintf (pfOut, "ESTITEM3 %-40s\n", ppsz [6]);
         // if ((*ppsz[7] != '\0') && (ItiStrLen(ppsz[7]) > 0))
         //    fprintf (pfOut, "ESTITEM4 %-40s\n", ppsz [7]);
         }


      ItiFreeStrArray (hheap, ppsz, usNumCols);
      usNumItems++;
      }

   return bOK;
   }




static BOOL ExportCategories (HHEAP hheap, 
                              FILE  *pfOut, 
                              PSZ   pszPCN)
   {
   HQRY     hqry;
   char     szQuery [510], szTemp [510];
   USHORT   usNumCols, usState, usNumCategories;
   PSZ      *ppsz;
   BOOL     bOK;

   ItiMbQueryQueryText (0, ITIRID_EXPORT, PESEXP_CATEGORY, 
                        szTemp, sizeof szTemp);
   sprintf (szQuery, szTemp, pszPCN);

   hqry = ItiDbExecQuery (szQuery, hheap, 0, ITIRID_EXPORT, 
                          PESEXP_CATEGORY, &usNumCols, &usState);
   if (hqry == NULL)
      {
      printf ("Could not execute the query to get the categories!\n");
      return FALSE;
      }

   bOK = TRUE;
   usNumCategories = 0;
   while (ItiDbGetQueryRow (hqry, &ppsz, &usState))
      {
      fprintf (pfOut, "ESTCATG1 %-4s\n",
               ppsz [0]);

      /* export the items for the category */
      bOK &= ExportProjectItem (hheap, pfOut, pszPCN, ppsz [0]);
      ItiFreeStrArray (hheap, ppsz, usNumCols);
      usNumCategories++;
      }

   return bOK;
   }





static BOOL ExportProjects (HHEAP   hheap, 
                            FILE    *pfOut, 
                            PSZ     pszJobKey)
   {
   HQRY     hqry;
   char     szQuery [2040], szTemp [2040];
   USHORT   usNumCols, usState, usNumProjects, i, usError;
   PSZ      *ppsz, *apszPCNs;
   BOOL     bOK;

   ItiMbQueryQueryText (0, ITIRID_EXPORT, PESEXP_GETPROJECTS, 
                        szTemp, sizeof szTemp);
   sprintf (szQuery, szTemp, pszJobKey);
   usNumProjects = ItiDbGetQueryCount (szQuery, &usNumCols, &usError);
   if (usNumProjects == 0)
      return TRUE;

   apszPCNs = ItiMemAlloc (hheap, sizeof (PSZ) * usNumProjects, MEM_ZERO_INIT);
   if (apszPCNs == NULL)
      return FALSE;

   ItiMbQueryQueryText (0, ITIRID_EXPORT, PESEXP_GETPROJECTS, 
                        szTemp, sizeof szTemp);
   sprintf (szQuery, szTemp, pszJobKey);
   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usNumCols, &usState);
   if (hqry == NULL)
      {
      printf ("Could not execute the query to get the projects!\n");
      return FALSE;
      }

   for (i=0; ItiDbGetQueryRow (hqry, &ppsz, &usState); i++)
      {
      if (i < usNumProjects)
         apszPCNs [i] = *ppsz;
      else
         ItiMemFree (hheap, *ppsz);
      ItiMemFree (hheap, ppsz);
      }

   bOK = TRUE;
   for (i=0; i < usNumProjects; i++)
      {
      ItiMbQueryQueryText (0, ITIRID_EXPORT, PESEXP_PROJECT, 
                           szTemp, sizeof szTemp);
      sprintf (szQuery, szTemp, apszPCNs [i]);
      ppsz = ItiDbGetRow1 (szQuery, hheap, 0, ITIRID_EXPORT, PESEXP_PROJECT, 
                           &usNumCols);

      if (ppsz == NULL)
         continue;

      fprintf (pfOut, "PROJECT1 %-13s %-2s %-4s %-5s %-s\n",
               ppsz [0], ppsz [1], ppsz [2], ppsz [3], ppsz [4]);
      fprintf (pfOut, "PROJECT2 %-60s\n",
               ppsz [5]);
      fprintf (pfOut, "PROJECT3 %-60s\n",
               ppsz [6]);
      ItiFreeStrArray (hheap, ppsz, usNumCols);

      bOK &= ExportCategories (hheap, pfOut, apszPCNs [i]);
      }

   ItiFreeStrArray (hheap, apszPCNs, usNumProjects);

   return bOK;
   }




static BOOL PESExportJob (HHEAP hheap, 
                          PSZ   pszFileName, 
                          PSZ   pszJobKey)
   {
   FILE     *pfOut;
   BOOL     bOK;

   pfOut = _fsopen (pszFileName, "wt", SH_DENYWR);
   if (pfOut == NULL)
      {
      printf ("Could not open file %s for output.\n", pszFileName);
      return FALSE;
      }

   fprintf (pfOut, "BAMS Import File,2.1,PES Project\n");

   bOK = ExportProjects (hheap, pfOut, pszJobKey);

   fclose (pfOut);
   return FALSE;
   }

