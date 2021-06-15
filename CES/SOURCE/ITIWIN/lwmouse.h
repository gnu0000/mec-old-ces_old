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


/************************************************************************
 *                                                                      *
 * lwmouse.h                                                            *
 *                                                                      *
 * Copyright (C) 1990-1992 Info Tech, Inc.                              *
 *                                                                      *
 * This file is part of the Window system of DS/SHELL. This is a        *
 * proprietary product of Info Tech. and no part of which may be        *
 * reproduced or transmitted in any form or by any means, including     *
 * photocopying and recording or in connection with any information     *
 * storage or retrieval system, without permission in writing           *
 * from Info Tech, Inc.                                                 *
 *                                                                      *
 *                                                                      *
 ************************************************************************/


/* returns either
 *
 * uRow       - if a row is selected
 * LWMP_BLANK - if mouse selects blank area
 * LWMP_BELOW - if mouse selects below window
 * LWMP_ABOVE - if mouse selects above window
 */
extern USHORT LWRowFromMouse (HWND hwndClient,
                              MPARAM mp1, 
                              USHORT *uDistance);


/* returns either
 *
 * uCol       - if a row is selected
 * LWMP_BLANK - if mouse selects blank area
 * LWMP_BELOW - if mouse selects below window
 * LWMP_ABOVE - if mouse selects above window
 */
extern USHORT LWColFromMouse (HWND hwndClient, 
                              MPARAM mp1, 
                              BOOL bClient);


/*
 * This fn is called when the user clicks in
 * a client window, it generates a msg
 */
extern void LWMouseClick (HWND hwndClient,
                          MPARAM mp1,
                          MPARAM mp2,
                          BOOL bClient,
                          BOOL bDblClk,
                          BOOL bLeftMouse);



/*
 * This fn contains a message loop to process messages while
 * the mouse button is depressed in a client window
 */
extern USHORT LWDoMouseSelect (HWND hwndClient, 
                               MPARAM mp1, 
                               USHORT umsg);
