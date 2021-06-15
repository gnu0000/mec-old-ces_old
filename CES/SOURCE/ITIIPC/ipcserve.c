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
#include <stdio.h>
#include <process.h>

#define VERSION_MAJOR   0
#define VERSION_MINOR   05
#define VERSION         005


#define PROCESS_SEM_NAME   "\\sem\\iti\\dsshell\\ipc\\local\\servinit"





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
 *    ERROR_NO_PROC_SLOTS
 */

USHORT EXPORT ItiProcCreateThread2 (PFNTHREAD pfnThreadFunc, 
                                    PVOID     pvParam,
                                    PTID      ptidThread,
                                    USHORT    usStackSize)
   {
   PCHAR pcStack;
   PPVOID ppvParameter;

   if (usStackSize < THREAD_MIN_STACK_SIZE)
      usStackSize = THREAD_STACK_SIZE;

   pcStack = ItiMemAllocSeg (usStackSize, SEG_NONSHARED, MEM_ZERO_INIT);
   if (pcStack == NULL)
      return ERROR_NOT_ENOUGH_MEMORY;

   pcStack += usStackSize - sizeof (PVOID);
   ppvParameter = (PPVOID) pcStack;
   *ppvParameter = pvParam;

   return DosCreateThread (pfnThreadFunc, ptidThread, pcStack);
   }





/* assumes that pcmd is big enough for one extra character */
static USHORT ProcessCommand (PCOMMANDBUF pcmd, USHORT usCommandSize)
   {
   char     szProgramName [MAX_FILE_NAME_SIZE];
   char     szTemp [MAX_FILE_NAME_SIZE];
   USHORT   usError;
   USHORT   usParameterSize;
   PSZ      pszParameters;
   USHORT   i;
   PID      pidProcess;
   SID      sidSession;
   BPSZ     bpszTemp;

   pszParameters = NULL;
   if (usCommandSize < sizeof (COMMANDBUF) || pcmd->usNumArgs == 0)
      return 0;

   if (pcmd->usVersion != VERSION)
      return ~0;

   /* build program name */
   i = ItiStrLen (pcmd->ppszArgs [0]);
   ItiStrCpy (szTemp, pcmd->ppszArgs [0], sizeof szTemp);

   /* this is required to get rid of C6.00 compiler bug */
   bpszTemp = pcmd->ppszArgs [0];
   if (ItiStrICmp (bpszTemp + i - 4, ".exe") != 0)
      {
      ItiStrCat (szTemp, ".exe", sizeof szTemp);
      }
   usError = DosSearchPath (DSP_ENVIRONMENT | DSP_CUR_DIRECTORY, "PATH", 
                            szTemp, szProgramName, sizeof szProgramName);

   if (usError)
      return usError;

   /* build program parameters */
   usParameterSize = 0;
   for (i=1; i < pcmd->usNumArgs; i++)
      {
      usParameterSize += ItiStrLen (pcmd->ppszArgs [i]) + 1;
      }

   if (usParameterSize > 0)
      {
      /* allocate memory for parameters */
      pszParameters = ItiMemAllocSeg (usParameterSize, SEG_NONSHARED, 0);
      if (pszParameters == NULL)
         return ERROR_NOT_ENOUGH_MEMORY;

      *pszParameters = '\0';
      for (i=1; i < pcmd->usNumArgs; i++)
         {
         ItiStrCat (pszParameters, pcmd->ppszArgs [i], usParameterSize);
         ItiStrCat (pszParameters, " ", usParameterSize);
         }
      }
   if (pszParameters == NULL)
      {
      usError = ItiProcCreateSession ("New Session", szProgramName, 
                                      "", pcmd->ulSessionFlags, 
                                      &pidProcess, &sidSession);
      }
   else
      {
      usError = ItiProcCreateSession ("New Session", szProgramName, 
                                      pszParameters, pcmd->ulSessionFlags, 
                                      &pidProcess, &sidSession);
      }

#ifdef DEBUG
   {
   char szBuffer [1024];
   sprintf (szBuffer, "ProcessCommand: Error %u. Program: %s. Parameters follow:",
            usError, szProgramName);
   ItiErrWriteDebugMessage (szBuffer);
   if (pszParameters == NULL)
      ItiErrWriteDebugMessage ("(null)");
   else
      ItiErrWriteDebugMessage (pszParameters);
   }
#endif
   if (pszParameters != NULL)
      {
      ItiMemFreeSeg (pszParameters);
      }

   if (usError == ERROR_SMG_START_IN_BACKGROUND)
      {
      usError = ItiProcSelectSession (sidSession);
      }

   return usError;
   }




void THREAD _cdecl ProcessCommands (HSEM hsemThreadInited)
   {
   USHORT      usError;
   HPIPE       hpPipe;
   PCOMMANDBUF pcmd;
   USHORT      usBytesRead;
   USHORT      usReturnValue;

   usError = ItiIpcCreateNamedPipe (SERVER_COMMAND_PIPE, &hpPipe, 1,
                                    MAX_COMMAND_SIZE, sizeof (USHORT));
   if (usError)
      {
      /* could not create named pipe.  Time to die. */
#ifdef DEBUG
      char szBuffer [256];

      sprintf (szBuffer, "Command: Error creating named pipe: %u (%x)", usError, usError);
      ItiErrWriteDebugMessage (szBuffer);
      *((PUSHORT) (0)) = 0;
#endif
      ItiSemClearEvent (hsemThreadInited);
      ItiProcExitThread ();
      }

   pcmd = ItiMemAllocSeg (MAX_COMMAND_SIZE, SEG_NONSHARED, MEM_ZERO_INIT);
   if (pcmd == NULL)
      {
      /* could allocate memory.  Time to die. */
#ifdef DEBUG
      char szBuffer [256];

      sprintf (szBuffer, "Command: Error allocating %u bytes.", MAX_COMMAND_SIZE);
      ItiErrWriteDebugMessage (szBuffer);
      *((PUSHORT) (0)) = 0;
#endif
      ItiSemClearEvent (hsemThreadInited);
      ItiProcExitThread ();
      }

   ItiSemClearEvent (hsemThreadInited);
   while (1)
      {
      usError = ItiIpcConnectNamedPipe (hpPipe);
      if (usError != 0)
         {
         /* could not connect to named pipe. */
#ifdef DEBUG
         char szBuffer [256];

         sprintf (szBuffer, "Command: Error connecting to named pipe: %u (%x)", usError, usError);
         ItiErrWriteDebugMessage (szBuffer);
#endif
         /* is this really cheating? */
         DosSleep (0);
         }
      else
         {
         /* connected to client.  Read message */
         do
            {
            usError = DosRead (hpPipe, pcmd, MAX_COMMAND_SIZE, &usBytesRead);
            
            if (usError != 0 && usError != ERROR_BROKEN_PIPE)
               {
#ifdef DEBUG
               char szBuffer [256];
   
               sprintf (szBuffer, "Command: Error reading from named pipe: %u (%x)", usError, usError);
               ItiErrWriteDebugMessage (szBuffer);
#endif
               }
            else if (usError != ERROR_BROKEN_PIPE && usBytesRead != 0)
               {
               /* process command */
               usReturnValue = ProcessCommand (pcmd, usBytesRead);
               usError = DosWrite (hpPipe, &usReturnValue, sizeof usReturnValue,
                                   &usBytesRead);
   
               if (usError)
                  {
#ifdef DEBUG
                  char szBuffer [256];
      
                  sprintf (szBuffer, "Command: Error writing to named pipe: %u (%x)", usError, usError);
                  ItiErrWriteDebugMessage (szBuffer);
#endif
                  }
               }
            } while (usError == 0 && usBytesRead != 0);
         }
      ItiIpcDisconnectNamedPipe (hpPipe);
      } /* while (1) */
   }



#define MAX_INIT_WAIT   1000UL

BOOL InitLocalIPCServer (HSEM hsemEndProcess)
   {
   USHORT   usError;
   TID      tidThread;
   HSEM     hsemThreadInited;

   usError = ItiSemCreateEvent (NULL, 0, &hsemThreadInited);
   if (usError != 0)
      {
      printf ("Could not create thread initialized semaphore.Error = %u (%x)\n", 
               usError, usError);
      return FALSE;
      }

   /* create the command thread */
   ItiSemSetEvent (hsemThreadInited);

#ifdef DEBUG
   tidThread = _beginthread (ProcessCommands, NULL, 10240, hsemThreadInited);
   if (tidThread == -1)
      usError = 1;
#else
   usError = ItiProcCreateThread2 (ProcessCommands, (PVOID) hsemThreadInited,
                                   &tidThread, 10240);
#endif
   if (usError != 0)
      {
      ItiSemCloseEvent (hsemThreadInited);
      }

   ItiSemWaitEvent (hsemThreadInited, MAX_INIT_WAIT); 

   /* create the local PBX threads */

   ItiSemCloseEvent (hsemThreadInited);

//   /* prepare to die! */
//   ItiSemClearEvent (hsemEndProcess);
   return TRUE;
   }


int _cdecl main (int argc, char *argv[])
   {
   HSEM     hsemStartProcess;
   USHORT   usError;
   HSEM     hsemEndProcess;

   fprintf (stderr, "IPC Local Server Version %u.%.2u\n", 
            VERSION_MAJOR, VERSION_MINOR);
   fprintf (stderr, "Copyright (C) 1991 by Info Tech, Inc.  All rights reserved.\n\n");

   if (argc <= 1)
      {
      /* not enough parameters */
      fprintf (stderr, "You should not run this program if you don't know what it does!\n"
                       "For all you know, it could format your hard disk (don't worry - it \n"
                       "should hardly ever do that).\n");
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

      case ERROR_ALREADY_EXISTS:
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
         if (usError != 0 && usError != ERROR_SMG_START_IN_BACKGROUND)
            {
            printf ("Session start error of %u.\n", usError);
            }
         return usError;
         break;

      case ERROR_INVALID_NAME:
         printf ("The IPC server tried to create a semaphore with an invalid name:\n%s\n",
                 PROCESS_SEM_NAME);
         return 1;
         break;

      case ERROR_INVALID_PARAMETER:
         printf ("The IPC server tried to call ItiSemCreateMutex with an invalid parameter:\n%s, %x, %p\n",
                 PROCESS_SEM_NAME, CSEM_PUBLIC, &hsemStartProcess);
         return 1;
         break;

      case ERROR_TOO_MANY_SEMAPHORES:
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

   if (!InitLocalIPCServer (hsemEndProcess))
      {
      /* the local IPC Server did not initialize */
      printf ("For some reason, the local IPC server did not initialize...\n");
      DosExit (EXIT_PROCESS, 0xfffc);
      }

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

