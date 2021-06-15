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
 * IPCServe.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This module provides the local IPC server.
 */

#define INCL_DOS
#define INCL_DOSERRORS
#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"
#include "..\include\itiipc.h"
#include "init.h"
#include "ipcserve.h"
#include <stdio.h>
#include <process.h>



int _cdecl main (int argc, char *argv[])
   {
   HSEM     hsemStartProcess;
   USHORT   usError;
   HSEM     hsemEndProcess;

   printf ("IPC Local Server Version %u.%.2u\n", 
           VERSION_MAJOR, VERSION_MINOR);
   printf ("Copyright (C) 1991 by Info Tech, Inc.  All rights reserved.\n\n");

   ItiErrWriteDebugMessage ("IPC Local server");

   if (!Init ())
      {
      printf ("Could not initialize!\n");
      return 1;
      }

   /* try to create the IPC local server initialized semaphore */
   usError = ItiSemCreateMutex (PROCESS_SEM_NAME, 
                                CSEM_PUBLIC, 
                                &hsemStartProcess);

   switch (usError)
      {
      case 0:
         /* 0 means A-OK! */
         break;

      case ITIBASE_SEM_ALREADY_EXISTS:
         usError = 0;
         if (argc > 1)
            {
            printf ("Sending commands to server...\n");
            /* the server has already begun.  Send it the command line, and 
               die when done. */
            usError = ItiIpcSendCommand (NULL, 
                                         SESSION_RELATED | SESSION_FOREGROUND,
                                         argc - 1, argv + 1);
            /* potential problem: if two processes are started at the same
               time, the second to execute will not be able to create the 
               semaphore, but may not be able to send command, since the first
               may be in the midst of initialization.  As coded, its tough
               crunchies to the user if this happens. */
            if (usError != 0 && usError != ITIBASE_SESSION_IN_BACKGROUND)
               {
               printf ("Session start error of %u.\n", usError);
               }
            }
         return usError;
         break;

      case ITIBASE_SEM_INVALID_NAME:
         printf ("The IPC server tried to create a semaphore with an invalid name:\n%s\n",
                 PROCESS_SEM_NAME);
         return 1;
         break;

      case ERROR_INVALID_PARAMETER:
         printf ("The IPC server tried to call ItiSemCreateMutex with an invalid parameter:\n%s, %x, %p\n",
                 PROCESS_SEM_NAME, CSEM_PUBLIC, &hsemStartProcess);
         return 1;
         break;

      case ERROR_NOT_ENOUGH_MEMORY:
         printf ("The IPC server tried to create a semaphore, but there are already too many!\n");
         return 1;
         break;

      default:
         printf ("The IPC server tried to create a semaphore, but an unknown error occured:\n%u (%x)\n",
                 usError, usError);
         return 1;
         break;
      }

   /* we have a handle to a semaphore. */
   usError = ItiSemRequestMutex (hsemStartProcess, 0);

   if (usError != 0)
      {
      printf ("Error requesting Mutex semaphore %s: %u (%x)\n", 
               PROCESS_SEM_NAME, usError, usError);
      DosExit (EXIT_PROCESS, 0xfffe);
      } 

   /* time to initialize the IPC server */
   usError = ItiSemCreateEvent (NULL, 0, &hsemEndProcess);
   if (usError)
      {
      printf ("Error creating anonymous event semaphore: %u (%x)\n", 
               PROCESS_SEM_NAME, usError, usError);
      DosExit (EXIT_PROCESS, 0xfffd);
      } 

   ItiSemSetEvent (hsemEndProcess);

   /* process the command line */
   if (usError = ItiIpcSendCommand (NULL, 
                                    SESSION_RELATED | SESSION_FOREGROUND,
                                    argc - 1, argv + 1))
      {
      /* should never happen */
      printf ("For some reason, I could not send a command to myself! %u (%x)\n", usError, usError);
      DosExit (EXIT_PROCESS, 0xfffc);
      }

   /* wait to die */
   ItiSemWaitEvent (hsemEndProcess, SEM_INDEFINITE_WAIT);

   return 0;
   }


