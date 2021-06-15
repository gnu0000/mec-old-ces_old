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
 * winmain.h                                                            *
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

#define SB_LWTRACK              (WNDMSG_START + 11)

/*
 *  --- LWINIT STRUCTURE ---
 *
 * This structure is used by the LWInit proc
 * to initialize the output metrics. This procedure
 * must be called before any information will be
 * displayed by the window:
 *
 *
 * > asterisks after the name indicate optional values.
 * > all metrics are specified in pixels.
 * > the coordinate system is the standard 0S/2 back-assward system
 *
 * uDataRows    The number of rows in the client window
 * uDataCols    The number of cols in the client window
 * uLabelCols   * The number of cols in the label window.
 *                This may be 0, which will default to uDataCols.
 *                This metric is only meaningful if the window
 *                contains the LWS_LABEL style (see below)
 * uYDataSize * The vertical size of a row. This may be 0,
 *                which will cause this value to be calculated by
 *                determining the bounding rectangle of all cols
 *                in a row, using the ymax, and setting ymin to zero
 * uYLabelSize  * The vertical size of the label area. This may be 0
 *                which will cause the label size to be calculated 
 *                in the method similar to uYDataSize
 * uXScrollInc  * The increment to scroll for the horizontal scroll
 *                bar. This may be 0, which defaults to 40. This only
 *                has meaning if the window contains the LWS_HSCROLL
 *                style.
 * ufOptions      This contains the style bits that the window is to
 *                have. It may be any combination of the below styles:
 *    LWS_HSCROLL ... window has a horizontal scrollbar
 *    LWS_VSPLIT  ... window has a vertical scroll bar
 *    LWS_VSCROLL ... window has a vertical split bar
 *    LWS_HSPLIT .... window has a horizontal split bar
 *    LWS_LABEL ..... window has a label area
 *    LWS_SELECT .... client area has single selection   
 *    LWS_MULTISELECT client area has multi selection
 *    LWS_SPLITATTOP  split bars start at top/left
 *
 */
typedef struct
   { USHORT uDataRows;
     USHORT uDataCols;
     USHORT uLabelCols;    /* may be zero */
     USHORT uDataYSize;    /* may be zero */
     USHORT uLabelYSize;   /* may be zero */
     USHORT uXScrollInc;   /* may be zero */
     USHORT uOptions;
   } LWINIT;
typedef LWINIT FAR *PLWINIT;


/*
 *  --- LWINIT PROC ---
 *
 * The LWInit procedure must be called after the window is created
 * The parameters are as follows:
 *
 * hwnd           The handle of the listwindow frame.
 * plwInit        The PLWINIT structure described above.
 * aedtData[]   Array of EDT structures described above.
 *                This array describes the Data columns.
 * aedtLabel[]  * Array of EDT structures described above.
 *                This array describes the Label columns.
 *                This pointer may be NULL, which will default to
 *                the same as aedtData[]. This value is only used
 *                if the LWS_LABEL style is used.
 * pszLabelTxt[]  Array of pointers to Label strings, identified
 *                positionally.  This parameter is only used if the
 *                LWS_LABEL parameter is specified.
 * GetText        This parameter identifies the function which is to
 *                specify text for a given row,col in the client area.
 *                the function is to be of the form:
 *                    int GetTextStr  (HWND hChildWnd, USHORT uRow,
 *                                     USHORT uCol, PSZ *ppszText)
 *                This parameter may be NULL, but then the client will
 *                only display a test pattern.
 *                This function is not called if the user is going to 
 *                do the drawing (I.E. the LWInitDrawProc fn is used)
 *
 */

extern int EXPORT LWInit (HWND  hwnd,
                  HMODULE    hmod,
                  PLWINIT    plwInit,
                  EDT        aedtData[],
                  EDT        aedtLabel[],
                  PSZ        pszLabelTxt[],
                  PFNTXT     GetText);
