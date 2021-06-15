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
 * Job.h
 * Mark Hampton
 * (C) 1991 AASHTO
 *
 * This module handles the Job window.
 */



/*
 * Window procedure for Job window.
 */

extern MRESULT EXPORT JobSProc (HWND   hwnd,
                                USHORT usMessage,
                                MPARAM mp1,
                                MPARAM mp2);




extern MRESULT EXPORT JobDProc (HWND   hwnd,
                                USHORT usMessage,
                                MPARAM mp1,
                                MPARAM mp2);


extern MRESULT EXPORT JobNewDProc (HWND   hwnd,
                                   USHORT usMessage,
                                   MPARAM mp1,
                                   MPARAM mp2);

extern BOOL PopulateJobZoneTable (HHEAP hheap, PSZ pszJobKey);





#define DEFAULT_ZONEKEY_STR  "1"
#define DEFAULT_ZONEKEY  1

#define  DEFAULT_PROFILEKEY_STR  "1"
#define  DEFAULT_PROFILEKEY       1



