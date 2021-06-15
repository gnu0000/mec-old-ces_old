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
 * LongText.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This file provides the LongText format method.
 */

#include "..\include\iti.h"
#include "..\include\itifmt.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "longtext.h"
#include "string.h"

#if !defined (min)
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#if !defined (max)
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif



#define SHORT_TEXT_COLUMN  0
#define LONG_TEXT_COLUMN   1


static PSZ pszQuery =
   "SELECT ShortText, LongText "
   "FROM LongText "
   "WHERE LongTextKey = ";


/*************************************************************************
 *
 * IMPORTANT NOTE!
 *
 * This executes a blocking query!  This means that the blocking SQL API
 * CANNOT use this format, since there is only one blocking connection!
 *
 * IMPORTANT NOTE!
 *
 *************************************************************************/


USHORT EXPORT FmtLongText (PSZ     pszSource,
                           PSZ     pszDest,
                           USHORT  usDestMax,
                           PSZ     pszFormatInfo,
                           PUSHORT pusDestLength)
   {
   HQRY hqry;
   char szQuery [1024];
   HHEAP    hheap;
   USHORT   usState, usCols;
   PSZ      *ppszData;

   hheap = ItiMemCreateHeap (0);
   if (hheap == NULL)
      {
      ItiStrCpy (pszDest, "?ItiFmtLongText Could not create a heap!", usDestMax);
      if (pusDestLength)
         *pusDestLength = ItiStrLen (pszDest);
      return 0;
      }

   /* build the query */
   ItiStrCpy (szQuery, pszQuery, sizeof szQuery);
   ItiStrCat (szQuery, pszSource, sizeof szQuery);

   /* execute the query */
   hqry = ItiDbExecQuery (szQuery, hheap, 0, 0, 0, &usCols, &usState);
   if (hqry == NULL)
      {
      ItiStrCpy (pszDest, "?ItiFmtLongText Could not exec query!", usDestMax);
      if (pusDestLength)
         *pusDestLength = ItiStrLen (pszDest);
      ItiMemDestroyHeap (0);
      return 0;
      }

   if (!ItiDbGetQueryRow (hqry, &ppszData, &usState))
      {
      usState = FmtString (NULL, pszDest, usDestMax, pszFormatInfo, 
                           pusDestLength);
      }
   else
      {
      pszSource = (ppszData [SHORT_TEXT_COLUMN] == NULL) ? 
                  ppszData [LONG_TEXT_COLUMN] :
                  ppszData [SHORT_TEXT_COLUMN];

      usState = FmtString (pszSource, pszDest, usDestMax, pszFormatInfo, 
                           pusDestLength);
      ItiMbFreeStrArray (hheap, ppszData, usCols);
      while (ItiDbGetQueryRow (hqry, &ppszData, &usState))
         ItiMbFreeStrArray (hheap, ppszData, usCols);
      }

   ItiMemDestroyHeap (0);
   return 0;
   }


