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



#define INCL_DOS
#define INCL_DOSERRORS
#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itiutil.h"
#include "..\include\itiipc.h"
#include "..\include\itierror.h"

#define VERSION_MAJOR   0
#define VERSION_MINOR   05
#define VERSION         005


/* this function converts OS/2 errors into ITIIPC errors */

static USHORT ItiIpcConvertError (USHORT usError)
   {
   switch (usError)
      {
      case 0:
      case ERROR_NOT_ENOUGH_MEMORY:
      case ERROR_INVALID_FUNCTION:
      case ERROR_INVALID_PARAMETER:
         return usError;
         break;

      case ERROR_PIPE_BUSY:
         return ITIIPC_PIPE_BUSY;
         break;

      case ERROR_BAD_PIPE:
         return ITIIPC_BAD_PIPE;
         break;

      case ERROR_SEM_TIMEOUT:
         return ITIIPC_TIMEOUT;
         break;

      case ERROR_PATH_NOT_FOUND:
         return ITIIPC_PATH_NOT_FOUND;
         break;

      case ERROR_BROKEN_PIPE:
         return ITIIPC_BROKEN_PIPE;
         break;

      case ERROR_PIPE_NOT_CONNECTED:
         return ITIIPC_PIPE_NOT_CONNECTED;
         break;

      default:
         return ITIIPC_ERROR_BASE;
         break;
      }
   }



static void ItiIpcBuildPipeName (PSZ    pszServer,
                                 PSZ    pszPipeName,
                                 PSZ    pszOut,
                                 USHORT usOutMax)
   {
   if (pszServer != NULL && *pszServer != '\0')
      {
      ItiStrCpy (pszOut, "\\\\", usOutMax);
      ItiStrCat (pszOut, pszServer, usOutMax);
      ItiStrCat (pszOut, pszPipeName, usOutMax);
      }
   else
      {
      ItiStrCpy (pszOut, pszPipeName, usOutMax);
      }
   }


/*
 * ItiIpcOpenNamedPipe opens the specified named pipe, for the given
 * server.  If the named pipe is busy, then the function will wait for
 * up to usTimeOut milliseconds to connect.  
 *
 * Parameters: pszServer         The name of the server that the
 *                               named pipe resides on.  If this
 *                               parameter may be NULL or a pointer to 
 *                               a null string, then the named pipe
 *                               is assumed to be a local named pipe.
 *
 *             pszPipeName       The name of the named pipe.  This
 *                               parameter may not be NULL.
 *
 *             usTimeOut         The number of milliseconds to wait
 *                               for an instance of the named pipe
 *                               to become available.  See the comments
 *                               section below.
 *
 *             phpPipe           A pointer to a variable that will
 *                               contain the pipe handle.
 *
 * Return value: 0 on success, or one of the following errors:
 *
 * ITIIPC_PIPE_BUSY
 * ITIIPC_BAD_PIPE
 * ITIIPC_TIMEOUT
 * ITIIPC_PATH_NOT_FOUND
 *
 * Comments:
 * The function may wait for much longer than usTimeOut.
 */


USHORT EXPORT ItiIpcOpenNamedPipe (PSZ    pszServer, 
                                   PSZ    pszPipeName, 
                                   USHORT usTimeOut,
                                   PHPIPE phpPipe)
   {
   char     szPipeName [MAX_FILE_NAME_SIZE];
   USHORT   usError;
   USHORT   usAction;

   ItiIpcBuildPipeName (pszServer, pszPipeName, szPipeName, sizeof szPipeName);
   
   usError = DosOpen2 (szPipeName, phpPipe, &usAction, 0, 0, FILE_OPEN,
                       OPEN_ACCESS_READWRITE | OPEN_SHARE_DENYREADWRITE |
                       OPEN_FLAGS_NOINHERIT, NULL, 0);

   while (usError == ERROR_PIPE_BUSY)
      {
      usError = DosWaitNmPipe (szPipeName, usTimeOut);
      if (usError == 0)
         {
         usError = DosOpen2 (szPipeName, phpPipe, &usAction, 0, 0, FILE_OPEN,
                       OPEN_ACCESS_READWRITE | OPEN_SHARE_DENYREADWRITE |
                       OPEN_FLAGS_NOINHERIT, NULL, 0);
         }
      }

   return ItiIpcConvertError (usError);
   }





/*
 * ItiIpcConnectNamedPipe connects a server to a named pipe.
 *
 * Parameters: hpPipe         The pipe to connect to.
 *
 * Return Value: 0 on success, or one of the following errors:
 *
 * ITIIPC_BAD_PIPE
 * ITIIPC_BROKEN_PIPE
 * ITIIPC_INVALID_FUNCTION
 * ITIIPC_PIPE_NOT_CONNECTED
 */

USHORT EXPORT ItiIpcConnectNamedPipe (HPIPE  hpPipe)
   {
   return ItiIpcConvertError (DosConnectNmPipe (hpPipe));
   }





/*
 * ItiIpcDisconnectNamedPipe disconnects a server from a named pipe.
 *
 * Parameters: hpPipe         The pipe to disconnect from.
 *
 * Return Value: 0 on success, or one of the following errors:
 *
 * ITIIPC_BAD_PIPE
 * ITIIPC_INVALID_FUNCTION
 */

USHORT EXPORT ItiIpcDisconnectNamedPipe (HPIPE  hpPipe)
   {
   return ItiIpcConvertError (DosDisConnectNmPipe (hpPipe));
   }






USHORT EXPORT ItiIpcCreateNamedPipe (PSZ     pszPipeName, 
                                     PHPIPE  phpPipe,
                                     USHORT  usInstances,
                                     USHORT  usReadSize,
                                     USHORT  usWriteSize)
   {
   UCHAR ucInstances;
   USHORT   usError;

   if (usInstances > MAX_INSTANCES)
      ucInstances = NP_UNLIMITED_INSTANCES;
   else  
      ucInstances = (UCHAR) usInstances;

   usError = DosMakeNmPipe (pszPipeName, phpPipe, 
               NP_ACCESS_DUPLEX | NP_NOINHERIT | NP_NOWRITEBEHIND,
               NP_WAIT | NP_READMODE_MESSAGE | NP_TYPE_MESSAGE | ucInstances,
               usReadSize, usWriteSize, MAX_CALL_WAIT);
   return ItiIpcConvertError (usError);
   }





USHORT EXPORT ItiIpcCallNamedPipe (PSZ       pszServer, 
                                   PSZ       pszPipeName,
                                   PVOID     pvSendData, 
                                   USHORT    usSendData,
                                   PPVOID    ppvReceiveData,
                                   PUSHORT   pusReceiveData)
   {
   char     szPipeName [MAX_FILE_NAME_SIZE];
   USHORT   usBufferSize;
   USHORT   usError;

   ItiIpcBuildPipeName (pszServer, pszPipeName, szPipeName, sizeof szPipeName);
   *ppvReceiveData = NULL;
   *pusReceiveData = 0;

   usBufferSize = (USHORT) MAX_SEG_SIZE - 1;
   *ppvReceiveData = ItiMemAllocSeg (usBufferSize, SEG_NONSHARED, 0);
   if (*ppvReceiveData == NULL)
      {
      return ERROR_NOT_ENOUGH_MEMORY;
      }

   usError = DosCallNmPipe (szPipeName, 
                            pvSendData, usSendData, 
                            *ppvReceiveData, usBufferSize, pusReceiveData,
                            MAX_CALL_WAIT);
   if (usError)
      {
      ItiMemFreeSeg (*ppvReceiveData);
      *ppvReceiveData = NULL;
      *pusReceiveData = 0;
      return ItiIpcConvertError (usError);
      }
   *ppvReceiveData = ItiMemReallocSeg (*ppvReceiveData, *pusReceiveData, 0);

   return 0;
   }





/* a very non-portable bit of code.  */

USHORT EXPORT ItiIpcSendCommand (PSZ      pszServer,
                                 ULONG    ulSessionFlags,
                                 USHORT   usNumArgs,
                                 char     *ppszArgs [])
   {
   PCOMMANDBUF pcmd;
   USHORT      i;
   USHORT      usBase;
   USHORT      usBufferSize;
   USHORT      usStrSize;
   PUSHORT     pusResult;
   USHORT      usResultSize;

   if (usNumArgs == 0)
      /* don't waste my time, bud. */
      return 0;

   /* figure out how big to make the command buffer */
   usBufferSize = sizeof (COMMANDBUF);
   usBase = usBufferSize;

   for (i=0; i < usNumArgs; i++)
      {
      usBufferSize += sizeof pcmd->ppszArgs [0];
      usBase       += sizeof pcmd->ppszArgs [0];
      usBufferSize += ItiStrLen (ppszArgs [i]) + 1;
      }

   pcmd = ItiMemAllocSeg (usBufferSize, SEG_NONSHARED, MEM_ZERO_INIT);
   if (pcmd == NULL)
      return ERROR_NOT_ENOUGH_MEMORY;

   pcmd->usNumArgs = usNumArgs;
   pcmd->usVersion = VERSION;
   pcmd->ulSessionFlags = ulSessionFlags;
   
   /* build the command buffer */
   for (i=0; i < usNumArgs; i++)
      {
      pcmd->ppszArgs [i] = (BPSZ) usBase;
      usStrSize = ItiStrLen (ppszArgs [i]) + 1;
      ItiStrCpy (pcmd->ppszArgs [i], ppszArgs [i], usStrSize);
      usBase += usStrSize;
      }

   /* cripes, that was fun.  Now that the buffer has been built,
      send it to the server. */

   i = ItiIpcCallNamedPipe (pszServer, SERVER_COMMAND_PIPE, 
                            pcmd, usBufferSize, 
                            &pusResult, &usResultSize);
   
   /* free the command buffer */
   ItiMemFreeSeg (pcmd);

   if (i == 0)
      {
      i = *pusResult;
      }
   ItiMemFreeSeg (pusResult);

   return i;
   }


