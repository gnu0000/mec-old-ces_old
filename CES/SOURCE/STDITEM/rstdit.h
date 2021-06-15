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
 *   rStdIt.h
 *   Timothy Blake
 */

#define  DLL_VERSION   1
#define  TITLE        "BAMS Standard Item Listing"
#define  SUB_TITLE    "for Spec Year "
#define  FOOTER       ""

#define  RPT_DLL_NAME "rStdIt"

#define  PEMETH_AVG_RPT_DLL_NAME      "rSIAvg"
         /* MUST be the SAME as the RPT_DLL_NAME in the rSIAvg.h file! */

#define  PEMETH_REG_RPT_DLL_NAME      "rSIReg"
         /* MUST be the SAME as the RPT_DLL_NAME in the rSIReg.h file! */

#define  CST_BASED_EST_RPT_DLL_NAME   "rSICbe"
         /* MUST be the SAME as the RPT_DLL_NAME in the rSICbe.h file! */

#define  DEF_UNIT_PRICE_RPT_DLL_NAME  "rDefUP"
         /* MUST be the SAME as the RPT_DLL_NAME in the rDefUP.h file! */




#define  ORDER_BY_ITEMNUM        'I'
#define  OPT_SW_SPEC_YR          'S'
#define  OPT_SW_INCL_DFLT_PRICES 'D'
#define  OPT_VAL_DFLT_PRICES     'x'
#define  OPT_SW_SUMMARY_ONLY     'U'

#define  OPT_SW_BASEDATE         'B'



#define  DEF_RPT     'f'
#define  DEF_RPT_LOC  3

#define  AVG_RPT     'A'
#define  AVG_RPT_LOC  3

#define  REG_RPT     'R'
#define  REG_RPT_LOC  3

#define  CST_RPT     'C'
#define  CST_RPT_LOC  3

#define  STD_RPT     'd'
#define  STD_RPT_LOC  3
          /* These letters are positional, in the dll name. */
          /* example: rSIAvg */


/* ---------------------------------------------- *\
 * -- SpecYear query info.                        *
\* ---------------------------------------------- */
#define  ID_QUERY_SPEC_YR                330

#define  SIZE_OF_SPEC_YR      128
#define  NUM_SPEC_YR_COLS       1

/* indices to use with ppsz[ ] */
#define  SPEC_YEAR          0




/* ---------------------------------------------- *\
 * -- Standard Item Key list query info.          *
\* ---------------------------------------------- */
#define  ID_QUERY_DEF_ITEM_KEYS          339

#define  ID_QUERY_ITEM_KEYS              340
#define  ID_CONJ_G_STDITEMKEY             40
#define  ID_CONJ_L_STDITEMKEY             41


#define  SIZE_OF_ITEM_KEYS    256
#define  NUM_ITEM_KEYS_COLS     2

/* indices to use with ppsz[ ] */
#define  KEY_STD_ITEM       0





/* ---------------------------------------------- *\
 * -- Query info for std item summary.            *
\* ---------------------------------------------- */
#define  ID_QUERY_LISTING                350
#define  ID_CONJ_SPECYEAR                 50 
#define  ID_CONJ_STDITEMKEY               51

#define  SIZE_OF_QUERY      640
#define  SIZE_OF_CONJ        80
#define  NUM_ATTRIB_COLS      9

/* indices to use with ppsz[ ] */
#define  SPEC_YEAR          0
#define  STD_ITEM_NUM       1
#define  SHORT_DESC         2
#define  CODE_VALUE_ID      3
#define  RD_PRECISION       4
#define  DEF_PROD_RATE      5
#define  DEF_START_TIME     6
#define  LONG_DESC          7
#define  STD_ITEM_KEY       8





/* ---------------------------------------------- *\
 * -- Predecessor query info.                     *
\* ---------------------------------------------- */
#define  ID_QUERY_PRED                   360
#define  ID_ORDER_PRED                    60 

#define  SIZE_OF_PRED       640
#define  SIZE_OF_ORDER       80
#define  NUM_PRED_COLS        6

/* indices to use with ppsz[ ] */
//define SPEC_YEAR          0
//define STD_ITEM_NUM       1
//define SHORT_DESC         2
#define  LINK               3
#define  LAG                4
#define  ITEM_KEY           5




/* ---------------------------------------------- *\
 * -- Successor query info.                       *
\* ---------------------------------------------- */
#define  ID_QUERY_SUCC                   370
#define  ID_ORDER_SUCC                    70 

#define  SIZE_OF_SUCC         640
//#define  SIZE_OF_ORDER       64
#define  NUM_SUCC_COLS          6

/* indices to use with ppsz[ ] */
//define SPEC_YEAR          0
//define  STD_ITEM_NUM      1
//define  SHORT_DESC        2
//define  LINK              3
//define  LAG               4
//define  ITEM_KEY          5




/* ---------------------------------------------- *\
 * -- PercentMajorItem query info.                *
\* ---------------------------------------------- */
#define  ID_QUERY_PCT_MAJ                   371

#define  SIZE_OF_PCT_MAJ         640
#define  NUM_PCT_MAJ_COLS          3

/* indices to use with ppsz[ ] */
#define   PCT_OF_COST       0
#define   MAJ_ITEM_ID       1
#define   DESC              2





/* ---------------------------------------------- *\
 * -- Sum of percents query info.                 *
\* ---------------------------------------------- */
#define  ID_QUERY_SUM_PCT        372

#define  SIZE_OF_SUM_PCT         640
#define  NUM_SUM_PCT_COLS          1

/* indices to use with ppsz[ ] */
#define   SUM_PCT           0





/* ---------------------------------------------- *\
 * -- Quantity Conversion Factor query info.      *
\* ---------------------------------------------- */
#define  ID_QUERY_MAJ_QCF        373

#define  SIZE_OF_MAJ_QCF         640
#define  NUM_MAJ_QCF_COLS          4

/* indices to use with ppsz[ ] */
#define  CVID               0
#define  QCF                1
#define  MAJID              2
#define  IDDESC             3




/* ---------------------------------------------- *\
 * -- Default Unit Prices query info.             *
\* ---------------------------------------------- */
#define  ID_QUERY_DEFLT        374

#define  SIZE_OF_DEFLT         640
#define  NUM_DEFLT_COLS          4

/* indices to use with ppsz[ ] */
#define  UNIT_PRICE         0


#define  ID_QUERY_DEF_LISTING  375

/* indices to use with ppsz[ ] */
//#define  SPEC_YEAR          0
//#define  STD_ITEM_NUM       1
//#define  SHORT_DESC         2
//#define  CODE_VALUE_ID      3
//#define  RD_PRECISION       4
//#define  DEF_PROD_RATE      5
//#define  DEF_START_TIME     6
//#define  LONG_DESC          7
//#define  STD_ITEM_KEY       8



#define  ID_QRY_DEFLT_RPT       380


extern USHORT  EXPORT ItiDllQueryVersion (VOID);                             
extern BOOL    EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion);     
extern VOID    EXPORT ItiDllQueryMenuName (PSZ pszBuffer, USHORT  usMaxSize, 
                                           PUSHORT pusWindowID);             
extern BOOL    EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID,      
                                               USHORT usActiveWindowID);     
extern MRESULT EXPORT ItiDllrStdItRptDlgProc (HWND   hwnd, USHORT usMessage,     
                                              MPARAM mp1,  MPARAM mp2);          
extern USHORT  EXPORT ItiRptDLLPrintReport                                   
                           (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[]); 
