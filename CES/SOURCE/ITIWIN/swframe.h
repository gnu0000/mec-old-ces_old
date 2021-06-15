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
 * swframe.h                                                            *
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

#if !defined (SWFRAME_INCLUDED)




/* resource ID for the system menu resource */
#define CHILD_SYSTEM_MENU  1

/* menu ID for the system menu */
#define IDM_SYSMENU        0x0011




#if !defined (RC_INVOKED)

/*
 * ItiWndSetMDI sets a given frame for an application document window
 * to behave as an ITI MDI window of an MDI application in a MDI
 * world in a MDI universe.
 *
 * Parameters: hwndFrame      The frame of the window to become an MDI
 *                            window.  Note that this must be an immediate
 *                            child window of the application window that
 *                            has been subclassed by the ItiWndSetMDIApp
 *                            function.
 *
 * Return value:
 * TRUE if the window was subclassed, or FALSE if not.
 */

extern BOOL EXPORT ItiWndSetMDI (HWND hwndFrame);





#endif   /* if !defined (RC_INVOKED) */



#endif   /* if !defined (SWFRAME_INCLUDED) */
