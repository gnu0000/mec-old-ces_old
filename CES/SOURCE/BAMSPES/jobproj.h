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
 * Project.h
 * Mark Hampton
 * (C) 1991 AASHTO
 *
 * This module handles the BAMSPES windows.
 */



/*
 * Window procedure for BAMSPES windows.
 */

extern MRESULT EXPORT ProjectsForJobStaticProc (HWND   hwnd,
                                                USHORT usMessage,
                                                MPARAM mp1,
                                                MPARAM mp2);



extern MRESULT EXPORT ProjectCategoryStaticProc (HWND   hwnd,
                                                 USHORT usMessage,
                                                 MPARAM mp1,
                                                 MPARAM mp2);


extern MRESULT EXPORT ProjectItemsSProc (HWND   hwnd,
                                         USHORT usMessage,
                                         MPARAM mp1,
                                         MPARAM mp2);



MRESULT EXPORT EditProjectsProc (HWND   hwnd,
                                 USHORT uMsg,
                                 MPARAM mp1,
                                 MPARAM mp2);



MRESULT EXPORT EditProjectCategoryProc (HWND   hwnd,
                                        USHORT uMsg,
                                        MPARAM mp1,
                                        MPARAM mp2);

MRESULT EXPORT ProjectItemDProc (HWND   hwnd,
                                 USHORT uMsg,
                                 MPARAM mp1,
                                 MPARAM mp2);


