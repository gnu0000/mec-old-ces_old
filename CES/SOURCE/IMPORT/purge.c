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
 * purge.c
 * Mark Hampton
 * Copyright (c) 1991 Info Tech, Inc.
 * This file is part of DS/Shell.
 *
 * This module purges the import database.
 */

#include "..\include\iti.h"
#include "..\include\itiutil.h"
#include "..\include\itidbase.h"
#include "..\include\itiimp.h"
#include "import.h"
#include "purge.h"


/*
 * I know it's bad form to use DISTINCT, but I want to join with the 
 * ImportInfo table, which should only exist in the import database.
 * This protects from deleting the entire production database.
 */

static PSZ pszGetTables = "SELECT DISTINCT TableName "
                          "FROM Tables, ImportInfo "
                          "WHERE ImportInfo.TableKey = Tables.TableKey "
                       //   " AND TableName != 'StandardItemMajorItem' ";
                          ;

/* use DELETE FROM for standard SQL, or TRUNCATE TABLE for SQL server */
//static PSZ pszDeleteTable = "DELETE FROM ";

static PSZ pszDeleteTable = "TRUNCATE TABLE ";


/* this function uses the global memory pszBuffer */

BOOL PurgeTables (HHEAP hheap)
   {
   USHORT   usNumTables;
   USHORT   usArrayPos;
   USHORT   usNumCols, usState;
   PSZ      *ppszColumns;
   USHORT   usTableNamePos;
   char     szTemp [256];
   HQRY hqry;

   hqry = ItiDbExecQuery (pszGetTables, hheap, 0, 0, 0, &usNumCols, &usState);
   usNumTables = 0;
   usArrayPos = 0;

   if (hqry == NULL)
      return FALSE;

   while (ItiDbGetQueryRow (hqry, &ppszColumns, &usState))
      {
      ItiStrCpy (pszBuffer + usArrayPos, ppszColumns [0], 
                 usBufferSize - usArrayPos);
      usArrayPos += ItiStrLen (pszBuffer + usArrayPos) + 1;
      usNumTables++;
      ItiFreeStrArray (hheap, ppszColumns, usNumCols);
      }

   /* delete those dirty rotten scummy tables */
   usTableNamePos = ItiStrLen (pszDeleteTable);
   ItiStrCpy (szTemp, pszDeleteTable, sizeof szTemp);
   usArrayPos = 0;
   while (usNumTables--)
      {
      ItiStrCpy (szTemp + usTableNamePos, pszBuffer + usArrayPos, 
                 sizeof szTemp - usTableNamePos);
      if (ItiDbExecSQLStatement (hheap, szTemp))
         return FALSE;
      usArrayPos += ItiStrLen (pszBuffer + usArrayPos) + 1;
      }


   ItiDbExecSQLStatement (hheap, "TRUNCATE TABLE DSShellImport..CodeValue ");
   ItiDbExecSQLStatement (hheap, "TRUNCATE TABLE DSShellImport..CodeTable ");
   ItiDbExecSQLStatement (hheap, "TRUNCATE TABLE DSShellImport..Vendor ");

   return TRUE;
   }

