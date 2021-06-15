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
 * Help.h
 * Mark Hampton
 * 
 * This file defines the entry points into the help system, as well as
 * unique identifiers for all types of windows.
 */

/* 
 * This section defines unique window identifiers for all types of windows.
 * Each type of window must have a unique identifier so that help can be
 * displayed for that window
 */ 

/* defines for Message Boxes */
#define MB_ERROR_WINDOW       1        


/* define the ID for the help subtable for menus */
#define HST_MENUS             100

/* define the ID for the help subtable for menus */
#define HST_HELP              101

/* define the ID for the help table */
#define HT_HELPTABLE          1


#define HELP_DEFAULT_HELP   1





#ifdef RC_INVOKED
#define HP_MENU_BASE             1000

#define HP_FILE_MENU                    1000
#define HP_OPEN_CATALOG_MENU            1001
#define HP_OPEN_APPLICATION_CATALOG     1002
#define HP_OPEN_AREA_CATALOG            1003
#define HP_OPEN_CODE_TABLE_CATALOG      1004
#define HP_OPEN_COST_SHEET_CATALOG      1005
#define HP_OPEN_EQUIPMENT_CATALOG       1006
#define HP_OPEN_LABOR_CATALOG           1007
#define HP_OPEN_MAP_CATALOG             1008
#define HP_OPEN_MATERIAL_CATALOG        1009
#define HP_OPEN_STANDARD_ITEM_CATALOG   1010
#define HP_OPEN_USER_CATALOG            1011
#define HP_OPEN_VENDOR_CATALOG          1012
#define HP_OPEN_ZONE_TYPE_CATALOG       1013
#define HP_CLOSE                        1014
#define HP_PRINT_MENU                   1015
#define HP_PRINT_CODE_TABLE_LISTING     1016
#define HP_PRINT_USER_LISTING           1017
#define HP_PRINTER_SETUP                1018
#define HP_IMPORT                       1019
#define HP_EXPORT                       1020
#define HP_RUN_MENU                     1021
#define HP_RUN_ITEM_BROWSER             1022
#define HP_RUN_PROPOSAL_BROWSER         1023
#define HP_RUN_FACILITY_BROWSER         1024
#define HP_RUN_SQL_BROWSER              1025
#define HP_RUN_OTHER                    1026
#define HP_PROCESS_STATUS               1027
#define HP_EXIT                         1028  

#define HP_EDIT_MENU                    1029  
#define HP_ADD                          1030
#define HP_CHANGE                       1031
#define HP_DELETE                       1032
#define HP_CUT                          1033
#define HP_COPY                         1034
#define HP_PASTE                        1035

#define HP_VIEW_MENU                    1036
#define HP_PARENT                       1037

#define HP_UTILITIES_MENU               1038
#define HP_FIND                         1039
#define HP_SORT                         1040
#define HP_SET_PREFERENCES              1041
#define HP_CHANGE_PASSWORD              1042
#define HP_SHOW_USERS                   1043
#define HP_SEND_MESSAGE                 1044
#define HP_INSTALL                      1045
#define HP_DENY_LOGON                   1046
#define HP_GENERATE_SUPPORT_REQUEST     1047
                                        
#define HP_WINDOW_MENU                  1048
#define HP_ARRANGE_CASCADED             1049
#define HP_ARRANGE_TILED                1050

#define HP_HELP_MENU                    1051
#define HP_INDEX                        1052
#define HP_KEYBOARD                     1053
#define HP_MOUSE                        1054
#define HP_COMMANDS                     1055
#define HP_PROCEDURES                   1056
#define HP_GLOSSARY                     1057
#define HP_USING_HELP                   1058
#define HP_ABOUT                        1059

#define HP_DEBUG_MENU                   1060
#define HP_DEBUG_BREAK                  1061
#define HP_DIE_SCUM                     1062

#endif


/* define help menu panels */
#define HP_HELP_MENU_BASE              100
#define HP_KEYBOARD_HELP               100
#define HP_MOUSE_HELP                  101
#define HP_COMMANDS_HELP               102
#define HP_PROCEDURES_HELP             103
#define HP_GLOSSARY_HELP               104
#define HP_USING_HELP_HELP             105
#define HP_WINDOWS_HELP                106

