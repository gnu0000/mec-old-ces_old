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


#define  CODETABL_DLL_VERSION  1

/*             
 *   rPgTest.h
 *   Timothy Blake
 *   Copyright (C) 1992 Info Tech, Inc.
 *
 */


#define  PREFACE      "DS/Shell "
#define  REPORT_NAME  "Page Test"
#define  TITLE        PREFACE REPORT_NAME
#define  SUB_TITLE    " "
#define  FOOTER       "Footer"

#define  RPT_DLL_NAME "rPgTest"


#define  ID_QUERY_CODE_TABLE_LISTING     2180
#define  ID_WHERE_CONJUNCTION             180

#define  SIZE_OF_QUERY      250
#define  SIZE_OF_CONJ        64
#define  NUM_ATTRIB_COLS      4

#define  CODETAB              0
#define  DESC                 1
#define  CODEVAL              2
#define  TRANS                3





USHORT  EXPORT ItiDllQueryVersion (VOID);
BOOL    EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion);
VOID    EXPORT ItiDllQueryMenuName (PSZ     pszBuffer, USHORT usMaxSize,
                                    PUSHORT pusWindowID);
BOOL    EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID,
                                        USHORT usActiveWindowID);
MRESULT EXPORT ItiDllCodeTabRptDlgProc (HWND   hwnd, USHORT usMessage,
                                        MPARAM mp1,  MPARAM mp2);
USHORT  EXPORT ItiRptDLLPrintReport
                  (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[]);
 

