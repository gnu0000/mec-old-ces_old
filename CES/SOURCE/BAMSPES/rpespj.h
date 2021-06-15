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
 *   rPESpj.h                                                   
 *   Timothy Blake                                          
 *                                                          
 */                                                         
#define  DLL_VERSION   1
#define  TITLE        "BAMS/PES Project Listing"
#define  SUB_TITLE    ""
#define  FOOTER       ""

#define  RPT_DLL_NAME "rPESpj"


#define  OPT_SW_NEWPG_STR   " /N n "
#define  OPT_SW_NEWPG_CHR   'N'
#define  OPT_SW_NEWPG_VAL   'n'



#define  ID_QUERY_PCN             1117
#define  PCN           0


#define  ID_QUERY_LISTING          1118
#define  SIZE_OF_QUERY_LISTING      640

#define  NUM_ATTRIB_COLS_LISTING      7

//define PCN                          0
#define  DESC                         1
#define  WRK_ID                       2
#define  WRK_TRNS                     3
#define  DIST_ID                      4
#define  YEAR                         5
#define  LOC                          6





#define  ID_QUERY_EST_AMT          1119
#define  SIZE_OF_QUERY_EST_AMT      128

#define  NUM_ATTRIB_COLS_EST_AMT      1

#define  EST_AMT                    0



#define  ID_QUERY_CATGS          1120
#define  SIZE_OF_QUERY_CATGS      128

#define  NUM_ATTRIB_COLS_CATGS      1

#define  CATEGORY_ID               0



// Catagory Item Stuff

#define  ID_QRY_CAT_ITM_TTL    1129
#define  CAT_ITM_TTL   0

#define  ID_QRY_CAT_ITM        1130

#define  SEQ          0
#define  ITEM         1
#define  QUAN         2
#define  PRICE        3
#define  UNIT         4
#define  EXT          5
#define  ITM_DESC     6
#define  EXT_RAW      7

//#define  STD_ITM_KEY  7




#define  ID_QRY_CAT_ITM_C1     1131
#define  ID_QRY_CAT_ITM_C2     1132
#define  ID_QRY_CAT_ITM_C3     1133
#define  ID_QRY_CAT_ITM_C4     1134
#define  ID_QRY_CAT_ITM_C5     1135
#define  ID_QRY_CAT_ITM_ALT    1136
#define  ID_QRY_CAT_ITM_NONALT 1137

#define  ID_QRY_CAT_ITM_CHOS     1140
#define  ID_QRY_CAT_ITM_CHOS_CAT 1141
#define  ID_QRY_CAT_ITM_CHOS_SEQ 1142


#define  SIZE_OF_QRY_CAT_ITM         1023
#define  SIZE_OF_QRY_CAT_ITM_CHOS     512

#define  SIZE_OF_QRY_CAT_ITM_TTL      256
#define  SIZE_OF_QRY_CAT_ITM_C1       256
#define  SIZE_OF_QRY_CAT_ITM_C2        30
#define  SIZE_OF_QRY_CAT_ITM_C3       320




extern USHORT  EXPORT ItiDllQueryVersion (VOID);                             
extern BOOL    EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion);     
extern VOID    EXPORT ItiDllQueryMenuName (PSZ pszBuffer, USHORT  usMaxSize, 
                                           PUSHORT pusWindowID);             
extern BOOL    EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID,      
                                               USHORT usActiveWindowID);     
extern MRESULT EXPORT ItiDllrPESpjRptDlgProc (HWND   hwnd, USHORT usMessage,     
                                              MPARAM mp1,  MPARAM mp2);          
extern USHORT  EXPORT ItiRptDLLPrintReport                                   
                           (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[]); 
