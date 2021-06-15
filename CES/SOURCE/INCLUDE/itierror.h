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
 * ItiError.h
 * Mark Hampton
 * This module is part of DS/Shell (CS/Base)
 *
 * This module displays various operating system error messages.
 *
 * The following DLLs are refereced by this DLL:
 *    ItiBase
 */



#if !defined (ITIERROR_INCLUDED)
#define ITIERROR_INCLUDED



#if !defined (RC_INVOKED)


/* define general errors */
#if !defined (ERROR_NOT_ENOUGH_MEMORY)
#define ERROR_NOT_ENOUGH_MEMORY  8
#endif

#if !defined (ERROR_INVALID_PARAMETER)
#define ERROR_INVALID_PARAMETER 87
#endif

#if !defined (ERROR_INVALID_FUNCTION)
#define ERROR_INVALID_FUNCTION   1
#endif 

#if !defined (ERROR_UNKNOWN)
#define ERROR_UNKNOWN         0xffff
#endif

/* define base error numbers for each module */
#define ITIBASE_ERROR_BASE    1000

#define ITIERROR_ERROR_BASE   1100

#define ITIFMT_ERROR_BASE     1200

#define ITIIPC_ERROR_BASE     1300

#define ITIRPT_ERROR_BASE    22000




/*--- Errors used in ItiWin, ItiDBase, ItiMBase, Apps, Dlls... ---*/

#define ITIERR_NULLHWND         1
#define ITIERR_BADMODE          2
#define ITIERR_NOKEY            3
#define ITIERR_NOCOL            4
#define ITIERR_NOTABLE          5
#define ITIERR_BLANKQUERY       6
#define ITIERR_NOQUERY          7
#define ITIERR_NOTRANSACTION    8
#define ITIERR_NOSQLEXEC        9
#define ITIERR_NOSQLCOMMIT      10
#define ITIERR_NODBLOCK         11
#define ITIERR_NOEXEC           12
#define ITIERR_BADBUFF          13
#define ITIERR_BADCOL           14
#define ITIERR_ROWCHANGED       15



/*
 * ItiErrDispalyWindowError displays the last Win subsystem error for
 * the PM thread identified by hab.  
 *
 * Parameters: hab:                 The anchor block in which the
 *                                  error occurred.  This parameter
 *                                  must be the anchor block for the 
 *                                  thread who had the error.
 *
 *             hwndActivate:        The window that is activated when 
 *                                  this function returns.  If 
 *                                  hwndActivate is not NULL, this has 
 *                                  the effect of creating an 
 *                                  application modal dialog box.
 *
 *             usHelpID:            The dialog box that this procedure
 *                                  creates has a HELP button.  If this
 *                                  button is pushed, then this parameter
 *                                  is sent to the help handler.
 *
 * Return Value: None.
 *
 * Comments: You must ensure that hab is the anchor block that the
 * error occured in, or this function will not display the correct
 * error.
 */


extern void EXPORT ItiErrDisplayWindowError (HAB    hab, 
                                             HWND   hwndActivate,
                                             USHORT usHelpID);



/*
 * ItiErrorFreeErrorString frees an error string previously retrieved 
 * by ItiErrGetErrorString.
 *
 * Parameters: hab      The anchor block in which the error occured.
 *
 * Return Value: A pointer to a string with a description of the 
 * error.  If no error has occured, NULL is returned.  Call 
 * ItiErrFreeErrorString to free the memory and error associated
 * with this string.
 *
 * Comments: If you are under debugging mode, the error string is written
 * to the error pipe.
 */

extern PSZ EXPORT ItiErrGetErrorString (HAB hab);




/*
 * ItiErrorFreeErrorString frees an error string previously retrieved 
 * by ItiErrGetErrorString.
 *
 * Parameters: pszWindowError    A pointer to a string returned by 
 *                               ItiGetErrorString.  This value may
 *                               be NULL.
 *
 * Return Value: None.
 *
 * Comments: The debugging version of this module will write a
 * string to the error pipe if NULL is passed to this function.  If 
 * this function was called, and there was no error, a string is also
 * written to the debugging pipe.
 */

extern VOID EXPORT ItiErrFreeErrorString (PSZ pszWindowError);



/*
 * ItiErrWriteDebugMessage writes a debugging message to the error named pipe.
 * The string is prefixed with "Debug: ", followed by pszMessage, followed
 * by a newline.
 *
 * Parameters: pszMessage     A null terminated string to be written.
 *
 * Return Value: none.
 *
 * Comments: 
 * The calling thread may block if the named pipe is full.
 */


extern void EXPORT ItiErrWriteDebugMessage (PSZ pszMessage);


extern void EXPORT ItiErrDisplayHardError (PSZ pszMessage);

/* this function returns a pointer to the file name that contains
   the errorlog information */
extern PSZ EXPORT ItiErrQueryLogFileName (void);

#endif /* if !defined (RC_INVOKED) */


#endif /* if !defined (ITIERROR_INCLUDED) */

