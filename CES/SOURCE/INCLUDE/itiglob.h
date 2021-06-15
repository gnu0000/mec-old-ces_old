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
 * ItiGlob.h
 * Mark Hampton
 * This module is part of DS/Shell (CS/Base)
 *
 * This module maintains the process-wide global variables for DS/Shell
 *
 * This module is implemented as a DLL.
 *
 * The following DLLs are refereced by this DLL:
 *    ITIERROR.DLL
 */


#if !defined (GLOBALS_INCLUDED)

#define GLOBALS_INCLUDED


/* check for possiblly confusing errors */

#ifndef INCL_DOSINFOSEG
/* this header requires that GINFOSEG and LINFOSEG are defined */
#error The macro INCL_DOS or INCL_DOSINFOSEG must be defined to use Gobals.h

#endif


#ifndef INCL_WINSYS
/* this header requires that the SV_CSYSVALUES macro to be defined */
#error The macro INCL_WIN or INCL_WINSYS must be defined to use Gobals.h

#endif


/*
 * Declare the structure of the global variables
 */

typedef struct 
   {
   /* 
    * system values
    */

   LONG  alSysValues [SV_CSYSVALUES];  /* an array of system values, 
                              indexed by the OS/2 SV_xxxx macros.  
                              See the description of the PM function 
                              WinQuerySysValue */

   /*
    * Average Character information
    */

   USHORT xChar;           /* average character width */

   USHORT yChar;           /* max character height */


   /*
    * Application constants
    */

   HWND hwndDesktop;       /* handle to the desktop window */

   HWND hwndObject;        /* handle to the object window */

   HWND hwndAppFrame;      /* handle to the application's frame window */

   HWND hwndAppClient;     /* handle to the application's client window */

   HAB  habMainThread;     /* handle to the anchor block of the main thread */

   HMQ  hmqMainMessageQueue; /* handle to the application's main thread 
                              message queue */

   HWND hwndHelp;          /* a handle to the help window -- not initialized 
                              until after a bit... */   

   PSZ  pszAppName;        /* a null terminated string containing the 
                              application's name */

   PSZ  pszApplicationID;  /* a null terminated string containing the 
                              application's ID (used for security queries) */

   PGINFOSEG   pgis;       /* pointer to the global info segment.  See the
                              documentation on the OS/2 structure GINFOSEG */

   PLINFOSEG   plis;       /* pointer to the local info segment.  This
                              contains values relative to the main thread.
                              See the documentation on the OS/2 structure
                              LINFOSEG */

   /* user stuff */
   PSZ  pszUserID;         /* the ID of the current user, or NULL if none. */
   PSZ  pszUserName;       /* the name of the current user, or NULL if none. */
   PSZ  pszUserKey;        /* the key of the current user, or NULL if none. */
   PSZ  pszPhoneNumber;    /* the phone number of the current user, or NULL if none. */
   USHORT usUserType;      /* the type of user - USER_NORMAL, etc. */

   /* version stuff */
   PSZ      pszVersionString;  /* the version string */
   USHORT   usVersionMajor;       /* major version */
   USHORT   usVersionMinor;       /* minor version */
   CHAR     ucVersionChar;        /* version character */

   /* metric flag */
   BOOL     bUseMetric;   /* True if the metric spec book is to be used. */

   /* current spec year */
   PSZ  pszCurrentSpecYear;
         /* Pointer to the text string of the current spec year. */

   BOOL bTestImportFlag;
   } GLOBALS;

/* application level privileges */
#define USER_NONE       0
#define USER_NORMAL     10
#define USER_SUPER      20
#define USER_DEVELOPER  30

/* job/program level privileges */
#define USER_VIEWER     10
#define USER_USER       20
#define USER_OWNER      30


typedef GLOBALS FAR *PGLOBALS;

CHAR    szFindPattern[100]; // csp


/*
 * ItiGlobInitGlobals This function initializes most of the global variables.  
 *
 * Parameters: habMainThread:       The anchor block of the main thread.
 *
 *             hmqMainThreadQueue:  The message queue of the main thread.
 *
 *             hwndAppFrame:        The handle to the application's main
 *                                  window frame.
 *
 *             hwndAppClient:       The handle to the application's main
 *                                  window client area.
 *
 *             pszAppName:          A null terminated string containing
 *                                  the application's name.  This must 
 *                                  point to a static variable.
 *
 * Return Value: This function returns a pointer to the global variables.
 *
 * WARNING!
 * The following globals are not initialized until "later".  Users of 
 * these globals should test for NULL or 0 before using them.
 *
 *             hwndHelp
 * 
 * Comments: ItiGlobInitGlobals should be called only once per process.  
 * It must be called by the main thread, after the main window is created, 
 * but before too much else happens.  Note that many DLLs requre that the
 * GlobInitGlobals be called before they can be called.
 */

extern PGLOBALS EXPORT ItiGlobInitGlobals (HAB  habMainThread,
                                           HMQ  hmqMainMessageQueue,
                                           HWND hwndAppFrame,
                                           HWND hwndAppClient,
                                           PSZ  pszAppName,
                                           PSZ  pszApplicationID,
                                           PSZ  pszVersion);





/*
 * ItiGlobQueryGlobalsPointer returns the pointer to the global varaibles for 
 * the calling process.  
 *
 * Parameters: None.
 *
 * Return Value: This function returns the pointer to the global variables
 * for the calling process.  
 *
 * Comments: If this function is called at DLL initialization
 * time, the pointer points to the uninitialized data.  No process
 * may use any of the global varaibles until habMainThread is set to
 * be NON-NULL.  
 */

extern PGLOBALS EXPORT ItiGlobQueryGlobalsPointer (void);


extern void EXPORT ItiGlobResetImportTestFlag (BOOL bFlag);

extern BOOL EXPORT ItiGlobGetImportTestFlag (void);



/*
 * ItiGlobReinitSystemValues reinitializes the OS/2 system values.
 *
 * Parameters: iFirst:        The index of the first OS/2 system value
 *                            to reinitialize.
 *
 *             iLast:         The index of the last OS/2 system value
 *                            to reinitialize.
 *
 * Return Value: None.
 *
 * Comment: This functino needs to be called when ever the main application 
 * window recieves the WM_SYSVALUECHANGED message.  iFirst and iLast come
 * from mp1 and mp2 of the message.
 */

extern void EXPORT ItiGlobReinitSysValues (int iFirst, int iLast);



#endif /* GLOBALS_INCLUDED */


