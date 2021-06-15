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
 * Mutex.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This module provides mutual exclusion semaphores.
 */

#define INCL_DOSERRORS
#define INCL_DOSSEMAPHORES
#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itierror.h"
#include "semarray.h"
#include "error.h"





/*
 * ItiSemOpenMutex opens an existing named semaphore.
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

USHORT EXPORT ItiSemCreateMutex (PSZ      pszSemName, 
                                 USHORT   usFlags,
                                 PHSEM    phsemMutex)
   {
   USHORT   usError;

   *phsemMutex = NULL;
   usError = 0;

   if (pszSemName == NULL)
      {
      /* create a ram semaphore */
      *phsemMutex = GetNextFreeRamSemaphore ();
      if (*phsemMutex == NULL)
         {
         /* could not allocate any more semaphores */
         usError = ERROR_NOT_ENOUGH_MEMORY;
         }
      }
   else
      {
      /* create a system semaphore */
      usError = DosCreateSem (usFlags, phsemMutex, pszSemName);
      }
   return ConvertBaseError (usError);
   }





/*
 * ItiSemOpenMutex opens an existing named semaphore.
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
 *    ITIBASE_SEM_ERROR_SEM_NOT_FOUND
 *    ITIBASE_SEM_INVALID_NAME
 */

USHORT EXPORT ItiSemOpenMutex (PSZ    pszSemName,
                               PHSEM  phsemMutex)
   {
   if (pszSemName == NULL)
      return ITIBASE_SEM_INVALID_NAME;
   else
      return ConvertBaseError (DosOpenSem (phsemMutex, pszSemName));
   }





/*
 * ItiSemCloseMutex closes an existing named or anonymous semaphore.
 *
 * Parameters: hsemMutex         A handle to an mutex semaphore.
 *
 * Return value:
 * Zero if the semaphore was created, or one of the following in case
 * of an error:
 *    ITIBASE_SEM_IS_SET
 *
 * Comments: 
 * The return value is ITIBASE_SEM_IS_SET if you close a named semaphore, 
 * and the semaphore is still set.  This seems to be informational, 
 * rather that an error.
 */

USHORT EXPORT ItiSemCloseMutex (HSEM hsemMutex)
   {
   if (IsRamSemaphore (hsemMutex))
      {
      /* we have a ram semaphore */
      return ConvertBaseError (FreeRamSemaphore (hsemMutex));
      }
   else
      {
      return ConvertBaseError (DosCloseSem (hsemMutex));
      }
   }





/*
 * ItiSemRequestMutex requests an mutex semaphore.  If the semaphore
 * is already owned, the calling thread will block until either the
 * semaphore is released, or until the call times out.
 *
 * Parameters: hsemMutex         A handle to an mutex semaphore.
 *
 *             lTimeOut          The number of to miliseconds to wait
 *                               before timing out.
 *
 * Return value:
 * Zero if the function was successfull.  Otherwise, it is one of the
 * following error codes:
 *    ITIBASE_SEM_TIMEOUT
 */

BOOL EXPORT ItiSemRequestMutex (HSEM    hsemMutex, 
                                LONG    lTimeOut)
   {
   return ConvertBaseError (DosSemRequest (hsemMutex, lTimeOut));
   }




/*
 * ItiSemReleaseMutex releases an mutex semaphore.
 *
 * Parameters: hsemMutex         A handle to an mutex semaphore.
 *
 * Return value:
 * Zero if the function was successfull.  Otherwise, it is one of the
 * following error codes:
 */

BOOL EXPORT ItiSemReleaseMutex (HSEM hsemMutex)
   {
   return ConvertBaseError (DosSemClear (hsemMutex));
   }

