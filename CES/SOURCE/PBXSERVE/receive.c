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
 * Receive.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 */

#define INCL_DOS
#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"
#include "..\include\itiipc.h"
#include <process.h>
#include <stdio.h>
#include "receive.h"
#include "pbxserve.h"




#define PBX_NAMED_PIPE     "\\pipe\\iti\\dsshell\\ipc\\server"
#define MAX_CONNECTIONS    30


static BOOL bInitialized = FALSE;

void THREAD _cdecl PBXReader (HSEM hsemInitialized)
   {
   USHORT usError;
   HFILE  hpPipe;
   char   szTemp [8192];
   USHORT usBytesWritten;
   USHORT usBytesRead;

   usError = ItiIpcCreateNamedPipe (PBX_NAMED_PIPE, &hpPipe, 
                                    MAX_CONNECTIONS,
                                    MAX_COMMAND_SIZE, MAX_COMMAND_SIZE);
   if (usError)
      {
      /* could not create named pipe.  Time to die. */
#ifdef DEBUG
      char szBuffer [256];

      sprintf (szBuffer, "Command: Error creating named pipe: %u (%x)", usError, usError);
      ItiErrWriteDebugMessage (szBuffer);
#endif
      ItiSemClearEvent (hsemInitialized);
      _endthread ();
      }

   bInitialized = TRUE;
   ItiSemClearEvent (hsemInitialized);
   while (1)
      {
      usError = ItiIpcConnectNamedPipe (hpPipe);
      if (usError != 0)
         {
         /* could not connect to named pipe. */
#ifdef DEBUG
         char szBuffer [256];

         sprintf (szBuffer, "LocalReader: Error connecting to named pipe: %u (%x)", usError, usError);
         ItiErrWriteDebugMessage (szBuffer);
#endif
         switch (usError)
            {
            case ITIIPC_BAD_PIPE:
               /* the handle to the pipe is invalid -- time to die */
#ifdef DEBUG
               ItiErrWriteDebugMessage ("LocalReader: The impossible happened: bad pipe error!");
#endif
               _endthread ();
               break;
            }

         /* is this really cheating? */
         DosSleep (0);
         }
      else
         {
         /* connected to client.  Read message */
         ItiErrWriteDebugMessage ("Connected to pipe.");
         do
            {
            usError = DosRead (hpPipe, szTemp, sizeof szTemp, &usBytesRead);

            if (usError != 0 && usError != ITIIPC_BROKEN_PIPE)
               {
               char szBuffer [256];
   
               sprintf (szBuffer, "Error reading from named pipe: %u (%x)", usError, usError);
               ItiErrWriteDebugMessage (szBuffer);
               }
            else if (usError != ITIIPC_BROKEN_PIPE && usBytesRead != 0)
               {
               /* process message */
               ItiErrWriteDebugMessage ("Read from pipe.");
               ItiErrWriteDebugMessage (szTemp);
               usError = 0;
               usError = DosWrite (hpPipe, &usError, sizeof usError,
                                   &usBytesWritten);

               if (usError)
                  {
                  char szBuffer [256];

                  sprintf (szBuffer, "Error writing to named pipe: %u (%x)", usError, usError);
                  ItiErrWriteDebugMessage (szBuffer);
                  }
               }
            } while (usError == 0 && usBytesRead != 0);
         }
      ItiIpcDisconnectNamedPipe (hpPipe);
      } /* while (1) */
   }




BOOL InitReceivers (void)
   {
   USHORT   usError;
   USHORT   i;
   TID      tid;
   HSEM     hsemInitialized;

   usError = ItiSemCreateEvent (NULL, 0, &hsemInitialized);
   if (usError)
      return FALSE;

   bInitialized = TRUE;
   for (i=0; i < MAX_CONNECTIONS && bInitialized; i++)
      {
      bInitialized = FALSE;
      ItiSemSetEvent (hsemInitialized);
      tid = _beginthread (PBXReader, NULL, 16384, hsemInitialized);
      if (tid != -1)
         ItiSemWaitEvent (hsemInitialized, SEM_INDEFINITE_WAIT);
      }

   ItiSemCloseEvent (hsemInitialized);
   return bInitialized;
   }

