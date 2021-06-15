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
 * Thread.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This module provides thread functions.
 *
 * Thread management API:
 *    ItiProcCreateThread
 *    ItiProcSuspendThread
 *    ItiProcResumeThread
 *    ItiProcExitThread
 *    ItiProcEnterCritSect
 *    ItiProcExitCritSect
 */

#define INCL_DOSERRORS
#define INCL_DOSPROCESS
#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itierror.h"
#include "error.h"


/*
 * ItiProcCreateThread2 creates a thread.  NOTE: THIS FUNCTION CANNOT BE
 * USED BY A PROCESS THAT USES THE C RUN TIME LIBRARY!
 *
 * Parameters: pfnThreadFunc     A pointer to the thread function.
 *
 *             ptidThread        A pointer to a TID (thread ID).
 *
 *             usStackSize       The number of bytes to allocate for
 *                               the thread's stack.  If this parameter
 *                               is less than THREAD_MIN_STACK_SIZE, then 
 *                               the default stack size of THREAD_STACK_SIZE 
 *                               is used.  
 *
 * Return Value: Zero is returned on success, or one of the following
 * error values:
 *
 *    ERROR_NOT_ENOUGH_MEMORY
 */

USHORT EXPORT ItiProcCreateThread2 (PFNTHREAD pfnThreadFunc, 
                                    PVOID     pvParam,
                                    PTID      ptidThread,
                                    USHORT    usStackSize)
   {
   PCHAR pcStack;
   PVOID *ppvParameter;

   if (usStackSize < THREAD_MIN_STACK_SIZE)
      usStackSize = THREAD_STACK_SIZE;

   pcStack = ItiMemAllocSeg (usStackSize, SEG_NONSHARED, MEM_ZERO_INIT);
   if (pcStack == NULL)
      return ERROR_NOT_ENOUGH_MEMORY;

   pcStack += usStackSize - sizeof (PVOID);
   ppvParameter = (PVOID *) pcStack;
   *ppvParameter = pvParam;

   return ConvertBaseError (DosCreateThread (pfnThreadFunc, ptidThread, 
                                             pcStack));
   }




/*
 * ItiProcCreateThread creates a thread.  NOTE: THIS FUNCTION CANNOT BE
 * USED BY A PROCESS THAT USES THE C RUN TIME LIBRARY!
 *
 * Parameters: pfnThreadFunc     A pointer to the thread function.
 *
 *             ptidThread        A pointer to a TID (thread ID).
 *
 *             usStackSize       The number of bytes to allocate for
 *                               the thread's stack.  If this parameter
 *                               is less than THREAD_MIN_STACK_SIZE, then 
 *                               the default stack size of THREAD_STACK_SIZE 
 *                               is used.  
 *
 * Return Value: Zero is returned on success, or one of the following
 * error values:
 *
 *    ERROR_NOT_ENOUGH_MEMORY
 *    ERROR_NO_PROC_SLOTS
 */

USHORT EXPORT ItiProcCreateThread (PFNTHREAD pfnThreadFunc, 
                                   PTID      ptidThread,
                                   USHORT    usStackSize)
   {
   PCHAR pcStack;

   if (usStackSize < THREAD_MIN_STACK_SIZE)
      usStackSize = THREAD_STACK_SIZE;

   pcStack = ItiMemAllocSeg (usStackSize, SEG_NONSHARED, MEM_ZERO_INIT);
   if (pcStack == NULL)
      return ERROR_NOT_ENOUGH_MEMORY;

   pcStack += usStackSize - sizeof (USHORT);

   return ConvertBaseError (DosCreateThread (pfnThreadFunc, ptidThread, 
                                             pcStack));
   }




/*
 * ItiProcSuspendThread suspends a given thread.
 *
 * Parameters: tidThread         The thread to suspend.
 *
 * Return Value: Zero is returned on success, or one of the following
 * error values:
 *
 *    ERROR_INVALID_THREADID
 */

USHORT EXPORT ItiProcSuspendThread (TID  tidThread)
   {
   return ConvertBaseError (DosSuspendThread (tidThread));
   }





/*
 * ItiProcResumeThread resumes a given thread.
 *
 * Parameters: tidThread         The thread to resume.
 *
 * Return Value: Zero is returned on success, or one of the following
 * error values:
 *
 *    ERROR_INVALID_THREADID
 */

USHORT EXPORT ItiProcResumeThread (TID  tidThread)
   {
   return ConvertBaseError (DosResumeThread (tidThread));
   }





/*
 * ItiProcExitThread exits the current thread.
 *
 * Parameters: None.
 *
 * Return Value: This function never returns.
 */

void EXPORT ItiProcExitThread (void)
   {
   DosExit (EXIT_THREAD, 0);
   }






/*
 * ItiProcEnterCritSect suspends all but the current thread for a
 * given process.
 *
 * Parameters: None.
 *
 * Return Value: Zero is returned on success, or nonzero on failure.
 *
 * Comments: You should never have to call this function.  You should 
 * use semaphores instead.  
 */

USHORT EXPORT ItiProcEnterCritSect (void)
   {
   return ConvertBaseError (DosEnterCritSec ());
   }






/*
 * ItiProcExitCritSect resumes all of the threads previously suspended
 * by ItiProcEnterCritSect.
 *
 * Parameters: None.
 *
 * Return Value: Zero is returned on success, or nonzero on error.
 */

USHORT EXPORT ItiProcExitCritSect (void)
   {
   return ConvertBaseError (DosExitCritSec ());
   }




