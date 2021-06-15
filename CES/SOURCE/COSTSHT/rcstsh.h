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
 *   rCstSh.h                                                   
 *   Timothy Blake                                          
 *                                                          
 */

#ifdef RCSTSHT

#define  DLL_VERSION   1
#define  TITLE        "BAMS/CES Cost Sheet Catalog Listing"
#define  SUB_TITLE    ""
#define  FOOTER       ""

#define  RPT_DLL_NAME "rCstSh"

#define  OPT_SW_NEWPG_STR   " /N n "
#define  OPT_SW_NEWPG_CHR   'N'
#define  OPT_SW_NEWPG_VAL   'n'

#define  OPT_SW_SUMMONLY_STR   " /S s "
#define  OPT_SW_SUMMONLY_CHR   'S'
#define  OPT_SW_SUMMONLY_VAL   's'


// Subsections:   (Don't monkey with these RELATIVE indices.)
#define  BRKDWN        0
#define  BRKDWN_OFF    5

#define  MATERIAL      6
#define  MATERIAL_OFF  6

#define  CREW         13
#define  CREW_OFF      9

#define SUBHEADER_INX   RPT_TTL_12



#define  ID_QUERY_CSTSHT_KEYS            961
#define  CST_SHT_KEY    0



#define  ID_QUERY_LISTING                862

#define  SIZE_OF_QUERY_LISTING      380
#define  NUM_ATTRIB_COLS_LISTING     11

#define  CSTSHT_ID          0
#define  DESC               1
#define  PROD_RATE          2
#define  DEF_HRS_PERDAY     3
#define  PROD_GRAN          4
#define  MAT_OVRHD_PCT      5
#define  LBR_OVRHD_PCT      6
#define  EQP_OVRHD_PCT      7
#define  MARKUP_PCT         8
#define  MARKUP_PCT_RAW     9
#define  CS_UNIT_SYS       10                     


#define  ID_QUERY_MAT                     963

#define  SIZE_OF_QUERY_MAT         640
#define  NUM_ATTRIB_COLS_MAT         6

#define  MAT_ID           0
#define  QUANTITY         1
#define  UNIT_PRICE       2
#define  EXT              3
#define  MAT_DESC         4
#define  MAT_UNIT         5




#define  ID_QUERY_MAT_SUM                 964

#define  SIZE_OF_QUERY_MAT_SUM     1023
#define  NUM_ATTRIB_COLS_MAT_SUM     1

#define  MAT_SUM          0
#define  MAT_SUM_FMT      1



#define  ID_QUERY_MAT_TTL                 966

#define  SIZE_OF_QUERY_MAT_TTL     1023
#define  NUM_ATTRIB_COLS_MAT_TTL     5

#define  MAT_TOTAL        0
#define  MAT_OVRHD        1
#define  MAT_OH_PCT       2
#define  MAT_EXT          3
#define  MAT_EXT_RAW      4




#define  ID_QUERY_CREWS                 967

#define  SIZE_OF_QUERY_CREWS     128
#define  NUM_ATTRIB_COLS_CREWS     3

#define  CREWS_ID         0
#define  CREWS_KEY        1
#define  CREWS_DESC       2





#define  ID_QUERY_CREWS_EQP                 968
#define  ID_QUERY_CREWS_EQP_CONJ            969

#define  SIZE_OF_QUERY_CREWS_EQP       800
#define  SIZE_OF_QUERY_CREWS_EQP_CONJ   80
#define  NUM_ATTRIB_COLS_CREWS_EQP       7

#define  EQP_ID           0
#define  EQP_QUAN         1
#define  EQP_RATE         2
#define  EQP_DESC         3
#define  EQP_HRS          4
#define  EQP_PROD_RATE    5
#define  EQP_DAILY_CST    6 



#define  ID_QUERY_CREWS_EQP_TTL            970
#define  SIZE_OF_QUERY_CREWS_EQP_TTL   764

#define  EQP_TTL          0
#define  EQP_TTL_RAW      1






#define  ID_QUERY_CREWS_LBR                 971

#define  SIZE_OF_QUERY_CREWS_LBR       800
#define  SIZE_OF_QUERY_CREWS_LBR_CONJ   80
#define  NUM_ATTRIB_COLS_CREWS_LBR       7

#define  LBR_ID           0
#define  LBR_QUAN         1
#define  LBR_RATE         2
#define  LBR_DESC         3
#define  LBR_HRS          4
#define  LBR_PROD_RATE    5
#define  LBR_DAILY_CST    6 



#define  ID_QUERY_CREWS_LBR_TTL            972
#define  SIZE_OF_QUERY_CREWS_LBR_TTL   764

#define  LBR_TTL          0
#define  LBR_TTL_RAW      1



#define  ID_QUERY_CSTSHT_PROD              973
#define  SIZE_OF_QUERY_CSTSHT_PROD   164

#define  CSTSHT_PROD_RATE     0
#define  CSTSHT_PROD_RATE_RAW 1




#define  ID_QUERY_EQP_TTL                  974 

#define  SIZE_OF_QUERY_EQP_TTL     1023
#define  NUM_ATTRIB_COLS_EQP_TTL     5

#define  EQP_TOTAL        0
#define  EQP_OVRHD        1
#define  EQP_OH_PCT       2
#define  EQP_EXT          3
#define  EQP_EXT_RAW      4





#define  ID_QUERY_LBR_TTL                  975

#define  SIZE_OF_QUERY_LBR_TTL     1023
#define  NUM_ATTRIB_COLS_LBR_TTL     5

#define  LBR_TOTAL        0
#define  LBR_OVRHD        1
#define  LBR_OH_PCT       2
#define  LBR_EXT          3
#define  LBR_EXT_RAW      4






extern USHORT  EXPORT ItiDllQueryVersion (VOID);                             
extern BOOL    EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion);     
extern VOID    EXPORT ItiDllQueryMenuName (PSZ pszBuffer, USHORT  usMaxSize, 
                                           PUSHORT pusWindowID);             
extern BOOL    EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID,      
                                               USHORT usActiveWindowID);     
extern MRESULT EXPORT ItiDllrCstShRptDlgProc (HWND   hwnd, USHORT usMessage,     
                                              MPARAM mp1,  MPARAM mp2);
extern USHORT  EXPORT ItiRptDLLPrintReport                                   
                           (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[]);
#endif
extern USHORT  EXPORT CostSheetBrkDwnSections (PSZ pszCstShtKey, BOOL bSetID);
extern USHORT  EXPORT CostSheetUnitPrice (PSZ pszCstSheetKey,
                                          PSZ pszCSUnitPrice, USHORT usLen);
