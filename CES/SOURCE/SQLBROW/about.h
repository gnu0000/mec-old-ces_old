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
 * About.h
 * Mark Hampton
 * Copyright (C) 1990 Info Tech, Inc.
 *
 */

/* 
 * Dialog identifier for the about box
 */



/*
 * Define identifiers for items in the dialog box
 */

#define DID_ABOUT_VERSION  100


/*
 * AboutDlgProc is the window procedure for the about box
 */

extern MRESULT EXPENTRY AboutDlgProc (HWND    hwnd,
                                      USHORT   usMessage,
                                      MPARAM   mp1,
                                      MPARAM   mp2);
