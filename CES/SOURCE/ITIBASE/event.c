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
 * Event.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This module provides event semaphores.
 */

#define INCL_DOSSEMAPHORES
#define INCL_DOSERRORS
#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "error.h"
#include "semarray.h"



/*
 * ItiSemCreateEvent creates a mutual exclusion semaphore.  The 
 * semaphore can be named or anonymous.
 *
 * Parameters: pszSemName        A pointer to a null terminiated
 *                               semaphore name.  If this parameter
 *                               is NULL, then an anonymous semaphore
 *                               is created.
 *
 *             usFlags           Semaphore creation flags.  For named
 *                               semaphores, specify CSEM_PRIVATE for
 *                               semaphores provate to your process,
 *                               or CSEM_PUBLIC if the semaphore is to
 *                               be available to all processes.
 *                               This parameter is ignored if pszSemName
 *                               is NULL.
 *
 *             phsem             A pointer to a variable of type HSEM
 *                               to receive the handle to the semaphore.
 *
 * Return Value:
 * Zero if the semaphore was created, or one of the following in case
 * of an error:
 *    ITIBASE_SEM_ALREADY_EXISTS
 *    ITIBASE_SEM_INVALID_NAME
 *    ITIBASE_INVALID_PARAMETER
 */

USHORT EXPORT ItiSemCreateEvent (PSZ     pszSemName, 
                                 USHORT  usFlags,
                                 PHSEM   phsemEvent)
   {
   return ItiSemCreateMutex (pszSemName, usFlags, phsemEvent);
   }


/*
 * ItiSemOpenEvent opens an existing named semaphore.
 *
 * Parameters: pszSemName        A pointer to a null terminiated
 *                               semaphore name.  This parameter cannot
 *                               be NULL.
 *
 *             phsem             A pointer to a variable of type HSEM
 *                               to receive the handle to the semaphore.
 *
 * Return Value:
 * Zero if the semaphore was created, or one of the following in case
 * of an error:
 *    ITIBASE_SEM_NOT_FOUND
 *    ITIBASE_SEM_INVALID_NAME
 */

USHORT EXPORT ItiSemOpenEvent (PSZ    pszSemName,
                               PHSEM  phsemEvent)
   {
   return ItiSemOpenMutex (pszSemName, phsemEvent);
   }


                                      
                                      
/*
 * ItiSemCloseEvent closes an existing named or anonymous semaphore.
 *
 * Parameters: hsemEvent         A handle to an event semaphore.
 *
 * Return value:
 * Zero if the semaphore was created, or one of the following in case
 * of an error:
 *    ITIBASE_SEM_IS_SET
 *
 * Comments: 
 * The return value is ITIBASE_SEM_SET if you close a named semaphore, 
 * and the semaphore is still set.  This seems to be informational, 
 * rather that an error.
 */
                                                                           
USHORT EXPORT ItiSemCloseEvent (HSEM hsemEvent)
   {
   return ItiSemCloseMutex (hsemEvent);
   }






/*
 * ItiSemWaitEvent waits for the specified event semaphore to be cleared.
 *
 * Parameters: hsemEvent      The event to wait for.
 *
 *             lTimeOut       The number of milliseconds to wait before
 *                            timing out.  Use SEM_INDEFINITE_WAIT
 *                            if you want to wait forever.
 * Return Value:
 * Zero if the event semaphore was cleared, or one of the following
 * errors:
 *
 *    ITIBASE_SEM_TIMEOUT
 */

USHORT EXPORT ItiSemWaitEvent (HSEM hsemEvent, LONG lTimeOut)
   {  
   return ConvertBaseError (DosSemWait (hsemEvent, lTimeOut));
   }



/*
 * ItiSemSetWaitEvent sets a event semaphore and then waits for it
 * to be cleared.
 *
 * Parameters: hsemEvent      The event semaphore to use.
 *
 *             lTimeOut       The number of milliseconds to wait before
 *                            timing out.  Use SEM_INDEFINITE_WAIT
 *                            if you want to wait forever.
 * Return Value:
 * Zero if the event semaphore was cleared, or one of the following
 * errors:
 *
 *    ITIBASE_SEM_TIMEOUT
 */

USHORT EXPORT ItiSemSetWaitEvent (HSEM hsemEvent, LONG lTimeOut)
   {  
   return ConvertBaseError (DosSemSetWait (hsemEvent, lTimeOut));
   }



/*
 * ItiSemSetEvent sets a given event semaphore.
 *
 * Parameters: hsemEvent      The event semaphore to set.
 *
 * Return Value:
 * Zero if the event semaphore was cleared, or one of the following
 * errors:
 *
 */

USHORT EXPORT ItiSemSetEvent (HSEM hsemEvent)
   {  
   return ConvertBaseError (DosSemSet (hsemEvent));
   }



/*
 * ItiSemClearEvent clears a given event semaphore.
 *
 * Parameters: hsemEvent      The event semaphore to clear.
 *
 * Return Value:
 * Zero if the event semaphore was cleared, or one of the following
 * errors:
 *
 */

USHORT EXPORT ItiSemClearEvent (HSEM hsemEvent)
   {  
   return ConvertBaseError (DosSemClear (hsemEvent));
   }



