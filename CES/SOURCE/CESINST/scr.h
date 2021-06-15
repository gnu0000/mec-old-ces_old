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
 * Scr.h
 *
 * (C) 1992 Info Tech Inc.
 * This file is part the EBS module
 */
extern BYTE bcSPC[2];

extern BYTE NORMAL;
extern BYTE BRIGHT;
extern BYTE INVERSENORMAL;
extern BYTE INVERSEBRIGHT;



typedef struct
   {
   USHORT uYScreen;    // vertical size of screen
   USHORT uYListPos;   // y pos of list display area
   USHORT uYListSize;  // y size of list display area (in items !)
   USHORT uScrollPos;  // index of first displayed item
   USHORT uScrollMax;  // max index of first displayed item
   USHORT uSelected;   // index if selected item
   USHORT uItemCount;  // count of items (and section headers)
   USHORT uDisplay;
   VIOCURSORINFO viou; // cursor data for hide / restore;
   } MET;
typedef MET *PMET;


typedef struct
   {
   USHORT uXLeft;
   USHORT uYTop;
   USHORT uXSize;
   USHORT uYSize;
   PCH    pScrDat;
   } GNUWIN;
typedef GNUWIN* PGNUWIN;


USHORT InitScreenMetrics (void);
//USHORT UpdateScreenMetrics (PMET pmet, PPROP pprop);
PMET GetScreenMetrics (void);
void ShowCursor (BOOL b);
USHORT DrawBox (USHORT uXLeft, USHORT uYTop, USHORT uXSize, USHORT uYSize);
USHORT PaintText (PGNUWIN pgw, USHORT uRow, USHORT uCol, USHORT uJust, USHORT uAtt, PSZ pszText);
USHORT PaintText2(PGNUWIN pgw, USHORT uRow, USHORT uCol, USHORT uJust, USHORT uAtt, PSZ pszText, USHORT uMaxLen);
PGNUWIN CreateGnuWin (USHORT uXSize, USHORT uYSize);
void DestroyGnuWin (PGNUWIN pgw);
void ClearWin (PGNUWIN pgw);
USHORT YWinPos(PGNUWIN pgw);
USHORT XWinPos(PGNUWIN pgw);

USHORT PaintBigText (PGNUWIN pgw,    
                     USHORT  uLeft,   
                     USHORT  uTop,
                     USHORT  uRight,
                     USHORT  uBottom,
                     USHORT  uAtt,   
                     PSZ     pszText);

