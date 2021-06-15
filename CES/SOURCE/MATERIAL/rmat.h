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
*   RMAT.h                                                   
*   Timothy Blake                                          
*                                                          
*/                                                         
#define  DLL_VERSION   1                                   

#define  TITLE        "BAMS Material Listing"                                   
#define  SUB_TITLE    ""                                   
#define  FOOTER       ""                                   

#define  RPT_DLL_NAME "rMat"                                 

#define  RMAT_OPTSW_CURUT_E     " /0 "
#define  RMAT_CURUT_E_SWITCH      '0'
#define  RMAT_CURUT_E_CHAR        "0"

#define  RMAT_OPTSW_CURUT_M     " /1 "
#define  RMAT_CURUT_M_SWITCH      '1'
#define  RMAT_CURUT_M_CHAR        "1"


#define  RMAT_OPTSW_INCL_ZP     " /P "
#define  RMAT_INCL_ZP_SWITCH      'P'
#define  RMAT_INCL_ZP_CHAR        "P"

#define  RMAT_OPTSW_INCL_FC     " /C "
#define  RMAT_INCL_FC_SWITCH      'C'
#define  RMAT_INCL_FC_CHAR        "C"



#define  ID_QUERY_KEYS                   982

#define  ID_QUERY_LISTING                512                
#define  ID_WHERE_CONJUNCTION             55                
#define  SIZE_OF_QUERY      512
#define  SIZE_OF_CONJ        64                            

#define  NUM_ATTRIB_COLS      4
                            
#define  ID                   0
#define  UNIT                 1
#define  ZTYPE                2
#define  DESC                 3  



#define  ID_QUERY_PRICES           513
#define  ID_QUERY_PRICES_CONJ      517

#define  NUM_ATTRIB_COLS_PRICES      3
                            
#define  ZONE_ID              0
#define  UNIT_PRICE           1
#define  MAT_ID               2
         




#define  ID_QUERY_FACIL            514

#define  NUM_ATTRIB_COLS_FACIL      7
                            
#define  FACIL_NUM            0
#define  CAPACITY             1
#define  MAT_UNIT             2
#define  APPROVED             3
#define  PROD_ID              4
#define  PROD_TRANS           5
#define  FMAT_ID              6
         


                 

extern USHORT  EXPORT ItiDllQueryVersion (VOID);                             

extern BOOL    EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion);     

extern VOID    EXPORT ItiDllQueryMenuName (PSZ pszBuffer, USHORT  usMaxSize, 
                                           PUSHORT pusWindowID);             

extern BOOL    EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID,      
                                               USHORT usActiveWindowID);     

extern MRESULT EXPORT ItiDllRMATRptDlgProc (HWND   hwnd, USHORT usMessage,     
                                            MPARAM mp1,  MPARAM mp2);          

extern USHORT  EXPORT ItiRptDLLPrintReport                                   
                           (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[]); 
