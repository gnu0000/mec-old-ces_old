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
 * ItiLock.h
 * Mark Hampton
 *
 * This is the lock manager module.
 *
 * General lock functions:
 *    ItiLockRequest
 *    ItiLockFree
 *    ItiLockLock
 */


typedef PVOID HLOCK;



/*
 * ItiLockRequest requests a lock on a row.
 *
 * Parameters: pszTableName         The name of the table for the lock.
 *
 *             apszColName          An array of column names that make 
 *                                  up the unique key for the row to lock.
 *
 *             apszColValue         An array of column values that make 
 *                                  up the unique key for the row to lock.
 *
 *             usNumCols            The number of columns for apszColName 
 *                                  and apszColValue.
 *
 * Return value:
 * A handle to a lock is returned, or NULL if the request has been denied.
 *
 * Comments:
 * If apszColName, and apszColValue are NULL, and usNumCols = 0, then a 
 * lock for the entire table is returned.
 *
 */

extern HLOCK EXPORT ItiLockRequest (PSZ      pszTableName,
                                    PSZ      *apszColName,
                                    PSZ      *apszColValue,
                                    USHORT   usNumCols);



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

extern void EXPORT ItiLockFree (HLOCK hlock, BOOL bUpdated);



/*
 * ItiLockLock marks a lock as impending change.  This will cause the lock
 * manager to not give your lock up to another caller.
 *
 * Parameters: hlock       The lock to lock.
 *
 * Return value:
 * TRUE if the lock was locked, or FALSE if you've already lost your lock.
 */

extern BOOL EXPORT ItiLockLock (HLOCK hlock);


