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
 *   rJEAvg.h                                                   
 *   Timothy Blake                                          
 *                                                          
 */ 

#define  DLL_VERSION   1

#define  TITLE_LEAD   "BAMS/CES Job "
#define  TITLE_TAIL   " Estimate"
#define  TITLE        TITLE_LEAD TITLE_TAIL
#define  SUB_TITLE    "Job Item Price Bases: PEMETH Averages"
#define  FOOTER       ""

#define  RPT_DLL_NAME "rJEAvg"



#define  OPT_SW_BASEDATE         'B'

#define  ITEMS_NO_PEMETH_DATA    "ALLITEMS"
      /* -- This environment variable (when set to 'Y' or 'y') will *
       * -- include selected items that did not have PEMETH data    *
       * -- in the database.                                        */


         /* -- Job's primary county and estimate names. */
#define  ID_QRY_TITLE_NAMES                 716
#define  SIZE_OF_QRY_TITLE_NAMES       512

#define  NUM_ATTRIB_COLS_TITLE_NAMES    5
#define  JOB_ID                        0
#define  COUNTY_NAME                   1
#define  COUNTY_ID                     2
#define  COUNTY_KEY                    3
#define  JOB_BASE_DATE                 4



       /* -- JobBreakdowns for this particular Job. */
#define  ID_QRY_JOB_BRKDWN                  717
#define  SIZE_OF_QRY_JOB_BRKDWN        256

#define  NUM_ATTRIB_COLS_JOB_BRKDWN    3
#define  JOB_BRKDWN_KEY                0
#define  JOB_BRKDWN_ID                 1
#define  JOB_BRKDWN_WORK_TYPE          2



       /* -- Job items associated with a particular Job Breakdown. */
#define  ID_QRY_JOB_ITEM                    720
#define  ID_CONJ_JOB_ITEM                   721
#define  SIZE_OF_QRY_JOB_ITEM        512
#define  SIZE_OF_CONJ_JOB_ITEM       256

#define  NUM_ATTRIB_COLS_JOB_ITEM      5
#define  JOB_ITEM_KEY                0
#define  JOB_ITEM_SIN                1
#define  JOB_ITEM_DESC               2
#define  JOB_ITEM_SIKEY              3
#define  JOB_ITEM_QTY                4



#define  ID_QRY_PCTS                        722
#define  ID_CONJ_PCTS                       723
#define  SIZE_OF_QRY_PCTS        320
#define  SIZE_OF_CONJ_PCTS       128

#define  NUM_ATTRIB_COLS_PCTS      7
#define  PCTS_AREA_KEY          0
#define  PCTS_5                 1
#define  PCTS_25                2
#define  PCTS_50                3
#define  PCTS_75                4
#define  PCTS_95                5
#define  PCTS_IMPORT_DATE       6



#define ID_QRY_AREA_INFO                    727
#define  SIZE_OF_QRY_AREA_INFO         560
                                
#define ID_CONJ_AREA_INFO_BDK               728
#define ID_CONJ_AREA_INFO_SIK               729

#define  NUM_ATTRIB_COLS_AREA_INFO  1
#define  AREA_INFO_ID               0





#define ID_QRY_AREA_TYPE_INFO               730
#define  SIZE_OF_QRY_AREA_TYPE_INFO    512
                                
#define  AREA_TYPE_INFO_ID         0



#define ID_QRY_WORK_TYPE_INFO               735
#define  SIZE_OF_QRY_WORK_TYPE_INFO    256
                                
#define  WORK_TYPE_INFO_ID         0
#define  WORK_TYPE_INFO_DESC       1




//////////////////

#define  ID_QRY_PRICES                 740
#define  ID_QRY_NUMBER                 741
#define  ID_QRY_LISTING_CONJ           742
 
#define  ID_QRY_PRICES_ORDER           743
#define  ID_QRY_LISTING_ORDER          744
#define  ID_QRY_PRICES_BASEDATE        745

#define  ID_CONJ_PRICES_JOB_KEY        746
#define  ID_CONJ_PRICES_JOB_BD_KEY     747


#define  SIZE_OF_QRY_PRICES      1500

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


#define  NUM_COLS_PRICES      6
//                   ppsz[ ]
//define STD_ITEM_NUM     0
#define  QTY_LEVEL        1
#define  WRK_TYPE         2
#define  AREA             3
#define  WT_AVG_PRICE     4
#define  AREA_KEY         5




//////////////////

#define  ID_QUERY_LISTING                790
#define  ID_WHERE_CONJUNCTION             17                

#define  SIZE_OF_CONJ        32                            
#define  NUM_ATTRIB_COLS      2                            
#define  ID                   0                            
#define  DESC                 1




extern USHORT  EXPORT ItiDllQueryVersion (VOID);

extern BOOL    EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion);

extern VOID    EXPORT ItiDllQueryMenuName (PSZ pszBuffer, USHORT  usMaxSize, 
                                           PUSHORT pusWindowID);

extern BOOL    EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID,      
                                               USHORT usActiveWindowID);

extern MRESULT EXPORT ItiDllrJEAvgRptDlgProc (HWND   hwnd, USHORT usMessage,     
                                              MPARAM mp1,  MPARAM mp2);

extern USHORT  EXPORT ItiRptDLLPrintReport                                   
                           (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[]); 
