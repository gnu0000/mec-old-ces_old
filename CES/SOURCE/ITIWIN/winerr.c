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


/************************************************************************
 *                                                                      *
 * winerr.c                                                             *
 *                                                                      *
 * Copyright (C) 1990-1992 Info Tech, Inc.                              *
 *                                                                      *
 * This file is part of the Window system of DS/SHELL. This is a        *
 * proprietary product of Info Tech. and no part of which may be        *
 * reproduced or transmitted in any form or by any means, including     *
 * photocopying and recording or in connection with any information     *
 * storage or retrieval system, without permission in writing           *
 * from Info Tech, Inc.                                                 *
 *                                                                      *
 *                                                                      *
 ************************************************************************/

#define  INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itierror.h"
#include "..\include\itiglob.h"
#include "initdll.h"
#include <stdio.h>


USHORT WinError (PSZ szStr)
   {
   ItiErrWriteDebugMessage ("LW-Data Error follows:");
   ItiErrWriteDebugMessage (szStr);

   return WinMessageBox (pglobals->hwndDesktop, pglobals->hwndAppFrame, 
                         szStr, "LWDATA-ERROR",
                         0, MB_OK | MB_ICONASTERISK | MB_MOVEABLE);
   }


USHORT WinErrorSU (PSZ szFormat, PSZ sz1, USHORT u2)
   {
   char szStr[LONGSTRLEN];

   sprintf (szStr, szFormat, sz1, u2);
   return WinError (szStr);
   }



