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
 *   rJEReg.h                               
 *   Timothy Blake                      
 *                           
 */                          

#define  DLL_VERSION   1    

#define  TITLE_LEAD   "BAMS/CES Job "
#define  TITLE_TAIL   " Estimate"
#define  TITLE        TITLE_LEAD TITLE_TAIL

#define  SUB_TITLE    "Job Item Price Bases: PEMETH Regression"
#define  FOOTER       ""    

#define  RPT_DLL_NAME "rJEReg"  



         /* -- Job's primary county and estimate names. */
#define  ID_QRY_TITLE_NAMES                  821
#define  SIZE_OF_QRY_TITLE_NAMES       128

#define  NUM_ATTRIB_COLS_TITLE_NAMES   4
#define  JOB_ID                        0
#define  COUNTY_NAME                   1
#define  COUNTY_ID                     2
#define  COUNTY_KEY                    3



       /* -- JobBreakdowns for this particular Job. */
#define  ID_QRY_JOB_BRKDWN                   822
#define  SIZE_OF_QRY_JOB_BRKDWN        256

#define  NUM_ATTRIB_COLS_JOB_BRKDWN    4
#define  JOB_BRKDWN_KEY                0
#define  JOB_BRKDWN_ID                 1
#define  JOB_BRKDWN_WORK_TYPE          2
#define  JOB_BRKDWN_WORK_TYPE_ID       3

       /* -- Job items associated with a particular Job Breakdown. */
#define  ID_QRY_JOB_ITEM                   823
#define  ID_CONJ_JOB_ITEM                  824
#define  SIZE_OF_QRY_JOB_ITEM        640
#define  SIZE_OF_CONJ_JOB_ITEM       128

#define  NUM_ATTRIB_COLS_JOB_ITEM      5
#define  JOB_ITEM_KEY                0
#define  JOB_ITEM_SIN                1
#define  JOB_ITEM_DESC               2
#define  JOB_ITEM_SIKEY              3
#define  JOB_ITEM_UNIT               4




#define  ID_QRY_BPQ                   830
#define  ID_CONJ_BPQ_CAP              831
#define  ID_CONJ_BPQ_KEY              832
#define  ID_CONJ_BPQ_ITEM_KEY         833
#define  ID_CONJ_BPQ_BRKDWN_KEY       834
#define  SIZE_OF_QRY_BPQ       1500
#define  SIZE_OF_CONJ_BPQ       256

#define  NUM_ATTRIB_COLS_BPQ      11
#define  BPQ_BUP                0
//#define  BPQ_UNIT               1
#define  BPQ_QTY                1
#define  BPQ_QTY_MEDIAN         2
#define  BPQ_QTY_ADJ            3
#define  BPQ_AREA_TYPE_KEY      4
#define  BPQ_LOG_RESULT         5

#define  BPQ_QTYADJ_p_BUP_FMT   6
#define  BPQ_QTYADJ_p_BUP_RAW   7
//       For increases,
//       this is BaseUnitPrice * (1.0 + QuantityAdjustment percentage)

#define  BPQ_QTYADJ_n_BUP_FMT   8
#define  BPQ_QTYADJ_n_BUP_RAW   9
//       For decreases,
//       this is BaseUnitPrice * (1.0 - QuantityAdjustment percentage)





#define  REG_BASE_PRICE_BASE   0
#define  REG_QUANTITY          1

#define  REG_QTY_ADJ           2
#define  REG_MEDIAN_QTY        3
#define  REG_BASE_PRICE        4

#define  REG_AREA_EXT          5
#define  REG_AREA_ADJ          6
#define  REG_AREA_ID           7

#define  REG_SEA_EXT           8
#define  REG_SEA_ADJ           9
#define  REG_SEA_ID           10

#define  REG_WRK_EXT          11
#define  REG_WRK_ADJ          12
#define  REG_WRK_ID           13

#define  REG_REG_VAL          14
#define  REG_LOG_VAL          15 





#define  ID_SLC_ADJ                   870
#define  ID_SLC_ADJ_RAW               871


#define  ID_QRY_AREA                   842
#define  ID_CONJ_AREA_CKEY             843
#define  ID_CONJ_AREA_SIKEY            844
#define  SIZE_OF_QRY_AREA        660

#define  NUM_ATTRIB_COLS_AREA       4
#define  AREA_ADJ_x_BUPQTY        0
#define  AREA_ADJ_x_BUPQTY_RAW    1
#define  AREA_ADJ                 2
#define  AREA_ID                  3


#define  ID_QRY_SEASON                   850
#define  ID_CONJ_SEASON                  851
#define  SIZE_OF_QRY_SEASON        256

#define  NUM_ATTRIB_COLS_SEASON       4
#define  SEASON_ADJ_x_BUPQTY        0
#define  SEASON_ADJ_x_BUPQTY_RAW    1
#define  SEASON_ADJ                 2
#define  SEASON_ID                  3


#define  ID_QRY_WORKTYPE                   860
#define  ID_CONJ_WORKTYPE                  861
#define  SIZE_OF_QRY_WORKTYPE        256

#define  NUM_ATTRIB_COLS_WORKTYPE       4
#define  WORKTYPE_ADJ_x_BUPQTY        0
#define  WORKTYPE_ADJ_x_BUPQTY_RAW    1
#define  WORKTYPE_ADJ                 2
#define  WORKTYPE_ID                  3


#define  ID_CONJ_ADJ_IMPDATE          872
#define  ID_CONJ_ADJ_IMPDATE_CAP      873



#define  ID_QRY_IMP                   880   



#define  ID_QRY_LISTING                899 
#define  ID_WHERE_CONJUNCTION            21 
#define  SIZE_OF_QUERY      128  
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
extern MRESULT EXPORT ItiDllrJERegRptDlgProc (HWND   hwnd, USHORT usMessage,     
                                          MPARAM mp1,  MPARAM mp2);          
extern USHORT  EXPORT ItiRptDLLPrintReport                                   
                           (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[]); 
