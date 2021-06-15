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
*   Rfund.h                                                   
*   Timothy Blake                                          
*                                                          
*/                                                         
#define  DLL_VERSION   1
#define  TITLE        "BAMS Fund Listing"
#define  SUB_TITLE    ""
#define  FOOTER       ""

#define  RPT_DLL_NAME "rFund"

#define  ID_QUERY_LISTING                351                
#define  ID_WHERE_CONJUNCTION             51                
#define  ID_WHERE_OR_CONJ                 52

#define  SIZE_OF_QUERY     1048                            
#define  SIZE_OF_CONJ       128
#define  NUM_ATTRIB_COLS      3                            

#define  ID                   0
#define  NAME                 1
#define  DESC                 2


extern USHORT  EXPORT ItiDllQueryVersion (VOID);                             
extern BOOL    EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion);     
extern VOID    EXPORT ItiDllQueryMenuName (PSZ pszBuffer, USHORT  usMaxSize, 
                                           PUSHORT pusWindowID);             
extern BOOL    EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID,      
                                               USHORT usActiveWindowID);     
extern MRESULT EXPORT ItiDllrFundRptDlgProc (HWND   hwnd, USHORT usMessage,     
                                             MPARAM mp1,  MPARAM mp2);          
extern USHORT  EXPORT ItiRptDLLPrintReport                                   
                           (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[]); 
