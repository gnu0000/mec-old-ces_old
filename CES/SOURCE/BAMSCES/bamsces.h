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
 * BAMSCES.h
 * Mark Hampton
 */




#define ID_RESOURCE  1

#define BAMSCES_ICON         ID_RESOURCE
#define BAMSCES_ACCELERATORS ID_RESOURCE


#ifndef RC_INVOKED

#include "..\include\itiglob.h"
#include "jobpath.h"

extern PGLOBALS  pglobals;
extern PWORKDAYS pWorkDays;


extern MRESULT EXPENTRY MainWindowProc (HWND   hwnd,
                                        USHORT usMessage,
                                        MPARAM mp1,
                                        MPARAM mp2);

CHAR szTitleBarString[128];

//extern BOOL bEnglishYearGiven, bMetricYearGiven;
//extern CHAR  szMetricYear[];
//extern CHAR  szEnglishYear[];
#define YEARLEN 5

#define APP_TITLE_METRIC   "BAMS/CES                          Metric SpecBook "
#define APP_TITLE_ENGLISH  "BAMS/CES                         English SpecBook "

#endif /* RC_INVOKED */
