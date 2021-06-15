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
 * Command.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This module processes command lines sent to the local IPC server.
 */

#define INCL_DOS
#define INCL_DOSERRORS
#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"
#include "..\include\itiipc.h"
#include <process.h>
#include <stdio.h>
#include "command.h"
#include "ipcserve.h"

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
   PSZ      pszTemp;

   pszParameters = NULL;
   if (usCommandSize < sizeof (COMMANDBUF) || pcmd->usNumArgs == 0)
      return 0;

   if (pcmd->usVersion != VERSION)
      return ~0;

   /* build program name */
   i = ItiStrLen (pcmd->ppszArgs [0]);
   ItiStrCpy (szTemp, pcmd->ppszArgs [0], sizeof szTemp);

   /* this is required to get rid of C6.00 compiler bug */
   pszTemp = pcmd->ppszArgs [0];
   if (ItiStrICmp (pszTemp + i - 4, ".exe") != 0)
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
   ItiProcSelectSession (0);
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

   if (usError == ITIBASE_SESSION_IN_BACKGROUND ||
       usError == ITIBASE_SESSION_NOT_FOREGROUND)
      {
      usError = ItiProcSelectSession (0);
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
#endif
      ItiSemClearEvent (hsemThreadInited);
      _endthread ();
      }

   pcmd = ItiMemAllocSeg (MAX_COMMAND_SIZE, SEG_NONSHARED, MEM_ZERO_INIT);
   if (pcmd == NULL)
      {
      /* could allocate memory.  Time to die. */
#ifdef DEBUG
      char szBuffer [256];

      sprintf (szBuffer, "Command: Error allocating %u bytes.", MAX_COMMAND_SIZE);
      ItiErrWriteDebugMessage (szBuffer);
#endif
      ItiSemClearEvent (hsemThreadInited);
      _endthread ();
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
            
            if (usError != 0 && usError != ITIIPC_BROKEN_PIPE)
               {
#ifdef DEBUG
               char szBuffer [256];
   
               sprintf (szBuffer, "Command: Error reading from named pipe: %u (%x)", usError, usError);
               ItiErrWriteDebugMessage (szBuffer);
#endif
               }
            else if (usError != ITIIPC_BROKEN_PIPE && usBytesRead != 0)
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

