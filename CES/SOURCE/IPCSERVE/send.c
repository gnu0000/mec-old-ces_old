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
 * Send.c
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
#include "send.h"
#include "ipcserve.h"




#define PBX_NAMED_PIPE  "\\pipe\\iti\\dsshell\\ipc\\server"


/* mutex semaphore for access to the send pipe */
static HSEM hsemSendPipeAccess = NULL;

/* handle to server pipe */
static HPIPE  hpSendPipe = 0;





USHORT SendToServer (PMESSAGE  pmsg, 
                     USHORT    usMessageSize,
                     PREPLY    prply,
                     USHORT    usReplyMax)
   {
   USHORT usError;
   USHORT usBytesRead, usBytesWritten;

   prply->usReply = 0;
   prply->usReplySize = 0;

   if (0 != ItiSemRequestMutex (hsemSendPipeAccess, SEM_INDEFINITE_WAIT))
      {
#if defined (DEBUG)
      ItiErrWriteDebugMessage ("SendToServer: Request mutex failed.\n");
#endif
      return 1;
      }

   usError = DosWrite (hpSendPipe, pmsg, usMessageSize, &usBytesWritten);
   if (usError == 0)
      {
      usError = DosRead (hpSendPipe, prply, usReplyMax, &usBytesRead);
      if (usError == 0)
         {
         prply->usReplySize = usBytesRead - sizeof (REPLY);
         }
      }

   ItiSemReleaseMutex (hsemSendPipeAccess);
   return usError;
   }









BOOL InitServerConnection (PSZ pszServer)
   {
   USHORT   usError;

   /* create the server queue access semaphore */
   if (0 != ItiSemCreateMutex (NULL, SEM_PRIVATE, &hsemSendPipeAccess))
      return FALSE;

   usError = ItiIpcOpenNamedPipe (pszServer, PBX_NAMED_PIPE, 10000, 
                                  &hpSendPipe);
   if (usError)
      {
      /* could not open named pipe.  Time to die. */
#ifdef DEBUG
      char szBuffer [256];

      sprintf (szBuffer, "InitServerConnection: Error opening named pipe: %u (%x)", usError, usError);
      ItiErrWriteDebugMessage (szBuffer);
#endif
      return FALSE;
      }

   return TRUE;
   }

