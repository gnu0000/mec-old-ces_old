/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1997 by Info Tech, Inc.  All Rights Reserved.         |
|                                                                           |
+--------------------------------------------------------------------------*/


/*                                                          RTF Utility
 *   rtf.h
 *   Timothy Blake
 *   Copyright (C) 1997 Info Tech, Inc.
 *
 */


#define  INCL_GPI
#define  INCL_WIN

#if !defined (RTF_GLOBALS_INCLUDED)

#define RTF_GLOBALS_INCLUDED

#define MAX_TOKEN_LENGTH 30
char szCurrentToken[ MAX_TOKEN_LENGTH + 1 ];

/*
 * Declare the structure of the RTF global variable
 */
typedef struct 
   {
   int  iTokenIDValue;
   char * pszCurrentToken;
   } RTFGLOBALS;

typedef RTFGLOBALS FAR *PRTFGLOBALS;


#endif  /* RTF_GLOBALS_INCLUDED */
