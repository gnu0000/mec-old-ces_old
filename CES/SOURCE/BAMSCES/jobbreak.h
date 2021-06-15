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
 * JobBreak.h
 * Mark Hampton
 * (C) 1991 AASHTO
 *
 * This module handles the Job Breakdown windows.
 */



/*
 * Window procedure procedure for Job Breakdown List window.
 */

extern MRESULT EXPORT JobBreakdownListSProc (HWND   hwnd,
                                            USHORT usMessage,
                                            MPARAM mp1,
                                            MPARAM mp2);


/*
 * Window procedure for the JobBreakdown window.
 */

extern MRESULT EXPORT JobBreakdownSProc (HWND   hwnd,
                                        USHORT usMessage,
                                        MPARAM mp1,
                                        MPARAM mp2);





extern MRESULT EXPORT JobBreakdownDProc (HWND    hwnd,
                                            USHORT   usMessage,
                                            MPARAM   mp1,
                                            MPARAM   mp2);






extern MRESULT EXPORT JobBreakdownFundSProc (HWND     hwnd, 
                                           USHORT   usMessage,
                                           MPARAM   mp1,
                                           MPARAM   mp2);







extern MRESULT EXPORT ChJobBreakFundDProc (HWND    hwnd,
                                              USHORT   usMessage,
                                              MPARAM   mp1,
                                              MPARAM   mp2);




extern MRESULT EXPORT AddJobBreakFundDProc (HWND    hwnd,
                                               USHORT  usMessage,
                                               MPARAM  mp1,
                                               MPARAM  mp2);
