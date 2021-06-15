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
 * Ipc.h
 * Mark Hampton
 *
 * General IPC functions, callable by clients and servers/
 */


#if !defined (IPC_INCLUDED) 

#define IPC_INCLUDED


#define UNLIMITED_INSTANCES 0xffff
#define MAX_INSTANCES       254

typedef char _based ((_segment) _self) *BPSZ;

typedef struct
   {
   USHORT   usNumArgs;
   USHORT   usVersion;
   ULONG    ulSessionFlags;
   BPSZ     ppszArgs [];
   } COMMANDBUF;           /* cmd */

typedef COMMANDBUF *PCOMMANDBUF;

#define MAX_COMMAND_SIZE   16374U

#define MAX_CALL_WAIT   10000UL

#define MAX_LOCAL_CONNECTIONS    10

typedef PVOID *PPVOID;

#define MAX_FILE_NAME_SIZE 261



/* define error values */
#define ITIIPC_PIPE_BUSY               (ITIIPC_ERROR_BASE +  1)
#define ITIIPC_BAD_PIPE                (ITIIPC_ERROR_BASE +  2)
#define ITIIPC_TIMEOUT                 (ITIIPC_ERROR_BASE +  3)
#define ITIIPC_PATH_NOT_FOUND          (ITIIPC_ERROR_BASE +  4)
#define ITIIPC_BROKEN_PIPE             (ITIIPC_ERROR_BASE +  5)
#define ITIIPC_PIPE_NOT_CONNECTED      (ITIIPC_ERROR_BASE +  6)


#define SERVER_COMMAND_PIPE  "\\pipe\\iti\\dsshell\\ipc\\command"


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


extern USHORT EXPORT ItiIpcOpenNamedPipe (PSZ    pszServer, 
                                          PSZ    pszPipeName, 
                                          USHORT usTimeOut,
                                          PHPIPE phpPipe);





/*
 * ItiIpcConnectNamedPipe connects a server to a named pipe.
 *
 * Parameters: hpPipe         The pipe to connect to.
 *
 * Return Value: 0 on success, or one of the following errors:
 *
 * ITIIPC_BAD_PIPE
 * ITIIPC_BROKEN_PIPE
 * ITIIPC_INTERRUPT
 * ERROR_INVALID_FUNCTION
 * ITIIPC_PIPE_NOT_CONNECTED
 */

extern USHORT EXPORT ItiIpcConnectNamedPipe (HPIPE  hpPipe);





/*
 * ItiIpcDisconnectNamedPipe disconnects a server from a named pipe.
 *
 * Parameters: hpPipe         The pipe to disconnect from.
 *
 * Return Value: 0 on success, or one of the following errors:
 *
 * ITIIPC_BAD_PIPE
 * ERROR_INVALID_FUNCTION
 */

extern USHORT EXPORT ItiIpcDisconnectNamedPipe (HPIPE  hpPipe);






extern USHORT EXPORT ItiIpcCreateNamedPipe (PSZ     pszPipeName, 
                                            PHPIPE  phpPipe,
                                            USHORT  usInstances,
                                            USHORT  usReadSize,
                                            USHORT  usWriteSize);





extern USHORT EXPORT ItiIpcCallNamedPipe (PSZ       pszServer, 
                                          PSZ       pszPipeName,
                                          PVOID     pvSendData, 
                                          USHORT    usSendData,
                                          PPVOID    ppvReceiveData,
                                          PUSHORT   pusReceiveData);




extern USHORT EXPORT ItiIpcSendCommand (PSZ      pszServer,
                                        ULONG    ulSessionFlags,
                                        USHORT   usNumArgs,
                                        char     *ppszArgs []);






#define ITIIPC_ALL_PROCESSES  0
#define ITIIPC_PBX            1
#define ITIIPC_LOCK_MANAGER   2
#define ITIIPC_LOCAL_PBX      3


#define ITIIPC_ALL_MACHINES   0
#define ITIIPC_SERVER         1

/*
 * pid is the process ID of the sender/receiver.  Special values:
 *    ITIIPC_ALL_PROCESSES    - send the message to all processes.
 *    ITIIPC_PBX              - send the message to the PBX server.
 *    ITIIPC_LOCK_MANAGER     - send the message to the lock manager.
 *    ITIIPC_LOCAL_PBX        - send the message to the local PBX server.
 *
 * usMachine is a unique machine identifier, assigned by the PBX server.
 * Special values:
 *    ITIIPC_ALL_MACHINES     - send the message to all machines.
 *    ITIIPC_SERVER           - send the message to the machine that
 *                              is running the PBX server.
 *
 */

typedef struct
   {
   PID      pid;              /* process ID */
   USHORT   usMachine;        /* machine ID */
   } ADDR;

typedef ADDR FAR *PADDR;



typedef struct
   {
   USHORT   usMessage;        /* message */
   ADDR     addrSender;       /* sender's address */
   ADDR     addrReceiver;     /* destination address */
   USHORT   usMessageSize;    /* size of the message data */
   BYTE     pbData [];        /* message data */
   } MESSAGE;

typedef MESSAGE FAR *PMESSAGE;



typedef struct 
   {
   USHORT   usReply;          /* reply */
   USHORT   usReplySize;      /* size of reply data */
   BYTE     pbByte [];        /* reply data */
   } REPLY;

typedef REPLY FAR *PREPLY;


#endif /* if !defined (IPC_INCLUDED) */

