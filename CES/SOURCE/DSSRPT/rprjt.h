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
 *   rPrjt.h                                                   
 *   Timothy Blake                                          
 */                                                         
#define  DLL_VERSION   1
#define  TITLE        "BAMS/DSS Project Listing"
#define  SUB_TITLE    "Including Awarded, Low Bid, and Estimated Prices"
#define  FOOTER       ""

#define  RPT_DLL_NAME "rPrjt "


#define  OPT_SW_SHORT_DESC_CHR    'S'
#define  OPT_SW_SHORT_DESC_STR   " /S s "


#define  ID_QUERY_LISTING               5250                
#define  ID_WHERE_CONJUNCTION           5251               

#define  SIZE_OF_QUERY_LISTING          1023
#define  SIZE_OF_CONJ                     80

#define  NUM_ATTRIB_COLS     18

#define  PCN                  0
#define  PROJ_NUM             1
#define  DESC                 2
#define  PROP_ID              3

#define  WRK_TYPE             4
#define  WRK_TRAN             5
#define  CLASS                6
#define  CLASS_TRAN           7
#define  DIST                 8
#define  DIST_TRAN            9

#define  SPEC_YR             10
#define  LOC                 11
#define  LENGTH              12
#define  WIDTH               13


#define  AMT_AWARD           14
#define  AMT_LOW             15
#define  AMT_EST             16
#define  PROP_KEY            17




#define  ID_QUERY_BIDKEYS                5252

#define  SIZE_OF_QUERY_BIDKEYS            255

#define  NUM_ATTRIB_COLS_BIDKEYS   2

#define  AWARDED_KEY           0
#define  LOWBID_KEY            1




#define  ID_QUERY_CAT                5253
#define  ID_QUERY_CAT_CONJ           5254

#define  SIZE_OF_QUERY_CAT            255

#define  NUM_ATTRIB_COLS_CAT   9

#define  CAT_NUM               0
#define  CAT_DESC              1
#define  CAT_LEN               2
#define  CAT_WTH               3
#define  CAT_SID               4
#define  CAT_SLEN              5
#define  CAT_SWTH              6
#define  CAT_STYP              7
#define  CAT_STYPE_TRANS       8




#define  ID_QUERY_CATKEYS                5255

#define  SIZE_OF_QUERY_CATKEYS            255

#define  NUM_ATTRIB_COLS_CATKEYS   2

#define  CAT_PKEY               0
#define  CAT_CN                 1



#define  ID_QUERY_ITEMS                5256
#define  ID_QUERY_ITEMS_CONJ           5257
#define  ID_QUERY_ITEMS_ORDER          5258

#define  SIZE_OF_QUERY_ITEMS            700

#define  NUM_ATTRIB_COLS_ITEMS   6

#define  ITM_NUM               0
#define  ITM_QTY               1
#define  ITM_LONG_DESC         2
#define  ITM_SHORT_DESC        3
#define  ITM_PROP_ITEM_KEY     4
#define  ITM_UNIT              5




#define  ID_QUERY_AWD_SLCT           5259
#define  ID_QUERY_AWD                5260
#define  ID_QUERY_AWD_CONJ           5261

#define  SIZE_OF_QUERY_AWD            700

#define  NUM_ATTRIB_COLS_AWD   3

#define  AWD_UNIT_PRICE        0
#define  AWD_LOWCOST_ALT       1
#define  AWD_EXT               2





#define  ID_QUERY_LOW_SLCT           5262
#define  ID_QUERY_LOW                5263
#define  ID_QUERY_LOW_CONJ           5264

#define  SIZE_OF_QUERY_LOW            700

#define  NUM_ATTRIB_COLS_LOW   3

#define  LOW_UNIT_PRICE        0
#define  LOW_LOWCOST_ALT       1
#define  LOW_EXT               2





#define  ID_QUERY_EST_SLCT           5265
#define  ID_QUERY_EST                5266
#define  ID_QUERY_EST_CONJ           5267

#define  SIZE_OF_QUERY_EST            700

#define  NUM_ATTRIB_COLS_EST   3

#define  EST_UNIT_PRICE        0
#define  EST_LOWCOST_ALT       1
#define  EST_EXT               2





extern USHORT  EXPORT ItiDllQueryVersion (VOID);                            

extern BOOL    EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion);    


extern VOID    EXPORT ItiDllQueryMenuName (PSZ pszBuffer, USHORT  usMaxSize,
                                           PUSHORT pusWindowID);            

extern BOOL    EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID,     
                                               USHORT usActiveWindowID);    

extern MRESULT EXPORT ItiDllrPrjtRptDlgProc (HWND   hwnd, USHORT usMessage, 
                                             MPARAM mp1,  MPARAM mp2);      

extern USHORT  EXPORT ItiRptDLLPrintReport                                  
                           (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[]);
