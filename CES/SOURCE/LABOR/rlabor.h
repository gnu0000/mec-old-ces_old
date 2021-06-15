/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1992 by AASHTO.  All Rights Reserved.                 |
|                                                                           |
|       This program module is part of BAMS/CES, a module of BAMS,          |
|       The American Association of State Highway and Transportation        |
|       Officials' (AASHTO) Information System for Managing Transportation  |
|       Programs, a proprietary product of AASHTO, no part of which may be  |
|       reproduced or transmitted in any form or by any means, electronic,  |
|       mechanical, or otherwise, including photocopying and recording      |
|       or in connection with any information storage or retrieval          |
|       system, without permission in writing from AASHTO.                  |
|                                                                           |
+--------------------------------------------------------------------------*/


/*                                        BAMS/CES Report  
 *   rLabor.h                                                   
 *   Timothy Blake                                          
 * 
 */

#define  DLL_VERSION   1
#define  TITLE        "BAMS/CES Labor Listing"
#define  SUB_TITLE    ""
#define  FOOTER       ""

#define  RPT_DLL_NAME "rLabor"




#define  ID_QUERY_ALL                    970
#define  SIZE_OF_QUERY_ALL         128

#define  NUM_ATTRIB_COLS_ALL         1
#define  LBR_KEY     0




#define  ID_QUERY_LISTING                971
#define  SIZE_OF_QUERY_LISTING      256

#define  NUM_ATTRIB_COLS_LISTING      3
#define  ID         0
#define  DESC       1
#define  ZONE_TYPE  2




#define  ID_QUERY_RATES                  972
#define  SIZE_OF_QUERY_RATES        640

#define  NUM_ATTRIB_COLS_RATES        5
#define  ZONE_ID     0
#define  DBR         1
#define  DBOR        2
#define  NDBR        3
#define  NDBOR       4



extern USHORT  EXPORT ItiDllQueryVersion (VOID);                             
extern BOOL    EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion);     
extern VOID    EXPORT ItiDllQueryMenuName (PSZ pszBuffer, USHORT  usMaxSize, 
                                           PUSHORT pusWindowID);             
extern BOOL    EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID,      
                                               USHORT usActiveWindowID);     
extern MRESULT EXPORT ItiDllrLaborRptDlgProc (HWND   hwnd, USHORT usMessage,     
                                              MPARAM mp1,  MPARAM mp2);          
extern USHORT  EXPORT ItiRptDLLPrintReport                                   
                           (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[]); 
