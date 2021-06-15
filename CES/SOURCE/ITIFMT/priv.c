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
 * Priv.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This file provides the Privilege format method.
 */

#include "..\include\iti.h"
#include "..\include\itifmt.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"
#include "Priv.h"


USHORT EXPORT FmtPrivilege (PSZ     pszSource,
                            PSZ     pszDest,
                            USHORT  usDestMax,
                            PSZ     pszFormatInfo,
                            PUSHORT pusDestLength)
   {
   USHORT usValue;

   if (!ItiStrToUSHORT (pszSource, &usValue))
      return ITIFMT_BAD_DATA;

   if (!ItiFmtQueryEnumString (ENUM_PRIVILEGE, usValue,
                               *pszFormatInfo == 'f', pszDest, usDestMax))
      return ITIFMT_BAD_DATA;

   if (pusDestLength)
      *pusDestLength = ItiStrLen (pszDest);

   return 0;
   }



USHORT EXPORT CheckPrivilege (PSZ     pszSource,
                              PSZ     pszDest,
                              USHORT  usDestMax,
                              PSZ     pszFormatInfo,
                              PUSHORT pusDestLength)
   {
   USHORT usValue;
   USHORT usLen;

   while (ItiCharIsSpace (*pszSource))
      pszSource++;

   if (!ItiFmtCheckEnumValue (ENUM_PRIVILEGE, pszSource, &usValue))
      return ITIFMT_BAD_DATA;

   usLen = ItiStrUSHORTToString (usValue, pszDest, usDestMax);

   if (pusDestLength)
      *pusDestLength = usLen;

   return 0;
   }


