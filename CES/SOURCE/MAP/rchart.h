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


/*                                        BAMS/CES Report  
 *   rChart.h                                 
 *   Timothy Blake                        
 */    
#define  DLL_VERSION   1  
#define  TITLE        "Iti Chart Utility"  
#define  SUB_TITLE    ""  
#define  FOOTER       ""  

#define  RPT_DLL_NAME "rChart"  

#define  OPT_SW_X     'X'
#define  OPT_SW_Y     'Y'


#define  ID_QRY_MAP   314
#define  ID_CNJ_MAP   315

#define  ID_QUERY_LISTING                100 



extern USHORT  EXPORT ItiDllQueryVersion (VOID);                             
extern BOOL    EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion);     
extern BOOL    EXPORT ItiDllQueryAvailability (USHORT usTargetWindowID,      
                                               USHORT usActiveWindowID);     



/* -- Returns TRUE iff the point specified by pszLong and pszLat, which
 * -- are in units of seconds (ie. 4 degrees is 14400 seconds).
 * -- 
 * -- Note that if a point is ON a boundry it is in the map region.
 */
extern BOOL EXPORT PointInMap (PSZ pszMapKey, PSZ pszLong, PSZ pszLat);



/* -- This function is only used for command line testing during development;
 * -- Usage:  itirpt rChart /X 99999 /Y 99999  MapKey
 * -- Note that nothing is printed, all the info goes into the Iti debug file.
 *
 * -- (The ItiRpt dll command line usage expects a function named
 */
extern USHORT  EXPORT ItiRptDLLPrintReport                                   
                           (HAB hab, HMQ hmq, INT argcnt, CHAR * argvars[]); 
