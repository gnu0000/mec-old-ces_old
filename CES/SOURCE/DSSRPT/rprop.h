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
 *   rProp.h                                                   
 *   Timothy Blake                                          
 */                                                         
                                                             
#define  DLL_VERSION   1
#define  TITLE        "BAMS/DSS Proposal Listing"
#define  SUB_TITLE    ""
#define  FOOTER       ""

#define  RPT_DLL_NAME "rProp "


#define  OPT_SW_SHORT_DESC_CHR    'S'
#define  OPT_SW_SHORT_DESC_STR   " /S s "


#define  ID_QUERY_LISTING               5250

#define  SIZE_OF_QUERY_LISTING          1023
#define  SIZE_OF_CONJ                     80

#define  NUM_ATTRIB_COLS     30

#define  PROPID               0    //ProposalID
#define  PROJ_NUM             1    //ProjectNumber
#define  DESC                 2    //HistoricalProposal.Description
#define  PCN                  3    //ProjectControlNumber
#define  LOC                  4    //HistoricalProposal.Location
#define  LOC_DESC             5    //LocationDescription
#define  DIST                 6    //a.CodeValueID
#define  COUNTY_ID            7    //CountyID
#define  COUNTY_NAME          8    //CountyName
#define  STATUS_ID            9    //b.CodeValueID
#define  STATUS_TRANS        10    //b.Translation
#define  PERCENT             11    //PercentComplete
#define  IMP_DATE            12    //ImportDate
#define  COMP_DATE           13    //DateComplete
#define  WRK_TYP             14    //c.CodeValueID
#define  WRK_TRANS           15    //c.Translation
#define  ROAD_TYP            16    //d.CodeValueID
#define  ROAD_TRANS          17    //d.Translation
#define  SPEC_YR             18    //HistoricalProposal.SpecYear
#define  LENGTH              19    //ProposalLength
#define  WIDTH               20    //AveragePavementWidth
#define  DEPTH               21    //AveragePavementDepth
#define  LETT_DATE           22    //LettingDate
#define  BID_CNT             23    //NumberOfBids
#define  REJ_FLAG            24    //RejectedFlag
#define  REJ_ID              25    //e.CodeValueID
#define  REJ_TRANS           26    //e.Translation
#define  AWD_TTL             27    //AwardedBidTotal
#define  LOW_TTL             28    //LowBidTotal
#define  EST_TTL             29    //EngineersEstimate 


#define  ID_QUERY_LISTING_PROJS         5251
#define  PROJ_CTRL_NUM   0




#define  ID_QUERY_BIDKEYS                5252

#define  SIZE_OF_QUERY_BIDKEYS            255

#define  NUM_ATTRIB_COLS_BIDKEYS   2

#define  AWARDED_KEY           0
#define  LOWBID_KEY            1



#define  ID_QUERY_PITM_KEYS                5253

#define  SIZE_OF_QUERY_PITM_KEYS            255

#define  NUM_ATTRIB_COLS_PITM_KEYS   2

#define  CAT_PKEY               0
#define  CAT_CN                 1



#define  ID_QUERY_ITEMS                5254
#define  ID_QUERY_ITEMS_CONJ           5255
#define  ID_QUERY_ITEMS_ORDER          5256

#define  SIZE_OF_QUERY_ITEMS           1200

#define  NUM_ATTRIB_COLS_ITEMS   13


#define  ITM_NUM               0  // StandardItemNumber
#define  ITM_QTY               1  // Quantity
#define  ITM_LONG_DESC         2  // LongDescription
#define  ITM_SHORT_DESC        3  // ShortDescription
#define  ITM_UNIT              4  // CodeValueID
#define  ITM_AWD_PRC           5  // AWD.UnitPrice
#define  ITM_LOW_PRC           6  // LOW.UnitPrice
#define  ITM_EST_PRC           7  // EST.UnitPrice
#define  ITM_AWD_EXT           8  // ExtensionAWD = Quantity * AWD.UnitPrice
#define  ITM_LOW_EXT           9  // ExtensionLOW = Quantity * LOW.UnitPrice
#define  ITM_EST_EXT          10  // ExtensionEST = Quantity * EST.UnitPrice
#define  ITM_AWD_LCA          11  // AWD.LowCostAlternate
#define  ITM_LOW_LCA          12  // LOW.LowCostAlternate







#define  ID_QUERY_RANK                 5257
#define  ID_QUERY_RANK_CONJ            5258
#define  ID_QUERY_RANK_ORDER           5259

#define  SIZE_OF_QUERY_RANK             512

#define  NUM_ATTRIB_COLS_RANK   9
                                       
#define  RANK_V_ID              0  //VendorID
#define  RANK_NAME              1  //ShortName
#define  RANK_BID_TTL           2  //BidTotal
#define  RANK_ENG_PCT           3  //PctEngrEst = BidTotal/EngineersEstimate
#define  RANK_LOW_PCT           4  //PctLowBid = BidTotal/LowBidTotal 
#define  RANK_BTEE              5  //BidTotal-EngineersEstimate
#define  RANK_ENG_EST           6  //EngineersEstimate 
#define  RANK_AWDD              7  //HP.AwardedBidderKey-HB.VendorKey
                                     // if 0 then this is the awarded vendor
#define  RANK_ENGLOW            8  //EngineersEstimate/LowBidTotal ie.%LowBid




#define  ID_QUERY_MJR                 5260
#define  ID_QUERY_MJR_ORDER           5261

#define  SIZE_OF_QUERY_MJR             640

#define  NUM_ATTRIB_COLS_MJR   8
                                       
#define  MJR_ID                0        //MajorItemID
#define  MJR_QTY               1        //Quantity
#define  MJR_PRC               2        //WeightedAverageUnitPrice
#define  MJR_UNIT              3        //CodeValueID
#define  MJR_EXT               4        //ExtendedAmount
#define  MJR_AWD_PCT           5        //ExtendedAmount/AwardedBidTotal 
#define  MJR_REJ_PCT           6        //ExtendedAmount/LowBidTotal 
#define  MJR_DESC              7        //Description



#define  ID_QUERY_MJR_TTL              5262
#define  SIZE_OF_QUERY_MJR_TTL          640

#define  NUM_ATTRIB_COLS_MJR_TTL   1
                                       
#define  MJR_TTL                   0      //total




#define  ID_QUERY_MJR_TTL_PCT              5263
#define  SIZE_OF_QUERY_MJR_TTL_PCT          640

#define  NUM_ATTRIB_COLS_MJR_TTL_PCT   2
                                       
#define  MJR_TTL_PCT_REJ               0        //if proposal was rejected.
#define  MJR_TTL_PCT_AWD               1        //if it was accepted.






//#define  ID_QUERY_CAT                359
//#define  ID_QUERY_CAT_CONJ           359
//
//#define  SIZE_OF_QUERY_CAT           255
//
//#define  NUM_ATTRIB_COLS_CAT   9
//
//#define  CAT_NUM               0
//#define  CAT_DESC              1
//#define  CAT_LEN               2
//#define  CAT_WTH               3
//#define  CAT_SID               4
//#define  CAT_SLEN              5
//#define  CAT_SWTH              6
//#define  CAT_STYP              7
//#define  CAT_STYPE_TRANS       8





#define  ID_QUERY_AWD_SLCT          5265
#define  ID_QUERY_AWD               5266
#define  ID_QUERY_AWD_CONJ          5267

#define  SIZE_OF_QUERY_AWD           700

#define  NUM_ATTRIB_COLS_AWD   3

#define  AWD_UNIT_PRICE        0
#define  AWD_LOWCOST_ALT       1
#define  AWD_EXT               2





#define  ID_QUERY_LOW_SLCT          5268
#define  ID_QUERY_LOW               5269
#define  ID_QUERY_LOW_CONJ          5270

#define  SIZE_OF_QUERY_LOW           700

#define  NUM_ATTRIB_COLS_LOW   3

#define  LOW_UNIT_PRICE        0
#define  LOW_LOWCOST_ALT       1
#define  LOW_EXT               2





#define  ID_QUERY_EST_SLCT          5271
#define  ID_QUERY_EST               5272
#define  ID_QUERY_EST_CONJ          5273

#define  SIZE_OF_QUERY_EST           700

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

extern MRESULT EXPORT ItiDllrPropRptDlgProc (HWND   hwnd, USHORT usMessage, 
                                             MPARAM mp1,  MPARAM mp2);      

extern USHORT  EXPORT ItiRptDLLPrintReport                                  
                           (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[]);
