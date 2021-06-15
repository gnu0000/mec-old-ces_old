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
 * String.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This file provides the String format method.
 */

#include "..\include\iti.h"
#include "..\include\itifmt.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"
#include "string.h"

#if !defined (min)
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#if !defined (max)
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif



USHORT EXPORT CheckString (PSZ     pszSource,
                           PSZ     pszDest,
                           USHORT  usDestMax,
                           PSZ     pszFormatInfo,
                           PUSHORT pusDestLength)
   {
   USHORT i, usFudge;
   PSZ    pszCopy;

   if (pszFormatInfo == NULL || *pszFormatInfo != '#')
      {
      ItiStrCpy (pszDest, "\"", usDestMax);
      usFudge = 2;
      }
   else
      {
      *pszDest = '\0';
      usFudge = 1;
      }

   pszCopy = pszSource;
   for (i=ItiStrLen (pszDest); i + usFudge < usDestMax && *pszCopy; i++)
      {
      if (*pszCopy == '"' && (pszFormatInfo == NULL || *pszFormatInfo != '#'))
         pszDest [i++] = '"';
      pszDest [i] = *pszCopy++;
      }
   pszDest [i] = '\0';

   /* trim trailing blanks and tabs */
   ItiClip (pszDest, "\t\n\r ");

   if (pszFormatInfo == NULL || *pszFormatInfo != '#')
      ItiStrCat (pszDest, "\"", usDestMax);

   /* Convert case */
   if (pszFormatInfo && *pszFormatInfo == '#')
      pszFormatInfo++;

   if (pszFormatInfo && pszFormatInfo [0] == 'C')
      ItiStrUpper (pszDest);
   else if (pszFormatInfo && pszFormatInfo [0] == 'l')
      ItiStrLower (pszDest);

   if (pusDestLength)
      *pusDestLength = ItiStrLen (pszDest);

   return 0;
   }




USHORT EXPORT FmtString (PSZ     pszSource,
                         PSZ     pszDest,
                         USHORT  usDestMax,
                         PSZ     pszFormatInfo,
                         PUSHORT pusDestLength)
   {
   PSZ pszTemp;
   PSZ pszStartSurround;
   USHORT usStartSurroundLen;
   PSZ pszEndSurround;
   USHORT usEndSurroundLen;

   pszStartSurround = NULL;
   usStartSurroundLen = 0;
   pszEndSurround = NULL;
   usEndSurroundLen = 0;

   if (pszFormatInfo != NULL)
      pszStartSurround = ItiStrChr (pszFormatInfo, '~');

   if (pszStartSurround != NULL)
      {
      pszStartSurround++;

      /* find the end of the first surround string */
      for (pszTemp = pszStartSurround; *pszTemp && *pszTemp != ','; pszTemp++)
         usStartSurroundLen++;

      if (*pszTemp != ',')
         return ITIFMT_FORMAT_ERROR;

      pszEndSurround = pszTemp + 1;
      /* find the end of the first surround string */
      for (pszTemp = pszEndSurround; *pszTemp && *pszTemp != '~'; pszTemp++)
         usEndSurroundLen++;

      if (*pszTemp != '~')
         return ITIFMT_FORMAT_ERROR;

      /* copy the start surround string */
      ItiStrCpy (pszDest, pszStartSurround, 
                 min (usDestMax, usStartSurroundLen + 1));
      }

   if (pszFormatInfo == NULL || 
       pszFormatInfo [0] == '\0' || 
       (pszFormatInfo [0] != 'N' && 
        pszFormatInfo [1] != 'N'))
      {
      for (pszTemp = pszSource; *pszTemp; pszTemp++)
         ;

      /* find the first non-space character */
      for (pszTemp -= 1; *pszTemp == ' '; pszTemp--)
         ;

      /* copy the string into the destination buffer */
      ItiStrCpy (pszDest + usStartSurroundLen, pszSource, 
                 min (usDestMax, (USHORT) (pszTemp - pszSource) + 2));
      }
   else
      /* copy the string into the destination buffer */
      ItiStrCpy (pszDest + usStartSurroundLen, pszSource, usDestMax);

   /* Convert case */
   if (pszFormatInfo && pszFormatInfo [0] == 'C')
      ItiStrUpper (pszDest);
   else if (pszFormatInfo && pszFormatInfo [0] == 'l')
      ItiStrLower (pszDest);

   /* copy the end surround string */
   if (pszEndSurround)
      ItiStrNCat (pszDest, pszEndSurround, usEndSurroundLen, usDestMax);

   if (pusDestLength)
      *pusDestLength = ItiStrLen (pszDest);

   return 0;
   }

