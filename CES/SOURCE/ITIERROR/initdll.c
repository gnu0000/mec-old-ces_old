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
 * InitDll.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This is an almost generic DLL initialization module.  The function 
 * DllInit is called by either the ITIDLLI.OBJ or ITIDLLC.OBJ module.
 * 
 * Use the ITIDLLI.OBJ module when your DLL does not use the C run time
 * library.  Use the ITIDLLC.OBJ module when you need to use the C run
 * time library, and you need to initialize your DLL.  Note that when
 * you do this, you will have to link with CRTDLL_I.OBJ, a standard 
 * Microsoft module.
 *
 * Note: Do not rename this file to DLLINIT, since this file name
 * is already used by Microsoft for C run time library initialization.
 */

#define INCL_DOS
#define INCL_DOSPROCESS
#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"
#include "initdll.h"
#include <stdio.h>

char szFileName [256];

/* hfDebugPipe is the handle to the debugging pipe/file */
HFILE hfDebugPipe = 0;

ULONG ulWriteSem = 0;
HSEM hsemWritePipe = &ulWriteSem;

/* plisLocalInfo is a global pointer to the local info seg. */
PLINFOSEG plisLocalInfo = NULL;
PGINFOSEG pgisGlobalInfo = NULL;


#ifdef DEBUG_PIPE


/* this variable is used for the name of the pipe to create.  InitDll 
   changes the '00000' to be the pid of the calling process. */

static char szPipeName [256];

static ULONG ulSemaphore1 = 0;
static ULONG ulSemaphore2 = 0;

/* have to cheat here, since sem module may no init before me */
static HSEM hsemPipeCreated = &ulSemaphore1;
static HSEM hsemPipeConnected = &ulSemaphore2;

#define MAX_CONNECT_TIME   2000

#endif /* ifdef DEBUG_PIPE */


PSZ EXPORT ItiErrQueryLogFileName (void)
   {
   return szFileName;
   }



#ifdef DEBUG_PIPE

void THREAD MakePipeThread (void)
   {
   USHORT      usError;
   USHORT      usAction;

   usError = DosMakeNmPipe (szPipeName, &hfDebugPipe, 
                        NP_NOINHERIT | NP_ACCESS_OUTBOUND | NP_WRITEBEHIND,
                        NP_WAIT | NP_READMODE_BYTE | NP_TYPE_BYTE | 1,
                        32767, 32767, 0x7fffffff);
   
   if (usError)
      {
      DosBeep (600, 175);
      DosBeep (1200, 175);
      usError = DosOpen (szPipeName + 6, &hfDebugPipe, &usAction, 
                         0, FILE_NORMAL, FILE_CREATE | FILE_TRUNCATE,
                         OPEN_ACCESS_WRITEONLY | OPEN_SHARE_DENYWRITE |
                         OPEN_FLAGS_NOINHERIT | OPEN_FLAGS_SEQUENTIAL,
                         0);
      if (usError)
         {
         DosBeep (600, 175);
         DosBeep (1200, 175);
         }

      DosSemClear (hsemPipeCreated);
      ItiProcExitThread ();
      }

   DosSemClear (hsemPipeCreated);

   DosSemSet (hsemPipeConnected);
   usError = DosConnectNmPipe (hfDebugPipe);

   DosSemClear (hsemPipeConnected);
   }

#endif /* ifdef DEBUG_PIPE */




void EXPORT ErrorTerminate (USHORT usTermCode)
   {
   if (hfDebugPipe != 0 && 
       0 == ItiSemRequestMutex (hsemWritePipe, 0))
      {
      char szTemp [256];

      ItiSemReleaseMutex (hsemWritePipe);

      sprintf (szTemp, "Termination time: %2d/%.2d/%.4d %2d:%.2d:%.2d%cm", 
               (USHORT) pgisGlobalInfo->month,
               (USHORT) pgisGlobalInfo->day,
               (USHORT) pgisGlobalInfo->year,
               (USHORT) pgisGlobalInfo->hour > 12 ? pgisGlobalInfo->hour - 12 : pgisGlobalInfo->hour,
               (USHORT) pgisGlobalInfo->minutes,
               (USHORT) pgisGlobalInfo->seconds,
               (USHORT) pgisGlobalInfo->hour >= 12 ? 'p' : 'a');
      ItiErrWriteDebugMessage (szTemp);

      sprintf (szTemp, "Termination reason (%d): %s", 
               usTermCode,
               usTermCode == TC_EXIT ? "Normal exit" :
               usTermCode == TC_HARDERROR ? "Hard Error" :
               usTermCode == TC_KILLPROCESS ? "Kill Process" :
               usTermCode == TC_TRAP ? "Trap" : "Unknown");
      ItiErrWriteDebugMessage (szTemp);

#ifndef DEBUG_PIPE

      DosClose (hfDebugPipe);

#endif /* ifndef DEBUG_PIPE */
      }

   DosExitList (EXLST_EXIT, NULL);
   }




BOOL DLLINITPROC DllInit (HMODULE hmodDll)
   {
   SEL         selLocalInfo, selGlobalInfo;
   char        szTmpFileName [9];
   char        szTemp [6];
   PSZ         pszTemp;
   USHORT      i, usError;
#ifdef DEBUG_PIPE
   char        szCommandLine [256];
   TID         tidThread;
   PID         pid;
#endif /* ifdef DEBUG_PIPE */

   DosGetInfoSeg (&selGlobalInfo, &selLocalInfo);
   plisLocalInfo = MAKEP (selLocalInfo, 0);
   pgisGlobalInfo = MAKEP (selGlobalInfo, 0);

   DosExitList (EXLST_ADD, ErrorTerminate);

   /* build the file name -- iti + pid of this process */
   ItiStrCpy (szTmpFileName, "iti00000", sizeof szTmpFileName);
   i = ItiStrUSHORTToString (plisLocalInfo->pidCurrent, szTemp, sizeof szTemp);
   ItiStrCpy (szTmpFileName + 8 - i, szTemp, sizeof szTmpFileName - 8 + i);
   
#ifdef DEBUG_PIPE
   /* build the pipe name -- \pipe\ + FileName */
   ItiStrCpy (szPipeName, "\\pipe\\", sizeof szPipeName);
   ItiStrCat (szPipeName, szTmpFileName, sizeof szPipeName);
#endif /* ifdef DEBUG_PIPE */

   /* find out if there is a TEMP environment variable */
   if (DosScanEnv ("TEMP", &pszTemp))
      if (DosScanEnv ("TMP", &pszTemp))
         pszTemp = ".\\";

   ItiStrCpy (szFileName, pszTemp, sizeof szFileName);
   if (szFileName [ItiStrLen (szFileName) - 1] != '\\')
      ItiStrCat (szFileName, "\\", sizeof szFileName);
   ItiStrCat (szFileName, szTmpFileName, sizeof szFileName);

#ifdef DEBUG_PIPE
   /* build a command line for teepee -- should look something like: 
      -p \pipe\iti00000 d:\tmp\iti00000 */
   ItiStrCpy (szCommandLine, "-p ", sizeof szCommandLine);
   ItiStrCat (szCommandLine, szPipeName, sizeof szCommandLine);
   ItiStrCat (szCommandLine, " ", sizeof szCommandLine);
   ItiStrCat (szCommandLine, szFileName, sizeof szPipeName);

   DosSemSet (hsemPipeCreated);

   /* create thread to that will handle the named pipe. */
   if (ItiProcCreateThread (MakePipeThread, &tidThread, 0))
      {
      ItiErrDisplayHardError ("Could not create error startup thread.");
      return TRUE;
      }

   /* wait for the thread to initialize */
   DosSemWait (hsemPipeCreated, SEM_INDEFINITE_WAIT);

   /* create the session */
   usError = ItiProcCreateSession ("ITI Debug Output", "tpee.exe",
                                   szCommandLine, 
                                   SESSION_UNRELATED | SESSION_BACKGROUND | 
                                   SESSION_ICONIC,
                                   &pid, &i);

   /* wait for a connection */
   if (DosSemWait (hsemPipeConnected, MAX_CONNECT_TIME))
      {
      DosClose (hfDebugPipe);
      hfDebugPipe = 0;
      }

#else

   usError = DosOpen2 (szFileName, &hfDebugPipe, &i, 0, FILE_NORMAL, 
                       FILE_TRUNCATE | FILE_CREATE, 
                       OPEN_ACCESS_WRITEONLY | OPEN_SHARE_DENYWRITE | 
                       OPEN_FLAGS_NOINHERIT, NULL, 0);
   if (usError)
      hfDebugPipe = 0;

#endif /* ifdef DEBUG_PIPE */

   /* -- Write file name to the file. */
   ItiErrWriteDebugMessage (szFileName);

#ifdef PRERELEASE
   ItiErrWriteDebugMessage ("Release "VERSION_STRING", "PRERELEASE", "__DATE__" "__TIME__);
#else
   ItiErrWriteDebugMessage ("Release "VERSION_STRING);
#endif

   return TRUE;
   }




