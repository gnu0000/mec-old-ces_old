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
 * Menu.h
 * Mark Hampton
 *
 */
  
/* define menu IDs */

#define BAMSCES_MAIN_MENU    ID_RESOURCE

#define CHILD_SYSTEM_MENU  (ID_RESOURCE + 1)
#define IDM_SYSMENU        0x0011


#define IDM_OPEN_JOB                    1098
#define IDM_OPEN_PROGRAM                1097
#define IDM_NEW_JOB                     1096
#define IDM_NEW_PROGRAM                 1095

// NOTE Numbers 1300 thru 1350 are used in include\itimenu.h
#define IDM_PURGE_BASE_DATES            1370
#define IDM_PRELOAD                     1371
#define IDM_UNLOAD                      1372
#define IDM_SYS_MGR                   1373

#define IDM_UPDATE_BASE_DATE            1398
#define IDM_UPDATE_SPEC_YEAR            1397
#define IDM_DECLARE_BASE_DATE           1396
#define IDM_CALCULATE_JOB_SCHEDULE      1395
#define IDM_CALCULATE_PROGRAM_SCHEDULE  1394

#define IDM_TOGGLE_SPECBOOK     1399

//#define IDM_OPEN_JOB          (IDM_FILE_MENU_LAST - 1)
//#define IDM_OPEN_PROGRAM      (IDM_FILE_MENU_LAST - 2)
//#define IDM_NEW_JOB           (IDM_FILE_MENU_LAST - 3)
//#define IDM_NEW_PROGRAM       (IDM_FILE_MENU_LAST - 4)
//#define IDM_UPDATE_BASE_DATE  (IDM_UTILITIES_MENU_LAST - 1)
//#define IDM_UPDATE_SPEC_YEAR  (IDM_UTILITIES_MENU_LAST - 2)
//#define IDM_DECLARE_BASE_DATE      (IDM_UTILITIES_MENU_LAST - 3)
//#define IDM_CALCULATE_JOB_SCHEDULE (IDM_UTILITIES_MENU_LAST - 4)


