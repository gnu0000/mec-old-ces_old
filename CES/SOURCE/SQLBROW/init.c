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
 * Init.c
 * Mark Hampton
 * Copyright (C) 1990 Info Tech, Inc.
 *
 * SQLBrow initialization code
 */


#define INCL_WIN
#define INCL_DOS
#include <stdio.h>
#include "..\include\iti.h"
#include "sqlbrow.h"
#include "init.h"
#include "dialog.h"
#include "..\include\message.h"
//#include "..\include\itiwin.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itibase.h"
#include "..\include\winid.h"

/* include references to DLLs */
#include "..\include\itierror.h"
#include "..\include\itiglob.h"
#include "..\include\itimenu.h"
#include "..\include\itifmt.h"

/* define global variables */
PGLOBALS pglobals = NULL;



/* define module specific globals */
#ifdef DEBUG
char szAppName [] = "BAMS/CES Browser: SQL";
#else
char szAppName [] = "BAMS/CES Browser: SQL";
#endif

#define NUM_DATA_BYTES  (1 * sizeof (void far *))





void MBExit (PSZ pszStr)
   {
   WinMessageBox (pglobals->hwndDesktop, pglobals->hwndAppFrame, 
                              pszStr, "BAMS/CES Init", 0, MB_OK);
   DosExit (EXIT_PROCESS, 0);
   }



BOOL RegisterWindows (void);

void Initialize (void)
   {
   HAB      habMainThread;
   HMQ      hmqMainThread;
   ULONG    flStyle;
   HPOINTER hptr;

   flStyle = FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER |
             FCF_MINMAX | FCF_SHELLPOSITION | FCF_TASKLIST |
             FCF_MENU | FCF_ACCELTABLE | FCF_ICON;

   /*
    * Initialize the thread for making Presentation Manager calls and
    * create the message queue.
    */

   habMainThread = WinInitialize (0);
   if (habMainThread == NULL)
      {
      ItiErrDisplayWindowError (habMainThread, HWND_DESKTOP, 0);
      DosExit (EXIT_PROCESS, 0);
      }

   hmqMainThread = WinCreateMsgQueue (habMainThread, 1000);

   if (hmqMainThread == NULL)
      {
      ItiErrDisplayWindowError (habMainThread, HWND_DESKTOP, 0);
      DosExit (EXIT_PROCESS, 0);
      }

   /* Initialize the Globals module. DO NOT MOVE THIS CODE TO BE
      ANY LATER THAN THIS LINE!!! OR ALL HELL WILL BREAK LOOSE!!! */
   pglobals = ItiGlobInitGlobals (habMainThread, hmqMainThread, 
                                  HWND_DESKTOP,
                                  HWND_DESKTOP,
                                  szAppName,
                                  "SQLBrow",
                                  VERSION_STRING);

   if (pglobals == NULL)
      {
      DosExit (EXIT_PROCESS, 0);
      }
   pglobals->usVersionMajor = VERSION_MAJOR;
   pglobals->usVersionMinor = VERSION_MINOR;
   pglobals->pszVersionString = VERSION_STRING;
   pglobals->ucVersionChar = VERSION_CHAR;

   /* set the multilingual character set */
   if (!WinSetCp (hmqMainThread, 850))
      {
      ItiErrWriteDebugMessage ("Could not set code page 850.");
      WinMessageBox (HWND_DESKTOP, HWND_DESKTOP, 
                     "Could not set code page 850.  See your OS/2 "
                     "documentation regarding the CODEPAGE and DEVINFO "
                     "lines in the CONFIG.SYS file.  "
                     "This error will only affect the appearance of "
                     "certain characters on the screen.", 
                     szAppName, 0, 
                     MB_OK | MB_ICONHAND | MB_MOVEABLE);
      }

   hptr = WinQuerySysPointer (HWND_DESKTOP, SPTR_WAIT, FALSE);
   WinSetPointer (HWND_DESKTOP, hptr);
   hptr = WinQuerySysPointer (HWND_DESKTOP, SPTR_ARROW, FALSE);

   /*
    * Do time consuming initiailzation here
    */

   if (!ItiFmtInitialize ())
      {
      WinSetPointer (HWND_DESKTOP, hptr);
      MBExit ("The format module could not be initialized.");
      }

   if (!ItiDbInit (0, 1))
      {
      WinSetPointer (HWND_DESKTOP, hptr);
      MBExit ("The data base module could not be initialized.");
      }

//   if (ItiWndRegister (habMainThread))
//      {
//      WinSetPointer (HWND_DESKTOP, hptr);
//      MBExit ("The list window module could not be initialized.");
//      }

   WinSetPointer (HWND_DESKTOP, hptr);
   }


void Deinitialize (void)
   {
//   ItiWndTerminate ();
   WinDestroyMsgQueue (pglobals->hmqMainMessageQueue); 
   WinTerminate (pglobals->habMainThread);
   }


