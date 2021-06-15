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
 * BamsNum.h
 * Mark Hampton
 * Charles Engelke
 * Copyright (C) 1989-1991 Info Tech, Inc.
 * 
 * This module provides Bams-number formatting.  
 */ 

#include "..\include\iti.h"
#include "..\include\itifmt.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"
#include "bamsnum.h"



USHORT EXPORT FmtBamsNumber (PSZ     pszSource, 
                             PSZ     pszDest, 
                             USHORT  usDestMax, 
                             PSZ     pszFormatInfo,
                             PUSHORT pusDestLength)
   {
   ItiStrCpy (pszDest, pszSource, usDestMax);
   return 0;
   }


USHORT EXPORT CheckBamsNumber (PSZ     pszSource, 
                               PSZ     pszDest, 
                               USHORT  usDestMax, 
                               PSZ     pszFormatInfo,
                               PUSHORT pusDestLength)
   {
   /* bams numbers can contain only digits and spaces */
   if (pszFormatInfo == NULL || *pszFormatInfo != '#')
      {
      *pszDest++ = '"';
      usDestMax -= 2;
      }

   while ((ItiCharIsDigit (*pszSource) || ItiCharIsSpace (*pszSource)) &&
          --usDestMax)
      *pszDest++ = *pszSource++;

   if (pszFormatInfo == NULL || *pszFormatInfo != '#')
      *pszDest++ = '"';

   *pszDest = '\0';
   return *pszSource == '\0' ? 0 : ITIFMT_BAD_DATA;
   }



