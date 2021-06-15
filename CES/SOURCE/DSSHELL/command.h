/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1992 by Info Tech, Inc.  All Rights Reserved.         |
|                                                                           |
|       This program module is part of DS/Shell (PC), Info Tech's           |
|       database interfaces for OS/2, a proprietary product of              |
|       Info Tech, Inc., no part of which may be reproduced or              |
|       transmitted in any form or by any means, electronic,                |
|       mechanical, or otherwise, including photocopying and recording      |
|       or in connection with any information storage or retrieval          |
|       system, without permission in writing from Info Tech, Inc.          |
|                                                                           |
+--------------------------------------------------------------------------*/


/*
 * Command.h
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 */


/*
 * returns TRUE if message processed, FALSE if not.
 */

extern BOOL ProcessCommand (HWND hwnd, USHORT usCommand, MPARAM mp2);




/*
 * returns TRUE if message processed, FALSE if not.
 */

extern BOOL GreyMenuItems (HWND hwndMenu, USHORT usMenuID);
