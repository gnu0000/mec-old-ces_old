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
 *   rVend.h                                                   
 *   Timothy Blake                           
 */

#define  DLL_VERSION   1
#define  TITLE        "BAMS Vendor Listing"
#define  SUB_TITLE    ""
#define  FOOTER       ""

#define  RPT_DLL_NAME "rVend" 

#define  OPTSW_INCL_FACIL            " /I "
#define  OPTSW_INCL_FACIL_CHR        'I'

#define  OPTSW_INCL_FACIL_VAL_STR    "i "
#define  OPTSW_INCL_FACIL_VAL_CHR    'i'



#define  ID_QUERY_DESC                360
#define  ID_QUERY_ADDR                361
#define  ID_QUERY_WORK                362

#define  SIZE_OF_QUERY      330

#define  NUM_ATTRIB_COLS_DESC      9
#define  ID                      0
#define  SHRT_NAME               1
#define  NAME                    2
#define  TYPE                    3
#define  CERT                    4
#define  DBEC                    5
#define  X_TYPE                  6
#define  X_CERT                  7
#define  X_DBEC                  8



#define  NUM_ATTRIB_COLS_ADDR      7
#define  ADDR                    0
#define  CITY                    1
#define  STATE                   2
#define  ZIP                     3
#define  PHONE                   4
#define  LNG                     5
#define  LAT                     6


#define  NUM_ATTRIB_COLS_WORK      2
//define ID                      0
#define  TRANS                   1






#define  ID_QUERY_FACIL_NUM                 514
#define  SIZE_OF_QUERY_FACIL_NUM  128

#define  NUM_ATTRIB_COLS_FACIL_NUM  2
                            
#define  VENDOR_KEY                 0
#define  FACIL_NUMBER               1




#define  ID_QUERY_FACIL                     515
#define  SIZE_OF_QUERY_FACIL        840

#define  ID_QUERY_FACIL_CONJ                516
#define  SIZE_OF_QUERY_FACIL_CONJ    80

#define  NUM_ATTRIB_COLS_FACIL  12
                            
#define  DATE_OPENED             0
#define  DATE_CLOSED             1
#define  PORT_FLAG               2
#define  LONGITUDE               3
#define  LATITUDE                4
#define  COUNTY_NAME             5
#define  MAT_ID                  6
#define  CAPACITY                7
#define  MAT_UNITS               8
#define  PROD_METH               9
#define  PROD_DESC              10
#define  APPROVED               11
             



extern USHORT  EXPORT ItiDllQueryVersion (VOID);                             
extern BOOL    EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion);     
extern VOID    EXPORT ItiDllQueryMenuName (PSZ pszBuffer, USHORT  usMaxSize, 
                                           PUSHORT pusWindowID);             
extern BOOL    EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID,      
                                               USHORT usActiveWindowID);     
extern MRESULT EXPORT ItiDllrVendRptDlgProc (HWND   hwnd, USHORT usMessage,     
                                          MPARAM mp1,  MPARAM mp2);          
extern USHORT  EXPORT ItiRptDLLPrintReport                                   
                           (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[]); 
