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
 * ItiFmt.h
 * Mark Hampton
 * This file is part of DS/Shell (CS/Base)
 */


#if !defined (ITIFMT_INCLUDED)
#define ITIFMT_INCLUDED

#if !defined (RC_INVOKED)


#define ITIFMT_TRUNCATED                  (ITIFMT_ERROR_BASE + 1)
#define ITIFMT_FORMAT_ERROR               (ITIFMT_ERROR_BASE + 2)
#define ITIFMT_BAD_DATA                   (ITIFMT_ERROR_BASE + 3)
#define ITIFMT_NAME_ALREADY_REGISTRERED   (ITIFMT_ERROR_BASE + 4)
#define ITIFMT_INVALID_NAME               (ITIFMT_ERROR_BASE + 5)
#define ITIFMT_NULL                       (ITIFMT_ERROR_BASE + 6)
#define ITIFMT_DEST_TOO_SMALL             (ITIFMT_ERROR_BASE + 7)


#define MAX_FORMAT_NAME_SIZE              32


/*
 * ItiFmtInitialize initializes the format module.  This function
 * should only be called once per process.
 *
 * Return Value: TRUE if the format module was initialized, FALSE if
 * not.
 */

extern BOOL EXPORT ItiFmtInitialize (void);






/*
 * ItiFmtFormatString formats a string for output to the user.
 *
 * Parameters: pszSource      A pointer to the string to format.
 *
 *             pszDest        A pointer to where to store the
 *                            formated string.
 *
 *             usDestMax      The maximum number of characters to store
 *                            in pszDest.
 *
 *             pszFormat      A pointer to the string that contains
 *                            formatting information.
 *
 *             pusDestLength  A pointer to a USHORT that recieves the number
 *                            of characters stored in pszDest, excluding
 *                            the null terminator.  This parameter may be
 *                            NULL.
 *
 * Return value: 0 if sucsessful,
 *               ITIFMT_TRUNCATED if pszDest was truncated because of space
 *               ITIFMT_FORMAT_ERROR if pszFormat was in error.
 *               ITIFMT_BAD_DATA if the data passed to the format function
 *               is invalid or out of range.
 */

extern USHORT EXPORT ItiFmtFormatString (PSZ     pszSource,
                                         PSZ     pszDest,
                                         USHORT  usDestMax,
                                         PSZ     pszFormat,
                                         PUSHORT pusDestLength);





/*
 * ItiFmtCheckString checks a string for valid data, and prepares it
 * for sending to the database.
 *
 * Parameters: pszSource      A pointer to the string to check.
 *
 *             pszDest        A pointer to where to store the
 *                            formated string.
 *
 *             usDestMax      The maximum number of characters to store
 *                            in pszDest.
 *
 *             pszFormat      A pointer to the string that contains
 *                            formatting information.
 *
 *             pusDestLength  A pointer to a USHORT that recieves the number
 *                            of characters stored in pszDest, excluding
 *                            the null terminator.  This parameter may be
 *                            NULL.
 *
 * Return value: 0 if sucsessful,
 *               ITIFMT_TRUNCATED if pszDest was truncated because of space
 *               ITIFMT_FORMAT_ERROR if pszFormat was in error.
 *               ITIFMT_BAD_DATA if the data passed to the format function
 *               is invalid or out of range.
 */

extern USHORT EXPORT ItiFmtCheckString (PSZ     pszSource,
                                        PSZ     pszDest,
                                        USHORT  usDestMax,
                                        PSZ     pszFormat,
                                        PUSHORT pusDestLength);



/*
 * ItiFmtGetEnumData
 *
 * Parameters: hheap             A heap to allocate from.
 *
 *             pszEnum           A pointer to a string form of the
 *                               enumeration.
 *
 *             pppszEnumShortStr A pointer to an array of PSZs that
 *                               will be filled in by this function.
 *                               This array will contain pointers to
 *                               the short form of the enumeration.
 *
 *             pppszEnumLongStr  A pointer to an array of PSZs that
 *                               will be filled in by this function.
 *                               This array will contain pointers to
 *                               the long form of the enumeration.
 *
 *             ppusEnumValues    A pointer to an array of USHORT that
 *                               will be filled in by this function.
 *                               This array will contain USHORT that
 *                               are the values for each enumeration
 *                               string.
 *
 *             pusNumValues      The number of entries for the previous
 *                               three parameters.
 *
 * Returns TRUE if the enumeration exists, FALSE if not.
 */
extern BOOL EXPORT ItiFmtGetEnumData (HHEAP   hheap,
                                      PSZ     pszEnum,
                                      PSZ     **pppszEnumShortStr,
                                      PSZ     **pppszEnumLongStr,
                                      PUSHORT *ppusEnumValues,
                                      PUSHORT pusNumValues);




/*
 * ItiFmtFreeEnumData
 *
 * Parameters: hheap             A heap to free from.
 *
 *             ppszEnumShortStr  A pointer to an array allocated by
 *                               ItiFmtGetEnumData.
 *                               This parameter may be NULL.
 *
 *             ppszEnumLongStr   A pointer to an array allocated by
 *                               ItiFmtGetEnumData.
 *                               This parameter may be NULL.
 *
 *             pusEnumValues     A pointer to an array allocated by
 *                               ItiFmtGetEnumData.
 *                               This parameter may be NULL.
 *
 *             usNumValues       The number of entries for the previous
 *                               three parameters.
 */
extern VOID EXPORT ItiFmtFreeEnumData (HHEAP   hheap,
                                       PSZ     *ppszEnumShortStr,
                                       PSZ     *ppszEnumLongStr,
                                       PUSHORT pusEnumValues,
                                       USHORT  usNumValues);


extern USHORT EXPORT ItiFmtQueryEnumCount (USHORT usEnumID);

extern USHORT EXPORT ItiFmtQueryEnumID (PSZ pszEnum);


/* system defined enumerations */
#define ENUM_PRECISION          1
#define ENUM_DEPENDENCY         2
#define ENUM_PRIVILEGE          3
#define ENUM_QUANTITYLEVEL      4
#define ENUM_UNITTIME           5
#define ENUM_MONTH              6
#define ENUM_SEASON             7
#define ENUM_USERTYPE           8
#define ENUM_UNITSYSTEM         9
#define ENUM_UNITTYPESYSTEM    10




typedef USHORT (EXPORT *PFNFORMAT) (PSZ      pszSource,
                                    PSZ      pszDest,
                                    USHORT   usDestMax,
                                    PSZ      pszFormatInfo,
                                    PUSHORT  pusDestLength);


/*
 * ItiFmtRegisterFormat registers a formatting function.
 *
 * Parameters: pszFormatName     A pointer to a string that contains
 *                               the name of the format method.  The name
 *                               must not contain any commas.  The name
 *                               of the format method is case insensitive.
 *
 *             pfnOutput         A pointer to a function that formats
 *                               strings for output to the user.
 *
 *             pfnInput          A pointer to a function that formats
 *                               strings for input to the database.
 *
 *             bOverride         Override the current format method.
 *
 *             ppfnOldFormat     A pointer to a pointer to the old
 *                               format function.  This parameter may
 *                               be NULL if you don't want a pointer
 *                               to the old format function.
 *
 * Return Value: 0 if succesful,
 *               ITIFMT_NAME_ALREADY_REGISTRERED if a format with the
 *               same name exists and bOverride is FALSE,
 *               ITIFMT_INVALID_NAME if pszFormatName is invalid, or
 *               ERROR_NOT_ENOUGH_MEMORY
 *
 * Comments:
 * The function pfnFormat must be defined as follows:
 * USHORT EXPORT FunctionName (PSZ     pszSource,
 *                             PSZ     pszDest,
 *                             USHORT  usDestMax,
 *                             PSZ     pszFormatInfo,
 *                             PUSHORT pusDestLength)
 *
 * The parameter pszFormatInfo points to the format string, just after the
 * comma that separates the format method name from the format method data.
 * For example, if ItiFmtFormatString was called with pszFormat = "Date,mmm"
 * then pszFormatInfo will point to "mmm".
 *
 * The format function should return 0 if the string was formated, or
 * ITIFMT_TRUNCATED if pszDest was truncated because of space,
 * ITIFMT_FORMAT_ERROR if pszFormat was in error, or ITIFMT_BAD_DATA if the
 * data passed to the format function is invalid or out of range.
 * The format function must ensure that pszDest is always null ('\0')
 * terminated.
 * The format function store the number of characters placed in the
 * pszDest string (excluding the null terminator) in the pusDestLength
 * parameter.  The pusDestLength parameter MAY BE A NULL pointer.
 *
 * If your function returns ITIFMT_BAD_DATA, ItiFmtFormatString fills pszDest
 * with the string "!Bad data!".  If your function return ITIFMT_FORMAT_ERROR,
 * ItiFmtFormatString fills pszDest with the string "*Bad format*".
 */

extern USHORT EXPORT ItiFmtRegisterFormat (PSZ        pszFormatName,
                                           PFNFORMAT  pfnOutput,
                                           PFNFORMAT  pfnInput,
                                           BOOL       bOverride,
                                           PFNFORMAT  *ppfnFormat);



/*
 * ItiFmtCheckEnumValue checks to see if a pszValue is a valid string 
 * representation of the enumeration usEnumID.  
 *
 * Parameters: usEnumID       The enumeration to check.
 *
 *             pszValue       The string value to check.
 *
 *             pusValue       A pointer to a USHORT that will contain
 *                            the enumeration value.  This parameter
 *                            may be NULL.
 *
 * Return Value: TRUE if the string exists in the enumeration, FALSE
 * otherwise.
 */

extern BOOL EXPORT ItiFmtCheckEnumValue (USHORT  usEnumID, 
                                         PSZ     pszValue, 
                                         PUSHORT pusValue);
       



/*
 * ItiFmtQueryEnumString returns the enumeration string for a 
 * given enumeration value.
 *
 * Parameters: usEnumID       The enumeration.
 *
 *             usValue        The value.
 *
 *             bLong          TRUE if you want the long version of the
 *                            string, FALSE if you want the short version.
 *
 *             pszDest        The place to store the string.
 *
 *             pusDestMax     The maximum number of bytes to store in
 *                            pszDest.
 *
 * Return Value: TRUE if the value exists in the enumeration, FALSE
 * otherwise.
 */

extern BOOL EXPORT ItiFmtQueryEnumString (USHORT  usEnumID, 
                                          USHORT  usValue,
                                          BOOL    bLong,
                                          PSZ     pszDest, 
                                          USHORT  usDestMax);



extern USHORT EXPORT ItiFmtParseDate (PSZ       *ppszDate, 
                                       PUSHORT   pusYear, 
                                       PUSHORT   pusMonth, 
                                       PUSHORT   pusDay);
       


extern USHORT EXPORT ItiFmtParseTime (PSZ       *ppszTime, 
                                       PUSHORT   pusHour,
                                       PUSHORT   pusMinute,
                                       PUSHORT   pusSecond);
       

extern LONG EXPORT ItiFmtCalendarToJulian (PSZ pszCalDate);



extern PSZ EXPORT ItiFmtJulianToCalendar (LONG lJulDate);


/* rounding */

#define ROUND_NONE        0
#define ROUND_0_001       10
#define ROUND_0_01        20
#define ROUND_0_1         30
#define ROUND_1           40
#define ROUND_10          50
#define ROUND_100         60
#define ROUND_1000        70
#define ROUND_10000       80
#define ROUND_100000      90



#endif /* if defined (RC_INVOKED) */

#endif /* if defined (ITIFMT_INCLUDED) */


