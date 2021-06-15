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
 * BAMSDSS.h
 * Mark Hampton
 *
 */





#define ID_RESOURCE  1

#define BAMSDSS_ICON         ID_RESOURCE
#define BAMSDSS_ACCELERATORS ID_RESOURCE

#define YEARLEN 5

#ifndef RC_INVOKED

#include "..\include\itiglob.h"

extern PGLOBALS pglobals;


extern MRESULT EXPENTRY MainWindowProc (HWND   hwnd,
                                        USHORT usMessage,
                                        MPARAM mp1,
                                        MPARAM mp2);

CHAR szTitleBarString[128];

#define APP_TITLE_METRIC   "Data Browser (BAMS/DSS)  Metric Spec Year: "
#define APP_TITLE_ENGLISH  "Data Browser (BAMS/DSS) English Spec Year: "


#endif /* RC_INVOKED */
