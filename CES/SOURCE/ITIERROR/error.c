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
 * Error.c
 * Mark Hampton
 * Copyright (C) 1990 Info Tech, Inc.
 *
 */

#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itierror.h"
#include "initdll.h"

static char szModuleName [] = "itierror.dll";



static PERRINFO perriErrorInfo [MAX_THREADS] = 
   {
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
   };



void EXPORT ItiErrDisplayWindowError (HAB     hab,
                                      HWND    hwndActivate,
                                      USHORT  usHelpID)
   {
   HWND     hwndDesktop;
   USHORT   usReturnValue;
   PSZ      pszError;

   hwndDesktop = WinQueryDesktopWindow (hab, NULL);

   pszError = ItiErrGetErrorString (hab);

   if (pszError == NULL)
      {
      usReturnValue = WinMessageBox (hwndDesktop, hwndActivate,
         "ErrDisplayWindowError was called, but no error was found.  "
         "Press OK to continue, or Cancel to activate Code View.",
         szModuleName, usHelpID,
         MB_OKCANCEL | MB_ICONEXCLAMATION | MB_HELP | MB_MOVEABLE | MB_SYSTEMMODAL);
      
      if (usReturnValue ==  MBID_CANCEL)
         DEBUG_BREAK;
         
      return;
      }

   usReturnValue = WinMessageBox (hwndDesktop, hwndActivate,
      pszError, "Win Subsystem Error",
      usHelpID, MB_OKCANCEL | MB_ICONEXCLAMATION | MB_HELP | MB_MOVEABLE |
      MB_SYSTEMMODAL);

   if (usReturnValue ==  MBID_CANCEL)
      DEBUG_BREAK;

   ItiErrFreeErrorString (pszError);
   }



VOID EXPORT ItiErrFreeErrorString (PSZ pszWindowError)
   {
   if (perriErrorInfo [plisLocalInfo->tidCurrent] == NULL)
      {
#ifdef DEBUG
      ItiErrWriteDebugMessage ("ItiErrFreeWindowError was called, but the thread had no error to free!");
#endif
      return;
      }

   if (pszWindowError == NULL)
      {
#ifdef DEBUG
      ItiErrWriteDebugMessage ("ItiErrFreeWindowError was called with a NULL parameter!");
#endif
      }
   WinFreeErrorInfo (perriErrorInfo [plisLocalInfo->tidCurrent]);
   perriErrorInfo [plisLocalInfo->tidCurrent] = NULL;
   }





PSZ EXPORT ItiErrGetErrorString (HAB hab)
   {
   SEL      selError;
   PUSHORT  pusOffsets;
   ERRORID  error;

   perriErrorInfo [plisLocalInfo->tidCurrent] = WinGetErrorInfo (hab);
   error = WinGetLastError (hab);

   if (perriErrorInfo [plisLocalInfo->tidCurrent] == NULL && error == 0)
      {
#ifdef DEBUG
      ItiErrWriteDebugMessage ("ItiErrGetWindowError was called, but no error was found");
#endif
      return NULL;
      }
   else if (perriErrorInfo [plisLocalInfo->tidCurrent] == NULL)
      {
#ifdef DEBUG
      ItiErrWriteDebugMessage ("ItiErrGetWindowError was called, and OS/2 is giving conflicting results!");
#endif
      }

   /* get the selector to the error message segment */
   selError = SELECTOROF (perriErrorInfo [plisLocalInfo->tidCurrent]);

   /* get a pointer to the array of offsets to messages */
   pusOffsets = MAKEP (selError, perriErrorInfo [plisLocalInfo->tidCurrent]->offaoffszMsg);

#ifdef DEBUG
   ItiErrWriteDebugMessage (MAKEP (selError, pusOffsets [0]));
#endif 

   return MAKEP (selError, pusOffsets [0]);
   }



