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
 *
 * General utility functions.
 *
 * Character MACROS:
 *    ItiCharLower      -- depends heavily on ASCII
 *    ItiCharUpper      -- depends heavily on ASCII
 *    ItiCharIsAlpha    -- depends heavily on ASCII
 *    ItiCharIsDigit    -- depends heavily on ASCII
 *    ItiCharIsAlphaNumeric -- depends heavily on ASCII
 *    ItiCharIsSpace
 *    ItiCharMakeHex
 *    ItiCharAsciiToHex
 *
 * String API:
 *    ItiStrCpy
 *    ItiStrCat
 *    ItiStrChr
 *    ItiStrRChr
 *    ItiStrIChr
 *    ItiStrCmp
 *    ItiStrNCmp
 *    ItiStrICmp
 *    ItiStrNICmp
 *    ItiStrLen
 *    ItiStrIsCharInString
 *    ItiStrSkipSpace
 *    ItiStrSkipNonspace
 *    ItiStrHexToULONG
 *    ItiStrHexToUCHAR
 *    ItiStrUCHARToHex
 *    ItiStrULONGToHex
 *    ItiStrToSHORT
 *    ItiStrToUSHORT
 *    ItiStrToLONG
 *    ItiStrToULONG
 *    ItiStrUpper
 *    ItiStrLower
 *    ItiStrSHORTToString
 *    ItiStrUSHORTToString
 *    ItiStrLONGToString
 *    ItiStrULONGToString
 *    ItiStrTok
 *    ItiStrMakePPSZ
 *    ItiStrIsBlank
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
 *
 *
 * General Util Procs:
 *    ItiStrMatchWildCard
 *    ItiStrIsWildCard
 *    ItiGetWord
 *    ItiEat
 *    ItiClip
 *    ItiStrip
 *    ItiTwoQuotes
 *    ItiSingleQuotes
 *    ItiStrDup
 *    ItiMemDup
 *    ItiFreeStrArray
 *    ItiGetProfileString
 *    ItiVerCmp
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



#ifndef MAX_USHORT

#define MAX_USHORT 0xffffU

#endif

#ifndef MAX_SHORT

#define MAX_SHORT 0x7fff

#endif


#ifndef CES_INIPRF

#define CES_INIPRF  "CES.INI"

#endif




/**************************************************************************
 * Start of Character MACROS.
 **************************************************************************/

#define ItiCharLower(c) (((c) >= 'A' && (c) <= 'Z') ? ((c) ^ (char)0x20) : (c))

#define ItiCharUpper(c) (((c) >= 'a' && (c) <= 'z') ? ((c) ^ (char)0x20) : (c))

#define ItiCharIsAlpha(c) (((c) >= 'a' && (c) <= 'z') || \
                           ((c) >= 'A' && (c) <= 'Z'))

#define ItiCharIsDigit(c) ((c) >= '0' && (c) <= '9')

#define ItiCharIsAlphaNumeric(c) (((c) >= 'a' && (c) <= 'z') || \
                                  ((c) >= 'A' && (c) <= 'Z') || \
                                  ((c) >= '0' && (c) <= '9'))

#define ItiCharIsSpace(c) ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r')

/* converts number from 0 to 15 to ASCII '0' - '9', 'A' - 'F' */

#define ItiCharMakeHex(c) (((c) <= 9) ? (UCHAR) ((c) + '0') : (UCHAR) (((c)-(UCHAR) 10) + 'A'))


extern BOOL EXPORT ItiCharAsciiToHex (CHAR c, PUCHAR puc);

/**************************************************************************
 * End of Character MACROS.
 **************************************************************************/


typedef struct
   {
   HFILE    hf;
   USHORT   usBufferSize;
   USHORT   usBufferPos;
   USHORT   usBufferEnd;
   CHAR     szBuffer[];
   } *PFILE;




typedef struct
   {
   PFILE    pf;         /* handle to the file */
   BOOL     bECSV;      /* TRUE if file is in ECSV format */
   BOOL     bHighEst;   /* TRUE if file is in HighEst format */
   ULONG    ulLine;     /* last line processed */
   ULONG    ulColumn;   /* last column looked at */
   ULONG    ulFilePos;  /* do not use -- for internal use only! */
   BOOL     bUnitSys;   /* TRUE if file contains a UnitSystem ID line */
   BOOL     bMetric;    /* TRUE if file UnitSystem is metric */
   } CSVFILEINFO;

typedef CSVFILEINFO *PCSVFILEINFO;


#define ERROR_COULD_NOT_OPEN     1
#define ERROR_NOT_CSV            2
#define ERROR_WRONG_VERSION      3





/**************************************************************************
 * Start of String API.
 **************************************************************************/

#define MAX_TOKEN_SIZE     64

/*
 * ItiStrReplaceParams copies the string pszSource to pszDest.  When a 
 * '%' followed by a string is encountered, the array apszTokens is 
 * searched.  If a match is found, then the corresponding element in 
 * apszValues replaces the '%' and the string.  If no match is found, 
 * the string is left as is.  If '%%' is encountered, no replacement is 
 * performed.
 *
 * Parameters: pszDest           The destination string.
 * 
 *             pszSource         The source string.
 * 
 *             usDestMax         The maximum number of characters to replace.
 * 
 *             apszTokens        The array of tokens.  None of these may be
 *                               a NULL pointer.  The tokens are case 
 *                               insignificant.  Tokens may consist only
 *                               of the following characters: A-Z, a-z, 0-9,
 *                               and '_'.  Only the first MAX_TOKEN_SIZE 
 *                               characters are significant for the tokens.
 * 
 *             apszValues        The array of replacement values.  These
 *                               may be NULL pointers.  (although 
 *                               apszValues itself may not be NULL).
 * 
 *             usNumValues       The number of elements in apszTokens and
 *                               apszValues.
 * Return Value:  The number of tokens not found in apszTokens.
 */

extern USHORT EXPORT ItiStrReplaceParams (PSZ    pszDest,
                                          PSZ    pszSource,
                                          USHORT usDestMax,
                                          PSZ    apszTokens [], 
                                          PSZ    apszValues [], 
                                          USHORT usNumValues);

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


extern PSZ EXPORT ItiStrCat (PSZ    pszDest, 
                             PSZ    pszSource, 
                             USHORT usDestMax);



/*
 * ItiStrNCat concatenates up to usCopyMax characters from pszString to 
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


extern PSZ EXPORT ItiStrNCat (PSZ    pszDest, 
                              PSZ    pszSource, 
                              USHORT usCopyMax,
                              USHORT usDestMax);


   
   
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

extern PSZ EXPORT ItiStrRChr (PSZ pszString, UCHAR c);





/*
 * ItiStrIChr returns a pointer to the first occurance of character c
 * in string pszStr.  c is case insignificant
 *
 * Parameters: pszString      A pointer to the string to search.
 *
 *             c              The character to search for.
 *
 * Return value: A pointer to the character c in the string if it is 
 * found, or NULL if it isn't.  c is case insignificant.
 */

extern PSZ EXPORT ItiStrIChr (PSZ pszString, UCHAR c);







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

extern USHORT EXPORT ItiStrNCmp (PSZ pszStr1, PSZ pszStr2, USHORT usMaxCmp);






/*
 * ItiStrNICmp performs a string compare on at most usMaxCmp characters.
 * The string comparison is case insignificant.
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

extern USHORT EXPORT ItiStrNICmp (PSZ pszStr1, PSZ pszStr2, USHORT usMaxCmp);






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

extern BOOL EXPORT ItiStrIsCharInString (UCHAR ucChar, PSZ pszString);





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
 * ItiStrToUCHAR converts the given string into UCHAR. The function
 * will stop converting at the first non hex digit character.  Leading 
 * whitespace is skiped.  *pulNumber is 0 if overflow occurs.
 *
 * Parameters: pszNumber      A pointer to a string that contains an
 *                            ASCII HEX number.
 *
 *             pulNumber      A pointer to a UCHAR to receive the
 *                            number.
 *
 * Return value: TRUE if the number was converted, FALSE if overflow.
 *
 * Comments:
 *   Valid numbers are:
 *     [whitespace][0x|0X][hexdigits]
 *
 * Note that if no hex digits are found in the string, 0 is returned.
 */

extern BOOL EXPORT ItiStrHexToUCHAR (PSZ pszNumber, PUCHAR pucChar);


/* 
 * ItiStrHexToULONG converts the given string into ULONG. The function
 * will stop converting at the first non hex digit character.  Leading 
 * whitespace is skiped.  *pulNumber is 0 if overflow occurs.
 *
 * Parameters: pszNumber      A pointer to a string that contains an
 *                            ASCII HEX number.
 *
 *             pulNumber      A pointer to a ULONG to receive the
 *                            number.
 *
 * Return value: TRUE if the number was converted, FALSE if overflow.
 *
 * Comments:
 *   Valid numbers are:
 *     [whitespace][0x|0X][hexdigits]
 *
 * Note that if no hex digits are found in the string, 0 is returned.
 */

extern BOOL EXPORT ItiStrHexToULONG (PSZ pszNumber, PULONG pulNumber);


/*
 * ItiStrLONGToString converts a ULONG to a hex string.
 *
 * Parameters: ulValue     The number to convert.
 *
 *             pszDest     The destination buffer.
 *
 *             usDestMax   The maximum number of characters (including
 *                         the null terminator) to store in pszDest.
 *
 * Return Value: The number of characters to stored in pszDest (excluding
 * the null terminator).
 */

extern USHORT EXPORT ItiStrULONGToHexString (LONG   ulValue, 
                                             PSZ    pszDest, 
                                             USHORT usDestMax);



/*
 * ItiStrLONGToHexString converts a UCHAR to a hex string.
 *
 * Parameters: ucValue     The number to convert.
 *
 *             pszDest     The destination buffer.
 *
 *             usDestMax   The maximum number of characters (including
 *                         the null terminator) to store in pszDest.
 *
 * Return Value: The number of characters to stored in pszDest (excluding
 * the null terminator).
 */

extern USHORT EXPORT ItiStrUCHARToHexString (LONG   lValue, 
                                             PSZ    pszDest, 
                                             USHORT usDestMax);


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



/*
 * ItiStrUpper converts the given string to all upper case letters,
 * in place.
 *
 * Parameters: pszString      The string to convert.
 * 
 * Return Value: pszString is always returned.
 */

extern PSZ EXPORT ItiStrUpper (PSZ pszString);





/*
 * ItiStrLower converts the given string to all lower case letters,
 * in place.
 *
 * Parameters: pszString      The string to convert.
 * 
 * Return Value: pszString is always returned.
 */

extern PSZ EXPORT ItiStrLower (PSZ pszString);


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

extern USHORT EXPORT ItiStrUSHORTToString (USHORT usValue, 
                                           PSZ    pszDest, 
                                           USHORT usDestMax);


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

extern USHORT EXPORT ItiStrSHORTToString (SHORT  sValue, 
                                          PSZ    pszDest, 
                                          USHORT usDestMax);



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

extern USHORT EXPORT ItiStrULONGToString (ULONG  ulValue, 
                                          PSZ    pszDest, 
                                          USHORT usDestMax);


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

extern USHORT EXPORT ItiStrLONGToString (LONG   lValue, 
                                         PSZ    pszDest, 
                                         USHORT usDestMax);



/*
 * ItiStrTok
 *
 * Parameters: pszString      The string to parse.  May be NULL if
 *                            ItiStrTok has been called before by the
 *                            current thread.
 *
 *             pszTokenList   A list of tokens.
 *
 * Return Value: A pointer to the token.
 *
 * Comments: Yes, this function is multi-threaded.
 * pszString is modified.
 */

extern PSZ EXPORT ItiStrTok (PSZ pszString, 
                             PSZ pszTokenList);

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

extern PVOID EXPORT ItiMemSet (PVOID pvData, UCHAR ucByte, USHORT usNumBytes);






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


/* -- Gets the iField-th comma separated field from the string
      pCSVString and puts it in sResult.  Returns a pointer to the
      result.  At most usResultSize characters are stored in pszResult.

      Field numbers start with 1.

      If there is no iField-th field, creates an empty string.

      Strips leading and trailing ", and collapses "" into ".

*/
extern PSZ EXPORT ItiCsvGetField (USHORT usField, 
                                  PSZ    pszCSVString, 
                                  PSZ    pszResult,
                                  USHORT usResultSize);



/* -- Changes the string pString, putting the changed one in pResult, 
      so it can be in a 
      CSV string.  Generally, this means replacing " with "", and
      putting " around it if there are internal " or ,.

      At most usResultSize characters are stored in pszResult.
*/
extern PSZ EXPORT ItiCsvMakeField (PSZ pszString, PSZ pszResult, USHORT usResultSize);








/**************************************************************************
 * Genera purpose string Utilities                                        *
 **************************************************************************/


/*
 * Eats a word from the front of a string and returns
 * that word and the rest of the string.
 *
 * 1> skip spaces and tabs
 * 2> read to delimeter character (exclusive)
 * 3> word ends at char before delim, string starts at char after delim
 * 4> returns word read and remaining string as parameters
 * 5> delimeter char is returns as fn val.
 *
 * returns 0xFFFF when there is nothing to return
 */
extern USHORT EXPORT ItiGetWord (PSZ *ppsz, PSZ pszWord, PSZ pszDelimeters, BOOL bEatDelim);



/*
 * 0 = match
 * 1 = no match
 * 2 = end of string
 */
extern USHORT EXPORT ItiEat (PSZ *ppszStr, PSZ pszTarget);



/*
 * Removes trailing chars from str
 */
extern PSZ EXPORT ItiClip (PSZ pszStr, PSZ pszClipList);



/*
 * Removes leading chars from str
 */
extern PSZ EXPORT ItiStrip (PSZ pszStr, PSZ pszStripList);

/*
 * Removes chars from anywhere in string
 *
 */
extern PSZ EXPORT ItiExtract (PSZ pszStr, PSZ pszExtractList);


/* if 1st 2 chars are double quotes, this fn eats the first 2 chars
 * and returns true.
 * else the fn returns false
 */
extern BOOL EXPORT ItiTwoQuotes (PSZ *ppsz);



/* if 1st char is double quote, this fn eats the first char
 * and returns true.
 * else the fn returns false
 */
extern BOOL EXPORT ItiSingleQuotes (PSZ *ppsz);



/* Standard String Dup
 *
 */
extern PSZ EXPORT ItiStrDup(HHEAP hheap, PSZ pszSourceStr);



/*
 * Standard Mem Dup
 */
extern PVOID EXPORT ItiMemDup (HHEAP hheap, PVOID pSrc, USHORT usSize);


/*
 * frees a pointer to an array of strings, as well as all of the strings.
 */
extern void EXPORT ItiFreeStrArray (HHEAP hheap, PSZ *ppsz, USHORT usCount);


/*
 * frees a pointer to an array of strings, as well as all of the strings.
 * This function assumes that the array ends with a NULL pointer.
 */
extern void EXPORT ItiFreeStrArray2 (HHEAP hheap, PSZ *ppsz);




/*
 * The pszIniFileName should be CES_INIPRF which is # defined near the top of 
 * this file.
 */
extern USHORT EXPORT ItiOpenProfile(HAB hab,PSZ pszIniFileName,PHINI phiniPrf);


extern USHORT EXPORT ItiCloseProfile (PHINI phiniPrf);


/*
 * Returns the text of a named string into the StrBuffer.
 * Note that an environment string pszStrName will override
 * the ini file string. 
 */                          
extern USHORT EXPORT ItiGetProfileString (HINI   hiniPrf,
                                          PSZ    pszAppName,
                                          PSZ    pszStrName,
                                          PSZ    pszStrBuffer,
                                          USHORT usStrBufferLength,
                                          PSZ    pszDefaultBuffer);




extern USHORT EXPORT ItiWriteProfileString (HINI hiniPrf,
                                            PSZ  pszAppName,
                                            PSZ  pszStrName,
                                            PSZ  pszStrValue);


/* returns TRUE if the string pszString contains a wildcard character */
extern BOOL EXPORT ItiStrIsWildCard (PSZ pszString);


extern BOOL EXPORT ItiStrMatchWildCard (PSZ pszWildCard, /* Wildcard to match */
                                        PSZ pszMatch);   /* string to match */




/*
 * ItiStrMakePPSZ makes an array of PSZs from the parameters.  The
 * array is allocated from the heap hheap.  Each PSZ is Duped from the
 * heap hheap.
 *
 * Parameters: hheap                   A heap to allocate from.
 *
 *             usCount                 The number of strings to store.
 * 
 *             subsequent parameters   The PSZs to duplicate and store.
 *
 * Return value: NULL on error, or a PPSZ.
 *
 * Comments: You'd better get usCount right, or you'll regret it.
 */

extern PSZ *_far _cdecl _loadds ItiStrMakePPSZ (HHEAP hheap, USHORT usCount, ...);


/*
 * ItiStrMakePPSZ2 makes an array of PSZs from the parameters.  The
 * array is allocated from the heap hheap.  Each PSZ is Duped from the
 * heap hheap.  This function keeps on duping until a NULL pointer is 
 * encountered.  This NULL is considered part of the returned PPSZ. 
 * 
 *
 * Parameters: hheap                   A heap to allocate from.
 *
 *             subsequent parameters   The PSZs to duplicate and store.
 *
 * Return value: NULL on error, or a PPSZ.
 *
 * Comments: You'd better not for get to put that terminating NULL...
 */

extern PSZ *_far _cdecl _loadds ItiStrMakePPSZ2 (HHEAP hheap, ...);



/*
 * this converts a string to a double
 */

extern DOUBLE EXPORT ItiStrToDouble (PSZ pszNumber);

extern BOOL EXPORT ItiStrDoubleToString (DOUBLE dNum,
                                         PSZ    pszDest, 
                                         USHORT usDestMax);



/* 
 * returns TRUE if the given string is NULL, or contains only white 
 * space.
 */

extern BOOL EXPORT ItiStrIsBlank (PSZ pszString);


/* this fills a ppsz with usCount bits of data
 *
 */

extern PPSZ _far _cdecl _loadds ItiFillPPSZ (PPSZ ppszList, USHORT usCount, ...);


/*
 * 1/11/93 mdh: added this function.
 *
 * ItiStrPrintfDlgItem gets the text for the control usID, and performs
 * a printf with it.  The parameters passed in "..." are used to replace the 
 * printf parameters.
 *
 */

extern int _far _cdecl _loadds ItiStrPrintfDlgItem (HWND hwndDlg, USHORT usID, ...);


extern USHORT EXPORT ItiStrGet1Param (PSZ    pszDest,
                                      PSZ    pszRequestedValue,
                                      USHORT usDestMax,
                                      PSZ    apszTokens [], 
                                      PSZ    apszValues []);



/**************************************************************************
 * Start of CSV File utilities.
 **************************************************************************/

/*
 * ItiFileQueryExist returns TRUE if the given file exists.
 *
 * Parameters: pszFileName       The name of the file.
 *
 * Return Value: TRUE if the file exists, FALSE if not.
 */

extern BOOL EXPORT ItiFileQueryExist (PSZ pszFileName);


/*
 * ItiFileOpenRead opens the given file for read access.  The file is
 * locked against write access.  The file must exist, or this
 * function will return 0.
 *
 * Parameters: pszFileName       The file to open.
 *
 * Return Value: A handle to the file, or 0 on error.
 */

extern PFILE EXPORT ItiFileOpenRead (PSZ pszFileName);





/*
 * ItiFileClose closes the given file.
 *
 * Parameters: hfFile            A handle to a file to close.
 *
 * Return Value: None.
 */

extern void EXPORT ItiFileClose (PFILE hfFile);





/*
 * ItiFileQueryPos returns the current file position of the given file.
 *
 * Parameters: hfFile            The file to check.
 *
 * Return value: A ULONG denoting the current file position.  0 is returned
 * on error.
 */
extern ULONG EXPORT ItiFileQueryPos (PFILE hfFile);




/*
 * ItiFileSetPos sets the file position for the given file.
 *
 * Parameters: hfFile            The file to set.
 *
 *             lPos              The new position.
 *
 *             usFromWhere       Use FROM_CURRENT, FROM_END, or
 *                               FROM_START.
 *
 * Return value: A ULONG denoting the current file position.  0 is returned
 * on error.
 */
extern ULONG EXPORT ItiFileSetPos (PFILE hfFile, LONG lPos, USHORT usFromWhere);

#define FROM_START      0
#define FROM_CURRENT    1
#define FROM_END        2



/*
 * ItiFileSkipLine skips to the next line.
 *
 * Parameters: hfFile            The file to use.
 *
 * Return value: TRUE if an error or the end of the file has been
 * reached.
 */

extern BOOL EXPORT ItiFileSkipLine (PFILE hf);




/*
 * ItiFileSkipBlankLines skips from the current line to the first
 * non-blank line.
 *
 * Parameters: hfFile            The file to use.
 *
 * Return value: The number of lines skipped.
 */

extern USHORT EXPORT ItiFileSkipBlankLines (PFILE hf);





/*
 * ItiFileOpenCSV opens a DS/Shell or HighEst CSV File.
 *
 * Parameters: pcfi              Pointer to a CSVFILEINFO structure.
 *
 *             pszFileName       The name of the file to open.
 *
 *             pszDSShell        A pointer to the DS/Shell version string.
 *
 *             pszHighEst        A pointer to the HighEst version string.
 * 
 * Return Value: 0 if the file was opened, or one of the following
 * errors:
 *    ERROR_COULD_NOT_OPEN
 *    ERROR_NOT_CSV
 *    ERROR_WRONG_VERSION
 *
 */

extern BOOL EXPORT ItiFileOpenCSV (PCSVFILEINFO pcfi,
                                   PSZ          pszFileName,
                                   PSZ          pszDSShellVersion,
                                   PSZ          pszHighEstVersion);


/*
 * ItiFileCloseCSV closes a DS/Shell CSV File.
 *
 * Parameters: pcfi              Pointer to a CSVFILEINFO structure.
 *
 * Return Value: nothing.
 *
 */

extern void EXPORT ItiFileCloseCSV (PCSVFILEINFO pcfi);





/*
 * ItiFileSkipCSVLine skips to the next CSV line.
 *
 * Parameters: pcfi              Pointer to a CSVFILEINFO structure.
 *
 * Return Value: Returns TRUE on EOF or error.
 */

extern BOOL EXPORT ItiFileSkipCSVLine (PCSVFILEINFO pcfi);




/*
 * ItiFileGetCSVWord gets the next CSV parameter from the file hfFile.
 *
 * Parameters: pcfi              Pointer to a CSVFILEINFO structure.
 *
 *             pszWord           The word read from the file.
 *
 *             pusWordLen        A pointer to a USHORT that will contain
 *                               the number of characters in the word.
 *
 *             usWordMax         The maximum number of characters to
 *                               store in pszWord.
 *
 * Return Value: FALSE if no more CSV words are on the line.  You
 * should call ItiFileSkipCSVLine to advance to the next line.
 */

extern BOOL EXPORT ItiFileGetCSVWord (PCSVFILEINFO pcfi,
                                      PSZ          pszWord,
                                      PUSHORT      pusWordLen,
                                      USHORT       usWordMax);




/*
 * ItiFileSkipCSVWord skips to the next CSV word.
 *
 * Parameters: pcfi        Pointer to a CSVFILEINFO structure.
 *
 *             bInQuote    TRUE if currently in a quoted string.
 *
 * Return Value: Returns TRUE on EOF or error.
 */

extern BOOL EXPORT ItiFileSkipCSVWord (PCSVFILEINFO pcfi,
                                       BOOL         bInQuote);
       

/*
 * ItiFileSaveCSVPos saves the current CSV file position.
 *
 * Parameters: pcfiSave          Where to store the file position.
 *
 *             pcfi              The file whose position is to be saved.
 *
 * Return value: none.
 */ 

extern void EXPORT ItiFileSaveCSVPos (PCSVFILEINFO pcfiSave, 
                                      PCSVFILEINFO pcfi);





/*
 * ItiFileRestoreCSVPos restores a saved CSV file position.
 *
 * Parameters: pcfi              The file whose position is to be restored.
 *
 *             pcfiSaved         The saved file position.
 *
 * Return value: none.
 */ 

extern VOID EXPORT ItiFileRestoreCSVPos (PCSVFILEINFO pcfi, 
                                         PCSVFILEINFO pcfiSaved);






/*
 * ItiFileSkipCSVSection skips a CSV file section.  The function
 * assumes that it is at the beginning of a line.
 *
 * Parameters: pcfi              The CSV file to process.
 *
 *             bCaseSensitive    TRUE if you want a case sensitive
 *                               string compare, FALSE if not.
 *
 * Return Value: TRUE if at end of file or error.
 *
 * Comments: A CSV section is denoted by a set of lines whose
 * first field is the same.  At least one line (the current) is 
 * always skipped.
 *
 * RESTRICTION: Only the first 255 characters of the first
 * field is used for comparison.
 */

extern BOOL EXPORT ItiFileSkipCSVSection (PCSVFILEINFO pcfi, 
                                          BOOL         bCaseSensitive);



/* ItiVerCmp
 *
 * Returns true if the dll's version is greater than or
 *    equal to the version of the application program. 
 */
extern BOOL EXPORT ItiVerCmp (PSZ pszDll);

