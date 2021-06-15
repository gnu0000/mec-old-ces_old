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
*   RCONTY.h                                                   
*   Timothy Blake                                          
*                                                          
*/                                                         
#define  DLL_VERSION   1                                   

#define  TITLE        "BAMS County Listing"                                   
#define  SUB_TITLE    ""                                   
#define  FOOTER       ""                                   

#define  RPT_DLL_NAME "rConty"                                 

#define  ID_QUERY_KEYS                   351
#define  KEY                 0


#define  ID_QUERY_LISTING                350                
#define  ID_WHERE_CONJUNCTION            64

#define  SIZE_OF_QUERY      256
#define  SIZE_OF_CONJ        64
#define  NUM_ATTRIB_COLS      2                            

#define  ID                   0                            
#define  DESC                 1                            



extern USHORT  EXPORT ItiDllQueryVersion (VOID);                             

extern BOOL    EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion);     

extern VOID    EXPORT ItiDllQueryMenuName (PSZ pszBuffer, USHORT  usMaxSize, 
                                           PUSHORT pusWindowID);             

extern BOOL    EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID,      
                                               USHORT usActiveWindowID);     

extern MRESULT EXPORT ItiDllRCONTYRptDlgProc (HWND   hwnd, USHORT usMessage,     
                                              MPARAM mp1,  MPARAM mp2);          

extern USHORT  EXPORT ItiRptDLLPrintReport                                   
                           (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[]); 
