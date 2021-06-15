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


/* 
 *   dPrice.h                                                   
 */                                                         
                                                             
#define  DLL_VERSION   1

#define  TITLE        "BAMS/DSS Major Item Price Analysis Model"  
#define  SUB_TITLE    ""  
#define  FOOTER       ""  

#define  RPT_DLL_NAME "dPrice "



extern USHORT  EXPORT ItiDllQueryVersion (VOID);                            

extern USHORT  EXPORT ItiRptDLLPrintReport                                  
                           (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[]);

/* query IDs */
#define  LIST_WORK_TYPES       200

#define  GET_MAJOR_ITEM_INFO   210

#define  GET_MAJOR_ITEMS       220


#define  GET_WORKTYPE_INFO     240
#define  GET_ROADTYPE_INFO     250
#define  GET_DISTRICT_INFO     260

/* The ordering of the GET_xxxxxxxx_INFO select clause (ppsz[]) */
#define  WORK    0
#define  ROAD    0
#define  DIST    0


#define  GET_QTYS              270

#define  GET_BASE_PRICE        280
#define  GET_ADJ_PCT           290

#define  GET_INTERCEPT        281
#define  GET_SLOPE            282
#define  GET_S_I              283

#define  DO_HISTOGRAM          300

#define  DO_HISTOGRAM_10       310
#define  DO_HISTOGRAM_20       320
#define  DO_HISTOGRAM_30       330
#define  DO_HISTOGRAM_40       340
#define  DO_HISTOGRAM_50       350
#define  DO_HISTOGRAM_60       360
#define  DO_HISTOGRAM_70       370
#define  DO_HISTOGRAM_80       380
#define  DO_HISTOGRAM_90       390
#define  DO_HISTOGRAM_100      400
