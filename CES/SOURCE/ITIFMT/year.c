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
 * Year.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This file provides the Year format method.
 */

#include "..\include\iti.h"
#include "..\include\itifmt.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"
#include "Year.h"


USHORT EXPORT FmtYear (PSZ     pszSource,
                       PSZ     pszDest,
                       USHORT  usDestMax,
                       PSZ     pszFormatInfo,
                       PUSHORT pusDestLength)
   {
   USHORT usYear;
   USHORT usNumChars;

   if (!ItiStrToUSHORT (pszSource, &usYear) || usYear > 9999)
      return ITIFMT_BAD_DATA;
   
   if (ItiStrCmp (pszFormatInfo, "yy") == 0)
      {
      usYear = usYear % 100;
      }

   if (*pszSource)
      {
      usNumChars = ItiStrUSHORTToString (usYear, pszDest, usDestMax);
      }
   else
      {
      ItiStrCpy (pszDest, "Not Available",  usDestMax);
      usNumChars = ItiStrLen (pszDest);
      }

   if (pusDestLength)
      *pusDestLength = usNumChars;
     
   return 0; 
   }


USHORT EXPORT CheckYear (PSZ     pszSource,
                         PSZ     pszDest,
                         USHORT  usDestMax,
                         PSZ     pszFormatInfo,
                         PUSHORT pusDestLength)
   {
   USHORT usYear;
   USHORT usNumChars;

   if (!ItiStrToUSHORT (pszSource, &usYear) || usYear > 9999)
      return ITIFMT_BAD_DATA;

   if (usYear < 70)
      usYear += 2000;

   if (usYear < 100)
      usYear += 1900;

   if (usYear < 1900)
      return ITIFMT_BAD_DATA;

   if (*pszSource)
      {
      usNumChars = ItiStrUSHORTToString (usYear, pszDest, usDestMax);
      }
   else
      {
      *pszDest = '\0';
      if (pszFormatInfo == NULL || *pszFormatInfo != '#')
         ItiStrCpy (pszDest, "''",  usDestMax);
      usNumChars = ItiStrLen (pszDest);
      }

   if (pusDestLength)
      *pusDestLength = usNumChars;
     
   return 0; 
   }

