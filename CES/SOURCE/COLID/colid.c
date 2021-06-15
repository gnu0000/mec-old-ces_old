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
 * colid.c
 */

#define INCL_WIN
#define INCL_DOSRESOURCES
#include "..\INCLUDE\Iti.h"
#include "..\include\resource.h"
#include "..\include\itimbase.h"
#include "..\include\itiglob.h"
#include "..\include\itierror.h"
#include "..\include\itiutil.h"
#include "..\include\colid.h"
#include "initdll.h"

#include <search.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


CHAR szEng[] = "0";
CHAR szMet[] = "1";

PSZ pszEnglish = szEng;
PSZ pszMetric = szMet;



IDX    *pidxColumns = NULL;
USHORT usColCount = 0;

IDX    *pidxTables = NULL;
USHORT usTableCount = 0;

TABCOL *ptabcol = NULL;
USHORT usTabColCount = 0;


#define MAX_COL_SIZE    32
#define MAX_TABLE_SIZE  32


/*
 * CopyDBName copies the name pszOriginal to pszDest, removing all
 * '-'s and '/'s.  The string is terminated at the first null character
 * or the first '.'.
 */

static void CopyDBName (PSZ pszDest, PSZ pszOriginal, USHORT usDestMax)
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


USHORT ColErr (PSZ psz)
   {
   if (pglobals->hwndAppFrame)
      WinMessageBox (HWND_DESKTOP, pglobals->hwndAppFrame, psz,
                     "Col ID Error", 0, MB_OK | MB_MOVEABLE);
   else
      DosBeep (500, 50);
   ItiErrWriteDebugMessage (psz);
   return 1;
   }


int _cdecl IDXCmpFn (void *p1, void *p2)
   {
   return stricmp ((PSZ)p1, MAKEP (SELECTOROF (p2), ((IDX *)p2)->uOffset));
   }



USHORT EXPORT ItiColStringToColId (PSZ pszCol)
   {
   PSZ   pszTemp;
   IDX   *pidxMatch;
   char  szColumn [MAX_COL_SIZE + 1];
   PGLOBALS pglobColID;

   pglobColID = ItiGlobQueryGlobalsPointer ();

   if (isdigit (*pszCol))
      return 32768U + (USHORT) atoi (pszCol);

   /*-- strip table specifier if qualified name (code by SuperKludge) --*/
   if ((pszTemp = strrchr (pszCol, '.')) != NULL)
      pszCol = pszTemp + 1;

   /* copy the name to a temp buffer to strip any 'bad' characters */
   CopyDBName (szColumn, pszCol, sizeof szColumn);

   if (0 == ItiStrCmp(szColumn, "CurSpecYear") )
      {
      return (228 + 500);  /* CurSpecYear */  
      }

   if (0 == ItiStrCmp(szColumn, "CurUnitType") )
      {
      return (300 + 500);  /* CurUnitType */
      }

   if ((pidxMatch = bsearch (szColumn, pidxColumns, usColCount,
                             sizeof (IDX), IDXCmpFn)) == NULL)
      return 0xFFFF;
   return pidxMatch->uId;
   }



PSZ EXPORT ItiColColIdToString (USHORT usCol)
   {
   USHORT i;
   PGLOBALS pglbC;

   pglbC = ItiGlobQueryGlobalsPointer ();

   if (usCol == (300 + 500))  /* CurUnitType */
      {
      if (pglbC->bUseMetric)
        return pszMetric;
      else
        return pszEnglish;
      }

   if (usCol == (228 + 500))  /* CurSpecYear */ 
      {
      return pglbC->pszCurrentSpecYear;
      }


   for (i = 0; i < usColCount; i++)
      if (pidxColumns[i].uId == usCol)
         return MAKEP (SELECTOROF (pidxColumns), pidxColumns[i].uOffset);
   return NULL;
   }






BOOL EXPORT ItiColInitialize (void)
   {
   PVOID pBuff;

   if (DosGetResource2 (hmod, ITIRT_COLUMNS, 1, &pBuff))
      return !ColErr ("Unable to load Column Resource");
   usColCount = *((USHORT *) pBuff);
   pidxColumns = (IDX *)((char *)pBuff + sizeof (USHORT));

   if (DosGetResource2 (hmod, ITIRT_TABLES, 1, &pBuff))
      return !ColErr ("Unable to load Table Resource");
   usTableCount = *((USHORT *) pBuff);
   pidxTables = (IDX *)((char *)pBuff + sizeof (USHORT));

   if (DosGetResource2 (hmod, ITIRT_TABLECOLUMN, 1, &pBuff))
      return !ColErr ("Unable to load Table/Column Resource");
   usTabColCount = *((USHORT *) pBuff);
   ptabcol = (TABCOL *)((char *)pBuff + sizeof (USHORT));

   return TRUE;
   }



/**********************************************************************
 * Table functions 
 **********************************************************************/

/*
 * ItiColStringToTableId returns a table ID for a given table name.
 */

USHORT EXPORT ItiColStringToTableId (PSZ pszTable)
   {
   PSZ      pszTemp;
   char     szTable [MAX_TABLE_SIZE];
   IDX      *pidxMatch;

   if (isdigit (*pszTable))
      return 32768U + (USHORT) atoi (pszTable);

   /* strip database or column specifiers if qualified name */
   if ((pszTemp = strrchr (pszTable, '.')) != NULL)
      pszTable = pszTemp + 1;

   /* copy the name to a temp buffer to strip any 'bad' characters */
   CopyDBName (szTable, pszTable, sizeof szTable);

   if ((pidxMatch = bsearch (szTable, pidxTables, usTableCount,
                             sizeof (IDX), IDXCmpFn)) == NULL)
      return 0xFFFF;
   return pidxMatch->uId;
   }



_cdecl TabColCmp (void *p1, void *p2)
   {
   TABCOL *ptabcol1, *ptabcol2;

   ptabcol1 = (TABCOL *) p1;
   ptabcol2 = (TABCOL *) p2;
   if (ptabcol1->uTableId == ptabcol1->uColumnId)
      return ptabcol1->uColumnId - ptabcol2->uColumnId;
   else
      return ptabcol1->uTableId - ptabcol2->uTableId;
   }


static TABCOL *FindTableColumn (USHORT usTable, USHORT usColumn)
   {
   TABCOL tabcol;

   tabcol.uTableId = usTable;
   tabcol.uColumnId = usColumn;
   return bsearch (&tabcol, ptabcol, usTabColCount, sizeof (TABCOL),
                   TabColCmp);
   }

static BOOL GetIds (PSZ       pszTableName, 
                    PUSHORT   pusTable,
                    PSZ       pszColumnName,
                    PUSHORT   pusColumn)
   {
   if (*pusTable == 0)
      if (pszTableName == NULL)
         return FALSE;
      else
         *pusTable = ItiColStringToTableId (pszTableName);
   if (*pusColumn == 0)
      if (pszColumnName == NULL)
         return FALSE;
      else
         *pusColumn = ItiColStringToColId (pszColumnName);

   if (*pusTable == 0)
      return !ColErr ("Invalid Table in ItiColIsColInTable.");
   if (*pusColumn == 0)
      return !ColErr ("Invalid column in ItiColIsColInTable.");
   return TRUE;
   }


/*
 * ItiColIsColInTable returns TRUE if the given column is in the
 * specified table.
 *
 * Parameters: pszTableName      The name of the table.  This
 *                               parameter may be NULL if usTable is
 *                               used.
 *
 */

BOOL EXPORT ItiColIsColInTable (PSZ    pszTableName,
                                USHORT usTable,
                                PSZ    pszColumn,
                                USHORT usColumn)
   {
   TABCOL *ptabcol;

   if (!GetIds (pszTableName, &usTable, pszColumn, &usColumn))
      return FALSE;

   ptabcol = FindTableColumn (usTable, usColumn);
   return ptabcol != NULL;
   }


/*
 * ItiColTableIdToString returns pointer to a string for a given table ID.
 */

PSZ EXPORT ItiColTableIdToString (USHORT usTable)
   {
   USHORT i;

   for (i = 0; i < usTableCount; i++)
      if (pidxTables [i].uId == usTable)
         return MAKEP (SELECTOROF (pidxTables), pidxTables[i].uOffset);
   return NULL;
   }




PSZ EXPORT ItiColGetDefaultValue (PSZ    pszTableName,
                                  USHORT usTable,
                                  PSZ    pszColumn,
                                  USHORT usColumn)
   {
   TABCOL *ptabcol;

   if (!GetIds (pszTableName, &usTable, pszColumn, &usColumn))
      return NULL;

   ptabcol = FindTableColumn (usTable, usColumn);
   if (ptabcol == NULL)
      return NULL;

   return MAKEP (SELECTOROF (ptabcol), ptabcol->uOffset);
   }

