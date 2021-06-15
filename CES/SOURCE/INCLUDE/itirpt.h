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


/*                                                          CES Report Module
 *   ItiRpt.h
 *   Timothy Blake
 *
 *   This file provides the structures and definitions used by all of
 *   the report DLLs and the supporting utility modules for printing
 *   and command line parsing.
 *
 */


#define  INCL_GPI
#define  INCL_WINERRORS


#if !defined (INCL_ITIRPT)
#define  INCL_ITIRPT


#define  ITIRPT_EXE                 "itirpt.exe"

#define  ITIRPT_APP_SECTION         "REPORTS"
         /* This is the name of the application section of the ini file.
          */


         /* NOTICE: The switch characters below must match those used in
          *         itirptut.h's manifest constants ITIRPTUT_..._SWITCH
          */
#define  ITIRPT_OPTSW_ORDERING      " /O "
#define  ITIRPT_OPTSW_KEYFILE       " /K "
#define  ITIRPT_OPTSW_FOOTER        " /F "
#define  ITIRPT_OPTSW_DATE          " /D "


#define  ITIRPT_OPTSW_NEWPG_STR   " /N n "
#define  ITIRPT_OPTSW_NEWPG_CHR   'N'
#define  ITIRPT_OPTSW_NEWPG_VAL   'n'




#define  NOT   !

// #define  DEVELOPMENT_CODE      RPT   
/*   -- Used for conditional compilations. */

/* ======================================================================= *\
 *                                   CONDITIONAL COMPILATION DEFINITIONS   *
\* ======================================================================= */
#ifndef  DEVELOPMENT_CODE 
#define ERRMSG(t)  // should be ItiErrorWindow or something to that effect.
#define DBGMSG(t)  //         
        /* ie. do NOT write a debug message. */
#else
#define ERRMSG(t)    ItiErrWriteDebugMessage ( t )
#define DBGMSG(t)    ItiErrWriteDebugMessage ( t )
#endif


//#if defined (DEBUG)
//#else
#define  ACK         ItiErrWriteDebugMessage ("OK")
#define  ERRORBEEP   ItiErrWriteDebugMessage ("ERROR")
//#endif




/* ======================================================================= *\
 *                                         MANIFEST CONSTANT DEFINITIONS   *
\* ======================================================================= */

#define MAX(A, B)   ((A) > (B) ? (A) : (B))

#define MIN(A, B)   ((A) < (B) ? (A) : (B))


/* ----------------------------------------------------------------------- *\
 *                                                            Error Codes  *
\* ----------------------------------------------------------------------- */
#define  ITIPRT_ERROR_BASE     ITIRPT_ERROR_BASE + 1000

#define  ITIRPTUT_ERROR_BASE   ITIPRT_ERROR_BASE + 1000


#define  ITIRPT_ERR                        (ITIRPT_ERROR_BASE + 11)
#define  ITIRPT_INVALID_PARM               (ITIRPT_ERROR_BASE + 17)
#define  ITIRPT_DLL_ERR                    (ITIRPT_ERROR_BASE + 19)
#define  ITIRPT_MEM_ERR                    (ITIRPT_ERROR_BASE + 23)





/* ----------------------------------------------------------------------- *\
 *                                                      Numeric Constants  *
\* ----------------------------------------------------------------------- */
//#define   ID_STATE_NAME          1
//#define   ID_AGENCY_NAME         2


#define   RPT_EXE_VERSION       "1"
#define   STR_ERR               13
#define   LNLEN                 78
#define   LGARRAY              255
#define   SMARRAY               31
           /* Must not be larger than 32 TOTAL elements; required        *
            * by the DevPostDeviceModes function (parm 4).               */


#define   STATENAME_LEN        (2*SMARRAY+1)
#define   AGENCYNAME_LEN       (2*SMARRAY+1)
#define   TITLE_LEN            (2*SMARRAY+1)
#define   SUBTITLE_LEN         (2*SMARRAY+1)
#define   FOOTER_LEN           (LNLEN+1)


           /* Length of the array to hold the character values of a key. */
#define   LEN_KEYID_SZKEY        6

#define   KEY_TERMINATOR_VAL     0


           /* The maximum number of columns that amy be used in an       *
            * "order by" clause as limited by the current SQL            *
            * implementation.                                            */
#define   IMP_MAX_ORDERING_COLS 15

           /* The current maximum number of option switches (arb). */
#define   MAX_NUM_OPTS          10



#define  FPL                   _far _pascal _loadds 
#define  VERSION_FUNC          "ITIDLLQUERYVERSION"
#define  VER_COMPAT_FUNC       "ITIDLLQUERYCOMPATIBILITY"
#define  RPT_PRT_FUNC          "ITIRPTDLLPRINTREPORT"
   /* -- A function name in a run-time DLL that will print a report. */

#define  FILENAMESIZE          LGARRAY
#define  UNITS                 PU_LOENGLISH




















/* ======================================================================= *\
 *                                        STRUCTURE AND TYPE DEFINITIONS   *
 *                                               used in all report DLLs   *
\* ======================================================================= */
/* The following structure holds data about a context's device. */
typedef struct _DevInfo
           {
           CHAR szDevice     [SMARRAY +1]; 
           CHAR szDetails    [LGARRAY +1];
           CHAR szDeviceName [SMARRAY +1];
           LONG lDeviceType;
           LONG lPsOptions;
           CHAR szDefaultDevice [SMARRAY +1];
           CHAR szDriverName    [LGARRAY +1];
           CHAR szLogPort       [LGARRAY +1];
           DEVOPENSTRUC dop;
           } DEVINFO;       /*  devi */

typedef DEVINFO *PDEVINFO;  /* pdevi */




typedef struct _RepTitles
           {
           CHAR    szStateName  [STATENAME_LEN];
           CHAR    szAgencyName [AGENCYNAME_LEN];
           CHAR    szTitle      [TITLE_LEN];
           CHAR    szSubTitle   [SUBTITLE_LEN];  
           CHAR    szSubSubTitle[SUBTITLE_LEN]; 
           CHAR    szFooter     [FOOTER_LEN];
           } REPTITLES; /* rept */

typedef REPTITLES *PREPT;  /* prept */




typedef struct _RepInfo
           {
           HAB     hab;                       /* h to an anchor block      */
           HMQ     hmq;                       /* h for message queue       */
           HDC     hdc;                       /* h to device context       */
           HPS     hps;                       /* h to presentation space   */
           HWND    hwndWin;                   /* h to a display window     */
           HMODULE hmod;                      /* h of the run-time DLL     */
           CHAR    szDLLName[FILENAMESIZE];   /* Name of the report DLL    */
           CHAR    szStatus[LGARRAY +1];      /* Status of report string   */
           CHAR    szStateName  [STATENAME_LEN];
           CHAR    szAgencyName [AGENCYNAME_LEN];
           CHAR    szTitle      [TITLE_LEN];
           CHAR    szSubTitle   [SUBTITLE_LEN]; 
           CHAR    szSubSubTitle[SUBTITLE_LEN]; 
           CHAR    szFooter     [FOOTER_LEN];
           DEVINFO devi;
           USHORT (EXPORT *func)(PVOID pRes, INT argcnt, CHAR * argvars[]);
                    /* The function in the run-time DLL that will be called. */
           } REPINFO;        /*  repi  */
    
typedef REPINFO *PREPINFO;   /* prepi  */









/*
 * The following are support modules that are always used by a report DLL.
 */

#if !defined (INCL_ITIPRT)
#include "..\include\itiprt.h"
#endif


#if !defined (INCL_RPTUTIL)
#include "..\include\itirptut.h"
#endif


#endif   /* if !defined (INCL_ITIRPT) */
