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
 * lwsplit.h                                                            *
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

#if !defined (LWSPLIT_INCLUDED)
#define LWSPLIT_INCLUDED


/* used by UpdateScrollBars */
#define LWSB_SETTHUMBPOS         1
#define LWSB_UPDATETHUMBSIZES    2
#define LWSB_UPDATESCROLLRANGES  3

 /* extra size of split bar intersection */
#define LW_EXTRACENTER           3

/* minimum sizes of window */
#define LW_XMINWINDOW            20
#define LW_YMINWINDOW            20

#define QW_POS                   0
#define QW_HOMELOCK              2

/* pointer resource ID's */
#define PTR_HORZ     2
#define PTR_VERT     3
#define PTR_HV       4

/* pointer resource handles */
extern HPOINTER hHptr;
extern HPOINTER hVptr;
extern HPOINTER hHVptr;

/* this proc uses a handle to a scroll bar and returns its pos, and max pos */
int GetScrollInfo (HWND hScrollWnd, USHORT *uScrollPos, USHORT *uScrollMax);
int GetScrollPos (HWND hwndChild, POINTS *pptsScrollPos, POINTS *pptsMax);
int UpdateScrollBars (HWND hFrameWnd, USHORT ucmd, USHORT uChildID, USHORT u1);

/*--- low level ---*/
BOOL _ScrollDoubled (HWND hwnd);
USHORT _GetScrollPos  (HWND hwnd, USHORT *puMax, USHORT *puPos);
BOOL _SetScrollThumbSize (HWND hwnd, USHORT uMax, USHORT uVis);
BOOL _SetScrollRange (HWND hwnd, USHORT uMax, USHORT uPos);
BOOL _SetScrollPos (HWND hwnd, USHORT uPos);


WNDPROC LWSplitProc  (HWND hwnd, USHORT umsg, MPARAM mp1, MPARAM mp2);

int  SetSplitLocation (HWND hChildWnd, SHORT iPos, SHORT bLock);

int  GetSplitLocation (HWND hChildWnd, SHORT *iPos, SHORT *bLock);

int InitMousePointers (void);
int DeInitMousePointers (void);





#endif

