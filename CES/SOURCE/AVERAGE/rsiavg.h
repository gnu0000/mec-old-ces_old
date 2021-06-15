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
*   rSIAvg.h                                                   
*   Timothy Blake                                          
*                                                          
*/                                                         
#define  DLL_VERSION   1                                   
#define  TITLE        "BAMS/CES Standard Item Listing"
#define  SUB_TITLE    "Standard Item Price Bases: PEMETH Averages"
#define  FOOTER       ""
#define  RPT_DLL_NAME "rSIAvg"                                 


#define  OPT_SW_BASEDATE         'B'


#define  ITEMS_NO_PEMETH_DATA    "ALLITEMS"
      /* -- This environment variable (when set to 'Y' or 'y') will *
       * -- include selected items that did not have PEMETH data    *
       * -- in the database.                                        */




#define  ID_QUERY_LISTING                713
#define  ID_QUERY_PRICES                 714
#define  ID_QUERY_NUMBER                 715
#define  ID_QUERY_LISTING_CONJ           716
 
#define  ID_QUERY_PRICES_ORDER           717
#define  ID_QUERY_LISTING_ORDER          718
#define  ID_QUERY_PRICES_BASEDATE        719

#define  SIZE_OF_QUERY      640

#define  NUM_COLS_LISTING     9
//                   ppsz[ ]
#define  STD_ITEM_NUM     0
#define  DESC             1
#define  IMPORT_DATE      2
#define  AREA_ID          3
#define  PCTL_5           4
#define  PCTL_25          5
#define  PCTL_50          6
#define  PCTL_75          7
#define  PCTL_95          8


#define  NUM_COLS_PRICES      5
//                   ppsz[ ]
//define STD_ITEM_NUM     0
#define  QTY_LEVEL        1
#define  WRK_TYPE         2
#define  AREA             3
#define  WT_AVG_PRICE     4




extern USHORT  EXPORT ItiDllQueryVersion (VOID);                             
extern BOOL    EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion);     
extern VOID    EXPORT ItiDllQueryMenuName (PSZ pszBuffer, USHORT  usMaxSize, 
                                           PUSHORT pusWindowID);             
extern BOOL    EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID,      
                                               USHORT usActiveWindowID);     
extern MRESULT EXPORT ItiDllrSIAvgRptDlgProc (HWND   hwnd, USHORT usMessage,     
                                          MPARAM mp1,  MPARAM mp2);          
extern USHORT  EXPORT ItiRptDLLPrintReport                                   
                           (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[]); 
