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


/*                                                         CES Report  
 *   rUser.h                                                   
 *   Timothy Blake                                          
 *   Copyright (C) 1991 Info Tech, Inc.                     
 */                                                         

#define  DLL_VERSION   1
#define  TITLE        "DS/Shell User Listing"
#define  SUB_TITLE    ""
#define  FOOTER       ""

#define  RPT_DLL_NAME "rUser"

#define  ORDER_SW_VAL_USERID  'u'
#define  ORDER_SW_VAL_NAME    'n'


#define  ID_QUERY_LISTING            220
#define  ID_QUERY_CONJ               221
#define  SIZE_OF_QUERY        128
#define  SIZE_OF_CONJ          60
#define  NUM_ATTRIB_COLS_LST  4
#define  ID                   0
#define  PHONE                1
#define  TYPE                 2
#define  NAME                 3


#define  ID_QUERY_APP                 21
#define  SIZE_OF_QUERY_APP    256
#define  NUM_ATTRIB_COLS_CAT  2
//define ID                   0
#define  DESC                 1


#define  ID_QUERY_CAT                 22
#define  SIZE_OF_QUERY_CAT    256
#define  NUM_ATTRIB_COLS_CAT  2
//define ID                   0
#define  PERM                 1


#define  ID_QUERY_KEYS                23
#define  SIZE_OF_QUERY_KEYS   128
#define  NUM_ATTRIB_COLS_KEYS   1
#define  KEY                    0




extern USHORT  EXPORT ItiDllQueryVersion (VOID);                             

extern BOOL    EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion);     

extern VOID    EXPORT ItiDllQueryMenuName (PSZ pszBuffer, USHORT  usMaxSize, 
                                           PUSHORT pusWindowID);             

extern BOOL    EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID,      
                                               USHORT usActiveWindowID);     

extern MRESULT EXPORT ItiDllrUserRptDlgProc (HWND   hwnd, USHORT usMessage,     
                                             MPARAM mp1,  MPARAM mp2);          
extern USHORT  EXPORT ItiRptDLLPrintReport                                   
                           (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[]); 
