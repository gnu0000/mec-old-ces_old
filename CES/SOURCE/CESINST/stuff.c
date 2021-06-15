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
 * Stuff.c
 *
 * (C) 1992 Info Tech Inc.
 * This file is part the EBS module
 */

#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "stuff.h"



/***********************************************************************/
/****                                                               ****/
/***                       STRING FUNCTIONS                          ***/
/****                                                               ****/
/***********************************************************************/
//
//
//USHORT GetWord (PSZ *ppsz, ...
//USHORT EatWord (PSZ *ppszStr, PSZ pszTarget)
//PSZ ClipString (PSZ pszStr, PSZ pszClipList)
//PSZ StripString (PSZ pszStr, PSZ pszStripList)
//PSZ ExtractString (PSZ pszStr, PSZ pszExtractList)



/*
 * Eats a word from the front of a string and returns
 * that word and the rest of the string.
 *
 * 1> skip spaces and tabs
 * 2> read to delimeter character (exclusive)
 * 3> word ends at char before delim, string starts at char at/after delim
 * 4> returns word read and remaining string as parameters
 * 5> delimeter char is returns as fn val.
 *
 * returns -1 when there is nothing to return
 */
USHORT GetWord (PSZ *ppsz,
             PSZ pszWord,
             PSZ pszDelimeters,
             USHORT  bEatDelim)
   {
   *pszWord = '\0';
   StripString (*ppsz, " \t");

   if (*ppsz == NULL || **ppsz == '\0')
      return -1;

   while (**ppsz != '\0' && strchr (pszDelimeters, **ppsz) == NULL)
      *(pszWord++) = *((*ppsz)++);
   *pszWord = '\0';

   if (bEatDelim)
      return (int) (**ppsz ? *((*ppsz)++): 0);
   else
      return (int) (**ppsz ? *(*ppsz): 0);
   }



/*
 * 0 = match
 * 1 = no match
 * 2 = end of string
 */
USHORT EatWord (PSZ *ppszStr, PSZ pszTarget)
   {
   char szWord [512];

   if ((GetWord (ppszStr, szWord, " ,\t\n", 1)) == -1)
      return 2;
   return !!stricmp (pszTarget, szWord);
   }



/*
 * Removes trailing chars from str
 */
PSZ ClipString (PSZ pszStr, PSZ pszClipList)
   {
   int i;

   i = strlen (pszStr);
   while (i >= 0 && strchr (pszClipList, pszStr[i]) != NULL)
      pszStr[i--] = '\0';
   return pszStr;
   }



/*
 * Removes leading chars from str
 * Moves memory, not ptr;
 */
PSZ StripString (PSZ pszStr, PSZ pszStripList)
   {
   USHORT uLen, i = 0;

   if (!(uLen = strlen (pszStr)))
      return pszStr;
   while (i < uLen && strchr (pszStripList, pszStr[i]) != NULL)
      i++;
   if (i)
      memmove (pszStr, pszStr + i, uLen - i + 1);
   return pszStr;
   }



/*
 * Removes chars from anywhere in string
 *
 */
PSZ ExtractString (PSZ pszStr, PSZ pszExtractList)
   {
   PSZ pszPtrSrc, pszPtrDest;

   if (pszStr == NULL)
      return NULL;

   pszPtrSrc = pszPtrDest = pszStr;
   while (*pszPtrSrc != '\0')
      {
      if (strchr (pszExtractList, *pszPtrSrc) == NULL)
         *pszPtrDest++ = *pszPtrSrc;
      pszPtrSrc++;
      }
   *pszPtrDest = '\0';
   return pszStr;
   }


BOOL BlankLine (PSZ pszLine)
   {
   for (; *pszLine == ' ' || *pszLine == '\t'; pszLine++)
      ;
   return !*pszLine;
   }
