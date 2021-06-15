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
 * Util.c
 *
 * (C) 1992 Info Tech Inc.
 * This file is part the EBS module
 */


                      
#define INCL_DOSMEMMGR
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <io.h>
#include "ebs.h"
#include "util.h"
#include "stuff.h"



/*
 * Removes chars from anywhere in string
 *
 */
PSZ ExtractString (PSZ pszStr, PSZ pszExtractList)
   {
   PSZ pszPtrSrc, pszPtrDest;

   if (pszStr == NULL)
      return NULL;

   pszPtrSrc = pszPtrDest = pszStr;
   while (*pszPtrSrc != '\0')
      {
      if (strchr (pszExtractList, *pszPtrSrc) == NULL)
         *pszPtrDest++ = *pszPtrSrc;
      pszPtrSrc++;
      }
   *pszPtrDest = '\0';
   return pszStr;
   }


USHORT Beep (void)
   {
   DosBeep (123, 75);
   return 1;
   }

PVOID MyDosAllocSeg   (USHORT uSize)
   {
   USHORT uSel;

   if (DosAllocSeg (uSize, &uSel, SEG_NONSHARED))
      return NULL;
   return MAKEP (uSel, 0);
   }

PVOID MyDosReAllocSeg (PVOID p, USHORT uSize)
   {
   if (!p)
      return MyDosAllocSeg (uSize);

   if (DosReallocSeg (uSize, SELECTOROF (p)))
      return NULL;
   return p;
   }




VOID MyDosFreeSeg (PVOID p)
   {
   if (!p)
      return;

   DosFreeSeg (SELECTOROF (p));
   }



/* the next lines are VERY important! */
#undef malloc
#undef realloc


PVOID MyDosAlloc2 (USHORT uSize, PSZ pszFile, USHORT usLine)
   {
   PVOID p;

   if (!uSize)
      return NULL;

   if (!(p = malloc (uSize)))
      {
      printf ("Could not alloc %d bytes\nSource file %s, line %u",
              uSize, pszFile, usLine);
      exit (1);
      }
   return p;
   }

PVOID MyDosReAlloc2 (PVOID p, USHORT uSize, PSZ pszFile, USHORT usLine)
   {
   if (!(p = realloc (p, uSize)))
      {
      printf ("Could not realloc %d bytes.\nSource file %s, line %u",
              uSize, pszFile, usLine);
      exit (1);
      }
   return p;
   }

PSZ MyStrDup2 (PSZ psz, PSZ pszFile, USHORT usLine)
   {
   USHORT uLen;
   PSZ    psz2;

   if (!psz)
      return NULL;

   uLen = strlen (psz);

   if (!(psz2 = malloc (uLen + 1)))
      {                                                         
      printf ("Could not strdup %d bytes\n Source file %s, line %u",
              uLen+1, pszFile, usLine);
      exit (1);
      }
   strcpy (psz2, psz);
   return psz2;
   }


/*
 * ItiStrCpy copies pszSource to pszDest, storing up to 
 * usDestMax bytes. pszDest is always null terminated.
 *
 * Parameters: pszDest        A pointer to the destination string.
 *
 *             pszSource      A pointer to the string to copy.
 *
 *             usDestMax      The maximum number of bytes to store 
 *                            in the string pszDest.
 *
 * Return value: pszDest is always returned.
 *
 * Comments:  If pszSource contains more characters that pszDest
 * can contain, then the copy is aborted, with only usDestMax-1 
 * characters being copied to pszDest.
 */

PSZ StrCpy (PSZ pszDest, PSZ pszSource, USHORT usDestMax)
   {
   USHORT   i;

   for (i=0; i < usDestMax && pszSource [i] != '\0'; i++)
      {
      pszDest [i] = pszSource [i];
      }
   i = (i > usDestMax - 1) ? usDestMax - 1 : i;
   pszDest [i] = '\0';
   return pszDest;
   }



USHORT ReplaceParams (PSZ    pszDest,
                      PSZ    pszSource,
                      USHORT usDestMax,
                      PSZ    apszTokens [], 
                      PSZ    apszValues [], 
                      USHORT usNumValues)
   {
   PSZ      pszEnd;
   USHORT   usNotFound, usLen, i;
   char     szToken [256];
   BOOL     bFound;


   usNotFound = 0;
   while (*pszSource && usDestMax)
      {
      if (*pszSource == '%')
         {
         pszSource++;
         if (*pszSource == '%')
            {
            *pszDest++ = *pszSource++;
            usDestMax--;
            }
         else
            {
            for (pszEnd = pszSource; 
                 *pszEnd && ((*pszEnd >= 'a' && *pszEnd <= 'z') || 
                             (*pszEnd >= 'A' && *pszEnd <= 'Z') ||
                             (*pszEnd >= '0' && *pszEnd <= '9') ||
                             *pszEnd == '_');
                 pszEnd++)
               ;
            StrCpy (szToken, pszSource, sizeof szToken < pszEnd - pszSource + 1? 
                                        sizeof szToken : pszEnd - pszSource + 1);
            for (i=0, bFound = FALSE; apszTokens [i] && i < usNumValues; i++)
               if ((bFound = stricmp (apszTokens [i], szToken) == 0))
                  break;
            if (bFound)
               {
               if (apszValues [i] == NULL)
                  *pszDest = '\0';
               else
                  strcpy (pszDest, apszValues [i]);
               usLen = strlen (pszDest);
               pszDest += usLen;
               usDestMax -= usLen;
               pszSource = pszEnd;
               }
            else
               {
               /* token not found, so just put the %token in the string */
               usNotFound++;
               *pszDest++ = '%';
               usDestMax--;
               }
            }
         }
      else
         {
         *pszDest++ = *pszSource++;
         usDestMax--;
         }
      }
   *pszDest = '\0';
   return usNotFound;
   }






PSZ GetNextPathElement (PSZ pszPath)
   {
   static PSZ pszBuffer = NULL;
   static int iOldPos = 0;
   static int iMaxLen = 0;
   int iCurrentPos;

   if (pszPath != NULL)
      {
      pszBuffer = pszPath;
      iOldPos = 0;
      iMaxLen = strlen (pszPath);
      iCurrentPos = 0;
      }
   else if (pszBuffer == NULL || iOldPos+1 >= iMaxLen)
      {
      return NULL;
      }
   else
      {
      *(pszBuffer + iOldPos) = '\\';
      pszPath = pszBuffer + iOldPos + 1;
      iCurrentPos = iOldPos + 1;
      }

   while (iCurrentPos < iMaxLen && *pszPath != '\0' && 
          *pszPath != '/' && *pszPath != '\\')
      {
      iCurrentPos++;
      pszPath++;
      }

   if (*pszPath != '\0' && iCurrentPos < iMaxLen)
      {
      *pszPath = '\0';
      }

   iOldPos = iCurrentPos;

   return pszBuffer;
   }




BOOL MakePath (PSZ pszPathName)
   {
   char  szCurrentDir [512];
   PSZ   pszPath;

   strcpy (szCurrentDir, pszPathName);

   /* start checking directories, starting at the root. */
   pszPath = GetNextPathElement (szCurrentDir);
   
   if (pszPath != NULL)
      {
      /* note: the first time through, skip the root dir */
      pszPath = GetNextPathElement (NULL);
      }
   else
      {
      /* this should never happen */
      return FALSE;
      }

   while (pszPath != NULL)
      {
      if (chdir (pszPath) != 0)
         {
         /* path does not exist */
         if (mkdir (pszPath) != 0)
            {
            return FALSE;
            }
         }
      pszPath = GetNextPathElement (NULL);
      }

   return TRUE;
   }



ULONG GetDriveFreeSpace (char cDrive)
   {
   FSALLOCATE fsa;
   ULONG ulFreeSpace;

   if (DosQFSInfo ((USHORT) (toupper (cDrive) - '@'), FSIL_ALLOC, 
                   (PBYTE) &fsa, sizeof fsa))
      {
      return 0;
      }


   ulFreeSpace = (ULONG) fsa.cbSector *
                 (ULONG) fsa.cSectorUnit *
                 (ULONG) fsa.cUnitAvail;
   return ulFreeSpace;
   }


BOOL DriveExists (char cDrive)
   {
   ULONG    ulDrives;
   USHORT   usCurrentDisk;

   DosQCurDisk (&usCurrentDisk, &ulDrives);

   return (BOOL) ((ulDrives >> (toupper (cDrive) - 'A')) & 1);
   }


BOOL CheckForFile (PSZ pszFile, char cDrive)
   {
   char szTemp [256];

   if (!DriveExists (cDrive))
      return FALSE;
   sprintf (szTemp, "%c:%s", cDrive, pszFile);
   return !access (szTemp, 0);
   }

