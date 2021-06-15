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
 *   dprof.h                                                   
 */                                                         
                                                             
#define  DLL_VERSION   1

#define  TITLE        "BAMS/DSS Parametric Profile Analysis Model"  
#define  SUB_TITLE    ""  
#define  FOOTER       ""  

#define  RPT_DLL_NAME "dprop "



extern USHORT  EXPORT ItiDllQueryVersion (VOID);                            

extern USHORT  EXPORT ItiRptDLLPrintReport                                  
                           (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[]);

/* query IDs */
#define LIST_WORK_TYPES          500
#define GET_PROPOSAL_TOTAL       501
#define GET_MAJORITEM_PERCENTS   502
#define GET_MAJORITEM_TOTAL      503


