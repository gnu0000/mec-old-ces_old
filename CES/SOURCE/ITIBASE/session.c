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
 * Session.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This file provides session management functions.
 */

#define INCL_DOSSESMGR
#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "error.h"

#define MAKEBOOL(ul) (!(!(ul)))



USHORT EXPORT ItiProcCreateSession (PSZ   pszSessionTitle,
                                    PSZ   pszProgramName,
                                    PSZ   pszProgramParameters,
                                    ULONG ulFlags,
                                    PPID  ppidProcess,
                                    PSID  psidSession)
   {
   STARTDATA stdata;
   USHORT    usProgramType, us;

   stdata.Length = sizeof stdata;
   stdata.Related = MAKEBOOL (ulFlags & SESSION_RELATED);
   stdata.FgBg = MAKEBOOL (ulFlags & SESSION_BACKGROUND);
   stdata.TraceOpt = 0;
   stdata.PgmTitle = pszSessionTitle;
   stdata.PgmName = pszProgramName;
   stdata.PgmInputs = pszProgramParameters;
   stdata.TermQ = 0;
   stdata.Environment = 0;
   stdata.InheritOpt = MAKEBOOL (ulFlags & SESSION_RELATED);
   stdata.SessionType = (USHORT) ((ulFlags >> 24) & 3);
   stdata.IconFile = 0;
   stdata.PgmHandle = 0;
   stdata.PgmControl = (USHORT) ulFlags & 0x000f;
   stdata.InitXPos = 0;
   stdata.InitYPos = 0;
   stdata.InitXSize = 0;
   stdata.InitYSize = 0;

   if (stdata.SessionType == 0)
      {
      /* determine the type of the executable */
      DosQAppType (pszProgramName, &usProgramType);
      if (usProgramType == WINDOWAPI)
         stdata.SessionType = 3;
      }

   us = DosStartSession (&stdata, psidSession, ppidProcess);

   return ConvertBaseError (us);


//  The following generates a General Protection Violation under
//  OS/2 version 2.0.
//
//   return ConvertBaseError (DosStartSession (&stdata, psidSession, 
//                                             ppidProcess));
   }





USHORT EXPORT ItiProcDestroySession (SID sidSession)
   {
   return ConvertBaseError (DosStopSession (0, sidSession, 0));
   }


USHORT EXPORT ItiProcSelectSession (SID sidSession)
   {
   return ConvertBaseError (DosSelectSession (sidSession, 0));
   }
