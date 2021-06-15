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
 * ProgJob.h
 * Mark Hampton
 * (C) 1991 AASHTO
 *
 * This module handles the Program Jobs window.
 */




/*
 * Window procedure for Program Job window.
 */

extern MRESULT EXPORT ProgramJobSProc (HWND   hwnd,
                                      USHORT usMessage,
                                      MPARAM mp1,
                                      MPARAM mp2);




extern MRESULT EXPORT ChProgJobDProc (HWND    hwnd,
                                        USHORT  uMsg,
                                        MPARAM  mp1,
                                        MPARAM  mp2);





extern MRESULT EXPORT AddProgJobDProc (HWND    hwnd,
                                         USHORT  uMsg,
                                         MPARAM  mp1,
                                         MPARAM  mp2);





extern MRESULT EXPORT ProgramJobSchedDProc (HWND    hwnd,
                                            USHORT  uMsg,
                                            MPARAM  mp1,
                                            MPARAM  mp2);





extern MRESULT EXPORT ProgramJobPredAddDProc (HWND    hwnd,
                                              USHORT  uMsg,
                                              MPARAM  mp1,
                                              MPARAM  mp2);





extern MRESULT EXPORT ProgramJobPredChangeDProc (HWND    hwnd,
                                                 USHORT  uMsg,
                                                 MPARAM  mp1,
                                                 MPARAM  mp2);





extern MRESULT EXPORT ProgramJobSuccAddDProc (HWND    hwnd,
                                              USHORT  uMsg,
                                              MPARAM  mp1,
                                              MPARAM  mp2);





extern MRESULT EXPORT ProgramJobSuccChangeDProc (HWND    hwnd,
                                                 USHORT  uMsg,
                                                 MPARAM  mp1,
                                                 MPARAM  mp2);


