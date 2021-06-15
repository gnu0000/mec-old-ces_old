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
 * Local.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This module processes local connections and messages.
 */

#define INCL_DOS
#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"
#include "..\include\itiipc.h"
#include <process.h>
#include <stdio.h>
#include "local.h"
#include "ipcserve.h"
#include "send.h"

#define MAX_MESSAGE_SIZE 16378
#define MAX_REPLY_SIZE   16378

static BOOL bInitialized = FALSE;

static void DispatchMessage (PMESSAGE  pmsg, 
                             USHORT    usMessageSize,
                             PREPLY    prply,
                             USHORT    usReplyMax);


void THREAD _cdecl LocalReader (HSEM hsemThreadInited)
   {
   USHORT      usError;
   HPIPE       hpPipe;
   USHORT      usBytesRead;
   USHORT      usBytesWritten;
   PREPLY      prply;
   PMESSAGE    pmsg;

   usError = ItiIpcCreateNamedPipe (LOCAL_READER_PIPE, &hpPipe, 
                                    MAX_LOCAL_CONNECTIONS,
                                    MAX_MESSAGE_SIZE, MAX_REPLY_SIZE);
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

   pmsg = ItiMemAllocSeg (MAX_MESSAGE_SIZE, SEG_NONSHARED, MEM_ZERO_INIT);
   prply = ItiMemAllocSeg (MAX_REPLY_SIZE, SEG_NONSHARED, MEM_ZERO_INIT);
   if (pmsg == NULL || prply == NULL)
      {
      /* could allocate memory.  Time to die. */
#ifdef DEBUG
      ItiErrWriteDebugMessage ("LocalReader: Error allocating memory for pipe buffers.");
#endif
      ItiSemClearEvent (hsemThreadInited);
      _endthread ();
      }

   bInitialized = TRUE;
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
         do
            {
            usError = DosRead (hpPipe, pmsg, MAX_MESSAGE_SIZE, &usBytesRead);
            
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
               /* process message */
               DispatchMessage (pmsg, usBytesRead, prply, MAX_REPLY_SIZE);
               usError = DosWrite (hpPipe, prply, 
                                   prply->usReplySize + sizeof (REPLY),
                                   &usBytesWritten);

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



static void DispatchMessage (PMESSAGE  pmsg, 
                             USHORT    usMessageSize,
                             PREPLY    prply,
                             USHORT    usReplyMax)
   {
//   if (pmsg->addrReceiver.usMachine == usThisMachine)
//      {
//      /* we shouldn't clutter the network, but for now just send it to 
//         the server, so it can send it back to me. */
//      SendToServer (pmsg, usMessageSize, prply, usReplyMax);
//      }
//   else 
      {
      /* send the message to the server */
      SendToServer (pmsg, usMessageSize, prply, usReplyMax);
      }
   }

#define LOCAL_READER_STACKSIZE   16384

BOOL InitLocalConnections (void)
   {
   HSEM     hsemInitialized;
   USHORT   usError;
   TID      tid;
   USHORT   i;

   usError = ItiSemCreateEvent (NULL, 0, &hsemInitialized);
   if (usError)
      return FALSE;

   bInitialized = TRUE;
   for (i=0; i < MAX_LOCAL_CONNECTIONS && bInitialized; i++)
      {
      bInitialized = FALSE;
      ItiSemSetEvent (hsemInitialized);
      tid = _beginthread (LocalReader, NULL, LOCAL_READER_STACKSIZE, 
                          hsemInitialized); 
      if (tid != -1)
         ItiSemWaitEvent (hsemInitialized, SEM_INDEFINITE_WAIT);
      }

   ItiSemCloseEvent (hsemInitialized);

   return bInitialized;
   }   



