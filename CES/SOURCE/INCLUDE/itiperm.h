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



extern BOOL EXPORT ItiPermLogin (HWND hwndLoading);
   
extern void EXPORT ItiPermChangePassword (HWND hwnd);

extern BOOL EXPORT ItiPermQueryEdit (HWND hwnd);

extern BOOL EXPORT ItiPermQueryDelete (PSZ pszKey, BOOL bJob);

extern BOOL EXPORT ItiPermIsSuperUser (void);

/*
 * call this whenever a job or program is closed.
 * returns TRUE if job was found in internal list, FALSE if not.
 */

extern BOOL EXPORT ItiPermJobClosed (PSZ pszJobKey);



/*
 * call this whenever a job or program is opened.  bJob == TRUE for Job,
 * FALSE for program.
 * returns TRUE if the current user has permission to open the job, false
 * if not.
 */

extern BOOL EXPORT ItiPermJobOpened (PSZ pszJobKey, BOOL bJob);

/* returns TRUE if the user may view the user catalog */
extern BOOL EXPORT ItiPermQueryUserCatAvail (void);



