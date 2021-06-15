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
 *   rCrew.h                                                   
 *   Timothy Blake                                          
 *                                                          
 */                                                         
#define  DLL_VERSION   1
#define  TITLE        "BAMS/CES Crew Catalog Listing"
#define  SUB_TITLE    ""
#define  FOOTER       ""
#define  RPT_DLL_NAME "rCrew"

#define  ID_QUERY_LISTING                967
#define  SIZE_OF_QUERY_LISTING      128
#define  NUM_ATTRIB_COLS_LISTING      2

#define  ID                   0
#define  DESC                 1



#define  ID_QUERY_EQP                968
#define  SIZE_OF_QUERY_EQP      256
#define  NUM_ATTRIB_COLS_EQP      2

#define  EQP_CNT                  0
#define  EQP_ID                   1
#define  EQP_DESC                 2



#define  ID_QUERY_LBR                969
#define  SIZE_OF_QUERY_LBR      256
#define  NUM_ATTRIB_COLS_LBR      2

#define  LBR_CNT                  0
#define  LBR_ID                   1
#define  LBR_DESC                 2



#define  ID_QUERY_ALL                970
#define  SIZE_OF_QUERY_ALL      128
#define  NUM_ATTRIB_COLS_ALL      2

#define  CREW_KEY                 0


extern USHORT  EXPORT ItiDllQueryVersion (VOID);                             
extern BOOL    EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion);     
extern VOID    EXPORT ItiDllQueryMenuName (PSZ pszBuffer, USHORT  usMaxSize, 
                                           PUSHORT pusWindowID);             
extern BOOL    EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID,      
                                               USHORT usActiveWindowID);     
extern MRESULT EXPORT ItiDllrCrewRptDlgProc (HWND   hwnd, USHORT usMessage,     
                                             MPARAM mp1,  MPARAM mp2);          
extern USHORT  EXPORT ItiRptDLLPrintReport                                   
                           (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[]); 
