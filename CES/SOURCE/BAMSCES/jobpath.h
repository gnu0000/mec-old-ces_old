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


#ifndef JP_INCL

#define JP_INCL

#define CalculateJobSchedule  12456
#define CalculateProgramSchedule  12457


typedef struct
   {
   USHORT  usYear;
   BOOL    bLeapYear;
   CHAR    szWorkingDayCalendarID[16];
   BOOL    bWorkDay[366];
   } WORKDAYS;

typedef WORKDAYS FAR *PWORKDAYS;

extern PWORKDAYS pWorkDays;

extern void DoCalculateJobSchedule (HWND hwnd);

extern void DoCalculateProgramSchedule (HWND hwnd);


extern USHORT GetJobDuration (HHEAP hheap, PSZ pszJobKey);

#endif  /* JP_INCL */
