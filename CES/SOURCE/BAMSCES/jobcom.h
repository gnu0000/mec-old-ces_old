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
 * JobCom.h
 * Mark Hampton
 * (C) 1991 AASHTO
 *
 * This module handles the JobComment windows.
 */



/*
 * Window procedure for JobComment windows.
 */


MRESULT EXPORT JobCommentSProc (HWND     hwnd, 
                                  USHORT   usMessage,
                                  MPARAM   mp1,
                                  MPARAM   mp2);



MRESULT EXPORT JobCommentDProc (HWND    hwnd,
                                   USHORT   usMessage,
                                   MPARAM   mp1,
                                   MPARAM   mp2);
