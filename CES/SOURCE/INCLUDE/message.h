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
 * Message.h
 * Mark Hampton
 *
 * This header file contains the window messages that DS/Shell uses.
 */

#if !defined (MESSAGE_INCLUDED)
#define MESSAGE_INCLUDED

#define WM_SETACTIVEDOC          (WM_USER + 1)
#define WM_QUERYACTIVEDOC        (WM_USER + 2)
#define WM_INITSUBCLASS          (WM_USER + 3)
#define WM_INSERTSYSMENU         (WM_USER + 4)
#define WM_REMOVESYSMENU         (WM_USER + 5)

#endif

