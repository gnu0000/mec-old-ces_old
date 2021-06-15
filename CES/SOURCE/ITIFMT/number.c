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
 * Number.c
 * Mark Hampton
 * Charles Engelke
 * Copyright (C) 1989-1991 Info Tech, Inc.
 * 
 * This module provides number formatting.  This module has been adapted
 * from the FORMAT module of HighEst.
 */ 

#pragma check_stack(on)


#include "..\include\iti.h"
#include "..\include\itifmt.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"
#include "GnuMath.h"
#include "number.h"

#include <stdio.h>
#include <malloc.h>

static char szInfinityString [] = "?";
#define isdigit(c) ((c) >= '0' && (c) <= '9')


/* FormatNumber works only if the input string pData is in the format

      pData = [sign]{digit}['.'{digit}] | szInfinityString .

   where:
      sign = '+' | '-' .
      digit = '0'| '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' .


   The output string (pData) is formatted according to the string in
   szFormat.  This string must be in the form:

      szFormat = [kind] [comma] {dot} .
      kind     = '$' | '%' .
      comma    = ',' .
      dot      = '.' .

   The interpretation of the format is as follows:

      kind  - if omitted, the number should be formatted in the
                 minimum space possible, consistent with the other
                 format specifications.  If negative, a leading '-'
                 will be inserted.

              if '$', a dollar sign should be prepended to the output 
                 string, and negative numbers should be enclosed in
                 parentheses, rather than using a minus sign.  Positive
                 numbers will have a trailing space, for alignment.

              if '%', a number 100 times as great as the input will
                 be output, and a '%' will be appended to the output
                 string.  Negative numbers will have a leading '-'.

      comma - if present, the whole number portion will have commas
                 every third space inserted.

      dot   - if present, the number will have a decimal point, and
                 as many decimal places as there are dots.  The number
                 will be rounded to fit.  If omitted, there will be
                 no decimal place, and the number will be rounded to
                 fit.



   Examples:

      input            format      output           result
      ---------------  ----------  ---------------  ------
      "12.234567"      ""         "12"             TRUE
      "12.234567"      "."        "12.2"           TRUE
      "12.234567"      "..."      "12.235"         TRUE
      "-12.234567"     "..."      "-12.235"        TRUE
      "12.234567"      "$.."      "$12.23 "        TRUE
      "-12.234567"     "$.."      "($12.23)"       TRUE
      "12.234567"      "$"        "$12 "           TRUE
      "12.234567"      "%"        "1223%"          TRUE
      "12.234567"      "%.."      "1223.46%"       TRUE
      "-12.234567"     "%.."      "-1223.46%"      TRUE

      "123456.789"     ","        "123,457"        TRUE
      "123456.789"     ",...."    "123,456.7890"   TRUE
      "-123456.789"    ",.."      "-123,456.79"    TRUE

      "."              ""         "0"              TRUE
      "."              "%"        "0%"             TRUE

      "12junk"         ""         "12junk"         FALSE
*/




#define NORMAL  0
#define DOLLAR  1
#define PERCENT 2


static BOOL ParseInput (PSZ   pszInput, 
                        PSZ   *ppszWholeStart, 
                        PSZ   *ppszWholeEnd,
                        PSZ   *ppszFractionStart, 
                        PSZ   *ppszFractionEnd,
                        PBOOL pbNegative);

static BOOL ParseFormatInfo (PSZ     pszFormatInfo, 
                             PUSHORT pusFormat, 
                             PBOOL   pbCommas, 
                             PUSHORT pusDecimals);

static void CleanUpRounding (PSZ pData, USHORT usPos);



/*
 * CopyAndStrip copies string pszSource to pszGivenDest (of size usDestMax), 
 * and strips (deletes) all cStrip characters.
 */

static void CopyAndStrip2 (PSZ    pszGivenDest, 
                           PSZ    pszSource, 
                           USHORT usDestMax, 
                           UCHAR  ucStrip1,
                           UCHAR  ucStrip2)
   {
   while (usDestMax > 1 && *pszSource)
      {
      if (*pszSource != ucStrip1 && *pszSource != ucStrip2)
         {
         *pszGivenDest++ = *pszSource;
         usDestMax--;
         }
      pszSource++;
      }
   *pszGivenDest = '\0';
   }

/*
 * PrepareNumber copies pszSource to pszLocalDest.  All commas are stripped, and
 * exponential numbers are converted to fixed point numbers.  If a '%' is 
 * at the end of the string, the number is divided by 100, and the '%'
 * is removed.  All dollar signs are removed.
 *
 * 1/14/93 mdh: added code to strip leading spaces.  This was causing a
 * problem with import.
 *
 * 11/12/93 clf: added expression parsing.  Zero is returned on error.
 * Percent is handled in a more natural way (unary postfix operator).
 * fn now returns BOOL. return is FALSE on error.
 */

static BOOL PrepareNumber (PSZ    pszLocalDest, 
                           PSZ    pszSource,
                           USHORT usDestMax)
   {
//   PSZ pszExp;
//   PSZ pszPct;
//   double d;
//
//// 1/14/93 mdh: added this code to strip leading spaces
//   while (ItiCharIsSpace (*pszSource) && *pszSource)
//      pszSource++;
//
//   pszExp = ItiStrIChr (pszSource, 'e');
//   pszPct = ItiStrChr (pszSource, '%');
//   if (pszExp == NULL && pszPct == NULL)
//      {
//      CopyAndStrip2 (pszLocalDest, pszSource, usDestMax, ',', '$');
//      ItiClip (pszLocalDest, " \t\n\r");
//      }
//   else if (pszExp != NULL)
//      {
//      sscanf (pszSource, "%le", &d);
//      sprintf (pszLocalDest, "%lf", d);
//      }
//   else
//      {
//      sscanf (pszSource, "%le", &d);
//      sprintf (pszLocalDest, "%lf", d / (double) 100.0);
//      }
//
   BIG bgNum;
   CHAR szFmt[64] = "";
   PSZ  psz;
   USHORT i = 0;
   UINT  uiSize = 0;
   UINT  uiAvail = 0;
   UINT  uiMax = 0; 

   CopyAndStrip2 (pszLocalDest, pszSource, usDestMax, ',', '$');
   AToBIG (&bgNum, pszLocalDest);
   if (!MthValid (bgNum))
      {
      ItiStrCpy (pszLocalDest, "0", usDestMax);
      return FALSE;
      }

uiSize = stackavail();
uiAvail = _memavl();
uiMax = _memmax();
   sprintf (&szFmt[0], "%Lf", bgNum); /* MUST have a large enough buffer! */
uiSize = stackavail();    
uiAvail = _memavl();
uiMax = _memmax();


   psz = pszLocalDest;
   i = 0;
   do {
      *(psz + i) = szFmt[i];
      }while (++i < usDestMax);

   *(psz + (usDestMax - 1)) = '\0';

   return TRUE;
   } /* end of PrepareNumber */



USHORT EXPORT FmtNumber (PSZ     pszSource, 
                         PSZ     pszDestination, 
                         USHORT  usDestMax, 
                         PSZ     pszFormatInfo,
                         PUSHORT pusDestLength)
   {
   char     szCopy [256];
   PSZ      pszWholeStart;
   PSZ      pszWholeEnd;
   PSZ      pszFractionStart;
   PSZ      pszFractionEnd;
   PSZ      pszDecimal;
   PSZ      pszPos;
   PSZ      pszOldDest;
   USHORT   usFormat;
   BOOL     bCommas;
   USHORT   usDecimals;
   USHORT   i, usSize;
   USHORT   usPos;
   BOOL     bNegative;

   pszOldDest = pszDestination;
   if (!PrepareNumber (pszDestination, pszSource, usDestMax))
      return ITIFMT_BAD_DATA;

   ItiStrCpy (szCopy, pszDestination, sizeof szCopy);

   if (!ParseInput (szCopy, &pszWholeStart, &pszWholeEnd, 
                    &pszFractionStart, &pszFractionEnd, &bNegative))
      return ITIFMT_BAD_DATA;


   if (!ParseFormatInfo (pszFormatInfo, &usFormat, &bCommas, &usDecimals))
      return ITIFMT_FORMAT_ERROR;

   if (pszDestination[0] == (char) 0 || ItiStrCmp (pszDestination, szInfinityString) == 0)
      {
      /* copy the infinity string into the buffer -- mdh */
      ItiStrCpy (pszDestination, szInfinityString, usDestMax);

      /* pad the string to put it in the 1's position -- mdh */
      i = 0;
      if (usDecimals)
         i = usDecimals + 1;
      if (usFormat == DOLLAR || usFormat == PERCENT)
         i++;
      while (i--)
         ItiStrCat (pszDestination, " ", usDestMax);

      if (pusDestLength)
         *pusDestLength = ItiStrLen (pszDestination);

      return 0;
      }

   /* If a percent, "multiply" by 100. */
   /* First, add up to two trailing zeros, if needed. */

   if (usFormat == PERCENT)
      {
      while (1 > (pszFractionEnd - pszFractionStart))
         {
         pszFractionEnd++;
         *pszFractionEnd = '0';
         *(pszFractionEnd+1) = (char) 0;
         }

      /* Now, move the decimal place (if found), two spaces over. */

      pszDecimal = szCopy;
      while ((*pszDecimal != '.') && (*pszDecimal != (char) 0))
         pszDecimal++;
      if (*pszDecimal != (char) 0)
         {
         *pszDecimal = *(pszDecimal+1);
         *(pszDecimal+1) = *(pszDecimal+2);
         *(pszDecimal+2) = '.';
         }
      pszWholeEnd += 2;
      pszFractionStart += 2;
      }


   usPos = 0;

   if (bNegative)
      {
      if (usFormat == DOLLAR)
         pszDestination[usPos] = '(';
      else
         pszDestination[usPos] = '-';
      usPos++;
      }

   if (usFormat == DOLLAR)
      {
      pszDestination[usPos] = '$';
      usPos++;
      }

   if (pszWholeEnd < pszWholeStart)
      {
      pszDestination[usPos] = '0';
      usPos++;
      }
   else
      {
      pszDestination[usPos] = *pszWholeStart;
      usPos++;
      }

   for (pszPos=pszWholeStart+1; pszPos <= pszWholeEnd; pszPos++)
      {
      if (bCommas && (2 == ((pszWholeEnd-pszPos) % 3)))
         {
         pszDestination[usPos] = ',';
         usPos++;
         }
      pszDestination[usPos] = *pszPos;
      usPos++;
      }

   pszDestination[usPos] = '.';
   usPos++;

   for (i=1; i<=usDecimals+1; i++)
      {
      if (pszFractionStart <= pszFractionEnd)
         {
         pszDestination[usPos] = *pszFractionStart;
         pszFractionStart++;
         }
      else
         pszDestination[usPos] = '0';
      usPos++;
      }
   usPos--;

   if (usDecimals == 0)
      {
      if (pszDestination[usPos] >= '5')
         (pszDestination[usPos-2])++;
      usPos--;
      pszDestination[usPos] = (char) 0;
      CleanUpRounding (pszDestination, usPos-1);
      while (pszDestination[usPos] != (char) 0)
         usPos++;
      }
   else
      {
      if (pszDestination[usPos] >= '5')
         (pszDestination[usPos-1])++;
      pszDestination[usPos] = (char) 0;
      CleanUpRounding (pszDestination, usPos-1);
      while (pszDestination[usPos] != (char) 0)
         usPos++;
      }


   if (usFormat == PERCENT)
      {
      pszDestination[usPos] = '%';
      usPos++;
      }

   if (usFormat == DOLLAR)
      {
      if (bNegative)
         pszDestination[usPos] = ')';
      else
         pszDestination[usPos] = ' ';
      usPos++;
      }

   pszDestination[usPos] = (char) 0;

   while ((*pszDestination != '\0') && !isdigit (*pszDestination))
      pszDestination++;

   while (*pszDestination == '0' &&
          *(pszDestination+1) != '.' &&
          *(pszDestination+1) != ',' &&
          *(pszDestination+1) != '%' &&
          *(pszDestination+1) != '\0')
      {
      usSize = ItiStrLen (pszDestination);
      for (i=0; i < usSize; i++)
         pszDestination [i] = pszDestination [i+1];
      }

   if (pusDestLength)
      *pusDestLength = ItiStrLen (pszOldDest);

   return 0;
   }/* end of FmtNumber */




static BOOL ParseInput (PSZ   pszInput, 
                        PSZ   *ppszWholeStart, 
                        PSZ   *ppszWholeEnd,
                        PSZ   *ppszFractionStart, 
                        PSZ   *ppszFractionEnd,
                        PBOOL pbNegative)
   {
   /* Parse sign */
   *pbNegative = FALSE;
   if (*pszInput == '-')
      {
      *pbNegative = TRUE;
      pszInput++;
      }
   else if (*pszInput == '+')
      {
      *pbNegative = FALSE;
      pszInput++;
      }

   /* Parse digits, and remember where they start and end. */
   *ppszWholeStart = pszInput;
   while (isdigit (*pszInput))
      pszInput++;
   *ppszWholeEnd = pszInput-1;

   /* Parse fractional part, if present. */
   if (*pszInput == '.')
      {
      pszInput++;
      *ppszFractionStart = pszInput;
      while (isdigit (*pszInput))
         pszInput++;
      *ppszFractionEnd = pszInput-1;
      }
   else
      {
      *ppszFractionStart = pszInput;
      *ppszFractionEnd = pszInput - 1;
      }

   /* Finally, check for being at end of string. */
   if (*pszInput != (char) 0 && ItiStrCmp (pszInput, szInfinityString) != 0)
      return FALSE;

   return TRUE;
   }




static BOOL ParseFormatInfo (PSZ     pszFormatInfo, 
                             PUSHORT pusFormat, 
                             PBOOL   pbCommas, 
                             PUSHORT pusDecimals)
   {
   if (*pszFormatInfo == '$')
      {
      *pusFormat = DOLLAR;
      pszFormatInfo++;
      }
   else if (*pszFormatInfo == '%')
      {
      *pusFormat = PERCENT;
      pszFormatInfo++;
      }
   else
      *pusFormat = NORMAL;

   if (*pbCommas = (*pszFormatInfo == ','))
      {
      pszFormatInfo++;
      }

   *pusDecimals = 0;
   while (*pszFormatInfo == '.')
      {
      (*pusDecimals)++;
      pszFormatInfo++;
      }

   if (*pszFormatInfo != (char) 0)
      return FALSE;

   return TRUE;
   }


static void CleanUpRounding (PSZ pszData, USHORT usPos)
   {
   SHORT i;
   SHORT sNextDigit;

   /* Check to see if the flagged digit has been rounded out of
      all recognition. */
   if (isdigit (pszData [usPos]))
      return;

   if (pszData [usPos] == (1+'9'))
      {
      pszData[usPos] = '0';
      sNextDigit = usPos - 1;
      while (sNextDigit >= 0 && !isdigit (pszData [sNextDigit]))
         {
         sNextDigit--;
         }
      if (sNextDigit >= 0)
         {
         pszData [sNextDigit] += 1;
         CleanUpRounding (pszData, sNextDigit);
         }
      else
         {
         /* Insert a leading '1' at usPos */
         i = usPos;
         while (pszData [i] != '\0')
            i++;
         while ((long) i >= (long) usPos)
            {
            pszData [i+1] = pszData [i];
            i--;
            }
         pszData [usPos] = '1';

         /* If needed, insert a comma after the leading '1' */
         i = usPos;
         while (pszData[i] != '\0')
            i++;
         if ((SHORT)(usPos+4) < i && pszData [usPos+4] == ',')
            {
            while (i >= (SHORT)(usPos+1))
               {
               pszData [i+1] = pszData [i];
               i--;
               }
            pszData [usPos+1] = ',';
            }
         }
      }
   }







/*********************************************************************/

USHORT EXPORT CheckNumber (PSZ     pszSource, 
                           PSZ     pszDest, 
                           USHORT  usDestMax, 
                           PSZ     pszFormatInfo,
                           PUSHORT pusDestLength)
   {
   char     szCopy [256];
   PSZ      pszWholeStart;
   PSZ      pszWholeEnd;
   PSZ      pszFractionStart;
   PSZ      pszFractionEnd;
   PSZ      pszPos;
   PSZ      pszOldDest;
   USHORT   usFormat;
   BOOL     bCommas;
   USHORT   usDecimals;
   USHORT   i, usSize;
   USHORT   usPos;
   BOOL     bNegative;

   pszOldDest = pszDest;

   if (!PrepareNumber (pszDest, pszSource, usDestMax))
      return ITIFMT_BAD_DATA;

   ItiStrCpy (szCopy, pszDest, sizeof szCopy);
   if (*pszDest == '\0')
      {
      ItiStrCpy (pszDest, "''", usDestMax);
      return 0;
      }

   if (!ParseInput (szCopy, &pszWholeStart, &pszWholeEnd, 
                    &pszFractionStart, &pszFractionEnd, &bNegative))
      return ITIFMT_BAD_DATA;


   if (!ParseFormatInfo (pszFormatInfo, &usFormat, &bCommas, &usDecimals))
      return ITIFMT_FORMAT_ERROR;

   if (pszDest[0] == (char) 0 || ItiStrCmp (pszDest, szInfinityString) == 0)
      {
      /* copy the null string into the buffer -- mdh */
      *pszDest = '\0';

      if (pusDestLength)
         *pusDestLength = ItiStrLen (pszDest);

      return 0;
      }


   usPos = 0;

   if (bNegative)
      {
      pszDest[usPos] = '-';
      usPos++;
      }

   if (usFormat == DOLLAR)
      {
      pszDest[usPos] = '$';
      usPos++;
      }

   if (pszWholeEnd < pszWholeStart)
      {
      pszDest[usPos] = '0';
      usPos++;
      }
   else
      {
      pszDest[usPos] = *pszWholeStart;
      usPos++;
      }

   for (pszPos=pszWholeStart+1; pszPos <= pszWholeEnd; pszPos++)
      {
      pszDest[usPos] = *pszPos;
      usPos++;
      }

   pszDest[usPos] = '.';
   usPos++;

   for (i=1; i<=usDecimals+1; i++)
      {
      if (pszFractionStart <= pszFractionEnd)
         {
         pszDest[usPos] = *pszFractionStart;
         pszFractionStart++;
         }
      else
         pszDest[usPos] = '0';
      usPos++;
      }
   usPos--;

   if (usDecimals == 0)
      {
      if (pszDest[usPos] >= '5')
         (pszDest[usPos-2])++;
      usPos--;
      pszDest[usPos] = (char) 0;
      CleanUpRounding (pszDest, usPos-1);
      while (pszDest[usPos] != (char) 0)
         usPos++;
      }
   else
      {
      if (pszDest[usPos] >= '5')
         (pszDest[usPos-1])++;
      pszDest[usPos] = (char) 0;
      CleanUpRounding (pszDest, usPos-1);
      while (pszDest[usPos] != (char) 0)
         usPos++;
      }

   pszDest[usPos] = (char) 0;

   while ((*pszDest != '\0') && !isdigit (*pszDest))
      pszDest++;

   while (*pszDest == '0' &&
          *(pszDest+1) != '.' &&
          *(pszDest+1) != ',' &&
          *(pszDest+1) != '%' &&
          *(pszDest+1) != '\0')
      {
      usSize = ItiStrLen (pszDest);
      for (i=0; i < usSize; i++)
         pszDest [i] = pszDest [i+1];
      }

   if (pusDestLength)
      *pusDestLength = ItiStrLen (pszOldDest);

   return 0;
   }


#pragma check_stack(off)

