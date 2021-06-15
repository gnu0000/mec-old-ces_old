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
 * lwclient.h                                                           *
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

#if !defined (LWCLIENT_INCLUDED)
#define LWCLIENT_INCLUDED



WNDPROC LWLabelProc  (HWND hwnd, USHORT umsg, MPARAM mp1, MPARAM mp2);

WNDPROC LWClientProc (HWND hwnd, USHORT umsg, MPARAM mp1, MPARAM mp2);

int EXPORT GetTextElement  (HWND   hwndChild,
                            USHORT uRow,
                            USHORT uCol,
                            PSZ    *ppszText);

int GetUpdateRange (HWND hwnd, PRECTL prclUpdate, USHORT uYScrPos,
                    USHORT *puFirst, USHORT *puLast,  USHORT *puInc);

int AddBaseYOffset (HWND   hChildWnd,
                    USHORT uYScrPos,
                    USHORT uFirstRow,
                    RECTL  *prclPos);

#endif

