/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1992 by Info Tech, Inc.  All Rights Reserved.         |
|                                                                           |
|       This program module is part of DS/Shell (PC), Info Tech's           |
|       database interfaces for OS/2, a proprietary product of              |
|       Info Tech, Inc., no part of which may be reproduced or              |
|       transmitted in any form or by any means, electronic,                |
|       mechanical, or otherwise, including photocopying and recording      |
|       or in connection with any information storage or retrieval          |
|       system, without permission in writing from Info Tech, Inc.          |
|                                                                           |
+--------------------------------------------------------------------------*/


#include "..\include\dialog.h"
#include "..\include\itirptdg.h"

#define IDD_EDITCODETABLE   15001

#define DID_NAME            200
#define DID_DESCRIPTION     201
#define DID_MAXCODELEN      202

#define DID_CODEVAL         203
#define DID_XLATE           204

#define DID_METRIC_UNIT      215
#define DID_ENGLISH_UNIT     216
#define DID_NEITHER_UNIT     217


#define RCODETAB_SESSION    "Code Table Listing"
#define IDD_RCODETAB         rCodeTab_RptDlgBox 
#define DID_ALL               250
#define DID_SELECTED          251
#define DID_KEY               252
