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
 * dssprop.h
 */


#define ID_RESOURCE  1

#define DSSPROP_ICON         ID_RESOURCE
#define DSSPROP_ACCELERATORS ID_RESOURCE
#define PropCustomQuery   18666


#ifndef RC_INVOKED

#include "..\include\itiglob.h"

extern PGLOBALS pglobals;


extern MRESULT EXPENTRY PropBrowserDProc (HWND   hwnd,
                                          USHORT usMessage,
                                          MPARAM mp1,
                                          MPARAM mp2);

extern MRESULT EXPORT PropBrowserResultsSProc (HWND   hwnd, 
                                               USHORT usMessage,
                                               MPARAM mp1,
                                               MPARAM mp2);

#endif /* RC_INVOKED */
