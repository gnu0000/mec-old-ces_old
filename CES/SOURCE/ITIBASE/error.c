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
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This module translates OS/2 errors to ITIBASE errors
 */

#define INCL_DOSERRORS
#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itierror.h"
#include "error.h"

USHORT ConvertBaseError (USHORT usError)
   {
   switch (usError)
      {
      case 0:
      case ERROR_NOT_ENOUGH_MEMORY:
      case ERROR_INVALID_PARAMETER:
      case ERROR_INVALID_FUNCTION:
         return usError;
         break;

      case ERROR_ALREADY_EXISTS:
         return ITIBASE_SEM_ALREADY_EXISTS;
         break;

      case ERROR_INVALID_NAME:
         return ITIBASE_SEM_INVALID_NAME;
         break;

      case ERROR_SEM_NOT_FOUND:
         return ITIBASE_SEM_NOT_FOUND;
         break;

      case ERROR_SEM_IS_SET:
         return ITIBASE_SEM_IS_SET;
         break;

      case ERROR_EXCL_SEM_ALREADY_OWNED:
         return ITIBASE_SEM_ALREADY_OWNED;
         break;

      case ERROR_SEM_TIMEOUT:
         return ITIBASE_SEM_TIMEOUT;
         break;

      case ERROR_INVALID_THREADID:
         return ITIBASE_INVALID_THREAD_ID;
         break;

      case ERROR_SMG_START_IN_BACKGROUND:
         return ITIBASE_SESSION_IN_BACKGROUND;
         break;
         
      case ERROR_SMG_SESSION_NOT_FOREGRND:
         return ITIBASE_SESSION_NOT_FOREGROUND;
         break;

      default:
         return ITIBASE_ERROR_BASE;
         break;
      }
   }
