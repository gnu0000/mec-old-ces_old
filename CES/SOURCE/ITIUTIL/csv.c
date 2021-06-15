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
 * CSV.C -- Comma-separated value parser and generator.
 * Charles Engelke
 * Modified by Mark Hampton to run under Windows
 */

#include "..\include\iti.h"
#include "..\include\itiutil.h"



static PSZ AdvanceAField (PSZ pszString);
static PSZ AdvanceQuotedField (PSZ pszString);
static void ConvertAField (PSZ pszString, PSZ pszResult, USHORT usResultSize);
//static void InsertChar (char c, PSZ pszString);



PSZ EXPORT ItiCSVGetField (USHORT usField, 
                           PSZ    pszCSVString, 
                           PSZ    pszResult,
                           USHORT usResultSize)
   {
   USHORT usFieldNumber;

   /* Grammar for pszCSVString is:
      pszCSVString     = Field {',' Field} .
      Field          = Empty | NonQuotedField | QuotedField .
      NonQuotedField = { NonQuoteComma } .
      QuotedField    = '"' { NonQuoteComma | ',' | '""' } '"' .
      Empty          = .
      NonQuoteComma  = anything other than '"' or ',' .
   */

   for (usFieldNumber = 1; usFieldNumber < usField; usFieldNumber++)
      {
      pszCSVString = AdvanceAField (pszCSVString);
      if (*pszCSVString != '\0')
         pszCSVString++;
      }

   ConvertAField (pszCSVString, pszResult, usResultSize);

   return pszResult;
   }





static PSZ AdvanceAField (PSZ pszString)
   {
   switch (*pszString)
      {
      case ',':
      case '\0':
         return pszString;
         break;

      case '"':
         return AdvanceQuotedField (pszString);
         break;

      default:
         while (*pszString != ',' && *pszString != '\0')
            pszString++;
         return pszString;
         break;
      }

   return pszString;
   }






static PSZ AdvanceQuotedField (PSZ pszString)
   {
   pszString++;

   while (TRUE)
      {
      switch (*pszString)
         {
         case '\0':
            return pszString;
            break;

         case '"':
            if (*(pszString+1) == '"')
               pszString += 2;
            else
               {
               pszString++;
               return pszString;
               }
            break;

         default:
            pszString++;
            break;
         }
      }
   return pszString;
   }



static void ConvertAField (PSZ pszString, PSZ pszResult, USHORT usResultSize)
   {
   switch (*pszString)
      {
      case ',':
      case '\0':
         *pszResult = '\0';
         return;
         break;

      case '"':
         pszString++;

         while (TRUE)
            {
            switch (*pszString)
               {
               case '\0':
                  *pszResult = '\0';
                  return;
                  break;

               case '"':
                  if (*(pszString+1) == '"')
                     {
                     pszString += 2;
                     *pszResult = '"';
                     pszResult++;
                     }
                  else
                     {
                     pszString++;
                     *pszResult = '\0';
                     return;
                     }
                  break;

               default:
                  *pszResult = *pszString;
                  pszString++;
                  pszResult++;
                  break;
               }
            }
         break;

      default:
         while (*pszString != ',' && *pszString != '\0')
            {
            *pszResult = *pszString;
            pszString++;
            pszResult++;
            }
         *pszResult = '\0';
         return;
         break;
      }
   }



PSZ EXPORT ItiCSVMakeField (PSZ pszInput, PSZ pszOutput, USHORT usOutputSize)
   {
   BOOL     bNeedsQuotes;
   USHORT   i;

   bNeedsQuotes = FALSE;

   i = 0;
   bNeedsQuotes = (ItiStrChr (pszInput, ',') != NULL) ||
                  (ItiStrChr (pszInput, '"') != NULL);

   if (!bNeedsQuotes)
      {
      ItiStrCpy (pszOutput, pszInput, usOutputSize);
      return pszOutput;
      }

   /* quote the string, and double the internal quotes */
   usOutputSize -= 2;
   pszOutput [0] = '"';
   for (i=1; i < usOutputSize && '\0' != *pszInput;)
      {
      pszOutput [i++] = *pszInput;
      if ('"' == *pszInput)
         pszOutput [i++] = *pszInput;

      pszInput++;
      }

   pszOutput [i] = '"';
   pszOutput [i+1] = '\0';

   return pszOutput;

//   BOOL bNeedsQuotes;
//   PSZ  pcChar;
//
//   bNeedsQuotes = FALSE;
//
//   ItiStrCpy (pszOutput, pszInput, usOutputSize);
//
//   for (pcChar = pszOutput; '\0' != *pcChar; pcChar++)
//      {
//      if ('"' == *pcChar)
//         {
//         InsertChar ('"', pcChar);
//         pcChar++;
//         bNeedsQuotes = TRUE;
//         }
//      if (',' == *pcChar)
//         bNeedsQuotes = TRUE;
//      }
//
//   if (bNeedsQuotes)
//      {
//      *pcChar = '"';
//      *(pcChar+1) = '\0';
//      InsertChar ('"', pszOutput);
//      }
//
//   return pszOutput;
   }



//static void InsertChar (char c, PSZ pszInput)
//   {
//   PSZ pszTemp;
//
//   pszTemp = pszInput;
//
//   while ('\0' != *pszTemp)
//      pszTemp++;
//
//   *(pszTemp+1) = '\0';
//
//   while (pszTemp >= pszInput)
//      {
//      *(pszTemp+1) = *pszTemp;
//      pszTemp--;
//      }
//
//   *pszInput = c;
//   }
//
//
//
//
//
