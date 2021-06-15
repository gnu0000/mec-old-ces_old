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
 * lwutil.h                                                             *
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

#if !defined (LWUTIL_INCLUDED)
#define LWUTIL_INCLUDED


#define LWM_YRowSize             0
#define LWM_XRowSize             1
#define LWM_YWindowSize          2
#define LWM_XWindowSize          3
#define LWM_YWindowPos           4
#define LWM_XWindowPos           5
#define LWM_YScrollInc           6
#define LWM_XScrollInc           7
#define LWM_NumRows              8
#define LWM_NumCols              9
#define LWM_NumLabelCols         10
#define LWM_YLabelSize           11
#define LWM_Options              12
#define LWM_Active               13
#define LWM_FullScreenRows       14

extern USHORT LWQuery (HWND hwnd, USHORT uCmd, USHORT bChild);

extern USHORT Init (HWND hFramewnd);
extern PRECTL ElementRect (HWND hChildWnd);

extern PEDT    LWGetPEDT    (HWND hwnd, USHORT bChild, USHORT bClient);
extern PLWDAT  LWGetPLWDAT  (HWND hwnd, USHORT bChild);
extern PSZ     *LWGetPPSZ   (HWND hChildWnd);

extern HHEAP   LWGetHHEAP   (HWND hwnd, USHORT bChild);
extern HMODULE LWGetHMODULE (HWND hwnd, USHORT bChild);
extern HBUF    LWGetHBUF    (HWND hwnd, USHORT bChild);
extern USHORT *LWGetSel     (HWND hwnd, USHORT bChild);


extern LONG LWColor (HWND hChildWnd, USHORT uColorType, USHORT bChild);

USHORT LWSendActiveMsg (HWND hwnd, BOOL bChild);

#endif

