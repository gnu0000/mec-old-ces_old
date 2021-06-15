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
 * Flag.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 * 
 * This file provides the Flag format method.
 */

#include "..\include\iti.h"
#include "..\include\itifmt.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"
#include "flag.h"


#if !defined (min)
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#if !defined (max)
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif




USHORT EXPORT FmtFlag (PSZ     pszSource,
                       PSZ     pszDest,
                       USHORT  usDestMax,
                       PSZ     pszFormatInfo,
                       PUSHORT pusDestLength)
   {
   LONG  lValue;
   PSZ   pszComma;

   if (!ItiStrToLONG (pszSource, &lValue))
      return ITIFMT_BAD_DATA;

   /* find the comma or the end of the string */
   for (pszComma = pszFormatInfo; *pszComma && *pszComma != ','; pszComma++)
      ;

   if (*pszFormatInfo == '\0' || *pszComma == '\0')
      return ITIFMT_FORMAT_ERROR;

   if (lValue)
      {
      ItiStrCpy (pszDest, pszComma + 1, usDestMax);
      }
   else if (*pszSource)
      {
      ItiStrCpy (pszDest, pszFormatInfo, 
                 min (usDestMax, (USHORT) (pszComma - pszFormatInfo + 1)));
      }
   else
      {
      // ItiStrCpy (pszDest, "Not Available", usDestMax);
      ItiStrCpy (pszDest, "N", usDestMax);
      }


   if (pusDestLength)
      *pusDestLength = ItiStrLen (pszDest);

   return 0;
   }




USHORT EXPORT CheckFlag (PSZ     pszSource,
                         PSZ     pszDest,
                         USHORT  usDestMax,
                         PSZ     pszFormatInfo,
                         PUSHORT pusDestLength)
   {
   USHORT usValue;
   PSZ    pszComma;
   char   szTemp [256];

   while (ItiCharIsSpace (*pszSource))
      pszSource++;

   if (*pszFormatInfo == '#')
      pszFormatInfo++;

   ItiStrCpy (szTemp, pszSource, sizeof szTemp);
   ItiClip (szTemp, " \n\r\t");
   if (ItiCharIsDigit (*szTemp))
      {
      if (!ItiStrToUSHORT (szTemp, &usValue))
         return ITIFMT_BAD_DATA;
      if (usValue != 0 && usValue != 1)
         return ITIFMT_BAD_DATA;

      ItiStrCpy (pszDest, szTemp, usDestMax);
      }
   else
      {
      /* find the comma or the end of the string */
      for (pszComma = pszFormatInfo; *pszComma && *pszComma != ','; pszComma++)
         ;
   
      if (*pszFormatInfo == '\0' || *pszComma == '\0')
         return ITIFMT_FORMAT_ERROR;
      if (*szTemp == '\0')
         *pszDest = '\0';
      if (ItiStrNICmp (szTemp, pszComma + 1, usDestMax) == 0)
         ItiStrCpy (pszDest, "1", usDestMax);
      else if (ItiStrNICmp (szTemp, pszFormatInfo, 
               min (usDestMax, (USHORT) (pszComma - pszFormatInfo))) == 0)
         ItiStrCpy (pszDest, "0", usDestMax);
      else
         return ITIFMT_BAD_DATA;
      }

   if (pusDestLength)
      *pusDestLength = ItiStrLen (pszDest);

   return 0;
   }




