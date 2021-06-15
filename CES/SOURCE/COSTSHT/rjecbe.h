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
 *   rJECbe.h                                                   
 *   Timothy Blake                                          
 *                                                          
 */                                                         
#define  DLL_VERSION   1

#define  TITLE_LEAD   "BAMS/CES Job "
#define  TITLE_TAIL   " Estimate"
#define  TITLE        TITLE_LEAD TITLE_TAIL

#define  SUB_TITLE    "Job Item Price Bases: Cost-Based Estimation"
#define  FOOTER       ""

#define  RPT_DLL_NAME "rJECbe"



// Subsections:   (Don't monkey with these RELATIVE indices.)
#define  BRKDWN        0
#define  BRKDWN_OFF    5

#define  MATERIAL      6
#define  MATERIAL_OFF  6

#define  CREW         13
#define  CREW_OFF      9

#define SUBHEADER_INX   RPT_TTL_28





/* -- Job/BrkDwns */
#define  ID_QUERY_JBDK                     900
#define  SIZE_OF_QUERY_JBDK      256
#define  NUM_ATTRIB_COLS_JBDK      2
#define  JBDK_KEY               0
#define  JBDK_ID                1


/* -- Job/Items */
#define  ID_QUERY_JIK                      901
#define  ID_CONJ_JIK                       902
#define  SIZE_OF_QUERY_JIK       400
#define  NUM_ATTRIB_COLS_JIK       2
#define  JIK_KEY                0
#define  JIK_ID                 1



/* -- Job/Items/CostBasedEst */
#define  ID_QUERY_JICBEK                   903
#define  ID_CONJ_JICBEK                    904
#define  SIZE_OF_QUERY_JICBEK       256
#define  NUM_ATTRIB_COLS_JICBEK       2
#define  JICBEK_KEY                0
#define  JICBEK_ID                 1



/* -- JobID for page headers */
#define  ID_QUERY_JOB_INFO                971
#define  SIZE_OF_QUERY_JOB_INFO      128
#define  NUM_ATTRIB_COLS_JOB_INFO      2
#define  JOB_ID                   0
#define  JOB_DESC                 1




/* -- JobBrkDwnItem */
#define  ID_QRY_JOB_BRKDWN_ITEM                905
#define  SIZE_OF_QRY_JOB_BRKDWN_ITEM   800
#define  ID_CONJ_JOB_BRKDWN_ITEM_JBDK          906
#define  ID_CONJ_JOB_BRKDWN_ITEM_JIK           907
#define  ID_CONJ_JOB_BRKDWN_ITEM_JICBEK        908
#define  NUM_ATTRIB_COLS_JOB_BRKDWN_ITEM    6
#define  JOB_BDK_ID               0
#define  JOB_BDK_SIN              1
#define  JOB_BDK_DESC             2
#define  JOB_BDK_CBE_ID           3
#define  JOB_BDK_CBE_DESC         4
#define  JOB_BDK_PREC             5



#define  ID_QRY_JOB_TASK_ITEM                909
#define  SIZE_OF_QRY_JOB_TASK_ITEM      256     

#define  ID_QRY_JOB_TASK_ITEM_QUP            947

#define  ID_QRY_JOB_TASK_ITEM_TTL            910
#define  SIZE_OF_QRY_JOB_TASK_ITEM_TTL  512     

#define  JOB_TASK_TTL_EXT          0

#define  JOB_TASK_QUP_QTY          0
#define  JOB_TASK_QUP_UNIT_PRICE   1


#define  ID_QRY_JOB_TASK_ITEM_FW             911
#define  SIZE_OF_QRY_JOB_TASK_ITEM_FW   448     

#define  ID_CONJ_JOB_TASK_ITEM_JIK           912
#define  ID_CONJ_JOB_TASK_ITEM_JICBEK        913
#define  NUM_ATTRIB_COLS_JOB_TASK    7
#define  JOB_TASK_JOB_CSTSHT_KEY   0
#define  JOB_TASK_ID               1
#define  JOB_TASK_QTY              2
#define  JOB_TASK_UNIT_PRICE       3
#define  JOB_TASK_UNIT_CODE_ID     4
#define  JOB_TASK_DESC             5
#define  JOB_TASK_EXT              6




#define  ID_QRY_JOB_TASK_PROD                   914
#define  SIZE_OF_QRY_JOB_TASK_PROD      400

#define  ID_CONJ_JOB_TASK_PROD_JIK              915
#define  ID_CONJ_JOB_TASK_PROD_JICBEK           916
#define  ID_CONJ_JOB_TASK_PROD_JCSK             917
#define  ID_CONJ_JOB_TASK_PROD_TID              918

#define  NUM_ATTRIB_COLS_JOB_TASK_PROD    7
#define  JOB_TASK_PROD_TASKID           0
#define  JOB_TASK_PROD_CSTSHT_ID        1
#define  JOB_TASK_PROD_DESC             2
#define  JOB_TASK_PROD_RATE             3
#define  JOB_TASK_PROD_HRS_PER_DAY      4
#define  JOB_TASK_PROD_OT_HRS           5
#define  JOB_TASK_PROD_GRAN             6




#define  ID_QRY_CST_MEL                   919
#define  SIZE_OF_QRY_CST_MEL      700

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


#define  ID_QRY_CST_TTL                   920
#define  SIZE_OF_QRY_CST_TTL         512

#define  ID_QRY_CST_MARKUP                921 
#define  SIZE_OF_QRY_CST_MARKUP      512
#define  CST_MARKUP_PCT   0
#define  CST_MARKUP_TTL   1


#define  ID_QRY_CST_UNITPRICE             922
#define  SIZE_OF_QRY_CST_UNITPRICE   900
#define  CST_UNITPRICE_TTL   0



#define  ID_QUERY_MAT                     923
#define  ID_CONJ_MAT                      924

#define  SIZE_OF_QUERY_MAT         640
#define  NUM_ATTRIB_COLS_MAT         6

#define  MAT_ID           0
#define  QUANTITY         1
#define  UNIT_PRICE       2
#define  MAT_UNIT         3
#define  EXT              4
#define  MAT_DESC         5



#define  ID_QUERY_MAT_SUM                 925

#define  SIZE_OF_QUERY_MAT_SUM      640
#define  NUM_ATTRIB_COLS_MAT_SUM     1

#define  MAT_SUM_FMT      0


#define  ID_QUERY_CREW                     926
#define  ID_QUERY_CREWKEY                 990

#define  SIZE_OF_QUERY_CREW     128
#define  NUM_ATTRIB_COLS_CREW     3

#define  CREW_ID         0
#define  CREW_KEY        1
#define  CREW_DESC       2




#define  ID_QUERY_CREW_EQP                 927
#define  ID_CONJ_CREW_EQP_JCSK             928
#define  ID_CONJ_CREW_EQP_JCK              929

#define  SIZE_OF_QUERY_CREW_EQP       800
#define  SIZE_OF_CONJ_CREW_EQP         80
#define  NUM_ATTRIB_COLS_CREW_EQP      10

#define  EQP_ID           0
#define  EQP_QUAN         1
#define  EQP_RATE         2
#define  EQP_DESC         3
#define  EQP_HRS          4
#define  EQP_PROD_RATE    5
#define  EQP_DAILY_CST    6 
#define  EQP_OT_RATE      7
#define  EQP_OT_HRS       8
#define  EQP_OT_DAILY_CST 9 



#define  ID_QUERY_CREW_LBR                 930
#define  ID_CONJ_CREW_LBR_JCSK             931
#define  ID_CONJ_CREW_LBR_JCK              932

#define  SIZE_OF_QUERY_CREW_LBR       800
#define  SIZE_OF_CONJ_CREW_LBR         80
#define  NUM_ATTRIB_COLS_CREW_LBR      10

#define  LBR_ID           0
#define  LBR_QUAN         1
#define  LBR_RATE         2
#define  LBR_DESC         3
#define  LBR_HRS          4
#define  LBR_PROD_RATE    5
#define  LBR_DAILY_CST    6 
#define  LBR_OT_RATE      7
#define  LBR_OT_HRS       8
#define  LBR_OT_DAILY_CST 9 




#define  ID_QUERY_CREW_EQP_TTL            933
#define  ID_CONJ_CREW_EQP_TTL             934
#define  SIZE_OF_QUERY_CREW_EQP_TTL   640

#define  EQP_TTL          0
#define  EQP_TTL_RAW      1



#define  ID_QUERY_CREW_LBR_TTL            935
#define  ID_CONJ_CREW_LBR_TTL             936
#define  SIZE_OF_QUERY_CREW_LBR_TTL   640

#define  LBR_TTL          0
#define  LBR_TTL_RAW      1




#define  ID_QUERY_CREW_EQP_PROD            937
#define  SIZE_OF_QUERY_CREW_EQP_PROD   800

#define  CREW_EQP_PROD_RATE          0
#define  CREW_EQP_PROD_UNIT          1
#define  CREW_EQP_PROD_TOTAL         2
#define  CREW_EQP_PROD_COST          3



#define  ID_QUERY_CREW_LBR_PROD            938
#define  SIZE_OF_QUERY_CREW_LBR_PROD   800

#define  CREW_LBR_PROD_RATE          0
#define  CREW_LBR_PROD_UNIT          1
#define  CREW_LBR_PROD_TOTAL         2
#define  CREW_LBR_PROD_COST          3


#define  ID_QUERY_UNIT_DAY     940

extern USHORT  EXPORT ItiDllQueryVersion (VOID);                             
extern BOOL    EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion);     
extern VOID    EXPORT ItiDllQueryMenuName (PSZ pszBuffer, USHORT  usMaxSize, 
                                           PUSHORT pusWindowID);             
extern BOOL    EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID,      
                                               USHORT usActiveWindowID);     
extern MRESULT EXPORT ItiDllrJECbeRptDlgProc (HWND   hwnd, USHORT usMessage,     
                                              MPARAM mp1,  MPARAM mp2);          
extern USHORT  EXPORT ItiRptDLLPrintReport                                   
                           (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[]); 
