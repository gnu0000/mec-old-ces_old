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
 *   rPESjo.h                                                   
 *   Timothy Blake                                          
 *                                                          
 */
#define  DLL_VERSION   1

#define  TITLE_LEAD   "BAMS/PES Job "
#define  TITLE_TAIL   "/Project Listing"
#define  TITLE       TITLE_LEAD TITLE_TAIL

#define  SUB_TITLE    ""
#define  FOOTER       ""

#define  RPT_DLL_NAME "rPESjo"


#define  ID_QUERY_KEYS                   1115
#define  SIZE_OF_QUERY_KEYS      80


#define  ID_QUERY_LISTING                1116
#define  SIZE_OF_QUERY_LISTING      128
#define  NUM_ATTRIB_COLS_LISTING      2
#define  LISTING_ID                   0
#define  LISTING_DESC                 1


          
#define  ID_QUERY_PCN                1117
#define  SIZE_OF_QUERY_PCN      256
#define  NUM_ATTRIB_COLS_PCN      2
#define  PCN                  0
#define  PCN_DESC             1



#define  ID_QUERY_CAT                1118
#define  SIZE_OF_QUERY_CAT      128
#define  NUM_ATTRIB_COLS_CAT      1
#define  CAT_ID                   0



#define  ID_QUERY_ITM                1119
#define  SIZE_OF_QUERY_ITM      900
#define  NUM_ATTRIB_COLS_ITM      8  
#define  ITM_NUM           0        
#define  ITM_JOB_QTY       1
#define  ITM_UNIT          2
#define  ITM_PCN           3
#define  ITM_CAT           4
#define  ITM_SEQ           5
#define  ITM_PCN_QTY       6
#define  ITM_DESC          7



extern USHORT  EXPORT ItiDllQueryVersion (VOID);                             
extern BOOL    EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion);     
extern VOID    EXPORT ItiDllQueryMenuName (PSZ pszBuffer, USHORT  usMaxSize, 
                                           PUSHORT pusWindowID);             
extern BOOL    EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID,      
                                               USHORT usActiveWindowID);     
extern MRESULT EXPORT ItiDllrPESjoRptDlgProc (HWND   hwnd, USHORT usMessage,     
                                          MPARAM mp1,  MPARAM mp2);          
extern USHORT  EXPORT ItiRptDLLPrintReport                                   
                           (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[]); 
