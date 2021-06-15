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
 * ItiStr.h
 * Charles Engelke
 * Mark Hampton
 * Craig Fitzgerald
 * Copyright (C) 1989, Info Tech, Inc.
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

export PSZ EXPORT ItiStrCpy (PSZ pszDest, PSZ pszSource, USHORT usDestMax);






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


export PSZ EXPORT ItiStrCat (PSZ pszDest, PSZ pszString, USHORT usDestMax);


   
   
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

export PSZ EXPORT ItiStrChr (PSZ pszString, UCHAR c);





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

export SHORT EXPORT ItiStrCmp (PSZ pszStr1, PSZ pszStr2);









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

export SHORT EXPORT ItiStrICmp (PSZ pszStr1, PSZ pszStr2);






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

export USHORT lpstrncmp (PSZ pszStr1, PSZ pszStr2, USHORT usMaxCmp);






/*
 * ItiStrLen returns the length of string pszString.
 *
 * Parameters: pszString      A pointer to a string.
 *
 * Return Value: The length of the string, not including the null 
 * terminator.
 */

export USHORT EXPORT ItiStrLen (PSZ pszString);






/*
 * ItiStrIsCharInSting returns TRUE if ucChar is in the string pszString.
 *
 * Parameters: ucChar      The character to look for.
 *
 *             pszString   The string to search.
 *
 * Return Value: TRUE if ucChar is in the string pszString, or
 * FALSE if not.
 */

export BOOL ItiStrIsCharInString (UCHAR ucChar, PSZ pszString);





/* 
 * ItiStrSkipSpace skips all space characters (' ', '\t', '\n', and '\r') and 
 * returns the address of the first non space character in pszString.
 *
 * Parameters: pszString      The string to process.
 *
 * Return value: a pointer to then next non-white-space character.
 */

export PSZ EXPORT ItiStrSkipSpace (PSZ pszString);



   
   
   
/* 
 * ItiStrSkipSpace skips all space characters (' ', '\t', '\n', and '\r') and 
 * returns the address of the first non space character in pszString.
 *
 * Parameters: pszString      The string to process.
 *
 * Return value: a pointer to then next non-white-space character.
 */

export PSZ EXPORT ItiStrSkipNonspace (PSZ pszString);



