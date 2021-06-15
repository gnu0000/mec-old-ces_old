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


/***************************************************************************\
* GPI.C -- GPI Helper routines
* Created by Microsoft Corporation, 1989
\***************************************************************************/


#define INCL_GPI
#include "tool.h"

/***************************************************************************\
* GetTextExtent helper function
\***************************************************************************/

ULONG EXPENTRY GetTextExtent(HPS hps, PCH lpstr, int cch) {
    POINTL rgptl[TXTBOX_COUNT];

    if (cch) {
        GpiQueryTextBox(hps, (LONG)cch, lpstr, 5L, rgptl);
        return(MAKEULONG((SHORT)(rgptl[TXTBOX_CONCAT].x - rgptl[TXTBOX_BOTTOMLEFT].x),
                     (SHORT)(rgptl[TXTBOX_TOPLEFT].y - rgptl[TXTBOX_BOTTOMLEFT].y)));
    } else
        return(0L);
    }
