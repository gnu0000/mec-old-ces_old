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
 * init.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This module initializes the global PBX server
 */

#define INCL_DOS
#define INCL_DOSERRORS
#include "..\include\iti.h"
#include "..\include\itiglob.h"
#include "..\include\itiipc.h"
#include "..\include\itibase.h"
#include "..\include\itierror.h"
#include <process.h>
#include <stdio.h>
#include "init.h"
#include "pbxserve.h"
#include "receive.h"

BOOL Init (void)
   {
   ItiGlobInitGlobals (NULL, NULL, NULL, NULL, "PBX Server", "PBXServe", "1.00");

   /* start reader threads */
   if (!InitReceivers ())
      return FALSE;

   return TRUE;
   }


