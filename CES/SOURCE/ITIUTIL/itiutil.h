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
 * ItiUtils.h
 * Charles Engelke
 * Mark Hampton
 * Craig Fitzgerald
 * Copyright (C) 1989, Info Tech, Inc.
 *
 * General utility functions.
 *
 * String API:
 *    ItiStrCpy
 *    ItiStrCat
 *    ItiStrChr
 *    ItiStrCmp
 *    ItiStrNCmp
 *    ItiStrICmp
 *    ItiStrLen
 *    ItiStrIsCharInString
 *    ItiStrSkipSpace
 *    ItiStrSkipNonspace
 *    ItiStrToSHORT
 *    ItiStrToUSHORT
 *    ItiStrToLONG
 *    ItiStrToULONG
 * 
 * Memory API:
 *    ItiMemBinSearch
 *    ItiMemQSort
 *    ItiMemCpy
 *    ItiMemRCpy
 *    ItiMemSwap
 *    ItiMemSet
 * 
 * CSV API:
 *    ItiCsvGetField
 *    ItiCsvMakeField
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



/**************************************************************************
 * Start of String API.
 **************************************************************************/



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

extern PSZ EXPORT ItiStrCpy (PSZ pszDest, PSZ pszSource, USHORT usDestMax);





/*
 * ItiStrCat concatenates pszString to string pszDest, storing at
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


extern PSZ EXPORT ItiStrCat (PSZ pszDest, PSZ pszString, USHORT usDestMax);



   
   
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

extern PSZ EXPORT ItiStrChr (PSZ pszString, UCHAR c);






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

extern SHORT EXPORT ItiStrCmp (PSZ pszStr1, PSZ pszStr2);




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

extern SHORT EXPORT ItiStrICmp (PSZ pszStr1, PSZ pszStr2);






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

extern USHORT ItiStrNCmp (PSZ pszStr1, PSZ pszStr2, USHORT usMaxCmp);






/*
 * ItiStrLen returns the length of string pszString.
 *
 * Parameters: pszString      A pointer to a string.
 *
 * Return Value: The length of the string, not including the null 
 * terminator.
 */

extern USHORT EXPORT ItiStrLen (PSZ pszString);






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

extern BOOL ItiStrIsCharInString (UCHAR ucChar, PSZ pszString);





/* 
 * ItiStrSkipSpace skips all space characters (' ', '\t', '\n', and '\r') and 
 * returns the address of the first non space character in pszString.
 *
 * Parameters: pszString      The string to process.
 *
 * Return value: a pointer to then next non-white-space character.
 */

extern PSZ EXPORT ItiStrSkipSpace (PSZ pszString);




   
   
   
/* 
 * ItiStrSkipSpace skips all space characters (' ', '\t', '\n', and '\r') and 
 * returns the address of the first non space character in pszString.
 *
 * Parameters: pszString      The string to process.
 *
 * Return value: a pointer to then next non-white-space character.
 */

extern PSZ EXPORT ItiStrSkipNonspace (PSZ pszString);






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

extern BOOL EXPORT ItiStrToSHORT (PSZ pszNumber, PSHORT psNumber);



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

extern BOOL EXPORT ItiStrToUSHORT (PSZ pszNumber, PUSHORT pusNumber);




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

extern BOOL EXPORT ItiStrToLONG (PSZ pszNumber, PLONG plNumber);


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

extern BOOL EXPORT ItiStrToULONG (PSZ pszNumber, PULONG pulNumber);




/**************************************************************************
 * End of String API.
 **************************************************************************/




/**************************************************************************
 * Start of memory API.
 **************************************************************************/


/*
 * ItiMemBinSearch performs a binary search on the array pvBase, looking for
 * a match of lpKey, using the compare function pCmpFunc.  nWidth is the
 * width in bytes of each element of the array, and nMax is the number of 
 * elements in the array.
 *
 *
 * Return Value: returns a pointer to the element that matches lpKey, or
 * NULL if no match found.
 */

extern PVOID EXPORT ItiMemBinSearch (PVOID   pvKey, 
                                     PVOID   pvBase, 
                                     USHORT  usMax, 
                                     USHORT  usWidth,
                                     SHORT   (*pfnCmpFunc) (PVOID pv1, PVOID pv2));




/*
 * ItiMemQSort performs a quick sort on the array pvArray. 
 * usElementSize is the size, in bytes, of each element of the array. 
 * usNumElements is the number of elements the array has. 
 * pfnCmpFunc is a compare fucntion.
 */

extern VOID EXPORT ItiMemQSort (PVOID    pvArray,
                                USHORT   usElementSize,
                                USHORT   usNumElements,
                                SHORT    (*pfnCmpFunc) (PVOID pv1, PVOID pv2));





/*
 * ItiMemCpy copies usSize bytes from pvSource to pvDest.  
 * The copy is performed in a forward copy, ie pvSource and pvDest
 * are both INCREMENTED for each byte copied. 
 *
 * Return Value: ItiMemCpy always returns pvDest.
 */

extern PVOID EXPORT ItiMemCpy (PVOID  pvDest, PVOID  pvSource, USHORT usSize);



/*
 * ItiMemRCpy copies usSize bytes from pvSource to pvDest.  
 * The copy is performed in a reverse copy, ie pvSource and pvDest
 * are both DECREMENTED for each byte copied. 
 *
 * Return Value: ItiMemCpy always returns pvDest.
 */

extern PVOID EXPORT ItiMemRCpy (PVOID  pvDest, PVOID  pvSource, USHORT usSize);





/*
 * ItiMemSet 
 */ 

extern PVOID EXPORT ItiMemSet (PVOID pvData, CHAR c, USHORT usNumBytes);






/*
 * ItiMemSwap swaps usSize bytes between pvBase + (usFirst * usSize) and
 * pvBase + (usSecond * usSize).  The swap is done in a forward direction.
 */

extern VOID EXPORT ItiMemSwap (PSZ     pvBase,
                               USHORT  usFirst,
                               USHORT  usSecond,
                               USHORT  usSize);


/**************************************************************************
 * End of memory API.
 **************************************************************************/



   /* Grammar for a CSVString is:
      CSVString      = Field {',' Field} .
      Field          = Empty | NonQuotedField | QuotedField .
      NonQuotedField = { NonQuoteComma } .
      QuotedField    = '"' { NonQuoteComma | ',' | '""' } '"' .
      Empty          = .
      NonQuoteComma  = anything other than '"' or ',' .
   */

extern PSZ EXPORT ItiCSVGetField (USHORT usField, 
                                  PSZ    pszCSVString, 
                                  PSZ    pszResult);



extern PSZ EXPORT ItiCSVMakeField (PSZ pszString, PSZ pszResult);
