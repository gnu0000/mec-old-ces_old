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
 * Variable.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This file provides the Variable format method.
 */

#include "..\include\iti.h"
#include "..\include\itifmt.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"
#include "Variable.h"


USHORT EXPORT FmtVariable (PSZ     pszSource,
                           PSZ     pszDest,
                           USHORT  usDestMax,
                           PSZ     pszFormatInfo,
                           PUSHORT pusDestLength)
   {
   ItiStrCpy (pszDest, pszSource, usDestMax);
   if (pusDestLength)
      *pusDestLength = ItiStrLen (pszDest);

   return 0;
   }

