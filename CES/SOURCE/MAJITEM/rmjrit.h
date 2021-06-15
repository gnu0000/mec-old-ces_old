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
 *   rMjrIt.h                                                   
 *   Timothy Blake                                          
 */                                                         

#define  DLL_VERSION   1
#define  TITLE        "BAMS Major Item Listing"
#define  SUB_TITLE    ""
#define  FOOTER       ""

#define  RPT_DLL_NAME "rMjrIt"

#define  ID_QUERY_DESC                   352
#define  ID_QUERY_LISTING                353
#define  SIZE_OF_QUERY      712

#define  NUM_ATTRIB_COLS_DESC      6
#define  ID                   0
#define  DESC                 1
#define  UNIT                 2
#define  TRANS                3
#define  SPECYR               4
#define  UNITTYPE             5

#define  NUM_ATTRIB_COLS_LISTING   6
#define  STD_NUM              0
//define DESC                 1
//define UNIT                 2
#define  QCF                  3
#define  COMM                 4
#define  PCT                  5



#define  ID_QRY_BASEDATE                354
#define  SIZE_OF_QRY_BASEDATE      164

#define  NUM_ATTRIB_COLS_BASEDATE  1
#define  BASEDATE_MAX             0




#define  ID_QRY_PRICE_REGRS                355
#define  ID_CNJ_PRICE_REGRS                356
#define  SIZE_OF_QRY_PRICE_REGRS      512

#define  NUM_ATTRIB_COLS_PRICE_REGRS    4
#define  PRICE_REGRS_MED_QTY              0
#define  PRICE_REGRS_UNIT                 1
#define  PRICE_REGRS_MED_QTY_PRICE        2
#define  PRICE_REGRS_QTY_ADJ              3




#define  ID_QRY_QVAR                357
#define  ID_CNJ_QVAR                358
#define  SIZE_OF_QRY_QVAR      200

#define  NUM_ATTRIB_COLS_QVAR  1
#define  QVAR             0


#define  ID_QRY_QV_ADJ                360
#define  ID_QRY_QEST_ADJ              361

#define  ID_CNJ_QV_ADJ                362
#define  ID_CNJ_QV_ADJ_BASEDATE       363
#define  SIZE_OF_QRY_QV_ADJ      200

#define  NUM_ATTRIB_COLS_QV_ADJ  2
#define  QVALUE           0
#define  QV_ADJ         1





#define  ID_QRY_QTY_REGRS                370
#define  ID_CNJ_QTY_REGRS                371
#define  SIZE_OF_QRY_QTY_REGRS      512

#define  NUM_ATTRIB_COLS_QTY_REGRS    8
#define  QTY_REGRS_BASE_COEF    0
#define  QTY_REGRS_DPTH         1
#define  QTY_REGRS_WTH          2
#define  QTY_REGRS_XSEC         3
#define  QTY_REGRS_S_BASE       4
#define  QTY_REGRS_S_LEN        5
#define  QTY_REGRS_S_WTH        6
#define  QTY_REGRS_S_AREA       7







extern USHORT  EXPORT ItiDllQueryVersion (VOID);                             
extern BOOL    EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion);     
extern VOID    EXPORT ItiDllQueryMenuName (PSZ pszBuffer, USHORT  usMaxSize, 
                                           PUSHORT pusWindowID);             
extern BOOL    EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID,      
                                               USHORT usActiveWindowID);     
extern MRESULT EXPORT ItiDllrMjrItRptDlgProc (HWND   hwnd, USHORT usMessage,     
                                              MPARAM mp1,  MPARAM mp2);          
extern USHORT  EXPORT ItiRptDLLPrintReport                                   
                           (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[]); 
