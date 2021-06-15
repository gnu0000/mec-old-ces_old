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
 * Write.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 */


#include "..\include\iti.h"
#include "..\include\itierror.h"
#include "..\include\itibase.h"
#include "initdll.h"


#define MAX_WAIT     10000

/*
 * ItiErrWriteDebugMessage writes a debugging message to the error named pipe.
 * The string is prefixed with "Debug: ", followed by pszMessage, followed
 * by a newline.
 *
 * Parameters: pszMessage     A null terminated string to be written.
 *
 * Return Value: none.
 *
 * Comments: 
 * The calling thread may block if the named pipe is full.
 */

VOID EXPORT ItiErrWriteDebugMessage (PSZ pszMessage)
   {
   USHORT      usBytesWritten;
   USHORT      usStrLen;
   char        szThread [6];
   BOOL        bDone;

   /* convert thread ID to a string -- NOTE: ASSUMES THAT A TID <= 99 */
   szThread [0] = '[';
   szThread [1] = (char) (plisLocalInfo->tidCurrent / 10 + '0');
   szThread [2] = (char) (plisLocalInfo->tidCurrent - 
                          ((plisLocalInfo->tidCurrent / 10) * 10) + '0');
   szThread [3] = ']';
   szThread [4] = ':';
   szThread [5] = ' ';

   /* get string length */
   usStrLen = 0;
   while (pszMessage && pszMessage [usStrLen] != '\0')
      usStrLen++;

   if (hfDebugPipe != 0 && 
       0 == ItiSemRequestMutex (hsemWritePipe, MAX_WAIT))
      {
      DosWrite (hfDebugPipe, szThread, sizeof szThread, &usBytesWritten);
      if (pszMessage == NULL)
         DosWrite (hfDebugPipe, "(null)", 6, &usBytesWritten);
      else if (!ItiMemQueryRead (pszMessage))
         DosWrite (hfDebugPipe, "(bad pointer)", 13, &usBytesWritten);
      else   
         for (bDone = FALSE; !bDone;)
            {
            for (usStrLen = 0; 
                 pszMessage [usStrLen] != '\0' &&
                 pszMessage [usStrLen] != '\n';
                 usStrLen++)
               ;
         
            DosWrite (hfDebugPipe, pszMessage, usStrLen, &usBytesWritten);
            if (pszMessage [usStrLen] == '\n')
               {
               if (pszMessage [usStrLen + 1] == '\0')
                  bDone = TRUE;
               else
                  {
                  DosWrite (hfDebugPipe, "\r\n", 2, &usBytesWritten);
                  pszMessage += usStrLen + 1;
                  if (*pszMessage == '\r')
                     pszMessage++;
                  bDone = *pszMessage == '\0';
                  }
               }
            else
               bDone = pszMessage [usStrLen] == '\0';
            }
      DosWrite (hfDebugPipe, "\r\n", 2, &usBytesWritten);
      ItiSemReleaseMutex (hsemWritePipe);
      }
   }


