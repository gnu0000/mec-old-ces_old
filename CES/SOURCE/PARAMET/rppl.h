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
 *   rPPL.h                                 
 *   Timothy Blake                        
 *     
 */    
#define  DLL_VERSION   1  
#define  TITLE        "BAMS/CES Parametric Profile Listing"  
#define  SUB_TITLE    ""  
#define  FOOTER       ""  

#define  RPT_DLL_NAME "rPPL"  


#define  ID_QUERY_LISTING                539 
#define  SIZE_OF_QUERY_LISTING   128 
#define  NUM_ATTRIB_COLS_LISTING    2 
#define  LISTING_ID                   0 
#define  LISTING_DESC                 1 



#define  ID_QUERY_KEYS                540
#define  SIZE_OF_QUERY_KEYS      128 
#define  NUM_ATTRIB_COLS_KEYS      2
#define  KEY                   0 
#define  ID                    1 



#define  ID_QUERY_MJR_ITEMS                541
#define  SIZE_OF_QUERY_MJR_ITEMS      300
#define  NUM_ATTRIB_COLS_ITEMS    3
#define  MJR_ITEMS_ID       0 
#define  MJR_ITEMS_DESC     1
#define  MJR_ITEMS_PCT      2 



#define  ID_QUERY_TTL                542
#define  SIZE_OF_QUERY_TTL      128 
#define  NUM_ATTRIB_COLS      1
#define  TOTAL               0 



extern USHORT  EXPORT ItiDllQueryVersion (VOID);                             
extern BOOL    EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion);     
extern VOID    EXPORT ItiDllQueryMenuName (PSZ pszBuffer, USHORT  usMaxSize, 
                                           PUSHORT pusWindowID);             
extern BOOL    EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID,      
                                               USHORT usActiveWindowID);     
extern MRESULT EXPORT ItiDllrPPLRptDlgProc (HWND   hwnd, USHORT usMessage,     
                                          MPARAM mp1,  MPARAM mp2);          
extern USHORT  EXPORT ItiRptDLLPrintReport                                   
                           (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[]); 
