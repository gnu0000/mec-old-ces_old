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
 * Binary.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This file provides the Binary format method.
 */

#include "..\include\iti.h"
#include "..\include\itifmt.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"
#include "binary.h"
#include "string.h"



/*
 * FormatInfo = '#' | .
 *
 * A '#' tells the check function to not quote the string.
 */



USHORT EXPORT CheckBinary (PSZ     pszSource,
                           PSZ     pszDest,
                           USHORT  usDestMax,
                           PSZ     pszFormatInfo,
                           PUSHORT pusDestLength)
   {
   USHORT usLen;

   if (*pszSource == '0' && (*(pszSource+1) == 'x' || *(pszSource+1) == 'X'))
      {
      ItiStrCpy (pszDest, pszSource, usDestMax);

      if (pusDestLength)
         *pusDestLength = ItiStrLen (pszDest);
      return 0;
      }

   if (pszFormatInfo == NULL || *pszFormatInfo != '#')
      {
      ItiStrCpy (pszDest, "0x", usDestMax);
      usLen = 2;
      }
   else
      usLen = 0;

   while (*pszSource && usLen+1 < usDestMax)
      {
      pszDest [usLen] = (CHAR) ItiCharMakeHex (((*pszSource >> 4) & 0xf));
      pszDest [usLen+1] = (CHAR) ItiCharMakeHex (((*pszSource) & 0xf));
      usLen += 2;
      pszSource++;
      }
   pszDest [usLen] = '\0';

   if (pusDestLength)
      *pusDestLength = usLen;

   return 0;
   }







/*
 * Mark, this is not making a string.
 * in fact, its totally hosing my buffers
 * because of all the nulls it returns
 * within its length
 *
 * Did you want this to return a hex string ?
 * what is the input string format ?
 */


USHORT EXPORT FmtBinary (PSZ     pszSource,
                         PSZ     pszDest,
                         USHORT  usDestMax,
                         PSZ     pszFormatInfo,
                         PUSHORT pusDestLength)
   {
   USHORT usLen;

   if (*pszSource == '0' && (*(pszSource+1) == 'x' || *(pszSource+1) == 'X'))
      pszSource += 2;

   usLen = 0;
   while (*pszSource && usLen < usDestMax)
      {
      if (!ItiStrHexToUCHAR (pszSource, pszDest))
         return ITIFMT_BAD_DATA;

      usLen += 1;
      pszSource += 2;
      pszDest++;
      }

   *pszDest = '\0';

   if (pusDestLength)
      *pusDestLength = usLen;

   return 0;
   }

