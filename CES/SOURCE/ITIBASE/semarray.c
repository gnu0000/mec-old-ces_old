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
 * Semarray.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This moudule handles dynamic allocation of the ram semaphore array, used
 * by the Event and Mutex semaphore modules.
 *
 * This module dynamically allocates RAM semaphores as needed.  The
 * management is done by allocating an array of ULONGS.  The first
 * element (element 0) is used as a index in the array to the first 
 * free RAM semaphore.  Each succeding free RAM semaphore contains
 * an index to the next free RAM semaphore.  The end of this free
 * list is denoted by an index of 0. 
 * 
 */ 

#define INCL_DOSERRORS
#define INCL_DOSSEMAPHORES
#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "semarray.h"
#include "error.h"



/*
 * STARTING_RAM_SEMAPHORES is the number of RAM semaphores that should
 *  be allocated when the module is initialized 
 */

#define STARTING_RAM_SEMAPHORES     1024




/*
 * SEMAPHORE_INCREASE_SIZE is the number of semaphores to try and
 *  allocate when the semaphore array needs to be increased. 
 */

#define SEMAPHORE_INCREASE_SIZE     512





/*
 * MAX_NUM_SEMAPHORES is the maximum number of semaphores that will
 * fit into a segment of MAX_SEG_SIZE 
 */
 
#define MAX_NUM_SEMAPHORES          (MAX_SEG_SIZE / sizeof (*phsemRamSem))





/*
 * ulSemArrayAccess is used as an mutex semaphore to restrict access to
 * the RAM semaphore array. 
 */

static ULONG ulSemArrayAccess = 0;




/* 
 * phsemRamSem is a pointer to the RAM semaphore array.  See the module
 * description for an overview of how this array is managed.
 */

static PULONG phsemRamSem;




/* 
 * usNumSems keeps track of the current number of elements in the RAM
 * semaphore array. 
 */
 
static USHORT  usNumSems;





/*
 * GetNextFreeRamSemaphore returns a handle to the next available
 * RAM semaphore.
 *
 * Parameters: None.
 *
 * Return value:
 * A handle to a RAM semaphore, or NULL if no more RAM semaphores can
 * be allocated.
 */


HSEM GetNextFreeRamSemaphore (void)
   {
   USHORT usIndex;

   if (DosSemRequest (&ulSemArrayAccess, SEM_INDEFINITE_WAIT))
      /* should never happen ? */
      return NULL;

   if (phsemRamSem [0] != 0)
      {
      usIndex = (USHORT) phsemRamSem [0];
      phsemRamSem [0] = (USHORT) phsemRamSem [usIndex];
      phsemRamSem [usIndex] = 0;
      }
   else
      {
      USHORT   usNewNumSems;   
      USHORT   i;
      PULONG   pulNew;

      /* must reallocate the semaphore array */
      if (usNumSems >= MAX_NUM_SEMAPHORES)
         {
         DosSemClear (&ulSemArrayAccess);
         return NULL;
         }

      usNewNumSems = usNumSems + SEMAPHORE_INCREASE_SIZE;
      if (usNewNumSems >= MAX_NUM_SEMAPHORES)
         {
         usNewNumSems = MAX_NUM_SEMAPHORES;
         }

      pulNew = ItiMemReallocSeg (phsemRamSem, 
                                 usNewNumSems * sizeof (*phsemRamSem), 0);
      if (pulNew == NULL)
         {
         DosSemClear (&ulSemArrayAccess);
         return NULL;
         }

      /* get the first new semaphore */
      usIndex = usNumSems;
      phsemRamSem [usIndex] = 0;

      /* initialize the new memory */
      phsemRamSem [0] = usNumSems + 1;
      for (i=usNumSems+1; i < usNewNumSems - 1; i++)
         {
         phsemRamSem [i] = i + 1;
         }
      phsemRamSem [usNewNumSems - 1] = 0;

      usNumSems = usNewNumSems;
      }
   DosSemClear (&ulSemArrayAccess);

   if (usIndex == 0)
      return NULL;
   else 
      return phsemRamSem + usIndex;
   }



/*
 * FreeRamSemaphore frees the specified ram semaphore from the semaphore
 * array.
 *
 * Parameters: hsem           A handle to a semaphore to free.
 *
 * Return value: 
 * Zero if no error occured, or ERROR_TOO_MANY_SEM_REQUESTS if an internal
 * semaphore is not received.
 */ 
   
USHORT FreeRamSemaphore (HSEM hsem)
   {
   USHORT usIndex;

   usIndex = OFFSETOF (hsem) / sizeof (*phsemRamSem);

   if (DosSemRequest (&ulSemArrayAccess, SEM_INDEFINITE_WAIT))
      /* should never happen ? */
      return ERROR_TOO_MANY_SEM_REQUESTS;

   phsemRamSem [usIndex] = phsemRamSem [0];
   phsemRamSem [0] = usIndex;

   DosSemClear (&ulSemArrayAccess);

   return 0;
   }






/*
 * IsRamSemaphore returns TRUE is the given semaphore is a RAM semapbore,
 * or FALSE if not.
 */

BOOL IsRamSemaphore (HSEM hsem)
   {
   return SELECTOROF (hsem) == SELECTOROF (phsemRamSem);
   }



BOOL InitSemArrayModule (HMODULE hmodDll)
   {
   USHORT   i;

   phsemRamSem = ItiMemAllocSeg (STARTING_RAM_SEMAPHORES * sizeof (ULONG), 
                                 SEG_NONSHARED, 0);
   if (phsemRamSem == NULL)
      return FALSE;

   usNumSems = STARTING_RAM_SEMAPHORES;
   
   /* initialize the free list for the semaphores */
   for (i = 0; i < usNumSems - 1; i++)
      {
      phsemRamSem [i] = i + 1;
      }
   
   /* fix the last element to be an index of 0 */
   phsemRamSem [usNumSems - 1] = 0;

   return TRUE;
   }



