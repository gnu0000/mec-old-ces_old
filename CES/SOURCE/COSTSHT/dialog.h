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
#include "..\include\dialog.h"


#define DID_COSTSHEETID      200
#define DID_DESCRIPTION      201
#define DID_PRODUCTIONRATE   202
#define DID_DEFHOURSPERDAY   203
#define DID_PRODUCTIONGRAN   204
#define DID_MARKUPPCT        205
#define DID_LABOVERHEADPCT   206
#define DID_EQUIPOVERHEADPCT 207
#define DID_MATOVERHEADPCT   208

#define DID_OLD_COSTSHEETID  209

#define DID_UNIT_SYS         210

#define DID_METRIC_UNIT      215
#define DID_ENGLISH_UNIT     216
#define DID_NEITHER_UNIT     217


#define RPT_CSTSHT_SESSION   "Cost Sheet Catalogs "
#define IDD_CSTSHT           rCstSh_RptDlgBox

#define RPT_JECBE_SESSION    "Job Estimate: Cost Based "
#define IDD_JECBE            rJECbe_RptDlgBox 

#define RPT_RSICBE_SESSION   "StdItm: Cost Based "
#define IDD_RSICBE           rSICbe_RptDlgBox 


#define DID_KEY               62                    
#define DID_SELECTED          63
#define DID_NEWPG             64
#define DID_SUMMONLY          65
