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
 * Command.h
 * Mark Hampton
 */


/*
 * returns TRUE if message processed, FALSE if not.
 */

extern BOOL ProcessCommand (HWND hwnd, USHORT usCommand, MPARAM mp2);




/*
 * returns TRUE if message processed, FALSE if not.
 */

extern BOOL GreyMenuItems (HWND hwndMenu, USHORT usMenuID);
