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
 * Iti.h
 * Mark Hampton
 * This module is part of DS/Shell (CS/Base)
 */


#if !defined (ITI_INCLUDED)

#define ITI_INCLUDED


#if !defined (RC_INVOKED)

#define INCL_DOSINFOSEG
#define INCL_DOSPROCESS
#define INCL_WINSYS
#define INCL_WINHEAP

#endif

#include <os2.h>


#define VERSION         001
#define VERSION_MAJOR   1
#define VERSION_MINOR   1
#define VERSION_STRING  "1.1b4"
#define VERSION_CHAR    'b'
#define METRIC          1
/* Note: versions 1.1a and above are metric compatible. */

/*
 * Define PRERELEASE to be a quoted string containing the number of
 * the pre-release number for this release of the software.  If this is 
 * a "final" release, then comment out the #define.
 */
// #define PRERELEASE     "(Pre-Release #1)"


/*
 * COPYRIGHT is the copyright date displayed in various places.
 */
#define COPYRIGHT    "1991-1997"


#ifndef RC_INVOKED

/*
 * Define standard function types
 */

/*
 * The EXPORT macro is used for functions that are exported to other modules.
 * For example, if you are writing a function in a DLL that other modules
 * will call directly, use the EXPORT macro.
 *
 * NOTE: The EXPORT macro goes between the function return type and the
 * function name, eg. PVOID EXPORT ItiMemAlloc (...).
 */

#define EXPORT _far _pascal _loadds

/* CEXPORT is used for exporting C calling convention functions */
#define CEXPORT _far _cdecl _loadds


/*
 * The DLLINITPROC macro is used for a dll initialization function.
 * The DLL initialization function has a function prototype as follows:
 *
 * BOOL DLLINITPROC DllInit (HMODULE hmodModule);
 *
 * Note that the name of the function should be DllInit, and that
 * it should return TRUE if it was succsessfully initialized, or
 * FALSE if initialization failed.  The parameter hmodModule is
 * the handle to the DLL.  You will need this parameter if your 
 * DLL has resources.
 */

#define DLLINITPROC _far _pascal


typedef PSZ *PPSZ;

/* to avoid conflict with the truely wonderful microsoft C library 
   definition of HUGE in math.h, the HUGE stuff has been commented 
   out.  Besides, nobody used it anyway.... */
//
///*
// * If you need to use huge pointers, use the macro HUGE.
// */
//
//#define HUGE   _huge
//
//
//
///*
// * The data type HPVOID is a huge pointer to an unknown data type.
// */
//
//typedef void HUGE *HPVOID;
//
//
//
///*
// * The data type HPSZ is a huge pointer to a null terminated string.
// */
//
//typedef char HUGE *HPSZ;
//
//
//
///*
// * The data type HPCHAR is a huge pointer to a character.
// */
//
//typedef char HUGE *HPCHAR;
//
//
//
///*
// * The data type HPUSHORT is a huge pointer to a USHORT.
// */
//
//typedef char HUGE *HPUSHORT;
//
//
//
///*
// * The data type HPULONG is a huge pointer to a ULONG.
// */
//
//typedef char HUGE *HPULONG;
//


/*
 * WNDPROC is a macro used for window functions
 */
#define WNDPROC MRESULT EXPENTRY


/*
 * time and dates
 */

#define  DATE       __DATE__ 
#define  TIME       __TIME__ 

/*
 * UM is a macro used as a typedef (UM) for messages returning
 * a USHORT
 */
#define UM  USHORT)(ULONG



typedef double DOUBLE;
typedef double *PDOUBLE;

#define NATLOG_2           ((DOUBLE) 0.693147180559945)


/*
 * MAX_THREADS is the maximum number of threads that a DS/SHELL 
 * application can support.  (don't go faster than 55)
 */

#define MAX_THREADS 55



/*
 * base values for messages for the different modules
 */
#define WNDMSG_START          (WM_USER + 100)
#define DBMSG_START           (WM_USER + 200)





/*
 * define some things that OS2.H didn't, but should
 */

#define QWP_USER     0

/*
 * some things used everywhere
 */
#define ITI_NONE     0xFFFF
#define ITI_UNKNOWN  0
#define ITI_ADD      1
#define ITI_INSERT   1
#define ITI_CHANGE   2
#define ITI_DELETE   3

#define ITI_STATIC   4
#define ITI_LIST     5
#define ITI_DIALOG   6

#define ITI_OTHER    7

#define ITI_SELECT   0x0000
#define ITI_UNSELECT 0x0001
#define ITI_INVERT   0x0002                  


#define UNDECLARED_BASE_DATE  "1/1/1900"


#define CODE_DEF_ROAD    3
#define CODE_DEF_STRU    2
#define CODE_DEF_UNIT    4
#define CODE_DEF_WORK    1



#if defined (DEBUG)

/*
 * define DEBUG_BREAK to cause a debug interrupt to happen.
 */
#define DEBUG_BREAK _asm int 3

#else

/* 
 * define DEBUG_BREAK to cause nothing to happen.
 */
#define DEBUG_BREAK

#endif   /* if defined (DEBUG) */





#endif   /* if !defined (RC_INVOKED) */

#endif   /* if !defined (ITI_INCLUDED) */
