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
 *   rSIReg.h                                 
 *   Timothy Blake                        
 */    
#define  DLL_VERSION   1  

#define  TITLE        "BAMS/CES Standard Item Listing"  
#define  SUB_TITLE    "Standard Item Price Bases: PEMETH Regression"  
#define  FOOTER       ""  

#define  RPT_DLL_NAME "rSIReg"  

#define  ID_QUERY_LISTING                818 
#define  SIZE_OF_QUERY_LISTING       900
#define  NUM_ATTRIB_COLS_LISTING  14
#define  SIN                       0
#define  DESC                      1
#define  IMPDATE                   2
#define  AREA                      3
#define  MIN_QTY                   4
#define  MAX_QTY                   5
#define  MED_QTY                   6
#define  CODE                      7
#define  BASE_DATE                 8
#define  BASE_PRICE                9 
#define  INFLA_YR                 10
#define  INFLA_MO                 11
#define  ADJ                      12
#define  QRY_IMPDATE              13

 

#define  ID_CONJ_SAW                    819
#define  SIZE_OF_CONJ_SAW          64

#define  ADJ_TXT       0
#define  ADJ_PCT       1


#define  ID_QUERY_AREA                  820
#define  SIZE_OF_QUERY_AREA         640


#define  ID_QUERY_SEASON                821 
#define  SIZE_OF_QUERY_SEASON       640


#define  ID_QUERY_WRKTYP                822 
#define  SIZE_OF_QUERY_WRKTYP       640

               




extern USHORT  EXPORT ItiDllQueryVersion (VOID);                             
extern BOOL    EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion);     
extern VOID    EXPORT ItiDllQueryMenuName (PSZ pszBuffer, USHORT  usMaxSize, 
                                           PUSHORT pusWindowID);             
extern BOOL    EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID,      
                                               USHORT usActiveWindowID);     
extern MRESULT EXPORT ItiDllrSIRegRptDlgProc (HWND   hwnd, USHORT usMessage,     
                                          MPARAM mp1,  MPARAM mp2);          
extern USHORT  EXPORT ItiRptDLLPrintReport                                   
                           (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[]); 
