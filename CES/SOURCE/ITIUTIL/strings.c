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
 * Strings.c
 * Charles Engelke
 * Mark Hampton
 * Craig Fitzgerald
 *
 * String handling functions for large data models.
 */


/*-----------------------------------------------------------------------+
|                                                                        |
|    Copyright (c) 1989 by Info Tech, Inc.  All Rights Reserved.         |
|                                                                        |
|    This program module is part of DS/Shell, a proprietary product of   |
|    Info Tech, Inc., no part of which may be reproduced or              |
|    transmitted in any form or by any means, electronic,                |
|    mechanical, or otherwise, including photocopying and recording      |
|    or in connection with any information storage or retrieval          |
|    system, without permission in writing from Info Tech, Inc.          |
|                                                                        |
+-----------------------------------------------------------------------*/

#define INCL_WINCOUNTRY
#include "..\include\iti.h"
#include "..\include\itiutil.h"





/*
 * ItiStrCpy copies pszSource to pszDest, storing up to 
 * usDestMax bytes. pszDest is always null terminated.
 *
 * Parameters: pszDest        A pointer to the destination string.
 *
 *             pszSource      A pointer to the string to copy.
 *
 *             usDestMax      The maximum number of bytes to store 
 *                            in the string pszDest.
 *
 * Return value: pszDest is always returned.
 *
 * Comments:  If pszSource contains more characters that pszDest
 * can contain, then the copy is aborted, with only usDestMax-1 
 * characters being copied to pszDest.
 */

PSZ EXPORT ItiStrCpy (PSZ pszDest, PSZ pszSource, USHORT usDestMax)
   {
   USHORT   i;

   if ((pszSource == NULL) || (pszDest == NULL)
       || (*pszSource == '\0'))  //added sept 95
      {
      if (pszDest != NULL)
         *pszDest = '\0';
      else
         pszDest = NULL;

      return pszDest;
      }

   for (i=0; i < usDestMax && pszSource [i] != '\0'; i++)
      {
      pszDest [i] = pszSource [i];
      }
   i = (i > usDestMax - 1) ? usDestMax - 1 : i;
   pszDest [i] = '\0';
   return pszDest;
   }





/*
 * ItiStrCat concatenates pszSource to string pszDest, storing at
 * most usDestMax characters in pszDest.
 *
 * Parameters: pszDest        A pointer to the destination string.
 *
 *             pszSource      A pointer to the string to copy.
 *
 *             usDestMax      The maximum number of bytes to store 
 *                            in the string pszDest.
 *
 * Return value: pszDest is always returned.
 *
 * Comments: pszDest is always null terminated.
 */


PSZ EXPORT ItiStrCat (PSZ pszDest, PSZ pszSource, USHORT usDestMax)
   {
   PSZ      pszReturnValue;
   USHORT   usDestLen;

   if (pszSource == NULL)
      return pszDest;

   pszReturnValue = pszDest;

   usDestLen = ItiStrLen (pszDest);
   pszDest += usDestLen;
   while (usDestLen < usDestMax && *pszSource != '\0')
      {
      *pszDest++ = *pszSource++;
      usDestLen++;
      }

   if (usDestLen == usDestMax)
      pszDest--;

   *pszDest = '\0';
   return pszReturnValue;
   }



   
   
/*
 * ItiStrNCat concatenates up to usCopyMax characters from pszSource to 
 * string pszDest, storing at most usDestMax characters in pszDest.
 *
 * Parameters: pszDest        A pointer to the destination string.
 *
 *             pszSource      A pointer to the string to copy.
 *
 *             usCopyMax      The maximum number of characters to copy from
 *                            pszSource.
 *
 *             usDestMax      The maximum number of bytes to store 
 *                            in the string pszDest.
 *
 * Return value: pszDest is always returned.
 *
 * Comments: pszDest is always null terminated.
 */


PSZ EXPORT ItiStrNCat (PSZ    pszDest, 
                       PSZ    pszSource, 
                       USHORT usCopyMax,
                       USHORT usDestMax)
   {
   PSZ      pszReturnValue;
   USHORT   usDestLen;

   pszReturnValue = pszDest;

   usDestLen = ItiStrLen (pszDest);
   pszDest += usDestLen;
   while (usDestLen < usDestMax && *pszSource != '\0' && usCopyMax--)
      {
      *pszDest++ = *pszSource++;
      usDestLen++;
      }

   if (usDestLen == usDestMax)
      pszDest--;

   *pszDest = '\0';
   return pszReturnValue;
   }


   
   

/*
 * ItiStrChr returns a pointer to the first occurance of character c
 * in string pszStr.
 *
 * Parameters: pszString      A pointer to the string to search.
 *
 *             c              The character to search for.
 *
 * Return value: A pointer to the character c in the string if it is 
 * found, or NULL if it isn't.
 */

PSZ EXPORT ItiStrChr (PSZ pszString, UCHAR c)
   {
   while (*pszString && *pszString != c)
      pszString++;
   return *pszString == c ? pszString : NULL;
   }



/*
 * ItiStrRChr returns a pointer to the last occurance of character c
 * in string pszStr.
 *
 * Parameters: pszString      A pointer to the string to search.
 *
 *             c              The character to search for.
 *
 * Return value: A pointer to the character c in the string if it is 
 * found, or NULL if it isn't.
 */

PSZ EXPORT ItiStrRChr (PSZ pszString, UCHAR c)
   {
   PSZ pszEnd;

   pszEnd = pszString + ItiStrLen (pszString);
   while (pszEnd > pszString && *pszEnd != c)
      pszEnd--;
   return *pszEnd == c ? pszEnd : NULL;
   }






/*
 * ItiStrChr returns a pointer to the first occurance of character c
 * in string pszStr.
 *
 * Parameters: pszString      A pointer to the string to search.
 *
 *             c              The character to search for.
 *
 * Return value: A pointer to the character c in the string if it is 
 * found, or NULL if it isn't.
 */

PSZ EXPORT ItiStrIChr (PSZ pszString, UCHAR c)
   {
   c = ItiCharLower (c);
   while (*pszString && ItiCharLower (*pszString) != c)
      pszString++;
   return (ItiCharLower (*pszString) == c) ? pszString : NULL;
   }




/*
 * ItiStrCmp compares string pszStr1 to string pszStr2.
 *
 * Parameters: pszStr1     The first string to compare.
 *
 *             pszStr2     The second string to compare.
 *
 * Return value:  
 *    -1 if pszStr1 <  pszStr2
 *     0 if pszStr1 == pszStr2
 *     1 if pszStr1 >  pszStr2
 *
 * Comments: The characters are treated as unsigned characters.
 */

SHORT EXPORT ItiStrCmp (PSZ pszStr1, PSZ pszStr2)
   {
   while ('\0' != *pszStr1 && '\0' != *pszStr2 && *pszStr1 == *pszStr2)
      {
      pszStr1++;
      pszStr2++;
      }

   return ((*pszStr1 - *pszStr2) == 0) ? 0 : 
          (((*pszStr1 - *pszStr2)  < 0) ? -1 : 1);
   }







/***********************************************************************
 * Can we get away with a NULL hab to WinUpperChar?????
 ***********************************************************************/


#define toupper(c) (WinUpperChar (NULL, 0, 0, (c)))

#define tolower(c) (((c) >= 'A' && (c) <= 'Z') ? ((c) ^ (char)0x20) : (c))

/*
 * ItiStrICmp performs a case insignificant string compare.
 *
 * Parameters: pszStr1     The first string to compare.
 *
 *             pszStr2     The second string to compare.
 *
 * Return value:  
 *    -1 if pszStr1 <  pszStr2
 *     0 if pszStr1 == pszStr2
 *     1 if pszStr1 >  pszStr2
 *
 * Comments: The characters are treated as unsigned characters.
 */

SHORT EXPORT ItiStrICmp (PSZ pszStr1, PSZ pszStr2)
   {
   if ((NULL == pszStr1) || (NULL == pszStr2))
      return 13;

   while ('\0' != *pszStr1 && 
          '\0' != *pszStr2 && 
          toupper (*pszStr1) == toupper (*pszStr2))
      {
      pszStr1++;
      pszStr2++;
      }

   return ((*pszStr1 - *pszStr2) == 0) ? 0 : 
          (((*pszStr1 - *pszStr2)  < 0) ? -1 : 1);
   }






/*
 * ItiStrNCmp performs a string compare on at most usMaxCmp characters.
 *
 * Parameters: pszStr1     The first string to compare.
 *
 *             pszStr2     The second string to compare.
 *
 *             usMaxCmp    The maximum number of characters to compare.
 *
 * Return value:  
 *    -1 if pszStr1 <  pszStr2
 *     0 if pszStr1 == pszStr2
 *     1 if pszStr1 >  pszStr2
 *
 * Comments: The characters are treated as unsigned characters.
 */

USHORT EXPORT ItiStrNCmp (PSZ pszStr1, PSZ pszStr2, USHORT usMaxCmp)
   {
   if ((NULL == pszStr1) || (NULL == pszStr2))
      return 13;

   while (*pszStr1 == *pszStr2 && 
          '\0' != *pszStr1     && 
          '\0' != *pszStr2     && 
          --usMaxCmp)
      {
      pszStr1++;
      pszStr2++;
      }

   return ((*pszStr1 - *pszStr2) == 0) ? 0 : 
          (((*pszStr1 - *pszStr2)  < 0) ? -1 : 1);

/* **************************************************************
   LOOK AT THIS: IS THIS WRONG?  If so, we need to fix HighEst, Install,
   and the HighEst Customization DLL. */
//   return (usMaxCmp ? *pszStr1 - *pszStr2 : 0);
   }




/*
 * ItiStrNICmp performs a string compare on at most usMaxCmp characters.
 *
 * Parameters: pszStr1     The first string to compare.
 *
 *             pszStr2     The second string to compare.
 *
 *             usMaxCmp    The maximum number of characters to compare.
 *
 * Return value:  
 *    -1 if pszStr1 <  pszStr2
 *     0 if pszStr1 == pszStr2
 *     1 if pszStr1 >  pszStr2
 *
 * Comments: The characters are treated as unsigned characters.
 */

USHORT EXPORT ItiStrNICmp (PSZ pszStr1, PSZ pszStr2, USHORT usMaxCmp)
   {
   if ((NULL == pszStr1) || (NULL == pszStr2))
      return 13;

   while (tolower (*pszStr1) == tolower (*pszStr2) && 
          '\0' != *pszStr1     && 
          '\0' != *pszStr2     && 
          --usMaxCmp)
      {
      pszStr1++;
      pszStr2++;
      }

   return ((tolower (*pszStr1) - tolower (*pszStr2)) == 0) ? 0 : 
          (((tolower (*pszStr1) - tolower (*pszStr2))  < 0) ? -1 : 1);

/* **************************************************************
   LOOK AT THIS: IS THIS WRONG?  If so, we need to fix HighEst, Install,
   and the HighEst Customization DLL. */
//   return (usMaxCmp ? *pszStr1 - *pszStr2 : 0);
   }



/*
 * ItiStrLen returns the length of string pszString.
 *
 * Parameters: pszString      A pointer to a string.
 *
 * Return Value: The length of the string, not including the null 
 * terminator.
 */

USHORT EXPORT ItiStrLen (PSZ pszString)
   {
   USHORT      i;

   if (pszString == NULL)
      return 0;

   for (i=0; '\0' != *pszString; i++)
      pszString++;

   return i;
   }






/*
 * ItiStrIsCharInString returns TRUE if ucChar is in the string pszString.
 *
 * Parameters: ucChar      The character to look for.
 *
 *             pszString   The string to search.
 *
 * Return Value: TRUE if ucChar is in the string pszString, or
 * FALSE if not.
 */

BOOL EXPORT ItiStrIsCharInString (UCHAR ucChar, PSZ pszString)
   {
   if (NULL == pszString)
      return FALSE;

   while ('\0' != *pszString && ucChar != *pszString)
      pszString++;

   return ucChar == *pszString;
   }





/* 
 * ItiStrSkipSpace skips all space characters (' ', '\t', '\n', and '\r') and 
 * returns the address of the first non space character in pszString.
 *
 * Parameters: pszString      The string to process.
 *
 * Return value: a pointer to then next non-white-space character.
 */

PSZ EXPORT ItiStrSkipSpace (PSZ pszString)
   {
   if (NULL == pszString)
      return NULL;

   while (*pszString && ItiStrIsCharInString (*pszString, " \t\n\r"))
      pszString++;

   return pszString;
   }




   
   
   
/* 
 * ItiStrSkipSpace skips all space characters (' ', '\t', '\n', and '\r') and 
 * returns the address of the first non space character in pszString.
 *
 * Parameters: pszString      The string to process.
 *
 * Return value: a pointer to then next non-white-space character.
 */

PSZ EXPORT ItiStrSkipNonspace (PSZ pszString)
   {  
   while (*pszString && !ItiStrIsCharInString (*pszString, " \t\n\r"))
      pszString++;

   return pszString;
   }









/* 
 * ItiStrToSHORT converts the given string into SHORT. The function
 * will stop converting at the first non digit character.  Leading 
 * whitespace is skiped.  *psNumber is 0 if overflow occurs.
 *
 * Parameters: pszNumber      A pointer to a string that contains an
 *                            ASCII number.
 *
 *             psNumber       A pointer to a SHORT to receive the
 *                            number.
 *
 * Return value: TRUE if the number was converted, FALSE if overflow.
 *
 * Comments:
 *   Valid numbers are:
 *     [whitespace]['-' | '+'][digits]
 *
 * Note that if no digits are found in the string, 0 is returned.
 */

BOOL EXPORT ItiStrToSHORT (PSZ pszNumber, PSHORT psNumber)
   {
   LONG  lNumber;
   SHORT iSign;

   pszNumber = ItiStrSkipSpace (pszNumber);
   
   /* get the sign (if any) */
   switch (*pszNumber)
      {
      case '-':
         iSign = -1;
         pszNumber++;
         break;

      case '+':
         iSign = 1;
         pszNumber++;
         break;

      default:
         iSign = 1;
         break;
      }

   for (lNumber=0; '0' <= *pszNumber && *pszNumber <= '9'; pszNumber++)
      {
      lNumber = lNumber * 10 + (*pszNumber - '0');
      }

   if (lNumber > MAX_SHORT)
      {
      *psNumber = 0;
      return FALSE;
      }

   *psNumber = ((SHORT) lNumber) * iSign;
   return TRUE;
   }





/* 
 * ItiStrToUSHORT converts the given string into USHORT. The function
 * will stop converting at the first non digit character.  Leading 
 * whitespace is skiped.  *psNumber is 0 if overflow occurs.
 *
 * Parameters: pszNumber      A pointer to a string that contains an
 *                            ASCII number.
 *
 *             psNumber       A pointer to a USHORT to receive the
 *                            number.
 *
 * Return value: TRUE if the number was converted, FALSE if overflow.
 *
 * Comments:
 *   Valid numbers are:
 *     [whitespace]['+'][digits]
 *
 * Note that if no digits are found in the string, 0 is returned.
 */

BOOL EXPORT ItiStrToUSHORT (PSZ pszNumber, PUSHORT pusNumber)
   {
   ULONG ulNumber;

   pszNumber = ItiStrSkipSpace (pszNumber);
   
   if (*pszNumber == '+')
      pszNumber++;

   for (ulNumber=0; '0' <= *pszNumber && *pszNumber <= '9'; pszNumber++)
      {
      ulNumber = ulNumber * 10 + (*pszNumber - '0');
      }

   if (ulNumber > MAX_USHORT)
      {
      *pusNumber = 0;
      return FALSE;
      }

   *pusNumber = (USHORT) ulNumber;
   return TRUE;
   }





/* 
 * ItiStrToLONG converts the given string into LONG. The function
 * will stop converting at the first non digit character.  Leading 
 * whitespace is skiped.  *plNumber is 0 if overflow occurs.
 *
 * Parameters: pszNumber      A pointer to a string that contains an
 *                            ASCII number.
 *
 *             plNumber       A pointer to a LONG to receive the
 *                            number.
 *
 * Return value: TRUE if the number was converted, FALSE if overflow.
 *
 * Comments:
 *   Valid numbers are:
 *     [whitespace]['-' | '+'][digits]
 *
 * Note that if no digits are found in the string, 0 is returned.
 */

BOOL EXPORT ItiStrToLONG (PSZ pszNumber, PLONG plNumber)
   {
   LONG  lNumber, lPrevious;
   SHORT iSign;
   BOOL  bOverFlow;

   pszNumber = ItiStrSkipSpace (pszNumber);
   
   /* get the sign (if any) */
   switch (*pszNumber)
      {
      case '-':
         iSign = -1;
         pszNumber++;
         break;

      case '+':
         iSign = 1;
         pszNumber++;
         break;

      default:
         iSign = 1;
         break;
      }

   bOverFlow = FALSE;
   lPrevious = 0;
   for (lNumber=0; '0' <= *pszNumber && *pszNumber <= '9' && !bOverFlow; pszNumber++)
      {
      lNumber = lNumber * 10 + (*pszNumber - '0');
      if (lNumber < lPrevious)
         bOverFlow = TRUE;
      else
         lPrevious = lNumber;
      }

   if (bOverFlow)
      {
      *plNumber = 0;
      return FALSE;
      }

   *plNumber = lNumber * iSign;
   return TRUE;
   }



/* 
 * ItiStrToULONG converts the given string into ULONG. The function
 * will stop converting at the first non digit character.  Leading 
 * whitespace is skiped.  *plNumber is 0 if overflow occurs.
 *
 * Parameters: pszNumber      A pointer to a string that contains an
 *                            ASCII number.
 *
 *             plNumber       A pointer to a ULONG to receive the
 *                            number.
 *
 * Return value: TRUE if the number was converted, FALSE if overflow.
 *
 * Comments:
 *   Valid numbers are:
 *     [whitespace]['+'][digits]
 *
 * Note that if no digits are found in the string, 0 is returned.
 */

BOOL EXPORT ItiStrToULONG (PSZ pszNumber, PULONG pulNumber)
   {
   ULONG ulNumber, ulPrevious;
   BOOL  bOverFlow;

   pszNumber = ItiStrSkipSpace (pszNumber);

   if (*pszNumber == '+')
      pszNumber++;

   bOverFlow = FALSE;
   ulPrevious = 0;
   for (ulNumber=0; '0' <= *pszNumber && *pszNumber <= '9' && !bOverFlow; pszNumber++)
      {
      ulNumber = ulNumber * 10 + (*pszNumber - '0');
      if (ulNumber < ulPrevious)
         bOverFlow = TRUE;
      else
         ulPrevious = ulNumber;
      }

   if (bOverFlow)
      {
      *pulNumber = 0;
      return FALSE;
      }

   *pulNumber = ulNumber;
   return TRUE;
   }





/*
 * ItiStrUpper converts the given string to all upper case letters,
 * in place.
 *
 * Parameters: pszString      The string to convert.
 * 
 * Return Value: pszString is always returned.
 */

PSZ EXPORT ItiStrUpper (PSZ pszString)
   {
   WinUpper (NULL, 0, 0, pszString);
   return pszString;
   }





/*
 * ItiStrLower converts the given string to all lower case letters,
 * in place.
 *
 * Parameters: pszString      The string to convert.
 * 
 * Return Value: pszString is always returned.
 */

PSZ EXPORT ItiStrLower (PSZ pszString)
   {
   PSZ pszTemp;

   for (pszTemp = pszString; *pszTemp; pszTemp++)
      *pszTemp = tolower (*pszTemp);

   return pszString;
   }






/*
 * ItiStrUSHORTToString converts a USHORT to a string.
 *
 * Parameters: usValue     The number to convert.
 *
 *             pszDest     The destination buffer.
 *
 *             usDestMax   The maximum number of characters (including
 *                         the null terminator) to store in pszDest.
 *
 * Return Value: The number of characters to stored in pszDest (excluding
 * the null terminator).
 */

USHORT EXPORT ItiStrUSHORTToString (USHORT usValue, 
                                    PSZ    pszDest, 
                                    USHORT usDestMax)
   {
   USHORT usNumChars = 0;
   USHORT i;
   char   c;

   if (usDestMax <= 1)
      if (usDestMax == 0)
         return 0;
      else
         return *pszDest = '\0';

   do
      {
      *pszDest++ = (char) (usValue % 10) + (char) '0';
      usValue /= 10;
      usNumChars++;
      usDestMax--;
      } while (usValue > 0 && usDestMax > 1);

   *pszDest = '\0';
   pszDest -= usNumChars;

   /* reverse the string */
   for (i=0; i < usNumChars / 2; i++)
      {
      c = *(pszDest + i);
      *(pszDest + i) = *(pszDest + usNumChars - i - 1);
      *(pszDest + usNumChars - i - 1) = c;
      }

   return usNumChars;
   }


/*
 * ItiStrSHORTToString converts a SHORT to a string.
 *
 * Parameters: sValue      The number to convert.
 *
 *             pszDest     The destination buffer.
 *
 *             usDestMax   The maximum number of characters (including
 *                         the null terminator) to store in pszDest.
 *
 * Return Value: The number of characters to stored in pszDest (excluding
 * the null terminator).
 */

USHORT EXPORT ItiStrSHORTToString (SHORT  sValue, 
                                   PSZ    pszDest, 
                                   USHORT usDestMax)
   {
   USHORT usNegative;

   if (usDestMax == 0)
      return 0;

   usNegative = 0;
   if (sValue < 0)
      {
      *pszDest++ = '-';
      usDestMax--;
      sValue = -sValue;
      usNegative = 1;
      }

   return ItiStrUSHORTToString (sValue, pszDest, usDestMax) + usNegative;
   }



/*
 * ItiStrULONGToString converts a ULONG to a string.
 *
 * Parameters: usValue     The number to convert.
 *
 *             pszDest     The destination buffer.
 *
 *             usDestMax   The maximum number of characters (including
 *                         the null terminator) to store in pszDest.
 *
 * Return Value: The number of characters to stored in pszDest (excluding
 * the null terminator).
 */

USHORT EXPORT ItiStrULONGToString (ULONG  ulValue, 
                                   PSZ    pszDest, 
                                   USHORT usDestMax)
   {
   USHORT usNumChars = 0;
   USHORT i;
   char   c;

   if (usDestMax <= 1)
      if (usDestMax == 0)
         return 0;
      else
         return *pszDest = '\0';

   do
      {
      *pszDest++ = (char) (ulValue % 10) + (char) '0';
      ulValue /= 10;
      usNumChars++;
      usDestMax--;
      } while (ulValue > 0 && usDestMax > 1);

   *pszDest = '\0';
   pszDest -= usNumChars;

   /* reverse the string */
   for (i=0; i < usNumChars / 2; i++)
      {
      c = *(pszDest + i);
      *(pszDest + i) = *(pszDest + usNumChars - i - 1);
      *(pszDest + usNumChars - i - 1) = c;
      }

   return usNumChars;
   }


/*
 * ItiStrLONGToString converts a LONG to a string.
 *
 * Parameters: sValue      The number to convert.
 *
 *             pszDest     The destination buffer.
 *
 *             usDestMax   The maximum number of characters (including
 *                         the null terminator) to store in pszDest.
 *
 * Return Value: The number of characters to stored in pszDest (excluding
 * the null terminator).
 */

USHORT EXPORT ItiStrLONGToString (LONG   lValue, 
                                  PSZ    pszDest, 
                                  USHORT usDestMax)
   {
   USHORT usNegative;

   if (usDestMax == 0)
      return 0;

   usNegative = 0;
   if (lValue < 0)
      {
      *pszDest++ = '-';
      usDestMax--;
      lValue = -lValue;
      usNegative = 1;
      }

   return ItiStrULONGToString (lValue, pszDest, usDestMax) + usNegative;
   }




///* this proc forces 1st string to be iLength before concatenation,
//   truncating or padding with spaces where necessary            */
//
//PSZ lpstrhardcat (PSZ to_string, PSZ from_string, USHORT iLength)
//   {
//   PSZ    temp_string;
//
//   for(temp_string = to_string;
//       ('\0' != *to_string) && (iLength != 0);
//       to_string++, iLength--);
//      {
//      }
//      *to_string++ = ' ';             /** extra blank for good measure ! **/
//   for(; iLength != 0; iLength--)
//      {
//      *to_string++ = ' ';
//      }
//   for(; '\0' != (*to_string = *from_string)  ; to_string++, from_string++);
//      {
//      }
//   return temp_string;
//   }
//


// THE FOLLOWING USES OTHER LIBRARIES
// --------------------------------------------------
//
///*
// * This procedure duplicates the string using hheap
// */
//PSZ EXPORT ItiDupStr(PSZ pszSourceStr, HHEAP hheap)
//   {
//   USHORT i;
//   USHORT uStringLen;
//   PSZ pszDestStr;
//
//   uStringLen = strlen(pszSourceStr) + 1;
//   pszDestStr = ItiMemAlloc(hheap, uStringLen, 0);
//   i = 0;
//   while (pszSourceStr[i] != '\0')
//      {
//      pszDestStr[i] = pszSourceStr[i];
//      i++;
//      }
//   if (pszSourceStr[i] == '\0')
//      pszDestStr[i] = '\0';
//   return(pszDestStr);
//   }
//
//
//
//
//
///*
// * Removes trailing chars from str
// */
//PSZ EXPORT Clip (PSZ pszStr, PSZ pszClipList)
//   {
//   SHORT i;
//
//   i = strlen (pszStr);
//   while (i >= 0 && strchr (pszClipList, pszStr[i]) != NULL)
//      pszStr[i--] = '\0';
//   return pszStr;
//   }
//
//
///*
// * Removes leading chars from str
// */
//PSZ EXPORT Strip (PSZ pszStr, PSZ pszStripList)
//   {
//   SHORT uLen, i = 0;
//
//   if (!(uLen = strlen (pszStr)))
//      return pszStr;
//   while (i < uLen && strchr (pszStripList, pszStr[i]) != NULL)
//      i++;
//   if (i)
//      memmove (pszStr, pszStr + i, uLen - i + 1);
//   return pszStr;
//   }
//




static PSZ apszBuffer [MAX_THREADS];
static PSZ apszNextBuffer [MAX_THREADS];




PSZ EXPORT ItiStrTok (PSZ pszString, PSZ pszTokenList)
   {
   TID       tidCurrent;
   SEL       selGis, selLis;
   PLINFOSEG plis;

   DosGetInfoSeg (&selGis, &selLis);
   plis = MAKEP (selLis, 0);
   tidCurrent = plis->tidCurrent - 1;
   if (tidCurrent >= MAX_THREADS)
      return NULL;

   if (NULL == pszString)
      apszBuffer [tidCurrent] = apszNextBuffer [tidCurrent];
   else
      apszBuffer [tidCurrent] = pszString;

   apszNextBuffer [tidCurrent] = apszBuffer [tidCurrent];


   if (NULL == apszBuffer [tidCurrent] || '\0' == *apszBuffer [tidCurrent])
      return NULL;

   while ('\0' != *apszBuffer [tidCurrent] && 
          ItiStrIsCharInString (*apszBuffer [tidCurrent], pszTokenList))
      apszBuffer [tidCurrent]++;

   apszNextBuffer [tidCurrent] = apszBuffer [tidCurrent];

   while ('\0' != *apszNextBuffer [tidCurrent] && 
          !ItiStrIsCharInString (*apszNextBuffer [tidCurrent], pszTokenList))
      apszNextBuffer [tidCurrent]++;

   if ('\0' != *apszNextBuffer [tidCurrent])
      {
      *apszNextBuffer [tidCurrent] = '\0';
      apszNextBuffer [tidCurrent]++;
      }

   return apszBuffer [tidCurrent];
   }




BOOL EXPORT ItiStrHexToUCHAR (PSZ pszHexString, PUCHAR pucOut)
   {
   UCHAR uc;
   
   *pucOut = '\0';
   if (!ItiCharAsciiToHex (*pszHexString, &uc))
      return FALSE;
   *pucOut = (UCHAR) (uc << 4);
   if (!ItiCharAsciiToHex (*(pszHexString+1), &uc))
      return FALSE;
   *pucOut |= uc;
   return TRUE;
   }



BOOL EXPORT ItiStrHexToULONG (PSZ pszNumber, PULONG pulNumber)
   {
   UCHAR uc;
   USHORT usNumBytes;

   if (*pszNumber == '0' && tolower (*(pszNumber+1)) == 'x')
      pszNumber += 2;

   for (usNumBytes = 0; usNumBytes < 4 && *pszNumber && *(pszNumber+1); 
        usNumBytes++)
      {
      if (!ItiStrHexToUCHAR (pszNumber, &uc))
         break;

      pszNumber += 2;
      *pulNumber = (*pulNumber << 8) | uc;
      }

   return *pszNumber == '\0' || usNumBytes == 4;
   }



USHORT EXPORT ItiStrULONGToHexString (LONG   ulValue, 
                                      PSZ    pszDest, 
                                      USHORT usDestMax)
   {
   USHORT usLen;
   USHORT usCount;

   usLen = 0;
   usCount = 0;
   while (usLen+2 < usDestMax && usCount < 4)
      {
      *pszDest = ItiCharMakeHex (((HIUCHAR (HIUSHORT (ulValue)) >> 4) & 0xf));
      pszDest++;
      *pszDest = ItiCharMakeHex (((HIUCHAR (HIUSHORT (ulValue))     ) & 0xf));
      pszDest++;
      ulValue <<= 8;
      usLen += 2;
      usCount++;
      }
   *pszDest = '\0';
   return usLen;
   }



USHORT EXPORT ItiStrUCHARToHexString (LONG   ucValue, 
                                      PSZ    pszDest, 
                                      USHORT usDestMax)
   {
   *pszDest = '\0';
   if (usDestMax < 3)
      return 0;

   *pszDest = ItiCharMakeHex (((ucValue >> 4) & 0xf));
   pszDest++;
   *pszDest = ItiCharMakeHex (((ucValue     ) & 0xf));
   pszDest++;
   *pszDest = '\0';
   return 2;
   }
