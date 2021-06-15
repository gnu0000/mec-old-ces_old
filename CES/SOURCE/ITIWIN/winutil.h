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
 * winutil.h                                                            *
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
/*
 * winutil.h
 * Copyright (C) 1990 Info Tech, Inc.
 *
 */

#if !defined (WINUTIL_INCLUDED)
#define WINUTIL_INCLUDED

extern char ITIWND_STATICCLASS[];
extern char ITIWND_LISTCLASS[];




extern void WndOs2WinRcl (HWND hwnd, PRECTL prcl);

extern BOOL EXPORT ItiWndIsSingleClick (MPARAM mp);

extern BOOL EXPORT ItiWndIsLeftClick  (MPARAM mp);


extern BOOL WndGetIds (HWND hwndClient, USHORT *puClientId,
                       USHORT *puVSBId,  USHORT *puHSBId);

extern USHORT WndAssignRcl (RECTL *rcl,   LONG xLeft, LONG yBottom,
                            LONG  xRight, LONG yTop);


extern USHORT WndAssignPts (POINTS *ppts1, POINTS *ppts2,
                            short xp1, short yp1,
                            short xp2, short yp2);

extern BOOL WndSetPtr (SHORT sPtr);

extern SHORT ItiWndDrawText (HPS    hps,
                             SHORT  sLen,
                             PSZ    pszTxt,
                             PRECTL prcl,
                             LONG   clrFore,
                             LONG   clrBkg,
                             ULONG  ulFlags);


/*
 * This function should be called with the static
 * window handle.
 *
 * Returns the count of windows deleted.
 */
extern USHORT ItiWndAxeKids (HWND hwnd);


extern USHORT ItiWndCopyIt (HWND hwnd);

#endif

