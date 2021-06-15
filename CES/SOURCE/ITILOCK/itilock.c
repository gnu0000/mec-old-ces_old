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
 * ItiLock.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This is the client side of the lock manager.
 */


#include "..\include\iti.h"
#include "..\include\itilock.h"
#include "..\include\itierror.h"
#include "..\include\itiutil.h"
#include "initdll.h"


/*
 * ItiLockRequest requests a lock on a row.
 *
 * Parameters: lots
 *
 * Return value:
 * A handle to a lock is returned, or NULL if the request has been denied.
 *
 */

HLOCK EXPORT ItiLockRequest (PSZ      pszTableName,
                             PSZ      *apszColName,
                             PSZ      *apszColValue,
                             USHORT   usNumCols)
   {
//   char szTemp [4096];
//   int  i;
//
//   ItiStrCpy (szTemp, "ItiLockRequest: Lock requested for table '", sizeof szTemp);
//   ItiStrCat (szTemp, pszTableName, sizeof szTemp);
//   ItiStrCat (szTemp, "'.", sizeof szTemp);
//   ItiErrWriteDebugMessage (szTemp);
//
//   ItiStrCpy (szTemp, "ItiLockRequest: Colums (", sizeof szTemp);
//   for (i=0; i < usNumCols; i++)
//      {
//      ItiStrCat (szTemp, apszColName [i], sizeof szTemp);
//      if (i + 1 < usNumCols)
//         ItiStrCat (szTemp, ",", sizeof szTemp);
//      }
//   ItiStrCat (szTemp, ")", sizeof szTemp);
//   ItiErrWriteDebugMessage (szTemp);
//
//   ItiStrCpy (szTemp, "ItiLockRequest: Values (", sizeof szTemp);
//   for (i=0; i < usNumCols; i++)
//      {
//      ItiStrCat (szTemp, apszColName [i], sizeof szTemp);
//      if (i + 1 < usNumCols)
//         ItiStrCat (szTemp, ",", sizeof szTemp);
//      }
//   ItiStrCat (szTemp, ")", sizeof szTemp);
//   ItiErrWriteDebugMessage (szTemp);
//
   return MAKEP (1,0);
   }



/*
 * ItiLockFree frees a previously locked row.  An update message is sent
 * to all users if bUpdated is TRUE.
 *
 * Parameters: hlock       The lock to free.
 *
 *             bUpdated    TRUE if the row has been changed, FALSE if not.
 *
 * Return value:
 * None.
 */

void EXPORT ItiLockFree (HLOCK hlock, BOOL bUpdated)
   {
   }



/*
 * ItiLockLock marks a lock as impending change.  This will cause the lock
 * manager to not give your lock up to another caller.
 *
 * Parameters: hlock       The lock to lock.
 *
 * Return value:
 * TRUE if the lock was locked, or FALSE if you've already lost your lock.
 */

BOOL EXPORT ItiLockLock (HLOCK hlock)
   {
   return TRUE;
   }




