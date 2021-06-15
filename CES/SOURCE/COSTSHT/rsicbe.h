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
 *   rSICbe.h                                 
 *   Timothy Blake                        
 *     
 */    

#define  DLL_VERSION   1  

#define  TITLE_LEAD   "BAMS/CES"
#define  TITLE_TAIL   " Standard Item Listing"
#define  TITLE        TITLE_LEAD TITLE_TAIL

#define  SUB_TITLE    "Standard Item Price Bases: Cost-Based Estimate"  
#define  FOOTER       ""  

#define  RPT_DLL_NAME "rSICbe"  




// Subsections:   (Don't monkey with these RELATIVE indices.)
#define  BRKDWN        0
#define  BRKDWN_OFF    5

#define  MATERIAL      6
#define  MATERIAL_OFF  6

#define  CREW         13
#define  CREW_OFF      9

#define SUBHEADER_INX   RPT_TTL_28




/* -- Std.Item info */
#define  ID_QUERY_STDITM_INFO                900
#define  SIZE_OF_QUERY_STDITM_INFO      256
#define  NUM_ATTRIB_COLS_STDITM_INFO      2
#define  STDITM_NUM                  0
#define  STDITM_DESC                 1



#define  ID_QUERY_CBESTS_CNT                901
#define  SIZE_OF_QUERY_CBESTS_CNT      128
#define  NUM_ATTRIB_COLS_CBESTS_CNT      1
#define  COUNT                       0


#define  ID_QUERY_CBE_INFO                  902
#define  SIZE_OF_QUERY_CBE_INFO        300
#define  NUM_ATTRIB_COLS_CBE_INFO        4
#define  CBE_KEY                     0
#define  CBE_ID                      1
#define  CBE_PREF                    2
#define  CBE_DESC                    3









//////////////////////////////////
/* -- Job/BrkDwns */
#define  ID_QUERY_JBDK                     800
#define  SIZE_OF_QUERY_JBDK      256
#define  NUM_ATTRIB_COLS_JBDK      2
#define  JBDK_KEY               0
#define  JBDK_ID                1


/* -- Job/Items */
#define  ID_QUERY_JIK                      801
#define  ID_CONJ_JIK                       802
#define  SIZE_OF_QUERY_JIK       400
#define  NUM_ATTRIB_COLS_JIK       2
#define  JIK_KEY                0
#define  JIK_ID                 1



/* -- Job/Items/CostBasedEst */
#define  ID_QUERY_JICBEK                   803
#define  ID_CONJ_JICBEK                    804
#define  SIZE_OF_QUERY_JICBEK       256
#define  NUM_ATTRIB_COLS_JICBEK       2
#define  JICBEK_KEY                0
#define  JICBEK_ID                 1



/* -- JobBrkDwnItem */
#define  ID_QRY_JOB_BRKDWN_ITEM                805
#define  SIZE_OF_QRY_JOB_BRKDWN_ITEM   800
#define  ID_CONJ_JOB_BRKDWN_ITEM_JBDK          806
#define  ID_CONJ_JOB_BRKDWN_ITEM_JIK           807
#define  ID_CONJ_JOB_BRKDWN_ITEM_JICBEK        808
#define  NUM_ATTRIB_COLS_JOB_BRKDWN_ITEM    6
#define  JOB_BDK_ID               0
#define  JOB_BDK_SIN              1
#define  JOB_BDK_DESC             2
#define  JOB_BDK_CBE_ID           3
#define  JOB_BDK_CBE_DESC         4
#define  JOB_BDK_PREC             5

////////////////////////

#define  ID_QRY_ITEM_CBE_TASK                909
#define  SIZE_OF_QRY_ITEM_CBE_TASK      256     


//#define  ID_QRY_ITEM_CBE_TASK_TTL            910
//#define  SIZE_OF_QRY_ITEM_CBE_TASK_TTL  256     
//
//#define  JOB_TASK_TTL_EXT          0
//#define  JOB_TASK_TTL_QTY          1
//#define  JOB_TASK_TTL_UNIT_PRICE   2
//
//
//#define  ID_QRY_ITEM_CBE_TASK_FW             911
//#define  SIZE_OF_QRY_ITEM_CBE_TASK_FW   256     

//#define  ID_CONJ_JOB_TASK_ITEM_JIK           912
//#define  ID_CONJ_JOB_TASK_ITEM_JICBEK        913
#define  NUM_ATTRIB_COLS_ITEM_CBE_TASK     7

#define  ITEM_CBE_TASK_JOB_CSTSHT_KEY   0
#define  ITEM_CBE_TASK_KEY              1
#define  ITEM_CBE_TASK_QTY              2
#define  ITEM_CBE_TASK_UNIT_CODE_ID     3
#define  ITEM_CBE_TASK_DESC             4
#define  ITEM_CBE_TASK_ID               5
#define  ITEM_CBE_TASK_QTY_RAW          6




#define  ID_QRY_ITEM_TASK_PROD                   914
#define  SIZE_OF_QRY_ITEM_TASK_PROD      512

#define  ID_CONJ_ITEM_TASK_PROD_CSK             916
#define  ID_CONJ_ITEM_TASK_PROD_TK              917

#define  NUM_ATTRIB_COLS_ITEM_TASK_PROD    6
#define  ITEM_TASK_PROD_TASKID           0
#define  ITEM_TASK_PROD_CSTSHT_ID        1
#define  ITEM_TASK_PROD_DESC             2
#define  ITEM_TASK_PROD_RATE             3
#define  ITEM_TASK_PROD_HRS_PER_DAY      4
#define  ITEM_TASK_PROD_GRAN             5





#define  ID_QRY_OVRHD_MEL                   918
#define  SIZE_OF_QRY_OVRHD_MEL      256

#define  NUM_ATTRIB_COLS_OVRHD_MEL    8
#define  OVRHD_MEL_MAT_PCT          0
#define  OVRHD_MEL_EQP_PCT          1
#define  OVRHD_MEL_LBR_PCT          2
#define  OVRHD_MEL_MARKUP_PCT       3
#define  OVRHD_MEL_MAT_RAW          4
#define  OVRHD_MEL_EQP_RAW          5
#define  OVRHD_MEL_LBR_RAW          6
#define  OVRHD_MEL_MARKUP_RAW       7





#define  ID_QRY_CST_MEL                   919
#define  SIZE_OF_QRY_CST_MEL      512

#define  NUM_ATTRIB_COLS_CST_MEL   12
#define  CST_MEL_MAT_UNIT_COST      0
#define  CST_MEL_MAT_OVRHD_AMT      1
#define  CST_MEL_MAT_OVRHD_PCT      2
#define  CST_MEL_MAT_EXT            3
#define  CST_MEL_EQU_UNIT_COST      4
#define  CST_MEL_EQU_OVRHD_AMT      5
#define  CST_MEL_EQU_OVRHD_PCT      6
#define  CST_MEL_EQU_EXT            7
#define  CST_MEL_LBR_UNIT_COST      8
#define  CST_MEL_LBR_OVRHD_AMT      9
#define  CST_MEL_LBR_OVRHD_PCT     10
#define  CST_MEL_LBR_EXT           11


#define  ID_QRY_CST_MAT_TTL                   920
#define  ID_SLC_CST_MAT_TTL                   921
#define  ID_QRY_CST_MAT_TTL_FW                922
#define  SIZE_OF_QRY_CST_MAT_TTL         128
#define  SIZE_OF_SLC_CST_MAT_TTL         128
#define  SIZE_OF_QRY_CST_MAT_TTL_FW      340
#define  CST_MAT_UNIT_COST      0
#define  CST_MAT_OVRHD_AMT      1
#define  CST_MAT_EXT            2
#define  CST_MAT_EXT_RAW        3




#define  ID_QRY_CST_EQP_TTL                   923
#define  ID_SLC_CST_EQP_TTL                   924
#define  ID_QRY_CST_EQP_TTL_FW                925
#define  SIZE_OF_QRY_CST_EQP_TTL         256
#define  SIZE_OF_SLC_CST_EQP_TTL         256
#define  SIZE_OF_QRY_CST_EQP_TTL_FW      560
#define  CST_EQP_UNIT_COST      0
#define  CST_EQP_OVRHD_AMT      1
#define  CST_EQP_EXT            2
#define  CST_EQP_EXT_RAW        3


#define  ID_QRY_CST_LBR_TTL                   926
#define  ID_SLC_CST_LBR_TTL                   927
#define  ID_QRY_CST_LBR_TTL_FW                928
#define  SIZE_OF_QRY_CST_LBR_TTL         128
#define  SIZE_OF_SLC_CST_LBR_TTL         128
#define  SIZE_OF_QRY_CST_LBR_TTL_FW      570
#define  CST_LBR_UNIT_COST      0
#define  CST_LBR_OVRHD_AMT      1
#define  CST_LBR_EXT            2
#define  CST_LBR_EXT_RAW        3


#define  ID_QRY_CST_TTL                   930





#define  ID_QRY_CST_MARKUP                932
#define  SIZE_OF_QRY_CST_MARKUP      256
#define  CST_MARKUP_PCT   0
#define  CST_MARKUP_TTL   1


#define  ID_QRY_CST_UNITPRICE             935
#define  SIZE_OF_QRY_CST_UNITPRICE   640
#define  CST_UNITPRICE_TTL   0



#define  ID_QUERY_MAT                     940
#define  ID_CONJ_MAT                      941

#define  SIZE_OF_QUERY_MAT         640
#define  NUM_ATTRIB_COLS_MAT         6

#define  MAT_ID           0
#define  QUANTITY         1
#define  UNIT_PRICE       2
#define  MAT_UNIT         3
#define  EXT              4
#define  MAT_DESC         5



#define  ID_QUERY_MAT_SUM                 945

#define  SIZE_OF_QUERY_MAT_SUM      640
#define  NUM_ATTRIB_COLS_MAT_SUM     1

#define  MAT_SUM_FMT      0


#define  ID_QUERY_CREW                     946
#define  ID_QUERY_CREWKEY                 990

#define  SIZE_OF_QUERY_CREW     128
#define  NUM_ATTRIB_COLS_CREW     3

#define  CREW_ID         0
#define  CREW_KEY        1
#define  CREW_DESC       2




#define  ID_QUERY_CREW_EQP                 947
#define  ID_CONJ_CREW_EQP_CREWKEY          949

#define  SIZE_OF_QUERY_CREW_EQP       800
#define  SIZE_OF_CONJ_CREW_EQP         80
#define  NUM_ATTRIB_COLS_CREW_EQP       6

#define  EQP_ID           0
#define  EQP_QUAN         1
#define  EQP_RATE         2
#define  EQP_DESC         3
#define  EQP_HRS          4
//#define  EQP_PROD_RATE    5
#define  EQP_DAILY_CST    5 
//#define  EQP_OT_RATE      7
//#define  EQP_OT_HRS       8
//#define  EQP_OT_DAILY_CST 9 



#define  ID_QUERY_CREW_LBR                 950
#define  ID_CONJ_CREW_LBR_JCSK             951
#define  ID_CONJ_CREW_LBR_CREWKEY              952

#define  SIZE_OF_QUERY_CREW_LBR       800
#define  SIZE_OF_CONJ_CREW_LBR         80
#define  NUM_ATTRIB_COLS_CREW_LBR       6

#define  LBR_ID           0
#define  LBR_QUAN         1
#define  LBR_RATE         2
#define  LBR_DESC         3
#define  LBR_HRS          4
//#define  LBR_PROD_RATE    5
#define  LBR_DAILY_CST    5 
//#define  LBR_OT_RATE      7
//#define  LBR_OT_HRS       8
//#define  LBR_OT_DAILY_CST 9 




#define  ID_QUERY_CREW_EQP_TTL            953
#define  ID_CONJ_CREW_EQP_TTL             954
#define  SIZE_OF_QUERY_CREW_EQP_TTL   640

#define  EQP_TTL          0
#define  EQP_TTL_RAW      1



#define  ID_QUERY_CREW_LBR_TTL            955
#define  ID_CONJ_CREW_LBR_TTL             956
#define  SIZE_OF_QUERY_CREW_LBR_TTL   640

#define  LBR_TTL          0
#define  LBR_TTL_RAW      1




#define  ID_QUERY_CREW_EQP_PROD            957
#define  ID_CONJ_CREW_EQP_PROD             958
#define  SIZE_OF_QUERY_CREW_EQP_PROD   850

#define  CREW_EQP_PROD_RATE          0
#define  CREW_EQP_PROD_TOTAL         1
#define  CREW_EQP_PROD_COST          2



#define  ID_QUERY_CREW_LBR_PROD            860
#define  ID_CONJ_CREW_LBR_PROD             861
#define  SIZE_OF_QUERY_CREW_LBR_PROD   850

#define  CREW_LBR_PROD_RATE          0
#define  CREW_LBR_PROD_TOTAL         1
#define  CREW_LBR_PROD_COST          2





extern USHORT  EXPORT ItiDllQueryVersion (VOID);                             
extern BOOL    EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion);     
extern VOID    EXPORT ItiDllQueryMenuName (PSZ pszBuffer, USHORT  usMaxSize, 
                                           PUSHORT pusWindowID);             
extern BOOL    EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID,      
                                               USHORT usActiveWindowID);     
extern MRESULT EXPORT ItiDllrSICbeRptDlgProc (HWND   hwnd, USHORT usMessage,     
                                          MPARAM mp1,  MPARAM mp2);          
extern USHORT  EXPORT ItiRptDLLPrintReport                                   
                           (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[]); 
