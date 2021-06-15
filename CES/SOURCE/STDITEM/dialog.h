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
#include "..\include\itirptdg.H"

#define  StanItemEditSY     2105

#define  DID_ITEMNUMBER          100
#define  DID_UNIT                101
#define  DID_DEFPRODUCTION       102
#define  DID_STARTTIME           103
#define  DID_PRECISION           104
#define  DID_SPECYEAR            105
#define  DID_SHORTDESCRIPTION    106
#define  DID_LONGDESCRIPTION     107
#define  DID_LUMPSUM             108
#define  DID_ADDPRED             109
#define  DID_DELETEPRED          110
#define  DID_ADDSUCC             111
#define  DID_DELETESUCC          112
#define  DID_ADDMAJITEM          113
#define  DID_DELETEMAJITEM       114

#define  DID_ADDEP               115
#define  DID_DEDEP               116

#define RPT_SESSION              "Standard Item List" 
#define IDD_RSTDIT               rStdIt_RptDlgBox
#define DID_KEY                     56                    
#define DID_SELECTED               357               
#define DID_DFLT_PRICES            358
#define DID_INCL_AVG               359
#define DID_INCL_REG               360
#define DID_INCL_CST               361
#define DID_RPT_LIST               362

                              
#define DID_DATEMSG                383
#define DID_DATEEDIT               384
#define DID_ACCEPT_EDIT            385
#define DID_BASEEDIT               386

#define RPT_AVG_SESSION         "StdItem(PEMETH Avgs)" 
#define IDD_RSIAVG               rSIAvg_RptDlgBox

#define RPT_MJR_SESSION         "Major Item Listing" 
#define IDD_RMJRIT               rMjrIt_RptDlgBox


#define DID_LAG                    102
#define DID_LINK                   103
#define DID_PRED                   104
#define DID_SUCC                   105
