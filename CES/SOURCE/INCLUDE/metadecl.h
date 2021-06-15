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


// metadecl.h
//
// This file is included by the .txt files
// and is preprocessed only.
// note that // is the only valid comment char
// in this header file !

#define RC_INVOKED

// THIS IS SAFER
//#define INCL_WIN
//#include <os2.h>

//THIS IS QUICKER
#define INCL_PRIMITIVES
#include <pmgpi.h>
#define INCL_WINDIALOGS
#define INCL_WINFRAMEMGR
#include <pmwin.h>




#define ITIWC_COMBOBOX 0xffff0002L


// label colors and flags
//--------------------------------------------------
#define ITI_SLABELFLAGS    DT_VCENTER | DT_LEFT
#define ITI_LLABELFLAGS    DT_VCENTER | DT_LEFT
#define ITI_SLABELRTFLAGS  DT_VCENTER | DT_RIGHT
#define ITI_LLABELRTFLAGS  DT_VCENTER | DT_RIGHT
#define ITI_SLABELCTRFLAGS DT_VCENTER | DT_CENTER
#define ITI_LLABELCTRFLAGS DT_VCENTER | DT_CENTER

#define ITI_SLABELCOLOR    CLR_BLACK
#define ITI_LLABELCOLOR    CLR_WHITE

// data colors and flags
//--------------------------------------------------
#define ITI_SDATAFLAGS           DT_VCENTER | DT_LEFT
#define ITI_LDATAFLAGS           DT_VCENTER | DT_LEFT
#define ITI_SDATARTFLAGS         DT_VCENTER | DT_RIGHT
#define ITI_LDATARTFLAGS         DT_VCENTER | DT_RIGHT
#define ITI_SDATACTRFLAGS        DT_VCENTER | DT_CENTER
#define ITI_LDATACTRFLAGS        DT_VCENTER | DT_CENTER
#define ITI_SDATAMULTILINEFLAGS  ITIWND_WORDBREAK | DT_WORDBREAK | DT_LEFT | DT_TOP
#define ITI_LDATAMULTILINEFLAGS  ITIWND_WORDBREAK | DT_WORDBREAK | DT_LEFT | DT_TOP
#define ITI_SDATAMLFLAGS         ITIWND_WORDBREAK | DT_WORDBREAK | DT_LEFT | DT_TOP
#define ITI_LDATAMLFLAGS         ITIWND_WORDBREAK | DT_WORDBREAK | DT_LEFT | DT_TOP


#define ITI_SDATACOLOR     CLR_RED     /* for 'static' data (static) */
#define ITI_LDATACOLOR     CLR_RED     /* for 'static' data (list) */

#define ITI_SLINKCOLOR     CLR_BLUE    /* for links to other windows (static) */
#define ITI_LLINKCOLOR     CLR_BLUE    /* for links to other windows (list) */

#define ITI_SDLGCOLOR      CLR_BLACK   /* for dialog box static data */
#define ITI_LDLGCOLOR      CLR_BLACK   /* for dialog box list data */

// background colors
//--------------------------------------------------
#define ITI_LLABELBACKGROUND  CLR_DARKGRAY
#define ITI_LDATABACKGROUND   CLR_BACKGROUND
#define ITI_STATICBACKGROUND  CLR_BACKGROUND

// window creation options
//--------------------------------------------------
#define ITI_STATICOPTIONS     FCF_TITLEBAR | FCF_SIZEBORDER | FCF_MAXBUTTON
#define ITI_FULLSTATICOPTIONS FCF_TITLEBAR | FCF_SIZEBORDER | FCF_MAXBUTTON | FCF_MINBUTTON | FCF_SYSMENU
#define ITI_LISTOPTIONS       LWS_LABEL  | LWS_SELECT | LWS_UNDERLINELABEL
#define ITI_LISTOPTIONSV      LWS_HSPLIT | LWS_LABEL  | LWS_MULTISELECT | LWS_UNDERLINELABEL | LWS_QBUTTON
#define ITI_LISTOPTIONSH      LWS_VSPLIT | LWS_LABEL  | LWS_MULTISELECT | LWS_UNDERLINELABEL | LWS_QBUTTON
#define ITI_LISTOPTIONSHV     LWS_VSPLIT | LWS_HSPLIT | LWS_LABEL | LWS_MULTISELECT | LWS_UNDERLINELABEL | LWS_QBUTTON
#define ITI_LISTOPTIONSHQ     LWS_HSPLIT | LWS_LABEL | LWS_MULTISELECT | LWS_UNDERLINELABEL | LWS_QBUTTON

/* list options without spliters and q buttons */
#define ITI_LISTOPTIONV       LWS_VSCROLL | LWS_LABEL  | LWS_SELECT | LWS_UNDERLINELABEL
#define ITI_LISTOPTIONH       LWS_HSCROLL | LWS_LABEL  | LWS_SELECT | LWS_UNDERLINELABEL
#define ITI_LISTOPTIONHQ      LWS_HSCROLL | LWS_LABEL  | LWS_SELECT | LWS_UNDERLINELABEL | LWS_QBUTTON
#define ITI_LISTOPTIONHV      LWS_HSCROLL | LWS_VSCROLL | LWS_LABEL | LWS_SELECT | LWS_UNDERLINELABEL
#define ITI_LISTOPTIONHVQ     LWS_HSCROLL | LWS_VSCROLL | LWS_LABEL | LWS_SELECT | LWS_UNDERLINELABEL | LWS_QBUTTON
#define ITI_LISTOPTIONVQ      LWS_VSCROLL | LWS_LABEL  | LWS_SELECT | LWS_UNDERLINELABEL | LWS_QBUTTON

// common coordinate types:
//  COORD_ABS--top left origin, dlg units, normal bounding rectangle
//  COORD_RNG--top left origin, dlg units, position, size
//  COORD_REL--top left origin, dlg units, size to parent, #s=border sizes
//  COORD_PCT--top left origin, dlg units, relative to parent, #s=window size%
//-----------------------------------------------------
#define COORD_ABS  TL | DLG | X0ABS | Y0ABS | X1ABS | Y1ABS
#define COORD_RNG  TL | DLG | X0ABS | Y0ABS | X1RNG | Y1RNG
#define COORD_REL  TL | DLG | X0REL | Y0REL | X1REL | Y1REL
#define COORD_PCT  TL | DLG | X0PCT | Y0PCT | X1PCT | Y1PCT

#define COORD_CHRABS  TL | CHR | X0ABS | Y0ABS | X1ABS | Y1ABS
#define COORD_CHRRNG  TL | CHR | X0ABS | Y0ABS | X1RNG | Y1RNG
#define COORD_CHRREL  TL | CHR | X0REL | Y0REL | X1REL | Y1REL
#define COORD_CHRPCT  TL | CHR | X0PCT | Y0PCT | X1PCT | Y1PCT
#define COORD_CHRRELX TL | CHR | X0REL | Y0REL | X1REL | Y1RNG
#define COORD_CHRPCTX TL | CHR | X0PCT | Y0PCT | X1PCT | Y1RNG

#define COORD_BLCHRRNG  BL | CHR | X0ABS | Y0ABS | X1RNG | Y1RNG

#define COORD_BLKABS  TL | BLK | X0ABS | Y0ABS | X1ABS | Y1ABS
#define COORD_BLKRNG  TL | BLK | X0ABS | Y0ABS | X1RNG | Y1RNG
#define COORD_BLKREL  TL | BLK | X0REL | Y0REL | X1REL | Y1REL
#define COORD_BLKPCT  TL | BLK | X0PCT | Y0PCT | X1PCT | Y1PCT



#define UNDECLARED_BASE_DATE "1/1/1900"


