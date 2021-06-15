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
 *   dQty.h                                                   
 */                                                         
                                                             
#define  DLL_VERSION   1

#define  TITLE        "BAMS/DSS Major Item Quantity Analysis Model"  
#define  SUB_TITLE    ""  
#define  FOOTER       ""  

#define  RPT_DLL_NAME "dQty "



extern USHORT  EXPORT ItiDllQueryVersion (VOID);                            

extern USHORT  EXPORT ItiRptDLLPrintReport                                  
                           (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[]);

/* query IDs */
#define LIST_WORK_TYPES       100

#define GET_MAJOR_ITEM_INFO   300
#define GET_MAJOR_ITEMS       301
#define DO_HISTOGRAM          302


#define  DO_HISTOGRAM_10       310
#define  DO_HISTOGRAM_20       320
#define  DO_HISTOGRAM_30       330
#define  DO_HISTOGRAM_40       340
#define  DO_HISTOGRAM_50       350
#define  DO_HISTOGRAM_60       360
#define  DO_HISTOGRAM_70       370
#define  DO_HISTOGRAM_80       380
#define  DO_HISTOGRAM_90       390
#define  DO_HISTOGRAM_100      393


