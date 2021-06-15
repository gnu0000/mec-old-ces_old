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


/************************************************************************
 *                                                                      *
 * update.c                                                             *
 *                                                                      *
 * Copyright (C) 1990-1992 Info Tech, Inc.                              *
 * Mark Hampton                                                         *
 *                                                                      *
 * This module handles updates to the database magically.               *
 *                                                                      *
 * This file is part of the Database system of DS/SHELL. This is a      *
 * proprietary product of Info Tech. and no part of which may be        *
 * reproduced or transmitted in any form or by any means, including     *
 * photocopying and recording or in connection with any information     *
 * storage or retrieval system, without permission in writing           *
 * from Info Tech, Inc.                                                 *
 *                                                                      *
 *                                                                      *
 ************************************************************************/

#define DATABASE_INTERNAL
#include "..\include\iti.h"
#define DBMSOS2
#include <sqlfront.h>
#include <sqldb.h>
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"
#include <limits.h>
#include <stdio.h>
#include "dbkernel.h"
#include "update.h"





BOOL EXPORT ItiDbBeginTransaction (HHEAP hheap)
   {
//   return 0 == ItiDbExecSQLStatement (hheap, "BEGIN TRANSACTION");
   return TRUE;
   }


BOOL EXPORT ItiDbCommitTransaction (HHEAP hheap)
   {
//   return 0 == ItiDbExecSQLStatement (hheap, "COMMIT TRANSACTION");
   return TRUE;
   }


BOOL EXPORT ItiDbRollbackTransaction (HHEAP hheap)
   {
//   return 0 == ItiDbExecSQLStatement (hheap, "ROLLBACK TRANSACTION");
   return TRUE;
   }



BOOL EXPORT ItiDbUpdateDeletedBuffers (PSZ pszDelete)
   {
   PSZ      pszWord;
   BOOL     bFound;
   PSZ      pszTemp;
   PSZ      pszTemp2;
   USHORT   usSize;

   /* make a copy of the insert string, since ItiStrTok clobbers the
      input string */
   usSize = ItiStrLen (pszDelete) + 1;
   pszTemp = ItiMemAllocSeg (usSize, 0, 0);
   if (pszTemp == NULL)
      return FALSE;
   ItiStrCpy (pszTemp, pszDelete, usSize);

   bFound = FALSE;
   pszWord = ItiStrTok (pszTemp, " \t\n\r(");
   while (pszWord != NULL)
      {
      if (0 == ItiStrICmp (pszWord, "delete"))
         {
         pszWord = ItiStrTok (NULL, " \t\n\r(");
         if (0 == ItiStrICmp (pszWord, "from"))
            pszWord = ItiStrTok (NULL, " \t\n\r(");

         /* strip off server, database, and owner names */
         if ((pszTemp2 = ItiStrRChr (pszWord, '.')) != NULL)
            pszWord = pszTemp2 + 1;
   
         /* update the buffers */
         ItiDbUpdateBuffers (pszWord);
         }
      else if (0 == ItiStrICmp (pszWord, "update"))
         {
         pszWord = ItiStrTok (NULL, " \t\n\r(");

         /* strip off server, database, and owner names */
         if ((pszTemp2 = ItiStrRChr (pszWord, '.')) != NULL)
            pszWord = pszTemp2 + 1;
   
         /* update the buffers */
         ItiDbUpdateBuffers (pszWord);
         }
      else
         {
         pszWord = ItiStrTok (NULL, " \t\n\r(");
         }
      }

   ItiMemFreeSeg (pszTemp);
   }


static PSZ pszGetKeyLock = NULL;
static PSZ pszGetKeyValue = NULL;
static PSZ pszGetKeyCommit = NULL;


/*
 * ItiDbGetKey returns a key for the specified table.  
 *
 * WARNING: This uses a direct, non blocking connection to the server.
 *
 * Returns NULL on error, or a pointer to a string containing the 
 * key value.  The string was allocated from hheap, and the caller is
 * responsible for freeing it.
 */

PSZ EXPORT ItiDbGetKey (HHEAP   hheap, 
                        PSZ     pszTableName,
                        ULONG   ulNumKeys)
   {
   USHORT   usColumns, usState;
   HQRY     hqry;
   PSZ      *ppszData;
   PSZ      pszKey;
   char     szQuery [1024];
   ULONG    ulKey;

   if (ulNumKeys == 0)
      ulNumKeys = 1;

   /* begin the transaction */
   ItiDbBeginTransaction (hheap);

   /* lock the row */
   sprintf (szQuery, pszGetKeyLock, pszTableName);
   ItiDbExecSQLStatement (hheap, szQuery);

   /* get the value */
   sprintf (szQuery, pszGetKeyValue, pszTableName);
   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usColumns, &usState);
   if (hqry == NULL) 
      return NULL;

   for (pszKey = NULL; ItiDbGetQueryRow (hqry, &ppszData, &usState);)
      {
      if (pszKey != NULL)
         ItiMemFree (hheap, pszKey);
      pszKey = ppszData [0];
      ItiMemFree (hheap, ppszData);
      }

   if (pszKey == NULL ||
       !ItiStrToULONG (pszKey, &ulKey) || 
       ulKey > ULONG_MAX - ulNumKeys)
      {
      ItiDbRollbackTransaction (hheap);
      if (pszKey)
         ItiMemFree (hheap, pszKey);
      return NULL;
      }

   sprintf (szQuery, pszGetKeyCommit, ulNumKeys, pszTableName);

   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usColumns, &usState);
   if (hqry == NULL) 
      return NULL;

   while (ItiDbGetQueryRow (hqry, &ppszData, &usState))
      ItiFreeStrArray (hheap, ppszData, usColumns);

   /* commit the transaction */
   ItiDbCommitTransaction (hheap);

   return pszKey;
   }





static PSZ *apszGetDbVal;


PSZ EXPORT ItiDbGetDbValue (HHEAP hheap, USHORT usDbValType)
   {
   if (usDbValType < ITIDB_MIN_DB_QUERY || usDbValType > ITIDB_MAX_DB_QUERY)
      return NULL;

   return ItiDbGetRow1Col1 (apszGetDbVal [usDbValType - 1], hheap, 0, 0, 0);
   }





static char szGetKeyLock [] = 
   "UPDATE %s..KeyMaster "
   "SET KeyValue = KeyValue "
   "WHERE TableName='%%s' ";

static char szGetKeyValue [] =
   "SELECT KeyValue "
   "FROM %s..KeyMaster "
   "WHERE TableName='%%s' ";

static char szGetKeyCommit [] =
   "UPDATE %s..KeyMaster "
   "SET KeyValue = KeyValue + %%lu "
   "WHERE TableName='%%s' ";

static char szGetMaxSpecYear [] = 
   "SELECT MAX (SpecYear) "
   "FROM %s..StandardItem ";

static char szGetMaxBaseDate [] =
   "SELECT \"'\"+CONVERT(varchar,MAX (BaseDate),109)+\"'\" "
   "FROM %s..BaseDate ";



BOOL InitUpdate (PSZ pszDatabaseName)
   {
   USHORT usPos;

   /* set the database name in the key master queries */
   pszGetKeyLock   = ItiMemAllocSeg (ItiStrLen (szGetKeyLock)     + 
                                     ItiStrLen (szGetKeyValue)    + 
                                     ItiStrLen (szGetKeyCommit)   +
                                     ItiStrLen (szGetMaxSpecYear) +
                                     ItiStrLen (szGetMaxBaseDate) +
                                     2 * sizeof (PSZ)             +
                                     5 * ItiStrLen (pszDatabaseName), 
                                     SEG_NONSHARED, MEM_ZERO_INIT);
   if (pszGetKeyLock == NULL)
      return FALSE;

   usPos = 1 + sprintf (pszGetKeyLock, szGetKeyLock, pszDatabaseName);

   pszGetKeyValue = pszGetKeyLock + usPos;
   usPos += 1 + sprintf (pszGetKeyValue,  szGetKeyValue, pszDatabaseName);

   pszGetKeyCommit = pszGetKeyLock + usPos;
   usPos += 1 + sprintf (pszGetKeyCommit, szGetKeyCommit, pszDatabaseName);

   apszGetDbVal = (PSZ *) (pszGetKeyLock + usPos);
   usPos += sizeof (PSZ) * 2;

   apszGetDbVal [0] = pszGetKeyLock + usPos;
   usPos += 1 + sprintf (apszGetDbVal [0], szGetMaxSpecYear, pszDatabaseName);

   apszGetDbVal [1] = pszGetKeyLock + usPos;
   usPos += 1 + sprintf (apszGetDbVal [1], szGetMaxBaseDate, pszDatabaseName);

   return TRUE;
   }

