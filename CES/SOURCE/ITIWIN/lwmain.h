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
 * lwmain.h                                                             *
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

#if !defined (LWMAIN_INCLUDED)
#define LWMAIN_INCLUDED


/*
   WM_SPLITMOVE
     this message is sent from the splitter bar proc to the
     frame proc when a splitter bar is moved.  
   WM_CHANGENUMELEMENTS
     this message is sent to the frame proc when the number of elements
     has been changed.  mp2 = new number of elements
   WM_UPDATEIFVISIBLE
     this message is sent from the frame window to the client or label
     window when an item must be updated from other than a paint condition.
     mp2 = element number to be updated.
   WM_SELECTELEMENT
     this message is sent from a client window to the frame window when
     a selection is changed
     mp1, mp2
*/

/* winproc user messages */
#define LWINTMSG_START            WNDMSG_START   + 50
#define WM_SPLITMOVE             (LWINTMSG_START + 0)
#define WM_RESET                 (LWINTMSG_START + 1)

#define LWID_FRAME                0x0000

/* pointer index into reserved memory for frame window pmet */
#define QWP_LWDAT                  0


/*
 *  This structure is internally kept by the frame window
 *It is initially set up by the Init proc
 */
typedef struct { USHORT Init;
                 USHORT uDataRows;
                 USHORT uDataCols;
                 USHORT uLabelCols;
                 USHORT uActive;
                 USHORT uStartSel;
                 USHORT uScrollType;
                 LONG   lDataColor;
                 LONG   lLabelColor;
                 LONG   lSelectColor;
                 LONG   lActiveColor;
                 LONG   lSplitBColor;
                 LONG   lSplitMin;
                 LONG   lSplitMax;
                 USHORT uXScrollInc;
                 USHORT uOptions;
                 PEDT   pedtData;
                 PEDT   pedtLabel;
                 PSZ    *ppszLabels;
                 PVOID  hbuf;
                 HHEAP  hheap;
                 USHORT uButtonId;
                 PFNTXT      pfnTxt;
                 PFNINITDRAW pfnInitDraw;
                 PFNDRAW     pfnDraw;
                 USHORT *puSel;
                 USHORT uSelSize;
                 PVOID  pExtra;
 
                 /*--- these are calculated from above for speed ---*/
                 USHORT uYLabel;
                 RECTL  rclData;

                 /*--- these are only used if a window has more than  ---*/
                 /*--- 32k items and needs to be scrolled by 2. These ---*/
                 /*--- vars provide the lsb of the scroll pos         ---*/
                 BOOL   bV0Odd;
                 BOOL   bV1Odd;
               } LWDAT;
typedef LWDAT FAR *PLWDAT;





extern USHORT LWID_LABEL[2];
extern USHORT LWID_HSB[2];
extern USHORT LWID_VSB[2];
extern USHORT LWID_CLIENT[2][2];
extern USHORT LWID_VSPLIT;
extern USHORT LWID_HSPLIT;
extern USHORT LWID_QBUTTON;

#define LW_CLIENTAREA             0x0100
#define LW_INITIALHEAPSIZE        4096

/* these globals are copied from the globals module */
extern SHORT xVSB;
extern SHORT yHSB;
extern SHORT yHSPLIT;
extern SHORT xVSPLIT;

extern char szLWLabelClass[];
extern char szLWClientClass[];
extern char szLWSplitClass[];



extern WNDPROC LWFrameProc (HWND hwnd, USHORT umsg, MPARAM mp1, MPARAM mp2);


int DoVScroll (HWND hFrameWnd, MPARAM mp1, MPARAM mp2);
int DoHScroll (HWND hFrameWnd, MPARAM mp1, MPARAM mp2);

int SizeAllChildWindows (HWND hFrameWnd, MPARAM mp);



/*
 * The LWRegister procedure must be called before any list windows
 * are created.
 */
extern USHORT EXPORT LWRegister (HAB hab);



#endif

