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
 * Columns.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 * This file is part of DS/Shell.
 *
 * This module parses the Table name/Column name of DS/Shell import
 * files.
 */

#include "..\include\iti.h"
#include "..\include\itiutil.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itiimp.h"
#include "..\include\colid.h"
#include "..\include\itierror.h"
#include "import.h"
// #include "util.h"
#include "columns.h"

#include <stdio.h>


static CHAR szMsg[180];


/*
 * CopyDBName copies the name pszOriginal to pszDest, removing all
 * '-'s and '/'s.  The string is terminated at the first null character
 * or the first '.'.
 */

static  void EXPORT ItiDbCopyName (PSZ pszDest, PSZ pszOriginal, USHORT usDestMax)
   {
   BOOL bDone;

   for (bDone = FALSE; !bDone && usDestMax; pszOriginal++)
      {
      if (*pszOriginal != '-' && *pszOriginal != '/' && *pszOriginal != '.')
         {
         *pszDest++ = *pszOriginal;
         usDestMax--;
         bDone = !*pszOriginal;
         }
      if (*pszOriginal == '.')
         {
         *pszDest = '\0';
         bDone = TRUE;
         }
      }
   }




/*
 * GetTableName gets the name of the table that's being imported.
 *
 */

static USHORT EXPORT GetTableName (HHEAP         hheap, 
                            PCSVFILEINFO  pcfi,
                            PIMPORTINFO   pii)
   {
   char     szTableName [MAX_TABLE_NAME_SIZE];
   char     szTemp [MAX_TABLE_NAME_SIZE + 100];
   PSZ      *ppsz, psz;
   USHORT   usNumCols;

   pii->pti = ItiMemAlloc (hheap, sizeof (TABLEINFO), MEM_ZERO_INIT);
   if (pii->pti == NULL)
      return ERROR_NO_MEMORY;

   /* get the table name */
   if (!ItiFileGetCSVWord (pcfi, szTableName, NULL, sizeof szTableName))
      return END_OF_FILE;

   /* verify that it is a table */
   pii->pti->usTableID = ItiColStringToTableId (szTableName);
   if (COLID_ERROR == pii->pti->usTableID)
      {
#if defined (DEBUG)
      printf ("Bad table '%s'\n", szTableName);
#endif
      return ERROR_BAD_TABLE;
      }

   /* store a copy of the table name */
   psz = ItiColTableIdToString (pii->pti->usTableID);
   pii->pti->pszTableName = ItiStrDup (hheap, psz);

   /* check for an associated DLL */
   pii->pti->hmod = 0;
   pii->pti->pfnCopy = NULL;
   sprintf (szTemp, "SELECT DllName, CopyFunction, MergeFunction, CalcFunction "
                    "FROM Tables "
                    "WHERE TableKey = %d ", 
                    pii->pti->usTableID);
   ppsz = ItiDbGetRow1 (szTemp, hheap, 0, 0, 0, &usNumCols);
   if (ppsz == NULL)
      {
#if defined (DEBUG)
      printf ("Table '%s' not found in metadata.\n", szTableName);
#endif
      return ERROR_BAD_TABLE;
      }

   if (ppsz [0] && *ppsz [0])
      {
      pii->pti->hmod = ItiDllLoadDll (ppsz [0], "1");
      if (pii->pti->hmod == 0)
         {
#if defined (DEBUG)
         printf ("Error loading DLL '%s'\n", ppsz [0]);
#endif
         ItiFreeStrArray (hheap, ppsz, usNumCols);
         return ERROR_DLL_LOAD_ERROR;
         }

      if (ppsz [1] && *ppsz [1])
         {
         pii->pti->pfnCopy = ItiDllQueryProcAddress (pii->pti->hmod,
                                                     ppsz [1]);
         if (pii->pti->pfnCopy == NULL)
            {
#if defined (DEBUG)
            printf ("Error loading function '%s' from DLL '%s'\n", 
                    ppsz [1], ppsz [0]);
#endif
            ItiFreeStrArray (hheap, ppsz, usNumCols);
            return ERROR_DLL_FUNCTION_NOT_FOUND;
            }
         }

      if (ppsz [2] && *ppsz [2])
         {
         pii->pti->pfnMerge = ItiDllQueryProcAddress (pii->pti->hmod,
                                                     ppsz [2]);
         if (pii->pti->pfnMerge == NULL)
            {
#if defined (DEBUG)
            printf ("Error loading function '%s' from DLL '%s'\n", 
                    ppsz [2], ppsz [0]);
#endif
            ItiFreeStrArray (hheap, ppsz, usNumCols);
            return ERROR_DLL_FUNCTION_NOT_FOUND;
            }
         }
      if (ppsz [3] && *ppsz [3])
         {
         pii->pti->pfnCalc = ItiDllQueryProcAddress (pii->pti->hmod,
                                                     ppsz [3]);
         if (pii->pti->pfnCalc == NULL)
            {
#if defined (DEBUG)
            printf ("Error loading function '%s' from DLL '%s'\n", 
                    ppsz [3], ppsz [0]);
#endif
            ItiFreeStrArray (hheap, ppsz, usNumCols);
            return ERROR_DLL_FUNCTION_NOT_FOUND;
            }
         }
      }
   ItiFreeStrArray (hheap, ppsz, usNumCols);

   return pii->pti->pszTableName == NULL ? ERROR_NO_MEMORY : 0;
   }





/*
 * GetFileColumnNames reads the columns to be imported from the
 * import file.
 */

static USHORT EXPORT GetFileColumnNames (HHEAP         hheap,
                                  PCSVFILEINFO  pcfi,
                                  PIMPORTINFO   pii,
                                  PCOLINFO      pci,
                                  USHORT        usMaxColInfos)
   {
   USHORT i, usId;
   char   szColumnName [MAX_COLUMN_NAME_SIZE];
   PSZ    psz;

   i = 0;
   while (i < usMaxColInfos && 
          ItiFileGetCSVWord (pcfi, szColumnName, NULL, sizeof szColumnName))
      {
//      ItiDbCopyName (szTemp, szColumnName, sizeof szTemp);
      usId = ItiColStringToColId (szColumnName);
      psz = ItiColColIdToString (usId);
      pci [i].pszImportName        = ItiStrDup (hheap, psz);
      pci [i].pszColumnName        = NULL;
      pci [i].pszColumnData        = NULL;
      pci [i].pszDefault           = NULL;
      pci [i].usColumnOrder        = 0;
      pci [i].usColumnMax          = 0;
      pci [i].pszFormat            = NULL;
      pci [i].bSysGenKey           = FALSE;
      pci [i].bNullable            = FALSE;
      pci [i].pfnFillInForeignKeys = NULL;
      pci [i].pszFileColumnName    = ItiStrDup (hheap, szColumnName); /* mar95 */
      i++;
      }

   ItiFileSkipCSVLine (pcfi);
   pii->usFileColumns = i;
   pii->usTotalColumns = i;

   if (i >= usMaxColInfos)
      return ERROR_TOO_MANY_COLUMNS;

   return 0;
   }


#define MAX_QUERY_SIZE 1024

#define IMPORT_NAME  0
#define COLUMN_NAME  1
#define FORMAT       2
#define COLUMN_ORDER 3
#define NULLABLE     4
#define MAX_SIZE     5
#define SYS_GEN_KEY  6
#define FOREIGN_KEY  7
#define DEFAULT      8
#define FOREIGN_FUNC 9


static USHORT EXPORT FindColumn (PCOLINFO     pci, 
                          PIMPORTINFO  pii, 
                          PSZ          pszImportName,
                          PUSHORT      pusIndex)
   {
   USHORT i;

   for (i=0; i < pii->usFileColumns; i++)
      {
      if (!ItiStrICmp (pci [i].pszImportName, pszImportName))
         {
         if (pci [i].pszColumnName != NULL)
            {
            return ERROR_METABASE;
            }
         else
            {
            *pusIndex = i;
            return 0;
            }
         }
      }

   /* column was not found -- append it to the end of the colinfo */
   if (pii->usTotalColumns + 1 < MAX_COLUMNS)
      {
      *pusIndex = pii->usTotalColumns;
      pii->usTotalColumns += 1;
      return 0;
      }

   return ERROR_TOO_MANY_COLUMNS;
   }




static PSZ pszCheckCol = "SELECT ImportName, ColumnName, Format, "
                         "ColumnOrder, Nullable, MaxColumnSize, "
                         "SystemGeneratedKey, ForeignKey, DefaultValue, "
                         "ForeignKeyFunction "
                         "FROM ImportInfo "
                         "WHERE TableKey=%d ";

/*
 *
 * 
 */

static USHORT EXPORT CheckColumns (HHEAP hheap, PCOLINFO pci, PIMPORTINFO pii)
   {
   HQRY     hqry;
   char     szQuery [MAX_QUERY_SIZE];
   USHORT   usNumColumns, usState;
   PSZ      *apszColumns;
   USHORT   usCount, i;
   USHORT   usError;

   sprintf (szQuery, pszCheckCol, pii->pti->usTableID);
   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usNumColumns, &usState);
   if (hqry == NULL)
      return ERROR_BAD_QUERY;

   usCount = 0;
   while (ItiDbGetQueryRow (hqry, &apszColumns, &usState))
      {
      usError = FindColumn (pci, pii, apszColumns [IMPORT_NAME], &i);
      if (usError)
         {
         ItiFreeStrArray (hheap, apszColumns, usNumColumns);
         ItiDbTerminateQuery (hqry);
         return usError;
         }

      pci [i].pszColumnName = apszColumns [COLUMN_NAME];
      pci [i].pszFormat = apszColumns [FORMAT];
      pci [i].pszDefault = apszColumns [DEFAULT];

      ItiStrToLONG (apszColumns [FOREIGN_KEY], &pci [i].kForeignKey);
      if (pci [i].kForeignKey != 0)
         {
         if (apszColumns [FOREIGN_FUNC] && *apszColumns [FOREIGN_FUNC] &&
             pii->pti->hmod != 0)
            {
            pci [i].pfnFillInForeignKeys = ItiDllQueryProcAddress (pii->pti->hmod,
                                           apszColumns [FOREIGN_FUNC]);
   
            if (pci [i].pfnFillInForeignKeys == NULL)
               {
               ItiFreeStrArray (hheap, apszColumns, usNumColumns);
               ItiDbTerminateQuery (hqry);
#if defined (DEBUG)
               printf ("Error loading function '%s'\n", 
                       apszColumns [FOREIGN_FUNC]);
#endif
               return ERROR_DLL_FUNCTION_NOT_FOUND;
               }
            }
         else
            {
            ItiFreeStrArray (hheap, apszColumns, usNumColumns);
            ItiDbTerminateQuery (hqry);
            return ERROR_MUST_HAVE_DLL;
            }
         }

      ItiStrToUSHORT (apszColumns [COLUMN_ORDER], &pci [i].usColumnOrder);
      ItiStrToUSHORT (apszColumns [NULLABLE], &pci [i].bNullable);
      ItiStrToUSHORT (apszColumns [SYS_GEN_KEY], &pci [i].bSysGenKey);
      ItiStrToUSHORT (apszColumns [MAX_SIZE], &pci [i].usColumnMax);

      /* add one for null terminator */
      pci [i].usColumnMax += 1;
      pci [i].pszColumnData = ItiMemAllocSeg ((pci[i].usColumnMax + 2), 0, 0);
      if (pci [i].pszColumnData == NULL)
         return ERROR_NO_MEMORY;
      pci [i].pszColumnData [0] = '\0';

      if (i >= pii->usFileColumns)
         {
         pci [i].pszImportName = apszColumns [IMPORT_NAME];
         if (pci[i].pszDefault)
            ItiStrCpy (pci [i].pszColumnData, pci [i].pszDefault, pci [i].usColumnMax);  
         }
      else
         ItiMemFree (hheap, apszColumns [IMPORT_NAME]);
      ItiMemFree (hheap, apszColumns [COLUMN_ORDER]);
      ItiMemFree (hheap, apszColumns [NULLABLE]);
      ItiMemFree (hheap, apszColumns [SYS_GEN_KEY]);
      ItiMemFree (hheap, apszColumns [FOREIGN_KEY]);
      ItiMemFree (hheap, apszColumns [MAX_SIZE]);
      ItiMemFree (hheap, apszColumns [FOREIGN_FUNC]);
      ItiMemFree (hheap, apszColumns);
      usCount++;
      }

   if (usCount == 0)
      {
      /* no rows returned from metabase -- return an error */
      return ERROR_METABASE;
      }

   /* make sure metadata exists for each column */
   for (i=0; i < pii->usFileColumns; i++)
      if (pci [i].pszColumnData == NULL)
         {
#if defined (DEBUG)
         printf ("Column '%s' - no metadata found in ImportInfo table\n", 
            //   pci [i].pszImportName);
                 pci[i].pszFileColumnName);
         sprintf(szMsg,"Column '%s' - no metadata found in ImportInfo table\n", pci[i].pszFileColumnName);
         ItiErrWriteDebugMessage(szMsg);

#endif
         return ERROR_BAD_COLUMN;
         }
   return 0;
   }





/*
 * GetColumnNames gets the columns to be imported from the file as
 * well as the columns to be defaluted.
 */

static USHORT EXPORT GetColumnNames (HHEAP          hheap,
                              PCSVFILEINFO   pcfi,
                              PIMPORTINFO    pii)
   {
   COLINFO  ci [MAX_COLUMNS];
   USHORT   usError;

   /* get the columns from the import file */
   usError = GetFileColumnNames (hheap, pcfi, pii, ci, MAX_COLUMNS);
   if (usError)
      return usError;

   /* verify the columns from the file, and add missing ones */
   usError = CheckColumns (hheap, ci, pii);
   if (usError)
      return usError;

   /* copy the local colinfo data to the pii */
   pii->pci = ItiMemDup (hheap, ci, sizeof (COLINFO) * pii->usTotalColumns);

   if (pii->pci == NULL)
      {
      FreeColumns (hheap, pii);
      return ERROR_NO_MEMORY;
      }

   return 0;
   }




/*
 * returns a pointer to a COLINFO, or NULL if the next CSV line
 * does not contain a valid table/column list.
 */

USHORT EXPORT  GetColumns (HHEAP         hheap, 
                   PCSVFILEINFO  pcfi,
                   PIMPORTINFO   *ppii)
   {
   USHORT      usError;
   CSVFILEINFO cfi;

   ItiFileSaveCSVPos (&cfi, pcfi);

   *ppii = ItiMemAlloc (hheap, sizeof (IMPORTINFO), MEM_ZERO_INIT);
   if (*ppii == NULL)
      return ERROR_NO_MEMORY;

   (*ppii)->ulRows = 0;
   (*ppii)->usError = 0;

   usError = GetTableName (hheap, pcfi, *ppii);
   if (usError)
      {
      ItiFileRestoreCSVPos (pcfi, &cfi);
      FreeColumns (hheap, *ppii);
      return usError;
      }

   usError = GetColumnNames (hheap, pcfi, *ppii);
   if (usError)
      {
      ItiFileRestoreCSVPos (pcfi, &cfi);
      FreeColumns (hheap, *ppii);
      return usError;
      }

   return 0;
   }



/* pci is not freed */

void EXPORT  FreeColumns (HHEAP hheap, PIMPORTINFO pii)
   {
   USHORT i;

   if (pii == NULL)
      return;

   if (pii->pti)
      {
      if (pii->pti->pszTableName)
         ItiMemFree (hheap, pii->pti->pszTableName);
      ItiMemFree (hheap, pii->pti);
      }

   if (pii->pci)
      {
      for (i=0; i < pii->usTotalColumns; i++)
         {
         if (pii->pci[i].pszImportName)
            ItiMemFree (hheap, pii->pci[i].pszImportName);
         if (pii->pci[i].pszColumnName)
            ItiMemFree (hheap, pii->pci[i].pszColumnName);
         if (pii->pci[i].pszFormat)
            ItiMemFree (hheap, pii->pci[i].pszFormat);
         if (pii->pci[i].pszColumnData)
            ItiMemFreeSeg (pii->pci[i].pszColumnData);
         if (pii->pci[i].pszDefault)
            ItiMemFree (hheap, pii->pci[i].pszDefault);
         }
      ItiMemFree (hheap, pii->pci);
      }

   ItiMemFree (hheap, pii);
   }



